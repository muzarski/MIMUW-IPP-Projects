#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include "parser.h"
#include "safe_allocs.h"

bool checkPlusMinus_ll (char *word, char **ptr, long long *ll_value) {
    // If word begins with +/-, then we try to parse
    // it into a long long number in decimal representation.
    if(word[0] == '+' || word[0] == '-') {
        *ll_value = strtoll(word, ptr, 10);
    }
    else {
        *ll_value = strtoll(word, ptr, 0);

        // Handles parsing numbers in decimal representation with leading zeros
        if (**ptr != '\0' || errno == ERANGE) {
            errno = 0;
            *ll_value = strtoll(word, ptr, 10);
        }
    }

    return !(**ptr != '\0' || errno == ERANGE);
}

bool checkPlusMinus_ull (char *word, char **ptr, long long *ll_value, unsigned long long *ull_value) {
    // If word begins with +/-, then we try to parse
    // it into an unsigned long long number in decimal representation.
    if (word[0] == '+' || word[0] == '-') {
        *ull_value = strtoull(word, ptr, 10);
    }
    else {
        *ull_value = strtoull(word, ptr, 0);

        // Number is negative or has leading zeros
        if ((**ptr != '\0' || errno == ERANGE)) {
            errno = 0;
            *ll_value = strtoll(word, ptr, 10);

            // Number has leading zeros and is out of long long range
            if ((**ptr != '\0' || errno == ERANGE)) {
                errno = 0;
                *ull_value = strtoull(word, ptr, 10);
            }
        }
    }

    return !(**ptr != '\0' || errno == ERANGE);
}

// Checks if it's a number in hexadecimal representation
// Needed before parsing to 'double', since we don't treat
// floating-point numbers in hexadec as numbers
bool check_if_hex_dec (char* word) {

    int i = 0;

    // Checks if word has at least 2/3 characters
    while (word[i] != '\0' && i < 2) {
        i++;
    }

    // "0X" / "0x"
    if (i >= 1 && word[0] == '0' && (word[1] == 'x' || word[1] == 'X'))
        return true;

    // "+0x" / "-0x" / "+0X" / "-0X"
    if(i == 2 && (word[0] == '+' || word[0] == '-') && word[1] == '0'
       && (word [2] == 'x' || word[2] == 'X'))
        return true;

    return false;

}

bool check_if_nan(char *word) {
    return (strcasecmp(word, "nan") == 0 || strcasecmp(word, "+nan") == 0
            || strcasecmp(word, "-nan") == 0);
}

// Returns integer value:
// 0 - string
// 1 - long long
// 2 - unsigned long long
// 3 - double
int parse (char *word, long long *ll_value, unsigned long long *ull_value, double *dbl_value) {

    char *endPtr;

    if (!checkPlusMinus_ll(word, &endPtr, ll_value) && strcasecmp(word, "0x") != 0) {

        errno = 0;

        if (!checkPlusMinus_ull(word, &endPtr, ll_value, ull_value)) {

            errno = 0;
            *dbl_value = strtod(word, &endPtr);

            if (*endPtr != '\0' || errno == ERANGE || check_if_nan(word) || check_if_hex_dec(word)) {
                return 0;
            }
            else {
                // Parsing 'double' to long long/unsigned long long if it represents an integer
                if (*dbl_value >= LONG_MIN && *dbl_value <= LONG_MAX && (long long) *dbl_value == *dbl_value) {
                    *ll_value = (long long) *dbl_value;
                    return 1;
                }
                else if (*dbl_value == (unsigned long long) *dbl_value) {
                    *ull_value = (unsigned long long) *dbl_value;
                    return 2;
                }
                else {
                    return 3;
                }
            }
        }
        else {
            return 2;
        }
    }
    else {
        return 1;
    }
}

void print_error (int number_of_line) {
    if (fprintf(stderr, "ERROR %d\n", number_of_line + 1) < 0)
        exit(EXIT_FAILURE);
}

// Checks if line has any illegal characters
// Casts every 'char' in buffer to 'int' that represents its ascii value
bool check_illegal_characters_or_comment (char *buffer, int len, int characters, int number_of_line) {

    // line is a comment so we ignore it
    if (buffer[0] == '#')
        return true;

    // Checks if there appears '\0' character in buffer
    // 'characters' is the value returned by 'getline' function.
    // Length of string is less then 'characters' -> there appears '\0'.
    if (len < characters) {
        print_error(number_of_line);
        return true;
    }

    for (int i = 0; i < len; i++) {
        int cast = (int) buffer[i];

        if ((cast < 32 || (int) cast > 126) && (cast < 9 || cast > 13)) {
            print_error(number_of_line);
            return true;
        }
    }

    return false;
}