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

#define OPTIONS "i:o:n:vh"

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
    int option;
    bool verbose_mode = false;

    // Default private key file
    char *private_key_file = malloc(sizeof(char) * 100);
    strcpy(private_key_file, "ss.priv");

    char *input_filepath = NULL;
    char *output_filepath = NULL;

    FILE *input_file = stdin;
    FILE *output_file = stdout;

    // Parse command-line arguments
    while ((option = getopt(argc, argv, OPTIONS)) != -1) {
        switch (option) {
            case 'i':
                input_filepath = optarg;
                input_file = open_file(input_filepath, "r");  // Open input file
                break;
            case 'o':
                output_filepath = optarg;
                output_file =
                    open_file(output_filepath, "w");  // Open output file
                break;
            case 'n':
                strcpy(private_key_file,
                       optarg);  // Update private key file path
                break;
            case 'v':
                verbose_mode = true;  // Enable verbose mode
                break;
            case 'h':
                // clang-format off
                printf(
                    "SYNOPSIS\n"
                    "   Decrypts encrypted files using the corresponding private key.\n"
                    "\n"
                    "USAGE\n"
                    "   %s [-i:o:n:vh] [-i input_file] [-o output_file] [-n private_key_file]\n"
                    "\n"
                    "OPTIONS\n"
                    "   -i              Specifies the input file to decrypt (default: stdin).\n"
                    "   -o              Specifies the output file to decrypt (default: stdout).\n"
                    "   -n              Specifies the file containing the private key (default: ss.priv).\n"
                    "   -v              Enables verbose output.\n"
                    "   -h              Prints this help message.\n",
                    argv[0]);
                // clang-format on
                exit(0);
                break;
            default:
                fprintf(stderr, "Invalid option: -%c\n", optopt);
                exit(1);
        }
    }
    // Open private key file
    FILE *private_key_fp = open_file(private_key_file, "r");

    // Initialize GMP variables
    mpz_t private_key_modulus, private_key_exponent;
    mpz_inits(private_key_modulus, private_key_exponent, NULL);

    // Read private key from file
    ss_read_priv(private_key_modulus, private_key_exponent, private_key_fp);

    // Verbose output: Display private key information
    if (verbose_mode) {
        gmp_printf("Private key modulus (pq, %u bits) = %Zd\n",
                   mpz_sizeinbase(private_key_modulus, 2), private_key_modulus);
        gmp_printf("Private key exponent (d, %u bits) = %Zd\n",
                   mpz_sizeinbase(private_key_exponent, 2),
                   private_key_exponent);
    }
    // Decrypt the input file
    ss_decrypt_file(input_file, output_file, private_key_exponent,
                    private_key_modulus);

    // Clear GMP variables and close files
    mpz_clears(private_key_modulus, private_key_exponent, NULL);
    fclose(private_key_fp);
    fclose(input_file);
    fclose(output_file);

    // Free dynamically allocated memory
    free(private_key_file);

    return 0;
}
