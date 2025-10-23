#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdlib.h>
#include <stdint.h>

typedef enum {
    ECB = 0,
    CFB,
    OFB,
    CBC
} CIPHER_MODES;

/**
 * @brief Encrypts the given plaintext using the specified pass.
 * 
 * @param plaintext The plaintext to encrypt.
 * @param len The length of the plaintext.
 * @param pass The encryption pass.
 * @param mode The cipher mode to use for encryption.
 * @param ciphertext A pointer to the location where the ciphertext will be stored.
 * @return The length of the ciphertext.
 */
typedef size_t (*Encrypt)(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **ciphertext);

/**
 * @brief Decrypts the given ciphertext using the specified pass.
 * 
 * @param ciphertext The ciphertext to decrypt.
 * @param len The length of the ciphertext.
 * @param pass The decryption pass.
 * @param mode The cipher mode to use for decryption.
 * @param plaintext A pointer to the location where the plaintext will be stored.
 * @return The length of the plaintext.
 */
typedef size_t (*Decrypt)(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **plaintext);

size_t aes_128_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **ciphertext);
size_t aes_128_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **plaintext);

size_t aes_192_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **ciphertext);
size_t aes_192_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **plaintext);

size_t aes_256_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **ciphertext);
size_t aes_256_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **plaintext);

size_t des_3_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **ciphertext);
size_t des_3_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **plaintext);

#endif
