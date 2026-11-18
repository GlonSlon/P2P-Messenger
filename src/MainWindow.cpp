#include "MainWindow.h"
#include "ChatWidget.h"
#include "DhtChatManager.h"
#include "FileCompressor.h"
#include "Theme.h"

#include <QSplitter>
#include <QStatusBar>
#include <QDir>
#include <QFile>

MainWindow::MainWindow(const QString& username, QWidget* parent)
    : QMainWindow(parent), username_(username) {
    setWindowTitle(QString("P2P Messenger — %1").arg(username_));
    resize(1000, 680);
    setStyleSheet(QString("background-color: %1;").arg(Theme::Background.name()));

    contactList_ = new ContactListWidget;
    chatWidget_  = new ChatWidget;

    auto* splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(contactList_);
    splitter->addWidget(chatWidget_);
    splitter->setSizes({280, 720});
    setCentralWidget(splitter);

    statusBar()->setStyleSheet(QString("QStatusBar { background-color: %1; color: %2; }")
        .arg(Theme::Surface.name(), Theme::Text.name()));
    statusBar()->showMessage(QString("Вы: %1").arg(username_));

    dht_ = new DhtChatManager(this);
    syncTimer_ = new QTimer(this);

    connect(contactList_, &ContactListWidget::contactSelected, this, &MainWindow::onContactSelected);
    connect(contactList_, &ContactListWidget::searchRequested, this, &MainWindow::onSearchRequested);
    connect(dht_, &DhtChatManager::messageReceived, this, &MainWindow::onMessageReceived);
    connect(dht_, &DhtChatManager::statusChanged, this, [this](const QString& status) {
        chatWidget_->appendSystem(status);
    });
    connect(chatWidget_, &ChatWidget::messageSendRequested, this, &MainWindow::onMessageSendRequested);
    connect(chatWidget_, &ChatWidget::fileAttachRequested, this, &MainWindow::onFileAttachRequested);
    connect(syncTimer_, &QTimer::timeout, this, &MainWindow::syncMessages);

    dht_->start(username_);
    syncTimer_->start(3000);
    chatWidget_->appendSystem("Введите username собеседника в поле поиска.");
}

MainWindow::~MainWindow() {
    syncTimer_->stop();
    if (dht_) dht_->stop();
}

void MainWindow::onContactSelected(const Contact& contact) {
    currentContact_ = contact;
    dht_->listenContact(contact.username);
    chatWidget_->setConnectionStatus(QString("Чат с %1").arg(contact.username));
    chatWidget_->appendSystem(QString("Можно отправлять сообщения пользователю %1.").arg(contact.username));
}

void MainWindow::onSearchRequested(const QString& username) {
    if (contactList_->hasContact(username)) {
        chatWidget_->appendSystem(QString("Пользователь %1 уже в списке.").arg(username));
        return;
    }

    Contact contact;
    contact.username = username;
    contactList_->addContact(contact);
    dht_->listenContact(username);
    chatWidget_->appendSystem(QString("Пользователь %1 добавлен.").arg(username));
}

void MainWindow::syncMessages() {
    if (currentContact_.username.isEmpty()) return;
    dht_->getValues(currentContact_.username);
}

void MainWindow::onMessageReceived(const QString& from, const QByteArray& packet) {
    if (packet.isEmpty()) return;

    quint8 type = static_cast<quint8>(packet[0]);
    QByteArray body = packet.mid(1);

    if (type == 0) {
        chatWidget_->appendMessage(from, QString::fromUtf8(body), false);
    } else if (type == 1) {
        if (body.size() < 4) return;

        quint32 nameLen = (quint32(static_cast<uchar>(body[0])) << 24) |
                          (quint32(static_cast<uchar>(body[1])) << 16) |
                          (quint32(static_cast<uchar>(body[2])) << 8)  |
                           quint32(static_cast<uchar>(body[3]));
        if (body.size() < 4 + static_cast<int>(nameLen)) return;

        QString fileName = QString::fromUtf8(body.mid(4, static_cast<int>(nameLen)));
        QByteArray compressed = body.mid(4 + static_cast<int>(nameLen));
        handleIncomingFile(from, fileName, compressed);
    }
}

void MainWindow::handleIncomingFile(const QString& sender, const QString& fileName, const QByteArray& compressedData) {
    QByteArray data = FileCompressor::decompress(compressedData);
    if (data.isEmpty()) {
        chatWidget_->appendSystem("Не удалось распаковать полученный файл.");
        return;
    }

    QString downloadsDir = QDir::homePath() + "/Downloads";
    QDir().mkpath(downloadsDir);
    QString savePath = downloadsDir + "/" + fileName;

    QFile f(savePath);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(data);
        f.close();
        chatWidget_->appendFile(sender, fileName, data);
        chatWidget_->appendSystem(QString("Файл сохранён: %1").arg(savePath));
    } else {
        chatWidget_->appendSystem("Не удалось сохранить файл.");
    }
}

void MainWindow::onMessageSendRequested(const QString& text) {
    if (currentContact_.username.isEmpty()) {
        chatWidget_->appendSystem("Сначала выберите собеседника.");
        return;
    }

    QByteArray packet;
    packet.append(char(0));
    packet.append(text.toUtf8());

    dht_->sendMessage(currentContact_.username, packet);
    chatWidget_->appendMessage("Вы", text, true);
}

void MainWindow::onFileAttachRequested(const QString& fileName, const QByteArray& data) {
    if (currentContact_.username.isEmpty()) {
        chatWidget_->appendSystem("Сначала выберите собеседника.");
        return;
    }

    QByteArray nameUtf8 = fileName.toUtf8();
    QByteArray body;
    quint32 nameLen = static_cast<quint32>(nameUtf8.size());

    body.append(char((nameLen >> 24) & 0xFF))
        .append(char((nameLen >> 16) & 0xFF))
        .append(char((nameLen >> 8) & 0xFF))
        .append(char(nameLen & 0xFF))
        .append(nameUtf8)
        .append(data);

    QByteArray packet;
    packet.append(char(1));
    packet.append(body);

    dht_->sendMessage(currentContact_.username, packet);
    chatWidget_->appendFile("Вы", fileName, data);
}