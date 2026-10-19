#pragma once
#include "DasExecutionRole.h"
class FDasPhysicsExecutionScope final
{
    EDasExecutionRole Previous;

public:
    FDasPhysicsExecutionScope();
    ~FDasPhysicsExecutionScope();
    FDasPhysicsExecutionScope(const FDasPhysicsExecutionScope&) = delete;
};
