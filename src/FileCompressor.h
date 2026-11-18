#pragma once
#include <QByteArray>

class FileCompressor {
public:
    static QByteArray compress(const QByteArray& input);
    static QByteArray decompress(const QByteArray& input);
};
