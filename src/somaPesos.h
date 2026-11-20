#ifndef SOMA_PESOS_H
#define SOMA_PESOS_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace detalhes {
// Os chamadores descartam infinito antes da soma. Operandos do mesmo sinal
// usam os modulos para detectar inclusive excesso que arredondaria para DBL_MAX.
inline double somarPesos(double a, double b) {
    if ((a > 0 && b > 0) || (a < 0 && b < 0)) {
        const double maior = std::max(std::abs(a), std::abs(b));
        const double menor = std::min(std::abs(a), std::abs(b));
        if (menor > std::numeric_limits<double>::max() - maior) {
            throw std::overflow_error("Soma dos pesos excede o intervalo finito de double");
        }
    }
    const double soma = a + b;
    if (!std::isfinite(soma)) {
        throw std::overflow_error("Soma dos pesos excede o intervalo finito de double");
    }
    return soma;
}
} // namespace detalhes

#endif
