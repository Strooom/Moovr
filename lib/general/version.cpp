#include "version.h"
#include <cstdio>

version::version(uint32_t a, uint32_t b, uint32_t c) : mainVersion(a), subVersion(b), minorVersion(c) {
    snprintf(versionString, version::maxVersionStringLenght, "V %d.%d.%d", mainVersion, subVersion, minorVersion);
}

const char* version::toString() {
    return versionString;
}