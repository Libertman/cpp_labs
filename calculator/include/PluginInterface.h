#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef PLUGIN_EXPORTS
        #define PLUGIN_API __declspec(dllexport)
    #else
        #define PLUGIN_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define PLUGIN_API __attribute__((visibility("default")))
    #else
        #define PLUGIN_API
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum PluginType {
    PLUGIN_TYPE_UNARY_FUNC = 0,    // sin(x), ln(x)
    PLUGIN_TYPE_BINARY_OP = 1,     // a ^ b
    PLUGIN_TYPE_BINARY_FUNC = 2    // pow(x, y)
};

enum PluginStatus {
    PLUGIN_SUCCESS = 0,
    PLUGIN_ERROR = 1
};

PLUGIN_API const char* get_plugin_name(void);
PLUGIN_API int get_plugin_type(void);
PLUGIN_API int get_plugin_arity(void);
PLUGIN_API int get_plugin_precedence(void);
PLUGIN_API int get_plugin_associativity(void);
PLUGIN_API const char* get_plugin_description(void);

// Функция вычисления: возвращает PLUGIN_SUCCESS (0) или PLUGIN_ERROR (1)
PLUGIN_API int calculate(const double* args, int num_args, double* out_result, char* err_buf, int err_buf_size);

#ifdef __cplusplus
}
#endif
