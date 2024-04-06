module unreal_boost //shared

require unreal public

def get_class_by_name(className: string) : UClass?
    var obj = StaticFindObject(
        StaticClass([[UObject?]]),
        AnyPackage(),
        FString(className) |> ToCStr()
    )
    unsafe { return reinterpret<UClass?> obj; }

def new_object_from_class(clazz: UClass?) : UObject?
    return StaticConstructObject_Internal(FStaticConstructObjectParameters(clazz))
