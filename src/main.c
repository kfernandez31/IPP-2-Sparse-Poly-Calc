#include "./calc_core/menu.h"
/**
 * Inicjalizuje kalkulator poprzez menu, wczytuje wielomiany i komendy,
 * na bieżąco zwracając żądany output lub wypisując błędy w przypadku
 * nieprawidłowych danych.
 */

int main() {
    Menu menu;
    run(&menu);

    exit(EXIT_SUCCESS);
}
