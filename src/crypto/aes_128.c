#include <crypto.h>
#include <logs.h>
#include <string.h>

#include <openssl/aes.h>
#include <openssl/des.h>
#include <openssl/evp.h>

#define MAX_ENCR_LENGTH 1024
#define SUCCESS 0
#define FAILURE !SUCCESS
#define AES_KEY_SIZE 16


// int AES_set_encrypt_key(const uint8_t *userKey, const int bits, AES_KEY *key);

size_t aes_128_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **ciphertext)
{
    int outl, templ;
    uint8_t key[AES_KEY_SIZE];
    uint8_t iv[AES_KEY_SIZE];

    EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), NULL, pass, strlen((char *)pass), 1, key, iv);

    /* Inicializar contexto */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);

    *ciphertext = malloc(MAX_ENCR_LENGTH); // Allocate memory for ciphertext
    if (*ciphertext == NULL)
    {
        perror("Memory allocation failed");

        EVP_CIPHER_CTX_free(ctx);

        return 0;
    }

    EVP_EncryptUpdate(ctx, *ciphertext, &outl, plaintext, len);
    EVP_EncryptFinal_ex(ctx, *ciphertext + outl, &templ);

    outl += templ;

    /* Borrar estructura de contexto */
    EVP_CIPHER_CTX_free(ctx);

    LOG("\nTamaño del texto cifrado: %d bytes.\n", outl);

    return outl;
}

size_t aes_128_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **plaintext)
{
    int outl, templ;
    uint8_t key[AES_KEY_SIZE];
    uint8_t iv[AES_KEY_SIZE];

    EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), NULL, pass, strlen((char *)pass), 1, key, iv);

    /* Inicializar contexto */
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv);

    *plaintext = malloc(MAX_ENCR_LENGTH); // Allocate memory for plaintext
    if (*plaintext == NULL)
    {
        perror("Memory allocation failed");

        EVP_CIPHER_CTX_free(ctx);

        return 0;
    }

    EVP_DecryptUpdate(ctx, *plaintext, &outl, ciphertext, len);
    EVP_DecryptFinal_ex(ctx, *plaintext + outl, &templ);

    outl += templ;

    /* Borrar estructura de contexto */
    EVP_CIPHER_CTX_free(ctx);

    LOG("\nTamaño del texto plano: %d bytes.\n", outl);

    return outl;
}

size_t aes_192_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **ciphertext)
{
    // Implement AES-192 encryption logic here
    return 0; // Placeholder return value
}

size_t aes_192_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **plaintext)
{
    // Implement AES-192 decryption logic here
    return 0; // Placeholder return value
}

size_t aes_256_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **ciphertext)
{
    // Implement AES-256 encryption logic here
    return 0; // Placeholder return value
}

size_t aes_256_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **plaintext)
{
    // Implement AES-256 decryption logic here
    return 0; // Placeholder return value
}

size_t des_3_encrypt(const uint8_t *plaintext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **ciphertext)
{
    // Implement 3DES encryption logic here
    return 0; // Placeholder return value
}

size_t des_3_decrypt(const uint8_t *ciphertext, const size_t len, const uint8_t *pass, const CipherMode mode, uint8_t **plaintext)
{
    // Implement 3DES decryption logic here
    return 0; // Placeholder return value
}
