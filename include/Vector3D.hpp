#pragma once
#include <cmath>

struct Vector3D {
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    [[nodiscard]] double distance_to(const Vector3D& other) const noexcept {
        return std::sqrt(std::pow(x - other.x, 2) + 
                         std::pow(y - other.y, 2) + 
                         std::pow(z - other.z, 2));
    }

    [[nodiscard]] Vector3D operator+(const Vector3D& other) const noexcept {
        return {x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] Vector3D operator-(const Vector3D& other) const noexcept {
        return {x - other.x, y - other.y, z - other.z};
    }

    [[nodiscard]] Vector3D operator*(double scalar) const noexcept {
        return {x * scalar, y * scalar, z * scalar};
    }

    [[nodiscard]] double dot(const Vector3D& other) const noexcept {
        return (x * other.x) + (y * other.y) + (z * other.z);
    }
};
