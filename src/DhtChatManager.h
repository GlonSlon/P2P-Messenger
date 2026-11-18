#pragma once
#include <QObject>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <set>
#include <chrono>
#include <opendht/dhtrunner.h>
#include <opendht/infohash.h>

class DhtChatManager : public QObject {
    Q_OBJECT
public:
    explicit DhtChatManager(QObject* parent = nullptr);
    ~DhtChatManager() override;

    void start(const QString& identityName);
    void stop();
    void listenContact(const QString& username);
    void getValues(const QString& username);
    void sendMessage(const QString& username, const QByteArray& payload);

signals:
    void messageReceived(const QString& from, const QByteArray& payload);
    void statusChanged(const QString& status);

private:
    void processValues(const std::vector<std::shared_ptr<dht::Value>>& values);
    static constexpr int64_t MAX_MESSAGE_AGE_SEC = 120;

    std::shared_ptr<dht::DhtRunner> dht_;
    std::thread workerThread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> dhtReady_{false};
    std::mutex mutex_;
    QString myName_;
    std::set<QString> listened_;
    std::set<dht::Value::Id> seenIds_;
};