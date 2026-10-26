#define PLUGIN_EXPORTS
#include "PluginInterface.h"
#include <cmath>
#include <cstring>
#include <string>

extern "C" {

PLUGIN_API const char* get_plugin_name(void) {
    return "sqrt";
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
    return "Square root function (e.g. sqrt(16) = 4)";
}

PLUGIN_API int calculate(const double* args, int num_args, double* out_result, char* err_buf, int err_buf_size) {
    if (!args || num_args != 1 || !out_result) {
        if (err_buf && err_buf_size > 0) {
            std::strncpy(err_buf, "sqrt expects exactly 1 argument", err_buf_size - 1);
            err_buf[err_buf_size - 1] = '\0';
        }
        return PLUGIN_ERROR;
    }

    double x = args[0];

    if (x < 0.0) {
        if (err_buf && err_buf_size > 0) {
            std::string msg = "sqrt: argument cannot be negative (got " + std::to_string(x) + ")";
            std::strncpy(err_buf, msg.c_str(), err_buf_size - 1);
            err_buf[err_buf_size - 1] = '\0';
        }
        return PLUGIN_ERROR;
    }

    *out_result = std::sqrt(x);
    return PLUGIN_SUCCESS;
}

} // extern "C"
