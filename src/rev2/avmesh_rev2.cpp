#include <map>
#include <string>
#include <vector> 
using namespace std;

#include <stdio.h>
#include <string.h>

#include "avmhdrs.h"
#include "../avmesh.h"
#include "../avmesh_file.h"
#include "avmesh_rev2.h"

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);
extern "C" void avm_set_error_str(char*);

#define RETURN_ERROR(str) { avm_set_error_str(str); return 1; }

using namespace rev2;

int rev2::avm_open(rev2_avmesh_file* avf, int* id)
{
#define CHECK_RETURN(v) if(v) { return 1; }
   if (id==NULL) RETURN_ERROR("avm_open: id is NULL");

   if (!avf) RETURN_ERROR("avm_open: avf is NULL");

   avf->selection_meshid = 0;
   avf->selection_patchid = 1;
   avf->selection_surfpatchid = 1;
   avf->selection_edgepatchid = 1;
   avf->write_endian = rev2_avmesh_file::ENDIAN_NATIVE;
   if (NULL==avf->fp) RETURN_ERROR("avm_open: file pointer is NULL");

   // read the first part of the header
   if(!rev2::file_header_t::read(avf->fp, avf->byte_swap, &avf->file_hdr)) {
      RETURN_ERROR("avm_open: failed reading file_header");
   }

   // read the variable length description record
   if (avf->file_hdr.descriptionSize>0) {
      char* description = new char[avf->file_hdr.descriptionSize];
      if (!description) {
         RETURN_ERROR("avm_open: failed to allocate file description");
      }
      if (!fread(description, sizeof(char)*avf->file_hdr.descriptionSize, 1, avf->fp)) {
         RETURN_ERROR("avm_open: failed reading file description");
      }
      avf->description = string(description, description+avf->file_hdr.descriptionSize);
      delete []description;
   }

   return 0;
#undef CHECK_RETURN
}

int rev2::avm_read_mesh_headers(rev2_avmesh_file* avf)
{
   if (!avf) RETURN_ERROR("avm_read_mesh_headers: avf is NULL");

   avf->mesh_hdrs.resize( avf->file_hdr.meshCount );
   avf->strand.resize( avf->file_hdr.meshCount ); // TODO: only allocate if needed
   avf->unstruc.resize( avf->file_hdr.meshCount ); // TODO: only allocate if needed

   for (int i=0; i<avf->file_hdr.meshCount; ++i)
   {
      // read the generic mesh information

      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      if(!mesh_header_t::read(avf->fp, avf->byte_swap, &mesh_hdr)) {
         RETURN_ERROR("avm_read_mesh_headers: failed reading mesh header");
      }

      // read the specific mesh information
      if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         if(!strand_info::read(avf->fp, avf->byte_swap, &avf->strand[i])) {
            RETURN_ERROR("avm_read_mesh_headers: failed reading strand header");
         }
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         if(!unstruc_info::read(avf->fp, avf->byte_swap, &avf->unstruc[i])) {
            RETURN_ERROR("avm_read_mesh_headers: failed reading unstruc header");
         }
      } else RETURN_ERROR("avm_read_mesh_headers: unsupported meshType");
   }

   return 0;
}

int rev2::avm_new_file(rev2_avmesh_file* avf, const char* filename)
{
   if (filename==NULL) RETURN_ERROR("avm_new_file: filename is NULL");

   if (!avf) RETURN_ERROR("avm_new_file: avf is NULL");

   avf->selection_meshid = 0;
   avf->selection_patchid = 1;
   avf->selection_surfpatchid = 1;
   avf->selection_edgepatchid = 1;
   avf->write_endian = rev2_avmesh_file::ENDIAN_NATIVE;
   avf->filename = filename;
   avf->fp = fopen(filename, "wb");
   if (avf->fp==NULL) RETURN_ERROR("avm_new_file: fopen failed");
   return 0;
}

int rev2::avm_write_headers(rev2_avmesh_file* avf)
{
   if (!avf) RETURN_ERROR("avm_write_headers: avf is NULL");

   if (avf->write_endian == rev2_avmesh_file::ENDIAN_NATIVE) avf->byte_swap = false;
   else {
      int one = 1;
      const char* p1 = (char*)&one;
      rev2_avmesh_file::Endian machine_endian = p1[0] ? rev2_avmesh_file::ENDIAN_LITTLE :
                                                   rev2_avmesh_file::ENDIAN_BIG;
      avf->byte_swap = avf->write_endian != machine_endian;
   }

   if (!file_id_prefix::write(avf->fp, avf->byte_swap, &avf->file_prefix)) {
      RETURN_ERROR("avm_write_headers: failed writing file_id_prefix");
   }
   if (!file_header::write(avf->fp, avf->byte_swap, &avf->file_hdr)) {
      RETURN_ERROR("avm_write_headers: failed writing file_header");
   }
   if (avf->file_hdr.descriptionSize>0) {
      if (!fwrite(avf->description.c_str(), avf->description.size(), 1, avf->fp)) {
         RETURN_ERROR("avm_write_headers: failed writing file description");
      }
   }

   for (int i=0; i<avf->file_hdr.meshCount; ++i)
   {
      // write the generic mesh information

      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      if (!mesh_header_t::write(avf->fp, avf->byte_swap, &mesh_hdr)) {
         RETURN_ERROR("avm_write_headers: failed writing mesh header");
      }

      // write the specific mesh information

      if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         if (!strand_info::write(avf->fp, avf->byte_swap, &avf->strand[i])) {
            RETURN_ERROR("avm_write_headers: failed writing strand header");
         }
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         if (!unstruc_info::write(avf->fp, avf->byte_swap, &avf->unstruc[i])) {
            RETURN_ERROR("avm_write_headers: failed writing unstruc header");
         }
      } else RETURN_ERROR("avm_write_headers: unsupported meshType");
   }

   return 0;
}

int rev2::avm_select(rev2_avmesh_file* avf, const char* section, int id)
{
   if (section==NULL) RETURN_ERROR("avm_select: section is NULL");
   if (!avf) RETURN_ERROR("avm_select: avf is NULL");
        if (0==strcmp("header",section)) avf->selection_meshid = 0;
   else if (0==strcmp("mesh",section)) avf->selection_meshid = id;
   else if (0==strcmp("patch",section)) avf->selection_patchid = id;
   else if (0==strcmp("surfPatch",section)) avf->selection_surfpatchid = id;
   else if (0==strcmp("edgePatch",section)) avf->selection_edgepatchid = id;
   return 0;
}

int rev2::avm_get_int(rev2_avmesh_file* avf, const char* field, int* value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANSWER(a) {*value=a; return 0;}
   if (field==NULL) RETURN_ERROR("avm_get_int: field is NULL");
   if (value==NULL) RETURN_ERROR("avm_get_int: value is NULL");
   if (!avf) RETURN_ERROR("avm_get_int: avf is NULL");

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("formatRevision")) RETURN_ANSWER(avf->file_prefix.formatRevision)
      if (FIELD_EQ("meshCount")) RETURN_ANSWER(avf->file_hdr.meshCount)
      if (FIELD_EQ("precision")) RETURN_ANSWER(avf->file_hdr.precision)
      if (FIELD_EQ("dimensions")) RETURN_ANSWER(avf->file_hdr.dimensions)
      if (FIELD_EQ("descriptionSize")) RETURN_ANSWER(avf->description.size()+1) // (account for null-terminator)
      RETURN_ERROR("avm_get_int: invalid field");
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   //all remaining fields require a valid meshid
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
      RETURN_ERROR("avm_get_int: invalid meshId");
   }

   bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
   bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

   if (strand_mesh) {
      if (FIELD_EQ("surfaceOnly")) RETURN_ANSWER(avf->strand[meshid].header.surfaceOnly)
      if (FIELD_EQ("nNodes")) RETURN_ANSWER(avf->strand[meshid].header.nNodes)
      if (FIELD_EQ("nStrands")) RETURN_ANSWER(avf->strand[meshid].header.nStrands)
      if (FIELD_EQ("nTriFaces")) RETURN_ANSWER(avf->strand[meshid].header.nTriFaces)
      if (FIELD_EQ("nQuadFaces")) RETURN_ANSWER(avf->strand[meshid].header.nQuadFaces)
      if (FIELD_EQ("nPolyFaces")) RETURN_ANSWER(avf->strand[meshid].header.nPolyFaces)
      if (FIELD_EQ("polyFacesSize")) RETURN_ANSWER(avf->strand[meshid].header.polyFacesSize)
      if (FIELD_EQ("nBndEdges")) RETURN_ANSWER(avf->strand[meshid].header.nBndEdges)
      if (FIELD_EQ("nPtsPerStrand")) RETURN_ANSWER(avf->strand[meshid].header.nPtsPerStrand)
      if (FIELD_EQ("nSurfPatches")) RETURN_ANSWER(avf->strand[meshid].header.nSurfPatches)
      if (FIELD_EQ("nEdgePatches")) RETURN_ANSWER(avf->strand[meshid].header.nEdgePatches)
      if (FIELD_EQ("nNodesOnGeometry")) RETURN_ANSWER(avf->strand[meshid].header.nNodesOnGeometry)
      if (FIELD_EQ("nEdgesOnGeometry")) RETURN_ANSWER(avf->strand[meshid].header.nEdgesOnGeometry)
      if (FIELD_EQ("nFacesOnGeometry")) RETURN_ANSWER(avf->strand[meshid].header.nFacesOnGeometry)
      if (FIELD_EQ("surfPatchId")) {
         //this field requires a valid surfpatch
         if (surfpatchi < 0 || surfpatchi >= avf->strand[meshid].header.nSurfPatches) {
            RETURN_ERROR("avm_get_int: invalid surfpatch");
         }
         RETURN_ANSWER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchId)
      }
      if (FIELD_EQ("edgePatchId")) {
         //this field requires a valid edgepatch
         if (edgepatchi < 0 || edgepatchi >= avf->strand[meshid].header.nEdgePatches) {
            RETURN_ERROR("avm_get_int: invalid edgepatch");
         }
         RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchId)
      }
      RETURN_ERROR("avm_get_int: invalid field");
   }

   if (unstruc_mesh) {
      if (FIELD_EQ("nNodes")) RETURN_ANSWER(avf->unstruc[meshid].header.nNodes)
      if (FIELD_EQ("nFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nFaces)
      if (FIELD_EQ("nCells")) RETURN_ANSWER(avf->unstruc[meshid].header.nCells)
      if (FIELD_EQ("nMaxNodesPerFace")) RETURN_ANSWER(avf->unstruc[meshid].header.nMaxNodesPerFace)
      if (FIELD_EQ("nMaxNodesPerCell")) RETURN_ANSWER(avf->unstruc[meshid].header.nMaxNodesPerCell)
      if (FIELD_EQ("nMaxFacesPerCell")) RETURN_ANSWER(avf->unstruc[meshid].header.nMaxFacesPerCell)
      if (FIELD_EQ("nPatches")) RETURN_ANSWER(avf->unstruc[meshid].header.nPatches)
      if (FIELD_EQ("nHexCells")) RETURN_ANSWER(avf->unstruc[meshid].header.nHexCells)
      if (FIELD_EQ("nTetCells")) RETURN_ANSWER(avf->unstruc[meshid].header.nTetCells)
      if (FIELD_EQ("nPriCells")) RETURN_ANSWER(avf->unstruc[meshid].header.nPriCells)
      if (FIELD_EQ("nPyrCells")) RETURN_ANSWER(avf->unstruc[meshid].header.nPyrCells)
      if (FIELD_EQ("nPolyCells")) RETURN_ANSWER(avf->unstruc[meshid].header.nPolyCells)
      if (FIELD_EQ("nBndTriFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nBndTriFaces)
      if (FIELD_EQ("nTriFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nTriFaces)
      if (FIELD_EQ("nBndQuadFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nBndQuadFaces)
      if (FIELD_EQ("nQuadFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nQuadFaces)
      if (FIELD_EQ("nBndPolyFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nBndPolyFaces)
      if (FIELD_EQ("nPolyFaces")) RETURN_ANSWER(avf->unstruc[meshid].header.nPolyFaces)
      if (FIELD_EQ("bndPolyFacesSize")) RETURN_ANSWER(avf->unstruc[meshid].header.bndPolyFacesSize)
      if (FIELD_EQ("polyFacesSize")) RETURN_ANSWER(avf->unstruc[meshid].header.polyFacesSize)
      if (FIELD_EQ("nEdges")) RETURN_ANSWER(avf->unstruc[meshid].header.nEdges)
      if (FIELD_EQ("nNodesOnGeometry")) RETURN_ANSWER(avf->unstruc[meshid].header.nNodesOnGeometry)
      if (FIELD_EQ("nEdgesOnGeometry")) RETURN_ANSWER(avf->unstruc[meshid].header.nEdgesOnGeometry)
      if (FIELD_EQ("nFacesOnGeometry")) RETURN_ANSWER(avf->unstruc[meshid].header.nFacesOnGeometry)
      if (FIELD_EQ("geomRegionId")) RETURN_ANSWER(avf->unstruc[meshid].header.geomRegionId)

      //all remaining unstruc fields require a valid patchid
      if (patchid < 0 || patchid >= avf->unstruc[meshid].header.nPatches) {
            RETURN_ERROR("avm_get_int: invalid patchid");
      }
      if (FIELD_EQ("patchId")) RETURN_ANSWER(avf->unstruc[meshid].patches[patchid].patchId)
      RETURN_ERROR("avm_get_int: invalid field");
   }

   RETURN_ERROR("avm_get_int: invalid meshType");
#undef FIELD_EQ
#undef RETURN_ANSWER
}

int rev2::avm_get_real(rev2_avmesh_file* avf, const char* field, double* value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANSWER(a) {*value=a; return 0;}
   if (field==NULL) RETURN_ERROR("avm_get_real: field is NULL");
   if (value==NULL) RETURN_ERROR("avm_get_real: value is NULL");
   if (!avf) RETURN_ERROR("avm_get_real: avf is NULL");

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   //all remaining fields require a valid meshid
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
      RETURN_ERROR("avm_get_real: invalid meshId");
   }

   if (FIELD_EQ("modelScale")) RETURN_ANSWER(avf->mesh_hdrs[meshid].modelScale)
   if (FIELD_EQ("referenceArea")) RETURN_ANSWER(avf->mesh_hdrs[meshid].referenceArea)

   bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
   bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

   if (strand_mesh) {
      if (FIELD_EQ("strandLength")) RETURN_ANSWER(avf->strand[meshid].header.strandLength)
      if (FIELD_EQ("stretchRatio")) RETURN_ANSWER(avf->strand[meshid].header.stretchRatio)
      if (FIELD_EQ("smoothingThreshold")) RETURN_ANSWER(avf->strand[meshid].header.smoothingThreshold)

      //all remaining strand fields require a valid edgepatch
      if (edgepatchi < 0 || edgepatchi >= avf->strand[meshid].header.nEdgePatches) {
         RETURN_ERROR("avm_get_real: invalid edgepatch");
      }
      if (FIELD_EQ("nx")) RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].nx)
      if (FIELD_EQ("ny")) RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].ny)
      if (FIELD_EQ("nz")) RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].nz)
      RETURN_ERROR("avm_get_real: invalid field");
   }

   if (unstruc_mesh) {
      RETURN_ERROR("avm_get_real: invalid field");
   }

   RETURN_ERROR("avm_get_real: invalid meshType");
#undef FIELD_EQ
#undef RETURN_ANSWER
}

int rev2::avm_get_str(rev2_avmesh_file* avf, const char* field, char* str, int len)
{
#define JMIN(len0,len1) len0 = (unsigned) len0 < (unsigned) len1 ? len0 : len1
// hack alert: This "JMIN" macro is only used in this routine (it is undefined at the end)
// It's purpose is to capture the length of the string field in order to trim whitespace
// and always null-terminate the outgoing string.
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANSWER(a,l) {strncpy(str,a,l); for (int i=len-1; i>=0 && (isspace(str[i]) || str[i]==0); --i) str[i]=0; str[len-1]=0; return 0;}

   if (field==NULL) RETURN_ERROR("avm_get_str: field is NULL");
   if (str==NULL) RETURN_ERROR("avm_get_str: str is NULL");
   if (len<=0) RETURN_ERROR("avm_get_str: len <= 0");
   if (!avf) RETURN_ERROR("avm_get_str: avf is NULL");

   if (FIELD_EQ("AVM_WRITE_ENDIAN")) {
      const char* endian = "native";
      if (avf->write_endian==rev2_avmesh_file::ENDIAN_LITTLE) endian = "little";
      if (avf->write_endian==rev2_avmesh_file::ENDIAN_BIG) endian = "big";
      RETURN_ANSWER(endian, JMIN(len,strlen(endian)+1))
   }

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("contactInfo")) RETURN_ANSWER(avf->file_hdr.contactInfo, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("description")) RETURN_ANSWER(avf->description.c_str(), JMIN(len,int(avf->description.size()+1)))
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("meshName")) RETURN_ANSWER(avf->mesh_hdrs[meshid].meshName, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("meshType")) RETURN_ANSWER(avf->mesh_hdrs[meshid].meshType, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("meshGenerator")) RETURN_ANSWER(avf->mesh_hdrs[meshid].meshGenerator, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("coordinateSystem")) RETURN_ANSWER(avf->mesh_hdrs[meshid].coordinateSystem, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("gridUnits")) RETURN_ANSWER(avf->mesh_hdrs[meshid].gridUnits, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("referencePointDescription")) RETURN_ANSWER(avf->mesh_hdrs[meshid].referencePointDescription, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("meshDescription")) RETURN_ANSWER(avf->mesh_hdrs[meshid].meshDescription, JMIN(len,AVM_STD_STRING_LENGTH))

      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

      if (strand_mesh) {
         if (FIELD_EQ("strandDistribution")) RETURN_ANSWER(avf->strand[meshid].header.strandDistribution, JMIN(len,32))
         //all remaining strand fields require a valid surfpatch
         if (surfpatchi < 0 || surfpatchi >= avf->strand[meshid].header.nSurfPatches) {
            RETURN_ERROR("avm_get_str: invalid surfpatch");
         }
         if (FIELD_EQ("surfPatchBody")) RETURN_ANSWER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBody, JMIN(len,32))
         if (FIELD_EQ("surfPatchComp")) RETURN_ANSWER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchComp, JMIN(len,32))
         if (FIELD_EQ("surfPatchBCType")) RETURN_ANSWER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBCType, JMIN(len,32))

         //all remaining strand fields require a valid surfpatch and edgepatch
         if (edgepatchi < 0 || edgepatchi >= avf->strand[meshid].header.nEdgePatches) {
            RETURN_ERROR("avm_get_str: invalid edgepatch");
         }
         if (FIELD_EQ("edgePatchBody")) RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBody, JMIN(len,32))
         if (FIELD_EQ("edgePatchComp")) RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchComp, JMIN(len,32))
         if (FIELD_EQ("edgePatchBCType")) RETURN_ANSWER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBCType, JMIN(len,32))
         RETURN_ERROR("avm_get_str: invalid field");
      }
      
      if (unstruc_mesh) {
         //all remaining unstruc fields require a valid patchid
         if (patchid < 0 || patchid >= avf->unstruc[meshid].header.nPatches) {
            RETURN_ERROR("avm_get_str: invalid patchid");
         }
            if (FIELD_EQ("patchLabel")) RETURN_ANSWER(avf->unstruc[meshid].patches[patchid].patchLabel, JMIN(len,32))
            if (FIELD_EQ("patchType")) RETURN_ANSWER(avf->unstruc[meshid].patches[patchid].patchType, JMIN(len,16))
         RETURN_ERROR("avm_get_str: invalid field");
      }
   }
   RETURN_ERROR("avm_get_str: invalid meshType");  
