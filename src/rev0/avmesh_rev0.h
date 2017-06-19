#ifndef AVMESH_REV0
#define AVMESH_REV0

#include <sys/types.h> /* for off_t */

namespace rev0 {

#define AVM_STD_STRING_LENGTH 128

int avm_read_headers(int* avmid, const char* filename);

int avm_new_file(rev0_avmesh_file* avf, const char* filename);

int avm_write_headers(rev0_avmesh_file* avf);

int avm_open(rev0_avmesh_file* avf, int* avmid);

int avm_read_mesh_headers(rev0_avmesh_file* avf);

int avm_close(int avmid);

int avm_select(rev0_avmesh_file* avf, const char* section, int id);

int avm_get_int(rev0_avmesh_file* avf, const char* field, int* value);

int avm_get_real(rev0_avmesh_file* avf, const char* field, double* value);

int avm_get_str(rev0_avmesh_file* avf, const char* field, char* str, int len);

int avm_get_int_array(rev0_avmesh_file* avf, const char* field, int* values, int len);

int avm_get_real_array(rev0_avmesh_file* avf, const char* field, double* values, int len);

int avm_set_int(rev0_avmesh_file* avf, const char* field, int value);

int avm_set_real(rev0_avmesh_file* avf, const char* field, double value);

int avm_set_str(rev0_avmesh_file* avf, const char* field, const char* str, int len);

int avm_set_int_array(rev0_avmesh_file* avf, const char* field, const int* values, int len);

int avm_set_real_array(rev0_avmesh_file* avf, const char* field, const double* values, int len);

int avm_mesh_header_offset(rev0_avmesh_file* avf, int mesh, off_t* offset);

int avm_mesh_data_offset(rev0_avmesh_file* avf, int mesh, off_t* offset);

int avm_seek_to_mesh(rev0_avmesh_file* avf, int mesh);


int avm_bfstruc_read_xyz_r4(rev0_avmesh_file* avf, float* x, float* y, float* z, int size);
int avm_bfstruc_read_xyz_r8(rev0_avmesh_file* avf, double* x, double* y, double* z, int size);
int avm_bfstruc_read_iblank(rev0_avmesh_file* avf, int* iblanks, int size);

int avm_bfstruc_write_xyz_r4(rev0_avmesh_file* avf, float* x, float* y, float* z, int size);
int avm_bfstruc_write_xyz_r8(rev0_avmesh_file* avf, double* x, double* y, double* z, int size);
int avm_bfstruc_write_iblank(rev0_avmesh_file* avf, int* iblanks, int size);

/** 
@} 
@name Strand Mesh Data Functions 
@brief Routines to process strand mesh data
@{ 
*/

int avm_strand_read_r4(rev0_avmesh_file* avf,
   int* triConn,       int triConn_size,
   int* quadConn,      int quadConn_size,
   int* bndEdges,      int bndEdges_size,
   int* nodeClip,      int nodeClip_size,
   int* faceClip,      int faceClip_size,
   int* faceTag,       int faceTag_size,
   int* edgeTag,       int edgeTag_size,
   float* xSurf,       int xSurf_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

int avm_strand_read_r8(rev0_avmesh_file* avf,
   int* triConn,        int triConn_size,
   int* quadConn,       int quadConn_size,
   int* bndEdges,       int bndEdges_size,
   int* nodeClip,       int nodeClip_size,
   int* faceClip,       int faceClip_size,
   int* faceTag,        int faceTag_size,
   int* edgeTag,        int edgeTag_size,
   double* xSurf,       int xSurf_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

int avm_strand_write_r4(rev0_avmesh_file* avf,
   int* triConn,       int triConn_size,
   int* quadConn,      int quadConn_size,
   int* bndEdges,      int bndEdges_size,
   int* nodeClip,      int nodeClip_size,
   int* faceClip,      int faceClip_size,
   int* faceTag,       int faceTag_size,
   int* edgeTag,       int edgeTag_size,
   float* xSurf,       int xSurf_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

int avm_strand_write_r8(rev0_avmesh_file* avf,
   int* triConn,        int triConn_size,
   int* quadConn,       int quadConn_size,
   int* bndEdges,       int bndEdges_size,
   int* nodeClip,       int nodeClip_size,
   int* faceClip,       int faceClip_size,
   int* faceTag,        int faceTag_size,
   int* edgeTag,        int edgeTag_size,
   double* xSurf,       int xSurf_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

/** 
@} 
@name Unstructured Mesh Data Functions 
@brief Routines to process unstruc mesh data
@{ 
*/

int avm_unstruc_read_nodes_r4(rev0_avmesh_file* avf, float* xyz, int xyz_size);
int avm_unstruc_read_nodes_r8(rev0_avmesh_file* avf, double* xyz, int xyz_size);
int avm_unstruc_read_faces(rev0_avmesh_file* avf, 
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);

int avm_unstruc_write_nodes_r4(rev0_avmesh_file* avf, float* xyz, int xyz_size);
int avm_unstruc_write_nodes_r8(rev0_avmesh_file* avf, double* xyz, int xyz_size);
int avm_unstruc_write_faces(rev0_avmesh_file* avf, 
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);

/** 
@} 
@name Cartesian Mesh Data Functions 
@brief Routines to process cart mesh data
@{ 
*/

int avm_cart_read_iblank(rev0_avmesh_file* avf, int* iblanks, int size);

int avm_cart_write_iblank(rev0_avmesh_file* avf, int* iblanks, int size);

/** @} */

} //namespace rev0

#endif /* AVMESH_REV1 */

/** @} */
