/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych

  @authors Jakub Pawlewicz <pan@mimuw.edu.pl>, Marcin Peczarski <marpe@mimuw.edu.pl>,
  Kacper Kramarz-Fernandez <k.kramarzfer@student.uw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly.h"

/**
 * Konwencja przyjęta w treści zadania.
 */
#define DEG_OF_ZERO -1

/**
 * Wykładnik, jaki posiada współczynnik gdy jest elementem tablicy
 * jednomianów.
 */
#define EXP_OF_COEFF 0

/**
 * Generyczne maksimum
 * @param[in] a : obiekt do porównania
 * @param[in] b : obiekt do porównania
 * @return : maksimum z dwóch obiektów
 */
#define MAX(a,b) (a >= b)? a : b



/**
 * Funkcja porównująca dla jednomianów, dokonuje porównania
 * wykładników jednomianów dla sortowania w kolejności malejącej.
 *
 * @param[in] a : jednomian
 * @param[in] b : jednomian
 *
 * @return Porównanie wykładników
 */
static int MonoCompDescending(const void *a, const void *b);

/**
 * Sprawdza, czy wielomian jest tożsamościowo równy jedynce.
 *
 * @param[in] p : wielomian
 *
 * @return Czy wielomian jest równy jedynce?
 */
static inline bool PolyIsOne(const Poly *p);

/**
 * Zwraca stopień jednomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru). Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego jednomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 *
 * @param[in] m : jednomian
 * @param[in] var_idx : indeks zmiennej
 *
 * @return stopień wielomianu @p m z względu na zmienną o indeksie @p var_idx
 */
static poly_exp_t MonoDegBy(const Mono *m, size_t var_idx);

/**
 * Zwraca stopień jednomianu.
 *
 * @param[in] m : jednomian
 *
 * @return stopień jednomianu @p m
 */
static poly_exp_t MonoDeg(const Mono *m);

/**
 * Sprawdza równość dwóch jednomianów.
 *
 * @param[in] m1 : jednomian
 * @param[in] m2 : jednomian
 *
 * @return Czy m1 równa się m2
 */
static bool MonoIsEq(const Mono *m1, const Mono *m2);

/**
 * Sprawdza czy wielomian jest postaci x_0^0*c
 *
 * @param[in] p : wielomian
 *
 * @return : czy wielomian reprezentuje współczynnik
 * w prostej postaci "łańcuchowej"
 */
static inline bool PolyIsCoeffBetter(const Poly *p);

/**
 * Funkcja potęgująca dla liczb całkowitych.
 *
 * @param[in] base : podstawa
 * @param[in] exp : wykładnik
 *
 * @return podstawa^wykładnik
*/
static poly_coeff_t ipow(poly_coeff_t base, poly_exp_t exp);

/**
 * Lączy ze sobą wyrazy podobne obu wielomianów,
 * zwraca wielomian będący ich połączeniem. Zwalnia
 * pamięć obu argumentów.
 *
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 *
 * @return : wielomian stworzony w oparciu o dwa składowe
*/
static Poly PolyMerge(Poly *p, Poly *q);

/**
 * Tworzy posortowane połączenie dwóch wielomianów niebędących
 * współczynnikami poprzez iterowanie się po nich dwoma wskaźnikami.
 * Zwalnia pamięć tablic argumentów.
 *
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 *
 * @return : wielomian stworzony w oparciu o dwa składowe
*/
static Poly PolyMergingIntersect(Poly *p, Poly *q);

/**
 * Funkcja pomocnicza dla PolyAt(), bierze @p m->p,
 * mnoży przez przez x^{m->exp}, i opakowuje w strukturę
 * jednomianu o nieistotnym wykładniku (dummy value),
 * na potrzeby dalszego korzystania z PolyAddMonos().
 * @see PolyAt(), PolyAddMonos()
 *
 * @param[in] m : jednomian
 * @param[in] x : argument do podstawienia
 *
 * @return : wielomian stworzony w oparciu o dwa składowe
*/
static Mono MonoAt(const Mono *m, poly_coeff_t x);

