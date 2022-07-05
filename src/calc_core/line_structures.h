/** @file
  Struktury wykorzystywane do przetwarzania danych wejściowych w kalkulatorze wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __LINE_STRUCTURES_H__
#define __LINE_STRUCTURES_H__

#include "calc_engine.h"
#include "../poly_core/poly_structures.h"

/**
 * Enum informujący o typie linii.
 */
typedef enum LineType {
    IGNORED,   ///< Linia jest pusta/komentarzem lub z błędną zawartością
    CMD_LINE,  ///< Linia zawiera komendę
    POLY_LINE  ///< Linia zawiera wielomian
} LineType;

/**
 * Enum informujący o typie komendy w linii.
 * Każda (poza pierwszą i ostatnią) ma funkcyjny odpowiednik o przedrostku "Calc"
 * @see calc.h
 */
typedef enum CommandCode {
    ZERO,
    IS_COEFF,
    IS_ZERO,
    CLONE,
    ADD,
    MUL,
    NEG,
    SUB,
    IS_EQ,
    DEG,
    DEG_BY,
    AT,
    PRINT,
    POP,
    COMPOSE,

    COMMAND_COUNT
} CommandCode;

/**
 * Struktura przechowująca informacje o linii.
 */
typedef struct Line {
    size_t index;         ///< Indeks linii
    LineType type;        ///< Typ linii (wielomian bądź komenda)
    union {
        CommandCode cmd;  ///< Kod komendy zawartej w linii
        Poly poly;        ///< Wielomian zawarty w linii
    } contents;           ///< Zawartość linii (wielomian bądź komenda)
} Line;

#endif //__LINE_STRUCTURES_H__