#undef FIELD_EQ
#undef RETURN_ANSWER
#undef JMIN
}

int rev2::avm_get_int_array(rev2_avmesh_file* avf, const char* field, int* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANSWER(a,l) {for(int i=0;i<l;++i) values[i]=a[i]; return 0;}
   if (field==NULL) RETURN_ERROR("avm_get_int_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_get_int_array: values is NULL");
   if (!avf) RETURN_ERROR("avm_get_int_array: avf is NULL");

   int meshid = avf->selection_meshid-1; 

   bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
   bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

   if (strand_mesh) {
      RETURN_ERROR("avm_get_int_array: invalid field");
   }

   if (unstruc_mesh) {
      RETURN_ERROR("avm_get_int_array: invalid field");
   }
   RETURN_ERROR("avm_get_int_array: invalid meshType");
#undef FIELD_EQ
#undef RETURN_ANSWER
}

int rev2::avm_get_real_array(rev2_avmesh_file* avf, const char* field, double* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANSWER(a,l) {for(int i=0;i<l;++i) values[i]=a[i]; return 0;}
   if (field==NULL) RETURN_ERROR("avm_get_real_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_get_real_array: values is NULL");
   if (!avf) RETURN_ERROR("avm_get_real_array: avf is NULL");

   int meshid = avf->selection_meshid-1; 

   //all remaining fields require a valid meshid
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
         RETURN_ERROR("avm_get_real_array: invalid meshid");
   }

   if (FIELD_EQ("referencePoint")) RETURN_ANSWER(avf->mesh_hdrs[meshid].referencePoint, min(len,3))
   if (FIELD_EQ("referenceLength")) RETURN_ANSWER(avf->mesh_hdrs[meshid].referenceLength, min(len,3))

   RETURN_ERROR("avm_get_real_array: invalid field");
#undef FIELD_EQ
#undef RETURN_ANSWER
}