/**
 * Zwraca oryginalny wielomian, jeśli nie jest on współczynnikiem,
 * lub wielomian współczynnikowy oparty o wejściowy, jeśli jest
 * on w prostej postaci łańcuchowej (x_0^0*coeff). W drugim przypadku
 * zwalnia pamięć 1-el. tablicy wielomianu.
 *
 * @param[in] p : wielomian
 *
 * @return : oryginalny wielomian, lub ukryty w nim współczynnik
*/
static Poly PolyExtractContents(Poly *p);

/**
 * Umieszcza w tablicy @p p->arr jednomiany z @p sourceMonos, ale w taki sposób,
 * aby znalazł się w niej tylko jeden o każdym występującym w niej wykładniku.
 * Wyrażenia o tych samych wykładnikach są merge'owane.
 *
 * @param[in] p : wielomian
 * @param[in] count : liczba jednomianów
 * @param[in] sourceMonos : tablica jednomianów
*/
static void PolySimplifyByMerging(Poly *p, size_t count, Mono *sourceMonos);

/**
 * Oczyszcza tablicę @p p->arr z zer i realokuje ją jeśli to konieczne.
 * W szczególnym przypadku gdy cała tablica jest złożona z zer, zamienia
 * @p p na wielomian tożsamościowo równy zeru.
 *
 * @param[in] p : wielomian
*/
static void PolyCleanFromZeros(Poly *p);

/**
 * Podnosi wielomian @p p do kwadratu.
 *
 * @param[in] p : wielomian
 *
 * @return : p^2
*/
static inline Poly PolySquare(Poly *p);

/**
 * Podnosi wielomian @p base do potęgi @p exp.
 * Korzysta z algorytmu szybkiego potęgowania.
 *
 * @param[in] base : podstawa (wielomian)
 * @param[in] exp : wykładnik
 *
 * @return : base^exp
*/
static Poly PolyPow(const Poly *base, poly_exp_t exp);

/**
 * Zwraca wyraz wolny wielomianu. W przypadku jego braku,
 * zwraca zero.
 *
 * @param[in] p : wielomian
 *
 * @return : wyraz wolny wielomianu
*/
static Poly PolyReturnConstantTerm(const Poly *p);



static inline bool PolyIsCoeffBetter(const Poly *p)
{
    return !PolyIsCoeff(p) && p->size == 1 && PolyIsCoeff(&p->arr[0].p) && (p->arr[0].exp == 0 || p->arr[0].p.coeff == 0);
}

static inline bool PolyIsOne(const Poly *p)
{
    return PolyIsCoeff(p) && p->coeff == 1;
}

static int MonoCompDescending(const void *a, const void *b)
{
    poly_exp_t exp_a = MonoGetExp((Mono *) a);
    poly_exp_t exp_b = MonoGetExp((Mono *) b);

    if (exp_a > exp_b) {
        return -1;
    }
    if (exp_a < exp_b) {
        return +1;
    }
    return 0;
}

static poly_exp_t MonoDegBy(const Mono *m, size_t var_idx)
{
    if (var_idx == 0) {
        return MonoGetExp(m);    // nie zagłębia się dalej niż stopień szukanej zmiennej
    }
    else {
        return PolyDegBy(&m->p, var_idx - 1);
    }
}

static poly_exp_t MonoDeg(const Mono *m)
{
    return m->exp + PolyDeg(&m->p);
}

static poly_coeff_t ipow(poly_coeff_t base, poly_exp_t exp)
{
    assert (exp >= 0);

    if (exp == 0 || base == 1) {
        return 1;
    }
    if (exp == 1) {
        return base;
    }

    poly_coeff_t temp = ipow(base, exp / 2);

    if (exp % 2 == 0) {
        return temp * temp;
    }
    else {
        return base * temp * temp;
    }
}

static Poly PolyMerge(Poly *p, Poly *q)
{
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyFromCoeff(p->coeff + q->coeff);
    }
    if (PolyIsCoeff(q)) {
        return PolyMerge(q, p);
    }

    Poly res;

    if (PolyIsCoeff(p)) {
        if (p->coeff == 0) { return *q; }

        res.size = q->size;

        if (MonoGetExp(&q->arr[q->size - 1]) == EXP_OF_COEFF) {
            Poly temp = PolyMerge(&q->arr[q->size - 1].p, p);

            if (!PolyIsZero(&temp)) {
                res.arr = safeMalloc(res.size * sizeof(Mono));
                res.arr[q->size - 1] = MonoFromPoly(&temp, EXP_OF_COEFF);
            }
            else {
                res.size -= 1;
                res.arr = safeMalloc(res.size * sizeof(Mono));
            }
            memcpy(res.arr, q->arr, (q->size - 1) * sizeof(Mono));
        }
        else {
            res.size += 1;
            res.arr = safeMalloc(res.size * sizeof(Mono));
            memcpy(res.arr, q->arr, q->size * sizeof(Mono));
            res.arr[res.size - 1] = MonoFromPoly(p, EXP_OF_COEFF);
        }
        free(q->arr);
    }
    else {
        res = PolyMergingIntersect(p, q);
    }
    return PolyExtractContents(&res);
}

