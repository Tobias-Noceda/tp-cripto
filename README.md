# stegobmp

## Program arguments

| Parameter  | Description                                                             |
| ---------- | ----------------------------------------------------------------------- |
| `-extract` | Extraction mode. Obtain hidden information.                             |
| `-embed`   | Embedding mode. Information will be hidden.                             |
| `-in`      | Payload to hide                                                         |
| `-p`       | Porter image, which will hide the payload (must be a BMP file).         |
| `-out`     | Output file                                                             |
| `-steg`    | Steganography method (`<steganography_method>`: LSB1, LSB4, LSBI)       |
| `-a`       | Encryption method (`<encryption_method>`: aes128, aes192, aes256, 3des) |
| `-m`       | Chaining mode (`<mode>`: ecb, cfb, ofb, cbc).                           |
| `-pass`    | Password for encryption(`<password>`).                                  |

> [!NOTE]
> The program can only hide or uncover information.
> 
> Default encryption: AES128 in CBC mode.
