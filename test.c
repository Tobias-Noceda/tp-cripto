#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/sha.h>

int main() {
    unsigned char hash[SHA_DIGEST_LENGTH];
    char *input = "10 diez";
    SHA1((unsigned char *)input, strlen(input), hash);

    printf("SHA1 hash of '%s': ", input);
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}