#include "crypto.h"

const EVP_CIPHER *get_aes_192_cipher(const CIPHER_MODES mode)
{
    const EVP_CIPHER *chiphers[] = {
        EVP_aes_192_ecb(),
        EVP_aes_192_cfb(),
        EVP_aes_192_ofb(),
        EVP_aes_192_cbc(),
    };

    return chiphers[mode];
}

size_t aes_192_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **ciphertext)
{
    LOG("Using AES-192 encryption with mode %d\n", mode);
    return enc(plaintext, len, pass, get_aes_192_cipher(mode), ciphertext);
}

size_t aes_192_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CIPHER_MODES mode, uint8_t **plaintext)
{
    LOG("Using AES-192 decryption with mode %d\n", mode);
    return dec(ciphertext, len, pass, get_aes_192_cipher(mode), plaintext);
}