static Poly PolyMergingIntersect(Poly *p, Poly *q)
{
    assert (!PolyIsCoeff(p) && !PolyIsCoeff(q));

    Poly res;
    res.size = p->size + q->size;
    res.arr = safeMalloc(res.size * sizeof(Mono));

    size_t new_size = res.size, i = 0, j = 0, k = 0;
    while (i < p->size || j < q->size) {
        if (i == p->size) {
            while (k < new_size && j < q->size) {   // dołączanie pozostałych jednomianów z q do res->arr
                res.arr[k++] = q->arr[j++];
            }
        }
        else if (j == q->size) {
            while (k < new_size && i < p->size) {   // dołączanie pozostałych jednomianów z p do res->arr
                res.arr[k++] = p->arr[i++];
            }
        }
        else if (MonoGetExp(&p->arr[i]) > MonoGetExp(&q->arr[j])) {
            res.arr[k++] = p->arr[i++];
        }
        else if (MonoGetExp(&p->arr[i]) < MonoGetExp(&q->arr[j])){
            res.arr[k++] = q->arr[j++];
        }
        else {
            new_size -= 1;
            Poly temp = PolyMerge(&p->arr[i].p, &q->arr[j].p);

            if (!PolyIsZero(&temp)) {
                res.arr[k++] = MonoFromPoly(&temp, MonoGetExp(&p->arr[i]));
            }
            else if (--new_size == 0) {
                free(res.arr);
                res = PolyZero();
            }
            i++; j++;
        }
    }
    if (new_size < res.size) {
        res.size = new_size;
        res.arr = safeRealloc(res.arr, res.size * sizeof(Mono));
    }
    free(p->arr);
    free(q->arr);

    return res;
}

static Mono MonoAt(const Mono *m, poly_coeff_t x)
{
    Poly c = PolyFromCoeff(ipow(x, m->exp));

    return (Mono) {
        .exp = -1,
        .p = PolyMul(&c, &m->p)
    };
}

static Poly PolyExtractContents(Poly *p)
{
    if (PolyIsCoeffBetter(p)) {
        poly_coeff_t coeff = p->arr[0].p.coeff;
        free(p->arr);
        return PolyFromCoeff(coeff);
    }
    else {
        return *p;
    }
}

static void PolySimplifyByMerging(Poly *p, size_t count, Mono *sourceMonos)
{
    assert (count > 0 && sourceMonos != NULL);

    qsort(sourceMonos, count, sizeof(Mono), MonoCompDescending);    // sortowanie aktualnej warstwy

    size_t size_after_merge = 1;
    p->arr[0] = sourceMonos[0];
    for (size_t i = 1; i < count; i++) {    // łączenie wyrażeń o tych samych wykładnikach
        if (!PolyIsZero(&sourceMonos[i].p)) {
            if (MonoGetExp(&p->arr[size_after_merge - 1]) == MonoGetExp(&sourceMonos[i])) {
                p->arr[size_after_merge - 1].p = PolyMerge(&p->arr[size_after_merge - 1].p, &sourceMonos[i].p);
            }
            else {
                p->arr[size_after_merge++] = sourceMonos[i];    // umieszczanie nowego wykładnika na nowy indeks
            }
        }
    }
    p->size = size_after_merge;
    PolyCleanFromZeros(p);
    *p = PolyExtractContents(p);
}

