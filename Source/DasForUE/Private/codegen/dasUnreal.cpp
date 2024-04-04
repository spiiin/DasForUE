// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

#include "dasUnreal.h"
#include "need_dasUnreal.h"
#include "dasUnreal.struct.impl.inc"
namespace das {
#include "dasUnreal.enum.class.inc"
#include "dasUnreal.struct.class.inc"
#include "dasUnreal.func.aot.inc"
Module_dasUnreal::Module_dasUnreal() : Module("unreal") {
}
bool Module_dasUnreal::initDependencies() {
	if ( initialized ) return true;
	initialized = true;
	lib.addModule(this);
	lib.addBuiltInModule();
	initAdditionalAnnotations();
	#include "dasUnreal.enum.add.inc"
	#include "dasUnreal.dummy.add.inc"
	#include "dasUnreal.struct.add.inc"
	#include "dasUnreal.struct.postadd.inc"
	#include "dasUnreal.alias.add.inc"
	initAdditionalFunctions();
	#include "dasUnreal.func.reg.inc"
	return true;
}
}
REGISTER_MODULE_IN_NAMESPACE(Module_dasUnreal,das);

