#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include "line_representation.h"
#include "safe_allocs.h"

// Inits an empty line structure
void line_init(Line *line, int number) {

    line->number = number;
    line->capacity = 0;
    line->str_size = 0;
    line->ll_size = 0;
    line->size = 0;
    line->dbl_size = 0;
    line->ull_size = 0;
}

// Frees all the memory that was alloc'd in the structure
void line_free(Line *line) {

    if (line->str_size != 0)
        free(line->str_array);
    if (line->ll_size != 0)
        free(line->ll_array);
    if (line->ull_size != 0)
        free(line->ull_array);
    if (line->dbl_size != 0)
        free(line->dbl_array);
    if (line->size != 0)
        free(line->similarLines);
}

// Next 4 functions add corresponding data type to line,
// while increasing the size and capacity of dynamic array if needed.
void add_str (Line *current, char *strValue, int *str_capacity) {

    if (*str_capacity == 0) {
        current->str_array = (char**) safe_malloc(INITIAL_CAPACITY * sizeof (char*));
        *str_capacity = INITIAL_CAPACITY;
    }
    else if (current->str_size == *str_capacity) {
        current->str_array = (char**) safe_realloc(current->str_array, sizeof (char*) * (*str_capacity *= 2));
    }
    current->str_array[current->str_size++] = strValue;
}

void add_ll (Line *current, long long llValue, int *ll_capacity) {

    if (*ll_capacity == 0) {
        current->ll_array = (long long *) safe_malloc(INITIAL_CAPACITY * sizeof (long long));
        *ll_capacity = INITIAL_CAPACITY;
    }
    else if (current->ll_size == *ll_capacity) {
        current->ll_array = (long long *) safe_realloc(current->ll_array, sizeof (long long) * (*ll_capacity *= 2));
    }

    current->ll_array[current->ll_size++] = llValue;
}

void add_ull (Line *current, unsigned long long ullValue, int *ull_capacity) {

    if (*ull_capacity == 0) {
        current->ull_array = (unsigned long long *) safe_malloc(INITIAL_CAPACITY * sizeof (unsigned long long));
        *ull_capacity = INITIAL_CAPACITY;
    }
    else if (current->ull_size == *ull_capacity) {
        current->ull_array = (unsigned long long *) safe_realloc(current->ull_array, sizeof (unsigned long long) * (*ull_capacity *= 2));
    }

    current->ull_array[current->ull_size++] = ullValue;
}

void add_dbl (Line *current, double dblValue, int *dbl_capacity) {

    if (*dbl_capacity == 0) {
        current->dbl_array = (double *) safe_malloc(INITIAL_CAPACITY * sizeof (double));
        *dbl_capacity = INITIAL_CAPACITY;
    }
    else if (current->dbl_size == *dbl_capacity) {
        current->dbl_array = (double *) safe_realloc(current->dbl_array, sizeof (double) * (*dbl_capacity *= 2));
    }

    current->dbl_array[current->dbl_size++] = dblValue;
}

int case_insensitive_cmp(const void *a, const void *b) {

    char **aTmp = (char**)a;
    char **bTmp = (char**)b;

    return strcasecmp(*aTmp, *bTmp);
}

int ll_cmp (const void *a, const void *b) {

    long long *ll1 = (long long *) a;
    long long *ll2 = (long long *) b;

    if (*ll1 > *ll2) {
        return 1;
    }
    else if (*ll1 < *ll2) {
        return -1;
    }
    else {
        return 0;
    }
}

int ull_cmp (const void *a, const void *b) {

    unsigned long long *ull1 = (unsigned long long *) a;
    unsigned long long *ull2 = (unsigned long long *) b;

    if (*ull1 > *ull2) {
        return 1;
    }
    else if (*ull1 < *ull2) {
        return -1;
    }
    else {
        return 0;
    }
}

int dbl_cmp (const void *a, const void *b) {

    double *d1 = (double *) a;
    double *d2 = (double *) b;

    if (*d1 > *d2) {
        return 1;
    }
    else if (*d1 < *d2) {
        return -1;
    }
    else {
        return 0;
    }
}

// Sorts all data that line holds
void sort_data_in_line (Line *current) {

    if (current->str_size != 0)
        qsort(current->str_array, current->str_size, sizeof (char *), case_insensitive_cmp);

    if (current->ll_size != 0)
        qsort(current->ll_array, current->ll_size, sizeof (long long), ll_cmp);

    if (current->ull_size != 0)
        qsort(current->ull_array, current->ull_size, sizeof (unsigned long long), ull_cmp);

    if (current->dbl_size != 0)
        qsort(current->dbl_array, current->dbl_size, sizeof (double), dbl_cmp);
}

