/** @file
 * Implementacja interfejsu poly.h.
 * Zawiera dodatkowe funkcje pomocnicze.
 *
 * Niezmienniki w implementacji wielomianów: \n
 *  1) Tablica jednomianów wielomianu posortowana jest malejąco po wykładnikach. \n
 *  2) Jeśli wielomian jest współczynnikiem (coeff), to trzymany jest jako (Poly) {.arr = NULL, .coeff = coeff}.
 *  Pozwala to na ujednoznacznienie reprezentacji takich wielomianów.

  @author Mikołaj Uzarski
  @date 2021
*/

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include "poly.h"
#include <stdio.h>

/**
 * Asercje czy dany wielomian jest poprawnie reprezentowany.
 * Sprawdza między innymi niezmiennik 2).
 * @param[in] p : wielomian
 * @return bool : sprawdzenie poprawności reprezentacji wielomianu @p p
 */
static bool PolyCheckIfCorrect(const Poly *p) {
    bool result = p != NULL;
    if (!PolyIsCoeff(p)) {
        result = result && !(PolyIsCoeff(&p->arr[0].p) && MonoGetExp(&p->arr[0]) == 0);
    }
    return result;
}

/**
 * Komparator jednomianów (porządek malejący po wykładniku)
 * @param[in] a : wskaźnik na pierwszy jednomian
 * @param[in] b : wskaźnik na drugi jednomian
 * @return liczba całkowita : wykładnik b - wykładnik a
 */
static int monoCmp (const void *a, const void *b) {

    Mono *m1 = (Mono*) a;
    Mono *m2 = (Mono*) b;

    if (m2->exp > m1->exp) {
        return 1;
    }
    else if (m2->exp < m1->exp) {
        return -1;
    }
    else {
        return 0;
    }
}

/**
 * Zwraca większą z dwóch liczb całkowitych.
 * @param[in] a : liczba całkowita
 * @param[in] b : liczba całkowita
 * @return liczba całkowita : @f$\max(a,b)@f$
 */
static int max (int a, int b) {
    if (a >= b)
        return a;

    return b;
}

/**
 * Zwraca @p x podniesione do potęgi @p exp.
 * @param[in] x : liczba całkowita
 * @param[in] exp : liczba całkowita, wykładnik
 * @return liczba całkowita @f$ x^{exp} @f$
 */
static poly_coeff_t power (poly_coeff_t x, poly_exp_t exp) {

    poly_coeff_t result = 1;

    while (exp > 0) {
        if (exp % 2 == 0) {
            x *= x;
            exp /= 2;
        }
        exp--;
        result *= x;
    }

    return result;
}

/**
 * Zwraca wielomian @p p przemnożony przez skalar @p s.
 * @param[in] p : wielomian
 * @param[in] s : liczba całkowita - skalar
 * @return wielomian @f$sp@f$
 */
static Poly PolyMulByScalar(const Poly *p, poly_coeff_t s) {
    assert(PolyCheckIfCorrect(p));

    if (s == 0) {
        return PolyZero();
    }

    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(s * p->coeff);
    }

    Mono *monos_tmp = (Mono*) safeMalloc(sizeof (Mono) * (p->size + 1));

    size_t size = 0;
    for (size_t i = 0; i < p->size; i++) {
        Poly poly_tmp = PolyMulByScalar(&p->arr[i].p, s);
        if (!PolyIsZero(&poly_tmp)) {
            monos_tmp[size] = (Mono) {.exp = p->arr[i].exp, .p = poly_tmp};
            size++;
        }
    }

    if (size == 0) {
        free(monos_tmp);
        return PolyZero();
    }

    // Zachowywanie niezmiennika 2).
    if (size == 1 && PolyIsCoeff(&monos_tmp[0].p) && MonoGetExp(&monos_tmp[0]) == 0) {
        Poly coeff = PolyFromCoeff(monos_tmp[0].p.coeff);
        free(monos_tmp);
        return coeff;
    }

    return (Poly) {.size = size, .arr = monos_tmp};
}

