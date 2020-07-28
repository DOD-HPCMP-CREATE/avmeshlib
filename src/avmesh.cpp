#include <map>
#include <string>
#include <vector> 
using namespace std;

#include <stdio.h>
#include <string.h>
#include "avmesh.h"
#include "avmesh_file.h"
#include "rev0/avmhdrs.h"
#include "rev0/avmesh_rev0.h"
#include "rev1/avmhdrs.h"
#include "rev1/avmesh_rev1.h"
#include "rev2/avmhdrs.h"
#include "rev2/avmesh_rev2.h"

static int file_counter=1;
static char error_msg[AVM_STD_STRING_LENGTH] = "\0";

static map<int, avmesh_file*> file_list;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

#define CHECK_PTR(v) if(NULL==v) return 1;
#define RETURN_ERROR(str) { avm_set_error_str(str); return 1; }

extern "C"
{

void avm_set_error_str(const char* str);

int avm_open(const char* filename, int* id)
{
   if (filename==NULL) RETURN_ERROR("avm_open: filename is NULL");
   if (id==NULL)       RETURN_ERROR("avm_open: id is NULL");
   avmesh_file *avf = new avmesh_file;
   avf->rev0 = NULL;
   avf->rev1 = NULL;
   avf->rev2 = NULL;
   avf->formatRevision = -1;
   file_list[file_counter] = avf;

   // is this a AVMesh File?
   FILE* fp = NULL;
   bool byte_swap = 0;
   file_id_prefix_t file_prefix;
   fp = fopen(filename, "rb");
   if (fp==NULL) RETURN_ERROR("avm_open: fopen failed");
   if(!file_id_prefix::read(fp, &byte_swap, &file_prefix)) {
      avm_close(file_counter);
      RETURN_ERROR("avm_open: failed reading file_id_prefix");
   }

   if (file_prefix.formatRevision == 0) {
      avf->formatRevision = 0;
      avf->rev0 = new rev0_avmesh_file;
      avf->rev0->fp = fp;
      avf->rev0->byte_swap = byte_swap;
      avf->rev0->file_prefix = file_prefix;
      avf->rev0->filename = filename;
      avf->rev0->file_prefix.formatRevision = 0;
      if (rev0::avm_open(avf->rev0, id)) {
         delete avf->rev0;
         delete avf;
         return 1;
      }
      *id = file_counter++;
      return 0;
   }
   else if (file_prefix.formatRevision == 1) {
      avf->formatRevision = 1;
      avf->rev1 = new rev1_avmesh_file;
      avf->rev1->fp = fp;
      avf->rev1->byte_swap = byte_swap;
      avf->rev1->file_prefix = file_prefix;
      avf->rev1->filename = filename;
      avf->rev1->file_prefix.formatRevision = 1;
      if (rev1::avm_open(avf->rev1, id)) {
         delete avf->rev1;
         delete avf;
         return 1;
      }
      *id = file_counter++;
      return 0;
   }
   else if (file_prefix.formatRevision == 2) {
      avf->formatRevision = 2;
      avf->rev2 = new rev2_avmesh_file;
      avf->rev2->fp = fp;
      avf->rev2->byte_swap = byte_swap;
      avf->rev2->file_prefix = file_prefix;
      avf->rev2->filename = filename;
      avf->rev2->file_prefix.formatRevision = 2;
      if (rev2::avm_open(avf->rev2, id)) {
         delete avf->rev2;
         delete avf;
         return 1;
      }
      *id = file_counter++;
      return 0;
   }

   avm_close(file_counter);
   RETURN_ERROR("avm_open: unsupported formatRevision in file");
}

int avm_read_mesh_headers(int fileid)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_read_mesh_headers: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_read_mesh_headers(avf->rev0);
   }

   else if (avf->formatRevision == 1) {
      return rev1::avm_read_mesh_headers(avf->rev1);
   }

   else if (avf->formatRevision == 2) {
      return rev2::avm_read_mesh_headers(avf->rev2);
   }

   RETURN_ERROR("avm_read_mesh_headers: unsupported formatRevision in file");
}

int avm_new_file(int* id, const char* filename, int rev)
{
#define CHECK_RETURN(v) if(v) { avm_close(file_counter); return 1; }
   if (filename==NULL) RETURN_ERROR("avm_new_file: filename is NULL");
   if (id==NULL)       RETURN_ERROR("avm_new_file: id is NULL");
   avmesh_file *avf = new avmesh_file;
   file_list[file_counter] = avf;

   if (rev == 0) {
      avf->formatRevision = 0;
      avf->rev0 = new rev0_avmesh_file;
      avf->rev0->file_prefix.formatRevision = 0;
      CHECK_RETURN(rev0::avm_new_file(avf->rev0, filename));
      *id = file_counter++;
      return 0;
   }
   else if (rev == 1) {
      avf->formatRevision = 1;
      avf->rev1 = new rev1_avmesh_file;
      avf->rev1->file_prefix.formatRevision = 1;
      CHECK_RETURN(rev1::avm_new_file(avf->rev1, filename));
      *id = file_counter++;
      return 0;
   }
   else if (rev == 2) {
      avf->formatRevision = 2;
      avf->rev2 = new rev2_avmesh_file;
      avf->rev2->file_prefix.formatRevision = 2;
      CHECK_RETURN(rev2::avm_new_file(avf->rev2, filename));
      *id = file_counter++;
      return 0;
   }
   RETURN_ERROR("avm_new_file: unsupported rev");
#undef CHECK_RETURN
}

int avm_write_headers(int fileid)
{
#define CHECK_RETURN(v) if(v) { avm_close(fileid); return 1; }
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_write_headers: fileid invalid");

   if (avf->formatRevision == 0) {
      CHECK_RETURN(rev0::avm_write_headers(avf->rev0));
      return 0;
   }
   else if (avf->formatRevision == 1) {
      CHECK_RETURN(rev1::avm_write_headers(avf->rev1));
      return 0;
   }
   else if (avf->formatRevision == 2) {
      CHECK_RETURN(rev2::avm_write_headers(avf->rev2));
      return 0;
   }

   RETURN_ERROR("avm_write_headers: unsupported formatRevision in file");
#undef CHECK_RETURN
}

int avm_read_headers(int* fileid, const char* filename)
{
   if (fileid==NULL)   RETURN_ERROR("avm_read_headers: id is NULL");
   if (filename==NULL) RETURN_ERROR("avm_read_headers: filename is NULL");
   if (avm_open(filename, fileid)) return 1;
   if (avm_read_mesh_headers(*fileid)) {
      avm_close(*fileid);
      return 1;
   }
   return 0;
}

int avm_close(int fileid)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_close: fileid invalid");

   if (avf->formatRevision == 0) {
      if (avf->rev0 && avf->rev0->fp) {
         fclose(avf->rev0->fp);
         delete avf->rev0;
      }
   }
   else if (avf->formatRevision == 1) {
      if (avf->rev1 && avf->rev1->fp) {
         fclose(avf->rev1->fp);
         delete avf->rev1;
      }
   }
   else if (avf->formatRevision == 2) {
      if (avf->rev2 && avf->rev2->fp) {
         fclose(avf->rev2->fp);
         delete avf->rev2;
      }
   }

   delete avf;
   file_list[fileid] = NULL;

   return 0;
}

int avm_select(int fileid, const char* section, int id)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_select: fileid invalid");
   if (section==NULL) RETURN_ERROR("avm_select: section is NULL");

   if (avf->formatRevision == 0) {
      return rev0::avm_select(avf->rev0, section, id);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_select(avf->rev1, section, id);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_select(avf->rev2, section, id);
   }
   RETURN_ERROR("avm_select: unsupported formatRevision in file");
}

int avm_get_int(int fileid, const char* field, int* value)
{
   if (field==NULL) RETURN_ERROR("avm_get_int: field is NULL");
   if (value==NULL) RETURN_ERROR("avm_get_int: value is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_get_int: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_get_int(avf->rev0, field, value);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_get_int(avf->rev1, field, value);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_get_int(avf->rev2, field, value);
   }

   RETURN_ERROR("avm_get_int: unsupported formatRevision in file");
}

int avm_get_real(int fileid, const char* field, double* value)
{
   if (field==NULL) RETURN_ERROR("avm_get_real: field is NULL");
   if (value==NULL) RETURN_ERROR("avm_get_real: value is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_get_real: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_get_real(avf->rev0, field, value);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_get_real(avf->rev1, field, value);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_get_real(avf->rev2, field, value);
   }

   RETURN_ERROR("avm_get_real: unsupported formatRevision in file");
}

int avm_get_str(int fileid, const char* field, char* str, int len)
{
   if (field==NULL) RETURN_ERROR("avm_get_str: field is NULL");
   if (str==NULL)   RETURN_ERROR("avm_get_str: str is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_get_str: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_get_str(avf->rev0, field, str, len);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_get_str(avf->rev1, field, str, len);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_get_str(avf->rev2, field, str, len);
   }

   RETURN_ERROR("avm_get_str: unsupported formatRevision in file");
}

int avm_get_int_array(int fileid, const char* field, int* values, int len)
{
   if (field==NULL) RETURN_ERROR("avm_get_int_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_get_int_array: values is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_get_int_array: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_get_int_array(avf->rev0, field, values, len);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_get_int_array(avf->rev1, field, values, len);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_get_int_array(avf->rev2, field, values, len);
   }

   RETURN_ERROR("avm_get_int_array: unsupported formatRevision in file");
}

