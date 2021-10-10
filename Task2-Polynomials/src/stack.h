/** @file
 * Interfejs stosu trzymającego wielomiany.
 *
 * Niektóre funkcje posiadają za argument numer wiersza.
 * W przypadku niedostatecznej ilości wielomianów na stosie
 * do przeprowadzenia danej operacji, na wyjście diagnostyczne
 * wypisywana jest informacja o błędzie.

  @author Mikołaj Uzarski
  @date 2021
*/

#ifndef __STACK_H__
#define __STACK_H__

#include "poly.h"

/** Makro reprezentujące wejściowy rozmiar dla dynamicznych struktur tablicowych. */
#define INITIAL_CAPACITY 4

/**
 * Struktura przechowująca stos.
 */
typedef struct Stack {
    /** Tablica przechowująca wielomiany. */
    Poly *polys;
    /** Liczba wielomianów w tablicy. */
    size_t size;
    /** Rozmiar tablicy polys. */
    size_t capacity;
} Stack;

/**
 * Inicjuje pusty stos.
 * @param[in] stack : stos
 */
void StackInit(Stack *stack);

/**
 * Zwalnia stos z pamięci.
 * @param[in] stack : stos
 */
void StackDestroy(Stack *stack);

/**
 * Wstawia na stos @p stack wielomian @p p.
 * @param[in] stack : stos
 * @param[in] p : wielomian
 */
void StackPut(Stack *stack, const Poly *p);

/**
 * Usuwa wielomian ze szczytu stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackDelete(Stack *stack, size_t line);

/**
 * Wstawia na stos @p stack wielomian tożsamościowo równy zeru.
 * @param[in] stack : stos
 */
void StackZero(Stack *stack);

/**
 * Sprawdza czy wielomian znajdujący się na szczycie
 * stosu @p stack jest współczynnikiem.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackIsCoeff(Stack *stack, size_t line);

/**
 * Sprawdza czy wielomian znajdujący się na szczycie
 * stosu @p stack jest wielomianem tożsamościowo równym zero.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackIsZero(Stack *stack, size_t line);

/**
 * Wypisuje na standardowe wyjście wielomian znajdujący
 * się na szczycie stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackPrint(Stack *stack, size_t line);

/**
 * Klonuje wielomian znajdujący się na szczycie stosu @p stack
 * i wstawia go na stos.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackClone(Stack *stack, size_t line);

/**
 * Wstawia na stos @p stack wielomian będący sumą dwóch
 * wierzchnich wielomianów ze stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackAdd(Stack *stack, size_t line);

/**
 * Wstawia na stos @p stack wielomian będący iloczynem dwóch
 * wierzchnich wielomianów ze stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackMul(Stack *stack, size_t line);

/**
 * Wstawia na stos @p stack wielomian będący różnicą dwóch
 * wierzchnich wielomianów ze stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackSub(Stack *stack, size_t line);

/**
 * Neguje wielomian znajdujący się na szczycie stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackNeg(Stack *stack, size_t line);

/**
 * Wypisuje na standardowe wyjście informację o tym,
 * czy dwa wierzchnie wielomiany znajdujące się na
 * stosie @p stack są równe.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackIsEq(Stack *stack, size_t line);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu
 * znajdującego się na szczycie stosu @p stack.
 * @param[in] stack : stos
 * @param[in] line : numer wiersza
 */
void StackDeg(Stack *stack, size_t line);

/**
 * Wypisuje na standardowe wyjście stopień wielomianu znajdującego się
 * na szczycie stosu @p stack, ze względu na zadaną zmienną.
 * @param[in] stack : stos
 * @param[in] var_idx : indeks zmiennej
 * @param[in] line : numer wiersza
 */
void StackDegBy(Stack *stack, size_t var_idx, size_t line);

/**
 * Wstawia na stos @p stack wielomian, który jest wartością
 * wielomianu ze szczytu stosu w zadanym punkcie.
 * @param[in] stack : stos
 * @param[in] x : wartość argumentu
 * @param[in] line : numer wiersza
 */
void StackAt(Stack *stack, poly_coeff_t x, size_t line);

/**
 * Wykonuje operację złożenia wielomianu ze szczytu stosu @p stack
 * z @ k wielomianami znajdującymi się pod nim. Ściąga ze stosu te @p k+1 wielomianów,
 * a wynik operacji wstawia na stos.
 * @param[in] stack : stos
 * @param[in] k : ilość wielomianów
 * @param[in] line : numer wiersza
 */
void StackCompose(Stack *stack, size_t k, size_t line);

#endif // __STACK_H__