static void PolyCleanFromZeros(Poly *p)
{
    size_t size_wout_zeros = 0;
    for (size_t j = 0; j < p->size; j++) {
        if (!PolyIsZero(&p->arr[j].p)){
            p->arr[size_wout_zeros++] = p->arr[j];
        }
    }
    if (size_wout_zeros == 0) {
        free(p->arr);
        *p = PolyZero();
    }
    else {
        if (size_wout_zeros < p->size) {
            p->size = size_wout_zeros;
            p->arr = safeRealloc(p->arr, size_wout_zeros * sizeof(Mono));
        }
    }
}

static Poly PolyReturnConstantTerm(const Poly *p)
{
    if (p->arr[p->size - 1].exp == EXP_OF_COEFF && PolyIsCoeff(&p->arr[p->size - 1].p)) {
        return p->arr[p->size - 1].p;
    }
    else {
        return PolyZero();
    }
}


Mono MonoFromPoly(const Poly *p, poly_exp_t n) {
    assert(n == EXP_OF_COEFF || !PolyIsZero(p));

    return (Mono) {
        .p = *p,
        .exp = n
    };
}

void MonoDestroy(Mono *m)
{
    PolyDestroy(&m->p);
}

Mono MonoClone(const Mono *m)
{
    return (Mono) {
            .p = PolyClone(&m->p),
            .exp = m->exp
    };
}

void PolyDestroy(Poly *p)
{
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; i++) {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
        p->arr = NULL;
    }
}

Poly PolyClone(const Poly *p)
{
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->coeff);
    }
    else {
        Poly clone;
        clone.size = p->size;
        clone.arr = safeMalloc(clone.size * sizeof(Mono));

        for (size_t i = 0; i < p->size; i++) {
            clone.arr[i] = MonoClone(&p->arr[i]);
        }
        return clone;
    }
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    Poly prod;

    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return PolyFromCoeff(p->coeff * q->coeff);
    }
    if (PolyIsCoeff(p)) {
        if (PolyIsZero(p)) { return PolyZero(); }
        if (PolyIsOne(p)) { return PolyClone(q); }

        prod.size = q->size;
        prod.arr = safeMalloc(q->size * sizeof(Mono));

        for (size_t i = 0; i < q->size; i++) {
            prod.arr[i].p = PolyMul(p, &q->arr[i].p);
            prod.arr[i].exp = q->arr[i].exp;
        }
    }
    else if (PolyIsCoeff(q)) {
        return PolyMul(q, p);
    }
    else {
        Mono *mulMonos = safeMalloc(p->size * q->size * sizeof(Mono));

        for (size_t i = 0; i < p->size; i++) {
            for (size_t j = 0; j < q->size; j++) {
                mulMonos[i * q->size + j].exp = p->arr[i].exp + q->arr[j].exp;
                mulMonos[i * q->size + j].p = PolyMul(&p->arr[i].p, &q->arr[j].p);
            }
        }
        prod = PolyAddMonos(p->size * q->size, mulMonos);
        free(mulMonos);
    }
    return PolyExtractContents(&prod);
}

Poly PolyNeg(const Poly *p)
{
    Poly c = PolyFromCoeff(-1);
    return PolyMul(p, &c);
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    Poly p_copy = PolyClone(p);
    Poly q_copy = PolyClone(q);
    return PolyMerge(&p_copy, &q_copy);
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly p_copy = PolyClone(p);
    Poly q_neg = PolyNeg(q);
    return PolyMerge(&p_copy, &q_neg);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx)
{
    if (PolyIsCoeff(p)) {
        if (p->coeff == 0) {
            return DEG_OF_ZERO;
        }
        else {
            return EXP_OF_COEFF;
        }
    }
    else {
        poly_exp_t deg_by_idx = 0;
        for (size_t i = 0; i < p->size; i++) {
            deg_by_idx = MAX(deg_by_idx, MonoDegBy(&(p->arr[i]), var_idx));
        }
        return deg_by_idx;
    }
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (PolyIsCoeff(p)) {
        if (p->coeff == 0) {
            return DEG_OF_ZERO;
        }
        else {
            return EXP_OF_COEFF;
        }
    }
    else {
        poly_exp_t deg = 0;
        for (size_t i = 0; i < p->size; i++) {
            deg = MAX(deg, MonoDeg(&p->arr[i]));
        }
        return deg;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        return p->coeff == q->coeff;
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        return false;
    }
    else {
        if (p->size != q->size) {
            return false;
        }
        for (size_t i = 0; i < p->size; i++) {
            if (!MonoIsEq(&p->arr[i], &q->arr[i])) {
                return false;
            }
        }
        return true;
    }
}

