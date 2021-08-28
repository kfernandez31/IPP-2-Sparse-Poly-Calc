#include "helper_funcs.h"

Mono M(Poly p, poly_exp_t n) {
    return MonoFromPoly(&p, n);
}

Poly MakePolyHelper(poly_exp_t dummy, ...) {
    va_list list;
    va_start(list, dummy);
    size_t count = 0;
    while (true) {
        va_arg(list, Poly);
        if (va_arg(list, poly_exp_t) < 0)
            break;
        count++;
    }
    va_start(list, dummy);
    Mono *arr = safeCalloc(count, sizeof(Mono));
    CHECK_POINTER(arr);
    for (size_t i = 0; i < count; ++i) {
        Poly p = va_arg(list, Poly);
        arr[i] = MonoFromPoly(&p, va_arg(list, poly_exp_t));
        assert(i == 0 || MonoGetExp(&arr[i]) > MonoGetExp(&arr[i - 1]));
    }
    va_end(list);
    Poly res = PolyAddMonos(count, arr);
    free(arr);
    return res;
}

bool TestOp(Poly a, Poly b, Poly res,
            Poly (*op)(const Poly *, const Poly *)) {
    Poly c = op(&a, &b);
    bool is_eq = PolyIsEq(&c, &res);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    PolyDestroy(&res);
    return is_eq;
}

bool TestAdd(Poly a, Poly b, Poly res) {
    return TestOp(a, b, res, PolyAdd);
}

bool TestAddMonos(size_t count, Mono monos[], Poly res) {
    Poly b = PolyAddMonos(count, monos);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}

bool TestMul(Poly a, Poly b, Poly res) {
    return TestOp(a, b, res, PolyMul);
}

bool TestSub(Poly a, Poly b, Poly res) {
    return TestOp(a, b, res, PolySub);
}

bool TestDegBy(Poly a, size_t var_idx, poly_exp_t res) {
    bool is_eq = PolyDegBy(&a, var_idx) == res;
    PolyDestroy(&a);
    return is_eq;
}

bool TestDeg(Poly a, poly_exp_t res) {
    bool is_eq = PolyDeg(&a) == res;
    PolyDestroy(&a);
    return is_eq;
}

bool TestEq(Poly a, Poly b, bool res) {
    bool is_eq = PolyIsEq(&a, &b) == res;
    PolyDestroy(&a);
    PolyDestroy(&b);
    return is_eq;
}

bool TestAt(Poly a, poly_coeff_t x, Poly res) {
    Poly b = PolyAt(&a, x);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}
