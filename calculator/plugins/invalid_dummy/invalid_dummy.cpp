#if defined(_WIN32) || defined(__CYGWIN__)
    #define DUMMY_API __declspec(dllexport)
#else
    #define DUMMY_API __attribute__((visibility("default")))
#endif

extern "C" {

// Экспортирует постороннюю функцию, но не реализует get_plugin_name и calculate
DUMMY_API int unrelated_function(void) {
    return 42;
}

}
