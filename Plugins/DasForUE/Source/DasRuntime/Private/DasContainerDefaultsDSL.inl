// Only resolved, bounded values enter the persisted default wire.
TSharedPtr<FJsonValue> DSLContainerDefaultValue(const das::TypeDeclPtr& T,das::ExpressionPtr E){
 if(!E)DSLFail(TEXT("Missing container default value"));
 if(DSLDirectEnum(T)){das::Structure::FieldDeclaration F;F.type=T;F.init=E;FString Member;
  if(E->rtti_isField()){auto* V=static_cast<das::ExprField*>(E);if(!V->value->rtti_isVar() || static_cast<das::ExprVar*>(V->value)->name!=T->enumType->name)DSLFail(TEXT("Container enum default requires its declared enum"));Member=DSLText(V->name);}
  else if(E->__rtti==das::string("ExprConstEnumeration")){Member=DSLText(static_cast<das::ExprConstEnumeration*>(E)->text);int32 Dot;if(Member.FindLastChar('.',Dot))Member=Member.Mid(Dot+1);int32 Colon=Member.Find(TEXT("::"),ESearchCase::CaseSensitive,ESearchDir::FromEnd);if(Colon!=INDEX_NONE)Member=Member.Mid(Colon+2);}
  else DSLFail(TEXT("Container enum default requires a named enum member"));return MakeShared<FJsonValueString>(Member);
 }
 if(!DSLMathPath(T).IsEmpty()){auto V=DSLDefaultMath(E);das::Structure::FieldDeclaration F;F.type=T;F.init=E;return MakeShared<FJsonValueString>(DSLMathPropertyDefault(F));}
 const auto K=DSLTypeName(T);
 if(K==TEXT("name")){auto* Call=E->rtti_isCall()?static_cast<das::ExprCall*>(E):nullptr;if(!Call || Call->name!="ue_name_value" || Call->arguments.size()!=1 || Call->arguments[0]->__rtti!=das::string("ExprConstString"))DSLFail(TEXT("Container FName defaults require ue_name_value(string)"));return MakeShared<FJsonValueString>(DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text));}
 if(K==TEXT("string") && E->__rtti==das::string("ExprConstString"))return MakeShared<FJsonValueString>(DSLText(static_cast<das::ExprConstString*>(E)->text));
 if(K==TEXT("bool") && E->__rtti==das::string("ExprConstBool"))return MakeShared<FJsonValueBoolean>(static_cast<das::ExprConstBool*>(E)->getValue());
 if((K==TEXT("int") && E->__rtti==das::string("ExprConstInt")) || (K==TEXT("float") && (E->__rtti==das::string("ExprConstFloat") || E->__rtti==das::string("ExprConstInt"))) || (K==TEXT("double") && (E->__rtti==das::string("ExprConstDouble") || E->__rtti==das::string("ExprConstInt"))))return MakeShared<FJsonValueNumber>(DSLDefaultNumber(E));
 DSLFail(TEXT("Container default value type is unsupported or differs from its declaration: ")+K);return nullptr;
}
FString DSLContainerDefault(das::Structure::FieldDeclaration& F,const FString& Path){
 if(!F.init)return {};auto* Items=DSLDefaultItems(F.init,F.type);TArray<TSharedPtr<FJsonValue>> Values;
 if(Items)for(const auto& E:Items->values){if(F.type->baseType==das::Type::tTable && !DSLSet(F.type)){if(E->__rtti!=das::string("ExprMakeTuple"))DSLFail(TEXT("Map defaults require pairs"));auto* Pair=static_cast<das::ExprMakeTuple*>(E);if(Pair->values.size()!=2)DSLFail(TEXT("Map defaults require pairs"));Values.Add(MakeShared<FJsonValueArray>(TArray<TSharedPtr<FJsonValue>>{DSLContainerDefaultValue(F.type->firstType,Pair->values[0]),DSLContainerDefaultValue(F.type->secondType,Pair->values[1])}));}else Values.Add(DSLContainerDefaultValue(F.type->firstType,E));}
 FString Wire;auto Writer=TJsonWriterFactory<TCHAR,TCondensedJsonPrintPolicy<TCHAR>>::Create(&Wire);FJsonSerializer::Serialize(Values,Writer);FInstancedStruct Parsed;FString Error;if(!ParseDasStructDefault(Path,Wire,Parsed,Error))DSLFail(Error);F.init=nullptr;return Wire;
}
