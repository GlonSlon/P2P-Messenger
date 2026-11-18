#include "LoginDialog.h"
#include "Theme.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("P2P Messenger — Вход");
    setFixedSize(440, 220);
    setStyleSheet(QString("background-color: %1;").arg(Theme::Background.name()));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(32, 32, 32, 32);
    layout->setSpacing(16);

    auto* title = new QLabel("Придумайте username");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(QString(
        "color: %1; font-size: 16pt; font-weight: bold; background: transparent;")
        .arg(Theme::Text.name()));

    usernameEdit_ = new QLineEdit;
    usernameEdit_->setPlaceholderText("username");
    usernameEdit_->setStyleSheet(QString(
        "QLineEdit { background-color: %1; border: 1px solid %2; "
        "border-radius: 8px; padding: 12px 14px; color: %3; font-size: 11pt; }"
        "QLineEdit:focus { border: 1px solid %4; }")
        .arg(Theme::SurfaceDark.name(), Theme::Border.name(),
             Theme::Text.name(), Theme::Accent.name()));

    auto* loginBtn = new QPushButton("Войти");
    loginBtn->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; border: none; "
        "border-radius: 8px; padding: 12px; font-weight: bold; font-size: 11pt; }"
        "QPushButton:hover { background-color: #3f6ad8; }"
        "QPushButton:pressed { background-color: #2f4fa8; }")
        .arg(Theme::Accent.name()));

    layout->addWidget(title);
    layout->addWidget(usernameEdit_);
    layout->addStretch();
    layout->addWidget(loginBtn);

    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(usernameEdit_, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}

void LoginDialog::onLoginClicked() {
    QString user = usernameEdit_->text().trimmed();
    if (user.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Username не может быть пустым.");
        return;
    }
    username_ = user;
    accept();
}