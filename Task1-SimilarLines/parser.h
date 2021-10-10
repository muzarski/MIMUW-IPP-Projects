#ifndef PARSER_H
#define PARSER_H

// Hexadecimal/octal representation of number cannot begin with +/-
bool checkPlusMinus_ll (char *word, char **ptr, long long *llValue);
bool checkPlusMinus_ull (char *word, char **ptr, long long *llValue, unsigned long long *ullValue);

// Checks if it's a number in hexadecimal representation
// Needed before parsing to 'double', since we don't treat
// floating-point numbers in hexadec as numbers
bool check_if_hex_dec (char* word);

// Checks if given word is a NaN value
bool check_if_nan(char *word);

// Parses string to corresponding data type
int parse (char *word, long long *ll_value, unsigned long long *ull_value, double *dbl_value);

void print_error(int number_of_line);

// Checks if line has any illegal characters or if line is a comment
bool check_illegal_characters_or_comment (char *buffer, int len, int characters, int numberOfLine);

#endif // PPARSER_H