/**
 * Zwraca jednomian @p m przemnożony przez skalar @p s.
 * @param[in] m : jednomian
 * @param[in] s : liczba całkowita - skalar
 * @return jednomian @f$sm@f$
 */
static Mono MonoMulByScalar(const Mono *m, poly_coeff_t s) {
    return (Mono) {.exp = m->exp, .p = PolyMulByScalar(&m->p, s)};
}

/**
 * Dodaje liczbę całkowitą @p coeff do wielomianu @p p, niebędącego liczbą całkowitą.
 * Modyfikuje wejściowy wielomian.
 * @param[in] p : wielomian
 * @param[in] coeff : liczba całkowita
 */
static void PolyAddCoeff (Poly *p, poly_coeff_t coeff) {
    assert(PolyCheckIfCorrect(p));
    assert(!PolyIsCoeff(p));

    if (coeff == 0) {
        return;
    }

    if (MonoGetExp(&p->arr[p->size - 1]) == 0) {
        if (PolyIsCoeff(&p->arr[p->size - 1].p)) {
            // Wyraz wolny zeruje się.
            if (p->arr[p->size - 1].p.coeff + coeff == 0) {
                p->size--;
            }
                // Dodajemy współczynnik do wielomianu.
            else {
                p->arr[p->size - 1].p.coeff += coeff;
            }
        }
        else {
            PolyAddCoeff(&p->arr[p->size - 1].p, coeff);
        }
    }
        // Wielomian nie posiada jednomianu o wykładniku 0.
    else {
        p->arr[p->size++] = (Mono) {.exp = 0, .p = PolyFromCoeff(coeff)};
    }
}

/**
 * Funkcja pomocnicza dla PolyAddMulByScalars.
 * Zakłada, że jeden z wielomianów jest współczynnikiem (coeff)
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @param[in] sp : liczba całkowita, skalar @f$ s_p @f$
 * @param[in] sq : liczba całkowita, skalar @f$ s_q @f$
 * @return wielomian @f$ s_pp + s_qq @f$
 */
static Poly PolyAddMulByScalarsHandleCoeff(const Poly *p, const Poly *q, poly_coeff_t sp, poly_coeff_t sq) {
    assert(PolyCheckIfCorrect(p));
    assert(PolyCheckIfCorrect(q));

    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyFromCoeff((sp * p->coeff) + (sq * q->coeff));

    // dodawanie coeff i wielomianu z arr != NULL
    Poly result;

    if (PolyIsCoeff(q)) {
        result = PolyMulByScalar(p, sp);
        if (PolyIsCoeff(&result))
            return PolyFromCoeff(result.coeff + sq * q->coeff);
        PolyAddCoeff(&result, sq * q->coeff);
    }

    if (PolyIsCoeff(p)) {
        result = PolyMulByScalar(q, sq);
        if (PolyIsCoeff(&result))
            return PolyFromCoeff(result.coeff + sp * p->coeff);
        PolyAddCoeff(&result, sp * p->coeff);
    }

    return result;
}

/**
 * Zwraca sumę wielomianów @p p i @p q przemnożonych
 * odpowiednio przez skalary @f$ s_p @f$ i @f$ s_q @f$.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @param[in] sp : liczba całkowita, skalar @f$ s_p @f$
 * @param[in] sq : liczba całkowita, skalar @f$ s_q @f$
 * @return wielomian @f$ s_pp + s_qq @f$
 */
