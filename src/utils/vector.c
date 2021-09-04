/** @file
  Implementacja dynamicznej tablicy w C inspirowanej wektorem z C++

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vector.h"



/**
 * Zwraca wskaźnik do i-tego elementu wektora @p v bez sprawdzania poprawności żądania.
 * @param[in] v : wektor
 * @param[in] i : indeks
 * @return : wskaźnik na i-ty element
 */
#define VECT_AT(v, i) ((void *) ((char *) (v)->items + (i) * (v)->obj_size))

/**
 * Minimalny zakres alokacji (w bajtach).
 */
#define VECT_MIN_ALLOC (64u)

/**
 * Próg, po którym wektor uznawany jest za "duży".
 */
#define GROWTH_THRESHOLD 4096



/**
 * Powiększa pojemność @p v przynajmniej do @p cap.
 * Jeśli jest potrzebne więcej miejsca, skaluje pojemność o
 * czynnik równy 1.5 (albo 2 dla małych pojemności).
 * @param[in] v : wektor
 * @param[in] cap : żądana pojemność
 */
static vect_errcode_t VectorExpand(vector_t *v, size_t cap);



static vect_errcode_t VectorExpand(vector_t *v, size_t cap)
{
	void *new_items;
	size_t new_cap;

	assert(v != NULL);
	assert(v->items != NULL);

	if (cap <= v->cap) {
		return VECT_OK;
	}

	assert(v->obj_size > 0);

	if (cap >= (size_t) -1 / v->obj_size) {
		return VECT_ERR;
	}

	/* Czynnik skalujący 2 dla małych wektorów, 1.5 dla większych */
	if (v->cap < GROWTH_THRESHOLD / v->obj_size) {
        new_cap = 2 * v->cap + 1;
	}
	else {
        new_cap = 3 * v->cap / 2 + 1;
	}

	if (cap > new_cap || new_cap >= (size_t) -1 / v->obj_size) {
        new_cap = cap;
	}

    new_items = safeRealloc(v->items, new_cap * v->obj_size);

	if (new_items == NULL) {
		return VECT_ENOMEM;
	}

	v->items = new_items;
	v->cap = new_cap;

	return VECT_OK;
}



vector_t *VectorNew(size_t obj_size, size_t cap)
{
	vector_t *v;

	if (cap == 0) {
		cap = 1;
	}

	if (obj_size == 0 || cap >= (size_t) -1 / obj_size) {
		return NULL;
	}

	v = safeMalloc(sizeof(vector_t));

	if (v == NULL) {
	    return NULL;
	}

	/* Minimalna pojemność to VECT_MIN_ALLOC bajtów lub 1 element */
	if (cap * obj_size < VECT_MIN_ALLOC) {
		cap = (VECT_MIN_ALLOC + (obj_size - 1)) / obj_size;
	}

	v->items = safeMalloc(cap * obj_size);

    if (v->items == NULL) {
        free(v);
        return NULL;
    }

	v->obj_size = obj_size;
	v->size = 0;
	v->cap = cap;

	return v;
}

void VectorDestroy(vector_t *v)
{
    if (v == NULL) {
        return;
    }

    if (v->items != NULL) {
        free(v->items);
        v->items = NULL;
    }

    v->obj_size = 0;
    v->size = 0;
    v->cap = 0;

    free(v);
}

vect_errcode_t VectorShrinkToFit(vector_t *v)
{
	void *new_items;
	size_t new_cap;

	assert(v != NULL);
	assert(v->items != NULL);

	if (v->cap == v->size) {
		return VECT_OK;
	}

    new_cap = v->size;

	assert(v->obj_size > 0);
	assert(new_cap < (size_t) -1 / v->obj_size);

    /* Minimalna pojemność to VECT_MIN_ALLOC bajtów lub 1 element */
	if (new_cap * v->obj_size < VECT_MIN_ALLOC) {
        new_cap = (VECT_MIN_ALLOC + (v->obj_size - 1)) / v->obj_size;
	}

    new_items = safeRealloc(v->items, new_cap * v->obj_size);

	if (new_items == NULL) {
	    return VECT_ENOMEM;
	}

	v->items = new_items;
	v->cap = new_cap;
	return VECT_OK;
}

vect_errcode_t VectorPush(vector_t *v, const void *item)
{
	assert(v != NULL);
	assert(v->items != NULL);
	assert(item != NULL);

	if (v->size + 1 > v->cap) {
        vect_errcode_t res = VectorExpand(v, v->size + 1);

        if (res != VECT_OK) {
            return res;
        }
	}

	assert(v->obj_size > 0);

	memcpy(VECT_AT(v, v->size), item, v->obj_size);

	v->size += 1;
	return VECT_OK;
}

void *VectorPop(vector_t *v)
{
    assert(v != NULL);
    assert(v->items != NULL);

    if (v->size == 0) {
        return NULL;
    }
    void *top = VectorPeek(v);

    v->size -= 1;

    return top;
}

bool VectorIsEmpty(vector_t *v)
{
    return v->size == 0 || v->items == NULL;
}

void *VectorAt(vector_t *v, size_t i)
{
    assert(v != NULL);
    assert(v->items != NULL);

    if (i >= v->size) {
        return NULL;
    }

    return VECT_AT(v, i);
}

void *VectorPeek(vector_t *v)
{
    assert(v != NULL);
    assert(v->items != NULL);

    if (v->size == 0) {
        return NULL;
    }

    return VECT_AT(v, v->size - 1);
}

vect_errcode_t VectorClear(vector_t *v)
{
    assert(v != NULL);
    assert(v->items != NULL);

    v->size = 0;

    return VECT_OK;
}