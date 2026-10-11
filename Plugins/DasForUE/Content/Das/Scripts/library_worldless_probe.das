options gen2
require ue_library_generated
[export]
def pure(input:bool):bool { return ue_lib_Add(20,22)==42 && ue_lib_Abs(-3.0lf)==3.0lf }
[export]
def world(input:bool):bool { return ue_lib_Context() }
