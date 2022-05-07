#include "hostinterfaceerror.h"

namespace hostinterface {

const char* toString(error theError) {
    switch (theError) {
        case error::none:
            return "none";
            break;
        case error::underflow:
            return "underflow";
            break;
        case error::overflow:
            return "overflow";
            break;
        default:
            return "unknown";
    }
}

}        // namespace hostinterface