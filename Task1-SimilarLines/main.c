// IPP: Małe zadanie
// Autor: Mikołaj Uzarski

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>
#include <stdbool.h>
#include "line_representation.h"
#include "safe_allocs.h"
#include "parser.h"

// Adds given word to the line structure
void add_value_to_line (Line *current, char *word, int *str_capacity,
                        int *ll_capacity, int *ull_capacity, int *dbl_capacity) {

    long long ll_value;
    unsigned long long ull_value;
    double dbl_value;

    errno = 0;
    // parses current word to corresponding data type
    int which_type = parse(word, &ll_value, &ull_value, &dbl_value);

    switch (which_type) {
        // string
        case 0:
            add_str(current, word, str_capacity);
            break;

        // long long
        case 1:
            add_ll(current, ll_value, ll_capacity);
            break;

        // unsigned long long
        case 2:
            add_ull(current, ull_value, ull_capacity);
            break;

        // double
        case 3:
            add_dbl(current, dbl_value, dbl_capacity);
            break;
    }
}

void output_and_freeing (char **buffers, Line *lines, Line **representatives, int lines_size, int rep_size) {

    for(int i = 0; i < rep_size; i++) {
        for (int j = 0; j < representatives[i]->size; j++) {

            if (j == representatives[i]->size - 1) {
                if (fprintf(stdout, "%d", representatives[i]->similarLines[j] + 1) < 0)
                    exit(EXIT_FAILURE);
            }
            else {
                if (fprintf(stdout, "%d ", representatives[i]->similarLines[j] + 1) < 0)
                    exit(EXIT_FAILURE);
            }

        }
        printf("\n");
    }

    for(int i = 0; i < lines_size; i++) {
        line_free(&lines[i]);
        free(buffers[i]);
    }

    free(lines);
    free(buffers);
    free(representatives);
}

int main() {

    // Holds the content of non-empty lines.
    // Needed to free all the memory that 'getline' allocates.
    char **buffers = (char**) safe_malloc(INITIAL_CAPACITY * sizeof (char*));
    int buffers_size = 0, buffers_capacity = INITIAL_CAPACITY;

    // Holds the lines
    Line *lines = (Line*) safe_malloc(INITIAL_CAPACITY * sizeof (Line));
    int lines_size = 0, lines_capacity = INITIAL_CAPACITY;

    size_t buf_size = 4;
    int characters;
    char *buffer =  (char*) safe_malloc(buf_size * sizeof(char));

    // variables used in 'strtok' function
    char *delimiter = " \t\r\v\f\n";
    char *word;

    int count = 0;
    while ((characters = getline(&buffer, &buf_size, stdin)) != -1 && errno != ENOMEM) {

        errno = 0;

        int len = (int) strlen(buffer);
        bool is_error_or_comment = check_illegal_characters_or_comment (buffer, len, characters, count);

        word = strtok(buffer, delimiter);

        // Checks if line is empty or should be ignored
        // If any of these is true, we just skip this line
        // If there is any illegal character, 'is_error_or_comment' function prints it to stderr
        if(word == NULL || is_error_or_comment){
            count++;
            continue;
        }

        Line current;
        line_init(&current, count);

        // Variables that hold capacity of arrays that are in current line.
        // NOTE: we don't need to hold these values in 'Line' struct
        int str_capacity = 0, ll_capacity = 0, ull_capacity = 0, dbl_capacity = 0;

        while(word != NULL) {
            add_value_to_line(&current, word, &str_capacity, &ll_capacity, &ull_capacity, &dbl_capacity);

            word = strtok(NULL, delimiter);
        }

        sort_data_in_line (&current);

        // saves content of current line as a string
        if (buffers_size == buffers_capacity) {
            buffers = (char**) safe_realloc(buffers, sizeof (char*) * (buffers_capacity *= 2));
        }
        buffers[buffers_size++] = buffer;
        buffer = (char*) safe_malloc(buf_size * sizeof(char));

        // saves current line
        if (lines_size == lines_capacity) {
            lines = (Line*) safe_realloc(lines, sizeof (Line) * (lines_capacity *= 2));
        }
        lines[lines_size++] = current;

        count++;
    }

    if (errno == ENOMEM)
        exit(EXIT_FAILURE);

    free(buffer);

    // Sorts all lines by data.
    // After that, we have our lines divided in "blocks",
    // where all of the lines in every "block" are similar.
    // The first element (we'll call him representative) in every block is the one with the smallest number,
    // since our array is sorted by line numbers at the beginning.
    // There is no guarantee that representatives will appear in the right order.
    qsort(lines, lines_size, sizeof (Line), line_cmp_by_data);

    // We create an array that will hold a pointer to every representative of each "block"
    Line **representatives = (Line **) safe_malloc(INITIAL_CAPACITY * sizeof (Line*));
    int rep_size = 0, rep_capacity = INITIAL_CAPACITY;

    find_representatives(lines, &representatives, lines_size, &rep_size, &rep_capacity);

    // sorting the representatives of every "block" by line number
    qsort(representatives, rep_size, sizeof (Line *), line_cmp_by_number);

    output_and_freeing(buffers, lines, representatives, lines_size, rep_size);

    return 0;
}