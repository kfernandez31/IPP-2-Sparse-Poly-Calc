/** @file
  Plik, za pomocą którego dokonywana jest obsługa kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "./calc_core/menu.h"

/**
 * Inicjalizuje kalkulator poprzez menu, wczytuje wielomiany i komendy,
 * na bieżąco zwracając żądany output lub wypisując błędy w przypadku
 * nieprawidłowych danych.
 */
int main() 
{
    Menu menu;
    run(&menu);

    exit(EXIT_SUCCESS);
}