static Poly PolyAddMulByScalars(const Poly *p, const Poly *q, poly_coeff_t sp, poly_coeff_t sq) {
    assert(PolyCheckIfCorrect(p));
    assert(PolyCheckIfCorrect(q));

    // ktorys to wyraz wolny
    if (PolyIsCoeff(p) || PolyIsCoeff(q))
        return PolyAddMulByScalarsHandleCoeff(p, q, sp, sq);

    size_t i = 0, ip = 0, iq = 0;
    Mono *monos_tmp = (Mono*) safeMalloc(sizeof(Mono) * (p->size + q->size + 1));

    while(ip < p->size && iq < q->size) {
        if (p->arr[ip].exp == q->arr[iq].exp) {

            // Wykładniki takie same, dodajemy rekurencyjnie współczynniki jednomianów.
            Mono tmp = (Mono) {.exp = p->arr[ip].exp,
                    .p = PolyAddMulByScalars(&p->arr[ip].p, &q->arr[iq].p, sp, sq)};

            if (!PolyIsZero(&tmp.p))
                monos_tmp[i++] = tmp;
            ip++;
            iq++;
        }
        else if (p->arr[ip].exp > q->arr[iq].exp) {
            Mono tmp = MonoMulByScalar(&p->arr[ip], sp);
            if (!PolyIsZero(&tmp.p))
                monos_tmp[i++] = tmp;
            ip++;
        }
        else {
            Mono tmp = MonoMulByScalar(&q->arr[iq], sq);
            if (!PolyIsZero(&tmp.p))
                monos_tmp[i++] = tmp;
            iq++;
        }
    }

    if (ip == p->size) {
        while (iq < q->size) {
            Mono tmp = MonoMulByScalar(&q->arr[iq], sq);
            if (!PolyIsZero(&tmp.p))
                monos_tmp[i++] = tmp;
            iq++;
        }
    }
    else if (iq == q->size) {
        while (ip < p->size) {
            Mono tmp = MonoMulByScalar(&p->arr[ip], sp);
            if (!PolyIsZero(&tmp.p))
                monos_tmp[i++] = tmp;
            ip++;
        }
    }

    if (i == 0) {
        free(monos_tmp);
        return PolyZero();
    }

    // Zachowywanie niezmiennika 2).
    if (i == 1 && PolyIsCoeff(&monos_tmp[0].p) && MonoGetExp(&monos_tmp[0]) == 0) {
        Poly result = PolyFromCoeff(monos_tmp[0].p.coeff);
        free(monos_tmp);
        return result;
    }

    return (Poly) {.size = i, .arr = monos_tmp};
}

/**
 * Sprawdza równość dwóch jednomianów.
 * @param[in] m : jednomian @f$p@f$
 * @param[in] n : jednomian @f$q@f$
 * @return @f$m = n@f$
 */
static bool MonoIsEq (const Mono *m, const Mono *n) {
    return m->exp == n->exp && PolyIsEq(&m->p, &n->p);
}

bool PolyIsEq (const Poly *p, const Poly *q) {
    assert(PolyCheckIfCorrect(p));
    assert(PolyCheckIfCorrect(q));

    if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
            return p->coeff == q->coeff;
        }
        else {
            return false;
        }
    }

    if (p->size != q->size)
        return false;

    for(size_t i = 0; i < p->size; i++) {
        if (!MonoIsEq(&p->arr[i], &q->arr[i]))
            return false;
    }

    return true;
}

void* safeMalloc (size_t size) {
    void *p = malloc (size);

    if (size > 0 && p == NULL) {
        exit(EXIT_FAILURE);
    }

    return p;
}

void* safeRealloc (void *ptr, size_t size) {
    void *p = realloc (ptr, size);

    if(size > 0 && p == NULL)
        exit(EXIT_FAILURE);

    return p;
}

