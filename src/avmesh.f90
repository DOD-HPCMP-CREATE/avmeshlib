module avmesh
   integer(4) :: AVM_NODES = 1
   integer(4) :: AVM_CELLS = 2
   integer(4) :: AVM_NODESCELLS = 4
   integer(4) :: AVM_BOUNDARY = 8
   integer(4) :: AVM_INTERIOR = 16

   type AVM_AMR_Node_Data
      sequence
      real*8  :: u, v
      integer :: nodeIndex, geomType, geomTopoId, dummy
   end type AVM_AMR_Node_Data
end module avmesh
