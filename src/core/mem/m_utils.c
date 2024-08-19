// Copyright (C) by gimura 2024
// --- PREFIX ---
//
// M_* - memory functions
//
// --- DESCRIPTION ---
//
// memory utils
//
// -------------------

#include "../../cl_def.h"

// reading size from string
// returns the size in bytes
usize M_ReadMemSize(const char* size_str) {
    char  type = '\0';
    usize size = 0;

    usize return_size = ULONG_MAX;

    if (sscanf(
        size_str,
        "%zu%c",
        &size, &type
    ) != 2) goto fail;

    switch (type) {
        case 'B': // bytes
            return_size = size;
            goto done;

        case 'K': // kilobytes
            return_size = KB(size);
            goto done;

        case 'M': // megabytes
            return_size = MB(size);
            goto done;

        case 'G': // gigabytes
            return_size = GB(size);
            goto done;

        default:
#           ifdef PARANOID
            printf("M_ReadMemSize: w-why d-didn't you s-specify the f-format o_0 (perceived as bytes)\n");
#           endif

            goto done;
    }

    done:
        if (return_size == ULONG_MAX) goto fail;
        return return_size;

    fail: ERROR("M_ReadMemSize: invalid memory format");
}