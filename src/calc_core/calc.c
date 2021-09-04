/** @file
  Interfejs menu do kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "calc.h"

/**
 * Tablica asocjacyjna komend z ich kodami.
 */
static command_pair_t commandList[] = {
        {"ZERO", CalcZero},
        {"IS_COEFF", CalcIsCoeff},
        {"IS_ZERO", CalcIsZero},
        {"CLONE", CalcClone},
        {"ADD", CalcAdd},
        {"MUL", CalcMul},
        {"NEG", CalcNeg},
        {"SUB", CalcSub},
        {"IS_EQ", CalcIsEq},
        {"DEG", CalcDeg},
        {"DEG_BY", CalcDegBy},
        {"AT", CalcAt},
        {"PRINT", CalcPrint},
        {"POP", CalcPop},
        {"COMPOSE", CalcCompose},
    };

/**
 * Wykonuje komendę zadaną kodem w linii i w przypadku błędu,
 * wypisuje go na wyjście diagnostyczne wraz z indeksem linii.
 *
 * @param[in, out] calc : kalkulator
 * @param[in] line : linia
 */
static void execCommand(Calculator *calc, const Line line);

/**
 * Znajduje w menu komend kod, jakiemu odpowiada komenda
 * reprezentowana przez @p cmd. W przypadku błędnej komendy,
 * wypisuje błąd na wyjście diagnostyczne wraz z indeksem linii.
 *
 * @param[in] cmd : komenda jako string
 * @param[in, out] line : linia
 *
 * @return : kod komendy
 */
static cmd_errcode_t getCommandCode(const char *cmd, Line *line);

/**
 * Przetwarza linię zawierającą tekstową reprezentację wielomianu.
 * Jeśli wielomian utworzono pomyślnie, dodaje go do stosu kalkulatora,
 * w przeciwnym wypadku wypisuje błąd na standardowe wyjście diagnostyczne.
 *
 * @param[in, out] menu : menu kalkulatora, z dostępem do stosu wielomianów
 * @param[in] line : struktura linii, wraz z jej indeksem
 * @param[in] str : linia przed obróbką, reprezentująca wielomian
 */
static void parseAndPushPoly(Menu *menu, Line line, const char *str);

/**
 * Przetwarza linię zawierającą tekstową reprezentację komendy.
 * Jeśli wpisano prawidłowo komendę, przekazuje ją do kalkulatora wraz
 * z jej argumentem, i wykonuję ją. W wypadku błędnych danych,
 * wypisuje błąd na standardowe wyjście diagnostyczne.
 *
 * @param[in, out] menu : menu kalkulatora, z dostępem do aktualnego argumentu
 * @param[in] line : struktura linii, wraz z jej indeksem
 * @param[in] str : linia przed obróbką, reprezentująca komendę
 */
static void parseAndExecCommand(Menu *menu, Line line, const char *str);


void run(Menu* menu)
{
    size_t line_index = 1, len;
    char *lineptr = NULL;
    long long nread;

    CalcInit(&menu->calc);

    errno = 0;
    while ( (nread = getline(&lineptr, &len, stdin)) != -1 ) {
        CHECK_POINTER(lineptr);
        Line line;

        detectLineType(&line, line_index++, lineptr, nread);

        if (line.type == IGNORED) {
            continue;
        }
        else if (line.type == POLY_LINE) {
            parseAndPushPoly(menu, line, lineptr);
        }
        else if (line.type == CMD_LINE) {
            parseAndExecCommand(menu, line, lineptr);
        }
    }
    if (lineptr != NULL) {
        free(lineptr);
    }
    if (nread == -1 && errno == ENOMEM) {
        exit(EXIT_FAILURE);
    }

    CalcDestroy(&menu->calc);
}



static cmd_errcode_t getCommandCode(const char *cmd, Line *line)
{
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(commandList[i].name, cmd) == 0) {
            line->contents.cmd = (CommandCode)i;
            return CMD_OK;
        }
    }
    fprintf(stderr, "ERROR %zu WRONG COMMAND\n", line->index);
    return CMD_WRONG_COMMAND;
}

static void parseAndPushPoly(Menu *menu, Line line, const char *str)
{
    if (parsePoly(&line.contents.poly, str, line) != POLY_ERR) {
        if (VectorPush(menu->calc.polyStack, &line.contents.poly) != VECT_OK) {
            exit(EXIT_FAILURE);
        }
    }
}

static void parseAndExecCommand(Menu *menu, Line line, const char *str)
{
    size_t lineptr_len = strlen(str);
    char *cmd_name = NULL, *arg = NULL;

    int separator_index = -1;    // znalezienie pierwszego wystąpienia nie-litery
    for (int i = 0; i < (int)lineptr_len; i++) {
        if (isspace(str[i])) {
            separator_index = i;
            break;
        }
    }

    if (separator_index == -1) {
        cmd_name = safeCalloc(lineptr_len + 1, sizeof(char));
        memcpy(cmd_name, str, lineptr_len * sizeof(char)); // nazwa komendy - cała linia, argument - nic
    }
    else {
        cmd_name = safeCalloc(separator_index + 1, sizeof(char));
        arg = safeCalloc(lineptr_len - separator_index + 1, sizeof(char));
        memcpy(cmd_name, str, separator_index * sizeof(char)); // nazwa komendy - linia aż do separator_index
        memcpy(arg, str + separator_index, (lineptr_len - separator_index + 1) * sizeof(char)); // argument - reszta linii
    }

    if (getCommandCode(cmd_name, &line) != CMD_WRONG_COMMAND) {
        if (CalcParseArg(&menu->calc, line, arg) != CMD_INVALID_ARG) {
            execCommand(&menu->calc, line);
        }
    }
    free(cmd_name);
    free(arg);
}

static void execCommand(Calculator *calc, const Line line)
{
    if ((*commandList[line.contents.cmd].func)(calc) == CMD_STACK_UNDERFLOW) {
        fprintf(stderr, "ERROR %zu STACK UNDERFLOW\n", line.index);
    }
}


/**
 * Inicjalizuje kalkulator poprzez menu, wczytuje wielomiany i komendy,
 * na bieżąco zwracając żądany output lub wypisując błędy w przypadku
 * nieprawidłowych danych.
 *
 * @return : program zakończony pomyślnie
 */
int main() {
    Menu menu;
    run(&menu);

    exit(EXIT_SUCCESS);
}
