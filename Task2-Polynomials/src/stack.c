/** @file
 * Implementacja interfejsu stack.h.
 * Zawiera dodatkowe funkcje pomocnicze.

  @author Mikołaj Uzarski
  @date 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "stack.h"

/**
 * Wypisuje na standardowe wyjście wartość logiczną
 * 1 - w przypadku true, 0 - w przypadku false.
 * @param[in] b : wartość logiczna
 */
static void PrintBool(bool b) {
    if (b) {
        printf("%d\n", 1);
    }
    else {
        printf("%d\n", 0);
    }
}

/**
 * Sprawdza czy stos @p stack zawiera @p needed wielomianów.
 * Jeśli nie, na wyjście diagnostyczne wypisywana jest informacja o błędzie.
 * @param[in] stack : stos
 * @param[in] number_of_line : numer wiersza
 * @param[in] needed : liczba potrzebnych wielomianów do wykonania operacji
 * @return wartośc logiczna - czy miał miejsce underflow
 */
static bool StackCheckUnderflow(Stack *stack, size_t number_of_line, size_t needed) {
    if (stack->size < needed || needed == ULONG_MAX) {
        fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", number_of_line + 1);
        return true;
    }
    return false;
}

/**
 * Ściąga i zwraca wielomian ze szczytu stosu @p stack.
 * @param[in] stack : stos
 * @return wielomian
 */
static Poly StackPop(Stack *stack) {
    assert(stack->size > 0);
    Poly res = stack->polys[stack->size - 1];
    stack->size--;
    return res;
}
/**
 * Wykonuje operację dwuargumentową na dwóch wierzchnich wielomianach stosu @p stack.
 * @param[in] stack : stos
 * @param[in] op : wskaźnik na funkcję, która jest operacją dwuargumentową na wielomianach
 * @param[in] line : numer wiersza
 */
static void StackTwoArgsOp(Stack *stack, Poly (*op)(const Poly *, const Poly *), size_t line) {
    if (!StackCheckUnderflow(stack, line, 2)) {
        assert(stack->size >= 2);
        Poly p = StackPop(stack);
        Poly q = StackPop(stack);
        Poly res = op(&p, &q);
        PolyDestroy(&p);
        PolyDestroy(&q);
        StackPut(stack, &res);
    }
}

/**
 * Zwraca wielomian znajdujący się na szczycie stosu @p stack, bez ściągania go ze stosu.
 * @param[in] stack : stos
 * @return wielomian
 */
static Poly StackPeek(Stack *stack) {
    assert(stack->size > 0);
    return stack->polys[stack->size - 1];
}

void StackInit(Stack *stack) {
    stack->size = 0;
    stack->capacity = INITIAL_CAPACITY;
    stack->polys = (Poly*) safeMalloc(sizeof(Poly) * stack->capacity);
}

void StackDestroy(Stack *stack) {
    for (size_t i = 0; i < stack->size; i++) {
        PolyDestroy(&stack->polys[i]);
    }
    free(stack->polys);
}

void StackPut(Stack *stack, const Poly *p) {
    if (stack->size == stack->capacity) {
        stack->polys = (Poly*) safeRealloc(stack->polys, sizeof(Poly) * (stack->capacity *= 2));
    }
    stack->polys[stack->size++] = *p;
}

void StackDelete(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly res = stack->polys[stack->size - 1];
        stack->size--;
        PolyDestroy(&res);
    }
}

void StackZero(Stack *stack) {
    Poly tmp = PolyZero();
    StackPut(stack, &tmp);
}

void StackIsCoeff(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly p = StackPeek(stack);
        PrintBool(PolyIsCoeff(&p));
    }
}

void StackIsZero(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly p = StackPeek(stack);
        PrintBool(PolyIsZero(&p));
    }
}

void StackPrint(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        PolyPrint(&stack->polys[stack->size - 1]);
        printf("\n");
    }
}

void StackClone(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly top = StackPeek(stack);
        Poly new = PolyClone(&top);
        StackPut(stack, &new);
    }
}

void StackAdd(Stack *stack, size_t line) {
    StackTwoArgsOp(stack, PolyAdd, line);
}

void StackMul(Stack *stack, size_t line) {
    StackTwoArgsOp(stack, PolyMul, line);
}

void StackSub(Stack *stack, size_t line) {
    StackTwoArgsOp(stack, PolySub, line);
}

void StackNeg(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly p = StackPop(stack);
        Poly neg = PolyNeg(&p);
        PolyDestroy(&p);
        StackPut(stack, &neg);
    }
}

void StackIsEq(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 2)) {
        assert(stack->size >= 2);
        Poly p = StackPeek(stack);
        Poly q = stack->polys[stack->size - 2];
        PrintBool(PolyIsEq(&p, &q));
    }
}

void StackDeg(Stack *stack, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly p = StackPeek(stack);
        printf("%d\n", PolyDeg(&p));
    }
}

void StackDegBy(Stack *stack, size_t var_idx, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly p = StackPeek(stack);
        printf("%d\n", PolyDegBy(&p, var_idx));
    }
}

void StackAt(Stack *stack, poly_coeff_t x, size_t line) {
    if (!StackCheckUnderflow(stack, line, 1)) {
        assert(stack->size > 0);
        Poly p = StackPop(stack);
        Poly res = PolyAt(&p, x);
        PolyDestroy(&p);
        StackPut(stack, &res);
    }
}

void StackCompose(Stack *stack, size_t k, size_t line) {
    if (k == ULONG_MAX) {
        fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", line + 1);
    }
    else if (!StackCheckUnderflow(stack, line, k + 1)) {
        assert(stack->size > k);
        Poly p = StackPop(stack);
        Poly *polys = (Poly*) safeMalloc(sizeof (Poly) * k);
        for (size_t i = 0; i < k; i++) {
            polys[i] = StackPop(stack);
        }
        Poly res = PolyCompose(&p, k, polys);
        StackPut(stack, &res);
        PolyDestroy(&p);
        for (size_t i = 0; i < k; i++) {
            PolyDestroy(&polys[i]);
        }
        free(polys);
    }
}