#pragma once
#include <QMainWindow>
#include <QTimer>
#include "ContactListWidget.h"

class ChatWidget;
class DhtChatManager;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(const QString& username, QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onContactSelected(const Contact& contact);
    void onSearchRequested(const QString& username);
    void onMessageReceived(const QString& from, const QByteArray& packet);
    void onMessageSendRequested(const QString& text);
    void onFileAttachRequested(const QString& fileName, const QByteArray& data);
    void syncMessages();

private:
    void handleIncomingFile(const QString& sender, const QString& fileName, const QByteArray& compressedData);

    QString          username_;
    Contact          currentContact_;

    ContactListWidget* contactList_ = nullptr;
    ChatWidget*        chatWidget_  = nullptr;
    DhtChatManager*    dht_         = nullptr;
    QTimer*            syncTimer_   = nullptr;
};