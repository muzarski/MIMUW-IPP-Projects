/** @file
 * Interfejs pliku odpowiadającego za zamianę wejścia na odpowiednie operacje
 * wykonywane na wielomianach i na stosie.
 *
 * Niektóre funkcje posiadają za argument numer wiersza.
 * W przypadku niedostatecznej ilości wielomianów na stosie
 * do przeprowadzenia danej operacji, na wyjście diagnostyczne
 * wypisywana jest informacja o błędzie.

  @author Mikołaj Uzarski
  @date 2021
*/

#ifndef __PARSER_H__
#define __PARSER_H__

#include "poly.h"
#include "stack.h"
#include <stdio.h>

/**
 * Sprawdza czy wiersz @p buffer jest pusty, lub czy należy go zignorować.
 * @param[in] buffer : wiersz
 * @return Czy ignorujemy wiersz?
 */
bool CheckIfEmptyOrComment(const char *buffer);

/**
 * Sprawdza czy znak @p c jest małą literą.
 * @param[in] c : znak
 * @return Czy znak jest małą literą?
 */
bool CheckIfLetter(char c);

/**
 * Sprawdza czy znak @p c jest liczbą lub znakiem '-'.
 * @param[in] c : znak
 * @return Czy znak jest liczbą lub minusem?
 */
bool CheckIfNumberOrMinus(char c);

/**
 * Sprawdza czy znak @p c jest liczbą.
 * @param[in] c : znak
 * @return Czy znak jest liczbą?
 */
bool CheckIfNumber(char c);

/**
 * Sprawdza czy dany napis zaczyna się od "DEG_BY".
 * @param[in] s : napis
 * @param[in] len : długość napisu (do pierwszego '\0')
 * @param[in] characters : długosć napisu
 * @return Czy to komenda "DEG_BY"?
 */
bool CheckIfDegBy(const char *s, size_t len, size_t characters);

/**
 * Sprawdza czy argument komendy "DEG_BY" jest poprawny.
 * Jeśli tak, to go zwraca, jeśli nie to wypisuje informację o błędzie na wyjście diagnostyczne.
 * @param[in] s : napis
 * @param[in] len : długość napisu
 * @return argument komendy "DEG_BY"
 */
unsigned long long CheckDegByArg(const char *s, size_t len);

/**
 * Sprawdza czy dany napis zaczyna się od "AT".
 * @param[in] s : napis
 * @param[in] len : długość napisu (do pierwszego '\0')
 * @param[in] characters : długosć napisu
 * @return Czy to komenda "AT"?
 */
bool CheckIfAt(const char *s, size_t len, size_t characters);

/**
 * Sprawdza czy argument komendy "AT" jest poprawny.
 * Jeśli tak, to go zwraca, jeśli nie to wypisuje informację o błędzie na wyjście diagnostyczne.
 * @param[in] s : napis
 * @param[in] len : długość napisu
 * @return argument komendy "AT"
 */
long CheckAtArg(const char *s, size_t len);


/**
 * Sprawdza czy dany napis zaczyna się od "COMPOSE".
 * @param[in] s : napis
 * @param[in] len : długość napisu (do pierwszego '\0')
 * @param[in] characters : długosć napisu
 * @return Czy to komenda "COMPOSE"?
 */
bool CheckIfCOMPOSE(const char *s, size_t len, size_t characters);

/**
 * Sprawdza czy argument komendy "COMPOSE" jest poprawny.
 * Jeśli tak, to go zwraca, jeśli nie to wypisuje informację o błędzie na wyjście diagnostyczne.
 * @param[in] s : napis
 * @param[in] len : długość napisu
 * @return argument komendy "COMPOSE"
 */
unsigned long long CheckCOMPOSEArg(const char *s, size_t len);

/**
 * Sprawdza czy wiersz posiada komendę "ZERO".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "ZERO"?
 */
bool CheckIfZERO(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "IS_COEFF".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "IS_COEFF"?
 */
bool CheckIfIS_COEFF(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "IS_ZERO".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "IS_ZERO"?
 */
bool CheckIfIS_ZERO(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "CLONE".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "CLONE"?
 */
bool CheckIfCLONE(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "ADD".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "ADD"?
 */
bool CheckIfADD(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "MUL".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "MUL"?
 */
bool CheckIfMUL(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "NEG".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "NEG"?
 */
bool CheckIfNEG(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "SUB".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "SUB"?
 */
bool CheckIfSUB(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "IS_EQ".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "IS_EQ"?
 */
bool CheckIfIS_EQ(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "DEG".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "DEG"?
 */
bool CheckIfDEG(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "PRINT".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "PRINT"?
 */
bool CheckIfPRINT(const char *buffer, char last);

/**
 * Sprawdza czy wiersz posiada komendę "POP".
 * @param[in] buffer : wiersz
 * @param[in] last : ostatni znak wiersza
 * @return Czy to komenda "POP"?
 */
bool CheckIfPOP(const char *buffer, char last);

/**
 * Sprawdza poprawność wielomianu ze standardowego wejścia.
 * @param[in] buffer : wiersz
 * @return Czy wielomian jest poprawny?
 */
bool CheckIfCorrectPoly(const char *buffer);

/**
 * Porusza do przodu indeks @p cur do czasu aż buffer[*cur] nie jest liczbą.
 * @param[in] buffer : wiersz
 * @param[in] cur : wskaźnik na aktualny indeks
 */
void TraverseByNumber(char *buffer, size_t *cur);

/**
 * Wypisuje na wyjście diagnostyczne informację o niepoprawnym wielomianie w wierszu @p line.
 * @param[in] line : numer wiersza
 */
void PrintWrongPoly(size_t line);

/**
 * Parsuje rekurencyjnie napis na wielomian. Sprawdza poprawność zakresu liczb występujących w wielomianie.
 * Jeśli któraś liczba jest spoza zakresu, na wyjście diagnostyczne wypisywana jest informacja
 * o wprowdzeniu błędnęgo wielomianu w wierszu @p line.
 * @param[in] buffer : wiersz
 * @param[in] cur : wskaźnik na aktualny indeks w tablicy znaków
 * @param[in] line : numer wiersza
 */
Poly PolyFromString(char *buffer, size_t *cur, size_t line);

/**
 * Główna funkcja obsługująca dany wiersz.
 * @param[in] buffer : wiersz
 * @param[in] line : numer wiersza
 * @param[in] s : stos
 * @param[in] len : długość wiersza do (pierwszego znaku '\0')
 * @param[in] characters : długość wiersza
 */
void HandleLine(char *buffer, size_t line, Stack *s, size_t len, size_t characters);

#endif // __PARSER_H__