int avm_get_real_array(int fileid, const char* field, double* values, int len)
{
   if (field==NULL) RETURN_ERROR("avm_get_real_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_get_real_array: values is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_get_real_array: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_get_real_array(avf->rev0, field, values, len);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_get_real_array(avf->rev1, field, values, len);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_get_real_array(avf->rev2, field, values, len);
   }

   RETURN_ERROR("avm_get_real_array: unsupported formatRevision in file");
}

int avm_set_int(int fileid, const char* field, int value)
{
   if (field==NULL) RETURN_ERROR("avm_set_int: field is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_set_int: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_set_int(avf->rev0, field, value);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_set_int(avf->rev1, field, value);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_set_int(avf->rev2, field, value);
   }

   RETURN_ERROR("avm_set_int: unsupported formatRevision in file");
}

int avm_set_real(int fileid, const char* field, double value)
{
   if (field==NULL) RETURN_ERROR("avm_set_real: field is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_set_real: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_set_real(avf->rev0, field, value);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_set_real(avf->rev1, field, value);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_set_real(avf->rev2, field, value);
   }

   RETURN_ERROR("avm_set_real: unsupported formatRevision in file");
}

int avm_set_str(int fileid, const char* field, const char* str, int len)
{
   if (field==NULL) RETURN_ERROR("avm_set_str: field is NULL");
   if (str==NULL) RETURN_ERROR("avm_set_str: str is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_set_str: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_set_str(avf->rev0, field, str, len);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_set_str(avf->rev1, field, str, len);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_set_str(avf->rev2, field, str, len);
   }

   RETURN_ERROR("avm_set_str: unsupported formatRevision in file");
}

int avm_set_int_array(int fileid, const char* field, const int* values, int len)
{
   if (field==NULL) RETURN_ERROR("avm_set_int_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_set_int_array: values is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_set_int_array: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_set_int_array(avf->rev0, field, values, len);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_set_int_array(avf->rev1, field, values, len);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_set_int_array(avf->rev2, field, values, len);
   }

   RETURN_ERROR("avm_set_int_array: unsupported formatRevision in file");
}

int avm_set_real_array(int fileid, const char* field, const double* values, int len)
{
   if (field==NULL) RETURN_ERROR("avm_set_real_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_set_real_array: values is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_set_real_array: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_set_real_array(avf->rev0, field, values, len);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_set_real_array(avf->rev1, field, values, len);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_set_real_array(avf->rev2, field, values, len);
   }

   RETURN_ERROR("avm_set_real_array: unsupported formatRevision in file");
}

char* avm_get_error_str()
{
   return error_msg;
}

void avm_set_error_str(const char* str)
{
   strncpy(error_msg,str,AVM_STD_STRING_LENGTH);
}

int avm_mesh_header_offset(int fileid, int mesh, off_t* offset)
{
   if (offset==NULL) RETURN_ERROR("avm_mesh_header_offset: offset is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_mesh_header_offset: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_mesh_header_offset(avf->rev0, mesh, offset);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_mesh_header_offset(avf->rev1, mesh, offset);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_mesh_header_offset(avf->rev2, mesh, offset);
   }

   RETURN_ERROR("avm_mesh_header_offset: unsupported formatRevision in file");
}

int avm_mesh_data_offset(int fileid, int mesh, off_t* offset)
{
   if (offset==NULL) RETURN_ERROR("avm_mesh_data_offset: offset is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_mesh_data_offset: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_mesh_data_offset(avf->rev0, mesh, offset);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_mesh_data_offset(avf->rev1, mesh, offset);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_mesh_data_offset(avf->rev2, mesh, offset);
   }

   RETURN_ERROR("avm_mesh_data_offset: unsupported formatRevision in file");
}

int avm_seek_to_mesh(int fileid, int mesh)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_seek_to_mesh: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_seek_to_mesh(avf->rev0, mesh);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_seek_to_mesh(avf->rev1, mesh);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_seek_to_mesh(avf->rev2, mesh);
   }

   RETURN_ERROR("avm_seek_to_mesh: unsupported formatRevision in file");
}

int avm_nodes_per_tri(int fileid, int p)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_nodes_per_tri: fileid invalid");

   if (avf->formatRevision == 2) {
      return rev2::avm_nodes_per_tri(p);
   }

   RETURN_ERROR("avm_nodes_per_tri: unsupported formatRevision in file");
}

int avm_nodes_per_quad(int fileid, int p)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_nodes_per_quad: fileid invalid");

   if (avf->formatRevision == 2) {
      return rev2::avm_nodes_per_quad(p);
   }

   RETURN_ERROR("avm_nodes_per_quad: unsupported formatRevision in file");
}

int avm_nodes_per_tet(int fileid, int p)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_nodes_per_tet: fileid invalid");

   if (avf->formatRevision == 2) {
      return rev2::avm_nodes_per_tet(p);
   }

   RETURN_ERROR("avm_nodes_per_tet: unsupported formatRevision in file");
}

int avm_nodes_per_pyr(int fileid, int p)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_nodes_per_pyr: fileid invalid");

   if (avf->formatRevision == 2) {
      return rev2::avm_nodes_per_pyr(p);
   }

   RETURN_ERROR("avm_nodes_per_pyr: unsupported formatRevision in file");
}

int avm_nodes_per_pri(int fileid, int p)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_nodes_per_pri: fileid invalid");

   if (avf->formatRevision == 2) {
      return rev2::avm_nodes_per_pri(p);
   }

   RETURN_ERROR("avm_nodes_per_pri: unsupported formatRevision in file");
}

int avm_nodes_per_hex(int fileid, int p)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_nodes_per_hex: fileid invalid");

   if (avf->formatRevision == 2) {
      return rev2::avm_nodes_per_hex(p);
   }

   RETURN_ERROR("avm_nodes_per_hex: unsupported formatRevision in file");
}

//
// bfstruc functions 
//

int avm_bfstruc_read_xyz_r4(
   int fileid,
   float* x,
   float* y,
   float* z,
   int size
)
{
   if (x==NULL) RETURN_ERROR("avm_bfstruc_read_xyz_r4: x is NULL");
   if (y==NULL) RETURN_ERROR("avm_bfstruc_read_xyz_r4: y is NULL");
   if (z==NULL) RETURN_ERROR("avm_bfstruc_read_xyz_r4: z is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_bfstruc_read_xyz_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_bfstruc_read_xyz_r4(avf->rev0, x, y, z, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_bfstruc_read_xyz_r4(avf->rev1, x, y, z, size);
   }

   RETURN_ERROR("avm_bfstruc_read_xyz_r4: unsupported formatRevision in file");
}

int avm_bfstruc_read_xyz_r8(
   int fileid,
   double* x,
   double* y,
   double* z,
   int size
)
{
   if (x==NULL) RETURN_ERROR("avm_bfstruc_read_xyz_r8: x is NULL");
   if (y==NULL) RETURN_ERROR("avm_bfstruc_read_xyz_r8: y is NULL");
   if (z==NULL) RETURN_ERROR("avm_bfstruc_read_xyz_r8: z is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_bfstruc_read_xyz_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_bfstruc_read_xyz_r8(avf->rev0, x, y, z, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_bfstruc_read_xyz_r8(avf->rev1, x, y, z, size);
   }

   RETURN_ERROR("avm_bfstruc_read_xyz_r8: unsupported formatRevision in file");
}

int avm_bfstruc_read_iblank(
   int fileid,
   int* iblanks,
   int size
)
{
   if (iblanks==NULL) RETURN_ERROR("avm_bfstruc_read_iblank: iblanks is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_bfstruc_read_iblank: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_bfstruc_read_iblank(avf->rev0, iblanks, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_bfstruc_read_iblank(avf->rev1, iblanks, size);
   }

   RETURN_ERROR("avm_bfstruc_read_iblank: unsupported formatRevision in file");
}

int avm_bfstruc_write_xyz_r4(int fileid, float* x, float* y, float* z, int size)
{
   if (x==NULL) RETURN_ERROR("avm_bfstruc_write_xyz_r4: x is NULL");
   if (y==NULL) RETURN_ERROR("avm_bfstruc_write_xyz_r4: y is NULL");
   if (z==NULL) RETURN_ERROR("avm_bfstruc_write_xyz_r4: z is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_bfstruc_write_xyz_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_bfstruc_write_xyz_r4(avf->rev0, x, y, z, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_bfstruc_write_xyz_r4(avf->rev1, x, y, z, size);
   }

   RETURN_ERROR("avm_bfstruc_write_xyz_r4: unsupported formatRevision in file");
}

int avm_bfstruc_write_xyz_r8(int fileid, double* x, double* y, double* z, int size)
{
   if (x==NULL) RETURN_ERROR("avm_bfstruc_write_xyz_r8: x is NULL");
   if (y==NULL) RETURN_ERROR("avm_bfstruc_write_xyz_r8: y is NULL");
   if (z==NULL) RETURN_ERROR("avm_bfstruc_write_xyz_r8: z is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_bfstruc_write_xyz_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_bfstruc_write_xyz_r8(avf->rev0, x, y, z, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_bfstruc_write_xyz_r8(avf->rev1, x, y, z, size);
   }

   RETURN_ERROR("avm_bfstruc_write_xyz_r8: unsupported formatRevision in file");
}

int avm_bfstruc_write_iblank(int fileid, int* iblanks, int size)
{
   if (iblanks==NULL) RETURN_ERROR("avm_bfstruc_write_iblank: iblanks is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_bfstruc_write_iblank: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_bfstruc_write_iblank(avf->rev0, iblanks, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_bfstruc_write_iblank(avf->rev1, iblanks, size);
   }

   RETURN_ERROR("avm_bfstruc_write_iblank: unsupported formatRevision in file");
}

