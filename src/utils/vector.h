/** @file
  Interfejs dynamicznej tablicy w C inspirowanej wektorem z C++

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stddef.h>
#include <stdbool.h>
#include "safe_allocations.h"

/**
 * Domyślny początkowy rozmiar wektora.
 */
#define INIT_CAP 1

/**
 * Zwraca i-ty element wektora z castem na oczekiwany typ @p TYPE.
 * @param[in] TYPE : typ, na który castujemy
 * @param[in] vector : wektor
 * @param[in] index : indeks
 * @return : wskaźnik na i-ty element po zcastowaniu
 */
#define GET_ITEM(TYPE,vector,index) (TYPE*)VectorAt(vector, index)

/**
 * Struktura reprezentująca wektor.
 */
typedef struct vector_t {
	void *items;         ///< Dane przechowywane w wektorze
	size_t obj_size;     ///< Rozmiar instancji danych z wektora (w bajtach)
    size_t cap;          ///< Pojemność (w ilości elementów)
	size_t size;         ///< Zajęte miejsce (w ilości elementów)
} vector_t;

/**
 * Kody stanu zwracane przez niektóre funkcje.
 */
typedef enum vect_errcode_t {
    VECT_OK = 0,         ///< Sukces
    VECT_ERR = -1,       ///< Generyczny błąd
    VECT_ENOMEM = -2,    ///< Brak pamięci
    VECT_UNDERFLOW = -3, ///< Underflow
    VECT_OVERFLOW = -4,  ///< Overflow
} vect_errcode_t;

/**
 * Tworzy nową instancję wektora.
 * @param[in] obj_size : rozmiar każdego elementu w bajtach
 * @param[in] cap : początkowa pojemność wektora
 * @return : nowy wektor
 */
vector_t *VectorNew(size_t obj_size, size_t cap);

/**
 * Destruktor dla dynamicznie zaalokowanego wektora.
 * @param[in] v : wektor
 */
void VectorDestroy(vector_t *v);

/**
 * Zwraca wskaźnik do @p i -tego elementu @p v.
 * @param[in] v : wektor
 * @param[in] i : indeks
 * @return : wskaźnik na i-ty element lub NULL w przypadku za dużego indeksu.
 */
void *VectorAt(vector_t *v, size_t i);

/**
 * Ścina pojemność wektora do ilości jego zajętych elementów (v->size).
 * @param[in] v : wektor
 */
vect_errcode_t VectorShrinkToFit(vector_t *v);

/**
 * Dodaje nie-NULL-owy obiekt (@p item) niezawarty w wektorze
 * @p v na jego koniec.
 * @param[in] v : wektor
 * @param[in] item : dodawany obiekt
 */
vect_errcode_t VectorPush(vector_t *v, const void *item);

/**
 * Zwraca wskaźnik na ostatni element wektora.
 * @param[in] v : wektor
 * @return : wskaźnik na ostatni el.
 */
void *VectorPeek(vector_t *v);

/**
 * Zwraca i usuwa ostatni element z wektora @p v.
 * @param v : wektor
 * @return : ostatni element wektora (lub NULL w przypadku pustego stosu)
 */
void *VectorPop(vector_t *v);

/**
 * Sprawdza czy stos jest pusty.
 * @param v : wektor
 * @return : czy stos ma rozmiar > 0
 */
bool VectorIsEmpty(vector_t *v);

/**
 * Opróżnia wektor.
 * @param v : wektor
 * @return : czy udało się opróżnić wektor
 */
vect_errcode_t VectorClear(vector_t *v);


#endif //__VECTOR_H__