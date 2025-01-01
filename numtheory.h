#ifndef NUMTHEORY_H
#define NUMTHEORY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>

#include "randstate.h"

// Extern declaration for the global random state
extern gmp_randstate_t state;

/**
 * Computes the greatest common divisor (GCD) of two integers using the Euclidean algorithm.
 *
 * Args:
 *   gcd_result (mpz_t): The output variable to store the GCD (output).
 *   num1 (const mpz_t): The first input integer.
 *   num2 (const mpz_t): The second input integer.
 */
void gcd(mpz_t gcd_result, const mpz_t num1, const mpz_t num2);

/**
 * Computes the modular inverse of a modulo n using the Extended Euclidean Algorithm.
 * If the modular inverse does not exist, the output is set to 0.
 *
 * Args:
 *   inverse (mpz_t): The output variable to store the modular inverse (output).
 *   value (const mpz_t): The input value.
 *   modulus (const mpz_t): The modulus.
 */
void mod_inverse(mpz_t inverse, const mpz_t value, const mpz_t modulus);

/**
 * Computes (base^exponent) % modulus using modular exponentiation.
 * This implementation uses the right-to-left binary method for efficiency.
 *
 * Args:
 *   result (mpz_t): The output variable to store the result (output).
 *   base (const mpz_t): The base.
 *   exponent (const mpz_t): The exponent.
 *   modulus (const mpz_t): The modulus.
 */
void pow_mod(mpz_t result, const mpz_t base, const mpz_t exponent, const mpz_t modulus);

/**
 * Performs the Miller-Rabin primality test to determine if a number is prime.
 *
 * Args:
 *   number (const mpz_t): The number to test for primality.
 *   iterations (uint64_t): The number of iterations for the test (higher values increase accuracy).
 *
 * Returns:
 *   true if the number is likely prime, false otherwise.
 */
bool is_prime(const mpz_t number, uint64_t iterations);

/**
 * Generates a random prime number with a specified number of bits.
 *
 * Args:
 *   prime (mpz_t): The output variable to store the generated prime number (output).
 *   bit_size (uint64_t): The number of bits for the prime number.
 *   iterations (uint64_t): The number of iterations for the primality test.
 */
void make_prime(mpz_t prime, uint64_t bit_size, uint64_t iterations);

#endif // NUMTHEORY_H