#include "Test/TestClass.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UTestClass::UTestClass() {
}

void UTestClass::SaySomething(FString Msg) {
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
	//UKismetSystemLibrary::PrintString(this, Msg, true, false, FLinearColor::Blue, 0);
}

void UTestClass::SaySomethingElse(FString Msg) {
	UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
}
