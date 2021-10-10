/** @file
 * Implementacja interfejsu parser.h.
 * Zawiera dodatkowe funkcje pomocnicze.

  @author Mikołaj Uzarski
  @date 2021
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <ctype.h>
#include "poly.h"
#include "stack.h"

/**
 * Dynamiczna struktura (vector) przechowująca jednomiany.
 */
typedef struct MonoVec {
    /** Tablica przechowująca jednomiany. */
    Mono *monos;
    /** Liczba jednomianów w tablicy. */
    size_t size;
    /** Rozmiar tablicy. */
    size_t capacity;
} MonoVec;

/**
 * Inicjuje pusty vector przechowujący jednomiany.
 * @param[in] v : vector
 */
static void VecInit(MonoVec *v) {
    v->monos = (Mono*) safeMalloc(sizeof (Mono) * 4);
    v->size = 0;
    v->capacity = 4;
}

/**
 * Dodaje do vectora @p v jednomian @p m.
 * @param[in] v : vector
 * @param[in] m : jednomian
 */
static void VecAdd(MonoVec *v, Mono *m) {
    if (v->size == v->capacity) {
        v->monos = (Mono*) safeRealloc(v->monos, sizeof (Mono) * (v->capacity *= 2));
    }
    v->monos[v->size++] = *m;
}

/**
 * Usuwa z pamięci vector.
 * @param[in] v : vector
 */
static void VecDestroy(MonoVec *v) {
    free(v->monos);
}

/**
 * Zwalnia z pamięci jednomiany znajdujące się w vectorze @p v.
 * Dodatkowo usuwa vector z pamięci. Funkcja uruchamiana w przypadku
 * znalezienia błędu w trakcie parsowania napisu na wielomian.
 * @param[in] v : vector
 */
static void VecError(MonoVec *v) {
    for (size_t i = 0; i < v->size; i++) {
        MonoDestroy(&v->monos[i]);
    }
    free(v->monos);
}

/**
 * Sprawdza czy dany znak powinien występować w wielomianie.
 * @param[in] c : znak
 * @return Czy znak jest poprawnym znakiem wielomianu?
 */
static bool CheckPolySign(char c) {
    int cast = (int) c;
    return (cast >= 40 && cast <= 41) || (cast >= 43 && cast <= 45) || (cast >= 48 && cast <= 57);
}

bool CheckIfEmptyOrComment(const char *buffer) {
    return buffer[0] == '#' || buffer[0] == '\n';
}

bool CheckIfLetter(char c) {
    int cast = (int) c;
    return (cast >= 65 && cast <= 90) || (cast >= 97 && cast <= 122);
}

bool CheckIfNumberOrMinus(char c) {
    int cast = (int) c;
    return (cast >= 48 && cast <= 57) || cast == 45;
}

bool CheckIfNumber(char c) {
    int cast = (int) c;
    return (cast >= 48 && cast <= 57);
}

bool CheckIfDegBy(const char *s, size_t len, size_t characters) {
    if (len < 6)
        return false;
    return strncmp(s, "DEG_BY", 6) == 0 && (isspace(s[6]) || (s[6] == '\0' && len == characters));
}

unsigned long long CheckDegByArg(const char *s, size_t len) {
    if (len < 7) {
        errno = ERANGE;
        return 0;
    }
    else {
        if (s[6] != ' ') {
            errno = ERANGE;
            return 0;
        }
        else {
            if (len < 8) {
                errno = ERANGE;
                return 0;
            }
            else {
                if (!CheckIfNumberOrMinus(s[7]) || s[7] == '-') {
                    errno = ERANGE;
                    return 0;
                }
                else {
                    size_t i = 7;
                    while(CheckIfNumberOrMinus(s[i + 1])) i++;
                    if (s[i + 1] != '\n' && s[i + 1] != '\0') {
                        errno = ERANGE;
                        return 0;
                    }
                    else {
                        i = 7;
                        char *ptr;
                        errno = 0;
                        unsigned long long arg = strtoull(s + i, &ptr, 10);
                        if (errno == ERANGE || (*ptr != '\n' && *ptr != '\0')){
                            return 0;
                        }
                        return arg;
                    }
                }
            }
        }
    }
}

bool CheckIfAt(const char *s, size_t len, size_t characters) {
    if (len < 2)
        return false;
    return (s[0] == 'A' && s[1] == 'T' && (isspace(s[2]) || (s[2] == '\0' && len == characters)));
}