// Checks if two lines are similar
// Since all data is sorted in each line, we can easily compare two lines
bool compareLines (Line *l1, Line *l2) {

    if(l1->ll_size != l2->ll_size || l1->ull_size != l2->ull_size
       || l1->str_size != l2->str_size || l1->dbl_size != l2->dbl_size) {

        return false;
    }

    for(int i = 0; i < l1->ll_size; i++) {
        if (l1->ll_array[i] != l2->ll_array[i])
            return false;
    }

    for(int i = 0; i < l1->ull_size; i++) {
        if (l1->ull_array[i] != l2->ull_array[i])
            return false;
    }

    for(int i = 0; i < l1->dbl_size; i++) {
        if (l1->dbl_array[i] != l2->dbl_array[i])
            return false;
    }

    for(int i = 0; i < l1->str_size; i++) {
        if (strcasecmp(l1->str_array[i], l2->str_array[i]) != 0)
            return false;
    }

    return true;
}

// adding the number of similar line to the line that is passed as an argument
void add_similar_line (Line *line, int number_of_similar) {

    if (line->capacity == 0) {
        line->similarLines = (int *) safe_malloc(INITIAL_CAPACITY * sizeof (int));
        line->capacity = INITIAL_CAPACITY;
    }
    if (line->size == line->capacity) {
        line->similarLines = (int *) safe_realloc(line->similarLines, sizeof (int) * (line->capacity *= 2));
    }
    line->similarLines[line->size++] = number_of_similar;
}

int min(int a, int b) {
    if (a <= b)
        return a;

    return b;
}

// Compares lines by the data they contain
// Data type order : string -> long long -> unsigned long long -> double
int line_cmp_by_data (const void *a, const void *b) {

    Line *l1 = (Line *) a;
    Line *l2 = (Line *) b;

    // strings comparison
    int m = min(l1->str_size, l2->str_size);
    for(int i = 0; i < m; i++) {
        int x = strcasecmp(l1->str_array[i], l2->str_array[i]);
        if (x != 0) {
            return x;
        }
    }
    if(l1->str_size > l2->str_size) {
        return 1;
    } else if (l1->str_size < l2->str_size) {
        return -1;
    }

    // long long comparison
    m = min (l1->ll_size, l2->ll_size);
    for(int i = 0; i < m; i++) {
        if(l1->ll_array[i] > l2->ll_array[i]) {
            return 1;
        }
        else if (l1->ll_array[i] < l2->ll_array[i]){
            return -1;
        }
    }
    if(l1->ll_size > l2->ll_size) {
        return 1;
    } else if (l1->ll_size < l2->ll_size) {
        return -1;
    }

    // unsigned long long comparison
    m = min (l1->ull_size, l2->ull_size);
    for(int i = 0; i < m; i++) {
        if(l1->ull_array[i] > l2->ull_array[i]) {
            return 1;
        }
        else if(l1->ull_array[i] < l2->ull_array[i]) {
            return -1;
        }

    }
    if(l1->ull_size > l2->ull_size) {
        return 1;
    } else if (l1->ull_size < l2->ull_size) {
        return -1;
    }

    // double comparison
    m = min (l1->dbl_size, l2->dbl_size);
    for(int i = 0; i < m; i++) {
        if(l1->dbl_array[i] > l2->dbl_array[i]) {
            return 1;
        } else if (l1->dbl_array[i] < l2->dbl_array[i]) {
            return -1;
        }
    }
    if(l1->dbl_size > l2->dbl_size) {
        return 1;
    } else if (l1->dbl_size < l2->dbl_size) {
        return -1;
    }

    // lines are similar
    return 0;
}

// Compares lines (pointers) by their number
int line_cmp_by_number (const void *a, const void *b) {
    Line **l1 = (Line **) a;
    Line **l2 = (Line **) b;

    return (*l1)->number - (*l2)->number;
}

// Having sorted (by data) array of lines, we look for representatives just by
// comparing i, and i + 1 neighbours. If neighbours are similar, then they are in the same "block".
// Otherwise, i + 1 element is a representative of a new "block"
void find_representatives(Line *lines, Line ***representatives, int lines_size, int *rep_size, int *rep_capacity) {

    bool has_representative = false;
    int rep;
    // We iterate through lines array and we look for representatives, that we add to representatives array.
    // Meanwhile, we update the 'similar_lines' arrays the representatives are holding
    for(int i = 0; i < lines_size; i++) {

        // New representative found
        if (!has_representative) {

            if(*rep_size == *rep_capacity) {
                *representatives = (Line **) safe_realloc(*representatives, sizeof (Line*) * (*rep_capacity *= 2));
            }
            add_similar_line(&lines[i], lines[i].number);
            (*representatives)[(*rep_size)++] = &lines[i];

            rep = i;
            has_representative = true;
        }

        // Compares neighbours
        // If these are similar lines, we update 'similar_lines' array of the representative.
        // Otherwise, we found another "block" with new representative.
        if (i < lines_size - 1 && compareLines(&lines[i], &lines[i + 1])) {
            add_similar_line(&lines[rep], lines[i + 1].number);
        }
        else {
            has_representative = false;
        }
    }
}