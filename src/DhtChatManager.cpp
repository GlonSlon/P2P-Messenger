#include "DhtChatManager.h"
#include <QDebug>
#include <QMetaObject>
#include <opendht/crypto.h>
#include <opendht/value.h>
#include <chrono>
#include <vector>
#include "CryptoUtils.h"


DhtChatManager::DhtChatManager(QObject* parent) : QObject(parent) {}
DhtChatManager::~DhtChatManager() { stop(); }

void DhtChatManager::start(const QString& identityName) {
    if (running_.exchange(true)) return;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        myName_ = identityName;
    }

    workerThread_ = std::thread([this]() {
        auto runner = std::make_shared<dht::DhtRunner>();
        {
            std::lock_guard<std::mutex> lock(mutex_);
            dht_ = runner;
        }

        runner->run(0, dht::crypto::generateIdentity(), true);
        runner->bootstrap("bootstrap.jami.net", "4222");

        std::this_thread::sleep_for(std::chrono::seconds(5));
        dhtReady_ = true;

        // --- ДИАГНОСТИКА СЕТИ ---
        auto info = runner->getNodeInfo();
        qDebug() << "==========================================";
        qDebug() << "DHT Node ID:" << QString::fromStdString(info.node_id.toString());
        qDebug() << "IPv4 good nodes:" << info.ipv4.good_nodes
                 << "dubious:" << info.ipv4.dubious_nodes;
        qDebug() << "IPv6 good nodes:" << info.ipv6.good_nodes
                 << "dubious:" << info.ipv6.dubious_nodes;
        qDebug() << "==========================================";
        // ------------------------

        QMetaObject::invokeMethod(this, [this]() {
            emit statusChanged("DHT запущен и готов к работе");
        }, Qt::QueuedConnection);

        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        runner->join();

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (dht_ == runner) dht_.reset();
        }
    });
}

void DhtChatManager::stop() {
    if (!running_.exchange(false)) return;
    if (workerThread_.joinable()) workerThread_.join();

    std::lock_guard<std::mutex> lock(mutex_);
    dht_.reset();
    listened_.clear();
    seenIds_.clear();
    dhtReady_ = false;
}

static QString makeRoomName(const QString& a, const QString& b) {
    if (a == b) return a;
    return (a < b) ? (a + ":" + b) : (b + ":" + a);
}

void DhtChatManager::listenContact(const QString& username) {
    if (username.isEmpty()) return;

    std::shared_ptr<dht::DhtRunner> runner;
    QString me;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (listened_.count(username)) return;
        listened_.insert(username);
        runner = dht_;
        me = myName_;
    }

    if (!runner) return;

    QString roomName = makeRoomName(me, username);
    auto hash = dht::InfoHash::get(roomName.toStdString());

    runner->listen(hash, [this](const std::vector<std::shared_ptr<dht::Value>>& values) {
        processValues(values);
        return true;
    });
}

void DhtChatManager::getValues(const QString& username) {
    if (username.isEmpty()) return;
    
    std::shared_ptr<dht::DhtRunner> runner;
    QString me;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        runner = dht_;
        me = myName_;
    }

    if (!runner || !dhtReady_) return;

    QString roomName = makeRoomName(me, username);
    auto hash = dht::InfoHash::get(roomName.toStdString());
    
    dht::GetCallback cb = [this](const std::vector<std::shared_ptr<dht::Value>>& values) {
        processValues(values);
        return true;
    };
    
    runner->get(hash, cb);
}

void DhtChatManager::processValues(const std::vector<std::shared_ptr<dht::Value>>& values) {
    QString me;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        me = myName_;
    }

    int64_t nowSec = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

    for (const auto& v : values) {
        if (!v || v->data.empty()) continue;

        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (seenIds_.count(v->id)) continue;
            seenIds_.insert(v->id);
        }

        const auto& d = v->data;
        if (d.size() < 4 + 8) continue;

        quint32 senderLen = (quint32(d[0]) << 24) | (quint32(d[1]) << 16) |
                            (quint32(d[2]) << 8)  |  quint32(d[3]);
        if (d.size() < 4 + senderLen + 8) continue;

        QString sender = QString::fromUtf8(
            reinterpret_cast<const char*>(d.data() + 4),
            static_cast<int>(senderLen));
        if (sender == me) continue;

        int64_t msgTime = 0;
        for (int i = 0; i < 8; ++i) {
            msgTime = (msgTime << 8) | static_cast<unsigned char>(d[4 + senderLen + i]);
        }

        if (nowSec - msgTime > MAX_MESSAGE_AGE_SEC) {
            continue;
        }

        // Извлекаем зашифрованные данные
        QByteArray encryptedPayload(
            reinterpret_cast<const char*>(d.data() + 4 + senderLen + 8),
            static_cast<int>(d.size() - 4 - senderLen - 8));

        // РАСШИФРОВКА
        QByteArray payload = CryptoUtils::decrypt(encryptedPayload);
        
        // Если расшифровать не удалось (чужой ключ или мусор) - игнорируем
        if (payload.isEmpty()) {
            continue;
        }

        QMetaObject::invokeMethod(this, [this, sender, payload]() {
            emit messageReceived(sender, payload);
        }, Qt::QueuedConnection);
    }
}

void DhtChatManager::sendMessage(const QString& username, const QByteArray& payload) {
    std::shared_ptr<dht::DhtRunner> runner;
    QString me;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        runner = dht_;
        me = myName_;
    }

    if (!runner || !dhtReady_) return;

    QByteArray senderUtf8 = me.toUtf8();
    quint32 senderLen = static_cast<quint32>(senderUtf8.size());
    int64_t nowSec = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();

    // ШИФРОВАНИЕ
    QByteArray encryptedPayload = CryptoUtils::encrypt(payload);
    if (encryptedPayload.isEmpty()) {
        qWarning() << "Encryption failed";
        return;
    }

    QByteArray value;
    value.append(char((senderLen >> 24) & 0xFF))
         .append(char((senderLen >> 16) & 0xFF))
         .append(char((senderLen >> 8) & 0xFF))
         .append(char(senderLen & 0xFF))
         .append(senderUtf8);

    for (int i = 7; i >= 0; --i) {
        value.append(char((nowSec >> (i * 8)) & 0xFF));
    }

    // Добавляем зашифрованные данные вместо открытых
    value.append(encryptedPayload);

    std::vector<uint8_t> data(value.begin(), value.end());
    
    QString roomName = makeRoomName(me, username);
    auto hash = dht::InfoHash::get(roomName.toStdString());

    // Устанавливаем время жизни сообщения: 1 час это 3600 секунд
    auto expiration = dht::from_time_t(std::time(nullptr) + 4731);
    
    runner->put(hash, std::make_shared<dht::Value>(data), [](bool ok) {
        qDebug() << "put" << ok;
    }, expiration, false);  // permanent = false
}