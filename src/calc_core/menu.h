#ifndef __MENU_H__
#define __MENU_H__

// Makro zdefiniowane, aby korzystać z właściwej funkcji getline().
#define _GNU_SOURCE

#include "calc.h"
#include "parsing.h"

/**
 * Struktura stanowiąca warstwę abstrakcji do obsługi kalkulatora.
 */
typedef struct Menu {
    Calculator calc; ///< Kalkulator
} Menu;

/**
 * Uruchamia działanie kalkulatora wraz z przetwarzaniem danych
 * i wypisywaniem błędów.
 *
 * @param[in] menu : interfejs kalkulatora
 */
void run(Menu* menu);

#endif //__MENU_H__
