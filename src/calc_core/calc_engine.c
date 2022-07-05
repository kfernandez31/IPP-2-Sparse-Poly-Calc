/** @file
  Implementacja kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <ctype.h>
#include "calc_engine.h"
#include "line_structures.h"

/**
 * Funkcja drukująca wielomian, wrapper dla PolyPrintRecursive().
 * @see PolyPrintRecursive()
 *
 * @param[in, out] stream : strumień wyjścia
 * @param[in] p : wielomian
 */
static void PolyPrint(FILE *stream, const Poly *p);

/**
 * Funkcja rekurencyjnie wypisująca wielomian
 * do wybranego strumienia wyjścia.
 *
 * @param[in, out] stream : strumień wyjścia
 * @param[in] p : wielomian
 */
static void PolyPrintRecursive(FILE *stream, const Poly *p);

/**
 * Funkcja rekurencyjnie wypisująca jednomian
 * do wybranego strumienia wyjścia.
 *
 * @param[in, out] stream : strumień wyjścia
 * @param[in] m : jednomian
 */
static void MonoPrint(FILE *stream, const Mono *m);

/**
 * Przetwarza argument dla komendy AT. Zwraca rezultat operacji,
 * wypisuje ewentualne błędy w konwersji na wyjście diagnostyczne.
 *
 * @param[in, out] calc : kalkulator
 * @param[in] line : linia, wraz z indeksem
 * @param[in] str : argument przed obróbką
 *
 * @return : czy wprowadzono poprawny argument
 */
static cmd_errcode_t CalcParseAtArg(Calculator *calc, const Line line, const char *str);

/**
 * Przetwarza argument dla komendy DEG_BY. Zwraca rezultat operacji,
 * wypisuje ewentualne błędy w konwersji na wyjście diagnostyczne.
 *
 * @param[in, out] calc : kalkulator
 * @param[in] line : linia, wraz z indeksem
 * @param[in] str : argument przed obróbką
 *
 * @return : czy wprowadzono poprawny argument
 */
static cmd_errcode_t CalcParseDegByArg(Calculator *calc, const Line line, const char *str);

/**
 * Przetwarza argument dla komendy COMPOSE. Zwraca rezultat operacji,
 * wypisuje ewentualne błędy w konwersji na wyjście diagnostyczne.
 *
 * @param[in, out] calc : kalkulator
 * @param[in] line : linia, wraz z indeksem
 * @param[in] str : argument przed obróbką
 *
 * @return : czy wprowadzono poprawny argument
 */
static cmd_errcode_t CalcParseComposeArg(Calculator *calc, const Line line, const char *str);



static void PolyPrint(FILE *stream, const Poly *p)
{
    PolyPrintRecursive(stream, p);
    fprintf(stream,"\n");
}

static void PolyPrintRecursive(FILE *stream, const Poly *p)
{
    if (PolyIsCoeff(p)) {
        fprintf(stream, "%ld", p->coeff);
    }
    else {
        for (size_t i = p->size; i > 0; i--) {
            if (i != p->size) {
                fprintf(stream, "+");
            }
            MonoPrint(stream, &p->arr[i - 1]);
        }
    }
}

static void MonoPrint(FILE *stream, const Mono *m)
{
    fprintf(stream, "(");
    PolyPrintRecursive(stream, &m->p);
    fprintf(stream, ",%d", MonoGetExp(m));
    fprintf(stream, ")");
}

static cmd_errcode_t CalcParseDegByArg(Calculator *calc, const Line line, const char *str)
{
    char *endptr = NULL;

    if (str == NULL || strlen(str) == 0 || str[0] != ' ' || strcmp(str, " ") == 0) {
        fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", line.index);
        return CMD_INVALID_ARG;
    }
    for (size_t i = 1; i < strlen(str); i++) {
        if (!isdigit(str[i])) {    //sprawdzenie czy w stringu są same cyfry
            fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", line.index);
            return CMD_INVALID_ARG;
        }
    }
    errno = 0;    // konwersja współczynnika
    long long unsigned res_ull = strtoull(str, &endptr, 10);

    if ((res_ull == 0 && (errno != 0 || endptr == str)) || !IS_VALID_IDX(res_ull)) {
        fprintf(stderr, "ERROR %zu DEG BY WRONG VARIABLE\n", line.index);
        return CMD_INVALID_ARG;    // input nie reprezentuje liczby lub overflow
    }
    else {
        calc->arg.y = res_ull;
        return CMD_OK;
    }
}

