/** @file
  Interfejs kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef __CALC_H__
#define __CALC_H__

#include <limits.h>
#include <errno.h>
#include "../poly_core/poly.h"
#include "../utils/vector.h"
#include "line_structures.h"

/**
 * Makro sprawdzające poprawność wprowadzanego współczynnika (dla komendy CalcAt i parsowania wielomianów)
 * @see CalcAt(), parsePoly()
 *
 * @param[in] x : współczynnik
 *
 * @return : czy współczynnik spełnia warunki zadania
 */
#define IS_VALID_COEFF(x) (!((x == LONG_MAX || x == LONG_MIN) && errno == ERANGE))

/**
 * Makro sprawdzające poprawność wprowadzanego wykładnika (dla parsowania jednomianów)
 * @see parseMonoRecursive()
 *
 * @param[in] y : wykładnik jednomianu
 *
 * @return : czy wykładnik spełnia warunki zadania
 */
#define IS_VALID_EXP(y) (y >= 0  && y <= 2147483647)

/**
 * Makro sprawdzające poprawność wprowadzanego indeksu zmiennej (dla komendy CalcDegBy)
 * @see CalcDegBy()
 *
 * @param[in] y : indeks zmiennej
 *
 * @return : czy indeks zmiennej spełnia warunki zadania
 */
#define IS_VALID_IDX(y) (y <= ULLONG_MAX && !(y == ULLONG_MAX && errno == ERANGE))

/**
 * Makro sprawdzające poprawność argumentu dla funkcji Compose
 * @see CalcCompose()
 * @param[in] y : liczba podstawianych wielomianów
 *
 * @return : czy liczba wielomianów spełnia warunki zadania
 */
#define IS_VALID_COMPOSE_ARG(y) IS_VALID_IDX(y)

/**
 * Kody stanu zwracane przez niektóre funkcje.
 */
typedef enum cmd_errcode_t
{
    CMD_OK,              ///< Generyczny sukces, w tym komenda wykonana pomyślnie
    CMD_WRONG_COMMAND,   ///< Błąd - niewłaściwa komenda
    CMD_STACK_UNDERFLOW, ///< Błąd - komenda korzysta ze zbyt małego stosu
    CMD_INVALID_ARG      ///< Błąd - podano nieprawidłowy argument do komendy
} cmd_errcode_t;

/**
 * Unia przechowująca aktualnie rozpatrywany argument
 * potrzebny dla funkcji CalcDegBy() i CalcAt()
 */
typedef union cmd_arg {
    poly_coeff_t x; ///< Argument dla CalcAt()
    size_t y;       ///< Argument dla CalcDegBy() i PolyCompose()
} cmd_arg;

/**
 * Stos wielomianów.
 */
typedef vector_t poly_stack_t;

/**
 * Kalkulator wykonujący działania na wielomianach.
 */
typedef struct Calculator {
    cmd_arg arg;              ///< Aktualnie rozpatrywany argument dla CalcAt/CalcDegBy/CalcCompose
    poly_stack_t *polyStack;   ///< Stos wielomianów
} Calculator;

/**
 * Wskaźnik na komendę kalkulatora.
 * @param[in, out] calc : kalkulator
 */
typedef cmd_errcode_t (*cmd_func) (Calculator *calc);

/**
 * Struktura asocjująca nazwę komendy z odpowiadającą
 * jej funkcją.
 */
typedef struct command_pair_t {
    const char *const name;    ///< Komenda (nazwa)
    cmd_func func;              ///< Komenda (funkcja)
} command_pair_t;



/**
 * Konstruktor dla kalkulatora - inicjalizuje go.
 *
 * @param[in, out] calc : kalkulator
 */
void CalcInit(Calculator *calc);

/**
 * Destruktor dla kalkulatora - zwalnia jego pamięć.
 *
 * @param[in, out] calc : kalkulator
 */
void CalcDestroy(Calculator *calc);

/**
 * Funkcja parsująca argument jako drugi token linii wejściowej
 * i przechowująca go w @p calc->arg
 *
 * @param[in, out] calc : kalkulator
 * @param[in] line : linia, znająca swój indeks
 * @param[in] str : argument przed obróbką
 *
 * @return : czy wprowadzono poprawny argument
 */
cmd_errcode_t CalcParseArg(Calculator *calc, const Line line, const char *str);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia ZERO przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcZero(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia IS_COEFF przez kalkulator.
 *
 * @param[in] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcIsCoeff(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia IS_ZERO przez kalkulator.
 *
 * @param[in] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcIsZero(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia CLONE przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcClone(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia ADD przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcAdd(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia MUL przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcMul(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia NEG przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcNeg(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia SUB przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcSub(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia IS_EQ przez kalkulator.
 *
 * @param[in] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcIsEq(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia DEG przez kalkulator.
 *
 * @param[in] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcDeg(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia DEG_BY x przez kalkulator.
 *
 * @param[in] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcDegBy(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia AT x przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcAt(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia PRINT przez kalkulator.
 *
 * @param[in] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcPrint(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia POP przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcPop(Calculator *calc);

/**
 * Funkcja odpowiedzialna za wykonanie polecenia COMPOSE przez kalkulator.
 *
 * @param[in, out] calc : kalkulator
 *
 * @return : wynik wykonania komendy (sukces/błąd)
 */
cmd_errcode_t CalcCompose(Calculator *calc);


#endif //__CALC_H__
