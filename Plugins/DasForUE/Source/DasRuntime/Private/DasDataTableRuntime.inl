// DataTable memory never escapes: each successful read enters the common owned snapshot table.
UDataTable* ScriptDataTable(const FDasObjectHandle& H,das::Context* C){
 RequireObjectContext(C);auto* Table=Cast<UDataTable>(ResolveObject(H,C));
 if(!IsValid(Table) || Table->IsTemplate() || !Table->GetRowStruct())C->throw_error("expected a live DataTable with a row struct");
 return Table;
}
FName ScriptDataTableName(const char* Text,das::Context* C){
 const FString Name=Text?UTF8_TO_TCHAR(Text):TEXT("");
 if(Name.IsEmpty() || Name.Len()>1024 || Name.Equals(TEXT("None"),ESearchCase::IgnoreCase))C->throw_error("DataTable row name must contain 1..1024 characters and cannot be None");
 return FName(*Name);
}
bool ScriptDataTableHas(const FDasObjectHandle& H,const char* Name,das::Context* C){
 auto* Table=ScriptDataTable(H,C);return Table->GetRowMap().Contains(ScriptDataTableName(Name,C));
}
FDasStructValue ScriptDataTableNames(const FDasObjectHandle& H,das::Context* C,das::LineInfoArg* At){
 auto* Table=ScriptDataTable(H,C);if(Table->GetRowMap().Num()>4096)C->throw_error("DataTable name listing exceeds 4096 rows");
 auto Names=Table->GetRowNames();Names.Sort(FNameLexicalLess());FInstancedStruct V;V.InitializeAs<FDasArrayStringCarrier>();
 auto& Items=V.GetMutable<FDasArrayStringCarrier>().Values;for(const auto Name:Names)Items.Add(Name.ToString());
 return StoreStructValue(MoveTemp(V),C,At);
}
FDasStructValue ScriptDataTableRow(const FDasObjectHandle& H,const char* Name,const char* Path,das::Context* C,das::LineInfoArg* At){
 auto* Table=ScriptDataTable(H,C);auto* Type=Table->GetRowStruct();
 if(!Path || Type->GetPathName()!=UTF8_TO_TCHAR(Path))C->throw_error("DataTable row struct identity mismatch");
 FString Error;if(!ValidateDasValueStruct(Type->GetPathName(),Error))C->throw_error(TCHAR_TO_UTF8(*Error));
 const auto* Row=Table->GetRowMap().Find(ScriptDataTableName(Name,C));if(!Row || !*Row)C->throw_error("DataTable row not found");
 FInstancedStruct Copy;Copy.InitializeAs(Type,*Row);return StoreStructValue(MoveTemp(Copy),C,At);
}
