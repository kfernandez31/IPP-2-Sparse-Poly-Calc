/** @file
  Implementacja funkcji przetwarzających dane wejściowe dla kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "parsing.h"

/**
 * Sprawdza czy dany string reprezentuje wielomian.
 *
 * @param[in] str : string
 *
 * @return : czy @p str jest wielomianem zapisanym w konwencji z zadania
 */
static bool doesStringRepresentPoly(const char *str);

/**
 * Sprawdza czy dany string reprezentuje wielomian (będący współczynnikiem).
 *
 * @param[in] str : string
 *
 * @return : czy @p str jest wielomianem współczynnikowym zapisanym w konwencji z zadania
 */
static bool doesStringRepresentCoeffPoly(const char *str);

/**
 * Sprawdza czy dany string reprezentuje wielomian (niebędący współczynnikiem).
 *
 * @param[in] str : string
 *
 * @return : czy @p str jest wielomianem niewspółczynnikowym zapisanym w konwencji z zadania
 */
static bool doesStringRepresentNonCoeffPoly(const char *str);

/**
 * Funkcja rekurencyjna, której parsePoly() jest wrapperem.
 * @see : parsePoly()
 *
 * @param[in, out] p : wielomian
 * @param[in] str : wielomian jako string
 * @param[in] start_idx : indeks, od którego rozpoczyna się wielomian w @p str
 * @param[in] end_idx : indeks, na którym kończy się wielomian w @p str
 *
 * @return : czy jednomiany/współczynnik wielomianu zostały zparsowane poprawnie
 */
static poly_errcode_t parsePolyRecursive(Poly *p, const char *str, const size_t start_idx, const size_t end_idx);

/**
 * Funkcja konstruująca wielomian współczynnikowy.
 * @see : parsePolyRecursive()
 *
 * @param[in, out] p : wielomian
 * @param[in] str : wielomian jako string
 * @param[in] start_idx : indeks, od którego rozpoczyna się wielomian w @p str
 * @param[in] end_idx : indeks, na którym kończy się wielomian w @p str
 *
 * @return : czy wielomian współczynnikowy został sparsowany poprawnie
 */
static poly_errcode_t parseCoeffPoly(Poly *p, const char *str, const size_t start_idx, const size_t end_idx);

/**
 * Funkcja parsująca wielomian niewspółczynnikowy. Wypisuje na wyjście
 * diagnostyczne komunikaty błędu, jeśli konwersja się nie powiedzie.
 * @see : parsePolyRecursive()
 *
 * @param[in, out] p : wielomian
 * @param[in] str : wielomian jako string
 * @param[in] start_idx : indeks, od którego rozpoczyna się wielomian w @p str
 * @param[in] end_idx : indeks, na którym kończy się wielomian w @p str
 *
 * @return : czy wielomian niewspółczynnikowy został sparsowany poprawnie
 */
static poly_errcode_t parseNonCoeffPoly(Poly *p, const char *str, const size_t start_idx, const size_t end_idx);

/**
 * Konstruuje jednomian w oparciu o string.
 * @see : parsePoly()
 *
 * @param[in, out] m : jednomian
 * @param[in] str : string
 * @param[in] start_idx : indeks, od którego rozpoczyna się jednomian w @p str
 * @param[in] end_idx : indeks, na którym kończy się jednomian w @p str
 *
 * @return : czy wielomian i współczynnik jednomianu zostały zparsowane poprawnie
 */
static poly_errcode_t parseMonoRecursive(Mono *m, const char *str, const size_t start_idx, const size_t end_idx);



static bool doesStringRepresentPoly(const char *str)
{
    size_t str_len = strlen(str);

    if (str_len == 0 || (str_len == 1 && !isdigit(str[0]))) {
        return false;
    }
    if (str[0] == '-' || isdigit(str[0])) {
        return doesStringRepresentCoeffPoly(str);
    }
    if (str[0] == '(') {
        return doesStringRepresentNonCoeffPoly(str);
    }
    return false;
}