poly_exp_t PolyDeg(const Poly *p) {
    assert(PolyCheckIfCorrect(p));

    if (PolyIsZero(p)) {
        return -1;
    }

    if(PolyIsCoeff(p)) {
        return 0;
    }

    poly_exp_t deg_max = 0;

    for (size_t i = 0; i < p->size; i++) {
        poly_exp_t tmp = MonoGetExp(&p->arr[i]) + PolyDeg(&(p->arr[i].p));
        deg_max = max(deg_max, tmp);
    }

    return deg_max;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    assert(PolyCheckIfCorrect(p));
    assert(PolyCheckIfCorrect(q));
    return PolyAddMulByScalars(p, q, 1, 1);
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {

    if (count == 0) {
        return PolyZero();
    }

    // Przepisywanie wejściowej tablicy.
    Mono *monos_tmp = (Mono*) safeMalloc (sizeof(Mono) * (count + 1));
    for (size_t i = 0; i < count; i++) {
        monos_tmp[i] = monos[i];
    }
    // Placeholder, dzięki temu nie martwimy się o wyjście poza zakres tablicy.
    monos_tmp[count] = (Mono) {.exp = INT_MIN};
    // Sortowanie tablicy jednomianów malejąco po wykładniku.
    qsort(monos_tmp, count + 1, sizeof(Mono), monoCmp);

    size_t size = count;
    for (size_t i = 0; i < count; i++) {
        assert (monos_tmp[i].exp >= monos_tmp[i + 1].exp);
        Mono cur = monos_tmp[i];
        Mono next = monos_tmp[i + 1];

        // Jeśli aktualny wielomian jest zerem, to oznaczamy go flagą (exp = INT_MIN).
        // Dzięki temu, przy następnym sortowaniu, znajdzie się on
        // za poprawnymi jednomianami.
        if (PolyIsZero(&cur.p)) {
            monos_tmp[i].exp = INT_MIN;
            size--;
        }
            // Dodajemy sąsiadów o tym samym wykładniku.
        else if (MonoGetExp(&cur) == MonoGetExp(&next)) {
            Mono tmp = next;
            monos_tmp[i + 1].p = PolyAdd(&cur.p, &tmp.p);
            monos_tmp[i].exp = INT_MIN;

            PolyDestroy(&cur.p);
            PolyDestroy(&tmp.p);

            size--;
        }
    }

    if (size == 0) {
        free(monos_tmp);
        return PolyZero();
    }

    // Drugie sortowanie, wyzerowane jednomiany (exp = -1) trafiają na koniec tablicy.
    qsort(monos_tmp, count + 1, sizeof(Mono), monoCmp);

    // Zachowanie niezmiennika 2).
    if (size == 1 && PolyIsCoeff(&monos_tmp[0].p) && MonoGetExp(&monos_tmp[0]) == 0) {
        Poly result = PolyFromCoeff(monos_tmp[0].p.coeff);
        free(monos_tmp);
        return result;
    }

    return (Poly) {.size = size, .arr = monos_tmp};
}

void PolyDestroy(Poly *p) {
    PolyCheckIfCorrect(p);

    if (PolyIsCoeff(p)){
        return;
    }

    for(size_t i = 0; i < p->size; i++) {
        MonoDestroy(&p->arr[i]);
    }

    free(p->arr);
}

Poly PolyClone(const Poly *p) {
    assert(PolyCheckIfCorrect(p));
    return PolyMulByScalar(p, 1);
}

Poly PolyNeg(const Poly *p) {
    assert(PolyCheckIfCorrect(p));
    return PolyMulByScalar(p, -1);
}

Poly PolySub(const Poly *p, const Poly *q) {
    assert(PolyCheckIfCorrect(p));
    assert(PolyCheckIfCorrect(q));
    return PolyAddMulByScalars(p, q, 1, -1);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    assert(PolyCheckIfCorrect(p));

    if (PolyIsZero(p)) {
        return -1;
    }

    if (PolyIsCoeff(p)) {
        return 0;
    }

    poly_exp_t result = -1;

    if (var_idx == 0) {
        for (size_t i = 0; i < p->size; i++) {
            result = max(result, p->arr[i].exp);
        }
    }
    else {
        for (size_t i = 0; i < p->size; i++) {
            result = max(result, PolyDegBy(&p->arr[i].p, var_idx - 1));
        }
    }
    return result;
}

Poly PolyAt (const Poly *p, poly_coeff_t x) {
    assert(PolyCheckIfCorrect(p));

    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }

    // Wielomian wynikowy, który będziemy nadpisywać
    // dodając do niego kolejne wielomiany przemnożone
    // przez odpowiedni skalar.
    Poly result = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        poly_coeff_t s = power(x, MonoGetExp(&p->arr[i]));
        if (s == 0) {
            continue;
        }
        Poly tmp = PolyAddMulByScalars(&result, &p->arr[i].p, 1, s);

        PolyDestroy(&result);
        result = tmp;
    }

    return result;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    assert(PolyCheckIfCorrect(p));
    assert(PolyCheckIfCorrect(q));

    if (PolyIsZero(p) || PolyIsZero(q)) {
        return PolyZero();
    }

    if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
            return PolyFromCoeff(p->coeff * q->coeff);
        }

        if (PolyIsCoeff(q)) {
            return PolyMulByScalar(p, q->coeff);
        }

        if (PolyIsCoeff(p)) {
            return PolyMulByScalar(q, p->coeff);
        }
    }

    Mono *monos = (Mono*) safeMalloc (sizeof (Mono) * p->size * q->size);
    size_t k = 0;
    // Wymnażanie dwóch "nawiasów", wyniki pakujemy do tablicy.
    for (size_t i = 0; i < p->size; i++) {
        for (size_t j = 0; j < q->size; j++) {
            monos[k] = (Mono) {.exp = MonoGetExp(&p->arr[i]) + MonoGetExp(&q->arr[j]),
                    .p = PolyMul(&p->arr[i].p, &q->arr[j].p)};
            k++;
        }
    }

    // Wynikowy wielomian będzie sumą jednomianów otrzymanych po
    // wymnożeniu "każdego z każdym".
    Poly result = PolyAddMonos(k, monos);
    free(monos);
    return result;
}

