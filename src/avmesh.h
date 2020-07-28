/** @defgroup api AVMesh API
   Routines for creating and reading AVMesh Files.

   The AVMesh library supports creating or reading multiple files at the same
   time. The first argument of all routines is an '<code>avmid</code>'
   argument.  The following routines return new <code>avmids</code>: @ref
   avm_new_file and @ref avm_read_headers.

   All AVMesh library routines begin with <code>avm_</code>.

   All routines return an error value; C routines as the value of the function
   and Fortran routines in the last argument. Return codes are: 0 on success,
   >0 on failure.

   All routines in Fortran have an additional argument (<code>istatus</code>)
   at the end of the argument list. <code>istatus</code> is an integer and has
   the same meaning as the return value of the routine in C.
@{
*/

#ifndef AVMESH_API
#define AVMESH_API

#include <sys/types.h> /* for off_t */

#ifdef __cplusplus
extern "C" {
#endif

#define AVM_STD_STRING_LENGTH 128

/*! @struct AMR_Node_Data_t
 * AMR Data for nodes.
 */
typedef struct AMR_Node_Data_t {
   /** Parameterized location u component */
   double u;
   /** Parameterized location v component */
   double v;
   /** index into the array of mesh nodes */
   int nodeIndex;
   /** 0=vertex, 1=edge, 2=face */
   int geomType;
   /** ID of the geometry this mesh node is associated with (matching the geomType) */
   int geomTopoId;
} AMR_Node_Data;

typedef enum AVM_Flags_t {
   AVM_NODES = 1,
   AVM_CELLS = 2,
   AVM_NODESCELLS = 4,
   AVM_BOUNDARY = 8,
   AVM_INTERIOR = 16
} AVM_Flags;

/** 
@name Header Functions
@brief Routines for creating and reading headers.
@{ 
*/

/**
@brief Opens an AVMesh file for reading and reads/caches all headers.
   This routine opens an AVMesh file for reading. It checks the file prefix for
   validity then reads the complete file header. Following this, it reads all
   the mesh headers (both general and specific). Following a successful return
   the "get" routines (such as @ref avm_get_int) are available for use.
@fortran
subroutine avm_read_headersf(avmid, filename, istatus)
   integer*4 avmid
   character*(*) filename
   integer*4 istatus
@endfortran
@param[out] avmid  - avmid is set if the file is successfully opened.
                      It is then passed to all other avm_ routines.
                      Once @ref avm_close is called with the avmid it
                      becomes invalid.
@param[in] filename - filename is the name of the AVMesh file
@return 0 on success, >0 on failure
*/
int avm_read_headers(int* avmid, const char* filename);

/**
@brief Opens a new AVMesh file for writing.
   This routine opens a new AVMesh file for writing. Following a successful
   return the "set" routines (such as @ref avm_set_int) are available for use.
   After all necessary header values are set, @ref avm_write_headers writes them
   to disk. After that mesh data is written.
@fortran
subroutine avm_new_filef(avmid, filename, istatus)
   integer*4 avmid
   character*(*) filename
   integer*4 istatus
@endfortran
@param[out] avmid  - avmid is set if the file is successfully opened.
                      It is then passed to all other avm_ routines.
                      Once @ref avm_close is called with the avmid it
                      becomes invalid.
@param[in] filename - filename is the name of the AVMesh file. If filename
                      already exists, it will be overwritten.
@param[in] rev - Target formatRevision for this file (0=rev0, 1=rev1)
@return 0 on success, >0 on failure
*/
int avm_new_file(int* avmid, const char* filename, int rev);

/**
@brief Writes cached header values to disk. 
   This routine writes file/mesh header values to disk. It is called following
   @ref avm_new_file and the "set" routines (such as @ref avm_set_int).
@fortran
subroutine avm_new_filef(avmid, filename, istatus)
   integer*4 avmid
   character*(*) filename
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@return 0 on success, >0 on failure
*/
int avm_write_headers(int avmid);

/**
@deprecated use @ref avm_read_headers instead  
@brief Opens an AVMesh file for reading.
   This routine opens an AVMesh file for reading. It checks the file
   prefix for validity then reads the complete file header. If the file
   is a valid AVMesh, then the file "get" routines (such as @ref
   avm_get_int) are availble for use.
@fortran
subroutine avm_openf(filename, avmid, istatus)
   character*(*) filename
   integer*4 avmid
   integer*4 istatus
@endfortran
@param[in] filename - filename is the name of the AVMesh file
@param[out] avmid  - avmid is set if the file is successfully opened.
                      It is then passed to all other avm_ routines.
                      Once @ref avm_close is called with the avmid it
                      becomes invalid.
@return 0 on success, >0 on failure
*/
int avm_open(const char* filename, int* avmid);

/**
@deprecated use @ref avm_read_headers instead  
@brief Read mesh headers.
   This routine is called following @ref avm_open. It reads all the mesh
   headers (both general and specific). Following a successful return
   the mesh "get" routines (such as @ref avm_get_int) are available
   for use.
@fortran
subroutine avm_read_mesh_headersf(avmid, istatus)
   integer*4 avmid
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_open
@return 0 on success, >0 on failure
*/
int avm_read_mesh_headers(int avmid);

/**
@brief Closes an AVMesh file.
   This routine is called to close an AVMesh file that was opened with
   @ref avm_open. It frees the internal memory occupied by the file. 
   Following a close, the avmid becomes invalid. 
@fortran
subroutine avm_closef(avmid, istatus)
   integer*4 avmid
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file or @ref
                    avm_read_headers
@return 0 on success, >0 on failure
*/
int avm_close(int avmid);

/**
@brief Selection routine for picking meshs, patches, etc.
   This routine selects sections in an open AVMesh file. If a file is open for
   reading (with @ref avm_read_headers) then the getter routines (such as @ref
   avm_get_int) are available for use. If a file is open for writing (with @ref
   avm_new_file) then the setter routines (such as @ref avm_set_int) are
   available.
@fortran
subroutine avm_selectf(avmid, section, id, istatus)
   integer*4 avmid
   character*(*) section
   integer*4 id
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file or @ref
                    avm_read_headers
@param[in] section - section is the part of the file to select; possible values
                     are: - "header" (applicable for all mesh types)
                          - "mesh" (applicable for all mesh types)
                          - "patch" (applicable for bfstruc,unstruc mesh types)
                          - "patchParam" (applicable for bfstruc mesh type)
                          - "surfPatch" (applicable for strand mesh type)
                          - "edgePatch" (applicable for strand mesh type)
                          - "level" (applicable for cart mesh type)
                          - "block" (applicable for cart mesh type)
@param[in] id - id is the one-based number of the section to select; possible
                ranges are:
                          - "header" (set id to 0 for this)
                          - "mesh" (1 to file header -> meshCount)
                          - "patch" (1 to bfstruc,unstruc mesh header -> nPatches)
                          - "patchParam" (1 to bfstruc mesh header -> 
                             nPatchIntParams, nPatchR8Params, nPatchC80Params)
                          - "surfPatch" (1 to strand mesh header -> nSurfPatches)
                          - "edgePatch" (1 to strand mesh header -> nEdgePatches)
                          - "level" (1 to cart mesh header -> nLevels)
                          - "block" (1 to cart mesh header -> nBlocks)
@return 0 on success, >0 on failure
*/
int avm_select(int avmid, const char* section, int id);

/** 
@} 
@name Header Query Functions
@brief Routines to pull information from cached AVMesh headers.
@{ 
*/

/**
@brief Queries an integer type header value.
   This routine pulls integer typed header values from cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_get_intf(avmid, field, value, istatus)
   integer*4 avmid
   character*(*) field
   integer*4 value
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] field - field name from @ref avmhdrs
@param[out] value - output value
@return 0 on success, >0 on failure
*/
int avm_get_int(int avmid, const char* field, int* value);

/**
@brief Queries a floating-point header value.
   This routine pulls real typed header values from cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_get_realf(avmid, field, value, istatus)
   integer*4 avmid
   character*(*) field
   real*8 value
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] field - field name from @ref avmhdrs
@param[out] value - output value
@return 0 on success, >0 on failure
*/
int avm_get_real(int avmid, const char* field, double* value);

/**
@brief Queries a string header value.
   This routine pulls string header values from cached headers. To find names
   for the field argument see @ref avmhdrs. To select specific parts of the
   header see the @ref avm_select routine.  
@fortran
subroutine avm_get_strf(avmid, field, value, istatus)
   integer*4 avmid
   character*(*) field
   character*(*) str
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] field - field name from @ref avmhdrs
@param[out] str - output string
@param[in] len - max length of incoming string buffer
@return 0 on success, >0 on failure
@note In C the output string is '\0' terminated, in Fortran it is padded with
whitespace.
*/
int avm_get_str(int avmid, const char* field, char* str, int len);

/**
@brief Queries an integer array header value.
   This routine pulls integer array header values from cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_get_int_arrayf(avmid, field, values, len, istatus)
   integer*4 avmid
   character*(*) field
   integer*4(*) values
   integer*4 len
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] field - field name from @ref avmhdrs
@param[out] values - output array
@param[in] len - max length of incoming array
@return 0 on success, >0 on failure
*/
int avm_get_int_array(int avmid, const char* field, int* values, int len);

/**
@brief Queries an floating-point array header value.
   This routine pulls real array header values from cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_get_real_arrayf(avmid, field, values, len, istatus)
   integer*4 avmid
   character*(*) field
   real*8(*) values
   integer*4 len
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] field - field name from @ref avmhdrs
@param[out] values - output array
@param[in] len - max length of incoming array
@return 0 on success, >0 on failure
*/
int avm_get_real_array(int avmid, const char* field, double* values, int len);

/** 
@} 
@name Header Set Functions
@brief Routines to put information into cached AVMesh headers.
@{ 
*/

/**
@brief Sets an integer type header value.
   This routine sets integer typed header values into cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_set_intf(avmid, field, value, istatus)
   integer*4 avmid
   character*(*) field
   integer*4 value
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@param[in] field - field name from @ref avmhdrs
@param[in] value - input value
@return 0 on success, >0 on failure
*/
int avm_set_int(int avmid, const char* field, int value);

/**
@brief Sets a float-point type header value.
   This routine sets real typed header values into cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_set_realf(avmid, field, value, istatus)
   integer*4 avmid
   character*(*) field
   real*8 value
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@param[in] field - field name from @ref avmhdrs
@param[in] value - input value
@return 0 on success, >0 on failure
*/
int avm_set_real(int avmid, const char* field, double value);

/**
@brief Sets a string type header value.
   This routine sets string typed header values into cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_set_strf(avmid, field, str, istatus)
   integer*4 avmid
   character*(*) field
   character*(*) str
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@param[in] field - field name from @ref avmhdrs
@param[in] str - input str
@param[in] len - max length of incoming string buffer 
@return 0 on success, >0 on failure
*/
int avm_set_str(int avmid, const char* field, const char* str, int len);

/**
@brief Sets an integer array header value.
   This routine sets integer array header values into cached headers. To find
   names for the field argument see @ref avmhdrs. To select specific parts of
   the header see the @ref avm_select routine.
@fortran
subroutine avm_set_int_arrayf(avmid, field, values, istatus)
   integer*4 avmid
   character*(*) field
   integer*4(*) value
   integer*4 length
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@param[in] field - field name from @ref avmhdrs
@param[in] values - input values
@param[in] len - max length of incoming array
@return 0 on success, >0 on failure
*/
int avm_set_int_array(int avmid, const char* field, const int* values, int len);

/**
@brief Sets a floating-point array header value.
   This routine sets real array header values into cached headers. To find names
   for the field argument see @ref avmhdrs. To select specific parts of the
   header see the @ref avm_select routine.
@fortran
subroutine avm_set_real_arrayf(avmid, field, values, istatus)
   integer*4 avmid
   character*(*) field
   real*8(*) values
   integer*4 length
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@param[in] field - field name from @ref avmhdrs
@param[in] values - input values
@param[in] len - max length of incoming array
@return 0 on success, >0 on failure
*/
int avm_set_real_array(int avmid, const char* field, const double* values, int len);

/**
@brief Return a descriptive error message for the most recently encountered avmesh error.
@fortran
subroutine avm_get_error_strf()
   character*(*) error_string
   integer*4 length
@endfortran
@return error string, or \0 if there have been no errors
*/
char* avm_get_error_str();

/** 
@} 
@name Mesh Offset Functions 
@brief Calculate and report byte offsets.
   These routines return the starting byte of a particular mesh header or mesh
   data block. The mesh integer parameter passed to these routines are one-based.
@{ 
*/

/**
@brief Returns the byte offset that mesh header begins.
@fortran
subroutine avm_mesh_header_offsetf(avmid, mesh, offset, istatus)
   integer*4 avmid
   integer*4 mesh
   integer*8 offset
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_new_file
@param[in] mesh - the mesh number (one based)
@param[out] offset - input values
@return 0 on success, >0 on failure
@note The type of the offset return value is off_t in C and integer*8 in
Fortran. 
*/
int avm_mesh_header_offset(int avmid, int mesh, off_t* offset);

/**
@brief Returns the byte offset that mesh data begins.
@fortran
subroutine avm_mesh_data_offsetf(avmid, mesh, offset, istatus)
   integer*4 avmid
   integer*4 mesh
   integer*8 offset
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] mesh - the mesh number (one based)
@param[out] offset - input values
@return 0 on success, >0 on failure
@note The type of the offset return value is off_t in C and integer*8 in
Fortran. 
*/
int avm_mesh_data_offset(int avmid, int mesh, off_t* offset);

/**
@brief Positions the file pointer to the desired mesh.
   The routine is used for positioning the file pointer to the desired mesh. It
   is used for reading mesh data in variable order or just reading a particular
   mesh from an multi-mesh avmesh file.
@fortran
subroutine avm_seek_to_meshf(avmid, mesh, offset, istatus)
   integer*4 avmid
   integer*4 mesh
   integer*4 istatus
@endfortran
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] mesh - the mesh number (one based)
@return 0 on success, >0 on failure
*/
int avm_seek_to_mesh(int avmid, int mesh);


/** 
@} 
@name Higher order node count functions
@brief Routines to determine the number of nodes at different polynomial orders
@{ 
*/
int avm_nodes_per_tri(int avmid, int p);
int avm_nodes_per_quad(int avmid, int p);
int avm_nodes_per_tet(int avmid, int p);
int avm_nodes_per_pyr(int avmid, int p);
int avm_nodes_per_pri(int avmid, int p);
int avm_nodes_per_hex(int avmid, int p);

/** 
@} 
@name Body-Fitted Structured Mesh Data Functions 
@brief Routines to process bfstruc mesh data
@{ 
*/

int avm_bfstruc_read_xyz_r4(int avmid, float* x, float* y, float* z, int size);
int avm_bfstruc_read_xyz_r8(int avmid, double* x, double* y, double* z, int size);
int avm_bfstruc_read_iblank(int avmid, int* iblanks, int size);

int avm_bfstruc_write_xyz_r4(int avmid, float* x, float* y, float* z, int size);
int avm_bfstruc_write_xyz_r8(int avmid, double* x, double* y, double* z, int size);
int avm_bfstruc_write_iblank(int avmid, int* iblanks, int size);

/** 
@} 
@name Strand Mesh Data Functions 
@brief Routines to process strand mesh data
@{ 
*/

/**
@deprecated Use @ref avm_strand_read_nodes_r4, @ref avm_strand_read_faces, @ref avm_strand_read_edges, and @ref avm_strand_read_amr instead.
*/
int avm_strand_read_r4(int avmid,
   int* triFaces,      int triFaces_size,
   int* quadFaces,     int quadFaces_size,
   int* bndEdges,      int bndEdges_size,
   int* nodeClip,      int nodeClip_size,
   int* faceClip,      int faceClip_size,
   int* faceTag,       int faceTag_size,
   int* edgeTag,       int edgeTag_size,
   float* xyz,         int xyz_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

/**
@deprecated Use @ref avm_strand_read_nodes_r8, @ref avm_strand_read_faces, @ref avm_strand_read_edges, and @ref avm_strand_read_amr instead.
*/
int avm_strand_read_r8(int avmid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* bndEdges,       int bndEdges_size,
   int* nodeClip,       int nodeClip_size,
   int* faceClip,       int faceClip_size,
   int* faceTag,        int faceTag_size,
   int* edgeTag,        int edgeTag_size,
   double* xyz,         int xyz_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

/**
@deprecated Use @ref avm_strand_write_nodes_r4, @ref avm_strand_write_faces, @ref avm_strand_write_edges, and @ref avm_strand_write_amr instead.
*/
int avm_strand_write_r4(int avmid,
   int* triFaces,      int triFaces_size,
   int* quadFaces,     int quadFaces_size,
   int* bndEdges,      int bndEdges_size,
   int* nodeClip,      int nodeClip_size,
   int* faceClip,      int faceClip_size,
   int* faceTag,       int faceTag_size,
   int* edgeTag,       int edgeTag_size,
   float* xyz,         int xyz_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

/**
@deprecated Use @ref avm_strand_write_nodes_r8, @ref avm_strand_write_faces, @ref avm_strand_write_edges, and @ref avm_strand_write_amr instead.
*/
int avm_strand_write_r8(int avmid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* bndEdges,       int bndEdges_size,
   int* nodeClip,       int nodeClip_size,
   int* faceClip,       int faceClip_size,
   int* faceTag,        int faceTag_size,
   int* edgeTag,        int edgeTag_size,
   double* xyz,         int xyz_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

/**
@brief Read strand node data (single-precision).
   This routine is used to read in the xyz node data for a strand mesh
   in a single-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each surface node. Length in bytes = 3 * nNodes * 4
@param[in] xyz_size - number of elements in xyz array
@param[in] nodeID - Node index for each strand. Length in bytes = nStrands * 4
@param[in] nodeID_size - Number of elements in nodeID array
@param[in] nodeClip - Clip index in the normal direction for each strand. Length in bytes = nStrands * 4
@param[in] nodeClip_size - Number of elements in nodeClip array
@param[in] pointingVec - x,y,z coordinates of normal vector for each strand, normalized to 1.0. Length in bytes = 3 * nStrands * (4 or 8)
@param[in] pointingVec_size - number of elements in pointingVec array
@param[in] xStrand - Normal distribution of of points applied to each strand, normalized to 1.0. Length in bytes = nPtsPerStrand * (4 or 8)
@param[in] xStrand_size - number of elements in xStrand array
@return 0 on success, >0 on failure
*/
int avm_strand_read_nodes_r4(int avmid,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

/**
@brief Read strand node data (double-precision).
   This routine is used to read in the xyz node data for a strand mesh
   in a double-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each surface node. Length in bytes = 3 * nNodes * 4
@param[in] xyz_size - number of elements in xyz array
@param[in] nodeID - Node index for each strand. Length in bytes = nStrands * 4
@param[in] nodeID_size - Number of elements in nodeID array
@param[in] nodeClip - Clip index in the normal direction for each strand. Length in bytes = nStrands * 4
@param[in] nodeClip_size - Number of elements in nodeClip array
@param[in] pointingVec - x,y,z coordinates of normal vector for each strand, normalized to 1.0. Length in bytes = 3 * nStrands * (4 or 8)
@param[in] pointingVec_size - number of elements in pointingVec array
@param[in] xStrand - Normal distribution of of points applied to each strand, normalized to 1.0. Length in bytes = nPtsPerStrand * (4 or 8)
@param[in] xStrand_size - number of elements in xStrand array
@return 0 on success, >0 on failure
*/
int avm_strand_read_nodes_r8(int avmid,
   double* xyz,         int xyz_size,
   int* nodeID,         int nodeID_size,
   int* nodeClip,       int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

/**
@brief Read strand face data.
   This routine is used to read in the mesh face data for a strand mesh.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 strand indices. Length in bytes = 3 * nTriFaces * 4
@param[in] triFaces_size - Number of elements in triFaces array
@param[in] quadFaces - For each face: 4 strand indices. Length in bytes = 4 * nQuadFaces * 4
@param[in] quadFaces_size - Number of elements in quadFaces array
@param[in] polyFaces - For each face: nNodesInFace, nNodesInFace indices into strand array
                      Length in bytes = polyFacesSize
@param[in] polyFaces_size - Number of elements in polyFaces array
@param[in] faceTag - Surface patch id for each face. Length in bytes = (nTriFaces+nQuadFaces+nPolyFaces) * 4
@param[in] faceTag_size - Number of elements in faceTag array
@return 0 on success, >0 on failure
*/
int avm_strand_read_faces(int avmid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
);

/**
@brief Read strand edge data.
   This routine is used to read in the mesh edge data for a strand mesh.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] bndEdges - For each boundary edge: 2 strand indices. Length in bytes = 2 * nBndEdges * 4
@param[in] bndEdges_size - Number of elements in bndEdges array
@param[in] edgeTag - Surface patch id for each edge. Length in bytes = nBndEdges * 4
@param[in] edgeTag_size - Number of elements in edgeTag array
@return 0 on success, >0 on failure
*/
int avm_strand_read_edges(int avmid,
  int* bndEdges,      int bndEdges_size,
   int* edgeTag,       int edgeTag_size
);

/**
@brief Read Strand AMR data.
   This routine is used to read in the AMR data for a strand mesh.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] nodesOnGeometry - Array of AMR_Node_Data structs, one for each nNodesOnGeometry
   Length in bytes = ( 3 * 4 + 2 * 8 ) * nNodesOnGeometry
@param[in] nodesOnGeometry_size - number of elements in nodesOnGeometry array
@param[in] edgesOnGeometry - For each edge: mesh edge ID, GeomType (1=edge, 2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nEdgesOnGeometry * 4
@param[in] edgesOnGeometry_size - number of elements in edgesOnGeometry array
@param[in] facesOnGeometry - For each face: mesh face ID, GeomType (2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nFacesOnGeometry * 4
@param[in] facesOnGeometry_size - number of elements in facesOnGeometry array
@return 0 on success, >0 on failure
*/
int avm_strand_read_amr(int avmid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);

/**
@brief Write strand node data (single-precision).
   This routine is used to write out the xyz node data for a strand mesh
   in a single-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each surface node. Length in bytes = 3 * nNodes * 4
@param[in] xyz_size - number of elements in xyz array
@param[in] nodeID - Node index for each strand. Length in bytes = nStrands * 4
@param[in] nodeID_size - Number of elements in nodeID array
@param[in] nodeClip - Clip index in the normal direction for each strand. Length in bytes = nStrands * 4
@param[in] nodeClip_size - Number of elements in nodeClip array
@param[in] pointingVec - x,y,z coordinates of normal vector for each strand, normalized to 1.0. Length in bytes = 3 * nStrands * (4 or 8)
@param[in] pointingVec_size - number of elements in pointingVec array
@param[in] xStrand - Normal distribution of of points applied to each strand, normalized to 1.0. Length in bytes = nPtsPerStrand * (4 or 8)
@param[in] xStrand_size - number of elements in xStrand array
@return 0 on success, >0 on failure
*/
int avm_strand_write_nodes_r4(int avmid,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

/**
@brief Write strand node data (double-precision).
   This routine is used to write out the xyz node data for a strand mesh
   in a double-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each surface node. Length in bytes = 3 * nNodes * 4
@param[in] xyz_size - number of elements in xyz array
@param[in] nodeID - Node index for each strand. Length in bytes = nStrands * 4
@param[in] nodeID_size - Number of elements in nodeID array
@param[in] nodeClip - Clip index in the normal direction for each strand. Length in bytes = nStrands * 4
@param[in] nodeClip_size - Number of elements in nodeClip array
@param[in] pointingVec - x,y,z coordinates of normal vector for each strand, normalized to 1.0. Length in bytes = 3 * nStrands * (4 or 8)
@param[in] pointingVec_size - number of elements in pointingVec array
@param[in] xStrand - Normal distribution of of points applied to each strand, normalized to 1.0. Length in bytes = nPtsPerStrand * (4 or 8)
@param[in] xStrand_size - number of elements in xStrand array
@return 0 on success, >0 on failure
*/
int avm_strand_write_nodes_r8(int avmid,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

/**
@brief Write strand face data.
   This routine is used to write out the mesh face data for a strand mesh.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 strand indices. Length in bytes = 3 * nTriFaces * 4
@param[in] triFaces_size - Number of elements in triFaces array
@param[in] quadFaces - For each face: 4 strand indices. Length in bytes = 4 * nQuadFaces * 4
@param[in] quadFaces_size - Number of elements in quadFaces array
@param[in] polyFaces - For each face: nNodesInFace, nNodesInFace indices into strand array
                      Length in bytes = polyFacesSize
@param[in] polyFaces_size - Number of elements in polyFaces array
@param[in] faceTag - Surface patch id for each face. Length in bytes = (nTriFaces+nQuadFaces+nPolyFaces) * 4
@param[in] faceTag_size - Number of elements in faceTag array
@return 0 on success, >0 on failure
*/
int avm_strand_write_faces(int avmid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
);

/**
@brief Write strand edge data.
   This routine is used to write out the mesh edge data for a strand mesh.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] bndEdges - For each boundary edge: 2 strand indices. Length in bytes = 2 * nBndEdges * 4
@param[in] bndEdges_size - Number of elements in bndEdges array
@param[in] edgeTag - Surface patch id for each edge. Length in bytes = nBndEdges * 4
@param[in] edgeTag_size - Number of elements in edgeTag array
@return 0 on success, >0 on failure
*/
int avm_strand_write_edges(int avmid,
   int* bndEdges,      int bndEdges_size,
   int* edgeTag,       int edgeTag_size
);

/**
@brief Write Strand AMR data.
   This routine is used to write out the AMR data for a strand mesh.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] nodesOnGeometry - Array of AMR_Node_Data structs, one for each nNodesOnGeometry
   Length in bytes = ( 3 * 4 + 2 * 8 ) * nNodesOnGeometry
@param[in] nodesOnGeometry_size - number of elements in nodesOnGeometry array
@param[in] edgesOnGeometry - For each edge: mesh edge ID, GeomType (1=edge, 2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nEdgesOnGeometry * 4
@param[in] edgesOnGeometry_size - number of elements in edgesOnGeometry array
@param[in] facesOnGeometry - For each face: mesh face ID, GeomType (2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nFacesOnGeometry * 4
@param[in] facesOnGeometry_size - number of elements in facesOnGeometry array
@return 0 on success, >0 on failure
*/
int avm_strand_write_amr(int avmid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);

/** 
@} 
@name Unstructured Mesh Data Functions 
@brief Routines to process unstruc mesh data
@{ 
*/

/**
@brief Read unstructured mesh node data (single-precision).
   This routine is used to read in the xyz node data for an unstructured mesh
   in a single-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each node. Length in bytes = 3 * nNodes * 4
@param[in] xyz_size - number of elements in xyz array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_nodes_r4(int avmid, float* xyz, int xyz_size);

/**
@brief Read unstructured mesh node data (double-precision).
   This routine is used to read in the xyz node data for an unstructured mesh
   in a double-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each node. Length in bytes = 3 * nNodes * 8
@param[in] xyz_size - number of elements in xyz array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_nodes_r8(int avmid, double* xyz, int xyz_size);

/**
@brief Read unstructured face data.
   This routine is used to read in the faces for an unstructured mesh.
   For 2D cases, use triFaces only. In this case, each "face" is actually a line, so 2 points should be specified.
   The unused 3rd index should be set to -1.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Faces on the boundary should be listed before non-boundary faces.
   Length in bytes = 5 * nTriFaces * 4
@param[in] triFaces_size - number of elements in triFaces array
@param[in] quadFaces - For each face: 4 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Faces on the boundary should be listed before non-boundary faces.
   Length in bytes = 6 * nQuadFaces * 4
@param[in] quadFaces_size - number of elements in quadFaces array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_faces(int avmid, 
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);

/**
@brief Read unstructured tri face data.
   This routine is used to read in the tri faces for an unstructured mesh.
   For 2D cases, each "face" is actually a line, so the unused 3rd index should be set to -1.
   The flags argument determines the exact form of the returned triFaces buffer.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Faces on the boundary should be listed before non-boundary faces.
@param[in] triFaces_size - number of elements in triFaces array
@param[in] stride - Currently assumed to be 5. For future use: the number of elements in each triFace.
   This can be set to a value higher than the actual length of each triFace to pad the array. This may
   be useful for reading the tri/quad faces into one large array with a uniform stride between faces.
@param[in] start - first index of triFaces to read
@param[in] end - last index of triFaces to read
@param[in] flags - A logically ORed set of AVM_Flags. Currently only two sets of flags are supported:
   AVM_BOUNDARY | AVM_NODESCELLS to read the face node IDs and neighboring cell/patch IDs for the
   tri faces on the boundary, or AVM_INTERIOR | AVM_NODESCELLS to read the face node IDs and neighboring
   cell/patch IDs for the interior tri faces.
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_tri(int avmid,
   int* triFaces, int triFaces_size,
   int stride, int start, int end, int flags);

/**
@brief Read unstructured quad face data.
   This routine is used to read in the quad faces for an unstructured mesh.
   For 2D cases, each "face" is actually a line, so the unused 3rd index should be set to -1.
   The flags argument determines the exact form of the returned quadFaces buffer.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] quadFaces - For each face: 3 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Faces on the boundary should be listed before non-boundary faces.
@param[in] quadFaces_size - number of elements in quadFaces array
@param[in] stride - Currently assumed to be 6. For future use: the number of elements in each quadFace.
   This can be set to a value higher than the actual length of each quadFace to pad the array. This may
   be useful for reading the tri/quad faces into one large array with a uniform stride between faces.
@param[in] start - first index of quadFaces to read
@param[in] end - last index of quadFaces to read
@param[in] flags - A logically ORed set of AVM_Flags. Currently only two sets of flags are supported:
   AVM_BOUNDARY | AVM_NODESCELLS to read the face node IDs and neighboring cell/patch IDs for the
   tri faces on the boundary, or AVM_INTERIOR | AVM_NODESCELLS to read the face node IDs and neighboring
   cell/patch IDs for the interior quad faces.
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_quad(int avmid,
   int* quadFaces, int quadFaces_size,
   int stride, int start, int end, int flags);

/**
@brief Read unstructured mesh node data (single-precision).
   This routine is used to read in a contiguous portion of the  xyz node data for an unstructured mesh
   in a single-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each node. Length in bytes = 3 * (end-start+1) * 4
@param[in] xyz_size - Number of elements in partial xyz array
@param[in] start - first index of xyz to read
@param[in] end - last index of xyz to read
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_partial_nodes_r4(int avmid, float* xyz, int xyz_size, int start, int end);

/**
@brief Read unstructured mesh node data (single-precision).
   This routine is used to read in a contiguous portion of the  xyz node data for an unstructured mesh
   in a double-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each node. Length in bytes = 3 * (end-start+1) * 4
@param[in] xyz_size - Number of elements in partial xyz array
@param[in] start - first index of xyz to read
@param[in] end - last index of xyz to read
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_partial_nodes_r8(int avmid, double* xyz, int xyz_size, int start, int end);

/**
@brief Read unstructured face data.
   This routine is used to read in a contiguous portion of the faces for an unstructured mesh.
   For 2D cases, use triFaces only. In this case, each "face" is actually a line, so 2 points should be specified.
   The unused 3rd index should be set to -1.
   If you don't want to read a certain face type at all, pass a size of zero, and start/end indices of -1.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Faces on the boundary should be listed before non-boundary faces.
   Length in bytes = 5 * nTriFaces * 4
@param[in] triFaces_size - number of elements in triFaces array
@param[in] triStart - first triFace to read
@param[in] triEnd - last triFace to read
@param[in] quadFaces - For each face: 4 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Faces on the boundary should be listed before non-boundary faces.
   Length in bytes = 6 * nQuadFaces * 4
@param[in] quadFaces_size - number of elements in quadFaces array
@param[in] quadStart - first quadFace to read
@param[in] quadEnd - last quadFace to read
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_partial_faces(int avmid,
   int* triFaces,  int triFaces_size,  int triStart,  int triEnd,
   int* quadFaces, int quadFaces_size, int quadStart, int quadEnd
);

/**
@brief Read unstructured boundary face data.
   This routine is used to read in the boundary faces for an unstructured mesh.
   This routine behaves exactly the same as avm_unstruc_read_faces, but only the boundary faces are read.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in,out] triFaces - For each face: 3 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Length in bytes = 5 * nTriFaces * 4
@param[in] triFaces_size - number of elements in triFaces array
@param[in] quadFaces - For each face: 4 node indices, left cell index, right cell index
   If negative, the left/right cell index refers to a patch id instead of a cell index.
   In this way, boundary faces can indicate which patch they are a part of.
   Length in bytes = 6 * nQuadFaces * 4
@param[in] quadFaces_size - number of elements in quadFaces array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_bnd_faces(int avmid,
   int* bndTriFaces,  int bndTriFaces_size,
   int* bndQuadFaces, int bndQuadFaces_size
);

/**
@brief Read unstructured cell data.
   This routine is used to read in the cells for an unstructured mesh.
   For 2D cases, use hexCells/tetCells only. In this case, each "cell" is actually a face, so 3 or 4 points should
   be specified, depending on whether triangles or quads are used. The unused indices should be set to -1.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] hexCells - For each cell: 8 node indices. Length in bytes = 8 * nHexCells * 4
@param[in] hexCells_size - number of elements in hexCells array
@param[in] tetCells - For each cell: 4 node indices. Length in bytes = 4 * nTetCells * 4
@param[in] tetCells_size - number of elements in tetCells array
@param[in] priCells - For each cell: 6 node indices. Length in bytes = 6 * nPriCells * 4
@param[in] priCells_size - number of elements in priCells array
@param[in] pyrCells - For each cell: 5 node indices. Length in bytes = 5 * nPyrCells * 4
@param[in] pyrCells_size - number of elements in pyrCells array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_cells(int avmid,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size
);

/**
@brief Read unstructured cell data.
   This routine is used to read in a contiguous portion of the cells for an unstructured mesh.
   For 2D cases, use hexCells/tetCells only. In this case, each "cell" is actually a face, so 3 or 4 points should
   be specified, depending on whether triangles or quads are used. The unused indices should be set to -1.
   If you don't want to read a certain cell type at all, pass a size of zero, and start/end indices of -1.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] hexCells - For each cell: 8 node indices. Length in bytes = 8 * nHexCells * 4
@param[in] hexCells_size - number of elements in hexCells array
@param[in] hexStart - first hexFace to read
@param[in] hexEnd - last hexFace to read
@param[in] tetCells - For each cell: 4 node indices. Length in bytes = 4 * nTetCells * 4
@param[in] tetCells_size - number of elements in tetCells array
@param[in] tetStart - first tetFace to read
@param[in] tetEnd - last tetFace to read
@param[in] priCells - For each cell: 6 node indices. Length in bytes = 6 * nPriCells * 4
@param[in] priCells_size - number of elements in priCells array
@param[in] priStart - first priFace to read
@param[in] priEnd - last priFace to read
@param[in] pyrCells - For each cell: 5 node indices. Length in bytes = 5 * nPyrCells * 4
@param[in] pyrCells_size - number of elements in pyrCells array
@param[in] pyrStart - first pyrFace to read
@param[in] pyrEnd - last pyrFace to read
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_partial_cells(int avmid,
   int* hexCells, int hexCells_size, int hexStart, int hexEnd,
   int* tetCells, int tetCells_size, int tetStart, int tetEnd,
   int* priCells, int priCells_size, int priStart, int priEnd,
   int* pyrCells, int pyrCells_size, int pyrStart, int pyrEnd
);

/**
@brief Read edge data.
   This routine is used to read in the surface edge data for an unstructured mesh.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] edges- For each edge: 2 node indices. Length in bytes = 2 * nEdges * 4
@param[in] edges_size - number of elements in edges array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_edges(int avmid,
   int* edges, int edges_size
);

/**
@brief Read unstruc AMR data.
   This routine is used to read in the AMR data for an unstructured mesh.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] nodesOnGeometry - Array of AMR_Node_Data structs, one for each nNodesOnGeometry
   Length in bytes = ( 3 * 4 + 2 * 8 ) * nNodesOnGeometry
@param[in] nodesOnGeometry_size - number of elements in nodesOnGeometry array
@param[in] edgesOnGeometry - For each edge: mesh edge ID, GeomType (1=edge, 2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nEdgesOnGeometry * 4
@param[in] edgesOnGeometry_size - number of elements in edgesOnGeometry array
@param[in] facesOnGeometry - For each face: mesh face ID, GeomType (2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nFacesOnGeometry * 4
@param[in] facesOnGeometry_size - number of elements in facesOnGeometry array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_amr(int avmid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);

/**
@brief Read cell geometry region ID data.
   This routine is used to read in the geometry region IDs associated with each mesh cell for AMR purposes.
   The arrays are implicitly ordered to match the cells in the hexCells/tetCells/priCells/pyrCells arrays.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] hexGeomIds - For each hexCell, a cooresponding geometry volume region Id. Length in bytes = nHexCells * 4
@param[in] hexGeomIds_size - number of elements in hexGeomIDs array
@param[in] tetGeomIds - For each tetCell, a cooresponding geometry volume region Id. Length in bytes = nTetCells * 4
@param[in] tetGeomIds_size - number of elements in tetGeomIDs array
@param[in] priGeomIds - For each priCell, a cooresponding geometry volume region Id. Length in bytes = nPriCells * 4
@param[in] priGeomIds_size - number of elements in priGeomIDs array
@param[in] pyrGeomIds - For each pyrCell, a cooresponding geometry volume region Id. Length in bytes = nPyrCells * 4
@param[in] pyrGeomIds_size - number of elements in pyrGeomIDs array
@return 0 on success, >0 on failure
*/
int avm_unstruc_read_amr_volumeids(int avmid,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size);

/**
@brief Write unstructured mesh node data (single-precision).
   This routine is used to write out the xyz node data for an unstructured mesh
   in a single-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each node. Length in bytes = 3 * nNodes * 4
@param[in] xyz_size - number of elements in xyz array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_nodes_r4(int avmid, float* xyz, int xyz_size);

/**
@brief Write unstructured mesh node data (double-precision).
   This routine is used to write out the xyz node data for an unstructured mesh
   in a double-precision format.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] xyz - array of x,y,z coordinates for each node. Length in bytes = 3 * nNodes * 8
@param[in] xyz_size - number of elements in xyz array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_nodes_r8(int avmid, double* xyz, int xyz_size);

/**
@brief Write unstructured face data.
   This routine is used to write out the faces for an unstructured mesh.
   For 2D cases, use triFaces only. In this case, each "face" is actually a line, so 2 points should be specified.
   The unused 3rd index should be set to -1.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 node indices, left cell index, right cell index
   If there is no left/right cell index, set to a negative number.
   Faces on the boundary should be listed before non-boundary faces.
   Length in bytes = 5 * nTriFaces * 4
@param[in] triFaces_size - number of elements in triFaces array
@param[in] quadFaces - For each face: 4 node indices, left cell index, right cell index
   If there is no left/right cell index, set to a negative number.
   Faces on the boundary should be listed before non-boundary faces.
   Length in bytes = 6 * nQuadFaces * 4
@param[in] quadFaces_size - number of elements in quadFaces array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_faces(int avmid, 
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);

/**
@brief Write unstructured boundary face data.
   This routine is used to write out the boundary faces for an unstructured mesh.
   For 2D cases, use triFaces only. In this case, each "face" is actually a line, so 2 points should be specified.
   The unused 3rd index should be set to -1.
   This routine behaves exactly the same as avm_unstruc_read_faces, but only the boundary faces are read.
@param[in] avmid - avmid as returned from @ref avm_read_headers
@param[in] triFaces - For each face: 3 node indices, left cell index, right cell index
   If there is no left/right cell index, set to a negative number.
   Length in bytes = 5 * nTriFaces * 4
@param[in] triFaces_size - number of elements in triFaces array
@param[in] quadFaces - For each face: 4 node indices, left cell index, right cell index
   If there is no left/right cell index, set to a negative number.
   Length in bytes = 6 * nQuadFaces * 4
@param[in] quadFaces_size - number of elements in quadFaces array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_bnd_faces(int avmid, 
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);

/**
@brief Write unstructured cell data.
   This routine is used to write out the cells for an unstructured mesh.
   For 2D cases, use hexCells/tetCells only. In this case, each "cell" is actually a face, so 3 or 4 points should
   be specified, depending on whether triangles or quads are used. The unused indices should be set to -1.
   This version does not check against array size arguments passed by the user. This is to accomadate large meshes
   where 8*nHexes (for example) would overflow an int. Rev3 will use an int64 interface to better avoid this issue.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] hexCells - For each cell: 8 node indices. Length in bytes = 8 * nHexCells * 4
@param[in] hexCells_size - number of elements in hexCells array
@param[in] tetCells - For each cell: 4 node indices. Length in bytes = 4 * nTetCells * 4
@param[in] tetCells_size - number of elements in tetCells array
@param[in] priCells - For each cell: 6 node indices. Length in bytes = 6 * nPriCells * 4
@param[in] priCells_size - number of elements in priCells array
@param[in] pyrCells - For each cell: 5 node indices. Length in bytes = 5 * nPyrCells * 4
@param[in] pyrCells_size - number of elements in pyrCells array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_cells_nosize(int avmid,
   int* hexCells,
   int* tetCells,
   int* priCells,
   int* pyrCells
);

/**
@brief Write unstructured cell data.
   This routine is used to write out the cells for an unstructured mesh.
   For 2D cases, use hexCells/tetCells only. In this case, each "cell" is actually a face, so 3 or 4 points should
   be specified, depending on whether triangles or quads are used. The unused indices should be set to -1.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] hexCells - For each cell: 8 node indices. Length in bytes = 8 * nHexCells * 4
@param[in] hexCells_size - number of elements in hexCells array
@param[in] tetCells - For each cell: 4 node indices. Length in bytes = 4 * nTetCells * 4
@param[in] tetCells_size - number of elements in tetCells array
@param[in] priCells - For each cell: 6 node indices. Length in bytes = 6 * nPriCells * 4
@param[in] priCells_size - number of elements in priCells array
@param[in] pyrCells - For each cell: 5 node indices. Length in bytes = 5 * nPyrCells * 4
@param[in] pyrCells_size - number of elements in pyrCells array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_cells(int avmid,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size
);

/**
@brief Write edge data.
   This routine is used to write out the surface edge data for an unstructured mesh.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] edges- For each edge: 2 node indices. Length in bytes = 2 * nEdges * 4
@param[in] edges_size - number of elements in edges array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_edges(int avmid,
   int* edges, int edges_size
);

/**
@brief Write unstruc AMR data.
   This routine is used to write out the AMR data for an unstructured mesh.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] nodesOnGeometry - Array of AMR_Node_Data structs, one for each nNodesOnGeometry
   Length in bytes = ( 3 * 4 + 2 * 8 ) * nNodesOnGeometry
@param[in] nodesOnGeometry_size - number of elements in nodesOnGeometry array
@param[in] edgesOnGeometry - For each edge: mesh edge ID, GeomType (1=edge, 2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nEdgesOnGeometry * 4
@param[in] edgesOnGeometry_size - number of elements in edgesOnGeometry array
@param[in] facesOnGeometry - For each face: mesh face ID, GeomType (2=face), geomTopoId (matching GeomType)
   Length in bytes = 3 * nFacesOnGeometry * 4
@param[in] facesOnGeometry_size - number of elements in facesOnGeometry array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_amr(int avmid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);

/**
@brief Write unstruc cell geometry region ID data.
   This routine is used to write out the geometry region IDs associated with each mesh cell for AMR purposes.
   The arrays are implicitly ordered to match the cells in the hexCells/tetCells/priCells/pyrCells arrays.
@param[in] fileid - fileid as returned from @ref avm_read_headers
@param[in] hexGeomIds - For each hexCell, a cooresponding geometry volume region Id. Length in bytes = nHexCells * 4
@param[in] hexGeomIds_size - number of elements in hexGeomIDs array
@param[in] tetGeomIds - For each tetCell, a cooresponding geometry volume region Id. Length in bytes = nTetCells * 4
@param[in] tetGeomIds_size - number of elements in tetGeomIDs array
@param[in] priGeomIds - For each priCell, a cooresponding geometry volume region Id. Length in bytes = nPriCells * 4
@param[in] priGeomIds_size - number of elements in priGeomIDs array
@param[in] pyrGeomIds - For each pyrCell, a cooresponding geometry volume region Id. Length in bytes = nPyrCells * 4
@param[in] pyrGeomIds_size - number of elements in pyrGeomIDs array
@return 0 on success, >0 on failure
*/
int avm_unstruc_write_amr_volumeids(int avmid,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size);

/** 
@} 
@name Cartesian Mesh Data Functions 
@brief Routines to process cart mesh data
@{ 
*/

int avm_cart_read_iblank(int avmid, int* iblanks, int size);

int avm_cart_write_iblank(int avmid, int* iblanks, int size);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AVMESH_API */

/** @} */
