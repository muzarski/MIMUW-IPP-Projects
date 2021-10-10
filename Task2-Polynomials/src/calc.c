/** @file
  * Plik calc.c. Część główna programu.

  @author Mikołaj Uzarski
  @date 2021
*/

/**
 * Makro potrzebne do poprawnego działania 'getline'.
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <errno.h>
#include "parser.h"

/**
 * Funkcja obsługująca standardowe wejście.
 */
void HandleInput() {
    Stack stack;
    StackInit(&stack);
    size_t buf_size = 4, line = 0;
    ssize_t characters;
    char *buffer = (char*) safeMalloc(sizeof(char) * INITIAL_CAPACITY);

    while ((characters = getline(&buffer, &buf_size, stdin)) != -1 && errno != ENOMEM) {
        size_t len = strlen(buffer);
        HandleLine(buffer, line, &stack, len, (size_t) characters);
        line++;
    }
    free(buffer);
    StackDestroy(&stack);
}

/**
 * Główna funkcja programu.
 * @return kod zakończenia programu
 */
int main() {
    HandleInput();
    return 0;
}