// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:8:12
class Enumeration_EVersionComponent : public das::Enumeration {
public:
	Enumeration_EVersionComponent() : das::Enumeration("EVersionComponent") {
		external = true;
		cppName = "EVersionComponent";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Major", "Major", int64_t(EVersionComponent::Major), das::LineInfo());
		addIEx("Minor", "Minor", int64_t(EVersionComponent::Minor), das::LineInfo());
		addIEx("Patch", "Patch", int64_t(EVersionComponent::Patch), das::LineInfo());
		addIEx("Changelist", "Changelist", int64_t(EVersionComponent::Changelist), das::LineInfo());
		addIEx("Branch", "Branch", int64_t(EVersionComponent::Branch), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/EngineVersionBase.h:19:12
class Enumeration_EVersionComparison : public das::Enumeration {
public:
	Enumeration_EVersionComparison() : das::Enumeration("EVersionComparison") {
		external = true;
		cppName = "EVersionComparison";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Neither", "Neither", int64_t(EVersionComparison::Neither), das::LineInfo());
		addIEx("First", "First", int64_t(EVersionComparison::First), das::LineInfo());
		addIEx("Second", "Second", int64_t(EVersionComparison::Second), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/CompressionFlags.h:12:6
class Enumeration_ECompressionFlags : public das::Enumeration {
public:
	Enumeration_ECompressionFlags() : das::Enumeration("ECompressionFlags") {
		external = true;
		cppName = "ECompressionFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("COMPRESS_None", "COMPRESS_None", int64_t(ECompressionFlags::COMPRESS_None), das::LineInfo());
		addIEx("COMPRESS_ZLIB", "COMPRESS_ZLIB", int64_t(ECompressionFlags::COMPRESS_ZLIB), das::LineInfo());
		addIEx("COMPRESS_GZIP", "COMPRESS_GZIP", int64_t(ECompressionFlags::COMPRESS_GZIP), das::LineInfo());
		addIEx("COMPRESS_Custom", "COMPRESS_Custom", int64_t(ECompressionFlags::COMPRESS_Custom), das::LineInfo());
		addIEx("COMPRESS_DeprecatedFormatFlagsMask", "COMPRESS_DeprecatedFormatFlagsMask", int64_t(ECompressionFlags::COMPRESS_DeprecatedFormatFlagsMask), das::LineInfo());
		addIEx("COMPRESS_NoFlags", "COMPRESS_NoFlags", int64_t(ECompressionFlags::COMPRESS_NoFlags), das::LineInfo());
		addIEx("COMPRESS_BiasMemory", "COMPRESS_BiasMemory", int64_t(ECompressionFlags::COMPRESS_BiasMemory), das::LineInfo());
		addIEx("COMPRESS_BiasSize", "COMPRESS_BiasSize", int64_t(ECompressionFlags::COMPRESS_BiasSize), das::LineInfo());
		addIEx("COMPRESS_BiasSpeed", "COMPRESS_BiasSpeed", int64_t(ECompressionFlags::COMPRESS_BiasSpeed), das::LineInfo());
		addIEx("COMPRESS_SourceIsPadded", "COMPRESS_SourceIsPadded", int64_t(ECompressionFlags::COMPRESS_SourceIsPadded), das::LineInfo());
		addIEx("COMPRESS_OptionsFlagsMask", "COMPRESS_OptionsFlagsMask", int64_t(ECompressionFlags::COMPRESS_OptionsFlagsMask), das::LineInfo());
		addIEx("COMPRESS_ForPackaging", "COMPRESS_ForPackaging", int64_t(ECompressionFlags::COMPRESS_ForPackaging), das::LineInfo());
		addIEx("COMPRESS_ForPurposeMask", "COMPRESS_ForPurposeMask", int64_t(ECompressionFlags::COMPRESS_ForPurposeMask), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Logging/LogVerbosity.h:16:7
class Enumeration_ELogVerbosity_Type : public das::Enumeration {
public:
	Enumeration_ELogVerbosity_Type() : das::Enumeration("ELogVerbosity_Type") {
		external = true;
		cppName = "ELogVerbosity::Type";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("NoLogging", "NoLogging", int64_t(ELogVerbosity::Type::NoLogging), das::LineInfo());
		addIEx("Fatal", "Fatal", int64_t(ELogVerbosity::Type::Fatal), das::LineInfo());
		addIEx("Error", "Error", int64_t(ELogVerbosity::Type::Error), das::LineInfo());
		addIEx("Warning", "Warning", int64_t(ELogVerbosity::Type::Warning), das::LineInfo());
		addIEx("Display", "Display", int64_t(ELogVerbosity::Type::Display), das::LineInfo());
		addIEx("Log", "Log", int64_t(ELogVerbosity::Type::Log), das::LineInfo());
		addIEx("Verbose", "Verbose", int64_t(ELogVerbosity::Type::Verbose), das::LineInfo());
		addIEx("VeryVerbose", "VeryVerbose", int64_t(ELogVerbosity::Type::VeryVerbose), das::LineInfo());
		addIEx("All", "All", int64_t(ELogVerbosity::Type::All), das::LineInfo());
		addIEx("NumVerbosity", "NumVerbosity", int64_t(ELogVerbosity::Type::NumVerbosity), das::LineInfo());
		addIEx("VerbosityMask", "VerbosityMask", int64_t(ELogVerbosity::Type::VerbosityMask), das::LineInfo());
		addIEx("SetColor", "SetColor", int64_t(ELogVerbosity::Type::SetColor), das::LineInfo());
		addIEx("BreakOnLog", "BreakOnLog", int64_t(ELogVerbosity::Type::BreakOnLog), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Misc/OutputDevice.h:107:7
class Enumeration_ELogTimes_Type : public das::Enumeration {
public:
	Enumeration_ELogTimes_Type() : das::Enumeration("ELogTimes_Type") {
		external = true;
		cppName = "ELogTimes::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(ELogTimes::Type::None), das::LineInfo());
		addIEx("UTC", "UTC", int64_t(ELogTimes::Type::UTC), das::LineInfo());
		addIEx("SinceGStartTime", "SinceGStartTime", int64_t(ELogTimes::Type::SinceGStartTime), das::LineInfo());
		addIEx("Local", "Local", int64_t(ELogTimes::Type::Local), das::LineInfo());
		addIEx("Timecode", "Timecode", int64_t(ELogTimes::Type::Timecode), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/Serialization/Archive.h:46:12
class Enumeration_EFileRegionType : public das::Enumeration {
public:
	Enumeration_EFileRegionType() : das::Enumeration("EFileRegionType") {
		external = true;
		cppName = "EFileRegionType";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public/UObject/UnrealNames.h:15:12
class Enumeration_EName : public das::Enumeration {
public:
	Enumeration_EName() : das::Enumeration("EName") {
		external = true;
		cppName = "EName";
		baseType = (das::Type) das::ToBasicType<uint32>::type;
		addIEx("None", "None", int64_t(EName::None), das::LineInfo());
		addIEx("ByteProperty", "ByteProperty", int64_t(EName::ByteProperty), das::LineInfo());
		addIEx("IntProperty", "IntProperty", int64_t(EName::IntProperty), das::LineInfo());
		addIEx("BoolProperty", "BoolProperty", int64_t(EName::BoolProperty), das::LineInfo());
		addIEx("FloatProperty", "FloatProperty", int64_t(EName::FloatProperty), das::LineInfo());
		addIEx("ObjectProperty", "ObjectProperty", int64_t(EName::ObjectProperty), das::LineInfo());
		addIEx("NameProperty", "NameProperty", int64_t(EName::NameProperty), das::LineInfo());
		addIEx("DelegateProperty", "DelegateProperty", int64_t(EName::DelegateProperty), das::LineInfo());
		addIEx("DoubleProperty", "DoubleProperty", int64_t(EName::DoubleProperty), das::LineInfo());
		addIEx("ArrayProperty", "ArrayProperty", int64_t(EName::ArrayProperty), das::LineInfo());
		addIEx("StructProperty", "StructProperty", int64_t(EName::StructProperty), das::LineInfo());
		addIEx("VectorProperty", "VectorProperty", int64_t(EName::VectorProperty), das::LineInfo());
		addIEx("RotatorProperty", "RotatorProperty", int64_t(EName::RotatorProperty), das::LineInfo());
		addIEx("StrProperty", "StrProperty", int64_t(EName::StrProperty), das::LineInfo());
		addIEx("TextProperty", "TextProperty", int64_t(EName::TextProperty), das::LineInfo());
		addIEx("InterfaceProperty", "InterfaceProperty", int64_t(EName::InterfaceProperty), das::LineInfo());
		addIEx("MulticastDelegateProperty", "MulticastDelegateProperty", int64_t(EName::MulticastDelegateProperty), das::LineInfo());
		addIEx("LazyObjectProperty", "LazyObjectProperty", int64_t(EName::LazyObjectProperty), das::LineInfo());
		addIEx("SoftObjectProperty", "SoftObjectProperty", int64_t(EName::SoftObjectProperty), das::LineInfo());
		addIEx("Int64Property", "Int64Property", int64_t(EName::Int64Property), das::LineInfo());
		addIEx("Int32Property", "Int32Property", int64_t(EName::Int32Property), das::LineInfo());
		addIEx("Int16Property", "Int16Property", int64_t(EName::Int16Property), das::LineInfo());
		addIEx("Int8Property", "Int8Property", int64_t(EName::Int8Property), das::LineInfo());
		addIEx("UInt64Property", "UInt64Property", int64_t(EName::UInt64Property), das::LineInfo());
		addIEx("UInt32Property", "UInt32Property", int64_t(EName::UInt32Property), das::LineInfo());
		addIEx("UInt16Property", "UInt16Property", int64_t(EName::UInt16Property), das::LineInfo());
		addIEx("MapProperty", "MapProperty", int64_t(EName::MapProperty), das::LineInfo());
		addIEx("SetProperty", "SetProperty", int64_t(EName::SetProperty), das::LineInfo());
		addIEx("Core", "Core", int64_t(EName::Core), das::LineInfo());
		addIEx("Engine", "Engine", int64_t(EName::Engine), das::LineInfo());
		addIEx("Editor", "Editor", int64_t(EName::Editor), das::LineInfo());
		addIEx("CoreUObject", "CoreUObject", int64_t(EName::CoreUObject), das::LineInfo());
		addIEx("EnumProperty", "EnumProperty", int64_t(EName::EnumProperty), das::LineInfo());
		addIEx("OptionalProperty", "OptionalProperty", int64_t(EName::OptionalProperty), das::LineInfo());
		addIEx("Cylinder", "Cylinder", int64_t(EName::Cylinder), das::LineInfo());
		addIEx("BoxSphereBounds", "BoxSphereBounds", int64_t(EName::BoxSphereBounds), das::LineInfo());
		addIEx("Sphere", "Sphere", int64_t(EName::Sphere), das::LineInfo());
		addIEx("Box", "Box", int64_t(EName::Box), das::LineInfo());
		addIEx("Vector2D", "Vector2D", int64_t(EName::Vector2D), das::LineInfo());
		addIEx("IntRect", "IntRect", int64_t(EName::IntRect), das::LineInfo());
		addIEx("IntPoint", "IntPoint", int64_t(EName::IntPoint), das::LineInfo());
		addIEx("Vector4", "Vector4", int64_t(EName::Vector4), das::LineInfo());
		addIEx("Name", "Name", int64_t(EName::Name), das::LineInfo());
		addIEx("Vector", "Vector", int64_t(EName::Vector), das::LineInfo());
		addIEx("Rotator", "Rotator", int64_t(EName::Rotator), das::LineInfo());
		addIEx("SHVector", "SHVector", int64_t(EName::SHVector), das::LineInfo());
		addIEx("Color", "Color", int64_t(EName::Color), das::LineInfo());
		addIEx("Plane", "Plane", int64_t(EName::Plane), das::LineInfo());
		addIEx("Matrix", "Matrix", int64_t(EName::Matrix), das::LineInfo());
		addIEx("LinearColor", "LinearColor", int64_t(EName::LinearColor), das::LineInfo());
		addIEx("AdvanceFrame", "AdvanceFrame", int64_t(EName::AdvanceFrame), das::LineInfo());
		addIEx("Pointer", "Pointer", int64_t(EName::Pointer), das::LineInfo());
		addIEx("Double", "Double", int64_t(EName::Double), das::LineInfo());
		addIEx("Quat", "Quat", int64_t(EName::Quat), das::LineInfo());
		addIEx("Self", "Self", int64_t(EName::Self), das::LineInfo());
		addIEx("Transform", "Transform", int64_t(EName::Transform), das::LineInfo());
		addIEx("Vector3f", "Vector3f", int64_t(EName::Vector3f), das::LineInfo());
		addIEx("Vector3d", "Vector3d", int64_t(EName::Vector3d), das::LineInfo());
		addIEx("Plane4f", "Plane4f", int64_t(EName::Plane4f), das::LineInfo());
		addIEx("Plane4d", "Plane4d", int64_t(EName::Plane4d), das::LineInfo());
		addIEx("Matrix44f", "Matrix44f", int64_t(EName::Matrix44f), das::LineInfo());
		addIEx("Matrix44d", "Matrix44d", int64_t(EName::Matrix44d), das::LineInfo());
		addIEx("Quat4f", "Quat4f", int64_t(EName::Quat4f), das::LineInfo());
		addIEx("Quat4d", "Quat4d", int64_t(EName::Quat4d), das::LineInfo());
		addIEx("Transform3f", "Transform3f", int64_t(EName::Transform3f), das::LineInfo());
		addIEx("Transform3d", "Transform3d", int64_t(EName::Transform3d), das::LineInfo());
		addIEx("Box3f", "Box3f", int64_t(EName::Box3f), das::LineInfo());
		addIEx("Box3d", "Box3d", int64_t(EName::Box3d), das::LineInfo());
		addIEx("BoxSphereBounds3f", "BoxSphereBounds3f", int64_t(EName::BoxSphereBounds3f), das::LineInfo());
		addIEx("BoxSphereBounds3d", "BoxSphereBounds3d", int64_t(EName::BoxSphereBounds3d), das::LineInfo());
		addIEx("Vector4f", "Vector4f", int64_t(EName::Vector4f), das::LineInfo());
		addIEx("Vector4d", "Vector4d", int64_t(EName::Vector4d), das::LineInfo());
		addIEx("Rotator3f", "Rotator3f", int64_t(EName::Rotator3f), das::LineInfo());
		addIEx("Rotator3d", "Rotator3d", int64_t(EName::Rotator3d), das::LineInfo());
		addIEx("Vector2f", "Vector2f", int64_t(EName::Vector2f), das::LineInfo());
		addIEx("Vector2d", "Vector2d", int64_t(EName::Vector2d), das::LineInfo());
		addIEx("Box2D", "Box2D", int64_t(EName::Box2D), das::LineInfo());
		addIEx("Box2f", "Box2f", int64_t(EName::Box2f), das::LineInfo());
		addIEx("Box2d", "Box2d", int64_t(EName::Box2d), das::LineInfo());
		addIEx("IntVector", "IntVector", int64_t(EName::IntVector), das::LineInfo());
		addIEx("IntVector4", "IntVector4", int64_t(EName::IntVector4), das::LineInfo());
		addIEx("UintVector", "UintVector", int64_t(EName::UintVector), das::LineInfo());
		addIEx("UintVector4", "UintVector4", int64_t(EName::UintVector4), das::LineInfo());
		addIEx("Object", "Object", int64_t(EName::Object), das::LineInfo());
		addIEx("Camera", "Camera", int64_t(EName::Camera), das::LineInfo());
		addIEx("Actor", "Actor", int64_t(EName::Actor), das::LineInfo());
		addIEx("ObjectRedirector", "ObjectRedirector", int64_t(EName::ObjectRedirector), das::LineInfo());
		addIEx("ObjectArchetype", "ObjectArchetype", int64_t(EName::ObjectArchetype), das::LineInfo());
		addIEx("Class", "Class", int64_t(EName::Class), das::LineInfo());
		addIEx("ScriptStruct", "ScriptStruct", int64_t(EName::ScriptStruct), das::LineInfo());
		addIEx("Function", "Function", int64_t(EName::Function), das::LineInfo());
		addIEx("Pawn", "Pawn", int64_t(EName::Pawn), das::LineInfo());
		addIEx("Int32Vector", "Int32Vector", int64_t(EName::Int32Vector), das::LineInfo());
		addIEx("Int64Vector", "Int64Vector", int64_t(EName::Int64Vector), das::LineInfo());
		addIEx("Uint32Vector", "Uint32Vector", int64_t(EName::Uint32Vector), das::LineInfo());
		addIEx("Uint64Vector", "Uint64Vector", int64_t(EName::Uint64Vector), das::LineInfo());
		addIEx("Int32Vector4", "Int32Vector4", int64_t(EName::Int32Vector4), das::LineInfo());
		addIEx("Int64Vector4", "Int64Vector4", int64_t(EName::Int64Vector4), das::LineInfo());
		addIEx("Uint32Vector4", "Uint32Vector4", int64_t(EName::Uint32Vector4), das::LineInfo());
		addIEx("Uint64Vector4", "Uint64Vector4", int64_t(EName::Uint64Vector4), das::LineInfo());
		addIEx("IntVector2", "IntVector2", int64_t(EName::IntVector2), das::LineInfo());
		addIEx("Int32Vector2", "Int32Vector2", int64_t(EName::Int32Vector2), das::LineInfo());
		addIEx("Int64Vector2", "Int64Vector2", int64_t(EName::Int64Vector2), das::LineInfo());
		addIEx("UintVector2", "UintVector2", int64_t(EName::UintVector2), das::LineInfo());
		addIEx("Uint32Vector2", "Uint32Vector2", int64_t(EName::Uint32Vector2), das::LineInfo());
		addIEx("Uint64Vector2", "Uint64Vector2", int64_t(EName::Uint64Vector2), das::LineInfo());
		addIEx("UintPoint", "UintPoint", int64_t(EName::UintPoint), das::LineInfo());
		addIEx("Int32Point", "Int32Point", int64_t(EName::Int32Point), das::LineInfo());
		addIEx("Int64Point", "Int64Point", int64_t(EName::Int64Point), das::LineInfo());
		addIEx("Uint32Point", "Uint32Point", int64_t(EName::Uint32Point), das::LineInfo());
		addIEx("Uint64Point", "Uint64Point", int64_t(EName::Uint64Point), das::LineInfo());
		addIEx("Ray", "Ray", int64_t(EName::Ray), das::LineInfo());
		addIEx("Ray3f", "Ray3f", int64_t(EName::Ray3f), das::LineInfo());
		addIEx("Ray3d", "Ray3d", int64_t(EName::Ray3d), das::LineInfo());
		addIEx("Sphere3f", "Sphere3f", int64_t(EName::Sphere3f), das::LineInfo());
		addIEx("Sphere3d", "Sphere3d", int64_t(EName::Sphere3d), das::LineInfo());
		addIEx("State", "State", int64_t(EName::State), das::LineInfo());
		addIEx("TRUE", "TRUE", int64_t(EName::TRUE), das::LineInfo());
		addIEx("FALSE", "FALSE", int64_t(EName::FALSE), das::LineInfo());
		addIEx("Enum", "Enum", int64_t(EName::Enum), das::LineInfo());
		addIEx("Default", "Default", int64_t(EName::Default), das::LineInfo());
		addIEx("Skip", "Skip", int64_t(EName::Skip), das::LineInfo());
		addIEx("Input", "Input", int64_t(EName::Input), das::LineInfo());
		addIEx("Package", "Package", int64_t(EName::Package), das::LineInfo());
		addIEx("Groups", "Groups", int64_t(EName::Groups), das::LineInfo());
		addIEx("Interface", "Interface", int64_t(EName::Interface), das::LineInfo());
		addIEx("Components", "Components", int64_t(EName::Components), das::LineInfo());
		addIEx("Global", "Global", int64_t(EName::Global), das::LineInfo());
		addIEx("Super", "Super", int64_t(EName::Super), das::LineInfo());
		addIEx("Outer", "Outer", int64_t(EName::Outer), das::LineInfo());
		addIEx("Map", "Map", int64_t(EName::Map), das::LineInfo());
		addIEx("Role", "Role", int64_t(EName::Role), das::LineInfo());
		addIEx("RemoteRole", "RemoteRole", int64_t(EName::RemoteRole), das::LineInfo());
		addIEx("PersistentLevel", "PersistentLevel", int64_t(EName::PersistentLevel), das::LineInfo());
		addIEx("TheWorld", "TheWorld", int64_t(EName::TheWorld), das::LineInfo());
		addIEx("PackageMetaData", "PackageMetaData", int64_t(EName::PackageMetaData), das::LineInfo());
		addIEx("InitialState", "InitialState", int64_t(EName::InitialState), das::LineInfo());
		addIEx("Game", "Game", int64_t(EName::Game), das::LineInfo());
		addIEx("SelectionColor", "SelectionColor", int64_t(EName::SelectionColor), das::LineInfo());
		addIEx("UI", "UI", int64_t(EName::UI), das::LineInfo());
		addIEx("ExecuteUbergraph", "ExecuteUbergraph", int64_t(EName::ExecuteUbergraph), das::LineInfo());
		addIEx("DeviceID", "DeviceID", int64_t(EName::DeviceID), das::LineInfo());
		addIEx("RootStat", "RootStat", int64_t(EName::RootStat), das::LineInfo());
		addIEx("MoveActor", "MoveActor", int64_t(EName::MoveActor), das::LineInfo());
		addIEx("All", "All", int64_t(EName::All), das::LineInfo());
		addIEx("MeshEmitterVertexColor", "MeshEmitterVertexColor", int64_t(EName::MeshEmitterVertexColor), das::LineInfo());
		addIEx("TextureOffsetParameter", "TextureOffsetParameter", int64_t(EName::TextureOffsetParameter), das::LineInfo());
		addIEx("TextureScaleParameter", "TextureScaleParameter", int64_t(EName::TextureScaleParameter), das::LineInfo());
		addIEx("ImpactVel", "ImpactVel", int64_t(EName::ImpactVel), das::LineInfo());
		addIEx("SlideVel", "SlideVel", int64_t(EName::SlideVel), das::LineInfo());
		addIEx("TextureOffset1Parameter", "TextureOffset1Parameter", int64_t(EName::TextureOffset1Parameter), das::LineInfo());
		addIEx("MeshEmitterDynamicParameter", "MeshEmitterDynamicParameter", int64_t(EName::MeshEmitterDynamicParameter), das::LineInfo());
		addIEx("ExpressionInput", "ExpressionInput", int64_t(EName::ExpressionInput), das::LineInfo());
		addIEx("Untitled", "Untitled", int64_t(EName::Untitled), das::LineInfo());
		addIEx("Timer", "Timer", int64_t(EName::Timer), das::LineInfo());
		addIEx("Team", "Team", int64_t(EName::Team), das::LineInfo());
		addIEx("Low", "Low", int64_t(EName::Low), das::LineInfo());
		addIEx("High", "High", int64_t(EName::High), das::LineInfo());
		addIEx("NetworkGUID", "NetworkGUID", int64_t(EName::NetworkGUID), das::LineInfo());
		addIEx("GameThread", "GameThread", int64_t(EName::GameThread), das::LineInfo());
		addIEx("RenderThread", "RenderThread", int64_t(EName::RenderThread), das::LineInfo());
		addIEx("OtherChildren", "OtherChildren", int64_t(EName::OtherChildren), das::LineInfo());
		addIEx("Location", "Location", int64_t(EName::Location), das::LineInfo());
		addIEx("Rotation", "Rotation", int64_t(EName::Rotation), das::LineInfo());
		addIEx("BSP", "BSP", int64_t(EName::BSP), das::LineInfo());
		addIEx("EditorSettings", "EditorSettings", int64_t(EName::EditorSettings), das::LineInfo());
		addIEx("AudioThread", "AudioThread", int64_t(EName::AudioThread), das::LineInfo());
		addIEx("ID", "ID", int64_t(EName::ID), das::LineInfo());
		addIEx("UserDefinedEnum", "UserDefinedEnum", int64_t(EName::UserDefinedEnum), das::LineInfo());
		addIEx("Control", "Control", int64_t(EName::Control), das::LineInfo());
		addIEx("Voice", "Voice", int64_t(EName::Voice), das::LineInfo());
		addIEx("Zlib", "Zlib", int64_t(EName::Zlib), das::LineInfo());
		addIEx("Gzip", "Gzip", int64_t(EName::Gzip), das::LineInfo());
		addIEx("LZ4", "LZ4", int64_t(EName::LZ4), das::LineInfo());
		addIEx("Mobile", "Mobile", int64_t(EName::Mobile), das::LineInfo());
		addIEx("Oodle", "Oodle", int64_t(EName::Oodle), das::LineInfo());
		addIEx("DGram", "DGram", int64_t(EName::DGram), das::LineInfo());
		addIEx("Stream", "Stream", int64_t(EName::Stream), das::LineInfo());
		addIEx("GameNetDriver", "GameNetDriver", int64_t(EName::GameNetDriver), das::LineInfo());
		addIEx("PendingNetDriver", "PendingNetDriver", int64_t(EName::PendingNetDriver), das::LineInfo());
		addIEx("BeaconNetDriver", "BeaconNetDriver", int64_t(EName::BeaconNetDriver), das::LineInfo());
		addIEx("FlushNetDormancy", "FlushNetDormancy", int64_t(EName::FlushNetDormancy), das::LineInfo());
		addIEx("DemoNetDriver", "DemoNetDriver", int64_t(EName::DemoNetDriver), das::LineInfo());
		addIEx("GameSession", "GameSession", int64_t(EName::GameSession), das::LineInfo());
		addIEx("PartySession", "PartySession", int64_t(EName::PartySession), das::LineInfo());
		addIEx("GamePort", "GamePort", int64_t(EName::GamePort), das::LineInfo());
		addIEx("BeaconPort", "BeaconPort", int64_t(EName::BeaconPort), das::LineInfo());
		addIEx("MeshPort", "MeshPort", int64_t(EName::MeshPort), das::LineInfo());
		addIEx("MeshNetDriver", "MeshNetDriver", int64_t(EName::MeshNetDriver), das::LineInfo());
		addIEx("LiveStreamVoice", "LiveStreamVoice", int64_t(EName::LiveStreamVoice), das::LineInfo());
		addIEx("LiveStreamAnimation", "LiveStreamAnimation", int64_t(EName::LiveStreamAnimation), das::LineInfo());
		addIEx("DataStream", "DataStream", int64_t(EName::DataStream), das::LineInfo());
		addIEx("Linear", "Linear", int64_t(EName::Linear), das::LineInfo());
		addIEx("Point", "Point", int64_t(EName::Point), das::LineInfo());
		addIEx("Aniso", "Aniso", int64_t(EName::Aniso), das::LineInfo());
		addIEx("LightMapResolution", "LightMapResolution", int64_t(EName::LightMapResolution), das::LineInfo());
		addIEx("UnGrouped", "UnGrouped", int64_t(EName::UnGrouped), das::LineInfo());
		addIEx("VoiceChat", "VoiceChat", int64_t(EName::VoiceChat), das::LineInfo());
		addIEx("Playing", "Playing", int64_t(EName::Playing), das::LineInfo());
		addIEx("Spectating", "Spectating", int64_t(EName::Spectating), das::LineInfo());
		addIEx("Inactive", "Inactive", int64_t(EName::Inactive), das::LineInfo());
		addIEx("PerfWarning", "PerfWarning", int64_t(EName::PerfWarning), das::LineInfo());
		addIEx("Info", "Info", int64_t(EName::Info), das::LineInfo());
		addIEx("Init", "Init", int64_t(EName::Init), das::LineInfo());
		addIEx("Exit", "Exit", int64_t(EName::Exit), das::LineInfo());
		addIEx("Cmd", "Cmd", int64_t(EName::Cmd), das::LineInfo());
		addIEx("Warning", "Warning", int64_t(EName::Warning), das::LineInfo());
		addIEx("Error", "Error", int64_t(EName::Error), das::LineInfo());
		addIEx("FontCharacter", "FontCharacter", int64_t(EName::FontCharacter), das::LineInfo());
		addIEx("InitChild2StartBone", "InitChild2StartBone", int64_t(EName::InitChild2StartBone), das::LineInfo());
		addIEx("SoundCueLocalized", "SoundCueLocalized", int64_t(EName::SoundCueLocalized), das::LineInfo());
		addIEx("SoundCue", "SoundCue", int64_t(EName::SoundCue), das::LineInfo());
		addIEx("RawDistributionFloat", "RawDistributionFloat", int64_t(EName::RawDistributionFloat), das::LineInfo());
		addIEx("RawDistributionVector", "RawDistributionVector", int64_t(EName::RawDistributionVector), das::LineInfo());
		addIEx("InterpCurveFloat", "InterpCurveFloat", int64_t(EName::InterpCurveFloat), das::LineInfo());
		addIEx("InterpCurveVector2D", "InterpCurveVector2D", int64_t(EName::InterpCurveVector2D), das::LineInfo());
		addIEx("InterpCurveVector", "InterpCurveVector", int64_t(EName::InterpCurveVector), das::LineInfo());
		addIEx("InterpCurveTwoVectors", "InterpCurveTwoVectors", int64_t(EName::InterpCurveTwoVectors), das::LineInfo());
		addIEx("InterpCurveQuat", "InterpCurveQuat", int64_t(EName::InterpCurveQuat), das::LineInfo());
		addIEx("FrameRate", "FrameRate", int64_t(EName::FrameRate), das::LineInfo());
		addIEx("AI", "AI", int64_t(EName::AI), das::LineInfo());
		addIEx("NavMesh", "NavMesh", int64_t(EName::NavMesh), das::LineInfo());
		addIEx("PerformanceCapture", "PerformanceCapture", int64_t(EName::PerformanceCapture), das::LineInfo());
		addIEx("EditorLayout", "EditorLayout", int64_t(EName::EditorLayout), das::LineInfo());
		addIEx("EditorKeyBindings", "EditorKeyBindings", int64_t(EName::EditorKeyBindings), das::LineInfo());
		addIEx("GameUserSettings", "GameUserSettings", int64_t(EName::GameUserSettings), das::LineInfo());
		addIEx("Filename", "Filename", int64_t(EName::Filename), das::LineInfo());
		addIEx("Lerp", "Lerp", int64_t(EName::Lerp), das::LineInfo());
		addIEx("Root", "Root", int64_t(EName::Root), das::LineInfo());
		addIEx("MaxHardcodedNameIndex", "MaxHardcodedNameIndex", int64_t(EName::MaxHardcodedNameIndex), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/Core/Public\ProfilingDebugging/ResourceSize.h:17:7
class Enumeration_EResourceSizeMode_Type : public das::Enumeration {
public:
	Enumeration_EResourceSizeMode_Type() : das::Enumeration("EResourceSizeMode_Type") {
		external = true;
		cppName = "EResourceSizeMode::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Exclusive", "Exclusive", int64_t(EResourceSizeMode::Type::Exclusive), das::LineInfo());
		addIEx("EstimatedTotal", "EstimatedTotal", int64_t(EResourceSizeMode::Type::EstimatedTotal), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:56:6
class Enumeration_ELoadFlags : public das::Enumeration {
public:
	Enumeration_ELoadFlags() : das::Enumeration("ELoadFlags") {
		external = true;
		cppName = "ELoadFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("LOAD_None", "LOAD_None", int64_t(ELoadFlags::LOAD_None), das::LineInfo());
		addIEx("LOAD_Async", "LOAD_Async", int64_t(ELoadFlags::LOAD_Async), das::LineInfo());
		addIEx("LOAD_NoWarn", "LOAD_NoWarn", int64_t(ELoadFlags::LOAD_NoWarn), das::LineInfo());
		addIEx("LOAD_EditorOnly", "LOAD_EditorOnly", int64_t(ELoadFlags::LOAD_EditorOnly), das::LineInfo());
		addIEx("LOAD_ResolvingDeferredExports", "LOAD_ResolvingDeferredExports", int64_t(ELoadFlags::LOAD_ResolvingDeferredExports), das::LineInfo());
		addIEx("LOAD_Verify", "LOAD_Verify", int64_t(ELoadFlags::LOAD_Verify), das::LineInfo());
		addIEx("LOAD_NoVerify", "LOAD_NoVerify", int64_t(ELoadFlags::LOAD_NoVerify), das::LineInfo());
		addIEx("LOAD_IsVerifying", "LOAD_IsVerifying", int64_t(ELoadFlags::LOAD_IsVerifying), das::LineInfo());
		addIEx("LOAD_SkipLoadImportedPackages", "LOAD_SkipLoadImportedPackages", int64_t(ELoadFlags::LOAD_SkipLoadImportedPackages), das::LineInfo());
		addIEx("LOAD_RegenerateBulkDataGuids", "LOAD_RegenerateBulkDataGuids", int64_t(ELoadFlags::LOAD_RegenerateBulkDataGuids), das::LineInfo());
		addIEx("LOAD_DisableDependencyPreloading", "LOAD_DisableDependencyPreloading", int64_t(ELoadFlags::LOAD_DisableDependencyPreloading), das::LineInfo());
		addIEx("LOAD_Quiet", "LOAD_Quiet", int64_t(ELoadFlags::LOAD_Quiet), das::LineInfo());
		addIEx("LOAD_FindIfFail", "LOAD_FindIfFail", int64_t(ELoadFlags::LOAD_FindIfFail), das::LineInfo());
		addIEx("LOAD_MemoryReader", "LOAD_MemoryReader", int64_t(ELoadFlags::LOAD_MemoryReader), das::LineInfo());
		addIEx("LOAD_NoRedirects", "LOAD_NoRedirects", int64_t(ELoadFlags::LOAD_NoRedirects), das::LineInfo());
		addIEx("LOAD_ForDiff", "LOAD_ForDiff", int64_t(ELoadFlags::LOAD_ForDiff), das::LineInfo());
		addIEx("LOAD_PackageForPIE", "LOAD_PackageForPIE", int64_t(ELoadFlags::LOAD_PackageForPIE), das::LineInfo());
		addIEx("LOAD_DeferDependencyLoads", "LOAD_DeferDependencyLoads", int64_t(ELoadFlags::LOAD_DeferDependencyLoads), das::LineInfo());
		addIEx("LOAD_ForFileDiff", "LOAD_ForFileDiff", int64_t(ELoadFlags::LOAD_ForFileDiff), das::LineInfo());
		addIEx("LOAD_DisableCompileOnLoad", "LOAD_DisableCompileOnLoad", int64_t(ELoadFlags::LOAD_DisableCompileOnLoad), das::LineInfo());
		addIEx("LOAD_DisableEngineVersionChecks", "LOAD_DisableEngineVersionChecks", int64_t(ELoadFlags::LOAD_DisableEngineVersionChecks), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:85:6
class Enumeration_ESaveFlags : public das::Enumeration {
public:
	Enumeration_ESaveFlags() : das::Enumeration("ESaveFlags") {
		external = true;
		cppName = "ESaveFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("SAVE_None", "SAVE_None", int64_t(ESaveFlags::SAVE_None), das::LineInfo());
		addIEx("SAVE_NoError", "SAVE_NoError", int64_t(ESaveFlags::SAVE_NoError), das::LineInfo());
		addIEx("SAVE_FromAutosave", "SAVE_FromAutosave", int64_t(ESaveFlags::SAVE_FromAutosave), das::LineInfo());
		addIEx("SAVE_KeepDirty", "SAVE_KeepDirty", int64_t(ESaveFlags::SAVE_KeepDirty), das::LineInfo());
		addIEx("SAVE_KeepGUID", "SAVE_KeepGUID", int64_t(ESaveFlags::SAVE_KeepGUID), das::LineInfo());
		addIEx("SAVE_Async", "SAVE_Async", int64_t(ESaveFlags::SAVE_Async), das::LineInfo());
		addIEx("SAVE_Unversioned_Native", "SAVE_Unversioned_Native", int64_t(ESaveFlags::SAVE_Unversioned_Native), das::LineInfo());
		addIEx("SAVE_CutdownPackage", "SAVE_CutdownPackage", int64_t(ESaveFlags::SAVE_CutdownPackage), das::LineInfo());
		addIEx("SAVE_KeepEditorOnlyCookedPackages", "SAVE_KeepEditorOnlyCookedPackages", int64_t(ESaveFlags::SAVE_KeepEditorOnlyCookedPackages), das::LineInfo());
		addIEx("SAVE_Concurrent", "SAVE_Concurrent", int64_t(ESaveFlags::SAVE_Concurrent), das::LineInfo());
		addIEx("SAVE_CompareLinker", "SAVE_CompareLinker", int64_t(ESaveFlags::SAVE_CompareLinker), das::LineInfo());
		addIEx("SAVE_BulkDataByReference", "SAVE_BulkDataByReference", int64_t(ESaveFlags::SAVE_BulkDataByReference), das::LineInfo());
		addIEx("SAVE_Unversioned_Properties", "SAVE_Unversioned_Properties", int64_t(ESaveFlags::SAVE_Unversioned_Properties), das::LineInfo());
		addIEx("SAVE_Optional", "SAVE_Optional", int64_t(ESaveFlags::SAVE_Optional), das::LineInfo());
		addIEx("SAVE_AllowTimeout", "SAVE_AllowTimeout", int64_t(ESaveFlags::SAVE_AllowTimeout), das::LineInfo());
		addIEx("SAVE_RehydratePayloads", "SAVE_RehydratePayloads", int64_t(ESaveFlags::SAVE_RehydratePayloads), das::LineInfo());
		addIEx("SAVE_Unversioned", "SAVE_Unversioned", int64_t(ESaveFlags::SAVE_Unversioned), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:116:6
class Enumeration_EPackageFlags : public das::Enumeration {
public:
	Enumeration_EPackageFlags() : das::Enumeration("EPackageFlags") {
		external = true;
		cppName = "EPackageFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("PKG_None", "PKG_None", int64_t(EPackageFlags::PKG_None), das::LineInfo());
		addIEx("PKG_NewlyCreated", "PKG_NewlyCreated", int64_t(EPackageFlags::PKG_NewlyCreated), das::LineInfo());
		addIEx("PKG_ClientOptional", "PKG_ClientOptional", int64_t(EPackageFlags::PKG_ClientOptional), das::LineInfo());
		addIEx("PKG_ServerSideOnly", "PKG_ServerSideOnly", int64_t(EPackageFlags::PKG_ServerSideOnly), das::LineInfo());
		addIEx("PKG_CompiledIn", "PKG_CompiledIn", int64_t(EPackageFlags::PKG_CompiledIn), das::LineInfo());
		addIEx("PKG_ForDiffing", "PKG_ForDiffing", int64_t(EPackageFlags::PKG_ForDiffing), das::LineInfo());
		addIEx("PKG_EditorOnly", "PKG_EditorOnly", int64_t(EPackageFlags::PKG_EditorOnly), das::LineInfo());
		addIEx("PKG_Developer", "PKG_Developer", int64_t(EPackageFlags::PKG_Developer), das::LineInfo());
		addIEx("PKG_UncookedOnly", "PKG_UncookedOnly", int64_t(EPackageFlags::PKG_UncookedOnly), das::LineInfo());
		addIEx("PKG_Cooked", "PKG_Cooked", int64_t(EPackageFlags::PKG_Cooked), das::LineInfo());
		addIEx("PKG_ContainsNoAsset", "PKG_ContainsNoAsset", int64_t(EPackageFlags::PKG_ContainsNoAsset), das::LineInfo());
		addIEx("PKG_NotExternallyReferenceable", "PKG_NotExternallyReferenceable", int64_t(EPackageFlags::PKG_NotExternallyReferenceable), das::LineInfo());
		addIEx("PKG_UnversionedProperties", "PKG_UnversionedProperties", int64_t(EPackageFlags::PKG_UnversionedProperties), das::LineInfo());
		addIEx("PKG_ContainsMapData", "PKG_ContainsMapData", int64_t(EPackageFlags::PKG_ContainsMapData), das::LineInfo());
		addIEx("PKG_IsSaving", "PKG_IsSaving", int64_t(EPackageFlags::PKG_IsSaving), das::LineInfo());
		addIEx("PKG_Compiling", "PKG_Compiling", int64_t(EPackageFlags::PKG_Compiling), das::LineInfo());
		addIEx("PKG_ContainsMap", "PKG_ContainsMap", int64_t(EPackageFlags::PKG_ContainsMap), das::LineInfo());
		addIEx("PKG_RequiresLocalizationGather", "PKG_RequiresLocalizationGather", int64_t(EPackageFlags::PKG_RequiresLocalizationGather), das::LineInfo());
		addIEx("PKG_PlayInEditor", "PKG_PlayInEditor", int64_t(EPackageFlags::PKG_PlayInEditor), das::LineInfo());
		addIEx("PKG_ContainsScript", "PKG_ContainsScript", int64_t(EPackageFlags::PKG_ContainsScript), das::LineInfo());
		addIEx("PKG_DisallowExport", "PKG_DisallowExport", int64_t(EPackageFlags::PKG_DisallowExport), das::LineInfo());
		addIEx("PKG_CookGenerated", "PKG_CookGenerated", int64_t(EPackageFlags::PKG_CookGenerated), das::LineInfo());
		addIEx("PKG_DynamicImports", "PKG_DynamicImports", int64_t(EPackageFlags::PKG_DynamicImports), das::LineInfo());
		addIEx("PKG_RuntimeGenerated", "PKG_RuntimeGenerated", int64_t(EPackageFlags::PKG_RuntimeGenerated), das::LineInfo());
		addIEx("PKG_ReloadingForCooker", "PKG_ReloadingForCooker", int64_t(EPackageFlags::PKG_ReloadingForCooker), das::LineInfo());
		addIEx("PKG_FilterEditorOnly", "PKG_FilterEditorOnly", int64_t(EPackageFlags::PKG_FilterEditorOnly), das::LineInfo());
		addIEx("PKG_TransientFlags", "PKG_TransientFlags", int64_t(EPackageFlags::PKG_TransientFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:162:6
class Enumeration_EStaticConstructor : public das::Enumeration {
public:
	Enumeration_EStaticConstructor() : das::Enumeration("EStaticConstructor") {
		external = true;
		cppName = "EStaticConstructor";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("EC_StaticConstructor", "EC_StaticConstructor", int64_t(EStaticConstructor::EC_StaticConstructor), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:163:6
class Enumeration_EInternal : public das::Enumeration {
public:
	Enumeration_EInternal() : das::Enumeration("EInternal") {
		external = true;
		cppName = "EInternal";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("EC_InternalUseOnlyConstructor", "EC_InternalUseOnlyConstructor", int64_t(EInternal::EC_InternalUseOnlyConstructor), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:164:6
class Enumeration_ECppProperty : public das::Enumeration {
public:
	Enumeration_ECppProperty() : das::Enumeration("ECppProperty") {
		external = true;
		cppName = "ECppProperty";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("EC_CppProperty", "EC_CppProperty", int64_t(ECppProperty::EC_CppProperty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:186:6
class Enumeration_EClassFlags : public das::Enumeration {
public:
	Enumeration_EClassFlags() : das::Enumeration("EClassFlags") {
		external = true;
		cppName = "EClassFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("CLASS_None", "CLASS_None", int64_t(EClassFlags::CLASS_None), das::LineInfo());
		addIEx("CLASS_Abstract", "CLASS_Abstract", int64_t(EClassFlags::CLASS_Abstract), das::LineInfo());
		addIEx("CLASS_DefaultConfig", "CLASS_DefaultConfig", int64_t(EClassFlags::CLASS_DefaultConfig), das::LineInfo());
		addIEx("CLASS_Config", "CLASS_Config", int64_t(EClassFlags::CLASS_Config), das::LineInfo());
		addIEx("CLASS_Transient", "CLASS_Transient", int64_t(EClassFlags::CLASS_Transient), das::LineInfo());
		addIEx("CLASS_Optional", "CLASS_Optional", int64_t(EClassFlags::CLASS_Optional), das::LineInfo());
		addIEx("CLASS_MatchedSerializers", "CLASS_MatchedSerializers", int64_t(EClassFlags::CLASS_MatchedSerializers), das::LineInfo());
		addIEx("CLASS_ProjectUserConfig", "CLASS_ProjectUserConfig", int64_t(EClassFlags::CLASS_ProjectUserConfig), das::LineInfo());
		addIEx("CLASS_Native", "CLASS_Native", int64_t(EClassFlags::CLASS_Native), das::LineInfo());
		addIEx("CLASS_NotPlaceable", "CLASS_NotPlaceable", int64_t(EClassFlags::CLASS_NotPlaceable), das::LineInfo());
		addIEx("CLASS_PerObjectConfig", "CLASS_PerObjectConfig", int64_t(EClassFlags::CLASS_PerObjectConfig), das::LineInfo());
		addIEx("CLASS_ReplicationDataIsSetUp", "CLASS_ReplicationDataIsSetUp", int64_t(EClassFlags::CLASS_ReplicationDataIsSetUp), das::LineInfo());
		addIEx("CLASS_EditInlineNew", "CLASS_EditInlineNew", int64_t(EClassFlags::CLASS_EditInlineNew), das::LineInfo());
		addIEx("CLASS_CollapseCategories", "CLASS_CollapseCategories", int64_t(EClassFlags::CLASS_CollapseCategories), das::LineInfo());
		addIEx("CLASS_Interface", "CLASS_Interface", int64_t(EClassFlags::CLASS_Interface), das::LineInfo());
		addIEx("CLASS_Const", "CLASS_Const", int64_t(EClassFlags::CLASS_Const), das::LineInfo());
		addIEx("CLASS_NeedsDeferredDependencyLoading", "CLASS_NeedsDeferredDependencyLoading", int64_t(EClassFlags::CLASS_NeedsDeferredDependencyLoading), das::LineInfo());
		addIEx("CLASS_CompiledFromBlueprint", "CLASS_CompiledFromBlueprint", int64_t(EClassFlags::CLASS_CompiledFromBlueprint), das::LineInfo());
		addIEx("CLASS_MinimalAPI", "CLASS_MinimalAPI", int64_t(EClassFlags::CLASS_MinimalAPI), das::LineInfo());
		addIEx("CLASS_RequiredAPI", "CLASS_RequiredAPI", int64_t(EClassFlags::CLASS_RequiredAPI), das::LineInfo());
		addIEx("CLASS_DefaultToInstanced", "CLASS_DefaultToInstanced", int64_t(EClassFlags::CLASS_DefaultToInstanced), das::LineInfo());
		addIEx("CLASS_TokenStreamAssembled", "CLASS_TokenStreamAssembled", int64_t(EClassFlags::CLASS_TokenStreamAssembled), das::LineInfo());
		addIEx("CLASS_HasInstancedReference", "CLASS_HasInstancedReference", int64_t(EClassFlags::CLASS_HasInstancedReference), das::LineInfo());
		addIEx("CLASS_Hidden", "CLASS_Hidden", int64_t(EClassFlags::CLASS_Hidden), das::LineInfo());
		addIEx("CLASS_Deprecated", "CLASS_Deprecated", int64_t(EClassFlags::CLASS_Deprecated), das::LineInfo());
		addIEx("CLASS_HideDropDown", "CLASS_HideDropDown", int64_t(EClassFlags::CLASS_HideDropDown), das::LineInfo());
		addIEx("CLASS_GlobalUserConfig", "CLASS_GlobalUserConfig", int64_t(EClassFlags::CLASS_GlobalUserConfig), das::LineInfo());
		addIEx("CLASS_Intrinsic", "CLASS_Intrinsic", int64_t(EClassFlags::CLASS_Intrinsic), das::LineInfo());
		addIEx("CLASS_Constructed", "CLASS_Constructed", int64_t(EClassFlags::CLASS_Constructed), das::LineInfo());
		addIEx("CLASS_ConfigDoNotCheckDefaults", "CLASS_ConfigDoNotCheckDefaults", int64_t(EClassFlags::CLASS_ConfigDoNotCheckDefaults), das::LineInfo());
		addIEx("CLASS_NewerVersionExists", "CLASS_NewerVersionExists", int64_t(EClassFlags::CLASS_NewerVersionExists), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:317:6
class Enumeration_EClassCastFlags : public das::Enumeration {
public:
	Enumeration_EClassCastFlags() : das::Enumeration("EClassCastFlags") {
		external = true;
		cppName = "EClassCastFlags";
		baseType = (das::Type) das::ToBasicType<uint64>::type;
		addIEx("CASTCLASS_None", "CASTCLASS_None", int64_t(EClassCastFlags::CASTCLASS_None), das::LineInfo());
		addIEx("CASTCLASS_UField", "CASTCLASS_UField", int64_t(EClassCastFlags::CASTCLASS_UField), das::LineInfo());
		addIEx("CASTCLASS_FInt8Property", "CASTCLASS_FInt8Property", int64_t(EClassCastFlags::CASTCLASS_FInt8Property), das::LineInfo());
		addIEx("CASTCLASS_UEnum", "CASTCLASS_UEnum", int64_t(EClassCastFlags::CASTCLASS_UEnum), das::LineInfo());
		addIEx("CASTCLASS_UStruct", "CASTCLASS_UStruct", int64_t(EClassCastFlags::CASTCLASS_UStruct), das::LineInfo());
		addIEx("CASTCLASS_UScriptStruct", "CASTCLASS_UScriptStruct", int64_t(EClassCastFlags::CASTCLASS_UScriptStruct), das::LineInfo());
		addIEx("CASTCLASS_UClass", "CASTCLASS_UClass", int64_t(EClassCastFlags::CASTCLASS_UClass), das::LineInfo());
		addIEx("CASTCLASS_FByteProperty", "CASTCLASS_FByteProperty", int64_t(EClassCastFlags::CASTCLASS_FByteProperty), das::LineInfo());
		addIEx("CASTCLASS_FIntProperty", "CASTCLASS_FIntProperty", int64_t(EClassCastFlags::CASTCLASS_FIntProperty), das::LineInfo());
		addIEx("CASTCLASS_FFloatProperty", "CASTCLASS_FFloatProperty", int64_t(EClassCastFlags::CASTCLASS_FFloatProperty), das::LineInfo());
		addIEx("CASTCLASS_FUInt64Property", "CASTCLASS_FUInt64Property", int64_t(EClassCastFlags::CASTCLASS_FUInt64Property), das::LineInfo());
		addIEx("CASTCLASS_FClassProperty", "CASTCLASS_FClassProperty", int64_t(EClassCastFlags::CASTCLASS_FClassProperty), das::LineInfo());
		addIEx("CASTCLASS_FUInt32Property", "CASTCLASS_FUInt32Property", int64_t(EClassCastFlags::CASTCLASS_FUInt32Property), das::LineInfo());
		addIEx("CASTCLASS_FInterfaceProperty", "CASTCLASS_FInterfaceProperty", int64_t(EClassCastFlags::CASTCLASS_FInterfaceProperty), das::LineInfo());
		addIEx("CASTCLASS_FNameProperty", "CASTCLASS_FNameProperty", int64_t(EClassCastFlags::CASTCLASS_FNameProperty), das::LineInfo());
		addIEx("CASTCLASS_FStrProperty", "CASTCLASS_FStrProperty", int64_t(EClassCastFlags::CASTCLASS_FStrProperty), das::LineInfo());
		addIEx("CASTCLASS_FProperty", "CASTCLASS_FProperty", int64_t(EClassCastFlags::CASTCLASS_FProperty), das::LineInfo());
		addIEx("CASTCLASS_FObjectProperty", "CASTCLASS_FObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FBoolProperty", "CASTCLASS_FBoolProperty", int64_t(EClassCastFlags::CASTCLASS_FBoolProperty), das::LineInfo());
		addIEx("CASTCLASS_FUInt16Property", "CASTCLASS_FUInt16Property", int64_t(EClassCastFlags::CASTCLASS_FUInt16Property), das::LineInfo());
		addIEx("CASTCLASS_UFunction", "CASTCLASS_UFunction", int64_t(EClassCastFlags::CASTCLASS_UFunction), das::LineInfo());
		addIEx("CASTCLASS_FStructProperty", "CASTCLASS_FStructProperty", int64_t(EClassCastFlags::CASTCLASS_FStructProperty), das::LineInfo());
		addIEx("CASTCLASS_FArrayProperty", "CASTCLASS_FArrayProperty", int64_t(EClassCastFlags::CASTCLASS_FArrayProperty), das::LineInfo());
		addIEx("CASTCLASS_FInt64Property", "CASTCLASS_FInt64Property", int64_t(EClassCastFlags::CASTCLASS_FInt64Property), das::LineInfo());
		addIEx("CASTCLASS_FDelegateProperty", "CASTCLASS_FDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FNumericProperty", "CASTCLASS_FNumericProperty", int64_t(EClassCastFlags::CASTCLASS_FNumericProperty), das::LineInfo());
		addIEx("CASTCLASS_FMulticastDelegateProperty", "CASTCLASS_FMulticastDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FMulticastDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FObjectPropertyBase", "CASTCLASS_FObjectPropertyBase", int64_t(EClassCastFlags::CASTCLASS_FObjectPropertyBase), das::LineInfo());
		addIEx("CASTCLASS_FWeakObjectProperty", "CASTCLASS_FWeakObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FWeakObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FLazyObjectProperty", "CASTCLASS_FLazyObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FLazyObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FSoftObjectProperty", "CASTCLASS_FSoftObjectProperty", int64_t(EClassCastFlags::CASTCLASS_FSoftObjectProperty), das::LineInfo());
		addIEx("CASTCLASS_FTextProperty", "CASTCLASS_FTextProperty", int64_t(EClassCastFlags::CASTCLASS_FTextProperty), das::LineInfo());
		addIEx("CASTCLASS_FInt16Property", "CASTCLASS_FInt16Property", int64_t(EClassCastFlags::CASTCLASS_FInt16Property), das::LineInfo());
		addIEx("CASTCLASS_FDoubleProperty", "CASTCLASS_FDoubleProperty", int64_t(EClassCastFlags::CASTCLASS_FDoubleProperty), das::LineInfo());
		addIEx("CASTCLASS_FSoftClassProperty", "CASTCLASS_FSoftClassProperty", int64_t(EClassCastFlags::CASTCLASS_FSoftClassProperty), das::LineInfo());
		addIEx("CASTCLASS_UPackage", "CASTCLASS_UPackage", int64_t(EClassCastFlags::CASTCLASS_UPackage), das::LineInfo());
		addIEx("CASTCLASS_ULevel", "CASTCLASS_ULevel", int64_t(EClassCastFlags::CASTCLASS_ULevel), das::LineInfo());
		addIEx("CASTCLASS_AActor", "CASTCLASS_AActor", int64_t(EClassCastFlags::CASTCLASS_AActor), das::LineInfo());
		addIEx("CASTCLASS_APlayerController", "CASTCLASS_APlayerController", int64_t(EClassCastFlags::CASTCLASS_APlayerController), das::LineInfo());
		addIEx("CASTCLASS_APawn", "CASTCLASS_APawn", int64_t(EClassCastFlags::CASTCLASS_APawn), das::LineInfo());
		addIEx("CASTCLASS_USceneComponent", "CASTCLASS_USceneComponent", int64_t(EClassCastFlags::CASTCLASS_USceneComponent), das::LineInfo());
		addIEx("CASTCLASS_UPrimitiveComponent", "CASTCLASS_UPrimitiveComponent", int64_t(EClassCastFlags::CASTCLASS_UPrimitiveComponent), das::LineInfo());
		addIEx("CASTCLASS_USkinnedMeshComponent", "CASTCLASS_USkinnedMeshComponent", int64_t(EClassCastFlags::CASTCLASS_USkinnedMeshComponent), das::LineInfo());
		addIEx("CASTCLASS_USkeletalMeshComponent", "CASTCLASS_USkeletalMeshComponent", int64_t(EClassCastFlags::CASTCLASS_USkeletalMeshComponent), das::LineInfo());
		addIEx("CASTCLASS_UBlueprint", "CASTCLASS_UBlueprint", int64_t(EClassCastFlags::CASTCLASS_UBlueprint), das::LineInfo());
		addIEx("CASTCLASS_UDelegateFunction", "CASTCLASS_UDelegateFunction", int64_t(EClassCastFlags::CASTCLASS_UDelegateFunction), das::LineInfo());
		addIEx("CASTCLASS_UStaticMeshComponent", "CASTCLASS_UStaticMeshComponent", int64_t(EClassCastFlags::CASTCLASS_UStaticMeshComponent), das::LineInfo());
		addIEx("CASTCLASS_FMapProperty", "CASTCLASS_FMapProperty", int64_t(EClassCastFlags::CASTCLASS_FMapProperty), das::LineInfo());
		addIEx("CASTCLASS_FSetProperty", "CASTCLASS_FSetProperty", int64_t(EClassCastFlags::CASTCLASS_FSetProperty), das::LineInfo());
		addIEx("CASTCLASS_FEnumProperty", "CASTCLASS_FEnumProperty", int64_t(EClassCastFlags::CASTCLASS_FEnumProperty), das::LineInfo());
		addIEx("CASTCLASS_USparseDelegateFunction", "CASTCLASS_USparseDelegateFunction", int64_t(EClassCastFlags::CASTCLASS_USparseDelegateFunction), das::LineInfo());
		addIEx("CASTCLASS_FMulticastInlineDelegateProperty", "CASTCLASS_FMulticastInlineDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FMulticastInlineDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FMulticastSparseDelegateProperty", "CASTCLASS_FMulticastSparseDelegateProperty", int64_t(EClassCastFlags::CASTCLASS_FMulticastSparseDelegateProperty), das::LineInfo());
		addIEx("CASTCLASS_FFieldPathProperty", "CASTCLASS_FFieldPathProperty", int64_t(EClassCastFlags::CASTCLASS_FFieldPathProperty), das::LineInfo());
		addIEx("CASTCLASS_FObjectPtrProperty", "CASTCLASS_FObjectPtrProperty", int64_t(EClassCastFlags::CASTCLASS_FObjectPtrProperty), das::LineInfo());
		addIEx("CASTCLASS_FClassPtrProperty", "CASTCLASS_FClassPtrProperty", int64_t(EClassCastFlags::CASTCLASS_FClassPtrProperty), das::LineInfo());
		addIEx("CASTCLASS_FLargeWorldCoordinatesRealProperty", "CASTCLASS_FLargeWorldCoordinatesRealProperty", int64_t(EClassCastFlags::CASTCLASS_FLargeWorldCoordinatesRealProperty), das::LineInfo());
		addIEx("CASTCLASS_FOptionalProperty", "CASTCLASS_FOptionalProperty", int64_t(EClassCastFlags::CASTCLASS_FOptionalProperty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:393:6
class Enumeration_EPropertyFlags : public das::Enumeration {
public:
	Enumeration_EPropertyFlags() : das::Enumeration("EPropertyFlags") {
		external = true;
		cppName = "EPropertyFlags";
		baseType = (das::Type) das::ToBasicType<uint64>::type;
		addIEx("CPF_None", "CPF_None", int64_t(EPropertyFlags::CPF_None), das::LineInfo());
		addIEx("CPF_Edit", "CPF_Edit", int64_t(EPropertyFlags::CPF_Edit), das::LineInfo());
		addIEx("CPF_ConstParm", "CPF_ConstParm", int64_t(EPropertyFlags::CPF_ConstParm), das::LineInfo());
		addIEx("CPF_BlueprintVisible", "CPF_BlueprintVisible", int64_t(EPropertyFlags::CPF_BlueprintVisible), das::LineInfo());
		addIEx("CPF_ExportObject", "CPF_ExportObject", int64_t(EPropertyFlags::CPF_ExportObject), das::LineInfo());
		addIEx("CPF_BlueprintReadOnly", "CPF_BlueprintReadOnly", int64_t(EPropertyFlags::CPF_BlueprintReadOnly), das::LineInfo());
		addIEx("CPF_Net", "CPF_Net", int64_t(EPropertyFlags::CPF_Net), das::LineInfo());
		addIEx("CPF_EditFixedSize", "CPF_EditFixedSize", int64_t(EPropertyFlags::CPF_EditFixedSize), das::LineInfo());
		addIEx("CPF_Parm", "CPF_Parm", int64_t(EPropertyFlags::CPF_Parm), das::LineInfo());
		addIEx("CPF_OutParm", "CPF_OutParm", int64_t(EPropertyFlags::CPF_OutParm), das::LineInfo());
		addIEx("CPF_ZeroConstructor", "CPF_ZeroConstructor", int64_t(EPropertyFlags::CPF_ZeroConstructor), das::LineInfo());
		addIEx("CPF_ReturnParm", "CPF_ReturnParm", int64_t(EPropertyFlags::CPF_ReturnParm), das::LineInfo());
		addIEx("CPF_DisableEditOnTemplate", "CPF_DisableEditOnTemplate", int64_t(EPropertyFlags::CPF_DisableEditOnTemplate), das::LineInfo());
		addIEx("CPF_NonNullable", "CPF_NonNullable", int64_t(EPropertyFlags::CPF_NonNullable), das::LineInfo());
		addIEx("CPF_Transient", "CPF_Transient", int64_t(EPropertyFlags::CPF_Transient), das::LineInfo());
		addIEx("CPF_Config", "CPF_Config", int64_t(EPropertyFlags::CPF_Config), das::LineInfo());
		addIEx("CPF_RequiredParm", "CPF_RequiredParm", int64_t(EPropertyFlags::CPF_RequiredParm), das::LineInfo());
		addIEx("CPF_DisableEditOnInstance", "CPF_DisableEditOnInstance", int64_t(EPropertyFlags::CPF_DisableEditOnInstance), das::LineInfo());
		addIEx("CPF_EditConst", "CPF_EditConst", int64_t(EPropertyFlags::CPF_EditConst), das::LineInfo());
		addIEx("CPF_GlobalConfig", "CPF_GlobalConfig", int64_t(EPropertyFlags::CPF_GlobalConfig), das::LineInfo());
		addIEx("CPF_InstancedReference", "CPF_InstancedReference", int64_t(EPropertyFlags::CPF_InstancedReference), das::LineInfo());
		addIEx("CPF_DuplicateTransient", "CPF_DuplicateTransient", int64_t(EPropertyFlags::CPF_DuplicateTransient), das::LineInfo());
		addIEx("CPF_SaveGame", "CPF_SaveGame", int64_t(EPropertyFlags::CPF_SaveGame), das::LineInfo());
		addIEx("CPF_NoClear", "CPF_NoClear", int64_t(EPropertyFlags::CPF_NoClear), das::LineInfo());
		addIEx("CPF_ReferenceParm", "CPF_ReferenceParm", int64_t(EPropertyFlags::CPF_ReferenceParm), das::LineInfo());
		addIEx("CPF_BlueprintAssignable", "CPF_BlueprintAssignable", int64_t(EPropertyFlags::CPF_BlueprintAssignable), das::LineInfo());
		addIEx("CPF_Deprecated", "CPF_Deprecated", int64_t(EPropertyFlags::CPF_Deprecated), das::LineInfo());
		addIEx("CPF_IsPlainOldData", "CPF_IsPlainOldData", int64_t(EPropertyFlags::CPF_IsPlainOldData), das::LineInfo());
		addIEx("CPF_RepSkip", "CPF_RepSkip", int64_t(EPropertyFlags::CPF_RepSkip), das::LineInfo());
		addIEx("CPF_RepNotify", "CPF_RepNotify", int64_t(EPropertyFlags::CPF_RepNotify), das::LineInfo());
		addIEx("CPF_Interp", "CPF_Interp", int64_t(EPropertyFlags::CPF_Interp), das::LineInfo());
		addIEx("CPF_NonTransactional", "CPF_NonTransactional", int64_t(EPropertyFlags::CPF_NonTransactional), das::LineInfo());
		addIEx("CPF_EditorOnly", "CPF_EditorOnly", int64_t(EPropertyFlags::CPF_EditorOnly), das::LineInfo());
		addIEx("CPF_NoDestructor", "CPF_NoDestructor", int64_t(EPropertyFlags::CPF_NoDestructor), das::LineInfo());
		addIEx("CPF_AutoWeak", "CPF_AutoWeak", int64_t(EPropertyFlags::CPF_AutoWeak), das::LineInfo());
		addIEx("CPF_ContainsInstancedReference", "CPF_ContainsInstancedReference", int64_t(EPropertyFlags::CPF_ContainsInstancedReference), das::LineInfo());
		addIEx("CPF_AssetRegistrySearchable", "CPF_AssetRegistrySearchable", int64_t(EPropertyFlags::CPF_AssetRegistrySearchable), das::LineInfo());
		addIEx("CPF_SimpleDisplay", "CPF_SimpleDisplay", int64_t(EPropertyFlags::CPF_SimpleDisplay), das::LineInfo());
		addIEx("CPF_AdvancedDisplay", "CPF_AdvancedDisplay", int64_t(EPropertyFlags::CPF_AdvancedDisplay), das::LineInfo());
		addIEx("CPF_Protected", "CPF_Protected", int64_t(EPropertyFlags::CPF_Protected), das::LineInfo());
		addIEx("CPF_BlueprintCallable", "CPF_BlueprintCallable", int64_t(EPropertyFlags::CPF_BlueprintCallable), das::LineInfo());
		addIEx("CPF_BlueprintAuthorityOnly", "CPF_BlueprintAuthorityOnly", int64_t(EPropertyFlags::CPF_BlueprintAuthorityOnly), das::LineInfo());
		addIEx("CPF_TextExportTransient", "CPF_TextExportTransient", int64_t(EPropertyFlags::CPF_TextExportTransient), das::LineInfo());
		addIEx("CPF_NonPIEDuplicateTransient", "CPF_NonPIEDuplicateTransient", int64_t(EPropertyFlags::CPF_NonPIEDuplicateTransient), das::LineInfo());
		addIEx("CPF_ExposeOnSpawn", "CPF_ExposeOnSpawn", int64_t(EPropertyFlags::CPF_ExposeOnSpawn), das::LineInfo());
		addIEx("CPF_PersistentInstance", "CPF_PersistentInstance", int64_t(EPropertyFlags::CPF_PersistentInstance), das::LineInfo());
		addIEx("CPF_UObjectWrapper", "CPF_UObjectWrapper", int64_t(EPropertyFlags::CPF_UObjectWrapper), das::LineInfo());
		addIEx("CPF_HasGetValueTypeHash", "CPF_HasGetValueTypeHash", int64_t(EPropertyFlags::CPF_HasGetValueTypeHash), das::LineInfo());
		addIEx("CPF_NativeAccessSpecifierPublic", "CPF_NativeAccessSpecifierPublic", int64_t(EPropertyFlags::CPF_NativeAccessSpecifierPublic), das::LineInfo());
		addIEx("CPF_NativeAccessSpecifierProtected", "CPF_NativeAccessSpecifierProtected", int64_t(EPropertyFlags::CPF_NativeAccessSpecifierProtected), das::LineInfo());
		addIEx("CPF_NativeAccessSpecifierPrivate", "CPF_NativeAccessSpecifierPrivate", int64_t(EPropertyFlags::CPF_NativeAccessSpecifierPrivate), das::LineInfo());
		addIEx("CPF_SkipSerialization", "CPF_SkipSerialization", int64_t(EPropertyFlags::CPF_SkipSerialization), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:484:12
class Enumeration_EArrayPropertyFlags : public das::Enumeration {
public:
	Enumeration_EArrayPropertyFlags() : das::Enumeration("EArrayPropertyFlags") {
		external = true;
		cppName = "EArrayPropertyFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EArrayPropertyFlags::None), das::LineInfo());
		addIEx("UsesMemoryImageAllocator", "UsesMemoryImageAllocator", int64_t(EArrayPropertyFlags::UsesMemoryImageAllocator), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:495:12
class Enumeration_EMapPropertyFlags : public das::Enumeration {
public:
	Enumeration_EMapPropertyFlags() : das::Enumeration("EMapPropertyFlags") {
		external = true;
		cppName = "EMapPropertyFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EMapPropertyFlags::None), das::LineInfo());
		addIEx("UsesMemoryImageAllocator", "UsesMemoryImageAllocator", int64_t(EMapPropertyFlags::UsesMemoryImageAllocator), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:503:12
class Enumeration_EPropertyObjectReferenceType : public das::Enumeration {
public:
	Enumeration_EPropertyObjectReferenceType() : das::Enumeration("EPropertyObjectReferenceType") {
		external = true;
		cppName = "EPropertyObjectReferenceType";
		baseType = (das::Type) das::ToBasicType<uint32>::type;
		addIEx("None", "None", int64_t(EPropertyObjectReferenceType::None), das::LineInfo());
		addIEx("Strong", "Strong", int64_t(EPropertyObjectReferenceType::Strong), das::LineInfo());
		addIEx("Weak", "Weak", int64_t(EPropertyObjectReferenceType::Weak), das::LineInfo());
		addIEx("Soft", "Soft", int64_t(EPropertyObjectReferenceType::Soft), das::LineInfo());
		addIEx("Conservative", "Conservative", int64_t(EPropertyObjectReferenceType::Conservative), das::LineInfo());
		addIEx("MAX", "MAX", int64_t(EPropertyObjectReferenceType::MAX), das::LineInfo());
		addIEx("Any", "Any", int64_t(EPropertyObjectReferenceType::Any), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:521:6
class Enumeration_EObjectFlags : public das::Enumeration {
public:
	Enumeration_EObjectFlags() : das::Enumeration("EObjectFlags") {
		external = true;
		cppName = "EObjectFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("RF_NoFlags", "RF_NoFlags", int64_t(EObjectFlags::RF_NoFlags), das::LineInfo());
		addIEx("RF_Public", "RF_Public", int64_t(EObjectFlags::RF_Public), das::LineInfo());
		addIEx("RF_Standalone", "RF_Standalone", int64_t(EObjectFlags::RF_Standalone), das::LineInfo());
		addIEx("RF_MarkAsNative", "RF_MarkAsNative", int64_t(EObjectFlags::RF_MarkAsNative), das::LineInfo());
		addIEx("RF_Transactional", "RF_Transactional", int64_t(EObjectFlags::RF_Transactional), das::LineInfo());
		addIEx("RF_ClassDefaultObject", "RF_ClassDefaultObject", int64_t(EObjectFlags::RF_ClassDefaultObject), das::LineInfo());
		addIEx("RF_ArchetypeObject", "RF_ArchetypeObject", int64_t(EObjectFlags::RF_ArchetypeObject), das::LineInfo());
		addIEx("RF_Transient", "RF_Transient", int64_t(EObjectFlags::RF_Transient), das::LineInfo());
		addIEx("RF_MarkAsRootSet", "RF_MarkAsRootSet", int64_t(EObjectFlags::RF_MarkAsRootSet), das::LineInfo());
		addIEx("RF_TagGarbageTemp", "RF_TagGarbageTemp", int64_t(EObjectFlags::RF_TagGarbageTemp), das::LineInfo());
		addIEx("RF_NeedInitialization", "RF_NeedInitialization", int64_t(EObjectFlags::RF_NeedInitialization), das::LineInfo());
		addIEx("RF_NeedLoad", "RF_NeedLoad", int64_t(EObjectFlags::RF_NeedLoad), das::LineInfo());
		addIEx("RF_KeepForCooker", "RF_KeepForCooker", int64_t(EObjectFlags::RF_KeepForCooker), das::LineInfo());
		addIEx("RF_NeedPostLoad", "RF_NeedPostLoad", int64_t(EObjectFlags::RF_NeedPostLoad), das::LineInfo());
		addIEx("RF_NeedPostLoadSubobjects", "RF_NeedPostLoadSubobjects", int64_t(EObjectFlags::RF_NeedPostLoadSubobjects), das::LineInfo());
		addIEx("RF_NewerVersionExists", "RF_NewerVersionExists", int64_t(EObjectFlags::RF_NewerVersionExists), das::LineInfo());
		addIEx("RF_BeginDestroyed", "RF_BeginDestroyed", int64_t(EObjectFlags::RF_BeginDestroyed), das::LineInfo());
		addIEx("RF_FinishDestroyed", "RF_FinishDestroyed", int64_t(EObjectFlags::RF_FinishDestroyed), das::LineInfo());
		addIEx("RF_BeingRegenerated", "RF_BeingRegenerated", int64_t(EObjectFlags::RF_BeingRegenerated), das::LineInfo());
		addIEx("RF_DefaultSubObject", "RF_DefaultSubObject", int64_t(EObjectFlags::RF_DefaultSubObject), das::LineInfo());
		addIEx("RF_WasLoaded", "RF_WasLoaded", int64_t(EObjectFlags::RF_WasLoaded), das::LineInfo());
		addIEx("RF_TextExportTransient", "RF_TextExportTransient", int64_t(EObjectFlags::RF_TextExportTransient), das::LineInfo());
		addIEx("RF_LoadCompleted", "RF_LoadCompleted", int64_t(EObjectFlags::RF_LoadCompleted), das::LineInfo());
		addIEx("RF_InheritableComponentTemplate", "RF_InheritableComponentTemplate", int64_t(EObjectFlags::RF_InheritableComponentTemplate), das::LineInfo());
		addIEx("RF_DuplicateTransient", "RF_DuplicateTransient", int64_t(EObjectFlags::RF_DuplicateTransient), das::LineInfo());
		addIEx("RF_StrongRefOnFrame", "RF_StrongRefOnFrame", int64_t(EObjectFlags::RF_StrongRefOnFrame), das::LineInfo());
		addIEx("RF_NonPIEDuplicateTransient", "RF_NonPIEDuplicateTransient", int64_t(EObjectFlags::RF_NonPIEDuplicateTransient), das::LineInfo());
		addIEx("RF_WillBeLoaded", "RF_WillBeLoaded", int64_t(EObjectFlags::RF_WillBeLoaded), das::LineInfo());
		addIEx("RF_HasExternalPackage", "RF_HasExternalPackage", int64_t(EObjectFlags::RF_HasExternalPackage), das::LineInfo());
		addIEx("RF_AllocatedInSharedPage", "RF_AllocatedInSharedPage", int64_t(EObjectFlags::RF_AllocatedInSharedPage), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:598:12
class Enumeration_EInternalObjectFlags : public das::Enumeration {
public:
	Enumeration_EInternalObjectFlags() : das::Enumeration("EInternalObjectFlags") {
		external = true;
		cppName = "EInternalObjectFlags";
		baseType = (das::Type) das::ToBasicType<int32>::type;
		addIEx("None", "None", int64_t(EInternalObjectFlags::None), das::LineInfo());
		addIEx("LoaderImport", "LoaderImport", int64_t(EInternalObjectFlags::LoaderImport), das::LineInfo());
		addIEx("Garbage", "Garbage", int64_t(EInternalObjectFlags::Garbage), das::LineInfo());
		addIEx("ReachableInCluster", "ReachableInCluster", int64_t(EInternalObjectFlags::ReachableInCluster), das::LineInfo());
		addIEx("ClusterRoot", "ClusterRoot", int64_t(EInternalObjectFlags::ClusterRoot), das::LineInfo());
		addIEx("Native", "Native", int64_t(EInternalObjectFlags::Native), das::LineInfo());
		addIEx("Async", "Async", int64_t(EInternalObjectFlags::Async), das::LineInfo());
		addIEx("AsyncLoading", "AsyncLoading", int64_t(EInternalObjectFlags::AsyncLoading), das::LineInfo());
		addIEx("Unreachable", "Unreachable", int64_t(EInternalObjectFlags::Unreachable), das::LineInfo());
		addIEx("RootSet", "RootSet", int64_t(EInternalObjectFlags::RootSet), das::LineInfo());
		addIEx("PendingConstruction", "PendingConstruction", int64_t(EInternalObjectFlags::PendingConstruction), das::LineInfo());
		addIEx("GarbageCollectionKeepFlags", "GarbageCollectionKeepFlags", int64_t(EInternalObjectFlags::GarbageCollectionKeepFlags), das::LineInfo());
		addIEx("MirroredFlags", "MirroredFlags", int64_t(EInternalObjectFlags::MirroredFlags), das::LineInfo());
		addIEx("AllFlags", "AllFlags", int64_t(EInternalObjectFlags::AllFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:630:12
class Enumeration_EEnumFlags : public das::Enumeration {
public:
	Enumeration_EEnumFlags() : das::Enumeration("EEnumFlags") {
		external = true;
		cppName = "EEnumFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EEnumFlags::None), das::LineInfo());
		addIEx("Flags", "Flags", int64_t(EEnumFlags::Flags), das::LineInfo());
		addIEx("NewerVersionExists", "NewerVersionExists", int64_t(EEnumFlags::NewerVersionExists), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/ObjectMacros.h:2129:7
class Enumeration_UE_ELoadConfigPropagationFlags : public das::Enumeration {
public:
	Enumeration_UE_ELoadConfigPropagationFlags() : das::Enumeration("UE_ELoadConfigPropagationFlags") {
		external = true;
		cppName = "UE::ELoadConfigPropagationFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("LCPF_None", "LCPF_None", int64_t(UE::ELoadConfigPropagationFlags::LCPF_None), das::LineInfo());
		addIEx("LCPF_ReadParentSections", "LCPF_ReadParentSections", int64_t(UE::ELoadConfigPropagationFlags::LCPF_ReadParentSections), das::LineInfo());
		addIEx("LCPF_PropagateToChildDefaultObjects", "LCPF_PropagateToChildDefaultObjects", int64_t(UE::ELoadConfigPropagationFlags::LCPF_PropagateToChildDefaultObjects), das::LineInfo());
		addIEx("LCPF_PropagateToInstances", "LCPF_PropagateToInstances", int64_t(UE::ELoadConfigPropagationFlags::LCPF_PropagateToInstances), das::LineInfo());
		addIEx("LCPF_ReloadingConfigData", "LCPF_ReloadingConfigData", int64_t(UE::ELoadConfigPropagationFlags::LCPF_ReloadingConfigData), das::LineInfo());
		addIEx("LCPF_PersistentFlags", "LCPF_PersistentFlags", int64_t(UE::ELoadConfigPropagationFlags::LCPF_PersistentFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:115:7
class Enumeration_EDuplicateMode_Type : public das::Enumeration {
public:
	Enumeration_EDuplicateMode_Type() : das::Enumeration("EDuplicateMode_Type") {
		external = true;
		cppName = "EDuplicateMode::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Normal", "Normal", int64_t(EDuplicateMode::Type::Normal), das::LineInfo());
		addIEx("World", "World", int64_t(EDuplicateMode::Type::World), das::LineInfo());
		addIEx("PIE", "PIE", int64_t(EDuplicateMode::Type::PIE), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:461:12
class Enumeration_EFindFirstObjectOptions : public das::Enumeration {
public:
	Enumeration_EFindFirstObjectOptions() : das::Enumeration("EFindFirstObjectOptions") {
		external = true;
		cppName = "EFindFirstObjectOptions";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EFindFirstObjectOptions::None), das::LineInfo());
		addIEx("ExactClass", "ExactClass", int64_t(EFindFirstObjectOptions::ExactClass), das::LineInfo());
		addIEx("NativeFirst", "NativeFirst", int64_t(EFindFirstObjectOptions::NativeFirst), das::LineInfo());
		addIEx("EnsureIfAmbiguous", "EnsureIfAmbiguous", int64_t(EFindFirstObjectOptions::EnsureIfAmbiguous), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:502:12
class Enumeration_EParseObjectLoadingPolicy : public das::Enumeration {
public:
	Enumeration_EParseObjectLoadingPolicy() : das::Enumeration("EParseObjectLoadingPolicy") {
		external = true;
		cppName = "EParseObjectLoadingPolicy";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("Find", "Find", int64_t(EParseObjectLoadingPolicy::Find), das::LineInfo());
		addIEx("FindOrLoad", "FindOrLoad", int64_t(EParseObjectLoadingPolicy::FindOrLoad), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:647:7
class Enumeration_EAsyncLoadingResult_Type : public das::Enumeration {
public:
	Enumeration_EAsyncLoadingResult_Type() : das::Enumeration("EAsyncLoadingResult_Type") {
		external = true;
		cppName = "EAsyncLoadingResult::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Failed", "Failed", int64_t(EAsyncLoadingResult::Type::Failed), das::LineInfo());
		addIEx("Succeeded", "Succeeded", int64_t(EAsyncLoadingResult::Type::Succeeded), das::LineInfo());
		addIEx("Canceled", "Canceled", int64_t(EAsyncLoadingResult::Type::Canceled), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:799:12
class Enumeration_EUniqueObjectNameOptions : public das::Enumeration {
public:
	Enumeration_EUniqueObjectNameOptions() : das::Enumeration("EUniqueObjectNameOptions") {
		external = true;
		cppName = "EUniqueObjectNameOptions";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EUniqueObjectNameOptions::None), das::LineInfo());
		addIEx("GloballyUnique", "GloballyUnique", int64_t(EUniqueObjectNameOptions::GloballyUnique), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:885:7
class Enumeration_EAsyncPackageState_Type : public das::Enumeration {
public:
	Enumeration_EAsyncPackageState_Type() : das::Enumeration("EAsyncPackageState_Type") {
		external = true;
		cppName = "EAsyncPackageState::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("TimeOut", "TimeOut", int64_t(EAsyncPackageState::Type::TimeOut), das::LineInfo());
		addIEx("PendingImports", "PendingImports", int64_t(EAsyncPackageState::Type::PendingImports), das::LineInfo());
		addIEx("Complete", "Complete", int64_t(EAsyncPackageState::Type::Complete), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:1021:12
class Enumeration_EObjectInitializerOptions : public das::Enumeration {
public:
	Enumeration_EObjectInitializerOptions() : das::Enumeration("EObjectInitializerOptions") {
		external = true;
		cppName = "EObjectInitializerOptions";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EObjectInitializerOptions::None), das::LineInfo());
		addIEx("CopyTransientsFromClassDefaults", "CopyTransientsFromClassDefaults", int64_t(EObjectInitializerOptions::CopyTransientsFromClassDefaults), das::LineInfo());
		addIEx("InitializeProperties", "InitializeProperties", int64_t(EObjectInitializerOptions::InitializeProperties), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2829:12
class Enumeration_EPackageReloadPhase : public das::Enumeration {
public:
	Enumeration_EPackageReloadPhase() : das::Enumeration("EPackageReloadPhase") {
		external = true;
		cppName = "EPackageReloadPhase";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2833:12
class Enumeration_EHotReloadedClassFlags : public das::Enumeration {
public:
	Enumeration_EHotReloadedClassFlags() : das::Enumeration("EHotReloadedClassFlags") {
		external = true;
		cppName = "EHotReloadedClassFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EHotReloadedClassFlags::None), das::LineInfo());
		addIEx("Changed", "Changed", int64_t(EHotReloadedClassFlags::Changed), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:2843:12
class Enumeration_EReloadCompleteReason : public das::Enumeration {
public:
	Enumeration_EReloadCompleteReason() : das::Enumeration("EReloadCompleteReason") {
		external = true;
		cppName = "EReloadCompleteReason";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EReloadCompleteReason::None), das::LineInfo());
		addIEx("HotReloadAutomatic", "HotReloadAutomatic", int64_t(EReloadCompleteReason::HotReloadAutomatic), das::LineInfo());
		addIEx("HotReloadManual", "HotReloadManual", int64_t(EReloadCompleteReason::HotReloadManual), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3064:85
class Enumeration_EConstructDynamicType : public das::Enumeration {
public:
	Enumeration_EConstructDynamicType() : das::Enumeration("EConstructDynamicType") {
		external = true;
		cppName = "EConstructDynamicType";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("OnlyAllocateClassObject", "OnlyAllocateClassObject", int64_t(EConstructDynamicType::OnlyAllocateClassObject), das::LineInfo());
		addIEx("CallZConstructor", "CallZConstructor", int64_t(EConstructDynamicType::CallZConstructor), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3226:13
class Enumeration_UECodeGen_Private_EPropertyGenFlags : public das::Enumeration {
public:
	Enumeration_UECodeGen_Private_EPropertyGenFlags() : das::Enumeration("UECodeGen_Private_EPropertyGenFlags") {
		external = true;
		cppName = "UECodeGen_Private::EPropertyGenFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(UECodeGen_Private::EPropertyGenFlags::None), das::LineInfo());
		addIEx("Byte", "Byte", int64_t(UECodeGen_Private::EPropertyGenFlags::Byte), das::LineInfo());
		addIEx("Int8", "Int8", int64_t(UECodeGen_Private::EPropertyGenFlags::Int8), das::LineInfo());
		addIEx("Int16", "Int16", int64_t(UECodeGen_Private::EPropertyGenFlags::Int16), das::LineInfo());
		addIEx("Int", "Int", int64_t(UECodeGen_Private::EPropertyGenFlags::Int), das::LineInfo());
		addIEx("Int64", "Int64", int64_t(UECodeGen_Private::EPropertyGenFlags::Int64), das::LineInfo());
		addIEx("UInt16", "UInt16", int64_t(UECodeGen_Private::EPropertyGenFlags::UInt16), das::LineInfo());
		addIEx("UInt32", "UInt32", int64_t(UECodeGen_Private::EPropertyGenFlags::UInt32), das::LineInfo());
		addIEx("UInt64", "UInt64", int64_t(UECodeGen_Private::EPropertyGenFlags::UInt64), das::LineInfo());
		addIEx("Float", "Float", int64_t(UECodeGen_Private::EPropertyGenFlags::Float), das::LineInfo());
		addIEx("Double", "Double", int64_t(UECodeGen_Private::EPropertyGenFlags::Double), das::LineInfo());
		addIEx("Bool", "Bool", int64_t(UECodeGen_Private::EPropertyGenFlags::Bool), das::LineInfo());
		addIEx("SoftClass", "SoftClass", int64_t(UECodeGen_Private::EPropertyGenFlags::SoftClass), das::LineInfo());
		addIEx("WeakObject", "WeakObject", int64_t(UECodeGen_Private::EPropertyGenFlags::WeakObject), das::LineInfo());
		addIEx("LazyObject", "LazyObject", int64_t(UECodeGen_Private::EPropertyGenFlags::LazyObject), das::LineInfo());
		addIEx("SoftObject", "SoftObject", int64_t(UECodeGen_Private::EPropertyGenFlags::SoftObject), das::LineInfo());
		addIEx("Class", "Class", int64_t(UECodeGen_Private::EPropertyGenFlags::Class), das::LineInfo());
		addIEx("Object", "Object", int64_t(UECodeGen_Private::EPropertyGenFlags::Object), das::LineInfo());
		addIEx("Interface", "Interface", int64_t(UECodeGen_Private::EPropertyGenFlags::Interface), das::LineInfo());
		addIEx("Name", "Name", int64_t(UECodeGen_Private::EPropertyGenFlags::Name), das::LineInfo());
		addIEx("Str", "Str", int64_t(UECodeGen_Private::EPropertyGenFlags::Str), das::LineInfo());
		addIEx("Array", "Array", int64_t(UECodeGen_Private::EPropertyGenFlags::Array), das::LineInfo());
		addIEx("Map", "Map", int64_t(UECodeGen_Private::EPropertyGenFlags::Map), das::LineInfo());
		addIEx("Set", "Set", int64_t(UECodeGen_Private::EPropertyGenFlags::Set), das::LineInfo());
		addIEx("Struct", "Struct", int64_t(UECodeGen_Private::EPropertyGenFlags::Struct), das::LineInfo());
		addIEx("Delegate", "Delegate", int64_t(UECodeGen_Private::EPropertyGenFlags::Delegate), das::LineInfo());
		addIEx("InlineMulticastDelegate", "InlineMulticastDelegate", int64_t(UECodeGen_Private::EPropertyGenFlags::InlineMulticastDelegate), das::LineInfo());
		addIEx("SparseMulticastDelegate", "SparseMulticastDelegate", int64_t(UECodeGen_Private::EPropertyGenFlags::SparseMulticastDelegate), das::LineInfo());
		addIEx("Text", "Text", int64_t(UECodeGen_Private::EPropertyGenFlags::Text), das::LineInfo());
		addIEx("Enum", "Enum", int64_t(UECodeGen_Private::EPropertyGenFlags::Enum), das::LineInfo());
		addIEx("FieldPath", "FieldPath", int64_t(UECodeGen_Private::EPropertyGenFlags::FieldPath), das::LineInfo());
		addIEx("LargeWorldCoordinatesReal", "LargeWorldCoordinatesReal", int64_t(UECodeGen_Private::EPropertyGenFlags::LargeWorldCoordinatesReal), das::LineInfo());
		addIEx("NativeBool", "NativeBool", int64_t(UECodeGen_Private::EPropertyGenFlags::NativeBool), das::LineInfo());
		addIEx("ObjectPtr", "ObjectPtr", int64_t(UECodeGen_Private::EPropertyGenFlags::ObjectPtr), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectGlobals.h:3769:12
class Enumeration_EDataValidationResult : public das::Enumeration {
public:
	Enumeration_EDataValidationResult() : das::Enumeration("EDataValidationResult") {
		external = true;
		cppName = "EDataValidationResult";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("Invalid", "Invalid", int64_t(EDataValidationResult::Invalid), das::LineInfo());
		addIEx("Valid", "Valid", int64_t(EDataValidationResult::Valid), das::LineInfo());
		addIEx("NotValidated", "NotValidated", int64_t(EDataValidationResult::NotValidated), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectMarks.h:45:6
class Enumeration_EObjectMark : public das::Enumeration {
public:
	Enumeration_EObjectMark() : das::Enumeration("EObjectMark") {
		external = true;
		cppName = "EObjectMark";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("OBJECTMARK_NOMARKS", "OBJECTMARK_NOMARKS", int64_t(EObjectMark::OBJECTMARK_NOMARKS), das::LineInfo());
		addIEx("OBJECTMARK_Saved", "OBJECTMARK_Saved", int64_t(EObjectMark::OBJECTMARK_Saved), das::LineInfo());
		addIEx("OBJECTMARK_TagImp", "OBJECTMARK_TagImp", int64_t(EObjectMark::OBJECTMARK_TagImp), das::LineInfo());
		addIEx("OBJECTMARK_TagExp", "OBJECTMARK_TagExp", int64_t(EObjectMark::OBJECTMARK_TagExp), das::LineInfo());
		addIEx("OBJECTMARK_NotForClient", "OBJECTMARK_NotForClient", int64_t(EObjectMark::OBJECTMARK_NotForClient), das::LineInfo());
		addIEx("OBJECTMARK_NotForServer", "OBJECTMARK_NotForServer", int64_t(EObjectMark::OBJECTMARK_NotForServer), das::LineInfo());
		addIEx("OBJECTMARK_NotAlwaysLoadedForEditorGame", "OBJECTMARK_NotAlwaysLoadedForEditorGame", int64_t(EObjectMark::OBJECTMARK_NotAlwaysLoadedForEditorGame), das::LineInfo());
		addIEx("OBJECTMARK_EditorOnly", "OBJECTMARK_EditorOnly", int64_t(EObjectMark::OBJECTMARK_EditorOnly), das::LineInfo());
		addIEx("OBJECTMARK_NotForTargetPlatform", "OBJECTMARK_NotForTargetPlatform", int64_t(EObjectMark::OBJECTMARK_NotForTargetPlatform), das::LineInfo());
		addIEx("OBJECTMARK_INHERITEDMARKS", "OBJECTMARK_INHERITEDMARKS", int64_t(EObjectMark::OBJECTMARK_INHERITEDMARKS), das::LineInfo());
		addIEx("OBJECTMARK_ALLMARKS", "OBJECTMARK_ALLMARKS", int64_t(EObjectMark::OBJECTMARK_ALLMARKS), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UObjectBaseUtility.h:43:12
class Enumeration_EObjectFullNameFlags : public das::Enumeration {
public:
	Enumeration_EObjectFullNameFlags() : das::Enumeration("EObjectFullNameFlags") {
		external = true;
		cppName = "EObjectFullNameFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EObjectFullNameFlags::None), das::LineInfo());
		addIEx("IncludeClassPackage", "IncludeClassPackage", int64_t(EObjectFullNameFlags::IncludeClassPackage), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:38:13
class Enumeration_UE_Net_EFragmentRegistrationFlags : public das::Enumeration {
public:
	Enumeration_UE_Net_EFragmentRegistrationFlags() : das::Enumeration("UE_Net_EFragmentRegistrationFlags") {
		external = true;
		cppName = "UE::Net::EFragmentRegistrationFlags";
		baseType = (das::Type) das::ToBasicType<uint32>::type;
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:51:7
class Enumeration_ECastCheckedType_Type : public das::Enumeration {
public:
	Enumeration_ECastCheckedType_Type() : das::Enumeration("ECastCheckedType_Type") {
		external = true;
		cppName = "ECastCheckedType::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("NullAllowed", "NullAllowed", int64_t(ECastCheckedType::Type::NullAllowed), das::LineInfo());
		addIEx("NullChecked", "NullChecked", int64_t(ECastCheckedType::Type::NullChecked), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:62:12
class Enumeration_EEditChangePropagationFlags : public das::Enumeration {
public:
	Enumeration_EEditChangePropagationFlags() : das::Enumeration("EEditChangePropagationFlags") {
		external = true;
		cppName = "EEditChangePropagationFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EEditChangePropagationFlags::None), das::LineInfo());
		addIEx("OnlyMarkRealignedInstancesAsDirty", "OnlyMarkRealignedInstancesAsDirty", int64_t(EEditChangePropagationFlags::OnlyMarkRealignedInstancesAsDirty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Object.h:1696:13
class Enumeration_UObject_ENetFields_Private : public das::Enumeration {
public:
	Enumeration_UObject_ENetFields_Private() : das::Enumeration("UObject_ENetFields_Private") {
		external = true;
		cppName = "UObject::ENetFields_Private";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("NETFIELD_REP_START", "NETFIELD_REP_START", int64_t(UObject::ENetFields_Private::NETFIELD_REP_START), das::LineInfo());
		addIEx("NETFIELD_REP_END", "NETFIELD_REP_END", int64_t(UObject::ENetFields_Private::NETFIELD_REP_END), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:756:6
class Enumeration_EStructFlags : public das::Enumeration {
public:
	Enumeration_EStructFlags() : das::Enumeration("EStructFlags") {
		external = true;
		cppName = "EStructFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("STRUCT_NoFlags", "STRUCT_NoFlags", int64_t(EStructFlags::STRUCT_NoFlags), das::LineInfo());
		addIEx("STRUCT_Native", "STRUCT_Native", int64_t(EStructFlags::STRUCT_Native), das::LineInfo());
		addIEx("STRUCT_IdenticalNative", "STRUCT_IdenticalNative", int64_t(EStructFlags::STRUCT_IdenticalNative), das::LineInfo());
		addIEx("STRUCT_HasInstancedReference", "STRUCT_HasInstancedReference", int64_t(EStructFlags::STRUCT_HasInstancedReference), das::LineInfo());
		addIEx("STRUCT_NoExport", "STRUCT_NoExport", int64_t(EStructFlags::STRUCT_NoExport), das::LineInfo());
		addIEx("STRUCT_Atomic", "STRUCT_Atomic", int64_t(EStructFlags::STRUCT_Atomic), das::LineInfo());
		addIEx("STRUCT_Immutable", "STRUCT_Immutable", int64_t(EStructFlags::STRUCT_Immutable), das::LineInfo());
		addIEx("STRUCT_AddStructReferencedObjects", "STRUCT_AddStructReferencedObjects", int64_t(EStructFlags::STRUCT_AddStructReferencedObjects), das::LineInfo());
		addIEx("STRUCT_RequiredAPI", "STRUCT_RequiredAPI", int64_t(EStructFlags::STRUCT_RequiredAPI), das::LineInfo());
		addIEx("STRUCT_NetSerializeNative", "STRUCT_NetSerializeNative", int64_t(EStructFlags::STRUCT_NetSerializeNative), das::LineInfo());
		addIEx("STRUCT_SerializeNative", "STRUCT_SerializeNative", int64_t(EStructFlags::STRUCT_SerializeNative), das::LineInfo());
		addIEx("STRUCT_CopyNative", "STRUCT_CopyNative", int64_t(EStructFlags::STRUCT_CopyNative), das::LineInfo());
		addIEx("STRUCT_IsPlainOldData", "STRUCT_IsPlainOldData", int64_t(EStructFlags::STRUCT_IsPlainOldData), das::LineInfo());
		addIEx("STRUCT_NoDestructor", "STRUCT_NoDestructor", int64_t(EStructFlags::STRUCT_NoDestructor), das::LineInfo());
		addIEx("STRUCT_ZeroConstructor", "STRUCT_ZeroConstructor", int64_t(EStructFlags::STRUCT_ZeroConstructor), das::LineInfo());
		addIEx("STRUCT_ExportTextItemNative", "STRUCT_ExportTextItemNative", int64_t(EStructFlags::STRUCT_ExportTextItemNative), das::LineInfo());
		addIEx("STRUCT_ImportTextItemNative", "STRUCT_ImportTextItemNative", int64_t(EStructFlags::STRUCT_ImportTextItemNative), das::LineInfo());
		addIEx("STRUCT_PostSerializeNative", "STRUCT_PostSerializeNative", int64_t(EStructFlags::STRUCT_PostSerializeNative), das::LineInfo());
		addIEx("STRUCT_SerializeFromMismatchedTag", "STRUCT_SerializeFromMismatchedTag", int64_t(EStructFlags::STRUCT_SerializeFromMismatchedTag), das::LineInfo());
		addIEx("STRUCT_NetDeltaSerializeNative", "STRUCT_NetDeltaSerializeNative", int64_t(EStructFlags::STRUCT_NetDeltaSerializeNative), das::LineInfo());
		addIEx("STRUCT_PostScriptConstruct", "STRUCT_PostScriptConstruct", int64_t(EStructFlags::STRUCT_PostScriptConstruct), das::LineInfo());
		addIEx("STRUCT_NetSharedSerialization", "STRUCT_NetSharedSerialization", int64_t(EStructFlags::STRUCT_NetSharedSerialization), das::LineInfo());
		addIEx("STRUCT_Trashed", "STRUCT_Trashed", int64_t(EStructFlags::STRUCT_Trashed), das::LineInfo());
		addIEx("STRUCT_NewerVersionExists", "STRUCT_NewerVersionExists", int64_t(EStructFlags::STRUCT_NewerVersionExists), das::LineInfo());
		addIEx("STRUCT_CanEditChange", "STRUCT_CanEditChange", int64_t(EStructFlags::STRUCT_CanEditChange), das::LineInfo());
		addIEx("STRUCT_Inherit", "STRUCT_Inherit", int64_t(EStructFlags::STRUCT_Inherit), das::LineInfo());
		addIEx("STRUCT_ComputedFlags", "STRUCT_ComputedFlags", int64_t(EStructFlags::STRUCT_ComputedFlags), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:1980:12
class Enumeration_EGetByNameFlags : public das::Enumeration {
public:
	Enumeration_EGetByNameFlags() : das::Enumeration("EGetByNameFlags") {
		external = true;
		cppName = "EGetByNameFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EGetByNameFlags::None), das::LineInfo());
		addIEx("ErrorIfNotFound", "ErrorIfNotFound", int64_t(EGetByNameFlags::ErrorIfNotFound), das::LineInfo());
		addIEx("CaseSensitive", "CaseSensitive", int64_t(EGetByNameFlags::CaseSensitive), das::LineInfo());
		addIEx("CheckAuthoredName", "CheckAuthoredName", int64_t(EGetByNameFlags::CheckAuthoredName), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2006:13
class Enumeration_UEnum_ECppForm : public das::Enumeration {
public:
	Enumeration_UEnum_ECppForm() : das::Enumeration("UEnum_ECppForm") {
		external = true;
		cppName = "UEnum::ECppForm";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Regular", "Regular", int64_t(UEnum::ECppForm::Regular), das::LineInfo());
		addIEx("Namespaced", "Namespaced", int64_t(UEnum::ECppForm::Namespaced), das::LineInfo());
		addIEx("EnumClass", "EnumClass", int64_t(UEnum::ECppForm::EnumClass), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2698:7
class Enumeration_EIncludeSuperFlag_Type : public das::Enumeration {
public:
	Enumeration_EIncludeSuperFlag_Type() : das::Enumeration("EIncludeSuperFlag_Type") {
		external = true;
		cppName = "EIncludeSuperFlag::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("ExcludeSuper", "ExcludeSuper", int64_t(EIncludeSuperFlag::Type::ExcludeSuper), das::LineInfo());
		addIEx("IncludeSuper", "IncludeSuper", int64_t(EIncludeSuperFlag::Type::IncludeSuper), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:2712:12
class Enumeration_EGetSparseClassDataMethod : public das::Enumeration {
public:
	Enumeration_EGetSparseClassDataMethod() : das::Enumeration("EGetSparseClassDataMethod") {
		external = true;
		cppName = "EGetSparseClassDataMethod";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("CreateIfNull", "CreateIfNull", int64_t(EGetSparseClassDataMethod::CreateIfNull), das::LineInfo());
		addIEx("ArchetypeIfNull", "ArchetypeIfNull", int64_t(EGetSparseClassDataMethod::ArchetypeIfNull), das::LineInfo());
		addIEx("ReturnIfNull", "ReturnIfNull", int64_t(EGetSparseClassDataMethod::ReturnIfNull), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3656:12
class Enumeration_EInstancePropertyValueFlags : public das::Enumeration {
public:
	Enumeration_EInstancePropertyValueFlags() : das::Enumeration("EInstancePropertyValueFlags") {
		external = true;
		cppName = "EInstancePropertyValueFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EInstancePropertyValueFlags::None), das::LineInfo());
		addIEx("CausesInstancing", "CausesInstancing", int64_t(EInstancePropertyValueFlags::CausesInstancing), das::LineInfo());
		addIEx("AllowSelfReference", "AllowSelfReference", int64_t(EInstancePropertyValueFlags::AllowSelfReference), das::LineInfo());
		addIEx("DoNotCreateNewInstance", "DoNotCreateNewInstance", int64_t(EInstancePropertyValueFlags::DoNotCreateNewInstance), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Class.h:3672:12
class Enumeration_EObjectInstancingGraphOptions : public das::Enumeration {
public:
	Enumeration_EObjectInstancingGraphOptions() : das::Enumeration("EObjectInstancingGraphOptions") {
		external = true;
		cppName = "EObjectInstancingGraphOptions";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EObjectInstancingGraphOptions::None), das::LineInfo());
		addIEx("DisableInstancing", "DisableInstancing", int64_t(EObjectInstancingGraphOptions::DisableInstancing), das::LineInfo());
		addIEx("InstanceTemplatesOnly", "InstanceTemplatesOnly", int64_t(EObjectInstancingGraphOptions::InstanceTemplatesOnly), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Package.h:54:12
class Enumeration_ESavePackageResult : public das::Enumeration {
public:
	Enumeration_ESavePackageResult() : das::Enumeration("ESavePackageResult") {
		external = true;
		cppName = "ESavePackageResult";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Success", "Success", int64_t(ESavePackageResult::Success), das::LineInfo());
		addIEx("Error", "Error", int64_t(ESavePackageResult::Error), das::LineInfo());
		addIEx("Canceled", "Canceled", int64_t(ESavePackageResult::Canceled), das::LineInfo());
		addIEx("ContainsEditorOnlyData", "ContainsEditorOnlyData", int64_t(ESavePackageResult::ContainsEditorOnlyData), das::LineInfo());
		addIEx("ReferencedOnlyByEditorOnlyData", "ReferencedOnlyByEditorOnlyData", int64_t(ESavePackageResult::ReferencedOnlyByEditorOnlyData), das::LineInfo());
		addIEx("ReplaceCompletely", "ReplaceCompletely", int64_t(ESavePackageResult::ReplaceCompletely), das::LineInfo());
		addIEx("GenerateStub", "GenerateStub", int64_t(ESavePackageResult::GenerateStub), das::LineInfo());
		addIEx("MissingFile", "MissingFile", int64_t(ESavePackageResult::MissingFile), das::LineInfo());
		addIEx("ValidatorError", "ValidatorError", int64_t(ESavePackageResult::ValidatorError), das::LineInfo());
		addIEx("ValidatorSuppress", "ValidatorSuppress", int64_t(ESavePackageResult::ValidatorSuppress), das::LineInfo());
		addIEx("EmptyRealm", "EmptyRealm", int64_t(ESavePackageResult::EmptyRealm), das::LineInfo());
		addIEx("Timeout", "Timeout", int64_t(ESavePackageResult::Timeout), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:73:6
class Enumeration_ELifetimeCondition : public das::Enumeration {
public:
	Enumeration_ELifetimeCondition() : das::Enumeration("ELifetimeCondition") {
		external = true;
		cppName = "ELifetimeCondition";
		baseType = (das::Type) das::ToBasicType<int>::type;
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:89:6
class Enumeration_EPropertyExportCPPFlags : public das::Enumeration {
public:
	Enumeration_EPropertyExportCPPFlags() : das::Enumeration("EPropertyExportCPPFlags") {
		external = true;
		cppName = "EPropertyExportCPPFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("CPPF_None", "CPPF_None", int64_t(EPropertyExportCPPFlags::CPPF_None), das::LineInfo());
		addIEx("CPPF_OptionalValue", "CPPF_OptionalValue", int64_t(EPropertyExportCPPFlags::CPPF_OptionalValue), das::LineInfo());
		addIEx("CPPF_ArgumentOrReturnValue", "CPPF_ArgumentOrReturnValue", int64_t(EPropertyExportCPPFlags::CPPF_ArgumentOrReturnValue), das::LineInfo());
		addIEx("CPPF_Implementation", "CPPF_Implementation", int64_t(EPropertyExportCPPFlags::CPPF_Implementation), das::LineInfo());
		addIEx("CPPF_CustomTypeName", "CPPF_CustomTypeName", int64_t(EPropertyExportCPPFlags::CPPF_CustomTypeName), das::LineInfo());
		addIEx("CPPF_NoConst", "CPPF_NoConst", int64_t(EPropertyExportCPPFlags::CPPF_NoConst), das::LineInfo());
		addIEx("CPPF_NoRef", "CPPF_NoRef", int64_t(EPropertyExportCPPFlags::CPPF_NoRef), das::LineInfo());
		addIEx("CPPF_NoStaticArray", "CPPF_NoStaticArray", int64_t(EPropertyExportCPPFlags::CPPF_NoStaticArray), das::LineInfo());
		addIEx("CPPF_BlueprintCppBackend", "CPPF_BlueprintCppBackend", int64_t(EPropertyExportCPPFlags::CPPF_BlueprintCppBackend), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:113:7
class Enumeration_EExportedDeclaration_Type : public das::Enumeration {
public:
	Enumeration_EExportedDeclaration_Type() : das::Enumeration("EExportedDeclaration_Type") {
		external = true;
		cppName = "EExportedDeclaration::Type";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Local", "Local", int64_t(EExportedDeclaration::Type::Local), das::LineInfo());
		addIEx("Member", "Member", int64_t(EExportedDeclaration::Type::Member), das::LineInfo());
		addIEx("Parameter", "Parameter", int64_t(EExportedDeclaration::Type::Parameter), das::LineInfo());
		addIEx("MacroParameter", "MacroParameter", int64_t(EExportedDeclaration::Type::MacroParameter), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:123:12
class Enumeration_EConvertFromTypeResult : public das::Enumeration {
public:
	Enumeration_EConvertFromTypeResult() : das::Enumeration("EConvertFromTypeResult") {
		external = true;
		cppName = "EConvertFromTypeResult";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("UseSerializeItem", "UseSerializeItem", int64_t(EConvertFromTypeResult::UseSerializeItem), das::LineInfo());
		addIEx("CannotConvert", "CannotConvert", int64_t(EConvertFromTypeResult::CannotConvert), das::LineInfo());
		addIEx("Converted", "Converted", int64_t(EConvertFromTypeResult::Converted), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:130:12
class Enumeration_EPropertyMemoryAccess : public das::Enumeration {
public:
	Enumeration_EPropertyMemoryAccess() : das::Enumeration("EPropertyMemoryAccess") {
		external = true;
		cppName = "EPropertyMemoryAccess";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("Direct", "Direct", int64_t(EPropertyMemoryAccess::Direct), das::LineInfo());
		addIEx("InContainer", "InContainer", int64_t(EPropertyMemoryAccess::InContainer), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:144:12
class Enumeration_EPropertyPointerType : public das::Enumeration {
public:
	Enumeration_EPropertyPointerType() : das::Enumeration("EPropertyPointerType") {
		external = true;
		cppName = "EPropertyPointerType";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("Direct", "Direct", int64_t(EPropertyPointerType::Direct), das::LineInfo());
		addIEx("Container", "Container", int64_t(EPropertyPointerType::Container), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:2827:12
class Enumeration_EObjectPropertyOptions : public das::Enumeration {
public:
	Enumeration_EObjectPropertyOptions() : das::Enumeration("EObjectPropertyOptions") {
		external = true;
		cppName = "EObjectPropertyOptions";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("None", "None", int64_t(EObjectPropertyOptions::None), das::LineInfo());
		addIEx("AllowNullValuesOnNonNullableProperty", "AllowNullValuesOnNonNullableProperty", int64_t(EObjectPropertyOptions::AllowNullValuesOnNonNullableProperty), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6486:12
class Enumeration_EFieldIterationFlags : public das::Enumeration {
public:
	Enumeration_EFieldIterationFlags() : das::Enumeration("EFieldIterationFlags") {
		external = true;
		cppName = "EFieldIterationFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("None", "None", int64_t(EFieldIterationFlags::None), das::LineInfo());
		addIEx("IncludeSuper", "IncludeSuper", int64_t(EFieldIterationFlags::IncludeSuper), das::LineInfo());
		addIEx("IncludeDeprecated", "IncludeDeprecated", int64_t(EFieldIterationFlags::IncludeDeprecated), das::LineInfo());
		addIEx("IncludeInterfaces", "IncludeInterfaces", int64_t(EFieldIterationFlags::IncludeInterfaces), das::LineInfo());
		addIEx("IncludeAll", "IncludeAll", int64_t(EFieldIterationFlags::IncludeAll), das::LineInfo());
		addIEx("Default", "Default", int64_t(EFieldIterationFlags::Default), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6502:7
class Enumeration_EFieldIteratorFlags_SuperClassFlags : public das::Enumeration {
public:
	Enumeration_EFieldIteratorFlags_SuperClassFlags() : das::Enumeration("EFieldIteratorFlags_SuperClassFlags") {
		external = true;
		cppName = "EFieldIteratorFlags::SuperClassFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("ExcludeSuper", "ExcludeSuper", int64_t(EFieldIteratorFlags::SuperClassFlags::ExcludeSuper), das::LineInfo());
		addIEx("IncludeSuper", "IncludeSuper", int64_t(EFieldIteratorFlags::SuperClassFlags::IncludeSuper), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6508:7
class Enumeration_EFieldIteratorFlags_DeprecatedPropertyFlags : public das::Enumeration {
public:
	Enumeration_EFieldIteratorFlags_DeprecatedPropertyFlags() : das::Enumeration("EFieldIteratorFlags_DeprecatedPropertyFlags") {
		external = true;
		cppName = "EFieldIteratorFlags::DeprecatedPropertyFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("ExcludeDeprecated", "ExcludeDeprecated", int64_t(EFieldIteratorFlags::DeprecatedPropertyFlags::ExcludeDeprecated), das::LineInfo());
		addIEx("IncludeDeprecated", "IncludeDeprecated", int64_t(EFieldIteratorFlags::DeprecatedPropertyFlags::IncludeDeprecated), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6514:7
class Enumeration_EFieldIteratorFlags_InterfaceClassFlags : public das::Enumeration {
public:
	Enumeration_EFieldIteratorFlags_InterfaceClassFlags() : das::Enumeration("EFieldIteratorFlags_InterfaceClassFlags") {
		external = true;
		cppName = "EFieldIteratorFlags::InterfaceClassFlags";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("ExcludeInterfaces", "ExcludeInterfaces", int64_t(EFieldIteratorFlags::InterfaceClassFlags::ExcludeInterfaces), das::LineInfo());
		addIEx("IncludeInterfaces", "IncludeInterfaces", int64_t(EFieldIteratorFlags::InterfaceClassFlags::IncludeInterfaces), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/UnrealType.h:6854:12
class Enumeration_EPropertyValueIteratorFlags : public das::Enumeration {
public:
	Enumeration_EPropertyValueIteratorFlags() : das::Enumeration("EPropertyValueIteratorFlags") {
		external = true;
		cppName = "EPropertyValueIteratorFlags";
		baseType = (das::Type) das::ToBasicType<uint8>::type;
		addIEx("NoRecursion", "NoRecursion", int64_t(EPropertyValueIteratorFlags::NoRecursion), das::LineInfo());
		addIEx("FullRecursion", "FullRecursion", int64_t(EPropertyValueIteratorFlags::FullRecursion), das::LineInfo());
	}
};

// from ../../../../../../../UnrealEngine/Engine/Source/Runtime/CoreUObject/Public\UObject/Stack.h:73:6
class Enumeration_EPropertyType : public das::Enumeration {
public:
	Enumeration_EPropertyType() : das::Enumeration("EPropertyType") {
		external = true;
		cppName = "EPropertyType";
		baseType = (das::Type) das::ToBasicType<int>::type;
		addIEx("CPT_None", "CPT_None", int64_t(EPropertyType::CPT_None), das::LineInfo());
		addIEx("CPT_Byte", "CPT_Byte", int64_t(EPropertyType::CPT_Byte), das::LineInfo());
		addIEx("CPT_UInt16", "CPT_UInt16", int64_t(EPropertyType::CPT_UInt16), das::LineInfo());
		addIEx("CPT_UInt32", "CPT_UInt32", int64_t(EPropertyType::CPT_UInt32), das::LineInfo());
		addIEx("CPT_UInt64", "CPT_UInt64", int64_t(EPropertyType::CPT_UInt64), das::LineInfo());
		addIEx("CPT_Int8", "CPT_Int8", int64_t(EPropertyType::CPT_Int8), das::LineInfo());
		addIEx("CPT_Int16", "CPT_Int16", int64_t(EPropertyType::CPT_Int16), das::LineInfo());
		addIEx("CPT_Int", "CPT_Int", int64_t(EPropertyType::CPT_Int), das::LineInfo());
		addIEx("CPT_Int64", "CPT_Int64", int64_t(EPropertyType::CPT_Int64), das::LineInfo());
		addIEx("CPT_Bool", "CPT_Bool", int64_t(EPropertyType::CPT_Bool), das::LineInfo());
		addIEx("CPT_Bool8", "CPT_Bool8", int64_t(EPropertyType::CPT_Bool8), das::LineInfo());
		addIEx("CPT_Bool16", "CPT_Bool16", int64_t(EPropertyType::CPT_Bool16), das::LineInfo());
		addIEx("CPT_Bool32", "CPT_Bool32", int64_t(EPropertyType::CPT_Bool32), das::LineInfo());
		addIEx("CPT_Bool64", "CPT_Bool64", int64_t(EPropertyType::CPT_Bool64), das::LineInfo());
		addIEx("CPT_Float", "CPT_Float", int64_t(EPropertyType::CPT_Float), das::LineInfo());
		addIEx("CPT_ObjectReference", "CPT_ObjectReference", int64_t(EPropertyType::CPT_ObjectReference), das::LineInfo());
		addIEx("CPT_Name", "CPT_Name", int64_t(EPropertyType::CPT_Name), das::LineInfo());
		addIEx("CPT_Delegate", "CPT_Delegate", int64_t(EPropertyType::CPT_Delegate), das::LineInfo());
		addIEx("CPT_Interface", "CPT_Interface", int64_t(EPropertyType::CPT_Interface), das::LineInfo());
		addIEx("CPT_Unused_Index_19", "CPT_Unused_Index_19", int64_t(EPropertyType::CPT_Unused_Index_19), das::LineInfo());
		addIEx("CPT_Struct", "CPT_Struct", int64_t(EPropertyType::CPT_Struct), das::LineInfo());
		addIEx("CPT_Unused_Index_21", "CPT_Unused_Index_21", int64_t(EPropertyType::CPT_Unused_Index_21), das::LineInfo());
		addIEx("CPT_Unused_Index_22", "CPT_Unused_Index_22", int64_t(EPropertyType::CPT_Unused_Index_22), das::LineInfo());
		addIEx("CPT_String", "CPT_String", int64_t(EPropertyType::CPT_String), das::LineInfo());
		addIEx("CPT_Text", "CPT_Text", int64_t(EPropertyType::CPT_Text), das::LineInfo());
		addIEx("CPT_MulticastDelegate", "CPT_MulticastDelegate", int64_t(EPropertyType::CPT_MulticastDelegate), das::LineInfo());
		addIEx("CPT_WeakObjectReference", "CPT_WeakObjectReference", int64_t(EPropertyType::CPT_WeakObjectReference), das::LineInfo());
		addIEx("CPT_LazyObjectReference", "CPT_LazyObjectReference", int64_t(EPropertyType::CPT_LazyObjectReference), das::LineInfo());
		addIEx("CPT_ObjectPtrReference", "CPT_ObjectPtrReference", int64_t(EPropertyType::CPT_ObjectPtrReference), das::LineInfo());
		addIEx("CPT_SoftObjectReference", "CPT_SoftObjectReference", int64_t(EPropertyType::CPT_SoftObjectReference), das::LineInfo());
		addIEx("CPT_Double", "CPT_Double", int64_t(EPropertyType::CPT_Double), das::LineInfo());
		addIEx("CPT_Map", "CPT_Map", int64_t(EPropertyType::CPT_Map), das::LineInfo());
		addIEx("CPT_Set", "CPT_Set", int64_t(EPropertyType::CPT_Set), das::LineInfo());
		addIEx("CPT_FieldPath", "CPT_FieldPath", int64_t(EPropertyType::CPT_FieldPath), das::LineInfo());
		addIEx("CPT_FLargeWorldCoordinatesReal", "CPT_FLargeWorldCoordinatesReal", int64_t(EPropertyType::CPT_FLargeWorldCoordinatesReal), das::LineInfo());
		addIEx("CPT_MAX", "CPT_MAX", int64_t(EPropertyType::CPT_MAX), das::LineInfo());
	}
};

