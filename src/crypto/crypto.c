#include "crypto.h"

#include <logs.h>

#include <string.h>

#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/evp.h>
#include <openssl/err.h>

static inline int OPENSSL_ERR(EVP_CIPHER_CTX *ctx)
{
    ERR_print_errors_fp(stderr);
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

static inline int CUSTOM_ERR(EVP_CIPHER_CTX *ctx, char *msg)
{
    perror(msg);
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}

size_t enc(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **ciphertext)
{
    uint8_t key[EVP_MAX_KEY_LENGTH];
    uint8_t iv[EVP_MAX_IV_LENGTH];
    if (!EVP_BytesToKey(cipher, EVP_md5(), NULL, pass, strlen((char *)pass), 1, key, iv))
    {
        return OPENSSL_ERR(NULL);
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        return OPENSSL_ERR(NULL);
    }

    if (!EVP_EncryptInit_ex(ctx, cipher, NULL, key, iv))
    {
        return OPENSSL_ERR(ctx);
    }

    int buffer = len + EVP_CIPHER_get_block_size(cipher) - 1;

    *ciphertext = malloc(buffer);
    if (*ciphertext == NULL)
    {
        CUSTOM_ERR(ctx, "Memory allocation failed");
        return 0;
    }

    int output_length = 0;
    if (!EVP_EncryptUpdate(ctx, *ciphertext, &output_length, plaintext, len))
    {
        return OPENSSL_ERR(ctx);
    }

    int padding_length = 0;
    if (!EVP_EncryptFinal_ex(ctx, *ciphertext + output_length, &padding_length))
    {
        return OPENSSL_ERR(ctx);
    }

    EVP_CIPHER_CTX_free(ctx);

    return output_length + padding_length;
}

size_t dec(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **plaintext)
{
    uint8_t key[EVP_MAX_KEY_LENGTH];
    uint8_t iv[EVP_MAX_IV_LENGTH];
    if (!EVP_BytesToKey(cipher, EVP_md5(), NULL, pass, strlen((char *)pass), 1, key, iv))
    {
        return OPENSSL_ERR(NULL);
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        return OPENSSL_ERR(NULL);
    }

    if (!EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv))
    {
        return OPENSSL_ERR(ctx);
    }

    *plaintext = malloc(len);
    if (*plaintext == NULL)
    {
        CUSTOM_ERR(ctx, "Memory allocation failed");
        return 0;
    }

    int output_length = 0;
    if (!EVP_DecryptUpdate(ctx, *plaintext, &output_length, ciphertext, len))
    {
        return OPENSSL_ERR(ctx);
    }

    int padding_length = 0;
    if (!EVP_DecryptFinal_ex(ctx, *plaintext + output_length, &padding_length))
    {
        return OPENSSL_ERR(ctx);
    }

    EVP_CIPHER_CTX_free(ctx);

    return output_length + padding_length;
}
