#pragma once
#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QLabel>
#include <QScrollBar>

class ChatWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChatWidget(QWidget* parent = nullptr);
    void appendMessage(const QString& sender, const QString& text, bool own);
    void appendFile(const QString& sender, const QString& fileName, const QByteArray& data);
    void appendSystem(const QString& text);
    void setConnectionStatus(const QString& status);

signals:
    void messageSendRequested(const QString& text);
    void fileAttachRequested(const QString& fileName, const QByteArray& data);

private slots:
    void sendMessage();
    void attachFile();

private:
    QListWidget* messagesList_;
    QLineEdit*   inputEdit_;
    QToolButton* attachBtn_;
    QToolButton* sendBtn_;
    QLabel*      statusLabel_;
    bool         autoScroll_ = true;
    void scrollIfNeeded();
};

