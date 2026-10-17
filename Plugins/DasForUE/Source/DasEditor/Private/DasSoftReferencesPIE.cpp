#include "DasDSLService.h"
#include "DasTypeService.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "DasPropertyTypes.h"
#include "DasAuthoring.h"
#include "DasSoftReferences.h"
#include "DasTypePins.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "EdGraphSchema_K2.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/StructOnScope.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace {
int32 SoftCount(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
class FSoftReferenceCommand:public IAutomationLatentCommand {
 TStrongObjectPtr<UBlueprint> Blueprint;FAutomationTestBase* Test;double Start=FPlatformTime::Seconds();bool Ending=false;
public:
 FSoftReferenceCommand(UBlueprint* BP,FAutomationTestBase* T):Blueprint(BP),Test(T){}
 bool Update()override{
  auto* W=GEditor->PlayWorld.Get();if(Ending)return !W;
  ADasProbeActor* A=nullptr;if(W)for(TActorIterator<ADasProbeActor> I(W);I;++I)if(I->GetFName()==TEXT("SoftReferences"))A=*I;
  if((!A || SoftCount(A,TEXT("Ready"))<3) && FPlatformTime::Seconds()-Start<30 && (!A || A->LastDasError.IsEmpty()))return false;
  Test->TestTrue(TEXT("All soft async callbacks completed"),A && SoftCount(A,TEXT("Ready"))==3);
  if(A){Test->TestEqual(TEXT("Generated native soft fields and methods"),SoftCount(A,TEXT("NativeChecks")),1);Test->TestTrue(TEXT("No soft runtime errors"),A->LastDasError.IsEmpty() && SoftCount(A,TEXT("Errors"))==0);if(!A->LastDasError.IsEmpty())Test->AddError(A->LastDasError);Test->TestEqual(TEXT("Invalid paths and wrong class rejected"),SoftCount(A,TEXT("Rejected")),2);Test->TestEqual(TEXT("Soft collections and authored record"),SoftCount(A,TEXT("CollectionChecks")),1);
   auto* F=A->FindFunction(TEXT("Change"));FStructOnScope Memory(F);
   auto* Input=F?FindFProperty<FSoftObjectProperty>(F,TEXT("value")):nullptr;auto* Previous=F?FindFProperty<FSoftObjectProperty>(F,TEXT("previous")):nullptr;auto* Result=F?CastField<FSoftObjectProperty>(F->GetReturnProperty()):nullptr;
   Test->TestTrue(TEXT("Native soft method pins"),Input && Previous && Result);
   if(Input && Previous && Result){const FSoftObjectPath Path(TEXT("/Game/NotLoaded/Asset.Asset"));Input->SetPropertyValue_InContainer(Memory.GetStructMemory(),FSoftObjectPtr(Path));A->ProcessEvent(F,Memory.GetStructMemory());Test->TestTrue(TEXT("Unloaded soft path return/ref/out roundtrip"),Input->GetPropertyValue_InContainer(Memory.GetStructMemory()).IsNull() && Previous->GetPropertyValue_InContainer(Memory.GetStructMemory()).ToSoftObjectPath()==Path && Result->GetPropertyValue_InContainer(Memory.GetStructMemory()).ToSoftObjectPath()==Path);}
  }
  if(A){auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto* Queue=A->FindFunction(TEXT("QueueForRelease"));Test->TestNotNull(TEXT("Queue release probe"),Queue);if(Queue){A->ProcessEvent(Queue,nullptr);Test->TestTrue(TEXT("Pending asset request before EndPlay"),SoftCount(A,TEXT("Errors"))==0 && Runtime.SubscriptionCount(A)>0);}A->Destroy();Test->TestTrue(TEXT("EndPlay revokes queued soft callbacks"),Runtime.SubscriptionCount(A)==0 && Runtime.PersistentContextCount(W)==0);}
  GEditor->RequestEndPlayMap();Ending=true;return false;
 }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasSoftReferencesPIE,"DasForUE.SoftReferences.RuntimePIE",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FDasSoftReferencesPIE::RunTest(const FString&){
 FString Error;auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));for(const TCHAR* Name:{TEXT("dsl_soft_bad_type.das"),TEXT("dsl_soft_bad_literal.das"),TEXT("dsl_soft_bad_subobject.das")}){FDasAuthoringModule Model;TestFalse(Name,Runtime.ReadAuthoringScript(Runtime.ScriptPath(Name),Model,Error));}for(const TCHAR* Name:{TEXT("native_soft_bad_class.das"),TEXT("native_soft_bad_array.das"),TEXT("native_soft_bad_ref.das")}){Error.Reset();TestFalse(Name,Runtime.ValidateIntMethod(Runtime.ScriptPath(Name),Error));TestTrue(TEXT("Native soft mismatch reports compile error"),!Error.IsEmpty());}UBlueprint* BP=nullptr;TArray<UObject*> Types;if(!FDasTypeService::Apply(TEXT("ue_soft_record.das"),TEXT("/Game/DasSoftReferences/Types"),Types,Error)){AddError(Error);return false;} if(!FDasDSLService::Apply(TEXT("dsl_soft_references.das"),TEXT("/Game/DasSoftReferences/BP_SoftReferences"),TEXT("dsl_soft_references.SoftReferenceActor"),BP,Error)){AddError(Error);return false;}
 TStrongObjectPtr<UBlueprint> Keep(BP);auto* Asset=FindFProperty<FSoftObjectProperty>(BP->GeneratedClass,TEXT("Asset"));auto* Class=FindFProperty<FSoftClassProperty>(BP->GeneratedClass,TEXT("ActorType"));TestTrue(TEXT("Native soft object/class fields"),Asset && Class && Class->MetaClass==AActor::StaticClass());
 if(Asset){FDasDeclaredType Type;FEdGraphPinType Pin;TestTrue(TEXT("Describe soft field and make native soft pin"),DescribeDasProperty(Asset,Type,Error) && DasTypeToPin(Type,Pin,Error) && Pin.PinCategory==UEdGraphSchema_K2::PC_SoftObject);FDasDeclaredType Round;TestTrue(TEXT("Soft pin roundtrip retains base"),DescribeDasPin(Pin,Round,Error) && Round.StorageIdentity()==Type.StorageIdentity());}
 auto* W=FAutomationEditorCommonUtils::CreateNewMap();if(!W)return false;FActorSpawnParameters Spawn;Spawn.Name=TEXT("SoftReferences");if(!W->SpawnActor<ADasProbeActor>(BP->GeneratedClass,FTransform::Identity,Spawn))return false;
 ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));ADD_LATENT_AUTOMATION_COMMAND(FSoftReferenceCommand(BP,this));return true;
}
#endif
