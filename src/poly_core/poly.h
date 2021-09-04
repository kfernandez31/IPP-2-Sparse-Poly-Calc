/** @file
  Interfejs klasy wielomianów rzadkich wielu zmiennych

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>,
  Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __POLY_H__
#define __POLY_H__

#include "poly_structures.h"
#include "../utils/safe_allocations.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/**
 * Daje wartość wykładnika jednomianu.
 * @param[in] m : jednomian
 * @return wartość wykładnika jednomianu
 */
static inline poly_exp_t MonoGetExp(const Mono *m) {
    return m->exp;
}

/**
 * Tworzy wielomian, który jest współczynnikiem (wielomian stały).
 * @param[in] c : wartość współczynnika
 * @return wielomian
 */
static inline Poly PolyFromCoeff(poly_coeff_t c) {
    return (Poly) {
        .coeff = c,
        .arr = NULL
    };
}

/**
 * Tworzy wielomian tożsamościowo równy zeru.
 *
 * @return : wielomian zerowy
 */
static inline Poly PolyZero() {
    return PolyFromCoeff(0);
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem (czy jest to wielomian stały).
 *
 * @param[in] p : wielomian
 *
 * @return : czy wielomian jest współczynnikiem
 */
static inline bool PolyIsCoeff(const Poly *p)
{
    return p->arr == NULL;
}

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 *
 * @param[in] p : wielomian
 *
 * @return czy wielomian jest równy zeru
 */
static inline bool PolyIsZero(const Poly *p) {
    return PolyIsCoeff(p) && p->coeff == 0;
}

/**
 * Tworzy jednomian @f$px_i^n@f$.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 *
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] n : wykładnik
 *
 * @return jednomian @f$px_i^n@f$
 */
Mono MonoFromPoly(const Poly *p, poly_exp_t n);

/**
 * Usuwa jednomian z pamięci.
 *
 * @param[in] m : jednomian
 */
void MonoDestroy(Mono *m);

/**
 * Robi pełną, głęboką kopię jednomianu.
 *
 * @param[in] m : jednomian
 *
 * @return skopiowany jednomian
 */
Mono MonoClone(const Mono *m);

/**
 * Usuwa wielomian z pamięci.
 *
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Robi pełną, głęboką kopię wielomianu.
 *
 * @param[in] p : wielomian
 *
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);

/**
 * Dodaje dwa wielomiany.
 *
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 *
 * @return @f$p + q@f$
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Mnoży dwa wielomiany.
 *
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 *
 * @return @f$p * q@f$
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 *
 * @param[in] p : wielomian @f$p@f$
 *
 * @return @f$-p@f$
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 *
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 *
 * @return @f$p - q@f$
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru). Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 *
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 *
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, size_t var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 *
 * @param[in] p : wielomian
 *
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch wielomianów.
 *
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 *
 * @return @f$p == q@f$
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianami.
 * Wtedy zmniejszane są o jeden indeksy zmiennych w takim wielomianie.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 *
 * @param[in] p : wielomian @f$p@f$
 * @param[in] x : wartość wielomianu dla argumentu @f$x@f$
 *
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Zmienia znaki współczynników w wielomianie na przeciwne.
 *
 * @param[in, out] p : wielomian
 */
void PolyNegateCoeffs(Poly *p);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 *
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 *
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(size_t count, const Mono monos[]);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian. Przejmuje na własność
 * pamięć wskazywaną przez @p monos i jej zawartość. Może dowolnie modyfikować
 * zawartość tej pamięci. Zakładamy, że pamięć wskazywana przez @p monos
 * została zaalokowana na stercie. Jeśli @p count lub @p monos jest równe zeru
 * (NULL), tworzy wielomian tożsamościowo równy zeru.
 *
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 *
 * @return wielomian będący sumą jednomianów
 */
Poly PolyOwnMonos(size_t count, Mono *monos);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian. Nie modyfikuje zawartości
 * tablicy @p monos. Jeśli jest to wymagane, to wykonuje pełne kopie jednomianów
 * z tablicy @p monos. Jeśli @p count lub @p monos jest równe zeru (NULL),
 * tworzy wielomian tożsamościowo równy zeru.
 *
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 *
 * @return wielomian będący sumą jednomianów
 */
Poly PolyCloneMonos(size_t count, const Mono monos[]);

/**
 * Dokonuje złożenia wielomianu @p p z @p k wielomianami z tablicy @p q.
 * Formalna definicja złożenia (z treści zadania):
 *
 * "Dany jest wielomian p oraz k wielomianów q0, q1, q2, …, qk−1.
 * Niech l oznacza liczbę zmiennych wielomianu p i niech te zmienne są oznaczone
 * odpowiednio x0, x1, x2, …, xl−1. Wynikiem złożenia jest wielomian p(q0,q1,q2,…),
 * czyli wielomian powstający przez podstawienie w wielomianie p pod zmienną
 * xi wielomianu qi dla i=0,1,2,…,min(k,l)−1. Jeśli k<l, to pod zmienne xk, …, xl−1
 * podstawiamy zera. Na przykład, jeśli k=0, to wynikiem złożenia jest liczba p(0,0,0,…)."
 *
 * @param[in] p : wielomian
 * @param[in] k : liczba wielomianów/podstawianych zmiennych
 * @param[in] q : tablica wielomianów (zmienne nowego wielomianu)
 *
 * @return @f$p(q_0,q_1,...,q_{min(k,l)-1)}@f$
 */
Poly PolyCompose(const Poly *p, size_t k, const Poly q[]);

#endif //__POLY_H__
