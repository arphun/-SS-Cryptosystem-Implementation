#ifndef SS_CRYPTOSYSTEM_H
#define SS_CRYPTOSYSTEM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

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
void ss_make_pub(mpz_t prime_p, mpz_t prime_q, mpz_t modulus_n, uint64_t total_bits, uint64_t iterations);

/**
 * Generates a private key (d) for the S-S cryptosystem.
 *
 * Args:
 *   private_key_d (mpz_t): The private key (output).
 *   modulus_pq (mpz_t): The product of the primes p and q (output).
 *   prime_p (const mpz_t): The first prime factor.
 *   prime_q (const mpz_t): The second prime factor.
 */
void ss_make_priv(mpz_t private_key_d, mpz_t modulus_pq, const mpz_t prime_p, const mpz_t prime_q);

/**
 * Writes the public key to a file.
 *
 * Args:
 *   modulus_n (const mpz_t): The public key modulus.
 *   username (const char[]): The associated username.
 *   pbfile (FILE*): The file to write the public key to.
 */
void ss_write_pub(const mpz_t modulus_n, const char username[], FILE *pbfile);

/**
 * Writes the private key to a file.
 *
 * Args:
 *   modulus_pq (const mpz_t): The product of the primes p and q.
 *   private_key_d (const mpz_t): The private key.
 *   pvfile (FILE*): The file to write the private key to.
 */
void ss_write_priv(const mpz_t modulus_pq, const mpz_t private_key_d, FILE *pvfile);

/**
 * Reads the public key from a file.
 *
 * Args:
 *   modulus_n (mpz_t): The public key modulus (output).
 *   username (char[]): The associated username (output).
 *   pbfile (FILE*): The file to read the public key from.
 */
void ss_read_pub(mpz_t modulus_n, char username[], FILE *pbfile);

/**
 * Reads the private key from a file.
 *
 * Args:
 *   modulus_pq (mpz_t): The product of the primes p and q (output).
 *   private_key_d (mpz_t): The private key (output).
 *   pvfile (FILE*): The file to read the private key from.
 */
void ss_read_priv(mpz_t modulus_pq, mpz_t private_key_d, FILE *pvfile);

/**
 * Encrypts a message using the public key.
 *
 * Args:
 *   ciphertext (mpz_t): The encrypted message (output).
 *   plaintext (const mpz_t): The plaintext message.
 *   modulus_n (const mpz_t): The public key modulus.
 */
void ss_encrypt(mpz_t ciphertext, const mpz_t plaintext, const mpz_t modulus_n);

/**
 * Encrypts the contents of an input file and writes them to an output file using the public key.
 *
 * Args:
 *   infile (FILE*): The file containing the plaintext messages.
 *   outfile (FILE*): The file to write the encrypted messages to.
 *   modulus_n (const mpz_t): The public key modulus.
 */
void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t modulus_n);

/**
 * Decrypts a message using the private key.
 *
 * Args:
 *   plaintext (mpz_t): The decrypted message (output).
 *   ciphertext (const mpz_t): The encrypted message.
 *   private_key_d (const mpz_t): The private key.
 *   modulus_pq (const mpz_t): The product of the primes p and q.
 */
void ss_decrypt(mpz_t plaintext, const mpz_t ciphertext, const mpz_t private_key_d, const mpz_t modulus_pq);

/**
 * Decrypts the contents of an input file and writes the plaintext to an output file using the private key.
 *
 * Args:
 *   infile (FILE*): The file containing the encrypted messages.
 *   outfile (FILE*): The file to write the decrypted messages to.
 *   private_key_d (const mpz_t): The private key.
 *   modulus_pq (const mpz_t): The product of the primes p and q.
 */
void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t private_key_d, const mpz_t modulus_pq);

#endif // SS_CRYPTOSYSTEM_H