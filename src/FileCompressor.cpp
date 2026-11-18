#include "FileCompressor.h"
#include <zlib.h>

QByteArray FileCompressor::compress(const QByteArray& input) {
    if (input.isEmpty()) return {};
    uLongf compressedSize = compressBound(static_cast<uLong>(input.size()));
    QByteArray output(static_cast<int>(compressedSize), '\0');

    int result = compress2(
        reinterpret_cast<Bytef*>(output.data()), &compressedSize,
        reinterpret_cast<const Bytef*>(input.constData()),
        static_cast<uLong>(input.size()), Z_BEST_COMPRESSION);

    if (result != Z_OK) return {};
    output.resize(static_cast<int>(compressedSize));
    return output;
}

QByteArray FileCompressor::decompress(const QByteArray& input) {
    if (input.isEmpty()) return {};
    uLongf bufSize = static_cast<uLongf>(input.size()) * 8 + 1024;

    for (int i = 0; i < 12; ++i) {
        QByteArray output(static_cast<int>(bufSize), '\0');
        uLongf destLen = bufSize;

        int result = uncompress(
            reinterpret_cast<Bytef*>(output.data()), &destLen,
            reinterpret_cast<const Bytef*>(input.constData()),
            static_cast<uLong>(input.size()));

        if (result == Z_OK) {
            output.resize(static_cast<int>(destLen));
            return output;
        }
        if (result != Z_BUF_ERROR) return {};
        bufSize *= 2;
    }
    return {};
}
