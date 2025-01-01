
# SS Cryptography Project

This project implements public key cryptography using the SS algorithm, enabling key generation, encryption, and decryption.

---

## Features

- **Key Generation**: Generates a pair of public and private keys using the RSA algorithm.
- **Encryption**: Secures plaintext messages into ciphertext using the public key.
- **Decryption**: Recovers plaintext messages from ciphertext using the private key.
- Implements key cryptographic concepts such as modular arithmetic and random state management.

---

## Installation

### Prerequisites
- A Unix-like environment (Linux/macOS).
- A C compiler such as GCC.

### Compilation
To compile the project, use the provided `Makefile`:
```bash
make
```
This generates the following executable files:
- `keygen`
- `encrypt`
- `decrypt`

---

## Usage

### Key Generation
To generate public and private keys:
```bash
./keygen -b <bits> -i <iterations> -n <public_key_file> -d <private_key_file> -s <seed> -v
```
Example:
```bash
./keygen -b 256 -i 100 -n public.key -d private.key -s 12345 -v
```

### Encryption
To encrypt a message:
```bash
./encrypt -i <input_file> -o <output_file> -n <public_key_file>
```
Example:
```bash
./encrypt -i message.txt -o encrypted.bin -n pubkey.pem
```

### Decryption
To decrypt a message:
```bash
./decrypt -i <input_file> -o <output_file> -n <private_key_file>
```
Example:
```bash
./decrypt -i encrypted.bin -o decrypted.txt -n privkey.pem
```

---

## Project Structure

- **`keygen.c`**: Implements key generation using the SS algorithm.
- **`encrypt.c`**: Encrypts plaintext messages using a public key.
- **`decrypt.c`**: Decrypts ciphertext messages using a private key.
- **`numtheory.c`**: Provides number-theoretic utilities, such as modular exponentiation.
- **`randstate.c`**: Manages random state for cryptographic operations.
- **`ss.c`**: Implements shared components of the RSA cryptographic process.
- **`Makefile`**: Simplifies compilation of the project.

---

## License

This project is for educational purposes only as part of the CSE 13S course.

---

## Acknowledgments

Thanks to Prof. Darrell D.E. Long for guidance and for structuring this assignment to introduce fundamental cryptographic concepts.

---
