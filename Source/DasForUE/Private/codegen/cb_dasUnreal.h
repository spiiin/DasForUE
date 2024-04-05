#pragma once
#include "das_config.h"
THIRD_PARTY_INCLUDES_START
#include "daScript/misc/platform.h"
#include "daScript/ast/ast.h"
#include "daScript/ast/ast_interop.h"
#include "daScript/ast/ast_handle.h"
#include "daScript/ast/ast_typefactory_bind.h"
#include "daScript/simulate/bind_enum.h"
#include "daScript/simulate/fs_file_info.h"
THIRD_PARTY_INCLUDES_END
#include "using_das.inc"

#include "CoreMinimal.h"

namespace das {
    template <> struct cast_arg<ESearchDir::Type> {
        static __forceinline ESearchDir::Type to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            ESearchDir::Type v; memcpy(&v, &res, sizeof(ESearchDir::Type));
            return v;
        }
    };

    template <> struct cast_arg<ESearchCase::Type> {
        static __forceinline ESearchCase::Type to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            ESearchCase::Type v; memcpy(&v, &res, sizeof(ESearchCase::Type));
            return v;
        }
    };

    //------------------------------------------------------------------------
    //Manual add, replace after re-generate

    template <> struct cast_arg<FName> {
        static __forceinline const FName& to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            return *cast<FName*>::to(res);
        }
    };

    template <> struct cast_arg<FStaticConstructObjectParameters> {
        static __forceinline const FStaticConstructObjectParameters& to(Context& ctx, SimNode* node) {
            vec4f res = node->eval(ctx);
            return *cast<FStaticConstructObjectParameters*>::to(res);
        }
    };
}

MAKE_EXTERNAL_TYPE_FACTORY(FStaticConstructObjectParameters, FStaticConstructObjectParameters);
MAKE_EXTERNAL_TYPE_FACTORY(FText, FText);
MAKE_EXTERNAL_TYPE_FACTORY(FName, FName);