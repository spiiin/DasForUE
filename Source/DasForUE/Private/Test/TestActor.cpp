#include "Test/TestActor.h"

#include "Components/StaticMeshComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void compile_and_run_script(const char* fname);

ATestActor::ATestActor() {
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = StaticMesh;
}

void ATestActor::BeginPlay() {
	Super::BeginPlay();

	FTCHARToUTF8 Convert(*ScriptFilePath);
	const char* Utf8String = (const char*)Convert.Get();
	compile_and_run_script(Utf8String);
}

void ATestActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ATestActor::SetColorByStringInMesh(FString ColorStr) {
	FLinearColor Color;
	bool bIsValid = Color.InitFromString(ColorStr);
	if(bIsValid) {
		StaticMesh->SetVectorParameterValueOnMaterials(FName("Base Color"), UKismetMathLibrary::Conv_LinearColorToVector(Color));
	} else if(!bIsValid) {
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Color Invalid blabla %s"), *ColorStr), true, false, FLinearColor::Red, 2);
	} else {
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("You need to set a mesh ")));
	}
}

void ATestActor::SetTestActorLocation(FVector NewLocation)
{
	this->SetActorLocation(NewLocation);
	UE_LOG(LogTemp, Log, TEXT("Test Actor Location called. The name is %s and the new loc is %s"), *GetName(), *NewLocation.ToCompactString());
}