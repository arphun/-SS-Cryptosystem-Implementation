#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "randstate.h"

// Global random state for generating random numbers
gmp_randstate_t state;

/**
 * Computes the greatest common divisor (GCD) of two integers using the
 * Euclidean algorithm.
 *
 * Args:
 *   gcd_result (mpz_t): The output variable to store the GCD (output).
 *   num1 (const mpz_t): The first input integer.
 *   num2 (const mpz_t): The second input integer.
 */
void gcd(mpz_t gcd_result, const mpz_t num1, const mpz_t num2) {
    mpz_t current_num1, current_num2, temp;
    mpz_init_set(current_num1, num1);
    mpz_init_set(current_num2, num2);
    mpz_init(temp);

    // Euclidean algorithm: Repeat until the second number (current_num2)
    // becomes 0.
    while (mpz_cmp_ui(current_num2, 0) != 0) {
        mpz_set(temp, current_num2);
        mpz_mod(current_num2, current_num1, current_num2);
        mpz_set(current_num1, temp);
    }
    mpz_set(gcd_result, current_num1);
    mpz_clears(current_num1, current_num2, temp, (mpz_ptr)NULL);
    return;
}

/**
 * Computes the modular inverse of a modulo n using the Extended Euclidean
 * Algorithm. If the modular inverse does not exist, the output is set to 0.
 *
 * Args:
 *   inverse (mpz_t): The output variable to store the modular inverse (output).
 *   value (const mpz_t): The input value.
 *   modulus (const mpz_t): The modulus.
 */
void mod_inverse(mpz_t inverse, const mpz_t value, const mpz_t modulus) {
    mpz_t cur_modulus, cur_value, coef_modulus, coef_value;
    // Init : value * 1 + modulus * 0 = gcd(value, modulus) = value (for now)
    mpz_init_set(cur_modulus, modulus);
    mpz_init_set(cur_value, value);
    mpz_init_set_ui(coef_modulus, 0);
    mpz_init_set_ui(coef_value, 1);

    mpz_t q, tmp, mul_result, sub_result;
    mpz_inits(q, tmp, mul_result, sub_result, (mpz_ptr)NULL);
    while (mpz_cmp_ui(cur_value, 0) != 0) {
        mpz_fdiv_q(q, cur_modulus, cur_value);
        // Euclidean algorithm
        mpz_set(tmp, cur_modulus);
        mpz_set(cur_modulus, cur_value);
        mpz_mul(mul_result, q, cur_value);
        mpz_sub(sub_result, tmp, mul_result);
        mpz_set(cur_value, sub_result);

        // Update the Bézout’s coeffient, proof :
        // https://hackmd.io/@Koios/rJ_lER719
        mpz_set(tmp, coef_modulus);
        mpz_set(coef_modulus, coef_value);
        mpz_mul(mul_result, q, coef_value);
        mpz_sub(sub_result, tmp, mul_result);
        mpz_set(coef_value, sub_result);
    }
    mpz_clears(q, tmp, mul_result, sub_result, (mpz_ptr)NULL);

    if (mpz_cmp_ui(cur_modulus, 1) > 0) {
        mpz_set_ui(inverse, 0);
    } else if (mpz_cmp_ui(coef_modulus, 0) < 0) {
        mpz_add(coef_modulus, coef_modulus, modulus);
        mpz_set(inverse, coef_modulus);
    } else {
        mpz_set(inverse, coef_modulus);
    }
    mpz_clears(cur_modulus, cur_value, coef_modulus, coef_value, (mpz_ptr)NULL);
    return;
}

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
void pow_mod(mpz_t result, const mpz_t base, const mpz_t exponent,
             const mpz_t modulus) {
    mpz_t current_result;
    mpz_init(current_result);
    // Sets current_result to 1 (multiplicative identity).
    mpz_set_ui(current_result, 1);

    mpz_t current_base, remainder, current_exponent;
    mpz_inits(current_base, remainder, current_exponent, (mpz_ptr)NULL);

    mpz_set(current_base, base);
    mpz_set(current_exponent, exponent);

    while (mpz_cmp_si(current_exponent, 0) > 0) {
        mpz_mod_ui(remainder, current_exponent, 2);
        if (mpz_cmp_si(remainder, 0) != 0) {  // odd exponent:
            mpz_mul(current_result, current_result, current_base);
            mpz_mod(current_result, current_result, modulus);
        }
        mpz_mul(current_base, current_base, current_base);
        mpz_mod(current_base, current_base, modulus);
        mpz_fdiv_q_ui(current_exponent, current_exponent, 2);
    }
    mpz_set(result, current_result);
    mpz_clears(current_base, remainder, current_exponent, current_result,
               (mpz_ptr)NULL);
}