void PolyPrint(const Poly *p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
    }
    else {
        size_t i = p->size - 1;
        bool flag = true;
        while (flag) {
            if (i != p->size - 1) printf("+");
            printf("(");
            PolyPrint(&p->arr[i].p);
            printf(",%d)", p->arr[i].exp);
            if(!i) flag = false;
            i--;
        }
    }
}

Poly PolyOwnMonos(size_t count, Mono *monos) {

    if (count == 0 || monos == NULL)
        return PolyZero();

    // Sortowanie tablicy jednomianów malejąco po wykładniku.
    qsort(monos, count, sizeof(Mono), monoCmp);

    size_t size = count;
    for (size_t i = 0; i < count; i++) {
        Mono cur = monos[i];

        // Jeśli aktualny wielomian jest zerem, to oznaczamy go flagą (exp = -1).
        // Dzięki temu, przy następnym sortowaniu, znajdzie się on
        // za poprawnymi jednomianami.
        if (PolyIsZero(&cur.p)) {
            monos[i].exp = -1;
            size--;
        }
        else if (i < count - 1) {
            Mono next = monos[i + 1];
            // Dodajemy sąsiadów o tym samym wykładniku.
            if (MonoGetExp(&cur) == MonoGetExp(&next)) {
                Mono tmp = next;
                monos[i + 1].p = PolyAdd(&cur.p, &tmp.p);
                monos[i].exp = -1;

                PolyDestroy(&cur.p);
                PolyDestroy(&tmp.p);

                size--;
            }
        }
    }

    if (size == 0) {
        free(monos);
        return PolyZero();
    }

    // Drugie sortowanie, wyzerowane jednomiany (exp = -1) trafiają na koniec tablicy.
    qsort(monos, count, sizeof(Mono), monoCmp);

    // Zachowanie niezmiennika 2).
    if (size == 1 && PolyIsCoeff(&monos[0].p) && MonoGetExp(&monos[0]) == 0) {
        Poly result = PolyFromCoeff(monos[0].p.coeff);
        free(monos);
        return result;
    }

    return (Poly) {.size = size, .arr = monos};
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {

    if (count == 0 || monos == NULL) {
        return PolyZero();
    }

    // Przepisywanie wejściowej tablicy.
    Mono *monos_tmp = (Mono*) safeMalloc (sizeof(Mono) * (count + 1));
    for (size_t i = 0; i < count; i++) {
        monos_tmp[i] = MonoClone(&monos[i]);
    }
    // Placeholder, dzięki temu nie martwimy się o wyjście poza zakres tablicy.
    monos_tmp[count] = (Mono) {.exp = -2};
    // Sortowanie tablicy jednomianów malejąco po wykładniku.
    qsort(monos_tmp, count + 1, sizeof(Mono), monoCmp);

    size_t size = count;
    for (size_t i = 0; i < count; i++) {
        assert (monos_tmp[i].exp >= monos_tmp[i + 1].exp);
        Mono cur = monos_tmp[i];
        Mono next = monos_tmp[i + 1];

        // Jeśli aktualny wielomian jest zerem, to oznaczamy go flagą (exp = -1).
        // Dzięki temu, przy następnym sortowaniu, znajdzie się on
        // za poprawnymi jednomianami.
        if (PolyIsZero(&cur.p)) {
            monos_tmp[i].exp = -1;
            size--;
        }
            // Dodajemy sąsiadów o tym samym wykładniku.
        else if (MonoGetExp(&cur) == MonoGetExp(&next)) {
            Mono tmp = next;
            monos_tmp[i + 1].p = PolyAdd(&cur.p, &tmp.p);
            monos_tmp[i].exp = -1;

            PolyDestroy(&cur.p);
            PolyDestroy(&tmp.p);

            size--;
        }
    }

    if (size == 0) {
        free(monos_tmp);
        return PolyZero();
    }

    // Drugie sortowanie, wyzerowane jednomiany (exp = -1) trafiają na koniec tablicy.
    qsort(monos_tmp, count + 1, sizeof(Mono), monoCmp);

    // Zachowanie niezmiennika 2).
    if (size == 1 && PolyIsCoeff(&monos_tmp[0].p) && MonoGetExp(&monos_tmp[0]) == 0) {
        Poly result = PolyFromCoeff(monos_tmp[0].p.coeff);
        free(monos_tmp);
        return result;
    }

    return (Poly) {.size = size, .arr = monos_tmp};
}

Poly PolyPow(const Poly *p, poly_exp_t exp) {
    if (exp == 0)
        return PolyFromCoeff(1);

    if (PolyIsZero(p))
        return PolyZero();

    Poly res = PolyFromCoeff(1);
    Poly mul = PolyClone(p);
    while (exp > 0) {
        if (exp % 2 == 0) {
            Poly tmp = PolyMul(&mul, &mul);
            PolyDestroy(&mul);
            mul = tmp;
            exp /= 2;
        }
        else {
            if (PolyIsZero(&mul) || PolyIsZero(&res)) {
                PolyDestroy(&mul);
                return res;
            }
            Poly tmp = PolyMul(&res, &mul);
            PolyDestroy(&res);
            res = tmp;
            exp--;
        }
    }
    PolyDestroy(&mul);
    return res;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {

    if (PolyIsCoeff(p))
        return PolyFromCoeff(p->coeff);

    if (k == 0) {
        if (MonoGetExp(&p->arr[p->size - 1]) == 0) {
            return PolyCompose(&p->arr[p->size - 1].p, 0, q);
        }
        else {
            return PolyZero();
        }
    }

    Poly res = PolyZero();
    for (size_t i = 0; i < p->size; i++) {
        Poly p1 = PolyPow(&q[k - 1], MonoGetExp(&p->arr[i]));
        Poly p2 = PolyCompose(&p->arr[i].p, k - 1, q);
        Poly p3 = PolyMul(&p1, &p2);
        PolyDestroy(&p1);
        PolyDestroy(&p2);
        Poly tmp = PolyAdd(&res, &p3);
        PolyDestroy(&p3);
        PolyDestroy(&res);
        res = tmp;
    }

    return res;
}