int avm_strand_read_r4(int fileid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* bndEdges,      int bndEdges_size,
   int* nodeClip,      int nodeClip_size,
   int* faceClip,      int faceClip_size,
   int* faceTag,       int faceTag_size,
   int* edgeTag,       int edgeTag_size,
   float* xyz,         int xyz_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
)
{
   CHECK_PTR(triFaces);
   CHECK_PTR(quadFaces);
   CHECK_PTR(bndEdges);
   CHECK_PTR(nodeClip);
   CHECK_PTR(faceClip);
   CHECK_PTR(faceTag);
   CHECK_PTR(edgeTag);
   CHECK_PTR(xyz);
   CHECK_PTR(pointingVec);
   CHECK_PTR(xStrand);
   if (triFaces_size > 0    && triFaces==NULL)    RETURN_ERROR("avm_strand_read_r4: triFaces is NULL");
   if (quadFaces_size > 0   && quadFaces==NULL)   RETURN_ERROR("avm_strand_read_r4: quadFaces is NULL");
   if (bndEdges_size > 0    && bndEdges==NULL)    RETURN_ERROR("avm_strand_read_r4: bndEdges is NULL");
   if (nodeClip_size > 0    && nodeClip==NULL)    RETURN_ERROR("avm_strand_read_r4: nodeClip is NULL");
   if (faceClip_size > 0    && faceClip==NULL)    RETURN_ERROR("avm_strand_read_r4: faceClip is NULL");
   if (faceTag_size > 0     && faceTag==NULL)     RETURN_ERROR("avm_strand_read_r4: faceTag is NULL");
   if (edgeTag_size > 0     && edgeTag==NULL)     RETURN_ERROR("avm_strand_read_r4: edgeTag is NULL");
   if (xyz_size > 0         && xyz==NULL)         RETURN_ERROR("avm_strand_read_r4: xyz is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_read_r4: pointingVec is NULL");
   if (xStrand_size > 0     && xStrand==NULL)     RETURN_ERROR("avm_strand_read_r4: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_strand_read_r4(avf->rev0, 
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         bndEdges,    bndEdges_size,
                         nodeClip,    nodeClip_size,
                         faceClip,    faceClip_size,
                         faceTag,     faceTag_size,
                         edgeTag,     edgeTag_size,
                         xyz,         xyz_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 1 || avf->formatRevision == 2) {
      RETURN_ERROR("avm_strand_read_r4: This call is only supported in formatRevision 0");
   }

   RETURN_ERROR("avm_strand_read_r4: unsupported formatRevision in file");
}

int avm_strand_read_r8(int fileid,
   int* triFaces,        int triFaces_size,
   int* quadFaces,       int quadFaces_size,
   int* bndEdges,       int bndEdges_size,
   int* nodeClip,       int nodeClip_size,
   int* faceClip,       int faceClip_size,
   int* faceTag,        int faceTag_size,
   int* edgeTag,        int edgeTag_size,
   double* xyz,         int xyz_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
)
{
   if (triFaces_size > 0    && triFaces==NULL)    RETURN_ERROR("avm_strand_read_r8: triFaces is NULL");
   if (quadFaces_size > 0   && quadFaces==NULL)   RETURN_ERROR("avm_strand_read_r8: quadFaces is NULL");
   if (bndEdges_size > 0    && bndEdges==NULL)    RETURN_ERROR("avm_strand_read_r8: bndEdges is NULL");
   if (nodeClip_size > 0    && nodeClip==NULL)    RETURN_ERROR("avm_strand_read_r8: nodeClip is NULL");
   if (faceClip_size > 0    && faceClip==NULL)    RETURN_ERROR("avm_strand_read_r8: faceClip is NULL");
   if (faceTag_size > 0     && faceTag==NULL)     RETURN_ERROR("avm_strand_read_r8: faceTag is NULL");
   if (edgeTag_size > 0     && edgeTag==NULL)     RETURN_ERROR("avm_strand_read_r8: edgeTag is NULL");
   if (xyz_size > 0         && xyz==NULL)         RETURN_ERROR("avm_strand_read_r8: xyz is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_read_r8: pointingVec is NULL");
   if (xStrand_size > 0     && xStrand==NULL)     RETURN_ERROR("avm_strand_read_r8: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_strand_read_r8(avf->rev0, 
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         bndEdges,    bndEdges_size,
                         nodeClip,    nodeClip_size,
                         faceClip,    faceClip_size,
                         faceTag,     faceTag_size,
                         edgeTag,     edgeTag_size,
                         xyz,         xyz_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 1 || avf->formatRevision == 2) {
      RETURN_ERROR("avm_strand_read_r8: This call is only supported in formatRevision 0");
   }

   RETURN_ERROR("avm_strand_read_r8: unsupported formatRevision in file");
}

int avm_strand_write_r4(int fileid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* bndEdges,      int bndEdges_size,
   int* nodeClip,      int nodeClip_size,
   int* faceClip,      int faceClip_size,
   int* faceTag,       int faceTag_size,
   int* edgeTag,       int edgeTag_size,
   float* xyz,         int xyz_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
)
{
   if (triFaces_size > 0    && triFaces==NULL)    RETURN_ERROR("avm_strand_write_r4: triFaces is NULL");
   if (quadFaces_size > 0   && quadFaces==NULL)   RETURN_ERROR("avm_strand_write_r4: quadFaces is NULL");
   if (bndEdges_size > 0    && bndEdges==NULL)    RETURN_ERROR("avm_strand_write_r4: bndEdges is NULL");
   if (nodeClip_size > 0    && nodeClip==NULL)    RETURN_ERROR("avm_strand_write_r4: nodeClip is NULL");
   if (faceClip_size > 0    && faceClip==NULL)    RETURN_ERROR("avm_strand_write_r4: faceClip is NULL");
   if (faceTag_size > 0     && faceTag==NULL)     RETURN_ERROR("avm_strand_write_r4: faceTag is NULL");
   if (edgeTag_size > 0     && edgeTag==NULL)     RETURN_ERROR("avm_strand_write_r4: edgeTag is NULL");
   if (xyz_size > 0         && xyz==NULL)         RETURN_ERROR("avm_strand_write_r4: xyz is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_write_r4: pointingVec is NULL");
   if (xStrand_size > 0     && xStrand==NULL)     RETURN_ERROR("avm_strand_write_r4: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_strand_write_r4(avf->rev0, 
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         bndEdges,    bndEdges_size,
                         nodeClip,    nodeClip_size,
                         faceClip,    faceClip_size,
                         faceTag,     faceTag_size,
                         edgeTag,     edgeTag_size,
                         xyz,         xyz_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 1 || avf->formatRevision == 2) {
      RETURN_ERROR("avm_strand_write_r4: This call is only supported in formatRevision 0");
   }

   RETURN_ERROR("avm_strand_write_r4: unsupported formatRevision in file");
}

int avm_strand_write_r8(int fileid,
   int* triFaces,        int triFaces_size,
   int* quadFaces,       int quadFaces_size,
   int* bndEdges,       int bndEdges_size,
   int* nodeClip,       int nodeClip_size,
   int* faceClip,       int faceClip_size,
   int* faceTag,        int faceTag_size,
   int* edgeTag,        int edgeTag_size,
   double* xyz,         int xyz_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
)
{
   if (triFaces_size > 0    && triFaces==NULL)    RETURN_ERROR("avm_strand_write_r8: triFaces is NULL");
   if (quadFaces_size > 0   && quadFaces==NULL)   RETURN_ERROR("avm_strand_write_r8: quadFaces is NULL");
   if (bndEdges_size > 0    && bndEdges==NULL)    RETURN_ERROR("avm_strand_write_r8: bndEdges is NULL");
   if (nodeClip_size > 0    && nodeClip==NULL)    RETURN_ERROR("avm_strand_write_r8: nodeClip is NULL");
   if (faceClip_size > 0    && faceClip==NULL)    RETURN_ERROR("avm_strand_write_r8: faceClip is NULL");
   if (faceTag_size > 0     && faceTag==NULL)     RETURN_ERROR("avm_strand_write_r8: faceTag is NULL");
   if (edgeTag_size > 0     && edgeTag==NULL)     RETURN_ERROR("avm_strand_write_r8: edgeTag is NULL");
   if (xyz_size > 0         && xyz==NULL)         RETURN_ERROR("avm_strand_write_r8: xyz is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_write_r8: pointingVec is NULL");
   if (xStrand_size > 0     && xStrand==NULL)     RETURN_ERROR("avm_strand_write_r8: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_strand_write_r8(avf->rev0, 
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         bndEdges,    bndEdges_size,
                         nodeClip,    nodeClip_size,
                         faceClip,    faceClip_size,
                         faceTag,     faceTag_size,
                         edgeTag,     edgeTag_size,
                         xyz,         xyz_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 1 || avf->formatRevision == 2) {
      RETURN_ERROR("avm_strand_write_r8: This call is only supported in formatRevision 0");
   }

   RETURN_ERROR("avm_strand_write_r8: unsupported formatRevision in file");
}

int avm_strand_read_nodes_r4(int fileid,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL)         RETURN_ERROR("avm_strand_read_nodes_r4: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL)    RETURN_ERROR("avm_strand_read_nodes_r4: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL)    RETURN_ERROR("avm_strand_read_nodes_r4: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL)     RETURN_ERROR("avm_strand_read_nodes_r4: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_nodes_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_read_nodes_r4: This call is only supported in formatRevision 1++");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_read_nodes_r4(avf->rev1,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_read_nodes_r4(avf->rev2,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }

   RETURN_ERROR("avm_strand_read_nodes_r4: unsupported formatRevision in file");
}

