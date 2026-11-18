#pragma once
#include <QDialog>
#include <QLineEdit>

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);
    QString username() const { return username_; }

private slots:
    void onLoginClicked();

private:
    QLineEdit* usernameEdit_;
    QString    username_;
};

