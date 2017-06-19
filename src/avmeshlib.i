%module avmeshlib

%{
#define SWIG_FILE_WITH_INIT
#include "avmesh.h"
%}

%rename(read_headers)               avm_read_headers;
%rename(new_file)                   avm_new_file;
%rename(write_headers)              avm_write_headers;
%rename(close)                      avm_close;
%rename(select)                     avm_select;
%rename(get_int)                    avm_get_int;
%rename(get_real)                   avm_get_real;
%rename(get_str)                    avm_get_str;
%rename(get_int_array)              avm_get_int_array;
%rename(get_real_array)             avm_get_real_array;
%rename(set_int)                    avm_set_int;
%rename(set_real)                   avm_set_real;
%rename(set_str)                    avm_set_str;
%rename(set_int_array)              avm_set_int_array;
%rename(set_real_array)             avm_set_real_array;
%rename(mesh_header_offset)         avm_mesh_header_offset;
%rename(mesh_data_offset)           avm_mesh_data_offset;
%rename(seek_to_mesh)               avm_seek_to_mesh;
%rename(get_error_str)              avm_get_error_str;
%rename(bfstruc_read_xyz_r4)        avm_bfstruc_read_xyz_r4;
%rename(bfstruc_read_xyz_r8)        avm_bfstruc_read_xyz_r8;
%rename(bfstruc_read_iblank)        avm_bfstruc_read_iblank;
%rename(bfstruc_write_xyz_r4)       avm_bfstruc_write_xyz_r4;
%rename(bfstruc_write_xyz_r8)       avm_bfstruc_write_xyz_r8;
%rename(bfstruc_write_iblank)       avm_bfstruc_write_iblank;
%rename(strand_read_r4)             avm_strand_read_r4;
%rename(strand_read_r8)             avm_strand_read_r8;
%rename(strand_write_r4)            avm_strand_write_r4;
%rename(strand_write_r8)            avm_strand_write_r8;
%rename(strand_read_nodes_r4)       avm_strand_read_nodes_r4;
%rename(strand_read_nodes_r8)       avm_strand_read_nodes_r8;
%rename(strand_write_nodes_r4)      avm_strand_write_nodes_r4;
%rename(strand_write_nodes_r8)      avm_strand_write_nodes_r8;
%rename(strand_read_faces)          avm_strand_read_faces;
%rename(strand_write_faces)         avm_strand_write_faces;
%rename(strand_read_edges)          avm_strand_read_edges;
%rename(strand_write_edges)         avm_strand_write_edges;
%rename(strand_read_amr)            avm_strand_read_amr;
%rename(strand_write_amr)           avm_strand_write_amr;
%rename(unstruc_read_nodes_r4)      avm_unstruc_read_nodes_r4;
%rename(unstruc_read_nodes_r8)      avm_unstruc_read_nodes_r8;
%rename(unstruc_read_partial_nodes_r4) avm_unstruc_read_partial_nodes_r4;
%rename(unstruc_read_partial_nodes_r8) avm_unstruc_read_partial_nodes_r8;
%rename(unstruc_read_faces)         avm_unstruc_read_faces;
%rename(unstruc_read_tri)           avm_unstruc_read_tri;
%rename(unstruc_read_quad)          avm_unstruc_read_quad;
%rename(unstruc_read_partial_faces) avm_unstruc_read_partial_faces;
%rename(unstruc_read_bnd_faces)     avm_unstruc_read_bnd_faces;
%rename(unstruc_read_cells)         avm_unstruc_read_cells;
%rename(unstruc_read_partial_cells) avm_unstruc_read_partial_cells;
%rename(unstruc_read_edges)         avm_unstruc_read_edges;
%rename(unstruc_read_amr)           avm_unstruc_read_amr;
%rename(unstruc_read_amr_volumeids) avm_unstruc_read_amr_volumeids;
%rename(unstruc_write_nodes_r4)     avm_unstruc_write_nodes_r4;
%rename(unstruc_write_nodes_r8)     avm_unstruc_write_nodes_r8;
%rename(unstruc_write_faces)        avm_unstruc_write_faces;
%rename(unstruc_write_bnd_faces)    avm_unstruc_write_bnd_faces;
%rename(unstruc_write_bnd_faces)    avm_unstruc_write_bnd_faces;
%rename(unstruc_write_cells)        avm_unstruc_write_cells;
%rename(unstruc_write_edges)        avm_unstruc_write_edges;
%rename(unstruc_write_amr)          avm_unstruc_write_amr;
%rename(unstruc_write_amr_volumeids) avm_unstruc_write_amr_volumeids;
%rename(cart_read_iblank)           avm_cart_read_iblank;
%rename(cart_write_iblank)          avm_cart_write_iblank;
%include "carrays.i"
%array_class(int, intArray);
%array_class(float, r4Array);
%array_class(double, r8Array);
%array_functions(AMR_Node_Data, AMR_Node_Data_Array);
%include "typemaps.i"
extern int avm_read_headers(int* OUTPUT, const char*);
extern int avm_new_file(int* OUTPUT, const char*, int rev);
extern int avm_write_headers(int);
extern int avm_close(int);
extern int avm_select(int, const char*, int);
extern int avm_get_int(int, const char*, int* OUTPUT);
extern int avm_get_real(int, const char*, double* OUTPUT);
%include "cstring.i"
%cstring_output_maxsize(char* out, int outsize);
typedef struct {
   double u;
   double v;
   int nodeIndex;
   int geomType;
   int geomTopoId;
} AMR_Node_Data;
typedef enum AVM_Flags_t {
   AVM_NODES = 1,
   AVM_CELLS = 2,
   AVM_NODESCELLS = 4,
   AVM_BOUNDARY = 8,
   AVM_INTERIOR = 16
} AVM_Flags;
extern int avm_get_str(int, const char*, char* out, int outsize);
extern int avm_get_int_array(int, const char*, int*, int);
extern int avm_get_real_array(int, const char*, double*, int);
extern int avm_set_int(int, const char*, int);
extern int avm_set_real(int, const char*, double);
extern int avm_set_str(int, const char*, const char*, int);
extern int avm_set_int_array(int, const char*, const int*, int);
extern int avm_set_real_array(int, const char*, const double*, int);
extern int avm_mesh_header_offset(int, int, off_t* OUTPUT);
/* off_t is defined as long long on the mac */
#ifdef __APPLE__
extern int avm_mesh_data_offset(int, int, off_t* OUTPUT);
#else
extern int avm_mesh_data_offset(int, int, long* OUTPUT);
#endif
extern int avm_seek_to_mesh(int, int);
extern char* avm_get_error_str();
extern int avm_bfstruc_read_xyz_r4(int, float*, float*, float*, int);
extern int avm_bfstruc_read_xyz_r8(int, double*, double*, double*, int);
extern int avm_bfstruc_read_iblank(int, int*, int);
extern int avm_bfstruc_write_xyz_r4(int, float*, float*, float*, int);
extern int avm_bfstruc_write_xyz_r8(int, double*, double*, double*, int);
extern int avm_bfstruc_write_iblank(int, int*, int);
extern int avm_strand_read_r4(int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   float*, int,
   float*, int,
   float*, int
);
extern int avm_strand_read_r8(int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   double*, int,
   double*, int,
   double*, int
);
extern int avm_strand_write_r4(int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   float*, int,
   float*, int,
   float*, int
);
extern int avm_strand_write_r8(int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   int*, int,
   double*, int,
   double*, int,
   double*, int
);
int avm_strand_read_nodes_r4(int,
   float*, int,
   int*,   int,
   int*,   int,
   float*, int,
   float*, int
);
int avm_strand_read_nodes_r8(int,
   double*, int,
   int*,    int,
   int*,    int,
   double*, int,
   double*, int
);
int avm_strand_read_faces(int,
   int*, int,
   int*, int,
   int*, int,
   int*, int
);
int avm_strand_read_edges(int,
   int*, int,
   int*, int
);
int avm_strand_read_amr(int,
   AMR_Node_Data*, int,
   int*,           int,
   int*,           int
);
int avm_strand_write_nodes_r4(int,
   float*, int,
   int*,   int,
   int*,   int,
   float*, int,
   float*, int
);
int avm_strand_write_nodes_r8(int,
   double*, int,
   int*,    int,
   int*,    int,
   double*, int,
   double*, int
);
int avm_strand_write_faces(int,
   int*, int,
   int*, int,
   int*, int,
   int*, int
);
int avm_strand_write_edges(int,
   int*, int,
   int*, int
);
int avm_strand_write_amr(int,
   AMR_Node_Data*, int,
   int*,           int,
   int*,           int
);
extern int avm_unstruc_read_nodes_r4(int, float*, int);
extern int avm_unstruc_read_nodes_r8(int, double*, int);
extern int avm_unstruc_read_partial_nodes_r4(int, float*, int, int, int);
extern int avm_unstruc_read_partial_nodes_r8(int, double*, int, int, int);
extern int avm_unstruc_read_faces(int, int*, int, int*, int, int*, int);
extern int avm_unstruc_read_tri(int, int*, int, int, int, int, int);
extern int avm_unstruc_read_quad(int, int*, int, int, int, int, int);
extern int avm_unstruc_read_partial_faces(int, int*, int, int, int, int*, int, int, int, int*, int, int, int);
extern int avm_unstruc_read_bnd_faces(int, int*, int, int*, int, int*, int);
extern int avm_unstruc_read_cells(int, int*, int, int*, int, int*, int, int*, int);
extern int avm_unstruc_read_partial_cells(int, int*, int, int, int, int*, int, int, int, int*, int, int, int, int*, int, int, int);
extern int avm_unstruc_read_edges(int, int*, int);
extern int avm_unstruc_read_amr(int, AMR_Node_Data*, int, int*, int, int*, int);
extern int avm_unstruc_read_amr_volumeids(int, int*, int, int*, int, int*, int, int*, int);
extern int avm_unstruc_write_nodes_r4(int, float*, int);
extern int avm_unstruc_write_nodes_r8(int, double*, int);
extern int avm_unstruc_write_faces(int, int*, int, int*, int, int*, int);
extern int avm_unstruc_write_bnd_faces(int, int*, int, int*, int, int*, int);
extern int avm_unstruc_write_cells(int, int*, int, int*, int, int*, int, int*, int);
extern int avm_unstruc_write_edges(int, int*, int);
extern int avm_unstruc_write_amr(int, AMR_Node_Data*, int, int*, int, int*, int);
extern int avm_unstruc_write_amr_volumeids(int, int*, int, int*, int, int*, int, int*, int);
extern int avm_cart_read_iblank(int, int*, int);
extern int avm_cart_write_iblank(int, int*, int);
