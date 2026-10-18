// All IO is synchronous on the frozen game thread. No mutex/wait or shared VM.
struct FPhysicsVMFrame {
 das::Context* Context=nullptr;
 const FDasAsyncPhysicsStep* Step=nullptr;
 const FDasPhysicsPacket* Input=nullptr;
 FDasPhysicsPacket* Output=nullptr;
 bool Failed=false;
 FDasPhysicsBodyScope* Bodies=nullptr;
};
thread_local FPhysicsVMFrame* ActivePhysicsVMFrame=nullptr;
void GuardPhysicsDataExtern(){
 auto* F=ActivePhysicsVMFrame;
 if(!IsInGameThread() || GetDasExecutionRole()!=EDasExecutionRole::PhysicsPreSim || !F || !F->Step || !F->Step->IsCurrent()){
  if(ActiveContext)ActiveContext->throw_error("physics data IO requires isolated VM step");
  if(RestrictedPhysicsContext)RestrictedPhysicsContext->throw_error("physics data IO requires isolated VM step");
  throw std::runtime_error("physics data IO requires isolated VM step");
 }
}
FPhysicsVMFrame& PhysicsVMIO(int32 Slot,das::Context* C){
 auto& F=*ActivePhysicsVMFrame;
 if(C!=F.Context || Slot<0 || Slot>=FDasPhysicsPacket::Capacity){F.Failed=true;C->throw_error("invalid physics data context or slot (0..15)");}
 return F;
}
double PhysicsInput(int32 Slot,das::Context* C){return PhysicsVMIO(Slot,C).Input->Values[Slot];}
void PhysicsOutput(int32 Slot,double Value,das::Context* C){
 auto& F=PhysicsVMIO(Slot,C);
 if(!FMath::IsFinite(Value)){F.Failed=true;C->throw_error("physics output must be finite");}
 F.Output->Values[Slot]=Value;
}

FDasPhysicsBodyToken PhysicsBody(int32 Slot,das::Context* C){
 auto& F=*ActivePhysicsVMFrame;FString Error;
 if(C!=F.Context || !F.Bodies){F.Failed=true;C->throw_error("physics body requires current isolated VM");}
 const uint64 Token=F.Bodies->Acquire(Slot,Error);
 if(!Token){F.Failed=true;C->throw_error(TCHAR_TO_UTF8(*Error));}return {Token};
}
void PhysicsAddForce(FDasPhysicsBodyToken Token,double X,double Y,double Z,bool AccelChange,das::Context* C){
 auto& F=*ActivePhysicsVMFrame;FString Error;
 if(C!=F.Context || !F.Bodies){F.Failed=true;C->throw_error("physics force requires current isolated VM");}
 if(!F.Bodies->AddForce(Token.Value,FVector(X,Y,Z),AccelChange,Error)){F.Failed=true;C->throw_error(TCHAR_TO_UTF8(*Error));}
}
struct FPhysicsBodyTokenAnnotation:das::ManagedStructureAnnotation<FDasPhysicsBodyToken,false>{
 FPhysicsBodyTokenAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UEPhysicsBodyToken",L,"FDasPhysicsBodyToken"){}
 virtual das::TypeDeclPtr makeValueType()const override{return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}
 virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* L,das::SimNode* R)const override{return C.code->makeNode<das::SimNode_Set<FDasPhysicsBodyToken>>(At,L,R);}
 virtual bool isRefType()const override{return false;}virtual bool isLocal()const override{return true;}
 virtual bool canCopy()const override{return true;}virtual bool canMove()const override{return true;}
 virtual bool hasNonTrivialCtor()const override{return false;}virtual bool isPod()const override{return true;}
};
