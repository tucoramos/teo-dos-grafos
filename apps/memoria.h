#ifndef MEMORIA_H
#define MEMORIA_H

#include <windows.h>
#include <psapi.h>

// memoria fisica usada pelo processo, em MB
double memoriaMB() {
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    return info.WorkingSetSize / (1024.0 * 1024.0);
}

#endif