/**
 * Performs the Miller-Rabin primality test to determine if a number is prime.
 *
 * Args:
 *   number (const mpz_t): The number to test for primality.
 *   iterations (uint64_t): The number of iterations for the test (higher values
 * increase accuracy).
 *
 * Returns:
 *   true if the number is likely prime, false otherwise.
 */
bool is_prime(const mpz_t number, uint64_t iterations) {
    // Handle small numbers and even cases
    if (mpz_cmp_ui(number, 1) == 0) {
        return false;
    } else if (mpz_cmp_ui(number, 2) == 0) {
        return true;
    } else if (mpz_even_p(number)) {
        return false;
    }
    // Initialize variables for (number - 1) = (2^s) * r
    uint64_t s = 1;  // Initial exponent for the power of 2.
    mpz_t r, number_minus_one;
    mpz_inits(r, number_minus_one, (mpz_ptr)NULL);
    mpz_sub_ui(number_minus_one, number, 1);
    while (true) {
        mpz_fdiv_q_2exp(r, number_minus_one, s);  // r = (number - 1) / 2^s.
        if (mpz_odd_p(r)) {                       // Stop when r is odd.
            break;
        }
        s++;
    }
    // Initialize variables for Miller-Rabin test
    mpz_t base, max_random, witness, two;
    mpz_inits(base, max_random, witness, two, (mpz_ptr)NULL);

    mpz_sub_ui(max_random, number, 4);  // max_random = number - 4.
    mpz_set_ui(two, 2);                 // Constant value 2.

    // Perform Miller-Rabin test for the given number of iterations
    for (uint64_t i = 0; i < iterations; i++) {
        mpz_urandomm(
            base, state,
            max_random);  // Generate random base in range [0, number - 4].
        mpz_add_ui(base, base, 2);  // Shift base to range [2, number - 2].
        pow_mod(witness, base, r, number);  // witness = (base^r) % number.

        // Check if witness is 1 or number - 1
        if (mpz_cmp_ui(witness, 1) == 0 ||
            mpz_cmp(witness, number_minus_one) == 0) {
            continue;  // This base passes the test; move to the next iteration.
        }
        // Perform s-1 iterations of squaring
        for (uint64_t j = 1;
             j <= s - 1 && mpz_cmp(witness, number_minus_one) != 0; j++) {
            pow_mod(witness, witness, two,
                    number);  // witness = (witness^2) % number.

            // If witness becomes 1, the number is composite
            if (mpz_cmp_ui(witness, 1) == 0) {
                mpz_clears(r, base, max_random, witness, number_minus_one, two,
                           (mpz_ptr)NULL);
                return false;
            }
        }

        // If witness does not become number - 1, the number is composite
        if (mpz_cmp(witness, number_minus_one) != 0) {
            mpz_clears(r, base, max_random, witness, number_minus_one, two,
                       (mpz_ptr)NULL);
            return false;
        }
    }
    mpz_clears(r, base, max_random, witness, number_minus_one, two,
               (mpz_ptr)NULL);
    return true;  // The number passes all tests and is likely prime.
}

/**
 * Generates a random prime number with a specified number of bits.
 *
 * Args:
 *   prime (mpz_t): The output variable to store the generated prime number
 * (output). bit_size (uint64_t): The number of bits for the prime number.
 *   iterations (uint64_t): The number of iterations for the primality test.
 */
void make_prime(mpz_t prime, uint64_t bit_size, uint64_t iterations) {
    // Variables to calculate the lower bound and initialize the prime
    mpz_t one, lower_bound;
    mpz_init_set_ui(one, 1);
    mpz_init(lower_bound);

    // Calculate the smallest number with the bit size: 2^(bit_size - 1)
    mpz_mul_2exp(lower_bound, one, bit_size - 1);
    mpz_set_ui(prime, 0);

    // Generate random numbers until a prime is found
    while (!is_prime(prime, iterations)) {
        mpz_urandomb(
            prime, state,
            bit_size);  // Generate a random number with 'bit_size' bits.
        mpz_add(prime, prime, lower_bound);  // Ensure the number is >=
                                             // lower_bound (bit_size bits).
    }

    mpz_clears(one, lower_bound, (mpz_ptr)NULL);
    return;
}