#pragma once

#include "das_config.h"
THIRD_PARTY_INCLUDES_START
#include "daScript/daScript.h"
THIRD_PARTY_INCLUDES_END

#include "using_das.inc"

namespace das {
    class Module_Unreal : public Module {
    public:
        Module_Unreal();
        virtual bool initDependencies() override;
    };
};