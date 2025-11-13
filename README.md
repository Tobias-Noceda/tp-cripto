# stegobmp
This project implements three methods of steganography in files `.bmp` LSB1, LSB4 and LSBI. It also provides the options to encrypt the data before hidding it if wanted.

## Compiling

```bash
make all
```

## Running
The command mentions `[args]`, these will be specified in [next section](#program-arguments).
```bash
./bin/stego [args]
```

## Program arguments

| Parameter  | Description                                                                     |
| ---------- | ------------------------------------------------------------------------------- |
| `-extract` | Extraction mode. Obtain hidden information.                                     |
| `-embed`   | Embedding mode. Information will be hidden.                                     |
| `-in`      | Path to payload to hide                                                         |
| `-p`       | Path to porter image, which will hide the payload (must be a BMP file).         |
| `-out`     | Path to output file                                                             |
| `-steg`    | Steganography method (`<steganography_method>`: LSB1, LSB4, LSBI)               |
| `-a`       | Encryption method (`<encryption_method>`: aes128, aes192, aes256, 3des)         |
| `-m`       | Chaining mode (`<mode>`: ecb, cfb, ofb, cbc).                                   |
| `-pass`    | Password for encryption(`<password>`).                                          |

> [!NOTE]
> The program can only hide or uncover information.
> 
> Default encryption: AES128 in CBC mode.
>
> File paths can be with or without "".

## Examples
Bellow there´s a list of examples of commands that can be ran:
- Embeding without encryption:
```bash
./bin/stego -embed –in "mensaje1.txt" –p "imagen1.bmp" -out "imagenmas1.bmp" –steg LSBI
```
- Embeding with encryption:
```bash
./bin/stego -embed –in mensaje1.txt –p imagen1.bmp -out imagenmas1.bmp –steg LSBI –a
3des –m cbc -pass oculto
```
- Extracting without encryption:
```bash
./bin/stego -extract -p files/lado.input.bmp -out example.bmp -steg LSB1
```
- Extracting with encryption:
```bash
./bin/stego –extract –p "imagenmas1.bmp" -out "mensaje1" –steg LSBI –a 3des –m cbc -pass
"oculto"
```
To read more examples run:
```bash
./examples.sh
```


## Cleaning

```bash
make clean
```