long CheckAtArg(const char *s, size_t len) {
    if (len < 3) {
        errno = ERANGE;
        return 0;
    }
    else {
        if (s[2] != ' ') {
            errno = ERANGE;
            return 0;
        }
        else {
            if (len < 4) {
                errno = ERANGE;
                return 0;
            }
            else {
                if (!CheckIfNumberOrMinus(s[3]) || (s[3] == '-' && !CheckIfNumber(s[4]))) {
                    errno = ERANGE;
                    return 0;
                }
                else {
                    size_t i = 3;
                    while(CheckIfNumberOrMinus(s[i + 1]))
                        i++;
                    if (s[i + 1] != '\n' && s[i + 1] != '\0') {
                        errno = ERANGE;
                        return 0;
                    }
                    else {
                        i = 3;
                        char *ptr;
                        errno = 0;
                        long arg = strtol(s + i, &ptr, 10);
                        if (errno == ERANGE || (*ptr != '\n' && *ptr != '\0')){
                            return 0;
                        }
                        return arg;
                    }
                }
            }
        }
    }
}

bool CheckIfCOMPOSE(const char *s, size_t len, size_t characters) {
    if (len < 7)
        return false;
    return strncmp(s, "COMPOSE", 7) == 0 && (isspace(s[7]) || (s[7] == '\0' && len == characters));
}

unsigned long long CheckCOMPOSEArg(const char *s, size_t len) {
    if (len < 8) {
        errno = ERANGE;
        return 0;
    }
    else {
        if (s[7] != ' ') {
            errno = ERANGE;
            return 0;
        }
        else {
            if (len < 9) {
                errno = ERANGE;
                return 0;
            }
            else {
                if (!CheckIfNumberOrMinus(s[8]) || s[8] == '-') {
                    errno = ERANGE;
                    return 0;
                }
                else {
                    size_t i = 8;
                    while(CheckIfNumberOrMinus(s[i + 1])) i++;
                    if (s[i + 1] != '\n' && s[i + 1] != '\0') {
                        errno = ERANGE;
                        return 0;
                    }
                    else {
                        i = 8;
                        char *ptr;
                        errno = 0;
                        unsigned long long arg = strtoull(s + i, &ptr, 10);
                        if (errno == ERANGE || (*ptr != '\n' && *ptr != '\0')){
                            return 0;
                        }
                        return arg;
                    }
                }
            }
        }
    }
}

bool CheckIfZERO(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "ZERO\n") == 0) || strcmp(buffer, "ZERO") == 0;
}

bool CheckIfIS_COEFF(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "IS_COEFF\n") == 0) || strcmp(buffer, "IS_COEFF") == 0;
}

bool CheckIfIS_ZERO(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "IS_ZERO\n") == 0) || strcmp(buffer, "IS_ZERO") == 0;
}

bool CheckIfCLONE(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "CLONE\n") == 0) || strcmp(buffer, "CLONE") == 0;
}

bool CheckIfADD(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "ADD\n") == 0) || strcmp(buffer, "ADD") == 0;
}

bool CheckIfMUL(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "MUL\n") == 0) || strcmp(buffer, "MUL") == 0;
}

bool CheckIfNEG(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "NEG\n") == 0) || strcmp(buffer, "NEG") == 0;
}

bool CheckIfSUB(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "SUB\n") == 0) || strcmp(buffer, "SUB") == 0;
}

bool CheckIfIS_EQ(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "IS_EQ\n") == 0) || strcmp(buffer, "IS_EQ") == 0;
}

bool CheckIfDEG(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "DEG\n") == 0) || strcmp(buffer, "DEG") == 0;
}

bool CheckIfPRINT(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "PRINT\n") == 0) || strcmp(buffer, "PRINT") == 0;
}

bool CheckIfPOP(const char *buffer, char last) {
    return (last == '\n' && strcmp(buffer, "POP\n") == 0) || strcmp(buffer, "POP") == 0;
}

bool CheckIfCorrectPoly(const char *buffer) {
    if ((buffer[0] != '(' && !CheckIfNumberOrMinus(buffer[0])))
        return false;

    size_t i = 0;
    size_t op_bracket = 0;
    size_t cl_bracket = 0;
    size_t commas = 0;
    while (buffer[i] != '\n' && buffer[i] != '\0') {
        char cur = buffer[i];
        char next = buffer[i + 1];
        if (!CheckPolySign(cur))
            return false;
        if (cur == '(') {
            op_bracket++;
            if (!(CheckIfNumberOrMinus(next) || next == '-' || next == '('))
                return false;
        }
        if (cur == ')') {
            cl_bracket++;
            if (next != '+' && next != ',' && next != '\n' && next != '\0')
                return false;
        }
        if (cur == ',') {
            commas++;
            if (!CheckIfNumberOrMinus(next))
                return false;
        }
        if (CheckIfNumberOrMinus(cur)) {
            while (CheckIfNumberOrMinus(buffer[i + 1]))
                i++;

            if (!(buffer[i + 1] == ',' || buffer[i + 1] == ')'
                || buffer[i + 1] == '\n' || buffer[i + 1] == '\0'))
                return false;
        }
        if (cur == '+' && next != '(')
            return false;
        if (cur == '-' && !CheckIfNumberOrMinus(next))
            return false;
        i++;
    }

    if ((op_bracket != cl_bracket) || (op_bracket != commas))
        return false;

    return true;
}