static bool doesStringRepresentCoeffPoly(const char *str)
{
    if (str[0] == '-') {
        for (size_t i = 1; i < strlen(str); i++) {
            if (!isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }
    if (isdigit(str[0])) {
        for (size_t i = 0; i < strlen(str); i++) {
            if (!isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }
    return false;
}

static bool doesStringRepresentNonCoeffPoly(const char *str)
{
    size_t str_len = strlen(str);
    if (str[0] != '(' || ((str[0] == '(') && str[str_len - 1] != ')')) {
        return false;
    }
    bool // flagi poprawności notacji
    minus = true, digit = true, open_par = true, plus = false, comma = false, close_par = false, coeff = true;

    int pars = 0;
    size_t i = 0;
    while (i < str_len) {
        if (str[i] == '-' && minus) {
            minus = false; open_par = false; plus = false; comma = false; close_par = false; digit = true; coeff = true;
            i++;
        }
        else if (isdigit(str[i]) && digit) {
            if (coeff) {
                while (isdigit(str[i])) {
                    i++;
                }
                minus = false; digit = false; open_par = false; plus = false; close_par = false; comma = true; coeff = false;
            }
            else {
                while (isdigit(str[i])) {
                    i++;
                }
                minus = false; digit = false; open_par = false; plus = false; comma = false;
                close_par = true; coeff = true;
            }
        }
        else if (str[i] == '(' && open_par) {
            pars++;
            plus = false; comma = false; close_par = false; minus = true; digit = true; open_par = true; coeff = true;
            i++;
        }
        else if (str[i] == '+' && plus) {
            minus = false; digit = false; plus = false; comma = false; close_par = false; open_par = true;
            i++;
        }
        else if (str[i] == ',' && comma) {
            minus = false;open_par = false;plus = false;comma = false; close_par = false; digit = true; coeff = false;
            i++;
        }
        else if (str[i] == ')' && close_par) {
            pars--; // dopasowano nawias
            if (pars < 0) {
                return false; // znaleziono zamykajacy bez otwierajacego na stosie
            }
            minus = false; open_par = false; plus = true; digit = true; comma = pars > 0; close_par = !comma;
            i++;
        }
        else return false;
    }
    return (pars == 0);
}

static poly_errcode_t parseCoeffPoly(Poly *p, const char *str, const size_t start_idx, const size_t end_idx)
{
    poly_coeff_t sign = 1;
    size_t new_start = start_idx;

    if (str[start_idx] == '-') {
        sign = -1;
        new_start++;
    }
    if (isdigit(str[new_start])) {
        size_t buff_len = 0;    // zliczenie długości buforu współczynnika
        for (size_t i = 0; isdigit(str[new_start + i]) && i <= end_idx; i++) {
            buff_len++;
        }
        char *preprocessedCoeff = safeCalloc(buff_len + 1, sizeof(char));

        for (size_t i = 0; i < buff_len; i++) {
            preprocessedCoeff[i] = str[new_start + i];    // zapisywanie cyfr do buforu współczynnika
        }
        char *endptr = NULL;    // konwersja i przypisanie współczynnika wielomianowi
        errno = 0;
        poly_coeff_t coeff = strtol(preprocessedCoeff, &endptr, 10);
        free(preprocessedCoeff);

        if ((coeff == 0 && (errno != 0 || endptr == preprocessedCoeff)) || !IS_VALID_COEFF(coeff)) {
            return POLY_ERR;    // input nie reprezentuje liczby lub overflow/underflow
        }
        else {
            *p = PolyFromCoeff(coeff * sign);
            return POLY_OK;
        }
    }
    return POLY_ERR;
}

static poly_errcode_t parseNonCoeffPoly(Poly *p, const char *str, const size_t start_idx, const size_t end_idx)
{
    size_t depth = 0, monos_count = 1;

    for (size_t i = start_idx; i <= end_idx; i++) {
        if (str[i] == '+' && depth == 0) {    // zliczanie jaki jest rozmiar tablicy jednomianów
            monos_count++;
        }
        else if (str[i] == '(') {
            depth++;
        }
        else if (str[i] == ')') {
            depth--;
        }
    }
    Mono *monosArr = safeMalloc(monos_count * sizeof(Mono));

    // znajdowanie i parsowanie jednomianów oraz przypisywanie ich wielomianowi
    size_t mono_start_idx, mono_end_idx, k = 0; depth = 0;
    for (size_t i = start_idx; i <= end_idx; i++) {
        if (str[i] == '(') {
            if (depth == 0) {
                mono_start_idx = i + 1;
            }
            depth++;
        }
        else if (str[i] == ')') {
            depth--;
            if (depth == 0) {
                mono_end_idx = i - 1;
                if (parseMonoRecursive(monosArr + k, str, mono_start_idx, mono_end_idx) != POLY_OK) {
                    free(monosArr);
                    return POLY_ERR;
                }
                k++;
            }
        }
    }
    *p = PolyAddMonos(monos_count, monosArr);
    free(monosArr);
    return POLY_OK;
}

static poly_errcode_t parsePolyRecursive(Poly *p, const char *str, const size_t start_idx, const size_t end_idx)
{
    if (str[start_idx] == '-' || isdigit(str[start_idx])) {
        return parseCoeffPoly(p, str, start_idx, end_idx);
    }
    else if (str[start_idx] == '(') {
        return parseNonCoeffPoly(p, str, start_idx, end_idx);
    }
    return POLY_ERR;
}

static poly_errcode_t parseMonoRecursive(Mono *m, const char *str, const size_t start_idx, const size_t end_idx)
{
    size_t comma_index = end_idx;
    while (str[comma_index] != ',') {
        comma_index--;        // wyszukanie ostatniego przecinka od końca
    }

    char *preprocessedExp = safeCalloc((end_idx - comma_index + 1), sizeof(char));
    for (size_t k = 1; isdigit(str[comma_index + k]); k++) {
        preprocessedExp[k - 1] = str[comma_index + k];    // zapisywanie zawartości stringa po przecinku do buforu wykładnika
    }

    char *endptr = NULL;    //konwersja i przypisanie wykładnika
    errno = 0;
    long exp = strtol(preprocessedExp, &endptr, 10);
    free(preprocessedExp);

    if ((exp == 0 && (errno != 0 || endptr == preprocessedExp))|| !IS_VALID_EXP(exp)) {
        // input nie reprezentuje liczby lub overflow/underflow
        return POLY_ERR;
    }
    else {
        m->exp = (poly_exp_t)exp;
    }

    if (parsePolyRecursive(&m->p, str, start_idx, comma_index - 1) != POLY_OK) {
        return POLY_ERR;    //konwersja i przypisanie jednomianu
    }
    return POLY_OK;
}



void detectLineType(Line *line, const size_t index, char *str, const long long nread)
{
    line->index = index;

    if (str == NULL || strcmp(str, "\n") == 0 || str[0] == '#') {
        line->type = IGNORED;
        return;
    }
    if (isalpha(str[0])) {
        if ((size_t)nread != strlen(str)) {
            line->type = IGNORED;
            fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line->index);
        }
        else {
            line->type = CMD_LINE;
            str[strcspn(str, "\n")] = '\0';
        }
    }
    else {
        if ((size_t)nread != strlen(str)) {
            line->type = IGNORED;    // wiersz zawiera za dużo znaków '\0'
            fprintf(stderr, "ERROR %zu WRONG POLY\n", line->index);
        }
        else {
            line->type = POLY_LINE;
            str[strcspn(str, "\n")] = '\0';
        }
    }
}

poly_errcode_t parsePoly(Poly *p, const char *str, const Line line)
{
    if (!doesStringRepresentPoly(str)) {
        fprintf(stderr, "ERROR %zu WRONG POLY\n", line.index);
        return POLY_ERR;
    }
    if (parsePolyRecursive(p, str, 0, strlen(str) - 1) == POLY_OK) {
        if (!PolyIsCoeff(p)) {
            Mono monoArr[p->size];
            memcpy(monoArr, p->arr, p->size * sizeof(Mono));
            free(p->arr);
            *p = PolyAddMonos(p->size, monoArr);
        }
        return POLY_OK;
    }
    else {
        fprintf(stderr, "ERROR %zu WRONG POLY\n", line.index);
        return POLY_ERR;
    }
}