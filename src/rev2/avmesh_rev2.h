#ifndef AVMESH_REV2
#define AVMESH_REV2

#include <sys/types.h> /* for off_t */

namespace rev2 {

#define AVM_STD_STRING_LENGTH 128

int avm_open(rev2_avmesh_file *avf, int* avmid);

int avm_read_mesh_headers(rev2_avmesh_file* avf);

int avm_new_file(rev2_avmesh_file *avf, const char* filename);

int avm_write_headers(rev2_avmesh_file *avf);

int avm_select(rev2_avmesh_file* avf, const char* section, int id);

int avm_get_int(rev2_avmesh_file* avf, const char* field, int* value);

int avm_get_real(rev2_avmesh_file* avf, const char* field, double* value);

int avm_get_str(rev2_avmesh_file* avf, const char* field, char* str, int len);

int avm_get_int_array(rev2_avmesh_file* avf, const char* field, int* values, int len);

int avm_get_real_array(rev2_avmesh_file* avf, const char* field, double* values, int len);

int avm_set_int(rev2_avmesh_file* avf, const char* field, int value);

int avm_set_real(rev2_avmesh_file* avf, const char* field, double value);

int avm_set_str(rev2_avmesh_file* avf, const char* field, const char* str, int len);

int avm_set_int_array(rev2_avmesh_file* avf, const char* field, const int* values, int len);

int avm_set_real_array(rev2_avmesh_file* avf, const char* field, const double* values, int len);

int avm_mesh_header_offset(rev2_avmesh_file* avf, int mesh, off_t* offset);

int avm_mesh_data_offset(rev2_avmesh_file* avf, int mesh, off_t* offset);

int avm_seek_to_mesh(rev2_avmesh_file* avf, int mesh);

int avm_unstruc_seek_to(rev2_avmesh_file* avf, const char* section, off_t start);

int avm_nodes_per_tri(int p);

int avm_nodes_per_quad(int p);

int avm_nodes_per_tet(int p);

int avm_nodes_per_pyr(int p);

int avm_nodes_per_pri(int p);

int avm_nodes_per_hex(int p);

int seek(FILE* fp, off_t offset);

/**
@}
@name Strand Mesh Data Functions
@brief Routines to process strand mesh data
@{
*/

int avm_strand_read_nodes_r4(rev2_avmesh_file* avf,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

int avm_strand_read_nodes_r8(rev2_avmesh_file* avf,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,       int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

int avm_strand_read_faces(rev2_avmesh_file* avf,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
);

int avm_strand_read_edges(rev2_avmesh_file* avf,
  int* bndEdges,      int bndEdges_size,
   int* edgeTag,       int edgeTag_size
);

int avm_strand_read_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);

int avm_strand_write_nodes_r4(rev2_avmesh_file* avf,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
);

int avm_strand_write_nodes_r8(rev2_avmesh_file* avf,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
);

int avm_strand_write_faces(rev2_avmesh_file* avf,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
);

int avm_strand_write_edges(rev2_avmesh_file* avf,
   int* bndEdges,      int bndEdges_size,
   int* edgeTag,       int edgeTag_size
);

int avm_strand_write_amr(rev2_avmesh_file* avf,
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

int avm_unstruc_read_nodes_r4(rev2_avmesh_file* avf, float* xyz, int xyz_size);
int avm_unstruc_read_nodes_r8(rev2_avmesh_file* avf, double* xyz, int xyz_size);
int avm_unstruc_read_partial_nodes_r4(rev2_avmesh_file* avf, float* xyz, int xyz_size, int start, int end);
int avm_unstruc_read_partial_nodes_r8(rev2_avmesh_file* avf, double* xyz, int xyz_size, int start, int end);
int avm_unstruc_read_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);
int avm_unstruc_read_tri(rev2_avmesh_file* avf,
   int* triFaces, int triFaces_size,
   int stride, int start, int end, int flags
);
int avm_unstruc_read_quad(rev2_avmesh_file* avf,
   int* quadFaces, int quadFaces_size,
   int stride, int start, int end, int flags
);
int avm_unstruc_read_partial_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,  int triStart,  int triEnd,
   int* quadFaces, int quadFaces_size, int quadStart, int quadEnd
);
int avm_unstruc_read_bnd_faces(rev2_avmesh_file* avf,
   int* bndTriFaces,  int bndTriFaces_size,
   int* bndQuadFaces, int bndQuadFaces_size
);
int avm_unstruc_read_cells(rev2_avmesh_file* avf,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size
);
int avm_unstruc_read_partial_cells(rev2_avmesh_file* avf,
   int* hexCells, int hexCells_size, int hexStart, int hexEnd,
   int* tetCells, int tetCells_size, int tetStart, int tetEnd,
   int* priCells, int priCells_size, int priStart, int priEnd,
   int* pyrCells, int pyrCells_size, int pyrStart, int pyrEnd
);
int avm_unstruc_read_edges(rev2_avmesh_file* avf,
   int* edges, int edges_size
);

int avm_unstruc_read_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);
int avm_unstruc_read_amr_volumeids(rev2_avmesh_file* avf,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size);

int avm_unstruc_write_nodes_r4(rev2_avmesh_file* avf, float* xyz, int xyz_size);
int avm_unstruc_write_nodes_r8(rev2_avmesh_file* avf, double* xyz, int xyz_size);
int avm_unstruc_write_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);
int avm_unstruc_write_bnd_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
);
int avm_unstruc_write_cells_nosize(rev2_avmesh_file* avf,
   int* hexCells,
   int* tetCells,
   int* priCells,
   int* pyrCells
);
int avm_unstruc_write_cells(rev2_avmesh_file* avf,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size
);
int avm_unstruc_write_edges(rev2_avmesh_file* avf,
   int* edges, int edges_size
);

int avm_unstruc_write_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
);
int avm_unstruc_write_amr_volumeids(rev2_avmesh_file* avf,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size);

/** @} */
} //namespace rev2

#endif /* AVMESH_REV2 */

/** @} */

