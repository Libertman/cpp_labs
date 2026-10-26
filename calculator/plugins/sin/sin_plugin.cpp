#define PLUGIN_EXPORTS
#include "PluginInterface.h"
#include <cmath>
#include <cstring>
#include <numbers>

namespace {
constexpr double PI = 3.14159265358979323846;
}

extern "C" {

PLUGIN_API const char* get_plugin_name(void) {
    return "sin";
}

PLUGIN_API int get_plugin_type(void) {
    return PLUGIN_TYPE_UNARY_FUNC;
}

PLUGIN_API int get_plugin_arity(void) {
    return 1;
}

PLUGIN_API int get_plugin_precedence(void) {
    return 0;
}

PLUGIN_API int get_plugin_associativity(void) {
    return 0;
}

PLUGIN_API const char* get_plugin_description(void) {
    return "Sine function in degrees (e.g. sin(90) = 1)";
}

PLUGIN_API int calculate(const double* args, int num_args, double* out_result, char* err_buf, int err_buf_size) {
    if (!args || num_args != 1 || !out_result) {
        if (err_buf && err_buf_size > 0) {
            std::strncpy(err_buf, "sin expects exactly 1 argument", err_buf_size - 1);
            err_buf[err_buf_size - 1] = '\0';
        }
        return PLUGIN_ERROR;
    }

    double deg = args[0];

    // Нормализация угла в градусах для точных значений
    double normDeg = std::fmod(deg, 360.0);
    if (normDeg < 0) {
        normDeg += 360.0;
    }

    if (normDeg == 0.0 || normDeg == 180.0 || normDeg == 360.0) {
        *out_result = 0.0;
    } else if (normDeg == 90.0) {
        *out_result = 1.0;
    } else if (normDeg == 270.0) {
        *out_result = -1.0;
    } else {
        double rad = deg * (PI / 180.0);
        *out_result = std::sin(rad);
    }

    return PLUGIN_SUCCESS;
}

} // extern "C"