bool MonoIsEq(const Mono *m1, const Mono *m2)
{
    return m1->exp == m2->exp && PolyIsEq(&m1->p, &m2->p);
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    if (PolyIsCoeff(p)) { return PolyClone(p); }

    Mono *monos = safeMalloc(p->size * sizeof(Mono));
    for (size_t i = 0; i < p->size; i++) {
        monos[i] = MonoAt(&p->arr[i], x);
    }
    Poly res = PolyAddMonos(p->size, monos);
    free(monos);

    if (PolyIsCoeff(&res)) {
        return res;
    }
    Poly temp = res.arr[0].p;
    free(res.arr);
    return temp;
}

Poly PolyAddMonos(size_t count, const Mono monos[])
{
    if (count == 0 || monos == NULL) { return PolyZero(); } // konwencja z zadania

    Poly p;
    p.arr = safeMalloc(count * sizeof(Mono));

    Mono* tempMonos = safeMalloc(count * sizeof(Mono));
    memcpy(tempMonos, monos, count * sizeof(Mono));

    PolySimplifyByMerging(&p, count, tempMonos);
    free(tempMonos);
    return p;
}

Poly PolyOwnMonos(size_t count, Mono *monos)
{
    if (monos == NULL) { return PolyZero(); } // konwencja z zadania

    if (count == 0) {
        free(monos);
        return PolyZero();
    }

    Poly p;
    p.arr = monos;

    PolySimplifyByMerging(&p, count, p.arr);
    return p;
}

Poly PolyCloneMonos(size_t count, const Mono monos[])
{
    if (count == 0 || monos == NULL) { return PolyZero(); } // konwencja z zadania

    Poly p;
    p.arr = safeMalloc(count * sizeof(Mono));

    Mono* tempMonos = safeMalloc(count * sizeof(Mono));
    for (size_t i = 0; i < count; i++) {
        tempMonos[i] = MonoClone(monos + i);
    }

    PolySimplifyByMerging(&p, count, tempMonos);
    free(tempMonos);
    return p;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[])
{
    if (PolyIsCoeff(p)) {
        return *p;
    }
    if (k == 0) {
        return PolyReturnConstantTerm(p);
    }
    Mono *resMonos = safeMalloc(p->size * sizeof(Mono));

    for (size_t i = 0; i < p->size; i++) {
        if (PolyIsCoeff(&p->arr[i].p) && p->arr[i].exp == 0) {
            resMonos[i].p = p->arr[i].p;
            resMonos[i].exp = -1;   // dummy value
        }
        else {
            Poly pow = PolyPow(q, p->arr[i].exp);    // "podstawienie" pod zmienną jednomianu
            Poly comp = PolyCompose(&p->arr[i].p, k - 1, q + 1);
            Poly mul = PolyMul(&pow, &comp);
            PolyDestroy(&pow);
            PolyDestroy(&comp);
            resMonos[i].p = mul;
            resMonos[i].exp = -1;   // dummy value
        }
    }

    Poly res = PolyOwnMonos(p->size, resMonos);
    if (PolyIsCoeff(&res)) {
        return res;
    }
    Poly temp = res.arr[0].p;
    free(res.arr);
    return temp;
}

static inline Poly PolySquare(Poly *p)
{
    return PolyMul(p, p);
}

static Poly PolyPow(const Poly *base, poly_exp_t exp)
{
    assert (exp >= 0);

    if (exp == 0 || PolyIsOne(base)) {
        return PolyFromCoeff(1);
    }
    if (exp == 1) {
        return PolyClone(base);
    }

    Poly temp = PolyPow(base, exp / 2);
    Poly square = PolySquare(&temp);

    if (exp % 2 == 0) {
        PolyDestroy(&temp);
        return square;
    }
    else {
        Poly mul = PolyMul(base, &square);
        PolyDestroy(&square);
        PolyDestroy(&temp);
        return mul;
    }
}

void PolyNegateCoeffs(Poly *p)
{
    if (PolyIsCoeff(p)) {
        p->coeff *= -1;
    }
    else {
        for (size_t i = 0; i < p->size; i++) {
            PolyNegateCoeffs(&p->arr[i].p);
        }
    }
}