void TraverseByNumber(char *buffer, size_t *cur) {
    while (CheckIfNumberOrMinus(buffer[*cur])) {
        (*cur)++;
    }
}

void PrintWrongPoly(size_t line) {
    fprintf(stderr, "ERROR %zu WRONG POLY\n", line + 1);
}

Poly PolyFromString(char *buffer, size_t *cur, size_t line) {
    if (CheckIfNumberOrMinus(buffer[*cur])) {
        errno = 0;
        long n = strtol(buffer + *cur, NULL, 10);
        if (errno == ERANGE) {
            PrintWrongPoly(line);
            return PolyZero();
        }
        TraverseByNumber(buffer, cur);
        Poly res = PolyFromCoeff(n);
        return res;
    }
    else if (buffer[*cur] == '(') {
        MonoVec v;
        VecInit(&v);
        bool flag = true;
        while (flag) {
            (*cur)++;
            Poly p = PolyFromString(buffer, cur, line);
            if (errno == ERANGE) {
                VecError(&v);
                return PolyZero();
            }
            assert(buffer[*cur] == ',');
            (*cur)++;
            assert(CheckIfNumberOrMinus(buffer[*cur]));

            long e = strtol(buffer + *cur, NULL, 10);
            if (errno == ERANGE || e < 0 || e > 2147483647) {
                PrintWrongPoly(line);
                errno = ERANGE;
                VecError(&v);
                return PolyZero();
            }
            TraverseByNumber(buffer, cur);

            if (e == 0 || !PolyIsZero(&p)) {
                Mono m = MonoFromPoly(&p, (poly_exp_t) e);
                VecAdd(&v, &m);
            }

            assert(buffer[*cur] == ')');
            (*cur)++;

            if (buffer[*cur] == '+') {
                (*cur)++;
                assert(buffer[*cur] == '(');
            }
            else {
                // koniec wielomianu
                flag = false;
            }
        }
        Poly res = PolyAddMonos(v.size, v.monos);
        VecDestroy(&v);
        return res;
    }
    else {
        PrintWrongPoly(line);
        errno = ERANGE;
        return PolyZero();
    }
}

void HandleLine(char *buffer, size_t line, Stack *s, size_t len, size_t characters) {
    if (CheckIfEmptyOrComment(buffer))
        return;

    if (CheckIfLetter(buffer[0])) {
        char last = buffer[len - 1];
        if (CheckIfDegBy(buffer, len, characters)) {
            errno = 0;
            unsigned long long arg = CheckDegByArg(buffer, len);
            if (errno == ERANGE || len < characters) {
                fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", line + 1);
            }
            else {
                StackDegBy(s, arg, line);
            }
        }
        else if (CheckIfAt(buffer, len, characters)) {
            errno = 0;
            long at_arg = CheckAtArg(buffer, len);
            if (errno == ERANGE || len < characters) {
                fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", line + 1);
            }
            else {
                StackAt(s, at_arg, line);
            }
        }
        else if (CheckIfCOMPOSE(buffer, len, characters)) {
            errno = 0;
            unsigned long long comp_arg = CheckCOMPOSEArg(buffer, len);
            if (errno == ERANGE || len < characters) {
                fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", line + 1);
            }
            else {
                StackCompose(s, comp_arg, line);
            }
        }
        else if (len < characters) {
            fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line + 1);
        }
        else if (CheckIfZERO(buffer, last)) {
            StackZero(s);
        }
        else if (CheckIfIS_COEFF(buffer, last)) {
            StackIsCoeff(s, line);
        }
        else if (CheckIfIS_ZERO(buffer, last)) {
            StackIsZero(s, line);
        }
        else if (CheckIfCLONE(buffer, last)) {
            StackClone(s, line);
        }
        else if (CheckIfADD(buffer, last)) {
            StackAdd(s, line);
        }
        else if (CheckIfMUL(buffer, last)) {
            StackMul(s, line);
        }
        else if (CheckIfNEG(buffer, last)) {
            StackNeg(s, line);
        }
        else if (CheckIfSUB(buffer, last)) {
            StackSub(s, line);
        }
        else if (CheckIfIS_EQ(buffer, last)) {
            StackIsEq(s, line);
        }
        else if (CheckIfDEG(buffer, last)) {
            StackDeg(s, line);
        }
        else if (CheckIfPRINT(buffer, last)) {
            StackPrint(s, line);
        }
        else if (CheckIfPOP(buffer, last)) {
            StackDelete(s, line);
        }
        else {
            fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line + 1);
        }
        return;
    }

    if (!CheckIfCorrectPoly(buffer) || len < characters) {
        fprintf(stderr, "ERROR %zu WRONG POLY\n", line + 1);
        return;
    }

    size_t cur = 0;
    Poly p = PolyFromString(buffer, &cur, line);
    if (errno != ERANGE)
        StackPut(s, &p);
}