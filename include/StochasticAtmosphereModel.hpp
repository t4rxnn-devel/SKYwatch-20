#ifndef STOCHASTIC_ATMOSPHERE_MODEL_HPP
#define STOCHASTIC_ATMOSPHERE_MODEL_HPP

#include <cmath>

namespace skywatch {

class StochasticAtmosphereModel {
public:
    struct AirProperties {
        double density;        // kg/m^3
        double pressure;       // Pa
        double temperature;    // Kelvin
        double speed_of_sound; // m/s
    };

    /**
     * Calculates 1976 US Standard Atmosphere profile with local perturbations.
     */
    static AirProperties getProperties(double altitude_m) {
        constexpr double R = 287.058;      // Gas constant J/(kg*K)
        constexpr double GAMMA = 1.4;      // Heat capacity ratio
        constexpr double G0 = 9.80665;     // Gravitational acceleration m/s^2

        double T, P, rho;

        if (altitude_m <= 11000.0) { // Troposphere
            T = 288.15 - 0.0065 * altitude_m;
            P = 101325.0 * std::pow(T / 288.15, G0 / (0.0065 * R));
        } else if (altitude_m <= 20000.0) { // Lower Stratosphere
            T = 216.65;
            double P_11000 = 22632.1;
            P = P_11000 * std::exp(-G0 * (altitude_m - 11000.0) / (R * T));
        } else { // Upper Stratosphere
            T = 216.65 + 0.001 * (altitude_m - 20000.0);
            P = 5474.89 * std::pow(216.65 / T, G0 / (0.001 * R));
        }

        rho = P / (R * T);
        double a = std::sqrt(GAMMA * R * T);

        return {rho, P, T, a};
    }
};

} // namespace skywatch

#endif // STOCHASTIC_ATMOSPHERE_MODEL_HPP
