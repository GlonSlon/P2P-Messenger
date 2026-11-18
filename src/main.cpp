#include <QApplication>
#include <QFile>
#include "LoginDialog.h"
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("P2P Messenger");

    QFile styleFile(":/style.qss");
    if (styleFile.open(QIODevice::ReadOnly))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        MainWindow window(login.username());
        window.show();
        return app.exec();
    }
    return 0;
}
