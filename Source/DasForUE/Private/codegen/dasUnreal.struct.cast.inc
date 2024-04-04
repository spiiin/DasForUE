// this file is generated via daScript automatic C++ binder
// all user modifications will be lost after this file is re-generated

template <> struct cast_arg<FString> {
	static __forceinline const FString & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FString *>::to(res);
	}
};
template <> struct cast_arg<FTextRange> {
	static __forceinline const FTextRange & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FTextRange *>::to(res);
	}
};
template <> struct cast_arg<FVTableHelper> {
	static __forceinline const FVTableHelper & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FVTableHelper *>::to(res);
	}
};
template <> struct cast_arg<FRestoreForUObjectOverwrite> {
	static __forceinline const FRestoreForUObjectOverwrite & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<FRestoreForUObjectOverwrite *>::to(res);
	}
};
template <> struct cast_arg<UObjectBase> {
	static __forceinline const UObjectBase & to ( Context & ctx, SimNode * node ) {
		vec4f res = node->eval(ctx);
		return * cast<UObjectBase *>::to(res);
	}
};