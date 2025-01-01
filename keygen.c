#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "numtheory.h"
#include "randstate.h"
#include "ss.h"

#define OPTIONS "b:i:n:d:s:vh"

/**
 * Opens a file with the specified mode and handles errors.
 *
 * Args:
 *   filepath (char *): The path of the file to open.
 *   mode (char *): The mode to open the file in ("r", "w", etc.).
 *
 * Returns:
 *   FILE*: A pointer to the opened file.
 */
FILE *open_file(const char *filepath, const char *mode) {
    FILE *file = fopen(filepath, mode);
    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filepath);
        exit(1);
    }
    fchmod(fileno(file), 0600);  // Set file permissions to owner read/write.
    return file;
}

int main(int argc, char **argv) {
    // Default values for command-line options
    int option;
    uint32_t total_bits = 10;
    uint32_t prime_test_iters = 50;
    char *public_key_file = (char *)malloc(sizeof(char) * 100);
    char *private_key_file = (char *)malloc(sizeof(char) * 100);

    strcpy(public_key_file, "ss.pub");
    strcpy(private_key_file, "ss.priv");

    uint32_t random_seed = time(NULL);
    bool verbose_mode = true;

    // Parse command-line options
    while ((option = getopt(argc, argv, OPTIONS)) != -1) {
        switch (option) {
            case 'b':
                total_bits = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'i':
                prime_test_iters = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'n':
                strcpy(public_key_file, optarg);
                break;
            case 'd':
                strcpy(private_key_file, optarg);
                break;
            case 's':
                random_seed = (uint32_t)strtoul(optarg, NULL, 10);
                break;
            case 'v':
                verbose_mode = true;
                break;
            case 'h':
                printf(
                    "SYNOPSIS\n"
                    "   Generates public and private keys for the S-S cryptosystem.\n"
                    "\n"
                    "USAGE\n"
                    "   %s [-b:i:n:d:s:vh] [-b bits] [-i iters] [-n public_key_file] "
                    "[-d private_key_file] [-s seed]\n"
                    "\n"
                    "OPTIONS\n"
                    "   -b bits               Specify the number of bits for the public modulus (default: 10).\n"
                    "   -i iterations         Number of Miller-Rabin primality test iterations (default: 50).\n"
                    "   -n public_key_file    Path to the public key file (default: ss.pub).\n"
                    "   -d private_key_file   Path to the private key file (default: ss.priv).\n"
                    "   -s seed               Random seed for initialization (default: UNIX time).\n"
                    "   -v                    Enable verbose output.\n"
                    "   -h                    Display this help message.\n",
                    argv[0]);
                exit(0);
            default:
                fprintf(stderr, "Invalid option: -%c\n", optopt);
                exit(1);
        }
    }

    // Open files for public and private keys
    FILE *public_key_fp = open_file(public_key_file, "w");
    FILE *private_key_fp = open_file(private_key_file, "w");

    // Initialize random state
    randstate_init(random_seed);

    // Generate public key
    mpz_t prime_p, prime_q, modulus_n;
    ss_make_pub(prime_p, prime_q, modulus_n, total_bits, prime_test_iters);
    char *username = getenv("USER");
    ss_write_pub(modulus_n, username, public_key_fp);

    // Generate private key
    mpz_t private_key_d, modulus_pq;
    ss_make_priv(private_key_d, modulus_pq, prime_p, prime_q);
    ss_write_priv(modulus_pq, private_key_d, private_key_fp);

    // Clear random state
    randstate_clear();

    if (verbose_mode) {
        printf("Username: %s\n", username);
        gmp_printf("prime_p  (%u bits) = %Zd\n", mpz_sizeinbase(prime_p, 2),
                   prime_p);
        gmp_printf("prime_q  (%u bits) = %Zd\n", mpz_sizeinbase(prime_q, 2),
                   prime_q);
        gmp_printf("modulus_n (%u bits) = %Zd\n", mpz_sizeinbase(modulus_n, 2),
                   modulus_n);
        gmp_printf("modulus_pq (%u bits) = %Zd\n",
                   mpz_sizeinbase(modulus_pq, 2), modulus_pq);
        gmp_printf("private_key_d (%u bits) = %Zd\n",
                   mpz_sizeinbase(private_key_d, 2), private_key_d);
    }

    // Cleanup GMP variables and close files
    mpz_clears(prime_p, prime_q, modulus_n, private_key_d, modulus_pq, NULL);
    fclose(public_key_fp);
    fclose(private_key_fp);
    free(public_key_file);
    free(private_key_file);
    return 0;
}