int avm_strand_read_nodes_r8(int fileid,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL)         RETURN_ERROR("avm_strand_read_nodes_r8: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL)    RETURN_ERROR("avm_strand_read_nodes_r4: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL)    RETURN_ERROR("avm_strand_read_nodes_r8: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL)     RETURN_ERROR("avm_strand_read_nodes_r8: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_nodes_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_read_nodes_r8: This call is only supported in formatRevision 1++");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_read_nodes_r8(avf->rev1,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_read_nodes_r8(avf->rev2,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }

   RETURN_ERROR("avm_strand_read_nodes_r8: unsupported formatRevision in file");
}

int avm_strand_read_faces(int fileid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
)
{
   if (triFaces_size > 0 && triFaces==NULL)    RETURN_ERROR("avm_strand_read_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL)   RETURN_ERROR("avm_strand_read_faces: quadFaces is NULL");
   if (polyFaces_size > 0 && polyFaces==NULL)   RETURN_ERROR("avm_strand_read_faces: polyFaces is NULL");
   if (faceTag_size > 0 && faceTag==NULL)     RETURN_ERROR("avm_strand_read_faces: faceTag is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_read_faces: This call is only supported in formatRevision 1++");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_read_faces(avf->rev1,
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         polyFaces,    polyFaces_size,
                         faceTag,     faceTag_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_read_faces(avf->rev2,
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         polyFaces,    polyFaces_size,
                         faceTag,     faceTag_size);
   }

   RETURN_ERROR("avm_strand_read_faces: unsupported formatRevision in file");
}

int avm_strand_read_edges(int fileid,
   int* bndEdges,       int bndEdges_size,
   int* edgeTag,        int edgeTag_size
)
{
   if (bndEdges_size > 0 && bndEdges==NULL)    RETURN_ERROR("avm_strand_read_edges: bndEdges is NULL");
   if (edgeTag_size > 0 && edgeTag==NULL)     RETURN_ERROR("avm_strand_read_edges: edgeTag is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_edges: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_read_edges: This call is only supported in formatRevision 1++");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_read_edges(avf->rev1,
                         bndEdges,    bndEdges_size,
                         edgeTag,     edgeTag_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_read_edges(avf->rev2,
                         bndEdges,    bndEdges_size,
                         edgeTag,     edgeTag_size);
   }

   RETURN_ERROR("avm_strand_read_edges: unsupported formatRevision in file");
}

int avm_strand_read_amr(int fileid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size)
{
   if (nodesOnGeometry_size > 0 && nodesOnGeometry==NULL) RETURN_ERROR("avm_strand_read_amr: nodesOnGeometry is NULL");
   if (edgesOnGeometry_size > 0 && edgesOnGeometry==NULL) RETURN_ERROR("avm_strand_read_amr: edgesOnGeometry is NULL");
   if (facesOnGeometry_size > 0 && facesOnGeometry==NULL) RETURN_ERROR("avm_strand_read_amr: facesOnGeometry is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_read_amr: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_read_amr: This call is only supported in formatRevision 1++");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_read_amr(avf->rev1,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_read_amr(avf->rev2,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }

   RETURN_ERROR("avm_strand_read_amr: unsupported formatRevision in file");
}

int avm_strand_write_nodes_r4(int fileid,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL)         RETURN_ERROR("avm_strand_write_nodes_r4: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL)    RETURN_ERROR("avm_strand_read_nodes_r4: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL)    RETURN_ERROR("avm_strand_write_nodes_r4: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_write_nodes_r4: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL)     RETURN_ERROR("avm_strand_write_nodes_r4: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_nodes_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_write_nodes_r4: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_write_nodes_r4(avf->rev1,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_write_nodes_r4(avf->rev2,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }

   RETURN_ERROR("avm_strand_write_nodes_r4: unsupported formatRevision in file");
}

int avm_strand_write_nodes_r8(int fileid,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL)         RETURN_ERROR("avm_strand_write_nodes_r8: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL)    RETURN_ERROR("avm_strand_read_nodes_r4: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL)    RETURN_ERROR("avm_strand_write_nodes_r8: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_write_nodes_r8: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL)     RETURN_ERROR("avm_strand_write_nodes_r8: xStrand is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_nodes_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_write_nodes_r8: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_write_nodes_r8(avf->rev1,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_write_nodes_r8(avf->rev2,
                         xyz,         xyz_size,
                         nodeID,      nodeID_size,
                         nodeClip,    nodeClip_size,
                         pointingVec, pointingVec_size,
                         xStrand,     xStrand_size);
   }

   RETURN_ERROR("avm_strand_write_nodes_r8: unsupported formatRevision in file");
}

int avm_strand_write_faces(int fileid,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
)
{
   if (triFaces_size > 0 && triFaces==NULL)    RETURN_ERROR("avm_strand_write_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL)   RETURN_ERROR("avm_strand_write_faces: quadFaces is NULL");
   if (polyFaces_size > 0 && polyFaces==NULL)   RETURN_ERROR("avm_strand_write_faces: polyFaces is NULL");
   if (faceTag_size > 0 && faceTag==NULL)     RETURN_ERROR("avm_strand_write_faces: faceTag is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_write_faces: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_write_faces(avf->rev1,
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         polyFaces,    polyFaces_size,
                         faceTag,     faceTag_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_write_faces(avf->rev2,
                         triFaces,     triFaces_size,
                         quadFaces,    quadFaces_size,
                         polyFaces,    polyFaces_size,
                         faceTag,     faceTag_size);
   }

   RETURN_ERROR("avm_strand_write_faces: unsupported formatRevision in file");
}

int avm_strand_write_edges(int fileid,
   int* bndEdges,       int bndEdges_size,
   int* edgeTag,        int edgeTag_size
)
{
   if (bndEdges_size > 0 && bndEdges==NULL)    RETURN_ERROR("avm_strand_write_edges: bndEdges is NULL");
   if (edgeTag_size > 0 && edgeTag==NULL)     RETURN_ERROR("avm_strand_write_edges: edgeTag is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_edges: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_write_edges: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_write_edges(avf->rev1,
                         bndEdges,    bndEdges_size,
                         edgeTag,     edgeTag_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_write_edges(avf->rev2,
                         bndEdges,    bndEdges_size,
                         edgeTag,     edgeTag_size);
   }

   RETURN_ERROR("avm_strand_write_edges: unsupported formatRevision in file");
}

int avm_strand_write_amr(int fileid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size)
{
   if (nodesOnGeometry_size > 0 && nodesOnGeometry==NULL) RETURN_ERROR("avm_strand_write_amr: nodesOnGeometry is NULL");
   if (edgesOnGeometry_size > 0 && edgesOnGeometry==NULL) RETURN_ERROR("avm_strand_write_amr: edgesOnGeometry is NULL");
   if (facesOnGeometry_size > 0 && facesOnGeometry==NULL) RETURN_ERROR("avm_strand_write_amr: facesOnGeometry is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_strand_write_amr: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_strand_write_amr: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_strand_write_amr(avf->rev1,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_strand_write_amr(avf->rev2,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }

   RETURN_ERROR("avm_strand_write_amr: unsupported formatRevision in file");
}

int avm_unstruc_read_nodes_r4(int fileid, float* xyz, int xyz_size)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_unstruc_read_nodes_r4: xyz is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_nodes_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_unstruc_read_nodes_r4(avf->rev0, xyz, xyz_size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_nodes_r4(avf->rev1, xyz, xyz_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_nodes_r4(avf->rev2, xyz, xyz_size);
   }

   RETURN_ERROR("avm_unstruc_read_nodes_r4: unsupported formatRevision in file");
}

int avm_unstruc_read_nodes_r8(int fileid, double* xyz, int xyz_size)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_unstruc_read_nodes_r8: xyz is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_nodes_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_unstruc_read_nodes_r8(avf->rev0, xyz, xyz_size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_nodes_r8(avf->rev1, xyz, xyz_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_nodes_r8(avf->rev2, xyz, xyz_size);
   }

   RETURN_ERROR("avm_unstruc_read_nodes_r8: unsupported formatRevision in file");
}

int avm_unstruc_read_partial_nodes_r4(int fileid, float* xyz, int xyz_size, int start, int end)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: xyz is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_partial_nodes_r4(avf->rev1, xyz, xyz_size, start, end);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_partial_nodes_r4(avf->rev2, xyz, xyz_size, start, end);
   }

   RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: unsupported formatRevision in file");
}

int avm_unstruc_read_partial_nodes_r8(int fileid, double* xyz, int xyz_size, int start, int end)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: xyz is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_partial_nodes_r8(avf->rev1, xyz, xyz_size, start, end);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_partial_nodes_r8(avf->rev2, xyz, xyz_size, start, end);
   }

   RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: unsupported formatRevision in file");
}

int avm_unstruc_read_faces(int fileid,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: quadFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_unstruc_read_faces(avf->rev0, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_faces(avf->rev1, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size,
                               NULL, 0);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_faces(avf->rev2, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size);
   }

   RETURN_ERROR("avm_unstruc_read_faces: unsupported formatRevision in file");
}

int avm_unstruc_read_tri(int fileid,
   int* triFaces, int triFaces_size,
   int stride, int start, int end, int flags
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_read_tri: triFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_tri: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_tri: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_tri(avf->rev1,
                               triFaces,  triFaces_size,
                               stride, start, end, flags);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_tri(avf->rev2,
                               triFaces,  triFaces_size,
                               stride, start, end, flags);
   }

   RETURN_ERROR("avm_unstruc_read_tri: unsupported formatRevision in file");
}

