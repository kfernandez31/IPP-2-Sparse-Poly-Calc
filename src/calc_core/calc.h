/** @file
  Implementacja menu do kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __MENU_H__
#define __MENU_H__

/** Makro zdefiniowane, aby korzystać z właściwej funkcji getline(). */
#define _GNU_SOURCE

#include "calc_engine.h"
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
