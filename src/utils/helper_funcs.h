#ifndef __HELPER_FUNCS_H__
#define __HELPER_FUNCS_H__

#include <stdarg.h>
#include "../poly_core/poly.h"
#include "safe_allocations.h"

#define C PolyFromCoeff

Mono M(Poly p, poly_exp_t n);

Poly MakePolyHelper(poly_exp_t dummy, ...);

#define P(...) MakePolyHelper(0, __VA_ARGS__, PolyZero(), -1)

bool TestOp(Poly a, Poly b, Poly res,
            Poly (*op)(const Poly *, const Poly *));

bool TestAdd(Poly a, Poly b, Poly res);

bool TestAddMonos(size_t count, Mono monos[], Poly res);

bool TestMul(Poly a, Poly b, Poly res);

bool TestSub(Poly a, Poly b, Poly res);

bool TestDegBy(Poly a, size_t var_idx, poly_exp_t res);

bool TestDeg(Poly a, poly_exp_t res);

bool TestEq(Poly a, Poly b, bool res);

bool TestAt(Poly a, poly_coeff_t x, Poly res);

#endif //__HELPER_FUNCS_H__
