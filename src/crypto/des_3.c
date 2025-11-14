#include "crypto.h"

static const CipherGetter chiphers[] = {
    EVP_des_ede3_cbc,
    EVP_des_ede3_ecb,
    EVP_des_ede3_cfb8,
    EVP_des_ede3_ofb,
};

static inline const EVP_CIPHER *get_des_3_cipher(const CIPHER_MODES mode)
{
    return ciphers[mode]();
}

size_t des_3_encrypt(const uint8_t *plaintext, const size_t len, const char *pass, const CIPHER_MODES mode, uint8_t **ciphertext)
{
    LOG("Using 3DES encryption with mode %d\n", mode);
    return enc(plaintext, len, pass, get_des_3_cipher(mode), ciphertext);
}

size_t des_3_decrypt(const uint8_t *ciphertext, const size_t len, const char *pass, const CIPHER_MODES mode, uint8_t **plaintext)
{
    LOG("Using 3DES decryption with mode %d\n", mode);
    return dec(ciphertext, len, pass, get_des_3_cipher(mode), plaintext);
}