int avm_unstruc_read_quad(int fileid,
   int* quadFaces, int quadFaces_size,
   int stride, int start, int end, int flags
)
{
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_read_quad: quadFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_quad: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_quad: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_quad(avf->rev1,
                               quadFaces,  quadFaces_size,
                               stride, start, end, flags);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_quad(avf->rev2,
                               quadFaces,  quadFaces_size,
                               stride, start, end, flags);
   }

   RETURN_ERROR("avm_unstruc_read_quad: unsupported formatRevision in file");
}

int avm_unstruc_read_partial_faces(int fileid,
   int* triFaces,  int triFaces_size,  int triStart,  int triEnd,
   int* quadFaces, int quadFaces_size, int quadStart, int quadEnd
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_read_partial_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_read_partial_faces: quadFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_partial_faces: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_partial_faces(avf->rev1,
                               triFaces,  triFaces_size, triStart, triEnd,
                               quadFaces, quadFaces_size, quadStart, quadEnd,
                               NULL, 0, 0, 0);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_partial_faces(avf->rev2,
                               triFaces,  triFaces_size, triStart, triEnd,
                               quadFaces, quadFaces_size, quadStart, quadEnd);
   }

   RETURN_ERROR("avm_unstruc_read_partial_faces: unsupported formatRevision in file");
}

int avm_unstruc_read_bnd_faces(int fileid,
   int* bndTriFaces,  int bndTriFaces_size,
   int* bndQuadFaces, int bndQuadFaces_size
)
{
   if (bndTriFaces_size > 0 && bndTriFaces==NULL) RETURN_ERROR("avm_unstruc_read_bnd_faces: bndTriFaces is NULL");
   if (bndQuadFaces_size > 0 && bndQuadFaces==NULL) RETURN_ERROR("avm_unstruc_read_bnd_faces: bndQuadFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_bnd_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_bnd_faces: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_bnd_faces(avf->rev1, 
                                bndTriFaces,  bndTriFaces_size,
                                bndQuadFaces, bndQuadFaces_size,
                                NULL, 0);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_bnd_faces(avf->rev2, 
                                bndTriFaces,  bndTriFaces_size,
                                bndQuadFaces, bndQuadFaces_size);
   }

   RETURN_ERROR("avm_unstruc_read_bnd_faces: unsupported formatRevision in file");
}

int avm_unstruc_read_cells(int fileid,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size)
{
   if (hexCells_size > 0 && hexCells==NULL) RETURN_ERROR("avm_unstruc_read_cells: hexCells is NULL");
   if (tetCells_size > 0 && tetCells==NULL) RETURN_ERROR("avm_unstruc_read_cells: tetCells is NULL");
   if (priCells_size > 0 && priCells==NULL) RETURN_ERROR("avm_unstruc_read_cells: priCells is NULL");
   if (pyrCells_size > 0 && pyrCells==NULL) RETURN_ERROR("avm_unstruc_read_cells: pyrCells is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_cells: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_cells: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_cells(avf->rev1, 
                                hexCells, hexCells_size,
                                tetCells, tetCells_size,
                                priCells, priCells_size,
                                pyrCells, pyrCells_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_cells(avf->rev2, 
                                hexCells, hexCells_size,
                                tetCells, tetCells_size,
                                priCells, priCells_size,
                                pyrCells, pyrCells_size);
   }

   RETURN_ERROR("avm_unstruc_read_cells: unsupported formatRevision in file");
}

