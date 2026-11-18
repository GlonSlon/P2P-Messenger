#pragma once
#include <QByteArray>

class CryptoUtils {
public:
    static QByteArray encrypt(const QByteArray& data);
    static QByteArray decrypt(const QByteArray& data);
};
