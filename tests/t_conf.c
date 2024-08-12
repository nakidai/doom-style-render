#include "cl_def.h"

static void T_ConfVariable(void) {
#   define VAR_CONF_ERR(x, y, z) ({ \
        char* __x = C_GetVariable(x, y); \
        char* __y = M_TempAlloc(strlen(z)); \
        memcpy(__y, z, strlen(z)); \
        if (strcmp(__x, __y) != 0) ERROR("T_ConfVariable: C_GetVariable error"); \
        M_TempFree(__x); \
        M_TempFree(__y); \
    })

    VAR_CONF_ERR("$var value;", "var", "value");
    VAR_CONF_ERR("$long_variable_name value;", "long_variable_name", "value");
    VAR_CONF_ERR("$var ultra_super_duper_extra_long_variable_value;", "var", "ultra_super_duper_extra_long_variable_value");

#   undef VAR_CONF_ERR
}

// static void T_AssertConfArray(const char* source, const char* name, conf_array_t array) {
//     conf_array_t out = C_GetArray(source, name);

//     if (out.s != array.s) ERROR("T_AssertConfArray: C_GetArray error");

//     for (usize i = 0; i < out.s; i++) {
//         if (strcmp(out.d[i], array.d[i]) == 0) M_TempFree(out.d[i]);
//         else                                   ERROR("T_AssertConfArray: C_GetArray error");
//     }
// }

// static void T_ConfArray(void) {
//     conf_array_t array = {};
//     array.d = { "test", "test2", "test3" };
//     array.s = 3;

//     const char* src = "*arr test;\n*arr test2;\n*arr test3;";

//     T_AssertConfArray(src, "arr", array);
// }

int main() {
    M_Init();

    T_ConfVariable();
    // T_ConfArray();

    M_Free();

    return 0;
}