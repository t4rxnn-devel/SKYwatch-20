#pragma once
#include <stdbool>

#ifdef __cplusplus
extern "C" {
#endif

bool evaluate_loss_of_separation(
    double ac1_x, double ac1_y, double ac1_alt,
    double ac2_x, double ac2_y, double ac2_alt,
    double min_horizontal_nm,
    double min_vertical_ft
);

#ifdef __cplusplus
}
#endif
