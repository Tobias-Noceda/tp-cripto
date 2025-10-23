#ifndef CRYPTO_PRIV_H
#define CRYPTO_PRIV_H

#include <openssl/evp.h>

#include <crypto.h>
#include <logs.h>

/**
 * @brief Encrypts the given plaintext using the specified cipher and password.
 * Uses OpenSSL EVP interface.
 *
 * @param plaintext The data to encrypt.
 * @param len Length of the plaintext data.
 * @param pass Password used for encryption (NULL-terminated string).
 * @param cipher The EVP_CIPHER to use for encryption.
 * @param ciphertext Ciphertext output (allocated within the function), must be freed by the caller.
 *
 * @return size_t Length of the ciphertext output.
 */
size_t enc(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **ciphertext);

/**
 * @brief Encrypts the given plaintext using the specified cipher and password.
 * Uses OpenSSL EVP interface.
 *
 * @param ciphertext The data to decrypt.
 * @param len Length of the ciphertext data.
 * @param pass Password used for decryption (NULL-terminated string).
 * @param cipher The EVP_CIPHER to use for decryption.
 * @param plaintext Plaintext output (allocated within the function), must be freed by the caller.
 *
 * @return size_t Length of the plaintext output.
 */
size_t dec(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const EVP_CIPHER *cipher, uint8_t **plaintext);

#endif
