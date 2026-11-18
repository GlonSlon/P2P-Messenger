#include "ChatWidget.h"
#include "Theme.h"
#include "FileCompressor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

ChatWidget::ChatWidget(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    messagesList_ = new QListWidget;
    messagesList_->setStyleSheet(QString(
        "QListWidget { background-color: %1; border: none; outline: none; padding: 12px; }"
        "QListWidget::item { padding: 10px 14px; margin: 2px 0px; border-radius: 10px; color: %2; }"
        "QListWidget::item:selected { background-color: transparent; }")
        .arg(Theme::SurfaceDark.name(), Theme::Text.name()));
    messagesList_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    messagesList_->setWordWrap(true);
    messagesList_->setFocusPolicy(Qt::NoFocus);

    connect(messagesList_->verticalScrollBar(), &QScrollBar::valueChanged, [this](int value) {
        autoScroll_ = (value == messagesList_->verticalScrollBar()->maximum());
    });

    statusLabel_ = new QLabel("Не подключено");
    statusLabel_->setStyleSheet(QString(
        "background-color: %1; color: %2; padding: 8px 16px; font-size: 9pt;")
        .arg(Theme::Surface.name(), Theme::Border.name()));

    inputEdit_ = new QLineEdit;
    inputEdit_->setPlaceholderText("Введите сообщение...");
    inputEdit_->setStyleSheet(QString(
        "QLineEdit { background-color: %1; color: %2; border: 1px solid %3; "
        "border-radius: 10px; padding: 10px 14px; font-size: 10pt; }"
        "QLineEdit:focus { border: 1px solid %4; }")
        .arg(Theme::SurfaceDark.name(), Theme::Text.name(),
             Theme::Border.name(), Theme::Accent.name()));

    attachBtn_ = new QToolButton;
    attachBtn_->setText(QString::fromUtf8("\xF0\x9F\x93\x8E"));
    attachBtn_->setFixedSize(40, 40);
    attachBtn_->setStyleSheet(QString("QToolButton { background-color: %1; color: %2; border: none; border-radius: 20px; font-family: 'Segoe UI Emoji', 'Apple Color Emoji', 'Noto Color Emoji'; font-size: 16px; }")
        .arg(Theme::Surface.name(), Theme::Text.name()));


    sendBtn_ = new QToolButton;
    sendBtn_->setText("➤");
    sendBtn_->setFixedSize(40, 40);
    sendBtn_->setStyleSheet(QString(
        "QToolButton { background-color: %1; color: white; border: none; "
        "border-radius: 20px; font-size: 13pt; }"
        "QToolButton:hover { background-color: #3f6ad8; }"
        "QToolButton:pressed { background-color: #2f4fa8; }")
        .arg(Theme::Accent.name()));

    auto* inputLayout = new QHBoxLayout;
    inputLayout->setContentsMargins(12, 8, 12, 12);
    inputLayout->setSpacing(8);
    inputLayout->addWidget(attachBtn_);
    inputLayout->addWidget(inputEdit_, 1);
    inputLayout->addWidget(sendBtn_);

    auto* inputContainer = new QWidget;
    inputContainer->setLayout(inputLayout);
    inputContainer->setStyleSheet(QString("background-color: %1;").arg(Theme::Background.name()));

    layout->addWidget(messagesList_, 1);
    layout->addWidget(statusLabel_);
    layout->addWidget(inputContainer);

    connect(sendBtn_, &QToolButton::clicked, this, &ChatWidget::sendMessage);
    connect(inputEdit_, &QLineEdit::returnPressed, this, &ChatWidget::sendMessage);
    connect(attachBtn_, &QToolButton::clicked, this, &ChatWidget::attachFile);
}

void ChatWidget::appendMessage(const QString& sender, const QString& text, bool own) {
    auto* item = new QListWidgetItem;
    item->setText(own ? QString("Вы: %1").arg(text) : QString("%1: %2").arg(sender, text));
    item->setForeground(own ? Theme::Accent : Theme::Text);
    messagesList_->addItem(item);
    scrollIfNeeded();
}

void ChatWidget::appendFile(const QString& sender, const QString& fileName, const QByteArray& data) {
    auto* item = new QListWidgetItem;
    item->setText(QString::fromUtf8("%1: \xF0\x9F\x93\x8E %2 (%3 byte)").arg(sender, fileName).arg(data.size()));
    item->setForeground(Theme::Text);
    messagesList_->addItem(item);
    scrollIfNeeded();
}

void ChatWidget::appendSystem(const QString& text) {
    auto* item = new QListWidgetItem;
    item->setText("ℹ " + text);
    item->setForeground(Theme::Border);
    messagesList_->addItem(item);
    scrollIfNeeded();
}

void ChatWidget::setConnectionStatus(const QString& status) { statusLabel_->setText(status); }
void ChatWidget::scrollIfNeeded() { if (autoScroll_) messagesList_->scrollToBottom(); }

void ChatWidget::sendMessage() {
    QString text = inputEdit_->text().trimmed();
    if (text.isEmpty()) return;
    inputEdit_->clear();
    emit messageSendRequested(text);
}

void ChatWidget::attachFile() {
    QString path = QFileDialog::getOpenFileName(this, "Select file");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Failed to open the file.");
        return;
    }
    QByteArray data = FileCompressor::compress(file.readAll());
    file.close();

    if (data.isEmpty()) {
        QMessageBox::warning(this, "Error", "Failed to compress the file.");
        return;
    }
    emit fileAttachRequested(QFileInfo(path).fileName(), data);
}