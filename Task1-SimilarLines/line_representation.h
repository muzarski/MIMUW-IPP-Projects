#ifndef  LINE_REPRESENTATION_H
#define LINE_REPRESENTATION_H

// representation of line
struct line_representation {

    int number;

    // dynamic array, holding the numbers of similar lines
    int *similarLines;
    int size;
    int capacity;

    // dynamic arrays, holding the data that appears in line
    char **str_array;
    long long *ll_array;
    unsigned long long *ull_array;
    double *dbl_array;

    // sizes of given arrays
    int str_size;
    int ll_size;
    int ull_size;
    int dbl_size;

};

typedef struct line_representation Line;

// Checks if two lines are similar
bool compareLines (Line *l1, Line *l2);

// Inits an empty line structure
void line_init(Line *line, int number);

// Frees all the memory that was alloc'd in the structure
void line_free(Line *line);

// Adds 'char*' value to line that is passed as an argument
void add_str (Line *current, char *strValue, int *str_capacity);

// Adds 'long long' value to line that is passed as an argument
void add_ll (Line *current, long long llValue, int *ll_capacity);

// Adds 'unsigned long long' value to line that is passed as an argument
void add_ull (Line *current, unsigned long long ullValue, int *ull_capacity);

// Adds 'double' value to line that is passed as an argument
void add_dbl (Line *current, double dblValue, int *dbl_capacity);

// Sorts all data that line contains
void sort_data_in_line (Line *current);

// adding the number of similar line to the line that is passed as an argument
void add_similar_line (Line *line, int number_of_similar);

int min(int a, int b);

// line comparator
// compares lines by its data (used in qsort)
int line_cmp_by_data (const void *a, const void *b);

// looks for representative of each "block" after lines are sorted by data
void find_representatives(Line *lines, Line ***representatives, int lines_size, int *rep_size, int *rep_capacity);

// Compares lines (pointers) by their number
int line_cmp_by_number (const void *a, const void *b);

// case insensitive comparator for strings (used in qsort)
int case_insensitive_cmp(const void *a, const void *b);

// long long comparator (used in qsort)
int ll_cmp (const void *a, const void *b);

// double comparator (used in qsort)
int dbl_cmp (const void *a, const void *b);

// unsigned long long comparator (used in qsort)
int ull_cmp (const void *a, const void *b);

#endif // LINE_REPRESENTATION_H