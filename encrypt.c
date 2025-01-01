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
    // Command-line option variables
    int option;
    bool verbose_mode = false;

    // Default file paths
    char *public_key_file = malloc(sizeof(char) * 100);
    strcpy(public_key_file, "ss.pub");

    char *username = malloc(sizeof(char) * 100);

    // Input and output file pointers
    FILE *input_file = stdin;    // Default to standard input
    FILE *output_file = stdout;  // Default to standard output
    // Parse command-line arguments
    while ((option = getopt(argc, argv, OPTIONS)) != -1) {
        switch (option) {
            case 'i': {
                char *input_filepath = optarg;  // Get input file path
                input_file = open_file(input_filepath, "r");
                break;
            }
            case 'o': {
                char *output_filepath = optarg;  // Get output file path
                output_file = open_file(output_filepath, "w");
                break;
            }
            case 'n':
                strcpy(public_key_file, optarg);  // Update public key file path
                break;
            case 'v':
                verbose_mode = true;  // Enable verbose mode
                break;
            case 'h':
                // Print program usage and exit
                // clang-format off
                printf(
                    "SYNOPSIS\n"
                    "   Encrypts files using a public key.\n"
                    "\n"
                    "USAGE\n"
                    "   %s [-i:o:n:vh] [-i input_file] [-o output_file] [-n public_key_file]\n"
                    "\n"
                    "OPTIONS\n"
                    "   -i              Specifies the input file to encrypt (default: stdin).\n"
                    "   -o              Specifies the output file to encrypt (default: stdout).\n"
                    "   -n              Specifies the public key file (default: ss.pub).\n"
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

    FILE *public_key_fp = open_file(public_key_file, "r");
    mpz_t public_modulus_n;
    mpz_init(public_modulus_n);
    ss_read_pub(public_modulus_n, username, public_key_fp);

    // Verbose output: Display user and public modulus information
    if (verbose_mode) {
        printf("Username: %s\n", username);
        gmp_printf("Public modulus n (%u bits) = %Zd\n",
                   mpz_sizeinbase(public_modulus_n, 2), public_modulus_n);
    }

    // Encrypt the input file using the public key
    ss_encrypt_file(input_file, output_file, public_modulus_n);

    // Clear GMP variables and close files
    mpz_clear(public_modulus_n);
    fclose(public_key_fp);
    fclose(input_file);
    fclose(output_file);

    // Free dynamically allocated memory
    free(username);
    free(public_key_file);
}
