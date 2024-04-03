// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

FString _dasUnreal_static_0_Chr(const FString &,wchar_t Ch) {
	return FString::Chr(Ch);
}

FString _dasUnreal_static_1_ChrN(const FString &,int NumCharacters,wchar_t Char) {
	return FString::ChrN(NumCharacters,Char);
}

FString _dasUnreal_static_2_FormatAsNumber(const FString &,int InNumber) {
	return FString::FormatAsNumber(InNumber);
}

FString _dasUnreal_static_3_FromInt(const FString &,int Num) {
	return FString::FromInt(Num);
}

FString _dasUnreal_static_4_FromBlob(const FString &,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_5_ToBlob(const FString &,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_6_FromHexBlob(const FString &,const unsigned char * SrcBuffer,const unsigned int SrcSize) {
	return FString::FromHexBlob(SrcBuffer,SrcSize);
}

bool _dasUnreal_static_7_ToHexBlob(const FString &,const FString & Source,unsigned char * DestBuffer,const unsigned int DestSize) {
	return FString::ToHexBlob(Source,DestBuffer,DestSize);
}

FString _dasUnreal_static_8_SanitizeFloat(const FString &,double InFloat,const int InMinFractionalDigits) {
	return FString::SanitizeFloat(InFloat,InMinFractionalDigits);
}

