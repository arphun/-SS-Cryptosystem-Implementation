#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

#include "numtheory.h"

/**
 * Generates a public key (n) for the S-S cryptosystem.
 *
 * Args:
 *   prime_p (mpz_t): The first prime factor (output).
 *   prime_q (mpz_t): The second prime factor (output).
 *   modulus_n (mpz_t): The public key modulus (output).
 *   total_bits (uint64_t): The total number of bits for modulus_n.
 *   iterations (uint64_t): Number of iterations for primality testing.
 */
void ss_make_pub(mpz_t prime_p, mpz_t prime_q, mpz_t modulus_n,
                 uint64_t total_bits, uint64_t iterations) {
    mpz_inits(prime_p, prime_q, modulus_n, NULL);

    // Determine the range for p's bit size
    uint64_t min_p_bits = total_bits / 5;
    uint64_t max_p_bits = (2 * total_bits) / 5;
    uint64_t prime_p_bits;

    mpz_t squared_p, p_minus1, q_minus1, mod_check1, mod_check2;
    mpz_inits(squared_p, p_minus1, q_minus1, mod_check1, mod_check2, NULL);

    while (true) {
        // Randomly determine bit size for p within range
        prime_p_bits = rand() % (max_p_bits - min_p_bits + 1) + min_p_bits;

        // Generate prime p
        make_prime(prime_p, prime_p_bits, iterations);
        mpz_mul(squared_p, prime_p, prime_p);

        // Calculate the bit size of squared_p
        uint64_t squared_p_bits = mpz_sizeinbase(squared_p, 2);
        uint64_t prime_q_bits = total_bits - squared_p_bits;

        // Generate prime q
        make_prime(prime_q, prime_q_bits, iterations);

        // Check if p and q satisfy the conditions
        mpz_sub_ui(p_minus1, prime_p, 1);
        mpz_sub_ui(q_minus1, prime_q, 1);
        mpz_mod(mod_check1, q_minus1, prime_p);
        mpz_mod(mod_check2, p_minus1, prime_q);
        if (mpz_cmp_ui(mod_check1, 0) != 0 && mpz_cmp_ui(mod_check2, 0) != 0) {
            break;  // Conditions satisfied, exit loop
        }
    }
    // Calculate modulus_n = p^2 * q
    mpz_mul(modulus_n, prime_p, prime_p);
    mpz_mul(modulus_n, modulus_n, prime_q);

    mpz_clears(squared_p, p_minus1, q_minus1, mod_check1, mod_check2, NULL);

    return;
}

/**
 * Generates a private key (d) for the S-S cryptosystem.
 *
 * Args:
 *   private_key_d (mpz_t): The private key (output).
 *   modulus_pq (mpz_t): The product of the primes p and q (output).
 *   prime_p (const mpz_t): The first prime factor.
 *   prime_q (const mpz_t): The second prime factor.
 */
void ss_make_priv(mpz_t private_key_d, mpz_t modulus_pq, const mpz_t prime_p,
                  const mpz_t prime_q) {
    mpz_inits(private_key_d, modulus_pq, NULL);

    mpz_t n, lambda, gcd_value;
    mpz_inits(lambda, gcd_value, n, NULL);

    // Compute modulus_pq = p * q
    mpz_mul(modulus_pq, prime_p, prime_q);
    mpz_mul(n, modulus_pq, prime_p);

    // Compute λ(n) = lcm(p-1, q-1)
    mpz_t p_minus1, q_minus1;
    mpz_inits(p_minus1, q_minus1, NULL);
    mpz_sub_ui(p_minus1, prime_p, 1);
    mpz_sub_ui(q_minus1, prime_q, 1);
    mpz_mul(lambda, p_minus1, q_minus1);
    gcd(gcd_value, p_minus1, q_minus1);
    mpz_divexact(lambda, lambda, gcd_value);

    // Compute d = n^(-1) mod λ(n)
    mod_inverse(private_key_d, n, lambda);

    mpz_clears(n, lambda, gcd_value, p_minus1, q_minus1, NULL);

    return;
}

/**
 * Writes the public key to a file.
 *
 * Args:
 *   modulus_n (const mpz_t): The public key modulus.
 *   username (const char[]): The associated username.
 *   pbfile (FILE*): The file to write the public key to.
 */
void ss_write_pub(const mpz_t modulus_n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", modulus_n, username);
}

/**
 * Writes the private key to a file.
 *
 * Args:
 *   modulus_pq (const mpz_t): The product of the primes p and q.
 *   private_key_d (const mpz_t): The private key.
 *   pvfile (FILE*): The file to write the private key to.
 */
void ss_write_priv(const mpz_t modulus_pq, const mpz_t private_key_d,
                   FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", modulus_pq, private_key_d);
}

/**
 * Reads the public key from a file.
 *
 * Args:
 *   modulus_n (mpz_t): The public key modulus (output).
 *   username (char[]): The associated username (output).
 *   pbfile (FILE*): The file to read the public key from.
 */
void ss_read_pub(mpz_t modulus_n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%s\n", modulus_n, username);
}