static cmd_errcode_t CalcParseAtArg(Calculator *calc, const Line line, const char *str)
{
    char *endptr = NULL;

    if (str == NULL || strlen(str) == 0 || str[0] != ' ' || strcmp(str, " ") == 0 || strcmp(str, " -") == 0) {
        fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", line.index);
        return CMD_INVALID_ARG;
    }
    for (size_t i = (str[1]=='-')? 2 : 1; i < strlen(str); i++) {
        if (!isdigit(str[i])) {    //sprawdzenie czy w stringu są same cyfry (i ewentualny minus na początku)
            fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", line.index);
            return CMD_INVALID_ARG;
        }
    }
    errno = 0;
    long res_l = strtol(str, &endptr, 10);

    if ((res_l == 0 && (errno != 0 || endptr == str)) || !IS_VALID_COEFF(res_l)) {
        fprintf(stderr, "ERROR %zu AT WRONG VALUE\n", line.index);
        return CMD_INVALID_ARG;    // input nie reprezentuje liczby lub overflow/underflow
    }
    else {
        calc->arg.x = res_l;
        return CMD_OK;
    }
}

static cmd_errcode_t CalcParseComposeArg(Calculator *calc, const Line line, const char *str)
{
    char *endptr = NULL;

    if (str == NULL || strlen(str) == 0 || str[0] != ' ' || strcmp(str, " ") == 0) {
        fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", line.index);
        return CMD_INVALID_ARG;
    }
    for (size_t i = 1; i < strlen(str); i++) {
        if (!isdigit(str[i])) {    //sprawdzenie czy w stringu są same cyfry
            fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", line.index);
            return CMD_INVALID_ARG;
        }
    }
    errno = 0;    // konwersja współczynnika
    long long unsigned res_ull = strtoull(str, &endptr, 10);

    if ((res_ull == 0 && (errno != 0 || endptr == str)) || !IS_VALID_COMPOSE_ARG(res_ull)) {
        fprintf(stderr, "ERROR %zu COMPOSE WRONG PARAMETER\n", line.index);
        return CMD_INVALID_ARG;    // input nie reprezentuje liczby lub overflow
    }
    else {
        calc->arg.y = res_ull;
        return CMD_OK;
    }
}



void CalcInit(Calculator *calc)
{
    calc->polyStack = VectorNew(sizeof(Poly), INIT_CAP);
}

void CalcDestroy(Calculator *calc)
{
    for (size_t i = 0; i < calc->polyStack->size; i++) {
        Poly *p = GET_ITEM(Poly, calc->polyStack, i);
        PolyDestroy(p);
    }
    VectorDestroy(calc->polyStack);
}

cmd_errcode_t CalcParseArg(Calculator *calc, const Line line, const char *str)
{
    if (line.contents.cmd == DEG_BY) {
        return CalcParseDegByArg(calc, line, str);
    }
    else if (line.contents.cmd == AT) {
        return CalcParseAtArg(calc, line, str);
    }
    else if (line.contents.cmd == COMPOSE) {
        return CalcParseComposeArg(calc, line, str);
    }
    else if (str != NULL) {
        fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line.index);
        return CMD_INVALID_ARG;    // wprowadzono argument dla funkcji, która go nie wymaga
    }
    return CMD_OK;
}



cmd_errcode_t CalcZero(Calculator *calc)
{
    Poly zero = PolyZero();
    if (VectorPush(calc->polyStack, &zero) != VECT_OK) {
        exit(EXIT_FAILURE);
    }
    return CMD_OK;
}

cmd_errcode_t CalcIsCoeff(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);
    fprintf(stdout, "%d\n", PolyIsCoeff(top));

    return CMD_OK;
}

cmd_errcode_t CalcIsZero(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);
    fprintf(stdout, "%d\n", PolyIsZero(top));

    return CMD_OK;
}

cmd_errcode_t CalcClone(Calculator *calc)
{
    Poly *top = NULL, clone;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);
    clone = PolyClone(top);

    if (VectorPush(calc->polyStack, &clone) != VECT_OK) {
        exit(EXIT_FAILURE);
    }

    return CMD_OK;
}

