#include "LoginDialog.h"
#include "Theme.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("P2P Messenger — Вход");
    setFixedSize(420, 180);
    setStyleSheet(QString("background-color: %1;").arg(Theme::Background.name()));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    auto* title = new QLabel("Придумайте username");
    title->setStyleSheet(QString("color: %1; font-size: 15pt; font-weight: bold;").arg(Theme::Text.name()));

    usernameEdit_ = new QLineEdit;
    usernameEdit_->setPlaceholderText("username");
    usernameEdit_->setStyleSheet(QString(
        "QLineEdit { background-color: %1; border: 1px solid %2; border-radius: 6px; padding: 8px; color: %3; }")
        .arg(Theme::SurfaceDark.name(), Theme::Border.name(), Theme::Text.name()));

    auto* loginBtn = new QPushButton("Войти");
    loginBtn->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; border: none; border-radius: 6px; padding: 10px; font-weight: bold; }")
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
