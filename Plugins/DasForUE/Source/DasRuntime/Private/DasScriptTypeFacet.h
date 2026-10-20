#pragma once
#include "DasTypeAdapters.h"
// Requires the daScript AST headers, included by DasRuntime.cpp with its packing and macro guards.

// daScript view of an adapted value type: recognising the view, crossing the boundary and naming the view
// that native properties of the type map to (Docs/Dev/Design/TypeAdapters.md).
class IDasScriptTypeFacet
{
public:
    virtual ~IDasScriptTypeFacet() = default;
    virtual bool MatchesScriptType(const das::TypeDeclPtr& Type) const = 0;
    virtual das::ExpressionPtr Pack(const das::LineInfo& At, das::ExpressionPtr Value) const = 0;
    virtual das::ExpressionPtr Unpack(const das::LineInfo& At, const das::TypeDeclPtr& Type,
                                      das::ExpressionPtr Handle) const = 0;
    // Qualified view type and the module that declares it.
    virtual const char* GetViewType() const = 0;
    virtual const TCHAR* GetViewModule() const = 0;
};