int rev2::avm_set_int(rev2_avmesh_file* avf, const char* field, int value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a) {a=value; return 0;}
   if (field==NULL) RETURN_ERROR("avm_set_int: field is NULL");
   if (!avf) RETURN_ERROR("avm_set_int: avf is NULL");

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("meshCount")) {
         avf->mesh_hdrs.resize( value );
         avf->strand.resize( value ); // TODO: only allocate if needed
         avf->unstruc.resize( value ); // TODO: only allocate if needed
         SET_AND_RETURN(avf->file_hdr.meshCount)
      }
      if (FIELD_EQ("precision")) SET_AND_RETURN(avf->file_hdr.precision)
      if (FIELD_EQ("dimensions")) SET_AND_RETURN(avf->file_hdr.dimensions)
      RETURN_ERROR("avm_set_int: invalid field");
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
      RETURN_ERROR("avm_set_int: invalid meshid");
   }

   bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
   bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

   if (strand_mesh) {
      if (FIELD_EQ("surfaceOnly")) SET_AND_RETURN(avf->strand[meshid].header.surfaceOnly)
      if (FIELD_EQ("nNodes")) SET_AND_RETURN(avf->strand[meshid].header.nNodes)
      if (FIELD_EQ("nStrands")) SET_AND_RETURN(avf->strand[meshid].header.nStrands)
      if (FIELD_EQ("nTriFaces")) SET_AND_RETURN(avf->strand[meshid].header.nTriFaces)
      if (FIELD_EQ("nQuadFaces")) SET_AND_RETURN(avf->strand[meshid].header.nQuadFaces)
      if (FIELD_EQ("nPolyFaces")) SET_AND_RETURN(avf->strand[meshid].header.nPolyFaces)
      if (FIELD_EQ("polyFacesSize")) SET_AND_RETURN(avf->strand[meshid].header.polyFacesSize)
      if (FIELD_EQ("nBndEdges")) SET_AND_RETURN(avf->strand[meshid].header.nBndEdges)
      if (FIELD_EQ("nPtsPerStrand")) SET_AND_RETURN(avf->strand[meshid].header.nPtsPerStrand)
      if (FIELD_EQ("nSurfPatches")) {
         avf->strand[meshid].surf_patches.resize(value);
         SET_AND_RETURN(avf->strand[meshid].header.nSurfPatches)
      }
      if (FIELD_EQ("nEdgePatches")) {
         avf->strand[meshid].edge_patches.resize(value);
         SET_AND_RETURN(avf->strand[meshid].header.nEdgePatches)
      }
      if (FIELD_EQ("surfPatchId")) {
         //this field requires a valid surfpatch
         if (surfpatchi < 0 || surfpatchi >= avf->strand[meshid].header.nSurfPatches) {
            RETURN_ERROR("avm_set_int: invalid surfpatch");
         }
         SET_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchId)
      }
      if (FIELD_EQ("edgePatchId")) {
         //this field requires a valid edgepatch
         if (edgepatchi < 0 || edgepatchi >= avf->strand[meshid].header.nEdgePatches) {
            RETURN_ERROR("avm_set_int: invalid edgepatch");
         }
         SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchId)
      }
      if (FIELD_EQ("nNodesOnGeometry")) SET_AND_RETURN(avf->strand[meshid].header.nNodesOnGeometry)
      if (FIELD_EQ("nEdgesOnGeometry")) SET_AND_RETURN(avf->strand[meshid].header.nEdgesOnGeometry)
      if (FIELD_EQ("nFacesOnGeometry")) SET_AND_RETURN(avf->strand[meshid].header.nFacesOnGeometry)
      RETURN_ERROR("avm_set_int: invalid field");
   }

   if (unstruc_mesh) {
      if (FIELD_EQ("nNodes")) SET_AND_RETURN(avf->unstruc[meshid].header.nNodes)
      if (FIELD_EQ("nFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nFaces)
      if (FIELD_EQ("nCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nCells)
      if (FIELD_EQ("nMaxNodesPerFace")) SET_AND_RETURN(avf->unstruc[meshid].header.nMaxNodesPerFace)
      if (FIELD_EQ("nMaxNodesPerCell")) SET_AND_RETURN(avf->unstruc[meshid].header.nMaxNodesPerCell)
      if (FIELD_EQ("nMaxFacesPerCell")) SET_AND_RETURN(avf->unstruc[meshid].header.nMaxFacesPerCell)
      if (FIELD_EQ("nPatches")) {
         avf->unstruc[meshid].patches.resize(value);
         SET_AND_RETURN(avf->unstruc[meshid].header.nPatches)
      }
      if (FIELD_EQ("nHexCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nHexCells)
      if (FIELD_EQ("nTetCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nTetCells)
      if (FIELD_EQ("nPriCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nPriCells)
      if (FIELD_EQ("nPyrCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nPyrCells)
      if (FIELD_EQ("nPolyCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nPolyCells)
      if (FIELD_EQ("nBndTriFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nBndTriFaces)
      if (FIELD_EQ("nTriFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nTriFaces)
      if (FIELD_EQ("nBndQuadFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nBndQuadFaces)
      if (FIELD_EQ("nQuadFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nQuadFaces)
      if (FIELD_EQ("nBndPolyFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nBndPolyFaces)
      if (FIELD_EQ("nPolyFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nPolyFaces)
      if (FIELD_EQ("bndPolyFacesSize")) SET_AND_RETURN(avf->unstruc[meshid].header.bndPolyFacesSize)
      if (FIELD_EQ("polyFacesSize")) SET_AND_RETURN(avf->unstruc[meshid].header.polyFacesSize)
      if (FIELD_EQ("nEdges")) SET_AND_RETURN(avf->unstruc[meshid].header.nEdges)
      if (FIELD_EQ("nNodesOnGeometry")) SET_AND_RETURN(avf->unstruc[meshid].header.nNodesOnGeometry)
      if (FIELD_EQ("nEdgesOnGeometry")) SET_AND_RETURN(avf->unstruc[meshid].header.nEdgesOnGeometry)
      if (FIELD_EQ("nFacesOnGeometry")) SET_AND_RETURN(avf->unstruc[meshid].header.nFacesOnGeometry)
      if (FIELD_EQ("geomRegionId")) SET_AND_RETURN(avf->unstruc[meshid].header.geomRegionId)

      //all remaining unstruc fields require a valid patchid
      if (patchid < 0 || patchid >= avf->unstruc[meshid].header.nPatches) {
         RETURN_ERROR("avm_set_int: invalid edgepatch");
      }
      if (FIELD_EQ("patchId")) SET_AND_RETURN(avf->unstruc[meshid].patches[patchid].patchId)
      RETURN_ERROR("avm_set_int: invalid field");
   }

   RETURN_ERROR("avm_set_int: invalid meshType");
#undef FIELD_EQ
#undef SET_AND_RETURN
}

int rev2::avm_set_real(rev2_avmesh_file* avf, const char* field, double value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a) {a=value; return 0;}
   if (field==NULL) RETURN_ERROR("avm_set_real: field is NULL");
   if (!avf) RETURN_ERROR("avm_set_real: avf is NULL");

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
      RETURN_ERROR("avm_set_real: invalid meshid");
   }

   if (FIELD_EQ("modelScale")) SET_AND_RETURN(avf->mesh_hdrs[meshid].modelScale)
   if (FIELD_EQ("referenceArea")) SET_AND_RETURN(avf->mesh_hdrs[meshid].referenceArea)

   bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
   bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

   if (strand_mesh) {
      if (FIELD_EQ("strandLength")) SET_AND_RETURN(avf->strand[meshid].header.strandLength)
      if (FIELD_EQ("stretchRatio")) SET_AND_RETURN(avf->strand[meshid].header.stretchRatio)
      if (FIELD_EQ("smoothingThreshold")) SET_AND_RETURN(avf->strand[meshid].header.smoothingThreshold)

      //all remaining strand fields require a valid edgepatch
      if (edgepatchi < 0 || edgepatchi >= avf->strand[meshid].header.nEdgePatches) {
         RETURN_ERROR("avm_set_real: invalid edgepatch");
      }
      if (FIELD_EQ("nx")) SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].nx)
      if (FIELD_EQ("ny")) SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].ny)
      if (FIELD_EQ("nz")) SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].nz)
      RETURN_ERROR("avm_set_real: invalid field");
   }

   if (unstruc_mesh) {
      RETURN_ERROR("avm_set_real: invalid field");
   }
   RETURN_ERROR("avm_set_real: invalid meshType");
#undef FIELD_EQ
#undef SET_AND_RETURN
}

int rev2::avm_set_str(rev2_avmesh_file* avf, const char* field, const char* str, int len)
{
#define JMIN(len0,len1) len0 = len0 < len1 ? len0 : len1
// hack alert: This "JMIN" macro is only used in this routine (it is undefined at the end)
// It's purpose is to capture the length of the string field in order to trim whitespace
#define FIELD_EQ(s) 0==strcmp(s,field)
#define STRCPY_AND_RETURN(a,l) {strncpy(a,str,l); for (int i=len-1; i>=0 && isspace(a[i]); --i) a[i]=0; return 0;}
#define SETSTR_AND_RETURN(a,l) {a=string(str,str+l); return 0;}

   if (field==NULL) RETURN_ERROR("avm_set_str: field is NULL");
   if (str==NULL) RETURN_ERROR("avm_set_str: str is NULL");
   if (!avf) RETURN_ERROR("avm_set_str: avf is NULL");

   if (FIELD_EQ("AVM_WRITE_ENDIAN")) {
      if (0==strcmp(str,"big")) avf->write_endian = rev2_avmesh_file::ENDIAN_BIG;
      else if (0==strcmp(str,"little")) avf->write_endian = rev2_avmesh_file::ENDIAN_LITTLE;
      else if (0==strcmp(str,"native")) avf->write_endian = rev2_avmesh_file::ENDIAN_NATIVE;
      else RETURN_ERROR("avm_set_str: invalid endianness");
      return 0;
   }

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("contactInfo")) STRCPY_AND_RETURN(avf->file_hdr.contactInfo, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("description")) {
         avf->file_hdr.descriptionSize = len;
         SETSTR_AND_RETURN(avf->description,len)
      }
      else RETURN_ERROR("avm_set_str: invalid field");
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
      RETURN_ERROR("avm_set_str: invalid meshid");
   }

   if (FIELD_EQ("meshName")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshName, JMIN(len,AVM_STD_STRING_LENGTH))
   if (FIELD_EQ("meshType")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshType, JMIN(len,AVM_STD_STRING_LENGTH))
   if (FIELD_EQ("meshGenerator")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshGenerator, JMIN(len,AVM_STD_STRING_LENGTH))
   if (FIELD_EQ("coordinateSystem")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].coordinateSystem, JMIN(len,AVM_STD_STRING_LENGTH))
   if (FIELD_EQ("gridUnits")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].gridUnits, JMIN(len,AVM_STD_STRING_LENGTH))
   if (FIELD_EQ("referencePointDescription")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].referencePointDescription, JMIN(len,AVM_STD_STRING_LENGTH))
   if (FIELD_EQ("meshDescription")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshDescription, JMIN(len,AVM_STD_STRING_LENGTH))

   bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
   bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

   if (strand_mesh) {
      if (FIELD_EQ("strandDistribution")) STRCPY_AND_RETURN(avf->strand[meshid].header.strandDistribution, JMIN(len,32))
      //all remaining strand fields require a valid surfpatch
      if (surfpatchi < 0 || surfpatchi >= avf->strand[meshid].header.nSurfPatches) {
         RETURN_ERROR("avm_set_str: invalid patchparam");
      }
      if (FIELD_EQ("surfPatchBody")) STRCPY_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBody, JMIN(len,32))
      if (FIELD_EQ("surfPatchComp")) STRCPY_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchComp, JMIN(len,32))
      if (FIELD_EQ("surfPatchBCType")) STRCPY_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBCType, JMIN(len,32))

      //all remaining strand fields require a valid surfpatch and edgepatch
      if (edgepatchi < 0 || edgepatchi >= avf->strand[meshid].header.nEdgePatches) {
         RETURN_ERROR("avm_set_str: invalid edgepatch");
      }
      if (FIELD_EQ("edgePatchBody")) STRCPY_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBody, JMIN(len,32))
      if (FIELD_EQ("edgePatchComp")) STRCPY_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchComp, JMIN(len,32))
      if (FIELD_EQ("edgePatchBCType")) STRCPY_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBCType, JMIN(len,32))
      RETURN_ERROR("avm_set_str: invalid field");
   }
      
   if (unstruc_mesh) {
      //all remaining unstruc fields require a valid patchid
      if (patchid < 0 || patchid >= avf->unstruc[meshid].header.nPatches) {
         RETURN_ERROR("avm_set_str: invalid patchid");
      }
      if (FIELD_EQ("patchLabel")) STRCPY_AND_RETURN(avf->unstruc[meshid].patches[patchid].patchLabel, JMIN(len,32))
      if (FIELD_EQ("patchType")) STRCPY_AND_RETURN(avf->unstruc[meshid].patches[patchid].patchType, JMIN(len,16))
      RETURN_ERROR("avm_set_str: invalid field");
   }

   RETURN_ERROR("avm_set_str: invalid meshType");
#undef FIELD_EQ
#undef STRCPY_AND_RETURN
#undef SETSTR_AND_RETURN
#undef JMIN
}

int rev2::avm_set_int_array(rev2_avmesh_file* avf, const char* field, const int* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a,l) {for(int i=0;i<l;++i) a[i]=values[i]; return 0;}
   if (field==NULL) RETURN_ERROR("avm_set_int_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_set_int_array: values is NULL");
   if (!avf) RETURN_ERROR("avm_set_int_array: avf is NULL");

   RETURN_ERROR("avm_set_int_array: invalid meshType");
#undef FIELD_EQ
#undef SET_AND_RETURN
}

int rev2::avm_set_real_array(rev2_avmesh_file* avf, const char* field, const double* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a,l) {for(int i=0;i<l;++i) a[i]=values[i]; return 0;}
   if (field==NULL) RETURN_ERROR("avm_set_real_array: field is NULL");
   if (values==NULL) RETURN_ERROR("avm_set_real_array: values is NULL");
   if (!avf) RETURN_ERROR("avm_set_real_array: avf is NULL");

   int meshid = avf->selection_meshid-1; 

   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) { // mesh_header
      RETURN_ERROR("avm_set_real_array: invalid meshid");
   }
   if (FIELD_EQ("referencePoint")) SET_AND_RETURN(avf->mesh_hdrs[meshid].referencePoint, min(len,3))
   if (FIELD_EQ("referenceLength")) SET_AND_RETURN(avf->mesh_hdrs[meshid].referenceLength, min(len,3))

   RETURN_ERROR("avm_set_real_array: invalid field");
#undef FIELD_EQ
#undef SET_AND_RETURN
}

int rev2::avm_mesh_header_offset(rev2_avmesh_file* avf, int mesh, off_t* offset)
{
   if (offset==NULL) RETURN_ERROR("avm_mesh_header_offset: offset is NULL");
   if (!avf) RETURN_ERROR("avm_mesh_header_offset: avf is NULL");

   mesh = mesh-1;
   if (mesh < 0 || mesh > avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_mesh_header_offset: invalid meshid");
   }

   *offset = 0;
   *offset += avf->file_prefix.size();
   *offset += avf->file_hdr.size();
   *offset += avf->file_hdr.descriptionSize;

   for (int i=0; i<mesh; ++i) {
      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      *offset += mesh_hdr.size();
      if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         *offset += avf->strand[i].hdrsize();
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         *offset += avf->unstruc[i].hdrsize();
      } else RETURN_ERROR("avm_mesh_header_offset: invalid meshType");
   }

   return 0;
}

int rev2::avm_mesh_data_offset(rev2_avmesh_file* avf, int mesh, off_t* offset)
{
   if (offset==NULL) RETURN_ERROR("avm_mesh_data_offset: offset is NULL");
   if (!avf) RETURN_ERROR("avm_mesh_data_offset: avf is NULL");

   mesh = mesh-1;
   if (mesh < 0 || mesh > avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_mesh_data_offset: invalid meshid");
   }
   
   *offset = 0;
   if (avm_mesh_header_offset(avf, avf->file_hdr.meshCount+1, offset)) return 1;

   for (int i=0; i<mesh; ++i) {
      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         *offset += avf->strand[i].datasize(avf->file_hdr, mesh_hdr);
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         *offset += avf->unstruc[i].datasize(avf->file_hdr, mesh_hdr);
      } else RETURN_ERROR("avm_mesh_data_offset: invalid meshType");
   }

   return 0;
}

int rev2::seek(FILE* fp, off_t offset)
{
#ifdef WIN32
   if (-1==fseek(fp, (long)offset, SEEK_SET)) return 1;
#else
   if (-1==fseeko(fp, offset, SEEK_SET)) return 1;
#endif
   return 0;
}

int rev2::avm_seek_to_mesh(rev2_avmesh_file* avf, int mesh)
{
   if (!avf) RETURN_ERROR("avm_seek_to_mesh: avf is NULL");

   off_t mesh_offset;
   if (avm_mesh_data_offset(avf, mesh, &mesh_offset)) return 1;

   return seek(avf->fp, mesh_offset);
}

int rev2::avm_strand_read_nodes_r4(rev2_avmesh_file* avf,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: xStrand is NULL");
   if (!avf) RETURN_ERROR("avm_strand_read_nodes_r4: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_read_nodes_r4: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_read_nodes_r4: selected mesh is not a strand mesh");
   }

   const strand_header& hdr = avf->strand[meshid].header;

   if (xyz_size != 3*hdr.nNodes) {
      RETURN_ERROR("avm_strand_read_nodes_r4: xyz_size does not match header");
   }
   if (hdr.nNodes > 0) {
      if (!fread(xyz, sizeof(float)*xyz_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r4: failed reading xyz array");
      }
   }

   if (nodeID_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_read_nodes_r4: nodeID_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fread(nodeID, sizeof(int)*nodeID_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r4: failed reading nodeID array");
      }
   }

   if (nodeClip_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_read_nodes_r4: nodeClip_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fread(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r4: failed reading nodeClip array");
      }
   }

   if (pointingVec_size != 3*hdr.nStrands) {
      RETURN_ERROR("avm_strand_read_nodes_r4: pointingVec_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fread(pointingVec, sizeof(float)*pointingVec_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r4: failed reading pointingVec array");
      }
   }

   if (xStrand_size != hdr.nPtsPerStrand) {
      RETURN_ERROR("avm_strand_read_nodes_r4: xStrand_size does not match header");
   }
   if (hdr.nPtsPerStrand > 0) {
      if (!fread(xStrand, sizeof(float)*xStrand_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r4: failed reading xStrand array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<xyz_size; ++i) byte_swap_float(xyz+i);
      for (int i=0; i<nodeID_size; ++i) byte_swap_int(nodeID+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_float(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_float(xStrand+i);
   }

   return 0;
}

int rev2::avm_strand_read_nodes_r8(rev2_avmesh_file* avf,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,       int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: xStrand is NULL");
   if (!avf) RETURN_ERROR("avm_strand_read_nodes_r8: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_read_nodes_r8: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_read_nodes_r8: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (xyz_size != 3*hdr.nNodes) {
      RETURN_ERROR("avm_strand_read_nodes_r8: xyz_size does not match header");
   }
   if (hdr.nNodes > 0) {
      if (!fread(xyz, sizeof(double)*xyz_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r8: failed reading xyz array");
      }
   }

   if (nodeID_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_read_nodes_r8: nodeID_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fread(nodeID, sizeof(int)*nodeID_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r8: failed reading nodeID array");
      }
   }

   if (nodeClip_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_read_nodes_r8: nodeClip_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fread(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r8: failed reading nodeClip array");
      }
   }

   if (pointingVec_size != 3*hdr.nStrands) {
      RETURN_ERROR("avm_strand_read_nodes_r8: pointingVec_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fread(pointingVec, sizeof(double)*pointingVec_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r8: failed reading pointingVec array");
      }
   }

   if (xStrand_size != hdr.nPtsPerStrand) {
      RETURN_ERROR("avm_strand_read_nodes_r8: xStrand_size does not match header");
   }
   if (hdr.nPtsPerStrand > 0) {
      if (!fread(xStrand, sizeof(double)*xStrand_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_nodes_r8: failed reading xStrand array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<xyz_size; ++i) byte_swap_double(xyz+i);
      for (int i=0; i<nodeID_size; ++i) byte_swap_int(nodeID+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_double(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_double(xStrand+i);
   }

   return 0;
}

int rev2::avm_strand_read_faces(rev2_avmesh_file* avf,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_strand_read_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_strand_read_faces: quadFaces is NULL");
   if (polyFaces_size > 0 && polyFaces==NULL) RETURN_ERROR("avm_strand_read_faces: polyFaces is NULL");
   if (faceTag_size > 0 && faceTag==NULL) RETURN_ERROR("avm_strand_read_faces: faceTag is NULL");
   if (!avf) RETURN_ERROR("avm_strand_read_faces: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_read_faces: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_read_faces: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (triFaces_size != 3*hdr.nTriFaces) {
      RETURN_ERROR("avm_strand_read_faces: triFaces_size does not match header");
   }
   if (hdr.nTriFaces > 0) {
      if (!fread(triFaces, sizeof(int)*triFaces_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_faces: failed reading triFaces array");
      }
   }

   if (quadFaces_size != 4*hdr.nQuadFaces) {
      RETURN_ERROR("avm_strand_read_faces: quadFaces_size does not match header");
   }
   if (hdr.nQuadFaces > 0) {
      if (!fread(quadFaces, sizeof(int)*quadFaces_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_faces: failed reading quadFaces array");
      }
   }

   if (polyFaces_size != hdr.polyFacesSize) {
      RETURN_ERROR("avm_strand_read_faces: polyFaces_size does not match header");
   }
   if (hdr.polyFacesSize > 0) {
      if (!fread(polyFaces, sizeof(int)*polyFaces_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_faces: failed reading polyFaces array");
      }
   }

   if (faceTag_size != hdr.nTriFaces+hdr.nQuadFaces+hdr.nPolyFaces) {
      RETURN_ERROR("avm_strand_read_faces: faceTag_size does not match header");
   }
   if (hdr.nTriFaces+hdr.nQuadFaces+hdr.nPolyFaces > 0) {
      if (!fread(faceTag, sizeof(int)*faceTag_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_faces: failed reading faceTag array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
      for (int i=0; i<polyFaces_size; ++i) byte_swap_int(polyFaces+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
   }

   return 0;
}

int rev2::avm_strand_read_edges(rev2_avmesh_file* avf,
  int* bndEdges,      int bndEdges_size,
   int* edgeTag,       int edgeTag_size
)
{

   if (bndEdges_size > 0 && bndEdges==NULL) RETURN_ERROR("avm_strand_read_edges: bndEdges is NULL");
   if (edgeTag_size > 0 && edgeTag==NULL) RETURN_ERROR("avm_strand_read_edges: edgeTag is NULL");
   if (!avf) RETURN_ERROR("avm_strand_read_edges: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_read_edges: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_read_edges: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (bndEdges_size != 2*hdr.nBndEdges) {
      RETURN_ERROR("avm_strand_read_edges: bndEdges_size does not match header");
   }
   if (hdr.nBndEdges > 0) {
      if (!fread(bndEdges, sizeof(int)*bndEdges_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_edges: failed reading bndEdges array");
      }
   }

   if (edgeTag_size != hdr.nBndEdges) {
      RETURN_ERROR("avm_strand_read_edges: edgeTag_size does not match header");
   }
   if (hdr.nBndEdges > 0) {
      if (!fread(edgeTag, sizeof(int)*edgeTag_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_edges: failed reading edgeTag array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
   }

   return 0;
}

int rev2::avm_strand_read_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
)
{
   if (nodesOnGeometry_size > 0 && nodesOnGeometry==NULL) RETURN_ERROR("avm_strand_read_amr: nodesOnGeometry is NULL");
   if (facesOnGeometry_size > 0 && facesOnGeometry==NULL) RETURN_ERROR("avm_strand_read_amr: facesOnGeometry is NULL");
   if (edgesOnGeometry_size > 0 && edgesOnGeometry==NULL) RETURN_ERROR("avm_strand_read_amr: edgesOnGeometry is NULL");
   if (!avf) RETURN_ERROR("avm_strand_read_amr: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_read_amr: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_read_amr: selected mesh is not a strand mesh");
   }

   const strand_header& hdr = avf->strand[meshid].header;

// validate size of amr buffers
// (this is strand format revision 1)
   if (nodesOnGeometry_size != hdr.nNodesOnGeometry) {
      RETURN_ERROR("avm_strand_read_amr: nodesOnGeometry_size does not match header");
   }
   if (edgesOnGeometry_size != 3*hdr.nEdgesOnGeometry) {
      RETURN_ERROR("avm_strand_read_amr: edgesOnGeometry_size does not match header");
   }
   if (facesOnGeometry_size != 3*hdr.nFacesOnGeometry) {
      RETURN_ERROR("avm_strand_read_amr: facesOnGeometry_size does not match header");
   }

// read amr
   if (hdr.nNodesOnGeometry > 0) {
      if (!fread(nodesOnGeometry, sizeof(AMR_Node_Data)*hdr.nNodesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_amr: failed reading nodesOnGeometry array");
      }
   }
   if (hdr.nEdgesOnGeometry > 0) {
      if (!fread(edgesOnGeometry, sizeof(int)*3*hdr.nEdgesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_amr: failed reading edgesOnGeometry array");
      }
   }
   if (hdr.nFacesOnGeometry > 0) {
      if (!fread(facesOnGeometry, sizeof(int)*3*hdr.nFacesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_read_amr: failed reading facesOnGeometry array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<nodesOnGeometry_size; ++i) {
         byte_swap_int(&(nodesOnGeometry[i].nodeIndex));
         byte_swap_int(&(nodesOnGeometry[i].geomType));
         byte_swap_int(&(nodesOnGeometry[i].geomTopoId));
         byte_swap_double(&(nodesOnGeometry[i].u));
         byte_swap_double(&(nodesOnGeometry[i].v));
      }
      for (int i=0; i<edgesOnGeometry_size; ++i) byte_swap_int(edgesOnGeometry+i);
      for (int i=0; i<facesOnGeometry_size; ++i) byte_swap_int(facesOnGeometry+i);
   }

   return 0;
}

int rev2::avm_strand_write_faces(rev2_avmesh_file* avf,
   int* triFaces,       int triFaces_size,
   int* quadFaces,      int quadFaces_size,
   int* polyFaces,      int polyFaces_size,
   int* faceTag,       int faceTag_size
)
{
   if (triFaces_size > 0 && triFaces==NULL) RETURN_ERROR("avm_strand_write_faces: triFaces is NULL");
   if (quadFaces_size > 0 && quadFaces==NULL) RETURN_ERROR("avm_strand_write_faces: quadFaces is NULL");
   if (polyFaces_size > 0 && polyFaces==NULL) RETURN_ERROR("avm_strand_write_faces: polyFaces is NULL");
   if (faceTag_size > 0 && faceTag==NULL) RETURN_ERROR("avm_strand_write_faces: faceTag is NULL");
   if (!avf) RETURN_ERROR("avm_strand_write_faces: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_write_faces: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_write_faces: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
      for (int i=0; i<polyFaces_size; ++i) byte_swap_int(polyFaces+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
   }

   if (triFaces_size != 3*hdr.nTriFaces) {
      RETURN_ERROR("avm_strand_write_faces: triFaces_size does not match header");
   }
   if (hdr.nTriFaces > 0) {
      if (!fwrite(triFaces, sizeof(int)*triFaces_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_faces: failed writing triFaces array");
      }
   }

   if (quadFaces_size != 4*hdr.nQuadFaces) {
      RETURN_ERROR("avm_strand_write_faces: quadFaces_size does not match header");
   }
   if (hdr.nQuadFaces > 0) {
      if (!fwrite(quadFaces, sizeof(int)*quadFaces_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_faces: failed writing quadFaces array");
      }
   }

   if (polyFaces_size != hdr.polyFacesSize) {
      RETURN_ERROR("avm_strand_write_faces: polyFaces_size does not match header");
   }
   if (hdr.polyFacesSize > 0) {
      if (!fwrite(polyFaces, sizeof(int)*hdr.polyFacesSize, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_faces: failed writing polyFaces array");
      }
   }

   if (faceTag_size != hdr.nTriFaces+hdr.nQuadFaces+hdr.nPolyFaces) {
      RETURN_ERROR("avm_strand_write_faces: faceTag_size does not match header");
   }
   if (hdr.nTriFaces+hdr.nQuadFaces+hdr.nPolyFaces > 0) {
      if (!fwrite(faceTag, sizeof(int)*faceTag_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_faces: failed writing faceTag array");
      }
   }
   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
      for (int i=0; i<polyFaces_size; ++i) byte_swap_int(polyFaces+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
   }

   return 0;
}

int rev2::avm_strand_write_nodes_r4(rev2_avmesh_file* avf,
   float* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   float* pointingVec, int pointingVec_size,
   float* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_strand_write_nodes_r4: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL) RETURN_ERROR("avm_strand_write_nodes_r4: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL) RETURN_ERROR("avm_strand_write_nodes_r4: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_write_nodes_r4: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL) RETURN_ERROR("avm_strand_read_nodes_r4: xStrand is NULL");
   if (!avf) RETURN_ERROR("avm_strand_write_nodes_r4: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_write_nodes_r4: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_write_nodes_r4: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (avf->byte_swap) {
      for (int i=0; i<xyz_size; ++i) byte_swap_float(xyz+i);
      for (int i=0; i<nodeID_size; ++i) byte_swap_int(nodeID+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_float(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_float(xStrand+i);
   }

   if (xyz_size != 3*hdr.nNodes) {
      RETURN_ERROR("avm_strand_write_nodes_r4: xyz_size does not match header");
   }
   if (hdr.nNodes > 0) {
      if (!fwrite(xyz, sizeof(float)*xyz_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r4: failed writing xyz array");
      }
   }

   if (nodeID_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_write_nodes_r4: nodeID_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fwrite(nodeID, sizeof(int)*nodeID_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r4: failed writing nodeID array");
      }
   }

   if (nodeClip_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_write_nodes_r4: nodeClip_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fwrite(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r4: failed writing nodeClip array");
      }
   }

   if (pointingVec_size != 3*hdr.nStrands) {
      RETURN_ERROR("avm_strand_write_nodes_r4: pointingVec_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fwrite(pointingVec, sizeof(float)*pointingVec_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r4: failed writing pointingVec array");
      }
   }

   if (xStrand_size != hdr.nPtsPerStrand) {
      RETURN_ERROR("avm_strand_write_nodes_r4: xStrand_size does not match header");
   }
   if (hdr.nPtsPerStrand > 0) {
      if (!fwrite(xStrand, sizeof(float)*xStrand_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r4: failed writing xStrand array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<xyz_size; ++i) byte_swap_float(xyz+i);
      for (int i=0; i<nodeID_size; ++i) byte_swap_int(nodeID+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_float(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_float(xStrand+i);
   }

   return 0;
}

int rev2::avm_strand_write_nodes_r8(rev2_avmesh_file* avf,
   double* xyz,         int xyz_size,
   int* nodeID,        int nodeID_size,
   int* nodeClip,      int nodeClip_size,
   double* pointingVec, int pointingVec_size,
   double* xStrand,     int xStrand_size
)
{
   if (xyz_size > 0 && xyz==NULL) RETURN_ERROR("avm_strand_write_nodes_r8: xyz is NULL");
   if (nodeID_size > 0 && nodeID==NULL) RETURN_ERROR("avm_strand_write_nodes_r8: nodeID is NULL");
   if (nodeClip_size > 0 && nodeClip==NULL) RETURN_ERROR("avm_strand_write_nodes_r8: nodeClip is NULL");
   if (pointingVec_size > 0 && pointingVec==NULL) RETURN_ERROR("avm_strand_write_nodes_r8: pointingVec is NULL");
   if (xStrand_size > 0 && xStrand==NULL) RETURN_ERROR("avm_strand_read_nodes_r8: xStrand is NULL");
   if (!avf) RETURN_ERROR("avm_strand_write_nodes_r8: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_write_nodes_r8: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_write_nodes_r8: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (avf->byte_swap) {
      for (int i=0; i<xyz_size; ++i) byte_swap_double(xyz+i);
      for (int i=0; i<nodeID_size; ++i) byte_swap_int(nodeID+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_double(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_double(xStrand+i);
   }

   if (xyz_size != 3*hdr.nNodes) {
      RETURN_ERROR("avm_strand_write_nodes_r8: xyz_size does not match header");
   }
   if (hdr.nNodes > 0) {
      if (!fwrite(xyz, sizeof(double)*xyz_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r8: failed writing xyz array");
      }
   }

   if (nodeID_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_write_nodes_r8: nodeID_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fwrite(nodeID, sizeof(int)*nodeID_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r8: failed writing nodeID array");
      }
   }

   if (nodeClip_size != hdr.nStrands) {
      RETURN_ERROR("avm_strand_write_nodes_r8: nodeClip_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fwrite(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r8: failed writing nodeClip array");
      }
   }

   if (pointingVec_size != 3*hdr.nStrands) {
      RETURN_ERROR("avm_strand_write_nodes_r8: pointingVec_size does not match header");
   }
   if (hdr.nStrands > 0) {
      if (!fwrite(pointingVec, sizeof(double)*pointingVec_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r8: failed writing pointingVec array");
      }
   }

   if (xStrand_size != hdr.nPtsPerStrand) {
      RETURN_ERROR("avm_strand_write_nodes_r8: xStrand_size does not match header");
   }
   if (hdr.nPtsPerStrand > 0) {
      if (!fwrite(xStrand, sizeof(double)*xStrand_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_nodes_r8: failed writing xStrand array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<xyz_size; ++i) byte_swap_double(xyz+i);
      for (int i=0; i<nodeID_size; ++i) byte_swap_int(nodeID+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_double(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_double(xStrand+i);
   }

   return 0;
}

int rev2::avm_strand_write_edges(rev2_avmesh_file* avf,
   int* bndEdges,      int bndEdges_size,
   int* edgeTag,       int edgeTag_size
)
{
   if (bndEdges_size > 0 && bndEdges==NULL) RETURN_ERROR("avm_strand_write_edges: bndEdges is NULL");
   if (edgeTag_size > 0 && edgeTag==NULL) RETURN_ERROR("avm_strand_write_edges: edgeTag is NULL");
   if (!avf) RETURN_ERROR("avm_strand_write_edges: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_write_edges: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_write_edges: selected mesh is not a strand mesh");
   }
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (avf->byte_swap) {
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
   }

   if (bndEdges_size != 2*hdr.nBndEdges) {
      RETURN_ERROR("avm_strand_write_edges: bndEdges_size does not match header");
   }
   if (hdr.nBndEdges > 0) {
      if (!fwrite(bndEdges, sizeof(int)*bndEdges_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_edges: failed writing bndEdges array");
      }
   }

   if (edgeTag_size != hdr.nBndEdges) {
      RETURN_ERROR("avm_strand_write_edges: edgeTag_size does not match header");
   }
   if (hdr.nBndEdges > 0) {
      if (!fwrite(edgeTag, sizeof(int)*edgeTag_size, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_edges: failed writing edgeTag array");
      }
   }

   if (avf->byte_swap) {
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
   }

   return 0;
}

int rev2::avm_strand_write_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size
)
{
   if (nodesOnGeometry_size > 0 && nodesOnGeometry==NULL) RETURN_ERROR("avm_strand_write_amr: nodesOnGeometry is NULL");
   if (facesOnGeometry_size > 0 && facesOnGeometry==NULL) RETURN_ERROR("avm_strand_write_amr: facesOnGeometry is NULL");
   if (edgesOnGeometry_size > 0 && edgesOnGeometry==NULL) RETURN_ERROR("avm_strand_write_amr: edgesOnGeometry is NULL");
   if (!avf) RETURN_ERROR("avm_strand_write_amr: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_strand_write_amr: invalid meshid");
   }
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) {
      RETURN_ERROR("avm_strand_write_amr: selected mesh is not a strand mesh");
   }

   const strand_header& hdr = avf->strand[meshid].header;

// validate size of amr buffers
// (this is strand format revision 1)
   if (nodesOnGeometry_size != hdr.nNodesOnGeometry) {
      RETURN_ERROR("avm_strand_write_amr: nodesOnGeometry_size does not match header");
   }
   if (edgesOnGeometry_size != 3*hdr.nEdgesOnGeometry) {
      RETURN_ERROR("avm_strand_write_amr: edgesOnGeometry_size does not match header");
   }
   if (facesOnGeometry_size != 3*hdr.nFacesOnGeometry) {
      RETURN_ERROR("avm_strand_write_amr: facesOnGeometry_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<nodesOnGeometry_size; ++i) {
         byte_swap_int(&(nodesOnGeometry[i].nodeIndex));
         byte_swap_int(&(nodesOnGeometry[i].geomType));
         byte_swap_int(&(nodesOnGeometry[i].geomTopoId));
         byte_swap_double(&(nodesOnGeometry[i].u));
         byte_swap_double(&(nodesOnGeometry[i].v));
      }
      for (int i=0; i<edgesOnGeometry_size; ++i) byte_swap_int(edgesOnGeometry+i);
      for (int i=0; i<facesOnGeometry_size; ++i) byte_swap_int(facesOnGeometry+i);
   }

// Write amr
   if (hdr.nNodesOnGeometry > 0) {
      if (!fwrite(nodesOnGeometry, sizeof(AMR_Node_Data)*hdr.nNodesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_amr: failed writing nodesOnGeometry array");
      }
   }
   if (hdr.nEdgesOnGeometry > 0) {
      if (!fwrite(edgesOnGeometry, sizeof(int)*3*hdr.nEdgesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_amr: failed writing edgesOnGeometry array");
      }
   }
   if (hdr.nFacesOnGeometry > 0) {
      if (!fwrite(facesOnGeometry, sizeof(int)*3*hdr.nFacesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_strand_write_amr: failed writing facesOnGeometry array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<nodesOnGeometry_size; ++i) {
         byte_swap_int(&(nodesOnGeometry[i].nodeIndex));
         byte_swap_int(&(nodesOnGeometry[i].geomType));
         byte_swap_int(&(nodesOnGeometry[i].geomTopoId));
         byte_swap_double(&(nodesOnGeometry[i].u));
         byte_swap_double(&(nodesOnGeometry[i].v));
      }
      for (int i=0; i<edgesOnGeometry_size; ++i) byte_swap_int(edgesOnGeometry+i);
      for (int i=0; i<facesOnGeometry_size; ++i) byte_swap_int(facesOnGeometry+i);
   }

   return 0;
}

int rev2::avm_unstruc_seek_to(rev2_avmesh_file* avf, char* section, off_t start=0)
{
   if (section==NULL) RETURN_ERROR("avm_unstruc_seek_to: section is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_seek_to: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_seek_to: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_seek_to: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;
   off_t offset;
   if (avm_mesh_data_offset(avf, meshid+1, &offset)) return 1;

   int precision;
   if (avf->file_hdr.precision==1) {
      precision = 4;
   }
   else if (avf->file_hdr.precision==2) {
      precision = 8;
   }
   else RETURN_ERROR("avm_unstruc_seek_to: invalid precision in metadata");

   if(0==strncmp("nodes",section,strlen("nodes"))) {
      offset += 3 * start * precision;
      return seek(avf->fp,offset);
   }

   offset += 3 * (off_t)hdr.nNodes * precision;

   if(0==strncmp("bndFaces",section,strlen("bndFaces"))) {
      return seek(avf->fp,offset);
   }
   if(0==strncmp("bndTris",section,strlen("bndTris"))) {
      offset += 5 * start * 4;
      return seek(avf->fp,offset);
   }

   offset += 5 * (off_t)hdr.nBndTriFaces * 4;

   if(0==strncmp("bndQuads",section,strlen("bndQuads"))) {
      offset += 6 * start * 4;
      return seek(avf->fp,offset);
   }

   offset += 6 * (off_t)hdr.nBndQuadFaces * 4;

   if(0==strncmp("bndPolys",section,strlen("bndPolys"))) {
      //since we don't know how big polys are, expect start to be the number of array indices to skip
      //or we could read only the nNodesInFace to jump to the next one as a way of counting...
      offset += start * 4;
      return seek(avf->fp,offset);
   }

   offset += (off_t)hdr.bndPolyFacesSize * 4;

   if(0==strncmp("faces",section,strlen("faces")))
      return seek(avf->fp,offset);
   if(0==strncmp("intTris",section,strlen("intTris"))) {
      offset += 5 * start * 4;
      return seek(avf->fp,offset);
   }

   offset += 5 * ((off_t)hdr.nTriFaces-(off_t)hdr.nBndTriFaces) * 4;

   if(0==strncmp("intQuads",section,strlen("intQuads"))) {
      offset += 6 * start * 4;
      return seek(avf->fp,offset);
   }

   offset += 6 * ((off_t)hdr.nQuadFaces-(off_t)hdr.nBndQuadFaces) * 4;

   if(0==strncmp("intPolys",section,strlen("intPolys"))) {
      //since we don't know how big polys are, expect start to be the number of array indices to skip
      //or we could read only the nNodesInFace to jump to the next one as a way of counting...
      offset += start * 4;
      return seek(avf->fp,offset);
   }

   offset += ((off_t)hdr.polyFacesSize-(off_t)hdr.bndPolyFacesSize) * 4;

   if(0==strncmp("cells",section,strlen("cells")))
      return seek(avf->fp,offset);
   if(0==strncmp("hexes",section,strlen("hexes"))) {
      offset += 8 * start * 4;
      return seek(avf->fp,offset);
   }

   offset += 8 * (off_t)hdr.nHexCells * 4;

   if(0==strncmp("tets",section,strlen("tets"))) {
      offset += 4 * start * 4;
      return seek(avf->fp,offset);
   }

   offset += 4 * (off_t)hdr.nTetCells * 4;

   if(0==strncmp("pris",section,strlen("prisms"))) {
      offset += 6 * (off_t)start * 4;
      return seek(avf->fp,offset);
   }

   offset += 6 * (off_t)hdr.nPriCells * 4;

   if(0==strncmp("pyrs",section,strlen("pyramids"))) {
      offset += 5 * (off_t)start * 4;
      return seek(avf->fp,offset);
   }

   offset += 5 * (off_t)hdr.nPyrCells * 4;

   if(0==strncmp("edges",section,strlen("edges")))
      return seek(avf->fp,offset);
   offset += 2 * (off_t)hdr.nEdges * 4;

   if(0==strncmp("amr",section,strlen("amr")))
      return seek(avf->fp,offset);
   offset += sizeof(AMR_Node_Data) * (off_t)hdr.nNodesOnGeometry;
   offset += 3 * (off_t)hdr.nEdgesOnGeometry * 4;
   offset += 3 * (off_t)hdr.nFacesOnGeometry * 4;

   if(0==strncmp("volumeids",section,strlen("volumeids")))
      return seek(avf->fp,offset);
   offset += hdr.nHexCells * 4;
   offset += hdr.nTetCells * 4;
   offset += hdr.nPriCells * 4;
   offset += hdr.nPyrCells * 4;

   RETURN_ERROR("avm_unstruc_seek_to: invalid seek section");
}

int rev2::avm_unstruc_read_nodes_r4(rev2_avmesh_file* avf, float* xyz, int xyz_size)
{
   if (!avf) RETURN_ERROR("avm_unstruc_read_nodes_r4: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_nodes_r4: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_nodes_r4: selected mesh is not an unstruc mesh");
   }
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   return avm_unstruc_read_partial_nodes_r4(avf, xyz, xyz_size, 1, hdr.nNodes);
}

int rev2::avm_unstruc_read_nodes_r8(rev2_avmesh_file* avf, double* xyz, int xyz_size)
{
   if (!avf) RETURN_ERROR("avm_unstruc_read_nodes_r8: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_nodes_r8: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_nodes_r8: selected mesh is not an unstruc mesh");
   }
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   return avm_unstruc_read_partial_nodes_r8(avf, xyz, xyz_size, 1, hdr.nNodes);
}

int rev2::avm_unstruc_read_partial_nodes_r4(rev2_avmesh_file* avf, float* xyz, int xyz_size, int start, int end)
{
   if (xyz_size>0 && xyz==NULL) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: xyz is NULL");
   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

   int nodesToRead = end-start+1;
   if (xyz_size != 3*nodesToRead) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: xyz_size does not match requested number of nodes");
   }

   if (start > hdr.nNodes) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: start index cannot be greater than nNodes");
   if (start > end) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: start index cannot be greater than end index");
   if (end > hdr.nNodes) RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: end index cannot be greater than nNodes");

   //seek to appropriate spot, taking into account the number of nodes to skip before the start
   if (avm_unstruc_seek_to(avf,"nodes",start-1)) return 1;

   if (hdr.nNodes > 0) {
      if (!fread(xyz, sizeof(float)*3*nodesToRead, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_partial_nodes_r4: failed reading xyz array");
      }
   }

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_float(xyz+i);

   return 0;
}

int rev2::avm_unstruc_read_partial_nodes_r8(rev2_avmesh_file* avf, double* xyz, int xyz_size, int start, int end)
{
   if (xyz_size>0 && xyz==NULL) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: xyz is NULL");
   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: selected mesh is not an unstruc mesh");
   }
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   int nodesToRead = end-start+1;
   if (xyz_size != 3*nodesToRead) {
      RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: xyz_size does not match requested number of nodes");
   }

   if (start > hdr.nNodes) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: start index cannot be greater than nNodes");
   if (start > end) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: start index cannot be greater than end index");
   if (end > hdr.nNodes) RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: end index cannot be greater than nNodes");

   //seek to appropriate spot, taking into account the number of nodes to skip before the start
   if (avm_unstruc_seek_to(avf,"nodes",start-1)) return 1;

   if (hdr.nNodes > 0) {
      if (!fread(xyz, sizeof(double)*3*nodesToRead, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_partial_nodes_r8: failed reading xyz array");
      }
   }

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_double(xyz+i);

   return 0;
}

int rev2::avm_unstruc_read_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size,
   int* polyFaces, int polyFaces_size
)
{
   if (triFaces_size>0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: triFaces is NULL");
   if (quadFaces_size>0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: quadFaces is NULL");
   if (polyFaces_size>0 && polyFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: polyFaces is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_faces: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_faces: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_faces: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of face buffers
// (this is unstruc format revision 1)
   if (triFaces && triFaces_size != 5*hdr.nTriFaces) {
      RETURN_ERROR("avm_unstruc_read_faces: triFaces_size does not match header");
   }
   if (quadFaces && quadFaces_size != 6*hdr.nQuadFaces) {
      RETURN_ERROR("avm_unstruc_read_faces: quadFaces_size does not match header");
   }
   if (polyFaces && polyFaces_size != hdr.polyFacesSize) {
      RETURN_ERROR("avm_unstruc_read_faces: polyFaces_size does not match header");
   }

   if (avm_unstruc_seek_to(avf,"bndFaces")) return 1;

// Tri patch faces
   if(triFaces && hdr.nBndTriFaces > 0) {
      if (avm_unstruc_seek_to(avf,"bndTris")) return 1;
      if (!fread(triFaces, sizeof(int)*5*hdr.nBndTriFaces, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading boundary triFaces array");
      }
   }

// Quad patch faces
   if(quadFaces && hdr.nBndQuadFaces > 0) {
      if (avm_unstruc_seek_to(avf,"bndQuads")) return 1;
      if (!fread(quadFaces, sizeof(int)*6*hdr.nBndQuadFaces, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading boundary quadFaces array");
      }
   }

// Poly patch faces
   if(polyFaces && hdr.bndPolyFacesSize > 0) {
      if (avm_unstruc_seek_to(avf,"bndPolys")) return 1;
      if (!fread(polyFaces, sizeof(int)*hdr.bndPolyFacesSize, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading boundary polyFaces array");
      }
   }

// Tri interior faces
   if(triFaces && hdr.nTriFaces-hdr.nBndTriFaces > 0) {
      if (avm_unstruc_seek_to(avf,"intTris")) return 1;
      if (!fread(triFaces+(5*hdr.nBndTriFaces), sizeof(int)*5*(hdr.nTriFaces-hdr.nBndTriFaces), 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading volume triFaces array");
      }
   }

// Quad interior faces
   if(quadFaces && hdr.nQuadFaces-hdr.nBndQuadFaces > 0) {
      if (avm_unstruc_seek_to(avf,"intQuads")) return 1;
      if (!fread(quadFaces+(6*hdr.nBndQuadFaces), sizeof(int)*6*(hdr.nQuadFaces-hdr.nBndQuadFaces), 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading volume quadFaces array");
      }
   }

// Poly interior faces
   if(polyFaces && hdr.polyFacesSize-hdr.bndPolyFacesSize > 0) {
      if (avm_unstruc_seek_to(avf,"intPolys")) return 1;
      if (!fread(polyFaces+hdr.bndPolyFacesSize, sizeof(int)*(hdr.polyFacesSize-hdr.bndPolyFacesSize), 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading volume polyFaces array");
      }
   }

// byte swap
   if (avf->byte_swap) for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
   if (avf->byte_swap) for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
   if (avf->byte_swap) for (int i=0; i<polyFaces_size; ++i) byte_swap_int(polyFaces+i);

   return 0;
}

int rev2::avm_unstruc_read_tri(rev2_avmesh_file* avf,
   int* triFaces, int triFaces_size,
   int stride, int start, int end, int flags
)
{
   if (triFaces_size>0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_read_tri: triFaces is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_tri: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_tri: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_tri: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

   bool nodes = AVM_NODES & flags;
   bool cells = AVM_CELLS & flags;
   bool nodescells = AVM_NODESCELLS & flags;
   bool boundary = AVM_BOUNDARY & flags;
   bool interior = AVM_INTERIOR & flags;

   if ((nodes && cells) || (nodes && nodescells) || (cells && nodescells))
      RETURN_ERROR("avm_unstruc_read_tri: Must specify exactly one of AVM_NODES, AVM_CELLS, AVM_NODESCELLS");
   if (!nodes && !cells && !nodescells)
      RETURN_ERROR("avm_unstruc_read_tri: Must specify exactly one of AVM_NODES, AVM_CELLS, AVM_NODESCELLS");
   //FIXME: currently assuming AVM_NODESCELLS
   if (!nodescells)
      RETURN_ERROR("avm_unstruc_read_tri: Must specify AVM_NODESCELLS");
   //FIXME: support AVM_BOUNDARY & AVM_INTERIOR
   if ((!boundary && !interior) || (boundary && interior))
      RETURN_ERROR("avm_unstruc_read_tri: Must specify exactly one of AVM_BOUNDARY and AVM_INTERIOR");

   int trisToRead = end-start+1;

   //handle the zero tri case gracefully
   if((start < 0 && end < 0) || triFaces_size == 0) trisToRead = 0;

   if (triFaces_size != 5*trisToRead) {
      RETURN_ERROR("avm_unstruc_read_tri: triFaces_size does not match requested number of tris");
   }

   //FIXME: currently ignoring stride
   if (boundary) {
      if (trisToRead > 0 && start > hdr.nBndTriFaces) RETURN_ERROR("avm_unstruc_read_tri: start cannot be greater than nBndTriFaces");
      if (trisToRead > 0 && start > end) RETURN_ERROR("avm_unstruc_read_tri: start cannot be greater than end");
      if (trisToRead > 0 && end > hdr.nBndTriFaces) RETURN_ERROR("avm_unstruc_read_tri: end cannot be greater than nBndTriFaces");

      if (hdr.nBndTriFaces > 0 && trisToRead > 0) {
         if (avm_unstruc_seek_to(avf,"bndTris",start-1)) return 1;
         if (!fread(triFaces, sizeof(int)*5*trisToRead, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_read_tri: failed reading boundary triFaces array");
         }
      }
   }
   if (interior) {
      int nInteriorFaces = hdr.nTriFaces - hdr.nBndTriFaces;
      if (trisToRead > 0 && start > nInteriorFaces) RETURN_ERROR("avm_unstruc_read_tri: start cannot be greater than nTriFaces - nBndTriFaces");
      if (trisToRead > 0 && start > end) RETURN_ERROR("avm_unstruc_read_tri: start cannot be greater than end");
      if (trisToRead > 0 && end > nInteriorFaces) RETURN_ERROR("avm_unstruc_read_tri: end cannot be greater than nTriFaces - nBndTriFaces");

      if (nInteriorFaces > 0 && trisToRead > 0) {
         if (avm_unstruc_seek_to(avf,"intTris",start-1)) return 1;
         if (!fread(triFaces, sizeof(int)*5*trisToRead, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_read_tri: failed reading interior triFaces array");
         }
      }
   }

   // byte swap
   if (avf->byte_swap) for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);

   return 0;
}

int rev2::avm_unstruc_read_quad(rev2_avmesh_file* avf,
   int* quadFaces, int quadFaces_size,
   int stride, int start, int end, int flags
)
{
   if (quadFaces_size>0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_read_quad: quadFaces is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_quad: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_quad: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_quad: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

   bool nodes = AVM_NODES & flags;
   bool cells = AVM_CELLS & flags;
   bool nodescells = AVM_NODESCELLS & flags;
   bool boundary = AVM_BOUNDARY & flags;
   bool interior = AVM_INTERIOR & flags;

   if ((nodes && cells) || (nodes && nodescells) || (cells && nodescells))
      RETURN_ERROR("avm_unstruc_read_quad: Must specify exactly one of AVM_NODES, AVM_CELLS, AVM_NODESCELLS");
   if (!nodes && !cells && !nodescells)
      RETURN_ERROR("avm_unstruc_read_quad: Must specify exactly one of AVM_NODES, AVM_CELLS, AVM_NODESCELLS");
   //FIXME: currently assuming AVM_NODESCELLS
   if (!nodescells)
      RETURN_ERROR("avm_unstruc_read_quad: Must specify AVM_NODESCELLS");
   //FIXME: support AVM_BOUNDARY & AVM_INTERIOR
   if ((!boundary && !interior) || (boundary && interior))
      RETURN_ERROR("avm_unstruc_read_quad: Must specify exactly one of AVM_BOUNDARY and AVM_INTERIOR");

   int quadsToRead = end-start+1;

   //handle the zero quad case gracefully
   if((start < 0 && end < 0) || quadFaces_size == 0) quadsToRead = 0;

   if (quadFaces_size != 6*quadsToRead) {
      RETURN_ERROR("avm_unstruc_read_quad: quadFaces_size does not match requested number of quads");
   }

   //FIXME: currently ignoring stride
   if (boundary) {
      if (quadsToRead > 0 && start > hdr.nBndQuadFaces) RETURN_ERROR("avm_unstruc_read_quad: start cannot be greater than nBndQuadFaces");
      if (quadsToRead > 0 && start > end) RETURN_ERROR("avm_unstruc_read_quad: start cannot be greater than end");
      if (quadsToRead > 0 && end > hdr.nBndQuadFaces) RETURN_ERROR("avm_unstruc_read_quad: end cannot be greater than nBndQuadFaces");

      if (hdr.nBndQuadFaces > 0 && quadsToRead > 0) {
         if (avm_unstruc_seek_to(avf,"bndQuads",start-1)) return 1;
         if (!fread(quadFaces, sizeof(int)*6*quadsToRead, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_read_quad: failed reading boundary quadFaces array");
         }
      }
   }
   if (interior) {
      int nInteriorFaces = hdr.nQuadFaces - hdr.nBndQuadFaces;
      if (quadsToRead > 0 && start > nInteriorFaces) RETURN_ERROR("avm_unstruc_read_quad: start cannot be greater than nQuadFaces - nBndQuadFaces");
      if (quadsToRead > 0 && start > end) RETURN_ERROR("avm_unstruc_read_quad: start cannot be greater than end");
      if (quadsToRead > 0 && end > nInteriorFaces) RETURN_ERROR("avm_unstruc_read_quad: end cannot be greater than nQuadFaces - nBndQuadFaces");

      if (nInteriorFaces > 0 && quadsToRead > 0) {
         if (avm_unstruc_seek_to(avf,"intQuads",start-1)) return 1;
         if (!fread(quadFaces, sizeof(int)*6*quadsToRead, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_read_quad: failed reading interior quadFaces array");
         }
      }
   }

   // byte swap
   if (avf->byte_swap) for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);

   return 0;
}

int rev2::avm_unstruc_read_partial_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size, int triStart, int triEnd,
   int* quadFaces, int quadFaces_size, int quadStart, int quadEnd,
   int* polyFaces, int polyFaces_size, int polyStart, int polyEnd
)
{
   if (triFaces_size>0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: triFaces is NULL");
   if (quadFaces_size>0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: quadFaces is NULL");
   if (polyFaces_size>0 && polyFaces==NULL) RETURN_ERROR("avm_unstruc_read_faces: polyFaces is NULL");

   if (polyStart > 0 || polyEnd > 0) RETURN_ERROR("avm_unstruc_read_faces: partial poly faces read is unimplemented");

   if (!avf) RETURN_ERROR("avm_unstruc_read_faces: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_faces: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_faces: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (avm_unstruc_seek_to(avf,"bndFaces")) return 1;

   int bndTriStart, bndTriEnd, bndTriSize, intTriStart, intTriEnd, intTriSize;
   int bndQuadStart, bndQuadEnd, bndQuadSize, intQuadStart, intQuadEnd, intQuadSize;
   int bndPolySize, intPolySize;

   bndTriStart = triStart - 1;
   bndTriEnd = triEnd - 1;
   if (bndTriEnd >= hdr.nBndTriFaces) bndTriEnd = hdr.nBndTriFaces - 1;
   bndTriSize = (bndTriEnd - bndTriStart + 1) * 5;
   intTriStart = triStart - hdr.nBndTriFaces - 1;
   if (intTriStart < 0) intTriStart = 0;
   intTriEnd = triEnd - hdr.nBndTriFaces - 1;
   if (intTriEnd >= hdr.nTriFaces - hdr.nBndTriFaces) bndTriEnd = hdr.nTriFaces - hdr.nBndTriFaces - 1;
   intTriSize = (intTriEnd - intTriStart + 1) * 5;
   if (bndTriSize < 0) bndTriSize = 0;
   if (intTriSize < 0) intTriSize = 0;

   bndQuadStart = quadStart - 1;
   bndQuadEnd = quadEnd - 1;
   if (bndQuadEnd >= hdr.nBndQuadFaces) bndQuadEnd = hdr.nBndQuadFaces - 1;
   bndQuadSize = (bndQuadEnd - bndQuadStart + 1) * 6;
   intQuadStart = quadStart - hdr.nBndQuadFaces - 1;
   if (intQuadStart < 0) intQuadStart = 0;
   intQuadEnd = quadEnd - hdr.nBndQuadFaces - 1;
   if (intQuadEnd >= hdr.nQuadFaces - hdr.nBndQuadFaces) bndQuadEnd = hdr.nQuadFaces - hdr.nBndQuadFaces - 1;
   intQuadSize = (intQuadEnd - intQuadStart + 1) * 6;
   if (bndQuadSize < 0) bndQuadSize = 0;
   if (intQuadSize < 0) intQuadSize = 0;

   //FIXME: partial poly reads not supported yet
   bndPolySize = intPolySize = 0;

   //handle the zero tri/quad/poly case gracefully
   if(triFaces_size == 0 && triStart < 0 && triEnd < 0) bndTriSize = intTriSize = 0;
   if(quadFaces_size == 0 && quadStart < 0 && quadEnd < 0) bndQuadSize = intQuadSize = 0;
   if(polyFaces_size == 0 && polyStart < 0 && polyEnd < 0) bndPolySize = intPolySize = 0;

// validate size of face buffers
// (this is unstruc format revision 1)
   if (triFaces_size != bndTriSize + intTriSize) {
      RETURN_ERROR("avm_unstruc_read_faces: triFaces_size does not match requested size/header");
   }
   if (quadFaces_size != bndQuadSize + intQuadSize) {
      RETURN_ERROR("avm_unstruc_read_faces: quadFaces_size does not match requested size/header");
   }
   //if (polyFaces_size != bndPolySize + intPolySize) {
   //   RETURN_ERROR("avm_unstruc_read_faces: polyFaces_size does not match requested size/header");
   //}

// Tri patch faces
   if(hdr.nBndTriFaces > 0 && bndTriStart <= hdr.nBndTriFaces && bndTriSize > 0) {
      if (avm_unstruc_seek_to(avf,"bndTris",bndTriStart)) return 1;
      if (!fread(triFaces, bndTriSize*4, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading boundary triFaces array");
      }
   }

// Quad patch faces
   if(hdr.nBndQuadFaces > 0 && bndQuadStart <= hdr.nBndQuadFaces && bndQuadSize > 0) {
      if (avm_unstruc_seek_to(avf,"bndTris",bndQuadStart)) return 1;
      if (!fread(quadFaces, bndQuadSize*4, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading boundary quadFaces array");
      }
   }

// Poly patch faces
   if(hdr.bndPolyFacesSize > 0 && bndPolySize > 0) {
      if (!fread(polyFaces, sizeof(int)*hdr.bndPolyFacesSize, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading boundary polyFaces array");
      }
   }

// Tri interior faces
   if(hdr.nTriFaces-hdr.nBndTriFaces > 0 && intTriStart <= hdr.nTriFaces-hdr.nBndTriFaces && intTriSize > 0) {
      if (avm_unstruc_seek_to(avf,"intTris",intTriStart)) return 1;
      if (!fread(triFaces+bndTriSize, intTriSize*4, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading volume triFaces array");
      }
   }

// Quad interior faces
   if(hdr.nQuadFaces-hdr.nBndQuadFaces > 0 && intQuadStart <= hdr.nQuadFaces-hdr.nBndQuadFaces && intQuadSize > 0) {
      if (avm_unstruc_seek_to(avf,"intQuads",intQuadStart)) return 1;
      if (!fread(quadFaces+bndQuadSize, intQuadSize*4, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading volume quadFaces array");
      }
   }

// Poly interior faces
   if(hdr.polyFacesSize-hdr.bndPolyFacesSize > 0 && intPolySize > 0) {
      if (!fread(polyFaces+hdr.bndPolyFacesSize, sizeof(int)*(hdr.polyFacesSize-hdr.bndPolyFacesSize), 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_faces: failed reading volume polyFaces array");
      }
   }

// byte swap
   if (avf->byte_swap) for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
   if (avf->byte_swap) for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
   if (avf->byte_swap) for (int i=0; i<polyFaces_size; ++i) byte_swap_int(polyFaces+i);

   return 0;
}

int rev2::avm_unstruc_read_bnd_faces(rev2_avmesh_file* avf,
   int* bndTriFaces,  int bndTriFaces_size,
   int* bndQuadFaces, int bndQuadFaces_size,
   int* bndPolyFaces, int bndPolyFaces_size
)
{
   if (bndTriFaces_size>0 && bndTriFaces==NULL) RETURN_ERROR("avm_unstruc_read_bnd_faces: bndTriFaces is NULL");
   if (bndQuadFaces_size>0 && bndQuadFaces==NULL) RETURN_ERROR("avm_unstruc_read_bnd_faces: bndQuadFaces is NULL");
   if (bndPolyFaces_size>0 && bndPolyFaces==NULL) RETURN_ERROR("avm_unstruc_read_bnd_faces: bndPolyFaces is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_bnd_faces: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_bnd_faces: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_bnd_faces: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of face buffers
// (this is unstruc format revision 1)
   if (bndTriFaces_size != 5*hdr.nBndTriFaces) {
      RETURN_ERROR("avm_unstruc_read_bnd_faces: bndTriFaces_size does not match header");
   }
   if (bndQuadFaces_size != 6*hdr.nBndQuadFaces) {
      RETURN_ERROR("avm_unstruc_read_bnd_faces: bndQuadFaces_size does not match header");
   }
   if (bndPolyFaces_size != hdr.bndPolyFacesSize) {
      RETURN_ERROR("avm_unstruc_read_bnd_faces: bndPolyFaces_size does not match header");
   }

   if (avm_unstruc_seek_to(avf,"bndFaces")) return 1;

// Tri patch faces
   if(hdr.nBndTriFaces > 0) {
      if (!fread(bndTriFaces, sizeof(int)*5*hdr.nBndTriFaces, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_bnd_faces: failed reading bndTriFaces array");
      }
   }

// Quad patch faces
   if(hdr.nBndQuadFaces > 0) {
      if (!fread(bndQuadFaces, sizeof(int)*6*hdr.nBndQuadFaces, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_bnd_faces: failed reading bndQuadFaces array");
      }
   }

// Poly patch faces
   if(hdr.bndPolyFacesSize > 0) {
      if (!fread(bndPolyFaces, sizeof(int)*hdr.bndPolyFacesSize, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_bnd_faces: failed reading bndPolyFaces array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<bndTriFaces_size; ++i) byte_swap_int(bndTriFaces+i);
      for (int i=0; i<bndQuadFaces_size; ++i) byte_swap_int(bndQuadFaces+i);
      for (int i=0; i<bndPolyFaces_size; ++i) byte_swap_int(bndPolyFaces+i);
   }

   return 0;
}

int rev2::avm_unstruc_read_cells(rev2_avmesh_file* avf,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size)
{
   if (!avf) RETURN_ERROR("avm_unstruc_read_cells: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_cells: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_cells: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

   return(rev2::avm_unstruc_read_partial_cells(avf,
                hexCells, hexCells_size, 1, hdr.nHexCells,
                tetCells, tetCells_size, 1, hdr.nTetCells,
                priCells, priCells_size, 1, hdr.nPriCells,
                pyrCells, pyrCells_size, 1, hdr.nPyrCells));
}

int rev2::avm_unstruc_read_partial_cells(rev2_avmesh_file* avf,
   int* hexCells, int hexCells_size, int hexStart, int hexEnd,
   int* tetCells, int tetCells_size, int tetStart, int tetEnd,
   int* priCells, int priCells_size, int priStart, int priEnd,
   int* pyrCells, int pyrCells_size, int pyrStart, int pyrEnd)
{
   if (hexCells_size>0 && hexCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: hexCells is NULL");
   if (tetCells_size>0 && tetCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: tetCells is NULL");
   if (priCells_size>0 && priCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: priCells is NULL");
   if (pyrCells_size>0 && pyrCells==NULL) RETURN_ERROR("avm_unstruc_read_partial_cells: pyrCells is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_partial_cells: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

   size_t hexesToRead = hexEnd-hexStart+1;
   size_t tetsToRead = tetEnd-tetStart+1;
   size_t prisToRead = priEnd-priStart+1;
   size_t pyrsToRead = pyrEnd-pyrStart+1;

   //handle the zero hex/tet/pri/pyr case gracefully
   if((hexStart < 0 && hexEnd < 0) || hexCells_size == 0) hexesToRead = 0;
   if((tetStart < 0 && tetEnd < 0) || tetCells_size == 0) tetsToRead = 0;
   if((priStart < 0 && priEnd < 0) || priCells_size == 0) prisToRead = 0;
   if((pyrStart < 0 && pyrEnd < 0) || pyrCells_size == 0) pyrsToRead = 0;

   if (hexCells_size != 8*hexesToRead) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: hexCells_size does not match requested number of hexes");
   }
   if (tetCells_size != 4*tetsToRead) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: tetCells_size does not match requested number of tets");
   }
   if (priCells_size != 6*prisToRead) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: priCells_size does not match requested number of pris");
   }
   if (pyrCells_size != 5*pyrsToRead) {
      RETURN_ERROR("avm_unstruc_read_partial_cells: pyrCells_size does not match requested number of pyrs");
   }

   if (hexesToRead > 0 && hexStart > hdr.nHexCells) RETURN_ERROR("avm_unstruc_read_partial_cells: hexStart cannot be greater than nHexCells");
   if (hexesToRead > 0 && hexStart > hexEnd) RETURN_ERROR("avm_unstruc_read_partial_cells: hexStart cannot be greater than hexEnd");
   if (hexesToRead > 0 && hexEnd > hdr.nHexCells) RETURN_ERROR("avm_unstruc_read_partial_cells: hexEnd cannot be greater than nHexCells");

   if (tetsToRead > 0 && tetStart > hdr.nTetCells) RETURN_ERROR("avm_unstruc_read_partial_cells: tetStart cannot be greater than nTetCells");
   if (tetsToRead > 0 && tetStart > tetEnd) RETURN_ERROR("avm_unstruc_read_partial_cells: tetStart cannot be greater than tetEnd");
   if (tetsToRead > 0 && tetEnd > hdr.nTetCells) RETURN_ERROR("avm_unstruc_read_partial_cells: tetEnd cannot be greater than nTetCells");

   if (prisToRead > 0 && priStart > hdr.nPriCells) RETURN_ERROR("avm_unstruc_read_partial_cells: priStart cannot be greater than nPriCells");
   if (prisToRead > 0 && priStart > priEnd) RETURN_ERROR("avm_unstruc_read_partial_cells: priStart cannot be greater than priEnd");
   if (prisToRead > 0 && priEnd > hdr.nPriCells) RETURN_ERROR("avm_unstruc_read_partial_cells: priEnd cannot be greater than nPriCells");

   if (pyrsToRead > 0 && pyrStart > hdr.nPyrCells) RETURN_ERROR("avm_unstruc_read_partial_cells: pyrStart cannot be greater than nPyrCells");
   if (pyrsToRead > 0 && pyrStart > pyrEnd) RETURN_ERROR("avm_unstruc_read_partial_cells: pyrStart cannot be greater than pyrEnd");
   if (pyrsToRead > 0 && pyrEnd > hdr.nPyrCells) RETURN_ERROR("avm_unstruc_read_partial_cells: pyrEnd cannot be greater than nPyrCells");

   if (avm_unstruc_seek_to(avf,"hexes",hexStart-1)) return 1;

// Hex cells
   if (hdr.nHexCells > 0 && hexesToRead > 0) {
      if (!fread(hexCells, sizeof(int)*8*hexesToRead, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_partial_cells: failed reading hexCells array");
      }
   }

   if (avm_unstruc_seek_to(avf,"tets",tetStart-1)) return 1;

// Tet cells
   if (hdr.nTetCells > 0 && tetsToRead > 0) {
      if (!fread(tetCells, sizeof(int)*4*tetsToRead, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_partial_cells: failed reading tetCells array");
      }
   }

   if (avm_unstruc_seek_to(avf,"pris",priStart-1)) return 1;

// Prism cells
   if (hdr.nPriCells > 0 && prisToRead > 0) {
      if (!fread(priCells, sizeof(int)*6*prisToRead, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_partial_cells: failed reading priCells array");
      }
   }

   if (avm_unstruc_seek_to(avf,"pyrs",pyrStart-1)) return 1;

// Pyramid cells
   if (hdr.nPyrCells > 0 && pyrsToRead > 0) {
      if (!fread(pyrCells, sizeof(int)*5*pyrsToRead, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_partial_cells: failed reading pyrCells array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<hexCells_size; ++i) byte_swap_int(hexCells+i);
      for (int i=0; i<tetCells_size; ++i) byte_swap_int(tetCells+i);
      for (int i=0; i<priCells_size; ++i) byte_swap_int(priCells+i);
      for (int i=0; i<pyrCells_size; ++i) byte_swap_int(pyrCells+i);
   }

   return 0;
}

int rev2::avm_unstruc_read_edges(rev2_avmesh_file* avf, int* edges, int edges_size)
{
   if (edges_size>0 && edges==NULL) RETURN_ERROR("avm_unstruc_read_edges: edges is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_edges: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_edges: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_edges: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of edge buffers
// (this is unstruc format revision 1)
   if (edges_size != 2*hdr.nEdges) {
      RETURN_ERROR("avm_unstruc_read_edges: edges_size does not match header");
   }

   if (avm_unstruc_seek_to(avf,"edges")) return 1;

// Read edges
   if (hdr.nEdges > 0) {
      if (!fread(edges, sizeof(int)*2*hdr.nEdges, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_edges: failed reading edges array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<edges_size; ++i) byte_swap_int(edges+i);
   }

   return 0;
}

int rev2::avm_unstruc_read_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size)
{
   if (nodesOnGeometry_size>0 && nodesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_read_amr: nodesOnGeometry is NULL");
   if (edgesOnGeometry_size>0 && edgesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_read_amr: edgesOnGeometry is NULL");
   if (facesOnGeometry_size>0 && facesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_read_amr: facesOnGeometry is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_amr: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_amr: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_amr: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of amr buffers
// (this is unstruc format revision 1)
   if (nodesOnGeometry_size != hdr.nNodesOnGeometry) {
      RETURN_ERROR("avm_unstruc_read_amr: nodesOnGeometry_size does not match header");
   }
   if (edgesOnGeometry_size != 3*hdr.nEdgesOnGeometry) {
      RETURN_ERROR("avm_unstruc_read_amr: edgesOnGeometry_size does not match header");
   }
   if (facesOnGeometry_size != 3*hdr.nFacesOnGeometry) {
      RETURN_ERROR("avm_unstruc_read_amr: facesOnGeometry_size does not match header");
   }

   if (avm_unstruc_seek_to(avf,"amr")) return 1;

// read amr
   if (hdr.nNodesOnGeometry > 0) {
      if (!fread(nodesOnGeometry, sizeof(AMR_Node_Data)*hdr.nNodesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr: failed reading nodesOnGeometry array");
      }
   }
   if (hdr.nEdgesOnGeometry > 0) {
      if (!fread(edgesOnGeometry, sizeof(int)*3*hdr.nEdgesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr: failed reading edgesOnGeometry array");
      }
   }
   if (hdr.nFacesOnGeometry > 0) {
      if (!fread(facesOnGeometry, sizeof(int)*3*hdr.nFacesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr: failed reading facesOnGeometry array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<nodesOnGeometry_size; ++i) {
         byte_swap_int(&(nodesOnGeometry[i].nodeIndex));
         byte_swap_int(&(nodesOnGeometry[i].geomType));
         byte_swap_int(&(nodesOnGeometry[i].geomTopoId));
         byte_swap_double(&(nodesOnGeometry[i].u));
         byte_swap_double(&(nodesOnGeometry[i].v));
      }
      for (int i=0; i<edgesOnGeometry_size; ++i) byte_swap_int(edgesOnGeometry+i);
      for (int i=0; i<facesOnGeometry_size; ++i) byte_swap_int(facesOnGeometry+i);
   }

   return 0;
}

int rev2::avm_unstruc_read_amr_volumeids(rev2_avmesh_file* avf,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size)
{
   if (hexGeomIds_size>0 && hexGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: hexGeomIds is NULL");
   if (tetGeomIds_size>0 && tetGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: tetGeomIds is NULL");
   if (priGeomIds_size>0 && priGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: priGeomIds is NULL");
   if (pyrGeomIds_size>0 && pyrGeomIds==NULL) RETURN_ERROR("avm_unstruc_read_amr_volumeids: pyrGeomIds is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_read_amr_volumeids: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// individual volume ID's can only be read when hdr.geomRegionId<0
   if (hdr.geomRegionId >= 0) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: geomRegionId has been globally set in header");
   }

// validate size of amr buffers
// (this is unstruc format revision 1)
   if (hexGeomIds_size != hdr.nHexCells) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: hexGeomIds_size does not match header");
   }
   if (tetGeomIds_size != hdr.nTetCells) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: tetGeomIds_size does not match header");
   }
   if (priGeomIds_size != hdr.nPriCells) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: priGeomIds_size does not match header");
   }
   if (pyrGeomIds_size != hdr.nPyrCells) {
      RETURN_ERROR("avm_unstruc_read_amr_volumeids: pyrGeomIds_size does not match header");
   }

   if (avm_unstruc_seek_to(avf,"volumeids")) return 1;

// read amr
   if (hdr.nHexCells > 0) {
      if (!fread(hexGeomIds, sizeof(int)*hexGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr_volumeids: failed reading hexGeomIds array");
      }
   }
   if (hdr.nTetCells > 0) {
      if (!fread(tetGeomIds, sizeof(int)*tetGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr_volumeids: failed reading tetGeomIds array");
      }
   }
   if (hdr.nPriCells > 0) {
      if (!fread(priGeomIds, sizeof(int)*priGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr_volumeids: failed reading priGeomIds array");
      }
   }
   if (hdr.nPyrCells > 0) {
      if (!fread(pyrGeomIds, sizeof(int)*pyrGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_read_amr_volumeids: failed reading pyrGeomIds array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<hexGeomIds_size; ++i) byte_swap_int(hexGeomIds+i);
      for (int i=0; i<tetGeomIds_size; ++i) byte_swap_int(tetGeomIds+i);
      for (int i=0; i<priGeomIds_size; ++i) byte_swap_int(priGeomIds+i);
      for (int i=0; i<pyrGeomIds_size; ++i) byte_swap_int(pyrGeomIds+i);
   }

   return 0;
}

int rev2::avm_unstruc_write_nodes_r4(rev2_avmesh_file* avf, float* xyz, int xyz_size)
{
   if (xyz_size>0 && xyz==NULL) RETURN_ERROR("avm_unstruc_write_nodes_r4: xyz is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_nodes_r4: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_nodes_r4: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_nodes_r4: selected mesh is not an unstruc mesh");
   }
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (xyz_size != 3*hdr.nNodes) {
      RETURN_ERROR("avm_unstruc_write_nodes_r4: xyz_size does not match header");
   }

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_float(xyz+i);

   if (avm_unstruc_seek_to(avf,"nodes")) return 1;

   if (hdr.nNodes > 0) {
      if (!fwrite(xyz, sizeof(float)*3*hdr.nNodes, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_nodes_r4: failed writing xyz array");
      }
   }

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_float(xyz+i);

   return 0;
}

int rev2::avm_unstruc_write_nodes_r8(rev2_avmesh_file* avf, double* xyz, int xyz_size)
{
   if (xyz_size>0 && xyz==NULL) RETURN_ERROR("avm_unstruc_write_nodes_r8: xyz is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_nodes_r8: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_nodes_r8: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_nodes_r8: selected mesh is not an unstruc mesh");
   }
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (xyz_size != 3*hdr.nNodes) {
      RETURN_ERROR("avm_unstruc_write_nodes_r8: xyz_size does not match header");
   }

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_double(xyz+i);

   if (avm_unstruc_seek_to(avf,"nodes")) return 1;

   if (hdr.nNodes > 0) {
      if (!fwrite(xyz, sizeof(double)*3*hdr.nNodes, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_nodes_r8: failed writing xyz array");
      }
   }

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_double(xyz+i);

   return 0;
}

int rev2::avm_unstruc_write_bnd_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size,
   int* polyFaces, int polyFaces_size
)
{
   if (triFaces_size>0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_write_bnd_faces: triFaces is NULL");
   if (quadFaces_size>0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_write_bnd_faces: quadFaces is NULL");
   if (polyFaces_size>0 && polyFaces==NULL) RETURN_ERROR("avm_unstruc_write_bnd_faces: polyFaces is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_bnd_faces: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_bnd_faces: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_bnd_faces: selected mesh is not an unstruc mesh");
   }
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   avf->unstruc[meshid].faces_reordering.resize(hdr.nTriFaces+hdr.nQuadFaces+hdr.nPolyFaces);
   int triFacesIndex = 0;
   int quadFacesIndex = 0;
   int polyFacesIndex = 0;

// validate size of face buffers
// (this is unstruc format revision 1)
   if (triFaces_size != 5*hdr.nTriFaces) {
      RETURN_ERROR("avm_unstruc_write_bnd_faces: triFaces_size does not match header");
   }
   if (quadFaces_size != 6*hdr.nQuadFaces) {
      RETURN_ERROR("avm_unstruc_write_bnd_faces: quadFaces_size does not match header");
   }
   if (polyFaces_size != hdr.polyFacesSize) {
      RETURN_ERROR("avm_unstruc_write_bnd_faces: polyFaces_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
   }

   if (avm_unstruc_seek_to(avf,"bndFaces")) return 1;

// Tri patch faces
   for (int i=0; i<hdr.nTriFaces; ++i) {
      if (triFaces[(i*5)+4] < 0) {
         avf->unstruc[meshid].faces_reordering[i] = triFacesIndex++;
         if (!fwrite(triFaces+(i*5), sizeof(int)*5, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_bnd_faces: failed writing triFaces array");
         }
      }
   }

// Quad patch faces
   for (int i=0; i<hdr.nQuadFaces; ++i) {
      if (quadFaces[(i*6)+5] < 0) {
         avf->unstruc[meshid].faces_reordering[hdr.nTriFaces+i] = hdr.nTriFaces + quadFacesIndex++;
         if (!fwrite(quadFaces+(i*6), sizeof(int)*6, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_bnd_faces: failed writing quadFaces array");
         }
      }
   }

// Poly patch faces
   int j=0;
   int rightCellIndex = 0;
   int nodesInFace = 0;
   for (int i=0; i<hdr.nPolyFaces; ++i) {
      nodesInFace = polyFaces[j];
      if(nodesInFace<1) return 1; //nNodesInFace must be > 0
      rightCellIndex = j + nodesInFace + 2;
      if (polyFaces[rightCellIndex] < 0) {
         avf->unstruc[meshid].faces_reordering[hdr.nTriFaces+hdr.nQuadFaces+i] = hdr.nTriFaces + hdr.nQuadFaces + polyFacesIndex++;
         if (avf->byte_swap) for (int k=j; k<j+nodesInFace+3; ++k) byte_swap_int(polyFaces+k);
         if (!fwrite(&polyFaces[j], sizeof(int)*(nodesInFace+3), 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_bnd_faces: failed writing polyFaces array");
         }
         if (avf->byte_swap) for (int k=j; k<j+nodesInFace+3; ++k) byte_swap_int(polyFaces+k);
      }
      j += nodesInFace + 3;
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
   }

   return 0;
}

int rev2::avm_unstruc_write_faces(rev2_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size,
   int* polyFaces, int polyFaces_size
)
{
   if (triFaces_size>0 && triFaces==NULL) RETURN_ERROR("avm_unstruc_write_faces: triFaces is NULL");
   if (quadFaces_size>0 && quadFaces==NULL) RETURN_ERROR("avm_unstruc_write_faces: quadFaces is NULL");
   if (polyFaces_size>0 && polyFaces==NULL) RETURN_ERROR("avm_unstruc_write_faces: polyFaces is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_faces: avf is NULL");

   int meshid = avf->selection_meshid-1; 
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_faces: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_faces: selected mesh is not an unstruc mesh");
   }
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   avf->unstruc[meshid].faces_reordering.resize(hdr.nTriFaces+hdr.nQuadFaces+hdr.nPolyFaces);
   int triFacesIndex = 0;
   int quadFacesIndex = 0;
   int polyFacesIndex = 0;

// validate size of face buffers
// (this is unstruc format revision 1)
   if (triFaces_size != 5*hdr.nTriFaces) {
      RETURN_ERROR("avm_unstruc_write_faces: triFaces_size does not match header");
   }
   if (quadFaces_size != 6*hdr.nQuadFaces) {
      RETURN_ERROR("avm_unstruc_write_faces: quadFaces_size does not match header");
   }
   if (polyFaces_size != hdr.polyFacesSize) {
      RETURN_ERROR("avm_unstruc_write_faces: polyFaces_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
   }

   if (avm_unstruc_seek_to(avf,"bndFaces")) return 1;

// Tri patch faces
   for (int i=0; i<hdr.nTriFaces; ++i) {
      if (triFaces[(i*5)+4] < 0) {
         avf->unstruc[meshid].faces_reordering[i] = triFacesIndex++;
         if (!fwrite(triFaces+(i*5), sizeof(int)*5, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_faces: failed writing patch triFaces array");
         }
      }
   }

// Quad patch faces
   for (int i=0; i<hdr.nQuadFaces; ++i) {
      if (quadFaces[(i*6)+5] < 0) {
         avf->unstruc[meshid].faces_reordering[hdr.nTriFaces+i] = hdr.nTriFaces + quadFacesIndex++;
         if (!fwrite(quadFaces+(i*6), sizeof(int)*6, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_faces: failed writing patch quadFaces array");
         }
      }
   }

// Poly patch faces
   int j=0;
   int rightCellIndex = 0;
   int nodesInFace = 0;
   for (int i=0; i<hdr.nPolyFaces; ++i) {
      nodesInFace = polyFaces[j];
      if(nodesInFace<1) { //nNodesInFace must be > 0
         RETURN_ERROR("avm_unstruc_write_faces: can't write a polyFace with nodesInFace < 1");
      }
      rightCellIndex = j + nodesInFace + 2;
      if (polyFaces[rightCellIndex] < 0) {
         avf->unstruc[meshid].faces_reordering[hdr.nTriFaces+hdr.nQuadFaces+i] = hdr.nTriFaces + hdr.nQuadFaces + polyFacesIndex++;
         if (avf->byte_swap) for (int k=j; k<j+nodesInFace+3; ++k) byte_swap_int(polyFaces+k);
         if (!fwrite(&polyFaces[j], sizeof(int)*(nodesInFace+3), 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_faces: failed writing patch polyFaces array");
         }
         if (avf->byte_swap) for (int k=j; k<j+nodesInFace+3; ++k) byte_swap_int(polyFaces+k);
      }
      j += nodesInFace + 3;
   }

// Tri interior faces
   for (int i=0; i<hdr.nTriFaces; ++i) {
      if (triFaces[(i*5)+4] > 0) {
         avf->unstruc[meshid].faces_reordering[i] = triFacesIndex++;
         if (!fwrite(triFaces+(i*5), sizeof(int)*5, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_faces: failed writing interior triFaces array");
         }
      }
   }

// Quad interior faces
   for (int i=0; i<hdr.nQuadFaces; ++i) {
      if (quadFaces[(i*6)+5] > 0) {
         avf->unstruc[meshid].faces_reordering[hdr.nTriFaces+i] = hdr.nTriFaces + quadFacesIndex++;
         if (!fwrite(quadFaces+(i*6), sizeof(int)*6, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_faces: failed writing interior quadFaces array");
         }
      }
   }

// Poly interior faces
   j=0;
   rightCellIndex = 0;
   nodesInFace = 0;
   for (int i=0; i<hdr.nPolyFaces; ++i) {
      nodesInFace = polyFaces[j];
      if(nodesInFace<1) { //nNodesInFace must be > 0
         RETURN_ERROR("avm_unstruc_write_faces: can't write a polyFace with nodesInFace < 1");
      }
      rightCellIndex = j + nodesInFace + 2;
      if (polyFaces[rightCellIndex] > 0) {
         avf->unstruc[meshid].faces_reordering[hdr.nTriFaces+hdr.nQuadFaces+i] = hdr.nTriFaces + hdr.nQuadFaces + polyFacesIndex++;
         if (avf->byte_swap) for (int i=j; i<j+nodesInFace+3; ++i) byte_swap_int(polyFaces+i);
         if (!fwrite(&polyFaces[j], sizeof(int)*(nodesInFace+3), 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_faces: failed writing interior polyFaces array");
         }
         if (avf->byte_swap) for (int i=j; i<j+nodesInFace+3; ++i) byte_swap_int(polyFaces+i);
      }
      j += nodesInFace + 3;
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
      for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);
   }

   return 0;
}

int rev2::avm_unstruc_write_cells(rev2_avmesh_file* avf,
   int* hexCells, int hexCells_size,
   int* tetCells, int tetCells_size,
   int* priCells, int priCells_size,
   int* pyrCells, int pyrCells_size)
{
   if (hexCells_size>0 && hexCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: hexCells is NULL");
   if (tetCells_size>0 && tetCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: tetCells is NULL");
   if (priCells_size>0 && priCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: priCells is NULL");
   if (pyrCells_size>0 && pyrCells==NULL) RETURN_ERROR("avm_unstruc_write_cells: pyrCells is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_cells: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_cells: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_cells: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of cell buffers
// (this is unstruc format revision 1)
   if (hexCells_size != 8*hdr.nHexCells) {
      RETURN_ERROR("avm_unstruc_write_cells: hexCells_size does not match header");
   }
   if (tetCells_size != 4*hdr.nTetCells) {
      RETURN_ERROR("avm_unstruc_write_cells: tetCells_size does not match header");
   }
   if (priCells_size != 6*hdr.nPriCells) {
      RETURN_ERROR("avm_unstruc_write_cells: priCells_size does not match header");
   }
   if (pyrCells_size != 5*hdr.nPyrCells) {
      RETURN_ERROR("avm_unstruc_write_cells: pyrCells_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<hexCells_size; ++i) byte_swap_int(hexCells+i);
      for (int i=0; i<tetCells_size; ++i) byte_swap_int(tetCells+i);
      for (int i=0; i<priCells_size; ++i) byte_swap_int(priCells+i);
      for (int i=0; i<pyrCells_size; ++i) byte_swap_int(pyrCells+i);
   }

   if (avm_unstruc_seek_to(avf,"cells")) return 1;

// Hex cells
   if (hdr.nHexCells > 0) {
      if (!fwrite(hexCells, sizeof(int)*hexCells_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_cells: failed writing hexCells array");
      }
   }

// Tet cells
   if (hdr.nTetCells > 0) {
      if (!fwrite(tetCells, sizeof(int)*tetCells_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_cells: failed writing tetCells array");
      }
   }

// Prism cells
   if (hdr.nPriCells > 0) {
      if (!fwrite(priCells, sizeof(int)*priCells_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_cells: failed writing priCells array");
      }
   }

// Pyramid cells
   if (hdr.nPyrCells > 0) {
      if (!fwrite(pyrCells, sizeof(int)*pyrCells_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_cells: failed writing pyrCells array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<hexCells_size; ++i) byte_swap_int(hexCells+i);
      for (int i=0; i<tetCells_size; ++i) byte_swap_int(tetCells+i);
      for (int i=0; i<priCells_size; ++i) byte_swap_int(priCells+i);
      for (int i=0; i<pyrCells_size; ++i) byte_swap_int(pyrCells+i);
   }

   return 0;
}

int rev2::avm_unstruc_write_edges(rev2_avmesh_file* avf, int* edges, int edges_size)
{
   if (edges_size>0 && edges==NULL) RETURN_ERROR("avm_unstruc_write_edges: edges is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_edges: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_edges: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_edges: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of edge buffers
// (this is unstruc format revision 1)
   if (edges_size != 2*hdr.nEdges) {
      RETURN_ERROR("avm_unstruc_write_edges: edges_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<edges_size; ++i) byte_swap_int(edges+i);
   }

   if (avm_unstruc_seek_to(avf,"edges")) return 1;

// Write edges
   if (hdr.nEdges > 0 ) {
      if (!fwrite(edges, sizeof(int)*2*hdr.nEdges, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_edges: failed writing edges array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<edges_size; ++i) byte_swap_int(edges+i);
   }

   return 0;
}

int rev2::avm_unstruc_write_amr(rev2_avmesh_file* avf,
   AMR_Node_Data* nodesOnGeometry, int nodesOnGeometry_size,
   int* edgesOnGeometry, int edgesOnGeometry_size,
   int* facesOnGeometry, int facesOnGeometry_size)
{
   if (nodesOnGeometry_size>0 && nodesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_write_amr: nodesOnGeometry is NULL");
   if (edgesOnGeometry_size>0 && edgesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_write_amr: edgesOnGeometry is NULL");
   if (facesOnGeometry_size>0 && facesOnGeometry==NULL) RETURN_ERROR("avm_unstruc_write_amr: facesOnGeometry is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_amr: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_amr: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_amr: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of amr buffers
// (this is unstruc format revision 1)
   if (nodesOnGeometry_size != hdr.nNodesOnGeometry) {
      RETURN_ERROR("avm_unstruc_write_amr: nodesOnGeometry_size does not match header");
   }
   if (edgesOnGeometry_size != 3*hdr.nEdgesOnGeometry) {
      RETURN_ERROR("avm_unstruc_write_amr: edgesOnGeometry_size does not match header");
   }
   if (facesOnGeometry_size != 3*hdr.nFacesOnGeometry) {
      RETURN_ERROR("avm_unstruc_write_amr: facesOnGeometry_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<nodesOnGeometry_size; ++i) {
         byte_swap_int(&(nodesOnGeometry[i].nodeIndex));
         byte_swap_int(&(nodesOnGeometry[i].geomType));
         byte_swap_int(&(nodesOnGeometry[i].geomTopoId));
         byte_swap_double(&(nodesOnGeometry[i].u));
         byte_swap_double(&(nodesOnGeometry[i].v));
      }
      for (int i=0; i<edgesOnGeometry_size; ++i) byte_swap_int(edgesOnGeometry+i);
   }

   if (avm_unstruc_seek_to(avf,"amr")) return 1;

// Write amr
   if (hdr.nNodesOnGeometry > 0) {
      if (!fwrite(nodesOnGeometry, sizeof(AMR_Node_Data)*hdr.nNodesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_amr: failed writing nodesOnGeometry array");
      }
   }
   if (hdr.nEdgesOnGeometry > 0) {
      if (!fwrite(edgesOnGeometry, sizeof(int)*3*hdr.nEdgesOnGeometry, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_amr: failed writing edgesOnGeometry array");
      }
   }
   if (hdr.nFacesOnGeometry > 0) {
      if(avf->unstruc[meshid].faces_reordering.size() > 0) {
         int reorderedFace;
         for(int i=0; i<hdr.nFacesOnGeometry; ++i) {
            reorderedFace = avf->unstruc[meshid].faces_reordering[facesOnGeometry[i*3]-1]+1;
            if (avf->byte_swap) {
               byte_swap_int(&reorderedFace);
               byte_swap_int(&facesOnGeometry[(i*3)+1]);
               byte_swap_int(&facesOnGeometry[(i*3)+2]);
            }
            if (!fwrite(&reorderedFace, sizeof(int), 1, avf->fp)) {
               RETURN_ERROR("avm_unstruc_write_amr: failed writing facesOnGeometry array");
            }
            if (!fwrite(&facesOnGeometry[(i*3)+1], sizeof(int), 1, avf->fp)) {
               RETURN_ERROR("avm_unstruc_write_amr: failed writing facesOnGeometry array");
            }
            if (!fwrite(&facesOnGeometry[(i*3)+2], sizeof(int), 1, avf->fp)) {
               RETURN_ERROR("avm_unstruc_write_amr: failed writing facesOnGeometry array");
            }
            if (avf->byte_swap) {
               byte_swap_int(&reorderedFace);
               byte_swap_int(&facesOnGeometry[(i*3)+1]);
               byte_swap_int(&facesOnGeometry[(i*3)+2]);
            }
          }
      }
      else {
         if (avf->byte_swap) for (int i=0; i<facesOnGeometry_size; ++i) byte_swap_int(facesOnGeometry+i);
         if (!fwrite(facesOnGeometry, sizeof(int)*3*hdr.nFacesOnGeometry, 1, avf->fp)) {
            RETURN_ERROR("avm_unstruc_write_amr: failed writing facesOnGeometry array");
         }
         if (avf->byte_swap) for (int i=0; i<facesOnGeometry_size; ++i) byte_swap_int(facesOnGeometry+i);
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<nodesOnGeometry_size; ++i) {
         byte_swap_int(&(nodesOnGeometry[i].nodeIndex));
         byte_swap_int(&(nodesOnGeometry[i].geomType));
         byte_swap_int(&(nodesOnGeometry[i].geomTopoId));
         byte_swap_double(&(nodesOnGeometry[i].u));
         byte_swap_double(&(nodesOnGeometry[i].v));
      }
      for (int i=0; i<edgesOnGeometry_size; ++i) byte_swap_int(edgesOnGeometry+i);
   }

   return 0;
}

int rev2::avm_unstruc_write_amr_volumeids(rev2_avmesh_file* avf,
   int* hexGeomIds, int hexGeomIds_size,
   int* tetGeomIds, int tetGeomIds_size,
   int* priGeomIds, int priGeomIds_size,
   int* pyrGeomIds, int pyrGeomIds_size)
{
   if (hexGeomIds_size>0 && hexGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: hexGeomIds is NULL");
   if (tetGeomIds_size>0 && tetGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: tetGeomIds is NULL");
   if (priGeomIds_size>0 && priGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: priGeomIds is NULL");
   if (pyrGeomIds_size>0 && pyrGeomIds==NULL) RETURN_ERROR("avm_unstruc_write_amr_volumeids: pyrGeomIds is NULL");

   if (!avf) RETURN_ERROR("avm_unstruc_write_amr_volumeids: avf is NULL");

   int meshid = avf->selection_meshid-1;
   if (meshid < 0 || meshid >= avf->file_hdr.meshCount) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: invalid meshid");
   }
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: selected mesh is not an unstruc mesh");
   }

   const unstruc_header& hdr = avf->unstruc[meshid].header;

// individual volume ID's can only be written when hdr.geomRegionId<0
   if (hdr.geomRegionId >= 0) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: geomRegionId has been globally set in header");
   }

// validate size of amr buffers
// (this is unstruc format revision 1)
   if (hexGeomIds_size != hdr.nHexCells) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: hexGeomIds_size does not match header");
   }
   if (tetGeomIds_size != hdr.nTetCells) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: tetGeomIds_size does not match header");
   }
   if (priGeomIds_size != hdr.nPriCells) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: priGeomIds_size does not match header");
   }
   if (pyrGeomIds_size != hdr.nPyrCells) {
      RETURN_ERROR("avm_unstruc_write_amr_volumeids: pyrGeomIds_size does not match header");
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<hexGeomIds_size; ++i) byte_swap_int(hexGeomIds+i);
      for (int i=0; i<tetGeomIds_size; ++i) byte_swap_int(tetGeomIds+i);
      for (int i=0; i<priGeomIds_size; ++i) byte_swap_int(priGeomIds+i);
      for (int i=0; i<pyrGeomIds_size; ++i) byte_swap_int(pyrGeomIds+i);
   }

   if (avm_unstruc_seek_to(avf,"volumeids")) return 1;

// Write amr
   if (hdr.nHexCells > 0) {
      if (!fwrite(hexGeomIds, sizeof(int)*hexGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_amr_volumeids: failed writing hexGeomIds array");
      }
   }
   if (hdr.nTetCells > 0) {
      if (!fwrite(tetGeomIds, sizeof(int)*tetGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_amr_volumeids: failed writing tetGeomIds array");
      }
   }
   if (hdr.nPriCells > 0) {
      if (!fwrite(priGeomIds, sizeof(int)*priGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_amr_volumeids: failed writing priGeomIds array");
      }
   }
   if (hdr.nPyrCells > 0) {
      if (!fwrite(pyrGeomIds, sizeof(int)*pyrGeomIds_size, 1, avf->fp)) {
         RETURN_ERROR("avm_unstruc_write_amr_volumeids: failed writing pyrGeomIds array");
      }
   }

// byte swap
   if (avf->byte_swap) {
      for (int i=0; i<hexGeomIds_size; ++i) byte_swap_int(hexGeomIds+i);
      for (int i=0; i<tetGeomIds_size; ++i) byte_swap_int(tetGeomIds+i);
      for (int i=0; i<priGeomIds_size; ++i) byte_swap_int(priGeomIds+i);
      for (int i=0; i<pyrGeomIds_size; ++i) byte_swap_int(pyrGeomIds+i);
   }

   return 0;
}