int avm_unstruc_read_partial_cells(int fileid,
   int* hexCells, int hexCells_size, int hexStart, int hexEnd,
   int* tetCells, int tetCells_size, int tetStart, int tetEnd,
   int* priCells, int priCells_size, int priStart, int priEnd,
   int* pyrCells, int pyrCells_size, int pyrStart, int pyrEnd)
{
   if (hexCells_size > 0 && hexCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: hexCells is NULL");
   if (tetCells_size > 0 && tetCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: tetCells is NULL");
   if (priCells_size > 0 && priCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: priCells is NULL");
   if (pyrCells_size > 0 && pyrCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: pyrCells is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_cells: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_partial_cells(avf->rev1, 
                                hexCells,  hexCells_size,  hexStart,  hexEnd,
                                tetCells,  tetCells_size,  tetStart,  tetEnd,
                                priCells,  priCells_size,  priStart,  priEnd,
                                pyrCells,  pyrCells_size,  pyrStart,  pyrEnd);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_partial_cells(avf->rev2, 
                                hexCells,  hexCells_size,  hexStart,  hexEnd,
                                tetCells,  tetCells_size,  tetStart,  tetEnd,
                                priCells,  priCells_size,  priStart,  priEnd,
                                pyrCells,  pyrCells_size,  pyrStart,  pyrEnd);
   }

   RETURN_ERROR("avm_unstruc_read_partial_cells: unsupported formatRevision in file");
}

int avm_unstruc_read_edges(int fileid, int* edges, int edges_size)
{
   if (edges_size > 0 && edges==NULL) RETURN_ERROR("avm_unstruc_read_edges: edges is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_edges: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_edges: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_edges(avf->rev1, edges, edges_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_edges(avf->rev2, edges, edges_size);
   }

   RETURN_ERROR("avm_unstruc_read_edges: unsupported formatRevision in file");
}

int avm_unstruc_read_amr(int fileid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size)
{
   if (nodesOnGeometry_size && nodesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_read_amr: nodesOnGeometry is NULL");
   if (edgesOnGeometry_size && edgesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_read_amr: edgesOnGeometry is NULL");
   if (facesOnGeometry_size && facesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_read_amr: facesOnGeometry is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_amr: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_amr: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_amr(avf->rev1,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_amr(avf->rev2,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }

   RETURN_ERROR("avm_unstruc_read_amr: unsupported formatRevision in file");
}

int avm_unstruc_read_amr_volumeids(int fileid,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size)
{
   if (hexGeomIds_size > 0 && hexGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: hexGeomIds is NULL");
   if (tetGeomIds_size > 0 && tetGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: tetGeomIds is NULL");
   if (priGeomIds_size > 0 && priGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: priGeomIds is NULL");
   if (pyrGeomIds_size > 0 && pyrGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: pyrGeomIds is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_read_amr_volumeids: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_read_amr_volumeids(avf->rev1,
                                     hexGeomIds, hexGeomIds_size,
                                     tetGeomIds, tetGeomIds_size,
                                     priGeomIds, priGeomIds_size,
                                     pyrGeomIds, pyrGeomIds_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_read_amr_volumeids(avf->rev2,
                                     hexGeomIds, hexGeomIds_size,
                                     tetGeomIds, tetGeomIds_size,
                                     priGeomIds, priGeomIds_size,
                                     pyrGeomIds, pyrGeomIds_size);
   }

   RETURN_ERROR("avm_unstruc_read_amr_volumeids: unsupported formatRevision in file");
}

int avm_unstruc_write_nodes_r4(int fileid, float* xyz, int xyz_size)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_unstruc_write_nodes_r4: xyz is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_nodes_r4: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_unstruc_write_nodes_r4(avf->rev0, xyz, xyz_size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_nodes_r4(avf->rev1, xyz, xyz_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_nodes_r4(avf->rev2, xyz, xyz_size);
   }

   RETURN_ERROR("avm_unstruc_write_nodes_r4: unsupported formatRevision in file");
}

int avm_unstruc_write_nodes_r8(int fileid, double* xyz, int xyz_size)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_unstruc_write_nodes_r8: xyz is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_nodes_r8: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_unstruc_write_nodes_r8(avf->rev0, xyz, xyz_size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_nodes_r8(avf->rev1, xyz, xyz_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_nodes_r8(avf->rev2, xyz, xyz_size);
   }

   RETURN_ERROR("avm_unstruc_write_nodes_r8: unsupported formatRevision in file");
}

int avm_unstruc_write_faces(int fileid,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_write_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_write_faces: quadFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_unstruc_write_faces(avf->rev0, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_faces(avf->rev1, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size,
                               NULL, 0);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_faces(avf->rev2, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size);
   }

   RETURN_ERROR("avm_unstruc_write_faces: unsupported formatRevision in file");
}

int avm_unstruc_write_bnd_faces(int fileid,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_write_bnd_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_write_bnd_faces: quadFaces is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_bnd_faces: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_write_bnd_faces: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_bnd_faces(avf->rev1, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size,
                               NULL, 0);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_bnd_faces(avf->rev2, 
                               triFaces,  triFaces_size,
                               quadFaces, quadFaces_size);
   }

   RETURN_ERROR("avm_unstruc_write_bnd_faces: unsupported formatRevision in file");
}

int avm_unstruc_write_cells_nosize(int fileid,
   int* hexCells,
   int* tetCells,
   int* priCells,
   int* pyrCells)
{
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_cells_nosize: fileid invalid");

   if (avf->formatRevision == 0 || avf->formatRevision == 1) {
      RETURN_ERROR("avm_unstruc_write_cells_nosize: This call is only supported in formatRevision 2");
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_cells_nosize(avf->rev2,
                                hexCells,
                                tetCells,
                                priCells,
                                pyrCells);
   }

   RETURN_ERROR("avm_unstruc_write_cells_nosize: unsupported formatRevision in file");
}

int avm_unstruc_write_cells(int fileid,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size)
{
   if (hexCells_size > 0 && hexCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: hexCells is NULL");
   if (tetCells_size > 0 && tetCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: tetCells is NULL");
   if (priCells_size > 0 && priCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: priCells is NULL");
   if (pyrCells_size > 0 && pyrCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: pyrCells is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_cells: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_write_cells: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_cells(avf->rev1, 
                                hexCells, hexCells_size,
                                tetCells, tetCells_size,
                                priCells, priCells_size,
                                pyrCells, pyrCells_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_cells(avf->rev2, 
                                hexCells, hexCells_size,
                                tetCells, tetCells_size,
                                priCells, priCells_size,
                                pyrCells, pyrCells_size);
   }

   RETURN_ERROR("avm_unstruc_write_cells: unsupported formatRevision in file");
}

int avm_unstruc_write_edges(int fileid, int* edges, int edges_size)
{
   if (edges_size > 0 && edges==NULL) RETURN_ERROR("avm_unstruc_write_edges: edges is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_edges: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_write_edges: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_edges(avf->rev1, edges, edges_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_edges(avf->rev2, edges, edges_size);
   }

   RETURN_ERROR("avm_unstruc_write_edges: unsupported formatRevision in file");
}

int avm_unstruc_write_amr(int fileid,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size)
{
   if (nodesOnGeometry_size && nodesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_write_amr: nodesOnGeometry is NULL");
   if (edgesOnGeometry_size && edgesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_write_amr: edgesOnGeometry is NULL");
   if (facesOnGeometry_size && facesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_write_amr: facesOnGeometry is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_amr: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_write_amr: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_amr(avf->rev1,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_amr(avf->rev2,
                      nodesOnGeometry,  nodesOnGeometry_size,
                      edgesOnGeometry,  edgesOnGeometry_size,
                      facesOnGeometry,  facesOnGeometry_size);
   }

   RETURN_ERROR("avm_unstruc_write_amr: unsupported formatRevision in file");
}

int avm_unstruc_write_amr_volumeids(int fileid,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size)
{
   if (hexGeomIds_size > 0 && hexGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: hexGeomIds is NULL");
   if (tetGeomIds_size > 0 && tetGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: tetGeomIds is NULL");
   if (priGeomIds_size > 0 && priGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: priGeomIds is NULL");
   if (pyrGeomIds_size > 0 && pyrGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: pyrGeomIds is NULL");

   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_unstruc_write_amr_volumeids: fileid invalid");

   if (avf->formatRevision == 0) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: This call is only supported in formatRevision 1+");
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_unstruc_write_amr_volumeids(avf->rev1,
                                     hexGeomIds, hexGeomIds_size,
                                     tetGeomIds, tetGeomIds_size,
                                     priGeomIds, priGeomIds_size,
                                     pyrGeomIds, pyrGeomIds_size);
   }
   else if (avf->formatRevision == 2) {
      return rev2::avm_unstruc_write_amr_volumeids(avf->rev2,
                                     hexGeomIds, hexGeomIds_size,
                                     tetGeomIds, tetGeomIds_size,
                                     priGeomIds, priGeomIds_size,
                                     pyrGeomIds, pyrGeomIds_size);
   }

   RETURN_ERROR("avm_unstruc_write_amr_volumeids: unsupported formatRevision in file");
}

int avm_cart_read_iblank(int fileid, int* iblanks, int size)
{
   if (iblanks==NULL) RETURN_ERROR("avm_cart_read_iblank: iblanks is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_cart_read_iblank: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_cart_read_iblank(avf->rev0, iblanks, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_cart_read_iblank(avf->rev1, iblanks, size);
   }

   RETURN_ERROR("avm_cart_read_iblank: unsupported formatRevision in file");
}

int avm_cart_write_iblank(int fileid, int* iblanks, int size)
{
   if (iblanks==NULL) RETURN_ERROR("avm_cart_write_iblank: iblanks is NULL");
   avmesh_file *avf = file_list[fileid];
   if (!avf) RETURN_ERROR("avm_cart_write_iblank: fileid invalid");

   if (avf->formatRevision == 0) {
      return rev0::avm_cart_write_iblank(avf->rev0, iblanks, size);
   }
   else if (avf->formatRevision == 1) {
      return rev1::avm_cart_write_iblank(avf->rev1, iblanks, size);
   }

   RETURN_ERROR("avm_cart_write_iblank: unsupported formatRevision in file");
}

// Fortran API

#ifndef FTNFUNC
#define FTNFUNC(name) name ## f_
#endif

static const char* cstr(const char* fstr, size_t flen) // convert a fortran string to a c string
{
   const size_t BUFF_LEN = 1028;
   static char str[BUFF_LEN], *s;
   if (flen>=BUFF_LEN) return 0;
   for (size_t i=0; i<flen; ++i) str[i]=fstr[i];
   for (size_t j=flen; j<BUFF_LEN; ++j) str[j]=0;
   for (s=str; isspace(*s); ++s);
   for (size_t len=strlen(s)-1; isspace(s[len]); --len) s[len]=0;
   return s;
}

static char* cstrm(const char* fstr, size_t flen) // convert a fortran string to a c string
                                               // - and let it be mutable
{
   const size_t BUFF_LEN = 1028;
   static char str[BUFF_LEN], *s;
   if (flen>=BUFF_LEN) return 0;
   for (size_t i=0; i<flen; ++i) str[i]=fstr[i];
   for (size_t j=flen; j<BUFF_LEN; ++j) str[j]=0;
   for (s=str; isspace(*s); ++s);
   for (size_t len=strlen(s)-1; isspace(s[len]); --len) s[len]=0;
   return s;
}

static void fstr(char* _cstr, size_t flen) // convert a c string to a fortran string
{
   size_t len=strlen(_cstr);
   for (unsigned int i=len; i<flen; ++i) _cstr[i]=' ';
}

void FTNFUNC(avm_open)(const char* _filename, int* id, int* status, size_t _filename_len)
{ const char* filename = cstr(_filename, _filename_len); 
  *status = avm_open(filename, id); }

void FTNFUNC(avm_read_mesh_headers)(int* id, int* status)
{ *status = avm_read_mesh_headers(*id); }

void FTNFUNC(avm_new_file)(int* id, const char* _filename, int* rev, int* status, size_t _filename_len)
{ const char* filename = cstr(_filename, _filename_len); 
  *status = avm_new_file(id, filename, *rev); }

void FTNFUNC(avm_write_headers)(int* id, int* status)
{ *status = avm_write_headers(*id); }

void FTNFUNC(avm_read_headers)(int* id, const char* _filename, int* status, size_t _filename_len)
{ const char* filename = cstr(_filename, _filename_len); 
  *status = avm_read_headers(id, filename); }

void FTNFUNC(avm_close)(int* id, int* status)
{ *status = avm_close(*id); }

void FTNFUNC(avm_select)(int* fileid, const char* _section, int* id, int* status, size_t _section_len)
{ const char* section = cstr(_section, _section_len); 
  *status = avm_select(*fileid, section, *id); }

//
// generic get/set functions 
//

void FTNFUNC(avm_get_int)(int* fileid, const char* _field, int* value, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_get_int(*fileid, field, value); }

void FTNFUNC(avm_get_real)(int* fileid, const char* _field, double* value, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_get_real(*fileid, field, value); }

void FTNFUNC(avm_get_str)(int* fileid, const char* _field, char* _str, int* status, size_t _field_len, size_t _str_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_get_str(*fileid, field, _str, _str_len); fstr(_str, _str_len); }

void FTNFUNC(avm_get_int_array)(int* fileid, const char* _field, int* values, int* len, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_get_int_array(*fileid, field, values, *len); }

void FTNFUNC(avm_get_real_array)(int* fileid, const char* _field, double* values, int* len, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_get_real_array(*fileid, field, values, *len); }

void FTNFUNC(avm_set_int)(int* fileid, const char* _field, int* value, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_set_int(*fileid, field, *value); }

void FTNFUNC(avm_set_real)(int* fileid, const char* _field, double* value, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_set_real(*fileid, field, *value); }

void FTNFUNC(avm_set_str)(int* fileid, const char* _field, char* _str, int* status, size_t _field_len, size_t _str_len)
{ const char* field = cstr(_field, _field_len); 
  char* str = cstrm(_str, _str_len);
  *status = avm_set_str(*fileid, field, str, strlen(str)); }

void FTNFUNC(avm_set_int_array)(int* fileid, const char* _field, int* values, int* len, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_set_int_array(*fileid, field, values, *len); }

void FTNFUNC(avm_set_real_array)(int* fileid, const char* _field, double* values, int* len, int* status, size_t _field_len)
{ const char* field = cstr(_field, _field_len); 
  *status = avm_set_real_array(*fileid, field, values, *len); }

void FTNFUNC(avm_get_error_str)(char* _str, size_t _str_len)
{ char* error = avm_get_error_str();
  int len = _str_len < AVM_STD_STRING_LENGTH ? _str_len : AVM_STD_STRING_LENGTH;
  strncpy(_str,error,len);
  fstr(_str, _str_len); }

void FTNFUNC(avm_mesh_header_offset)(int* fileid, int* mesh, off_t* offset, int* status)
{ *status = avm_mesh_header_offset(*fileid, *mesh, offset); }

void FTNFUNC(avm_mesh_data_offset)(int* fileid, int* mesh, off_t* offset, int* status)
{ *status = avm_mesh_data_offset(*fileid, *mesh, offset); }

void FTNFUNC(avm_seek_to_mesh)(int* fileid, int* mesh, int* status)
{ *status = avm_seek_to_mesh(*fileid, *mesh); }

void FTNFUNC(avm_nodes_per_tri)(int* fileid, int* p)
{ avm_nodes_per_tri(*fileid, *p); }

void FTNFUNC(avm_nodes_per_quad)(int* fileid, int* p)
{ avm_nodes_per_quad(*fileid, *p); }

void FTNFUNC(avm_nodes_per_tet)(int* fileid, int* p)
{ avm_nodes_per_tet(*fileid, *p); }

void FTNFUNC(avm_nodes_per_pyr)(int* fileid, int* p)
{ avm_nodes_per_pyr(*fileid, *p); }

void FTNFUNC(avm_nodes_per_pri)(int* fileid, int* p)
{ avm_nodes_per_pri(*fileid, *p); }

void FTNFUNC(avm_nodes_per_hex)(int* fileid, int* p)
{ avm_nodes_per_hex(*fileid, *p); }

//
// bfstruc specific functions 
//

void FTNFUNC(avm_bfstruc_read_xyz_r4)(int* fileid, float* x, float* y, float* z, int* size, int* status)
{ *status = avm_bfstruc_read_xyz_r4(*fileid, x,y,z, *size); }

void FTNFUNC(avm_bfstruc_read_xyz_r8)(int* fileid, double* x, double* y, double* z, int* size, int* status)
{ *status = avm_bfstruc_read_xyz_r8(*fileid,x,y,z, *size); }

void FTNFUNC(avm_bfstruc_read_iblank)(int* fileid, int* iblanks, int* size, int* status)
{ *status = avm_bfstruc_read_iblank(*fileid, iblanks, *size); }

void FTNFUNC(avm_bfstruc_write_xyz_r4)(int* fileid, float* x, float* y, float* z, int* size, int* status)
{ *status = avm_bfstruc_write_xyz_r4(*fileid, x,y,z, *size); }

void FTNFUNC(avm_bfstruc_write_xyz_r8)(int* fileid, double* x, double* y, double* z, int* size, int* status)
{ *status = avm_bfstruc_write_xyz_r8(*fileid,x,y,z, *size); }

void FTNFUNC(avm_bfstruc_write_iblank)(int* fileid, int* iblanks, int* size, int* status)
{ *status = avm_bfstruc_write_iblank(*fileid, iblanks, *size); }

//
// strand specific functions 
//

void FTNFUNC(avm_strand_read_r4)(int* fileid,
   int* triFaces,       int* triFaces_size,
   int* quadFaces,      int* quadFaces_size,
   int* bndEdges,      int* bndEdges_size,
   int* nodeClip,      int* nodeClip_size,
   int* faceClip,      int* faceClip_size,
   int* faceTag,       int* faceTag_size,
   int* edgeTag,       int* edgeTag_size,
   float* xyz,         int* xyz_size,
   float* pointingVec, int* pointingVec_size,
   float* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_read_r4(*fileid,
               triFaces,     *triFaces_size,
               quadFaces,    *quadFaces_size,
               bndEdges,    *bndEdges_size,
               nodeClip,    *nodeClip_size,
               faceClip,    *faceClip_size,
               faceTag,     *faceTag_size,
               edgeTag,     *edgeTag_size,
               xyz,         *xyz_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_read_r8)(int* fileid,
   int* triFaces,        int* triFaces_size,
   int* quadFaces,       int* quadFaces_size,
   int* bndEdges,       int* bndEdges_size,
   int* nodeClip,       int* nodeClip_size,
   int* faceClip,       int* faceClip_size,
   int* faceTag,        int* faceTag_size,
   int* edgeTag,        int* edgeTag_size,
   double* xyz,         int* xyz_size,
   double* pointingVec, int* pointingVec_size,
   double* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_read_r8(*fileid,
               triFaces,     *triFaces_size,
               quadFaces,    *quadFaces_size,
               bndEdges,    *bndEdges_size,
               nodeClip,    *nodeClip_size,
               faceClip,    *faceClip_size,
               faceTag,     *faceTag_size,
               edgeTag,     *edgeTag_size,
               xyz,         *xyz_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_write_r4)(int* fileid,
   int* triFaces,       int* triFaces_size,
   int* quadFaces,      int* quadFaces_size,
   int* bndEdges,      int* bndEdges_size,
   int* nodeClip,      int* nodeClip_size,
   int* faceClip,      int* faceClip_size,
   int* faceTag,       int* faceTag_size,
   int* edgeTag,       int* edgeTag_size,
   float* xyz,         int* xyz_size,
   float* pointingVec, int* pointingVec_size,
   float* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_write_r4(*fileid,
               triFaces,     *triFaces_size,
               quadFaces,    *quadFaces_size,
               bndEdges,    *bndEdges_size,
               nodeClip,    *nodeClip_size,
               faceClip,    *faceClip_size,
               faceTag,     *faceTag_size,
               edgeTag,     *edgeTag_size,
               xyz,         *xyz_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_write_r8)(int* fileid,
   int* triFaces,        int* triFaces_size,
   int* quadFaces,       int* quadFaces_size,
   int* bndEdges,       int* bndEdges_size,
   int* nodeClip,       int* nodeClip_size,
   int* faceClip,       int* faceClip_size,
   int* faceTag,        int* faceTag_size,
   int* edgeTag,        int* edgeTag_size,
   double* xyz,         int* xyz_size,
   double* pointingVec, int* pointingVec_size,
   double* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_write_r8(*fileid,
               triFaces,     *triFaces_size,
               quadFaces,    *quadFaces_size,
               bndEdges,    *bndEdges_size,
               nodeClip,    *nodeClip_size,
               faceClip,    *faceClip_size,
               faceTag,     *faceTag_size,
               edgeTag,     *edgeTag_size,
               xyz,         *xyz_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_read_nodes_r4)(int* fileid,
   float* xyz,         int* xyz_size,
   int* nodeID,        int* nodeID_size,
   int* nodeClip,      int* nodeClip_size,
   float* pointingVec, int* pointingVec_size,
   float* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_read_nodes_r4(*fileid,
               xyz,         *xyz_size,
               nodeID,      *nodeID_size,
               nodeClip,    *nodeClip_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_read_nodes_r8)(int* fileid,
   double* xyz,         int* xyz_size,
   int* nodeID,        int* nodeID_size,
   int* nodeClip,      int* nodeClip_size,
   double* pointingVec, int* pointingVec_size,
   double* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_read_nodes_r8(*fileid,
               xyz,         *xyz_size,
               nodeID,      *nodeID_size,
               nodeClip,    *nodeClip_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_read_faces)(int* fileid,
   int* triFaces,       int* triFaces_size,
   int* quadFaces,      int* quadFaces_size,
   int* polyFaces,      int* polyFaces_size,
   int* faceTag,       int* faceTag_size,
   int* status
) {
 *status = avm_strand_read_faces(*fileid,
               triFaces,     *triFaces_size,
               quadFaces,    *quadFaces_size,
               polyFaces,    *polyFaces_size,
               faceTag,     *faceTag_size);
}

void FTNFUNC(avm_strand_read_edges)(int* fileid,
   int* bndEdges,       int* bndEdges_size,
   int* edgeTag,        int* edgeTag_size,
   int* status
) {
 *status = avm_strand_read_edges(*fileid,
               bndEdges,    *bndEdges_size,
               edgeTag,     *edgeTag_size);
}

void FTNFUNC(avm_strand_read_amr)(int* fileid,
   AMR_Node_Data* nodesOnGeometry, int* nodesOnGeometry_size,
   int* edgesOnGeometry, int* edgesOnGeometry_size,
   int* facesOnGeometry, int* facesOnGeometry_size,
   int* status
) {
 *status = avm_strand_read_amr(*fileid,
               nodesOnGeometry, *nodesOnGeometry_size,
               edgesOnGeometry, *edgesOnGeometry_size,
               facesOnGeometry, *facesOnGeometry_size);
}

void FTNFUNC(avm_strand_write_nodes_r4)(int* fileid,
   float* xyz,         int* xyz_size,
   int* nodeID,        int* nodeID_size,
   int* nodeClip,      int* nodeClip_size,
   float* pointingVec, int* pointingVec_size,
   float* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_write_nodes_r4(*fileid,
               xyz,         *xyz_size,
               nodeID,      *nodeID_size,
               nodeClip,    *nodeClip_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_write_nodes_r8)(int* fileid,
   double* xyz,         int* xyz_size,
   int* nodeID,        int* nodeID_size,
   int* nodeClip,      int* nodeClip_size,
   double* pointingVec, int* pointingVec_size,
   double* xStrand,     int* xStrand_size,
   int* status
) {
 *status = avm_strand_write_nodes_r8(*fileid,
               xyz,         *xyz_size,
               nodeID,      *nodeID_size,
               nodeClip,    *nodeClip_size,
               pointingVec, *pointingVec_size,
               xStrand,     *xStrand_size);
}

void FTNFUNC(avm_strand_write_faces)(int* fileid,
   int* triFaces,       int* triFaces_size,
   int* quadFaces,      int* quadFaces_size,
   int* polyFaces,      int* polyFaces_size,
   int* faceTag,       int* faceTag_size,
   int* status
) {
 *status = avm_strand_write_faces(*fileid,
               triFaces,     *triFaces_size,
               quadFaces,    *quadFaces_size,
               polyFaces,    *polyFaces_size,
               faceTag,     *faceTag_size);
}

void FTNFUNC(avm_strand_write_edges)(int* fileid,
   int* bndEdges,       int* bndEdges_size,
   int* edgeTag,        int* edgeTag_size,
   int* status
) {
 *status = avm_strand_write_edges(*fileid,
               bndEdges,    *bndEdges_size,
               edgeTag,     *edgeTag_size);
}

void FTNFUNC(avm_strand_write_amr)(int* fileid,
   AMR_Node_Data* nodesOnGeometry, int* nodesOnGeometry_size,
   int* edgesOnGeometry, int* edgesOnGeometry_size,
   int* facesOnGeometry, int* facesOnGeometry_size,
   int* status
) {
 *status = avm_strand_write_amr(*fileid,
               nodesOnGeometry, *nodesOnGeometry_size,
               edgesOnGeometry, *edgesOnGeometry_size,
               facesOnGeometry, *facesOnGeometry_size);
}

//
// unstruc specific functions 
//

void FTNFUNC(avm_unstruc_read_nodes_r4)(int* fileid, float* xyz, int* xyz_size, int* status)
{ *status = avm_unstruc_read_nodes_r4(*fileid, xyz, *xyz_size); }

void FTNFUNC(avm_unstruc_read_nodes_r8)(int* fileid, double* xyz, int* xyz_size, int* status)
{ *status = avm_unstruc_read_nodes_r8(*fileid, xyz, *xyz_size); }

void FTNFUNC(avm_unstruc_read_partial_nodes_r4)(int* fileid, float* xyz, int* xyz_size, int* start, int* end, int* status)
{ *status = avm_unstruc_read_partial_nodes_r4(*fileid, xyz, *xyz_size, *start, *end); }

void FTNFUNC(avm_unstruc_read_partial_nodes_r8)(int* fileid, double* xyz, int* xyz_size, int* start, int* end, int* status)
{ *status = avm_unstruc_read_partial_nodes_r8(*fileid, xyz, *xyz_size, *start, *end); }

void FTNFUNC(avm_unstruc_read_faces)(int* fileid,
   int* triFaces,  int* triFaces_size,
   int* quadFaces, int* quadFaces_size,
   int* status
) {
 *status = avm_unstruc_read_faces(*fileid,
               triFaces,  *triFaces_size,
               quadFaces, *quadFaces_size);
}

void FTNFUNC(avm_unstruc_read_tri)(int* fileid,
   int* triFaces,  int* triFaces_size,
   int* stride, int* start, int* end, int* flags,
   int* status
) {
 *status = avm_unstruc_read_tri(*fileid,
               triFaces, *triFaces_size,
               *stride,  *start,  *end,  *flags);
}

void FTNFUNC(avm_unstruc_read_quad)(int* fileid,
   int* quadFaces,  int* quadFaces_size,
   int* stride, int* start, int* end, int* flags,
   int* status
) {
 *status = avm_unstruc_read_quad(*fileid,
               quadFaces, *quadFaces_size,
               *stride,  *start,  *end,  *flags);
}

void FTNFUNC(avm_unstruc_read_partial_faces)(int* fileid,
   int* triFaces,  int* triFaces_size, int* triStart, int* triEnd,
   int* quadFaces, int* quadFaces_size, int* quadStart, int* quadEnd,
   int* status
) {
 *status = avm_unstruc_read_partial_faces(*fileid,
               triFaces,  *triFaces_size, *triStart, *triEnd,
               quadFaces, *quadFaces_size, *quadStart, *quadEnd);
}

void FTNFUNC(avm_unstruc_read_bnd_faces)(int* fileid,
   int* bndTriFaces,  int* bndTriFaces_size,
   int* bndQuadFaces, int* bndQuadFaces_size,
   int* status
) {
 *status = avm_unstruc_read_bnd_faces(*fileid,
               bndTriFaces,  *bndTriFaces_size,
               bndQuadFaces, *bndQuadFaces_size);
}
void FTNFUNC(avm_unstruc_read_cells)(int* fileid,
   int* hexCells, int* hexCells_size,
   int* tetCells, int* tetCells_size,
   int* priCells, int* priCells_size,
   int* pyrCells, int* pyrCells_size,
   int* status
) {
 *status = avm_unstruc_read_cells(*fileid,
               hexCells, *hexCells_size,
               tetCells, *tetCells_size,
               priCells, *priCells_size,
               pyrCells, *pyrCells_size);
}
void FTNFUNC(avm_unstruc_read_partial_cells)(int* fileid,
   int* hexCells, int *hexCells_size, int *hexStart, int *hexEnd,
   int* tetCells, int *tetCells_size, int *tetStart, int *tetEnd,
   int* priCells, int *priCells_size, int *priStart, int *priEnd,
   int* pyrCells, int *pyrCells_size, int *pyrStart, int *pyrEnd,
   int* status
) {
 *status = avm_unstruc_read_partial_cells(*fileid,
               hexCells, *hexCells_size, *hexStart, *hexEnd,
               tetCells, *tetCells_size, *tetStart, *tetEnd,
               priCells, *priCells_size, *priStart, *priEnd,
               pyrCells, *pyrCells_size, *pyrStart, *pyrEnd);
}
void FTNFUNC(avm_unstruc_read_edges)(int* fileid,
   int* edges, int* edges_size,
   int* status
) {
 *status = avm_unstruc_read_edges(*fileid,
               edges, *edges_size);
}
void FTNFUNC(avm_unstruc_read_amr)(int* fileid,
   AMR_Node_Data* nodesOnGeometry, int* nodesOnGeometry_size,
   int* edgesOnGeometry, int* edgesOnGeometry_size,
   int* facesOnGeometry, int* facesOnGeometry_size,
   int* status
) {
 *status = avm_unstruc_read_amr(*fileid,
               nodesOnGeometry, *nodesOnGeometry_size,
               edgesOnGeometry, *edgesOnGeometry_size,
               facesOnGeometry, *facesOnGeometry_size);
}
void FTNFUNC(avm_unstruc_read_amr_volumeids)(int* fileid,
   int* hexGeomIds, int* hexGeomIds_size,
   int* tetGeomIds, int* tetGeomIds_size,
   int* priGeomIds, int* priGeomIds_size,
   int* pyrGeomIds, int* pyrGeomIds_size,
   int* status
) {
 *status = avm_unstruc_read_amr_volumeids(*fileid,
               hexGeomIds, *hexGeomIds_size,
               tetGeomIds, *tetGeomIds_size,
               priGeomIds, *priGeomIds_size,
               pyrGeomIds, *pyrGeomIds_size);
}

void FTNFUNC(avm_unstruc_write_nodes_r4)(int* fileid, float* xyz, int* xyz_size, int* status)
{ *status = avm_unstruc_write_nodes_r4(*fileid, xyz, *xyz_size); }

void FTNFUNC(avm_unstruc_write_nodes_r8)(int* fileid, double* xyz, int* xyz_size, int* status)
{ *status = avm_unstruc_write_nodes_r8(*fileid, xyz, *xyz_size); }

void FTNFUNC(avm_unstruc_write_faces)(int* fileid,
   int* triFaces,  int* triFaces_size,
   int* quadFaces, int* quadFaces_size,
   int* status
) {
 *status = avm_unstruc_write_faces(*fileid,
               triFaces,  *triFaces_size,
               quadFaces, *quadFaces_size);
}

void FTNFUNC(avm_unstruc_write_bnd_faces)(int* fileid,
   int* triFaces,  int* triFaces_size,
   int* quadFaces, int* quadFaces_size,
   int* status
) {
 *status = avm_unstruc_write_bnd_faces(*fileid,
               triFaces,  *triFaces_size,
               quadFaces, *quadFaces_size);
}

void FTNFUNC(avm_unstruc_write_cells_nosize)(int* fileid,
   int* hexCells,
   int* tetCells,
   int* priCells,
   int* pyrCells,
   int* status
) {
 *status = avm_unstruc_write_cells_nosize(*fileid,
               hexCells,
               tetCells,
               priCells,
               pyrCells);
}

void FTNFUNC(avm_unstruc_write_cells)(int* fileid,
   int* hexCells, int* hexCells_size,
   int* tetCells, int* tetCells_size,
   int* priCells, int* priCells_size,
   int* pyrCells, int* pyrCells_size,
   int* status
) {
 *status = avm_unstruc_write_cells(*fileid,
               hexCells, *hexCells_size,
               tetCells, *tetCells_size,
               priCells, *priCells_size,
               pyrCells, *pyrCells_size);
}
void FTNFUNC(avm_unstruc_write_edges)(int* fileid,
   int* edges, int* edges_size,
   int* status
) {
 *status = avm_unstruc_write_edges(*fileid,
               edges, *edges_size);
}
void FTNFUNC(avm_unstruc_write_amr)(int* fileid,
   AMR_Node_Data* nodesOnGeometry, int* nodesOnGeometry_size,
   int* edgesOnGeometry, int* edgesOnGeometry_size,
   int* facesOnGeometry, int* facesOnGeometry_size,
   int* status
) {
 *status = avm_unstruc_write_amr(*fileid,
               nodesOnGeometry, *nodesOnGeometry_size,
               edgesOnGeometry, *edgesOnGeometry_size,
               facesOnGeometry, *facesOnGeometry_size);
}
void FTNFUNC(avm_unstruc_write_amr_volumeids)(int* fileid,
   int* hexGeomIds, int* hexGeomIds_size,
   int* tetGeomIds, int* tetGeomIds_size,
   int* priGeomIds, int* priGeomIds_size,
   int* pyrGeomIds, int* pyrGeomIds_size,
   int* status
) {
 *status = avm_unstruc_write_amr_volumeids(*fileid,
               hexGeomIds, *hexGeomIds_size,
               tetGeomIds, *tetGeomIds_size,
               priGeomIds, *priGeomIds_size,
               pyrGeomIds, *pyrGeomIds_size);
}

//
// cart specific functions 
//

void FTNFUNC(avm_cart_read_iblank)(int* fileid, int* iblanks, int* size, int* status) 
{ *status = avm_cart_read_iblank(*fileid, iblanks, *size); }

void FTNFUNC(avm_cart_write_iblank)(int* fileid, int* iblanks, int* size, int* status) 
{ *status = avm_cart_write_iblank(*fileid, iblanks, *size); }

} // extern "C"