/**
 * Reads the private key from a file.
 *
 * Args:
 *   modulus_pq (mpz_t): The product of the primes p and q (output).
 *   private_key_d (mpz_t): The private key (output).
 *   pvfile (FILE*): The file to read the private key from.
 */
void ss_read_priv(mpz_t modulus_pq, mpz_t private_key_d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", modulus_pq, private_key_d);
}

/**
 * Encrypts a message using the public key.
 *
 * Args:
 *   ciphertext (mpz_t): The encrypted message (output).
 *   plaintext (const mpz_t): The plaintext message.
 *   modulus_n (const mpz_t): The public key modulus.
 */
void ss_encrypt(mpz_t ciphertext, const mpz_t plaintext,
                const mpz_t modulus_n) {
    mpz_powm(ciphertext, plaintext, modulus_n, modulus_n);
}

/**
 * Encrypts the contents of an input file and writes them to an output file
 * using the public key.
 *
 * Args:
 *   infile (FILE*): The file containing the plaintext messages.
 *   outfile (FILE*): The file to write the encrypted messages to.
 *   modulus_n (const mpz_t): The public key modulus.
 */
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t modulus_n) {
    // Determine block size based on the square root of modulus_n
    mpz_t sqrt_modulus;
    mpz_init(sqrt_modulus);
    mpz_sqrt(sqrt_modulus, modulus_n);

    uint16_t block_size = 0;
    while (mpz_cmp_ui(sqrt_modulus, 0) != 0) {
        mpz_div_ui(sqrt_modulus, sqrt_modulus, 2);
        block_size++;
    }
    block_size--;
    block_size /= 8;

    while (true) {
        uint8_t *plaintext_block =
            (uint8_t *)calloc(block_size, sizeof(uint8_t));
        plaintext_block[0] = 0xFF;  // Padding byte

        // Read a block of data from the input file
        uint16_t bytes_read = fread(&(plaintext_block[1]), sizeof(uint8_t),
                                    block_size - 1, infile);
        if (bytes_read == 0) {
            free(plaintext_block);
            break;  // End of file
        }

        // Encrypt the block
        mpz_t plaintext, ciphertext;
        mpz_inits(plaintext, ciphertext, NULL);
        mpz_import(plaintext, block_size, 1, 1, 1, 0, plaintext_block);

        ss_encrypt(ciphertext, plaintext, modulus_n);
        gmp_fprintf(outfile, "%Zx\n", ciphertext);

        // Clean up and free memory
        mpz_clears(plaintext, ciphertext, NULL);
        free(plaintext_block);
    }

    mpz_clear(sqrt_modulus);
}

/**
 * Decrypts a message using the private key.
 *
 * Args:
 *   plaintext (mpz_t): The decrypted message (output).
 *   ciphertext (const mpz_t): The encrypted message.
 *   private_key_d (const mpz_t): The private key.
 *   modulus_pq (const mpz_t): The product of the primes p and q.
 */
void ss_decrypt(mpz_t plaintext, const mpz_t ciphertext,
                const mpz_t private_key_d, const mpz_t modulus_pq) {
    mpz_powm(plaintext, ciphertext, private_key_d, modulus_pq);
}

/**
 * Decrypts the contents of an input file and writes the plaintext to an output
 * file using the private key.
 *
 * Args:
 *   infile (FILE*): The file containing the encrypted messages.
 *   outfile (FILE*): The file to write the decrypted messages to.
 *   private_key_d (const mpz_t): The private key.
 *   modulus_pq (const mpz_t): The product of the primes p and q.
 */
void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t private_key_d,
                     const mpz_t modulus_pq) {
    // Determine block size based on modulus_pq
    mpz_t modulus_copy;
    mpz_init(modulus_copy);
    mpz_set(modulus_copy, modulus_pq);

    uint16_t block_size = 0;
    while (mpz_cmp_ui(modulus_copy, 0) != 0) {
        mpz_div_ui(modulus_copy, modulus_copy, 2);
        block_size++;
    }
    block_size--;
    block_size /= 8;

    mpz_t ciphertext, plaintext;
    mpz_inits(ciphertext, plaintext, NULL);

    while (true) {
        // Read an encrypted message from the input file
        if (gmp_fscanf(infile, "%Zx\n", ciphertext) == -1) {
            break;  // End of file
        }

        // Decrypt the message
        ss_decrypt(plaintext, ciphertext, private_key_d, modulus_pq);

        // Export the decrypted message to a byte array
        uint8_t *plaintext_block =
            (uint8_t *)calloc(block_size, sizeof(uint8_t));
        size_t *decrypted_size = (size_t *)calloc(1, sizeof(size_t));
        mpz_export(plaintext_block, decrypted_size, 1, 1, 1, 0, plaintext);

        // Write the decrypted message to the output file
        fwrite(&(plaintext_block[1]), 1, *decrypted_size - 1, outfile);

        // Clean up and free memory
        free(plaintext_block);
        free(decrypted_size);
    }
    mpz_clears(ciphertext, plaintext, modulus_copy, NULL);
}