cmd_errcode_t CalcAdd(Calculator *calc)
{
    Poly *first = NULL, *second = NULL, res;

    if (VectorIsEmpty(calc->polyStack) || calc->polyStack->size < 2) {
        return CMD_STACK_UNDERFLOW;
    }

    first = (Poly *) VectorPop(calc->polyStack);
    second = (Poly *) VectorPop(calc->polyStack);

    res = PolyAdd(first, second);
    PolyDestroy(first);
    PolyDestroy(second);

    VectorPush(calc->polyStack, &res);

    return CMD_OK;
}

cmd_errcode_t CalcMul(Calculator *calc)
{
    Poly *first = NULL, *second = NULL, res;

    if (VectorIsEmpty(calc->polyStack) || calc->polyStack->size < 2) {
        return CMD_STACK_UNDERFLOW;
    }

    first = (Poly *) VectorPop(calc->polyStack);
    second = (Poly *) VectorPop(calc->polyStack);

    res = PolyMul(first, second);
    PolyDestroy(first);
    PolyDestroy(second);

    VectorPush(calc->polyStack, &res);

    return CMD_OK;
}

cmd_errcode_t CalcNeg(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);
    PolyNegateCoeffs(top);

    return CMD_OK;
}

cmd_errcode_t CalcSub(Calculator *calc)
{
    Poly *first = NULL, *second = NULL, res;

    if (VectorIsEmpty(calc->polyStack) || calc->polyStack->size < 2) {
        return CMD_STACK_UNDERFLOW;
    }

    first = (Poly *) VectorPop(calc->polyStack);
    second = (Poly *) VectorPop(calc->polyStack);

    res = PolySub(first, second);
    PolyDestroy(first);
    PolyDestroy(second);

    VectorPush(calc->polyStack, &res);

    return CMD_OK;
}

cmd_errcode_t CalcIsEq(Calculator *calc)
{
    Poly *first = NULL, *second = NULL;

    if (VectorIsEmpty(calc->polyStack) || calc->polyStack->size < 2) {
        return CMD_STACK_UNDERFLOW;
    }

    first = (Poly *) VectorPeek(calc->polyStack);
    second = (Poly *) VectorAt(calc->polyStack, calc->polyStack->size - 2);

    fprintf(stdout, "%d\n", PolyIsEq(first, second));

    return CMD_OK;
}

cmd_errcode_t CalcDeg(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);

    fprintf(stdout, "%d\n", PolyDeg(top));

    return CMD_OK;
}

cmd_errcode_t CalcDegBy(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);

    fprintf(stdout, "%d\n", PolyDegBy(top, calc->arg.y));

    return CMD_OK;
}

cmd_errcode_t CalcAt(Calculator *calc)
{
    Poly *top = NULL, res;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPop(calc->polyStack);
    res = PolyAt(top, calc->arg.x);

    PolyDestroy(top);
    VectorPush(calc->polyStack, &res);

    return CMD_OK;
}

cmd_errcode_t CalcPrint(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPeek(calc->polyStack);

    PolyPrint(stdout, top);

    return CMD_OK;
}

cmd_errcode_t CalcPop(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack)) {
        return CMD_STACK_UNDERFLOW;
    }

    top = (Poly *) VectorPop(calc->polyStack);

    PolyDestroy(top);

    return CMD_OK;
}

cmd_errcode_t CalcCompose(Calculator *calc)
{
    Poly *top = NULL;

    if (VectorIsEmpty(calc->polyStack) || calc->polyStack->size < calc->arg.y + 1 || calc->arg.y == ULLONG_MAX) {
        return CMD_STACK_UNDERFLOW;
    }
    Poly *q = safeMalloc(calc->arg.y * sizeof(Mono));
    top = (Poly *) VectorPop(calc->polyStack);

    for (size_t i = calc->arg.y; i > 0; i--) {
        q[i - 1] = *(Poly *)VectorPop(calc->polyStack);
    }

    Poly res = PolyCompose(top, calc->arg.y, q);
    PolyDestroy(top);
    VectorPush(calc->polyStack, &res);

    for (size_t i = 0; i < calc->arg.y; i++) {
        PolyDestroy(q + i);
    }
    free(q);

    return CMD_OK;
}