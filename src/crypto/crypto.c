#include "crypto.h"

#include <logs.h>

#include <stdbool.h>
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

/**
 * @brief Converts plaintext to ciphertext and vice versa using OpenSSL EVP interface.
 *
 * @param in The input data (plaintext or ciphertext).
 * @param len Length of the input data.
 * @param pass The password used for key derivation.
 * @param cipher The EVP_CIPHER to use (e.g., EVP_aes_256_cbc()).
 * @param out Pointer to store the output data (ciphertext or plaintext).
 * @param encrypt Boolean flag indicating whether to encrypt (true) or decrypt (false).
 * @return size_t Length of the output data, or 0 on error.
 */
static size_t ssl(const uint8_t *in, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **out, bool encrypt)
{
    int key_len = EVP_CIPHER_get_key_length(cipher);

    uint8_t keyiv[key_len];
    // Or EVP_BytesToKey, pick your poison
    if (!PKCS5_PBKDF2_HMAC((const char *)pass, strlen((char *)pass), NULL, 0, 10000, EVP_sha256(), key_len, keyiv))
        return OPENSSL_ERR(NULL);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        return OPENSSL_ERR(NULL);

    if (!EVP_CipherInit_ex2(ctx, cipher, keyiv, keyiv + key_len / 2, encrypt ? 1 : 0, NULL))
        return OPENSSL_ERR(ctx);

    *out = malloc(len + EVP_CIPHER_get_block_size(cipher));
    if (*out == NULL)
        return CUSTOM_ERR(ctx, "Memory allocation failed");

    int output_length = 0;
    if (!EVP_CipherUpdate(ctx, *out, &output_length, in, len))
        return OPENSSL_ERR(ctx);

    int padding_length = 0;
    if (!EVP_CipherFinal_ex(ctx, *out + output_length, &padding_length))
        return OPENSSL_ERR(ctx);

    EVP_CIPHER_CTX_free(ctx);

    return output_length + padding_length;
}

inline size_t enc(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **ciphertext)
{
    return ssl(plaintext, len, pass, cipher, ciphertext, true);
}

inline size_t dec(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **plaintext)
{
    return ssl(ciphertext, len, pass, cipher, plaintext, false);
}
