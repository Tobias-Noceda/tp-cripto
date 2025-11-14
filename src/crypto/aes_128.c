#include "crypto.h"

static const CipherGetter ciphers[] = {
    EVP_aes_128_cbc,
    EVP_aes_128_ecb,
    EVP_aes_128_cfb8,
    EVP_aes_128_ofb,
};

static inline const EVP_CIPHER *get_aes_128_cipher(const CIPHER_MODES mode)
{
    return ciphers[mode]();
}

size_t aes_128_encrypt(const uint8_t *plaintext, const size_t len, const char *pass, const CIPHER_MODES mode, uint8_t **ciphertext)
{
    LOG("Using AES-128 encryption with mode %d\n", mode);
    return enc(plaintext, len, pass, get_aes_128_cipher(mode), ciphertext);
}

size_t aes_128_decrypt(const uint8_t *ciphertext, const size_t len, const char *pass, const CIPHER_MODES mode, uint8_t **plaintext)
{
    LOG("Using AES-128 decryption with mode %d\n", mode);
    return dec(ciphertext, len, pass, get_aes_128_cipher(mode), plaintext);
}
