#ifndef POINTY_INTERFACES_H
#define POINTY_INTERFACES_H

#include "pointy.h"
#include "Interface/GlobalInterface.h"

namespace Interfaces {
    static void Register(pointy::PointyContext *P) {
        P->RegisterInterface<GlobalInterface>();
    }
}

#endif //POINTY_INTERFACES_H
