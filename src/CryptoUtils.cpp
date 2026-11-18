#include "CryptoUtils.h"
#include "CryptoKey.h" // <--- Подключаем сгенерированный ключ
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QDebug>

QByteArray CryptoUtils::encrypt(const QByteArray& data) {
    if (data.isEmpty()) return {};

    unsigned char iv[16];
    if (RAND_bytes(iv, sizeof(iv)) != 1) return {};

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, g_key, iv);

    int len;
    int ciphertext_len = data.size() + EVP_MAX_BLOCK_LENGTH;
    unsigned char* ciphertext = new unsigned char[ciphertext_len];

    EVP_EncryptUpdate(ctx, ciphertext, &len, reinterpret_cast<const unsigned char*>(data.constData()), data.size());
    int final_len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &final_len);
    ciphertext_len = len + final_len;

    EVP_CIPHER_CTX_free(ctx);

    QByteArray result;
    result.append(reinterpret_cast<const char*>(iv), 16);
    result.append(reinterpret_cast<const char*>(ciphertext), ciphertext_len);
    
    delete[] ciphertext;
    return result;
}

QByteArray CryptoUtils::decrypt(const QByteArray& data) {
    if (data.size() <= 16) return {};
    
    const unsigned char* iv = reinterpret_cast<const unsigned char*>(data.constData());
    const unsigned char* ciphertext = iv + 16;
    int ciphertext_len = data.size() - 16;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, g_key, iv);

    int len;
    int plaintext_len = ciphertext_len + EVP_MAX_BLOCK_LENGTH;
    unsigned char* plaintext = new unsigned char[plaintext_len];

    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    int final_len;
    
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &final_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        delete[] plaintext;
        return {};
    }
    plaintext_len = len + final_len;

    EVP_CIPHER_CTX_free(ctx);

    QByteArray result(reinterpret_cast<const char*>(plaintext), plaintext_len);
    delete[] plaintext;
    return result;
}