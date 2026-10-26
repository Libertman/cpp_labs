#define PLUGIN_EXPORTS
#include "PluginInterface.h"
#include <cmath>
#include <cstring>
#include <string>

extern "C" {

PLUGIN_API const char* get_plugin_name(void) {
    return "^";
}

PLUGIN_API int get_plugin_type(void) {
    return PLUGIN_TYPE_BINARY_OP;
}

PLUGIN_API int get_plugin_arity(void) {
    return 2;
}

PLUGIN_API int get_plugin_precedence(void) {
    return 3; // Выше умножения (*) и деления (/), у которых приоритет 2
}

PLUGIN_API int get_plugin_associativity(void) {
    return 1; // 1 = правосторонняя ассоциативность (2^3^2 = 2^(3^2) = 512)
}

PLUGIN_API const char* get_plugin_description(void) {
    return "Exponentiation binary operator ^ (e.g. 2^4 = 16)";
}

PLUGIN_API int calculate(const double* args, int num_args, double* out_result, char* err_buf, int err_buf_size) {
    if (!args || num_args != 2 || !out_result) {
        if (err_buf && err_buf_size > 0) {
            std::strncpy(err_buf, "^ operator expects exactly 2 operands", err_buf_size - 1);
            err_buf[err_buf_size - 1] = '\0';
        }
        return PLUGIN_ERROR;
    }

    double base = args[0];
    double exponent = args[1];

    if (base == 0.0 && exponent < 0.0) {
        if (err_buf && err_buf_size > 0) {
            std::strncpy(err_buf, "Division by zero: 0 raised to negative power", err_buf_size - 1);
            err_buf[err_buf_size - 1] = '\0';
        }
        return PLUGIN_ERROR;
    }

    if (base < 0.0 && std::floor(exponent) != exponent) {
        if (err_buf && err_buf_size > 0) {
            std::strncpy(err_buf, "Invalid exponentiation: negative base with non-integer power", err_buf_size - 1);
            err_buf[err_buf_size - 1] = '\0';
        }
        return PLUGIN_ERROR;
    }

    *out_result = std::pow(base, exponent);
    return PLUGIN_SUCCESS;
}

} // extern "C"
