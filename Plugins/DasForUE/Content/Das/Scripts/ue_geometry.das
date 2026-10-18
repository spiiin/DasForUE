options gen2
module ue_geometry public
require ue_foundation public
// Owned snapshot of one layout revision. Does not retain a widget or refresh itself.
def ue_geometry(value:UEStructValue):UEGeometry { return __ue_unpack_geometry(value) }
