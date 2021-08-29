/** @file
  Struktury wykorzystywane przy tworzeniu i operowaniu na wielomianach rzadkich wielu zmiennych

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>,
  Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __POLY_STRUCTURES_H__
#define __POLY_STRUCTURES_H__

#include <stdlib.h>
#include <stdbool.h>

/** Typ reprezentujący współczynniki. */
typedef long poly_coeff_t;


/** Typ reprezentujący wykładniki. */
typedef int poly_exp_t;

/**
 * Struktura przechowująca wielomian.
 * Wielomian jest albo liczbą całkowitą, czyli wielomianem stałym
 * (wtedy `arr == NULL`), albo niepustą listą jednomianów (wtedy `arr != NULL`).
 */
typedef struct Poly {
    /**
    * Unia przechowująca współczynnik wielomianu lub
    * liczbę jednomianów w wielomianie.
    * Jeżeli `arr == NULL`, wtedy jest to współczynnik będący liczbą całkowitą.
    * W przeciwnym przypadku jest to niepusta lista jednomianów.
    */
    union {
        poly_coeff_t coeff; ///< współczynnik
        size_t       size;  ///< rozmiar wielomianu, liczba jednomianów
    };
    /** Tablica przechowująca listę jednomianów. */
    struct Mono *arr;
} Poly;


/**
 * Struktura przechowująca jednomian.
 * Jednomian ma postać @f$px_i^n@f$.
 * Współczynnik @f$p@f$ może też być
 * wielomianem nad kolejną zmienną @f$x_{i+1}@f$.
 */
typedef struct Mono {
    Poly p;         ///< współczynnik
    poly_exp_t exp; ///< wykładnik
} Mono;


#endif //__POLY_STRUCTURES_H__