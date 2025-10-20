#include "crypto.h"

const EVP_CIPHER * get_des_3_cipher(const CipherMode mode)
{
    if (!mode) {
        perror("Invalid cipher mode for 3DES.");
        return NULL;
    }

    const EVP_CIPHER *chiphers[] = {
        EVP_des_ede3_ecb(),
        EVP_des_ede3_cfb(),
        EVP_des_ede3_ofb(),
        EVP_des_ede3_cbc()
    };

    return chiphers[mode - 1];
};

size_t des_3_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **ciphertext)
{
    LOG("Using 3DES encryption with mode %d\n", mode);
    return enc(plaintext, len, pass, get_des_3_cipher(mode), ciphertext);
}

size_t des_3_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **plaintext)
{
    LOG("Using 3DES decryption with mode %d\n", mode);
    return dec(ciphertext, len, pass, get_des_3_cipher(mode), plaintext);
}
