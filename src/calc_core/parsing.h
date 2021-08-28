#ifndef __PARSING_H__
#define __PARSING_H__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "calc.h"

/**
 * Kody stanu zwracane podczas parsowania wielomianów
 */
typedef enum poly_errcode_t
{
    POLY_OK,  ///< Pomyślne parsowanie
    POLY_ERR  ///< Błąd w parsowaniu
} poly_errcode_t;

/**
 * Wczytuje linię ze standardowego wejścia, odnotowuje czy zawiera ona
 * wielomian czy polecenie kalkulatora, indeksuje ją.
 *
 * @param[in, out] line : struktura przechowująca informacje o linii
 * @param[in] index : indeks, który zostanie przypisany linii
 * @param[in] str : linia reprezentowana stringiem
 * @param[in] nread : ilość wczytanych znaków w linii (do '\n' albo EOF)
 */
void detectLineType(Line *line, const size_t index, char *str, const long long nread);

/**
 * Konstruuje wielomian w oparciu o string @p str. Wypisuje na wyjście
 * diagnostyczne komunikaty błędu wraz z indeksem linii,
 * jeśli konwersja się nie powiodła i kasuje pamięć dotychczas zbudowanej struktury.
 *
 * @param[in] p : wielomian
 * @param[in] str : string
 * @param[in] line : linia, przechowująca swój indeks
 *
 * @return : czy udało się stworzyć wielomian
 */
poly_errcode_t parsePoly(Poly *p, const char *str, const Line line);


#endif //__PARSING_H__
