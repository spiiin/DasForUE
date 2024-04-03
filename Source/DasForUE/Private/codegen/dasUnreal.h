// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

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
#include "cb_dasUnreal.h"
namespace das {
class Module_dasUnreal : public Module {
public:
	Module_dasUnreal();
protected:
	virtual bool initDependencies() override;
	void initAdditional();
	void initMain ();
	virtual ModuleAotType aotRequire ( TextWriter & tw ) const override;
	#include "dasUnreal.func.decl.inc"
public:
	ModuleLibrary lib;
	bool initialized = false;
};
}

