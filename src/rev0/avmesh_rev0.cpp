#include <map>
#include <string>
#include <vector> 

#include <stdio.h>
#include <string.h>

#include "avmhdrs.h"
#include "../avmesh.h"
#include "../avmesh_file.h"
#include "avmesh_rev0.h"

using namespace std;
using namespace rev0;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

#define CHECK_PTR(v) if(NULL==v) return 1;

/**
@author
 Josh Calahan
@history
 May 26, 2010 - Initial creation
*/
int rev0::avm_open(rev0_avmesh_file* avf, int* id)
{
#define CHECK_RETURN(v) if(v) { return 1; }
   CHECK_PTR(id);

   if (!avf) return 1;

   avf->selection_meshid = 0;
   avf->selection_patchid = 1;
   avf->selection_patchparam = 1;
   avf->selection_surfpatchid = 1;
   avf->selection_edgepatchid = 1;
   avf->selection_levelid = 1;
   avf->selection_blockid = 1;
   avf->write_endian = rev0_avmesh_file::ENDIAN_NATIVE; 
   CHECK_RETURN(NULL==avf->fp);

   // read the first part of the header
   CHECK_RETURN(
      !file_header_t::read(
         avf->fp, 
         avf->byte_swap, 
         &avf->file_hdr)
   );

   // read the variable length description record
   if (avf->file_hdr.descriptionSize>0) {
      char* description = new char[avf->file_hdr.descriptionSize];
      if (!description) return 1;
      if (!fread(description, sizeof(char)*avf->file_hdr.descriptionSize, 1, avf->fp)) return 1;
      avf->description = string(description, description+avf->file_hdr.descriptionSize);
      delete []description;
   }

   return 0;
#undef CHECK_RETURN
}

/**
@author
 Josh Calahan
@history
 May 26, 2010 - Initial creation
*/
int rev0::avm_read_mesh_headers(rev0_avmesh_file* avf)
{
   if (!avf) return 1;

   avf->mesh_hdrs.resize( avf->file_hdr.meshCount );
   avf->mesh_descriptions.resize( avf->file_hdr.meshCount );
   avf->bfstruc.resize( avf->file_hdr.meshCount ); // TODO: only allocate if needed
   avf->strand.resize( avf->file_hdr.meshCount ); // TODO: only allocate if needed
   avf->unstruc.resize( avf->file_hdr.meshCount ); // TODO: only allocate if needed
   avf->cart.resize( avf->file_hdr.meshCount ); // TODO: only allocate if needed

   for (int i=0; i<avf->file_hdr.meshCount; ++i)
   {
      // read the generic mesh information

      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      if(!mesh_header_t::read(avf->fp, avf->byte_swap, &mesh_hdr)) return 1;

      // read the mesh description

      if (mesh_hdr.descriptionSize>0) {
         char* description = new char[mesh_hdr.descriptionSize];
         if (!description) return 1;
         if (!fread(description, sizeof(char)*mesh_hdr.descriptionSize, 1, avf->fp)) return 1;
         avf->mesh_descriptions[i] = string(description, description+mesh_hdr.descriptionSize);
         delete []description;
      }

      // read the specific mesh information

      if (0==strncmp("bfstruc", mesh_hdr.meshType, strlen("bfstruc"))) {
         if(!bfstruc_info::read(avf->fp, avf->byte_swap, &avf->bfstruc[i])) return 1;
      } else if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         if(!strand_info::read(avf->fp, avf->byte_swap, &avf->strand[i])) return 1;
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         if(!unstruc_info::read(avf->fp, avf->byte_swap, &avf->unstruc[i])) return 1;
      } else if (0==strncmp("cart", mesh_hdr.meshType, strlen("cart"))) {
         if(!cart_info::read(avf->fp, avf->byte_swap, &avf->cart[i])) return 1;
      } else return 1;
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_new_file(rev0_avmesh_file* avf, const char* filename)
{
#define CHECK_RETURN(v) if(v) { return 1; }
   CHECK_PTR(filename);

   if (!avf) return 1;

   avf->selection_meshid = 0;
   avf->selection_patchid = 1;
   avf->selection_patchparam = 1;
   avf->selection_surfpatchid = 1;
   avf->selection_edgepatchid = 1;
   avf->selection_levelid = 1;
   avf->selection_blockid = 1;
   avf->write_endian = rev0_avmesh_file::ENDIAN_NATIVE; 
   avf->filename = filename;
   avf->fp = fopen(filename, "wb");
   CHECK_RETURN(NULL==avf->fp);
   return 0;
#undef CHECK_RETURN
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_write_headers(rev0_avmesh_file* avf)
{
#define CHECK_RETURN(v) if(v) { return 1; }
   if (!avf) return 1;

   if (avf->write_endian == rev0_avmesh_file::ENDIAN_NATIVE) avf->byte_swap = false;
   else {
      int one = 1;
      const char* p1 = (char*)&one;
      rev0_avmesh_file::Endian machine_endian = p1[0] ? rev0_avmesh_file::ENDIAN_LITTLE :
                                                   rev0_avmesh_file::ENDIAN_BIG;
      avf->byte_swap = avf->write_endian != machine_endian;
   }

   CHECK_RETURN(!file_id_prefix::write(avf->fp, avf->byte_swap, &avf->file_prefix));
   CHECK_RETURN(!file_header::write(avf->fp, avf->byte_swap, &avf->file_hdr));
   if (avf->file_hdr.descriptionSize>0) {
      CHECK_RETURN(!fwrite(avf->description.c_str(), avf->description.size(), 1, avf->fp));
   }

   for (int i=0; i<avf->file_hdr.meshCount; ++i)
   {
      // write the generic mesh information

      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      CHECK_RETURN(!mesh_header_t::write(avf->fp, avf->byte_swap, &mesh_hdr));

      // write the mesh description

      if (mesh_hdr.descriptionSize>0) {
         const string& description = avf->mesh_descriptions[i];
         CHECK_RETURN(!fwrite(description.c_str(), description.size(), 1, avf->fp));
      }

      // write the specific mesh information

      if (0==strncmp("bfstruc", mesh_hdr.meshType, strlen("bfstruc"))) {
         CHECK_RETURN(!bfstruc_info::write(avf->fp, avf->byte_swap, &avf->bfstruc[i]));
      } else if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         CHECK_RETURN(!strand_info::write(avf->fp, avf->byte_swap, &avf->strand[i]));
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         CHECK_RETURN(!unstruc_info::write(avf->fp, avf->byte_swap, &avf->unstruc[i]));
      } else if (0==strncmp("cart", mesh_hdr.meshType, strlen("cart"))) {
         CHECK_RETURN(!cart_info::write(avf->fp, avf->byte_swap, &avf->cart[i]));
      } else return 1;
   }

   return 0;
#undef CHECK_RETURN
}

/**
@author
 Josh Calahan
@history
 December 15, 2010 - Initial creation
*/
int rev0::avm_select(rev0_avmesh_file* avf, const char* section, int id)
{
   CHECK_PTR(section);
   if (!avf) return 1;
        if (0==strcmp("header",section)) avf->selection_meshid = 0;
   else if (0==strcmp("mesh",section)) avf->selection_meshid = id;
   else if (0==strcmp("patch",section)) avf->selection_patchid = id;
   else if (0==strcmp("patchParam",section)) avf->selection_patchparam = id;
   else if (0==strcmp("surfPatch",section)) avf->selection_surfpatchid = id;
   else if (0==strcmp("edgePatch",section)) avf->selection_edgepatchid = id;
   else if (0==strcmp("level",section)) avf->selection_levelid = id;
   else if (0==strcmp("block",section)) avf->selection_blockid = id;
   return 0;
}

/**
@author
 Josh Calahan
@history
 December 15, 2010 - Initial creation
*/
int rev0::avm_get_int(rev0_avmesh_file* avf, const char* field, int* value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANWSER(a) {*value=a; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   CHECK_PTR(value);
   if (!avf) return 1;

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("formatRevision")) RETURN_ANWSER(avf->file_prefix.formatRevision)
      if (FIELD_EQ("meshCount")) RETURN_ANWSER(avf->file_hdr.meshCount)
      if (FIELD_EQ("precision")) RETURN_ANWSER(avf->file_hdr.precision)
      if (FIELD_EQ("dimensions")) RETURN_ANWSER(avf->file_hdr.dimensions)
      if (FIELD_EQ("descriptionSize")) RETURN_ANWSER(avf->description.size()+1) // (account for null-terminator)
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int patchparam = avf->selection_patchparam-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 
   int levelid    = avf->selection_levelid-1; 
   int blockid    = avf->selection_blockid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("refinementLevel")) RETURN_ANWSER(avf->mesh_hdrs[meshid].refinementLevel)
      if (FIELD_EQ("descriptionSize")) RETURN_ANWSER(avf->mesh_descriptions[meshid].size()+1) // (account for null-terminator)

      bool bfstruc_mesh = 0==strncmp("bfstruc", avf->mesh_hdrs[meshid].meshType, strlen("bfstruc"));
      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));
      bool cart_mesh    = 0==strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"));

      if (bfstruc_mesh) {
         if (FIELD_EQ("jmax")) RETURN_ANWSER(avf->bfstruc[meshid].header.jmax)
         if (FIELD_EQ("kmax")) RETURN_ANWSER(avf->bfstruc[meshid].header.kmax)
         if (FIELD_EQ("lmax")) RETURN_ANWSER(avf->bfstruc[meshid].header.lmax)
         if (FIELD_EQ("iblank")) RETURN_ANWSER(avf->bfstruc[meshid].header.iblank)
         if (FIELD_EQ("nPatches")) RETURN_ANWSER(avf->bfstruc[meshid].header.nPatches)
         if (FIELD_EQ("nPatchIntParams")) RETURN_ANWSER(avf->bfstruc[meshid].header.nPatchIntParams)
         if (FIELD_EQ("nPatchR8Params")) RETURN_ANWSER(avf->bfstruc[meshid].header.nPatchR8Params)
         if (FIELD_EQ("nPatchC80Params")) RETURN_ANWSER(avf->bfstruc[meshid].header.nPatchC80Params)

         if (0 <= patchid && patchid < avf->bfstruc[meshid].header.nPatches) {
            if (FIELD_EQ("bctype")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].bctype)
            if (FIELD_EQ("direction")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].direction)
            if (FIELD_EQ("jbegin")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].jbegin)
            if (FIELD_EQ("jend")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].jend)
            if (FIELD_EQ("kbegin")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].kbegin)
            if (FIELD_EQ("kend")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].kend)
            if (FIELD_EQ("lbegin")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].lbegin)
            if (FIELD_EQ("lend")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].lend)

            if (0 <= patchparam && patchparam < avf->bfstruc[meshid].header.nPatchIntParams) {
               if (FIELD_EQ("patch_int_param"))
                  RETURN_ANWSER(avf->bfstruc[meshid].patchIntParams[patchid][patchparam])
            }
         }
      }

      if (strand_mesh) {
         if (FIELD_EQ("surfaceOnly")) RETURN_ANWSER(avf->strand[meshid].header.surfaceOnly)
         if (FIELD_EQ("nSurfNodes")) RETURN_ANWSER(avf->strand[meshid].header.nSurfNodes)
         if (FIELD_EQ("nTriFaces")) RETURN_ANWSER(avf->strand[meshid].header.nTriFaces)
         if (FIELD_EQ("nQuadFaces")) RETURN_ANWSER(avf->strand[meshid].header.nQuadFaces)
         if (FIELD_EQ("nBndEdges")) RETURN_ANWSER(avf->strand[meshid].header.nBndEdges)
         if (FIELD_EQ("nPtsPerStrand")) RETURN_ANWSER(avf->strand[meshid].header.nPtsPerStrand)
         if (FIELD_EQ("nSurfPatches")) RETURN_ANWSER(avf->strand[meshid].header.nSurfPatches)
         if (FIELD_EQ("nEdgePatches")) RETURN_ANWSER(avf->strand[meshid].header.nEdgePatches)
         if (FIELD_EQ("surfPatchId"))
            if (0 <= surfpatchi && surfpatchi < avf->strand[meshid].header.nSurfPatches) 
               RETURN_ANWSER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchId)
         if (FIELD_EQ("edgePatchId"))
            if (0 <= edgepatchi && edgepatchi < avf->strand[meshid].header.nEdgePatches) 
               RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchId)
      }

      if (unstruc_mesh) {
         if (FIELD_EQ("fullViscousLayerCount")) RETURN_ANWSER(avf->unstruc[meshid].header.fullViscousLayerCount)
         if (FIELD_EQ("partialViscousLayerCount")) RETURN_ANWSER(avf->unstruc[meshid].header.partialViscousLayerCount)
         if (FIELD_EQ("nNodes")) RETURN_ANWSER(avf->unstruc[meshid].header.nNodes)
         if (FIELD_EQ("nEdges")) RETURN_ANWSER(avf->unstruc[meshid].header.nEdges)
         if (FIELD_EQ("nFaces")) RETURN_ANWSER(avf->unstruc[meshid].header.nFaces)
         if (FIELD_EQ("nMaxNodesPerFace")) RETURN_ANWSER(avf->unstruc[meshid].header.nMaxNodesPerFace)
         if (FIELD_EQ("nTriFaces")) RETURN_ANWSER(avf->unstruc[meshid].header.nTriFaces)
         if (FIELD_EQ("nQuadFaces")) RETURN_ANWSER(avf->unstruc[meshid].header.nQuadFaces)
         if (FIELD_EQ("nBndTriFaces")) RETURN_ANWSER(avf->unstruc[meshid].header.nBndTriFaces)
         if (FIELD_EQ("nBndQuadFaces")) RETURN_ANWSER(avf->unstruc[meshid].header.nBndQuadFaces)
         if (FIELD_EQ("nCells")) RETURN_ANWSER(avf->unstruc[meshid].header.nCells)
         if (FIELD_EQ("nMaxNodesPerCell")) RETURN_ANWSER(avf->unstruc[meshid].header.nMaxNodesPerCell)
         if (FIELD_EQ("nMaxFacesPerCell")) RETURN_ANWSER(avf->unstruc[meshid].header.nMaxFacesPerCell)
         if (FIELD_EQ("nHexCells")) RETURN_ANWSER(avf->unstruc[meshid].header.nHexCells)
         if (FIELD_EQ("nTetCells")) RETURN_ANWSER(avf->unstruc[meshid].header.nTetCells)
         if (FIELD_EQ("nPriCells")) RETURN_ANWSER(avf->unstruc[meshid].header.nPriCells)
         if (FIELD_EQ("nPyrCells")) RETURN_ANWSER(avf->unstruc[meshid].header.nPyrCells)
         if (FIELD_EQ("nPatches")) RETURN_ANWSER(avf->unstruc[meshid].header.nPatches)
         if (FIELD_EQ("nPatchNodes")) RETURN_ANWSER(avf->unstruc[meshid].header.nPatchNodes)
         if (FIELD_EQ("nPatchFaces")) RETURN_ANWSER(avf->unstruc[meshid].header.nPatchFaces)

         if (0 <= patchid && patchid < avf->unstruc[meshid].header.nPatches) {
            if (FIELD_EQ("patchId")) RETURN_ANWSER(avf->unstruc[meshid].patches[patchid].patchId)
         }
      }

      if (cart_mesh) {
         if (FIELD_EQ("nLevels")) RETURN_ANWSER(avf->cart[meshid].header.nLevels)
         if (FIELD_EQ("nBlocks")) RETURN_ANWSER(avf->cart[meshid].header.nBlocks)
         if (FIELD_EQ("nFringe")) RETURN_ANWSER(avf->cart[meshid].header.nFringe)
         if (FIELD_EQ("nxc")) RETURN_ANWSER(avf->cart[meshid].header.nxc)
         if (FIELD_EQ("nyc")) RETURN_ANWSER(avf->cart[meshid].header.nyc)
         if (FIELD_EQ("nzc")) RETURN_ANWSER(avf->cart[meshid].header.nzc)

         if (FIELD_EQ("iRatio"))
            if (0 <= levelid && levelid < avf->cart[meshid].header.nLevels)
               RETURN_ANWSER(avf->cart[meshid].ratios[levelid])

         if (0 <= blockid && blockid < avf->cart[meshid].header.nBlocks) {
            if (FIELD_EQ("levNum")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].levNum)
            if (FIELD_EQ("iblFlag")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].iblFlag)
            if (FIELD_EQ("bdryFlag")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].bdryFlag)
         }
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef RETURN_ANWSER
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 December 15, 2010 - Initial creation
*/
int rev0::avm_get_real(rev0_avmesh_file* avf, const char* field, double* value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANWSER(a) {*value=a; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   CHECK_PTR(value);
   if (!avf) return 1;

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int patchparam = avf->selection_patchparam-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("modelScale")) RETURN_ANWSER(avf->mesh_hdrs[meshid].modelScale)
      if (FIELD_EQ("reynoldsNumber")) RETURN_ANWSER(avf->mesh_hdrs[meshid].reynoldsNumber)
      if (FIELD_EQ("referenceLength")) RETURN_ANWSER(avf->mesh_hdrs[meshid].referenceLength)
      if (FIELD_EQ("wallDistance")) RETURN_ANWSER(avf->mesh_hdrs[meshid].wallDistance)
      if (FIELD_EQ("periodicity")) RETURN_ANWSER(avf->mesh_hdrs[meshid].periodicity)

      bool bfstruc_mesh = 0==strncmp("bfstruc", avf->mesh_hdrs[meshid].meshType, strlen("bfstruc"));
      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool cart_mesh    = 0==strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"));

      if (bfstruc_mesh && FIELD_EQ("patch_r8_param")) {
         if (0 <= patchid && patchid < avf->bfstruc[meshid].header.nPatches)
            if (0 <= patchparam && patchparam < avf->bfstruc[meshid].header.nPatchR8Params) 
               RETURN_ANWSER(avf->bfstruc[meshid].patchR8Params[patchid][patchparam])
      }

      if (strand_mesh) {
         if (FIELD_EQ("strandLength")) RETURN_ANWSER(avf->strand[meshid].header.strandLength)
         if (FIELD_EQ("stretchRatio")) RETURN_ANWSER(avf->strand[meshid].header.stretchRatio)
         if (FIELD_EQ("smoothingThreshold")) RETURN_ANWSER(avf->strand[meshid].header.smoothingThreshold)

         if (0 <= edgepatchi && edgepatchi < avf->strand[meshid].header.nEdgePatches) {
            if (FIELD_EQ("nx")) RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].nx)
            if (FIELD_EQ("ny")) RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].ny)
            if (FIELD_EQ("nz")) RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].nz)
         }
      }

      if (cart_mesh) {
         if (FIELD_EQ("domXLo")) RETURN_ANWSER(avf->cart[meshid].header.domXLo)
         if (FIELD_EQ("domYLo")) RETURN_ANWSER(avf->cart[meshid].header.domYLo)
         if (FIELD_EQ("domZLo")) RETURN_ANWSER(avf->cart[meshid].header.domZLo)
         if (FIELD_EQ("domXHi")) RETURN_ANWSER(avf->cart[meshid].header.domXHi)
         if (FIELD_EQ("domYHi")) RETURN_ANWSER(avf->cart[meshid].header.domYHi)
         if (FIELD_EQ("domZHi")) RETURN_ANWSER(avf->cart[meshid].header.domZHi)
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef RETURN_ANWSER
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 December 15, 2010 - Initial creation
*/
int rev0::avm_get_str(rev0_avmesh_file* avf, const char* field, char* str, int len)
{
#define JMIN(len0,len1) len0 = (unsigned) len0 < (unsigned) len1 ? len0 : len1
// hack alert: This "JMIN" macro is only used in this routine (it is undefined at the end)
// It's purpose is to capture the length of the string field in order to trim whitespace
// and always null-terminate the outgoing string.
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANWSER(a,l) {strncpy(str,a,l); for (int i=len-1; i>=0 && (isspace(str[i]) || str[i]==0); --i) str[i]=0; str[len-1]=0; return 0;}
#define RETURN_ERROR() {return 1;}

   CHECK_PTR(field);
   CHECK_PTR(str);
   if (!avf) return 1;

   if (FIELD_EQ("AVM_WRITE_ENDIAN")) {
      const char* endian = "native";
      if (avf->write_endian==rev0_avmesh_file::ENDIAN_LITTLE) endian = "little";
      if (avf->write_endian==rev0_avmesh_file::ENDIAN_BIG) endian = "big";
      RETURN_ANWSER(endian, JMIN(len,strlen(endian)+1))
   }

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("meshRevision")) RETURN_ANWSER(avf->file_hdr.meshRevision, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("contactName")) RETURN_ANWSER(avf->file_hdr.contactName, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("contactOrg")) RETURN_ANWSER(avf->file_hdr.contactOrg, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("contactEmail")) RETURN_ANWSER(avf->file_hdr.contactEmail, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("description")) RETURN_ANWSER(avf->description.c_str(), JMIN(len,avf->description.size()+1))
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int patchparam = avf->selection_patchparam-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("meshName")) RETURN_ANWSER(avf->mesh_hdrs[meshid].meshName, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("meshType")) RETURN_ANWSER(avf->mesh_hdrs[meshid].meshType, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("meshGenerator")) RETURN_ANWSER(avf->mesh_hdrs[meshid].meshGenerator, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("changedDate")) RETURN_ANWSER(avf->mesh_hdrs[meshid].changedDate, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("coordinateSystem")) RETURN_ANWSER(avf->mesh_hdrs[meshid].coordinateSystem, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("gridUnits")) RETURN_ANWSER(avf->mesh_hdrs[meshid].gridUnits, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("referencePointDescription")) RETURN_ANWSER(avf->mesh_hdrs[meshid].referencePointDescription, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("periodicityDescription")) RETURN_ANWSER(avf->mesh_hdrs[meshid].periodicityDescription, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("description")) RETURN_ANWSER(avf->mesh_descriptions[meshid].c_str(), JMIN(len,avf->mesh_descriptions[meshid].size()+1))

      bool bfstruc_mesh = 0==strncmp("bfstruc", avf->mesh_hdrs[meshid].meshType, strlen("bfstruc"));
      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

      if (bfstruc_mesh) {
         if (0 <= patchid && patchid < avf->bfstruc[meshid].header.nPatches) {
            if (FIELD_EQ("patch_description")) RETURN_ANWSER(avf->bfstruc[meshid].patches[patchid].description, JMIN(len,AVM_STD_STRING_LENGTH))

            if (FIELD_EQ("patch_c80_param")) {
               if (0 <= patchparam && patchparam < avf->bfstruc[meshid].header.nPatchC80Params) {
                  string tmp = avf->bfstruc[meshid].patchC80Params[patchid][patchparam];
                  RETURN_ANWSER(tmp.c_str(), JMIN(len,80))
               }
            }
         }
      }

      if (strand_mesh) {
         if (FIELD_EQ("strandDistribution")) RETURN_ANWSER(avf->strand[meshid].header.strandDistribution, JMIN(len,32))
         if (0 <= surfpatchi && surfpatchi < avf->strand[meshid].header.nSurfPatches) {
            if (FIELD_EQ("surfPatchBody")) RETURN_ANWSER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBody, JMIN(len,32))
            if (FIELD_EQ("surfPatchComp")) RETURN_ANWSER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchComp, JMIN(len,32))
            if (FIELD_EQ("surfPatchBCType")) RETURN_ANWSER(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBCType, JMIN(len,32))
         }
         if (0 <= edgepatchi && edgepatchi < avf->strand[meshid].header.nEdgePatches) {
            if (FIELD_EQ("edgePatchBody")) RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBody, JMIN(len,32))
            if (FIELD_EQ("edgePatchComp")) RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchComp, JMIN(len,32))
            if (FIELD_EQ("edgePatchBCType")) RETURN_ANWSER(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBCType, JMIN(len,32))
         }
      }
      
      if (unstruc_mesh) {
         if (0 <= patchid && patchid < avf->unstruc[meshid].header.nPatches) {
            if (FIELD_EQ("patchLabel")) RETURN_ANWSER(avf->unstruc[meshid].patches[patchid].patchLabel, JMIN(len,32))
            if (FIELD_EQ("patchType")) RETURN_ANWSER(avf->unstruc[meshid].patches[patchid].patchType, JMIN(len,16))
         }
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef RETURN_ANWSER
#undef RETURN_ERROR
#undef JMIN
}

/**
@author
 Josh Calahan
@history
 December 15, 2010 - Initial creation
*/
int rev0::avm_get_int_array(rev0_avmesh_file* avf, const char* field, int* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANWSER(a,l) {for(int i=0;i<l;++i) values[i]=a[i]; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   CHECK_PTR(values);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   int blockid = avf->selection_blockid-1; 
   bool cart_mesh = 0==strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"));

   if (cart_mesh) {
      if (0 <= meshid && meshid < avf->file_hdr.meshCount) {
         if (0 <= blockid && blockid < avf->cart[meshid].header.nBlocks) {
            if (FIELD_EQ("ilo")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].ilo, min(len,3))
            if (FIELD_EQ("ihi")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].ihi, min(len,3))
            if (FIELD_EQ("iblDim")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].iblDim, min(len,3))
            if (FIELD_EQ("blo")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].blo, min(len,3))
            if (FIELD_EQ("bhi")) RETURN_ANWSER(avf->cart[meshid].blocks[blockid].bhi, min(len,3))
         }
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef RETURN_ANWSER
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 December 15, 2010 - Initial creation
*/
int rev0::avm_get_real_array(rev0_avmesh_file* avf, const char* field, double* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define RETURN_ANWSER(a,l) {for(int i=0;i<l;++i) values[i]=a[i]; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   CHECK_PTR(values);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 
      if (FIELD_EQ("referencePoint")) RETURN_ANWSER(avf->mesh_hdrs[meshid].referencePoint, min(len,3))
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef RETURN_ANWSER
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_set_int(rev0_avmesh_file* avf, const char* field, int value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a) {a=value; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   if (!avf) return 1;

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("meshCount")) {
         avf->mesh_hdrs.resize( value );
         avf->mesh_descriptions.resize( value );
         avf->bfstruc.resize( value ); // TODO: only allocate if needed
         avf->strand.resize( value ); // TODO: only allocate if needed
         avf->unstruc.resize( value ); // TODO: only allocate if needed
         avf->cart.resize( value ); // TODO: only allocate if needed
         SET_AND_RETURN(avf->file_hdr.meshCount)
      }
      if (FIELD_EQ("precision")) SET_AND_RETURN(avf->file_hdr.precision)
      if (FIELD_EQ("dimensions")) SET_AND_RETURN(avf->file_hdr.dimensions)
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int patchparam = avf->selection_patchparam-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 
   int levelid    = avf->selection_levelid-1; 
   int blockid    = avf->selection_blockid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("refinementLevel")) SET_AND_RETURN(avf->mesh_hdrs[meshid].refinementLevel)

      bool bfstruc_mesh = 0==strncmp("bfstruc", avf->mesh_hdrs[meshid].meshType, strlen("bfstruc"));
      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));
      bool cart_mesh    = 0==strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"));

      if (bfstruc_mesh) {
         if (FIELD_EQ("jmax")) SET_AND_RETURN(avf->bfstruc[meshid].header.jmax)
         if (FIELD_EQ("kmax")) SET_AND_RETURN(avf->bfstruc[meshid].header.kmax)
         if (FIELD_EQ("lmax")) SET_AND_RETURN(avf->bfstruc[meshid].header.lmax)
         if (FIELD_EQ("iblank")) SET_AND_RETURN(avf->bfstruc[meshid].header.iblank)
         if (FIELD_EQ("nPatches")) {
            avf->bfstruc[meshid].patches.resize(value);
            avf->bfstruc[meshid].patchIntParams.resize(value);
            avf->bfstruc[meshid].patchR8Params.resize(value);
            avf->bfstruc[meshid].patchC80Params.resize(value);
            SET_AND_RETURN(avf->bfstruc[meshid].header.nPatches)
         }
         if (FIELD_EQ("nPatchIntParams")) {
            vector<int> ints(value);
            for (int i=0; i<avf->bfstruc[meshid].header.nPatches; ++i)
               avf->bfstruc[meshid].patchIntParams[i] = ints;
            SET_AND_RETURN(avf->bfstruc[meshid].header.nPatchIntParams)
         }
         if (FIELD_EQ("nPatchR8Params")) {
            vector<double> reals(value);
            for (int i=0; i<avf->bfstruc[meshid].header.nPatches; ++i)
               avf->bfstruc[meshid].patchR8Params[i] = reals;
            SET_AND_RETURN(avf->bfstruc[meshid].header.nPatchR8Params)
         }
         if (FIELD_EQ("nPatchC80Params")) {
            string str;
            str.resize(80);
            vector<string> strings(value);
            for (int i=0; i<value; ++i)
               strings[i] = str;
            for (int i=0; i<avf->bfstruc[meshid].header.nPatches; ++i)
               avf->bfstruc[meshid].patchC80Params[i] = strings;
            SET_AND_RETURN(avf->bfstruc[meshid].header.nPatchC80Params)
         }

         if (0 <= patchid && patchid < avf->bfstruc[meshid].header.nPatches) {
            if (FIELD_EQ("bctype")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].bctype)
            if (FIELD_EQ("direction")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].direction)
            if (FIELD_EQ("jbegin")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].jbegin)
            if (FIELD_EQ("jend")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].jend)
            if (FIELD_EQ("kbegin")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].kbegin)
            if (FIELD_EQ("kend")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].kend)
            if (FIELD_EQ("lbegin")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].lbegin)
            if (FIELD_EQ("lend")) SET_AND_RETURN(avf->bfstruc[meshid].patches[patchid].lend)

            if (0 <= patchparam && patchparam < avf->bfstruc[meshid].header.nPatchIntParams) {
               if (FIELD_EQ("patch_int_param"))
                  SET_AND_RETURN(avf->bfstruc[meshid].patchIntParams[patchid][patchparam])
            }
         }
      }

      if (strand_mesh) {
         if (FIELD_EQ("surfaceOnly")) SET_AND_RETURN(avf->strand[meshid].header.surfaceOnly)
         if (FIELD_EQ("nSurfNodes")) SET_AND_RETURN(avf->strand[meshid].header.nSurfNodes)
         if (FIELD_EQ("nTriFaces")) SET_AND_RETURN(avf->strand[meshid].header.nTriFaces)
         if (FIELD_EQ("nQuadFaces")) SET_AND_RETURN(avf->strand[meshid].header.nQuadFaces)
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
         if (FIELD_EQ("surfPatchId"))
            if (0 <= surfpatchi && surfpatchi < avf->strand[meshid].header.nSurfPatches) 
               SET_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchId)
         if (FIELD_EQ("edgePatchId"))
            if (0 <= edgepatchi && edgepatchi < avf->strand[meshid].header.nEdgePatches) 
               SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchId)
      }

      if (unstruc_mesh) {
         if (FIELD_EQ("fullViscousLayerCount")) SET_AND_RETURN(avf->unstruc[meshid].header.fullViscousLayerCount)
         if (FIELD_EQ("partialViscousLayerCount")) SET_AND_RETURN(avf->unstruc[meshid].header.partialViscousLayerCount)
         if (FIELD_EQ("nNodes")) SET_AND_RETURN(avf->unstruc[meshid].header.nNodes)
         if (FIELD_EQ("nEdges")) SET_AND_RETURN(avf->unstruc[meshid].header.nEdges)
         if (FIELD_EQ("nFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nFaces)
         if (FIELD_EQ("nMaxNodesPerFace")) SET_AND_RETURN(avf->unstruc[meshid].header.nMaxNodesPerFace)
         if (FIELD_EQ("nTriFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nTriFaces)
         if (FIELD_EQ("nQuadFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nQuadFaces)
         if (FIELD_EQ("nBndTriFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nBndTriFaces)
         if (FIELD_EQ("nBndQuadFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nBndQuadFaces)
         if (FIELD_EQ("nCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nCells)
         if (FIELD_EQ("nMaxNodesPerCell")) SET_AND_RETURN(avf->unstruc[meshid].header.nMaxNodesPerCell)
         if (FIELD_EQ("nMaxFacesPerCell")) SET_AND_RETURN(avf->unstruc[meshid].header.nMaxFacesPerCell)
         if (FIELD_EQ("nHexCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nHexCells)
         if (FIELD_EQ("nTetCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nTetCells)
         if (FIELD_EQ("nPriCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nPriCells)
         if (FIELD_EQ("nPyrCells")) SET_AND_RETURN(avf->unstruc[meshid].header.nPyrCells)
         if (FIELD_EQ("nPatches")) {
            avf->unstruc[meshid].patches.resize(value);
            SET_AND_RETURN(avf->unstruc[meshid].header.nPatches)
         }
         if (FIELD_EQ("nPatchNodes")) SET_AND_RETURN(avf->unstruc[meshid].header.nPatchNodes)
         if (FIELD_EQ("nPatchFaces")) SET_AND_RETURN(avf->unstruc[meshid].header.nPatchFaces)

         if (0 <= patchid && patchid < avf->unstruc[meshid].header.nPatches) {
            if (FIELD_EQ("patchId")) SET_AND_RETURN(avf->unstruc[meshid].patches[patchid].patchId)
         }
      }

      if (cart_mesh) {
         if (FIELD_EQ("nLevels")) {
            avf->cart[meshid].ratios.resize(value);
            SET_AND_RETURN(avf->cart[meshid].header.nLevels)
         }
         if (FIELD_EQ("nBlocks")) {
            avf->cart[meshid].blocks.resize(value);
            SET_AND_RETURN(avf->cart[meshid].header.nBlocks)
         }
         if (FIELD_EQ("nFringe")) SET_AND_RETURN(avf->cart[meshid].header.nFringe)
         if (FIELD_EQ("nxc")) SET_AND_RETURN(avf->cart[meshid].header.nxc)
         if (FIELD_EQ("nyc")) SET_AND_RETURN(avf->cart[meshid].header.nyc)
         if (FIELD_EQ("nzc")) SET_AND_RETURN(avf->cart[meshid].header.nzc)

         if (FIELD_EQ("iRatio"))
            if (0 <= levelid && levelid < avf->cart[meshid].header.nLevels)
               SET_AND_RETURN(avf->cart[meshid].ratios[levelid])

         if (0 <= blockid && blockid < avf->cart[meshid].header.nBlocks) {
            if (FIELD_EQ("levNum")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].levNum)
            if (FIELD_EQ("iblFlag")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].iblFlag)
            if (FIELD_EQ("bdryFlag")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].bdryFlag)
         }
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef SET_AND_RETURN
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_set_real(rev0_avmesh_file* avf, const char* field, double value)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a) {a=value; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   if (!avf) return 1;

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int patchparam = avf->selection_patchparam-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("modelScale")) SET_AND_RETURN(avf->mesh_hdrs[meshid].modelScale)
      if (FIELD_EQ("reynoldsNumber")) SET_AND_RETURN(avf->mesh_hdrs[meshid].reynoldsNumber)
      if (FIELD_EQ("referenceLength")) SET_AND_RETURN(avf->mesh_hdrs[meshid].referenceLength)
      if (FIELD_EQ("wallDistance")) SET_AND_RETURN(avf->mesh_hdrs[meshid].wallDistance)
      if (FIELD_EQ("periodicity")) SET_AND_RETURN(avf->mesh_hdrs[meshid].periodicity)

      bool bfstruc_mesh = 0==strncmp("bfstruc", avf->mesh_hdrs[meshid].meshType, strlen("bfstruc"));
      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool cart_mesh    = 0==strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"));

      if (bfstruc_mesh && FIELD_EQ("patch_r8_param")) {
         if (0 <= patchid && patchid < avf->bfstruc[meshid].header.nPatches)
            if (0 <= patchparam && patchparam < avf->bfstruc[meshid].header.nPatchR8Params) 
               SET_AND_RETURN(avf->bfstruc[meshid].patchR8Params[patchid][patchparam])
      }

      if (strand_mesh) {
         if (FIELD_EQ("strandLength")) SET_AND_RETURN(avf->strand[meshid].header.strandLength)
         if (FIELD_EQ("stretchRatio")) SET_AND_RETURN(avf->strand[meshid].header.stretchRatio)
         if (FIELD_EQ("smoothingThreshold")) SET_AND_RETURN(avf->strand[meshid].header.smoothingThreshold)

         if (0 <= edgepatchi && edgepatchi < avf->strand[meshid].header.nEdgePatches) {
            if (FIELD_EQ("nx")) SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].nx)
            if (FIELD_EQ("ny")) SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].ny)
            if (FIELD_EQ("nz")) SET_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].nz)
         }
      }

      if (cart_mesh) {
         if (FIELD_EQ("domXLo")) SET_AND_RETURN(avf->cart[meshid].header.domXLo)
         if (FIELD_EQ("domYLo")) SET_AND_RETURN(avf->cart[meshid].header.domYLo)
         if (FIELD_EQ("domZLo")) SET_AND_RETURN(avf->cart[meshid].header.domZLo)
         if (FIELD_EQ("domXHi")) SET_AND_RETURN(avf->cart[meshid].header.domXHi)
         if (FIELD_EQ("domYHi")) SET_AND_RETURN(avf->cart[meshid].header.domYHi)
         if (FIELD_EQ("domZHi")) SET_AND_RETURN(avf->cart[meshid].header.domZHi)
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef SET_AND_RETURN
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_set_str(rev0_avmesh_file* avf, const char* field, const char* str, int len)
{
#define JMIN(len0,len1) len0 = len0 < len1 ? len0 : len1
// hack alert: This "JMIN" macro is only used in this routine (it is undefined at the end)
// It's purpose is to capture the length of the string field in order to trim whitespace
#define FIELD_EQ(s) 0==strcmp(s,field)
#define STRCPY_AND_RETURN(a,l) {strncpy(a,str,l); for (int i=len-1; i>=0 && isspace(a[i]); --i) a[i]=0; return 0;}
#define SETSTR_AND_RETURN(a,l) {a=string(str,str+l); return 0;}
#define RETURN_ERROR() {return 1;}

   CHECK_PTR(field);
   CHECK_PTR(str);
   if (!avf) return 1;

   if (FIELD_EQ("AVM_WRITE_ENDIAN")) {
      if (0==strcmp(str,"big")) avf->write_endian = rev0_avmesh_file::ENDIAN_BIG;
      else if (0==strcmp(str,"little")) avf->write_endian = rev0_avmesh_file::ENDIAN_LITTLE;
      else if (0==strcmp(str,"native")) avf->write_endian = rev0_avmesh_file::ENDIAN_NATIVE;
      else RETURN_ERROR()
      return 0;
   }

   if (0==avf->selection_meshid) { // file_header
      if (FIELD_EQ("meshRevision")) STRCPY_AND_RETURN(avf->file_hdr.meshRevision, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("contactName")) STRCPY_AND_RETURN(avf->file_hdr.contactName, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("contactOrg")) STRCPY_AND_RETURN(avf->file_hdr.contactOrg, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("contactEmail")) STRCPY_AND_RETURN(avf->file_hdr.contactEmail, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("description")) {
         avf->file_hdr.descriptionSize = len;
         SETSTR_AND_RETURN(avf->description,len)
      }
   }

   int meshid     = avf->selection_meshid-1; 
   int patchid    = avf->selection_patchid-1; 
   int patchparam = avf->selection_patchparam-1; 
   int surfpatchi = avf->selection_surfpatchid-1; 
   int edgepatchi = avf->selection_edgepatchid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 

      if (FIELD_EQ("meshName")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshName, JMIN(len,AVM_STD_STRING_LENGTH))
      if (FIELD_EQ("meshType")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshType, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("meshGenerator")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].meshGenerator, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("changedDate")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].changedDate, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("coordinateSystem")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].coordinateSystem, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("gridUnits")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].gridUnits, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("referencePointDescription")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].referencePointDescription, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("periodicityDescription")) STRCPY_AND_RETURN(avf->mesh_hdrs[meshid].periodicityDescription, JMIN(len,AVM_STD_STRING_LENGTH)) 
      if (FIELD_EQ("description")) {
         avf->mesh_hdrs[meshid].descriptionSize = len;
         SETSTR_AND_RETURN(avf->mesh_descriptions[meshid],len)
      }

      bool bfstruc_mesh = 0==strncmp("bfstruc", avf->mesh_hdrs[meshid].meshType, strlen("bfstruc"));
      bool strand_mesh  = 0==strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"));
      bool unstruc_mesh = 0==strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"));

      if (bfstruc_mesh) {
         if (0 <= patchid && patchid < avf->bfstruc[meshid].header.nPatches) {
            if (FIELD_EQ("patch_description")) STRCPY_AND_RETURN(avf->bfstruc[meshid].patches[patchid].description, JMIN(len,AVM_STD_STRING_LENGTH))

            if (FIELD_EQ("patch_c80_param")) {
               if (0 <= patchparam && patchparam < avf->bfstruc[meshid].header.nPatchC80Params) {
                  SETSTR_AND_RETURN(avf->bfstruc[meshid].patchC80Params[patchid][patchparam],len)
               }
            }
         }
      }

      if (strand_mesh) {
         if (FIELD_EQ("strandDistribution")) STRCPY_AND_RETURN(avf->strand[meshid].header.strandDistribution, JMIN(len,32))
         if (0 <= surfpatchi && surfpatchi < avf->strand[meshid].header.nSurfPatches) {
            if (FIELD_EQ("surfPatchBody")) STRCPY_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBody, JMIN(len,32))
            if (FIELD_EQ("surfPatchComp")) STRCPY_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchComp, JMIN(len,32))
            if (FIELD_EQ("surfPatchBCType")) STRCPY_AND_RETURN(avf->strand[meshid].surf_patches[surfpatchi].surfPatchBCType, JMIN(len,32))
         }
         if (0 <= edgepatchi && edgepatchi < avf->strand[meshid].header.nEdgePatches) {
            if (FIELD_EQ("edgePatchBody")) STRCPY_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBody, JMIN(len,32))
            if (FIELD_EQ("edgePatchComp")) STRCPY_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchComp, JMIN(len,32))
            if (FIELD_EQ("edgePatchBCType")) STRCPY_AND_RETURN(avf->strand[meshid].edge_patches[edgepatchi].edgePatchBCType, JMIN(len,32))
         }
      }
      
      if (unstruc_mesh) {
         if (0 <= patchid && patchid < avf->unstruc[meshid].header.nPatches) {
            if (FIELD_EQ("patchLabel")) STRCPY_AND_RETURN(avf->unstruc[meshid].patches[patchid].patchLabel, JMIN(len,32))
            if (FIELD_EQ("patchType")) STRCPY_AND_RETURN(avf->unstruc[meshid].patches[patchid].patchType, JMIN(len,16))
         }
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef STRCPY_AND_RETURN
#undef SETSTR_AND_RETURN
#undef RETURN_ERROR
#undef JMIN
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_set_int_array(rev0_avmesh_file* avf, const char* field, const int* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a,l) {for(int i=0;i<l;++i) a[i]=values[i]; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   CHECK_PTR(values);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   int blockid = avf->selection_blockid-1; 
   bool cart_mesh = 0==strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"));

   if (cart_mesh) {
      if (0 <= meshid && meshid < avf->file_hdr.meshCount) {
         if (0 <= blockid && blockid < avf->cart[meshid].header.nBlocks) {
            if (FIELD_EQ("ilo")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].ilo, min(len,3))
            if (FIELD_EQ("ihi")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].ihi, min(len,3))
            if (FIELD_EQ("iblDim")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].iblDim, min(len,3))
            if (FIELD_EQ("blo")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].blo, min(len,3))
            if (FIELD_EQ("bhi")) SET_AND_RETURN(avf->cart[meshid].blocks[blockid].bhi, min(len,3))
         }
      }
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef SET_AND_RETURN
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_set_real_array(rev0_avmesh_file* avf, const char* field, const double* values, int len)
{
#define FIELD_EQ(s) 0==strcmp(s,field)
#define SET_AND_RETURN(a,l) {for(int i=0;i<l;++i) a[i]=values[i]; return 0;}
#define RETURN_ERROR() {return 1;}
   CHECK_PTR(field);
   CHECK_PTR(values);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 

   if (0 <= meshid && meshid < avf->file_hdr.meshCount) { // mesh_header 
      if (FIELD_EQ("referencePoint")) SET_AND_RETURN(avf->mesh_hdrs[meshid].referencePoint, min(len,3))
   }

   RETURN_ERROR()  
#undef FIELD_EQ
#undef SET_AND_RETURN
#undef RETURN_ERROR
}

/**
@author
 Josh Calahan
@history
 January 24, 2011 - Initial creation
*/
int rev0::avm_mesh_header_offset(rev0_avmesh_file* avf, int mesh, off_t* offset)
{
   CHECK_PTR(offset);
   if (!avf) return 1;

   mesh = mesh-1;
   if (!(0 <= mesh && mesh <= avf->file_hdr.meshCount)) return 1;

   *offset = 0;
   *offset += avf->file_prefix.size();
   *offset += avf->file_hdr.size();
   *offset += avf->file_hdr.descriptionSize;

   for (int i=0; i<mesh; ++i) {
      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      *offset += mesh_hdr.size();
      *offset += mesh_hdr.descriptionSize;
      if (0==strncmp("bfstruc", mesh_hdr.meshType, strlen("bfstruc"))) {
         *offset += avf->bfstruc[i].hdrsize();
      } else if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         *offset += avf->strand[i].hdrsize();
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         *offset += avf->unstruc[i].hdrsize();
      } else if (0==strncmp("cart", mesh_hdr.meshType, strlen("cart"))) {
         *offset += avf->cart[i].hdrsize();
      } else return 1;
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 24, 2011 - Initial creation
*/
int rev0::avm_mesh_data_offset(rev0_avmesh_file* avf, int mesh, off_t* offset)
{
   CHECK_PTR(offset);
   if (!avf) return 1;

   mesh = mesh-1;
   if (!(0 <= mesh && mesh <= avf->file_hdr.meshCount)) return 1;
   
   *offset = 0;
   if (avm_mesh_header_offset(avf, avf->file_hdr.meshCount+1, offset)) return 1;

   for (int i=0; i<mesh; ++i) {
      mesh_header& mesh_hdr = avf->mesh_hdrs[i];
      if (0==strncmp("bfstruc", mesh_hdr.meshType, strlen("bfstruc"))) {
         *offset += avf->bfstruc[i].datasize(avf->file_hdr, mesh_hdr);
      } else if (0==strncmp("strand", mesh_hdr.meshType, strlen("strand"))) {
         *offset += avf->strand[i].datasize(avf->file_hdr, mesh_hdr);
      } else if (0==strncmp("unstruc", mesh_hdr.meshType, strlen("unstruc"))) {
         *offset += avf->unstruc[i].datasize(avf->file_hdr, mesh_hdr);
      } else if (0==strncmp("cart", mesh_hdr.meshType, strlen("cart"))) {
         *offset += avf->cart[i].datasize(avf->file_hdr, mesh_hdr);
      } else return 1;
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 25, 2011 - Initial creation
*/
int rev0::avm_seek_to_mesh(rev0_avmesh_file* avf, int mesh)
{
   if (!avf) return 1;

   off_t mesh_offset;
   if (avm_mesh_data_offset(avf, mesh, &mesh_offset)) return 1;
#ifdef WIN32
   if (-1==fseek(avf->fp, (long)mesh_offset, SEEK_SET)) return 1;
#else
   if (-1==fseeko(avf->fp, mesh_offset, SEEK_SET)) return 1;
#endif

   return 0;
}

//
// bfstruc functions 
//

/**
@author
 Josh Calahan
@history
 May 26, 2010 - Initial creation
*/
int rev0::avm_bfstruc_read_xyz_r4(
   rev0_avmesh_file* avf,
   float* x,
   float* y,
   float* z,
   int size
)
{
   CHECK_PTR(x);
   CHECK_PTR(y);
   CHECK_PTR(z);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1;

   int array_size =
      (avf->bfstruc[meshid].header.jmax *
       avf->bfstruc[meshid].header.kmax *
       avf->bfstruc[meshid].header.lmax);
   if (array_size != size) return 1;

   if (!fread(x, sizeof(float)*array_size, 1, avf->fp)) return 1;
   if (!fread(y, sizeof(float)*array_size, 1, avf->fp)) return 1;
   if (!fread(z, sizeof(float)*array_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<array_size; ++i) {
         byte_swap_float(x+i);
         byte_swap_float(y+i);
         byte_swap_float(z+i);
      }
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 May 26, 2010 - Initial creation
*/
int rev0::avm_bfstruc_read_xyz_r8(
   rev0_avmesh_file* avf,
   double* x,
   double* y,
   double* z,
   int size
)
{
   CHECK_PTR(x);
   CHECK_PTR(y);
   CHECK_PTR(z);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1;

   int array_size =
      (avf->bfstruc[meshid].header.jmax *
       avf->bfstruc[meshid].header.kmax *
       avf->bfstruc[meshid].header.lmax);
   if (array_size != size) return 1;

   if (!fread(x, sizeof(double)*array_size, 1, avf->fp)) return 1;
   if (!fread(y, sizeof(double)*array_size, 1, avf->fp)) return 1;
   if (!fread(z, sizeof(double)*array_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<array_size; ++i) {
         byte_swap_double(x+i);
         byte_swap_double(y+i);
         byte_swap_double(z+i);
      }
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 May 26, 2010 - Initial creation
*/
int rev0::avm_bfstruc_read_iblank(
   rev0_avmesh_file* avf,
   int* iblanks,
   int size
)
{
   CHECK_PTR(iblanks);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1;

   int array_size =
      (avf->bfstruc[meshid].header.jmax *
       avf->bfstruc[meshid].header.kmax *
       avf->bfstruc[meshid].header.lmax);
   if (array_size != size) return 1;

   if (!fread(iblanks, sizeof(int)*array_size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<array_size; ++i)
         byte_swap_int(iblanks+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_bfstruc_write_xyz_r4(rev0_avmesh_file* avf, float* x, float* y, float* z, int size)
{
   CHECK_PTR(x);
   CHECK_PTR(y);
   CHECK_PTR(z);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1;

   int array_size =
      (avf->bfstruc[meshid].header.jmax *
       avf->bfstruc[meshid].header.kmax *
       avf->bfstruc[meshid].header.lmax);
   if (array_size != size) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<array_size; ++i) {
         byte_swap_float(x+i);
         byte_swap_float(y+i);
         byte_swap_float(z+i);
      }
   }

   if (!fwrite(x, sizeof(float)*array_size, 1, avf->fp)) return 1;
   if (!fwrite(y, sizeof(float)*array_size, 1, avf->fp)) return 1;
   if (!fwrite(z, sizeof(float)*array_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<array_size; ++i) {
         byte_swap_float(x+i);
         byte_swap_float(y+i);
         byte_swap_float(z+i);
      }
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_bfstruc_write_xyz_r8(rev0_avmesh_file* avf, double* x, double* y, double* z, int size)
{
   CHECK_PTR(x);
   CHECK_PTR(y);
   CHECK_PTR(z);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1;

   int array_size =
      (avf->bfstruc[meshid].header.jmax *
       avf->bfstruc[meshid].header.kmax *
       avf->bfstruc[meshid].header.lmax);
   if (array_size != size) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<array_size; ++i) {
         byte_swap_double(x+i);
         byte_swap_double(y+i);
         byte_swap_double(z+i);
      }
   }

   if (!fwrite(x, sizeof(double)*array_size, 1, avf->fp)) return 1;
   if (!fwrite(y, sizeof(double)*array_size, 1, avf->fp)) return 1;
   if (!fwrite(z, sizeof(double)*array_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<array_size; ++i) {
         byte_swap_double(x+i);
         byte_swap_double(y+i);
         byte_swap_double(z+i);
      }
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 20, 2011 - Initial creation
*/
int rev0::avm_bfstruc_write_iblank(rev0_avmesh_file* avf, int* iblanks, int size)
{
   CHECK_PTR(iblanks);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1;

   int array_size =
      (avf->bfstruc[meshid].header.jmax *
       avf->bfstruc[meshid].header.kmax *
       avf->bfstruc[meshid].header.lmax);
   if (array_size != size) return 1;

   if (avf->byte_swap)
      for (int i=0; i<array_size; ++i)
         byte_swap_int(iblanks+i);

   if (!fwrite(iblanks, sizeof(int)*array_size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<array_size; ++i)
         byte_swap_int(iblanks+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 December 13, 2010 - Initial creation
*/
int rev0::avm_strand_read_r4(rev0_avmesh_file* avf,
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
)
{
   CHECK_PTR(triConn);
   CHECK_PTR(quadConn);
   CHECK_PTR(bndEdges);
   CHECK_PTR(nodeClip);
   CHECK_PTR(faceClip);
   CHECK_PTR(faceTag);
   CHECK_PTR(edgeTag);
   CHECK_PTR(xSurf);
   CHECK_PTR(pointingVec);
   CHECK_PTR(xStrand);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) return 1;
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (triConn_size != 3*hdr.nTriFaces) return 1;
   if (!fread(triConn, sizeof(int)*triConn_size, 1, avf->fp)) return 1;

   if (quadConn_size != 4*hdr.nQuadFaces) return 1;
   if (!fread(quadConn, sizeof(int)*quadConn_size, 1, avf->fp)) return 1;

   if (bndEdges_size != 2*hdr.nBndEdges) return 1;
   if (!fread(bndEdges, sizeof(int)*bndEdges_size, 1, avf->fp)) return 1;

   if (nodeClip_size != hdr.nSurfNodes) return 1;
   if (!fread(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) return 1;

   if (faceClip_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fread(faceClip, sizeof(int)*faceClip_size, 1, avf->fp)) return 1;

   if (faceTag_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fread(faceTag, sizeof(int)*faceTag_size, 1, avf->fp)) return 1;

   if (edgeTag_size != hdr.nBndEdges) return 1;
   if (!fread(edgeTag, sizeof(int)*edgeTag_size, 1, avf->fp)) return 1;

   if (xSurf_size != 3*hdr.nSurfNodes) return 1;
   if (!fread(xSurf, sizeof(float)*xSurf_size, 1, avf->fp)) return 1;

   if (pointingVec_size != 3*hdr.nSurfNodes) return 1;
   if (!fread(pointingVec, sizeof(float)*pointingVec_size, 1, avf->fp)) return 1;

   if (xStrand_size != hdr.nPtsPerStrand) return 1;
   if (!fread(xStrand, sizeof(float)*xStrand_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<triConn_size; ++i) byte_swap_int(triConn+i);
      for (int i=0; i<quadConn_size; ++i) byte_swap_int(quadConn+i);
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<faceClip_size; ++i) byte_swap_int(faceClip+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
      for (int i=0; i<xSurf_size; ++i) byte_swap_float(xSurf+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_float(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_float(xStrand+i);
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 December 13, 2010 - Initial creation
*/
int rev0::avm_strand_read_r8(rev0_avmesh_file* avf,
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
)
{
   CHECK_PTR(triConn);
   CHECK_PTR(quadConn);
   CHECK_PTR(bndEdges);
   CHECK_PTR(nodeClip);
   CHECK_PTR(faceClip);
   CHECK_PTR(faceTag);
   CHECK_PTR(edgeTag);
   CHECK_PTR(xSurf);
   CHECK_PTR(pointingVec);
   CHECK_PTR(xStrand);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) return 1;
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (triConn_size != 3*hdr.nTriFaces) return 1;
   if (!fread(triConn, sizeof(int)*triConn_size, 1, avf->fp)) return 1;

   if (quadConn_size != 4*hdr.nQuadFaces) return 1;
   if (!fread(quadConn, sizeof(int)*quadConn_size, 1, avf->fp)) return 1;

   if (bndEdges_size != 2*hdr.nBndEdges) return 1;
   if (!fread(bndEdges, sizeof(int)*bndEdges_size, 1, avf->fp)) return 1;

   if (nodeClip_size != hdr.nSurfNodes) return 1;
   if (!fread(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) return 1;

   if (faceClip_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fread(faceClip, sizeof(int)*faceClip_size, 1, avf->fp)) return 1;

   if (faceTag_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fread(faceTag, sizeof(int)*faceTag_size, 1, avf->fp)) return 1;

   if (edgeTag_size != hdr.nBndEdges) return 1;
   if (!fread(edgeTag, sizeof(int)*edgeTag_size, 1, avf->fp)) return 1;

   if (xSurf_size != 3*hdr.nSurfNodes) return 1;
   if (!fread(xSurf, sizeof(double)*xSurf_size, 1, avf->fp)) return 1;

   if (pointingVec_size != 3*hdr.nSurfNodes) return 1;
   if (!fread(pointingVec, sizeof(double)*pointingVec_size, 1, avf->fp)) return 1;

   if (xStrand_size != hdr.nPtsPerStrand) return 1;
   if (!fread(xStrand, sizeof(double)*xStrand_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<triConn_size; ++i) byte_swap_int(triConn+i);
      for (int i=0; i<quadConn_size; ++i) byte_swap_int(quadConn+i);
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<faceClip_size; ++i) byte_swap_int(faceClip+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
      for (int i=0; i<xSurf_size; ++i) byte_swap_double(xSurf+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_double(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_double(xStrand+i);
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 21, 2011 - Initial creation
*/
int rev0::avm_strand_write_r4(rev0_avmesh_file* avf,
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
)
{
   CHECK_PTR(triConn);
   CHECK_PTR(quadConn);
   CHECK_PTR(bndEdges);
   CHECK_PTR(nodeClip);
   CHECK_PTR(faceClip);
   CHECK_PTR(faceTag);
   CHECK_PTR(edgeTag);
   CHECK_PTR(xSurf);
   CHECK_PTR(pointingVec);
   CHECK_PTR(xStrand);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) return 1;
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (avf->byte_swap) {
      for (int i=0; i<triConn_size; ++i) byte_swap_int(triConn+i);
      for (int i=0; i<quadConn_size; ++i) byte_swap_int(quadConn+i);
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<faceClip_size; ++i) byte_swap_int(faceClip+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
      for (int i=0; i<xSurf_size; ++i) byte_swap_float(xSurf+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_float(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_float(xStrand+i);
   }

   if (triConn_size != 3*hdr.nTriFaces) return 1;
   if (!fwrite(triConn, sizeof(int)*triConn_size, 1, avf->fp)) return 1;

   if (quadConn_size != 4*hdr.nQuadFaces) return 1;
   if (!fwrite(quadConn, sizeof(int)*quadConn_size, 1, avf->fp)) return 1;

   if (bndEdges_size != 2*hdr.nBndEdges) return 1;
   if (!fwrite(bndEdges, sizeof(int)*bndEdges_size, 1, avf->fp)) return 1;

   if (nodeClip_size != hdr.nSurfNodes) return 1;
   if (!fwrite(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) return 1;

   if (faceClip_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fwrite(faceClip, sizeof(int)*faceClip_size, 1, avf->fp)) return 1;

   if (faceTag_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fwrite(faceTag, sizeof(int)*faceTag_size, 1, avf->fp)) return 1;

   if (edgeTag_size != hdr.nBndEdges) return 1;
   if (!fwrite(edgeTag, sizeof(int)*edgeTag_size, 1, avf->fp)) return 1;

   if (xSurf_size != 3*hdr.nSurfNodes) return 1;
   if (!fwrite(xSurf, sizeof(float)*xSurf_size, 1, avf->fp)) return 1;

   if (pointingVec_size != 3*hdr.nSurfNodes) return 1;
   if (!fwrite(pointingVec, sizeof(float)*pointingVec_size, 1, avf->fp)) return 1;

   if (xStrand_size != hdr.nPtsPerStrand) return 1;
   if (!fwrite(xStrand, sizeof(float)*xStrand_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<triConn_size; ++i) byte_swap_int(triConn+i);
      for (int i=0; i<quadConn_size; ++i) byte_swap_int(quadConn+i);
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<faceClip_size; ++i) byte_swap_int(faceClip+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
      for (int i=0; i<xSurf_size; ++i) byte_swap_float(xSurf+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_float(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_float(xStrand+i);
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 21, 2011 - Initial creation
*/
int rev0::avm_strand_write_r8(rev0_avmesh_file* avf,
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
)
{
   CHECK_PTR(triConn);
   CHECK_PTR(quadConn);
   CHECK_PTR(bndEdges);
   CHECK_PTR(nodeClip);
   CHECK_PTR(faceClip);
   CHECK_PTR(faceTag);
   CHECK_PTR(edgeTag);
   CHECK_PTR(xSurf);
   CHECK_PTR(pointingVec);
   CHECK_PTR(xStrand);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("strand", avf->mesh_hdrs[meshid].meshType, strlen("strand"))) return 1;
      
   const strand_header& hdr = avf->strand[meshid].header;

   if (avf->byte_swap) {
      for (int i=0; i<triConn_size; ++i) byte_swap_int(triConn+i);
      for (int i=0; i<quadConn_size; ++i) byte_swap_int(quadConn+i);
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<faceClip_size; ++i) byte_swap_int(faceClip+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
      for (int i=0; i<xSurf_size; ++i) byte_swap_double(xSurf+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_double(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_double(xStrand+i);
   }

   if (triConn_size != 3*hdr.nTriFaces) return 1;
   if (!fwrite(triConn, sizeof(int)*triConn_size, 1, avf->fp)) return 1;

   if (quadConn_size != 4*hdr.nQuadFaces) return 1;
   if (!fwrite(quadConn, sizeof(int)*quadConn_size, 1, avf->fp)) return 1;

   if (bndEdges_size != 2*hdr.nBndEdges) return 1;
   if (!fwrite(bndEdges, sizeof(int)*bndEdges_size, 1, avf->fp)) return 1;

   if (nodeClip_size != hdr.nSurfNodes) return 1;
   if (!fwrite(nodeClip, sizeof(int)*nodeClip_size, 1, avf->fp)) return 1;

   if (faceClip_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fwrite(faceClip, sizeof(int)*faceClip_size, 1, avf->fp)) return 1;

   if (faceTag_size != hdr.nTriFaces+hdr.nQuadFaces) return 1;
   if (!fwrite(faceTag, sizeof(int)*faceTag_size, 1, avf->fp)) return 1;

   if (edgeTag_size != hdr.nBndEdges) return 1;
   if (!fwrite(edgeTag, sizeof(int)*edgeTag_size, 1, avf->fp)) return 1;

   if (xSurf_size != 3*hdr.nSurfNodes) return 1;
   if (!fwrite(xSurf, sizeof(double)*xSurf_size, 1, avf->fp)) return 1;

   if (pointingVec_size != 3*hdr.nSurfNodes) return 1;
   if (!fwrite(pointingVec, sizeof(double)*pointingVec_size, 1, avf->fp)) return 1;

   if (xStrand_size != hdr.nPtsPerStrand) return 1;
   if (!fwrite(xStrand, sizeof(double)*xStrand_size, 1, avf->fp)) return 1;

   if (avf->byte_swap) {
      for (int i=0; i<triConn_size; ++i) byte_swap_int(triConn+i);
      for (int i=0; i<quadConn_size; ++i) byte_swap_int(quadConn+i);
      for (int i=0; i<bndEdges_size; ++i) byte_swap_int(bndEdges+i);
      for (int i=0; i<nodeClip_size; ++i) byte_swap_int(nodeClip+i);
      for (int i=0; i<faceClip_size; ++i) byte_swap_int(faceClip+i);
      for (int i=0; i<faceTag_size; ++i) byte_swap_int(faceTag+i);
      for (int i=0; i<edgeTag_size; ++i) byte_swap_int(edgeTag+i);
      for (int i=0; i<xSurf_size; ++i) byte_swap_double(xSurf+i);
      for (int i=0; i<pointingVec_size; ++i) byte_swap_double(pointingVec+i);
      for (int i=0; i<xStrand_size; ++i) byte_swap_double(xStrand+i);
   }

   return 0;
}

/**
@author
 Josh Calahan
@history
 March 15, 2011 - Initial creation
 May 4, 2011 - Added byte swapping
*/
int rev0::avm_unstruc_read_nodes_r4(rev0_avmesh_file* avf, float* xyz, int xyz_size)
{
   CHECK_PTR(xyz);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) return 1;
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (xyz_size != 3*hdr.nNodes) return 1;
   if (!fread(xyz, sizeof(float)*xyz_size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_float(xyz+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 March 15, 2011 - Initial creation
 May 4, 2011 - Added byte swapping
*/
int rev0::avm_unstruc_read_nodes_r8(rev0_avmesh_file* avf, double* xyz, int xyz_size)
{
   CHECK_PTR(xyz);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) return 1;
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (xyz_size != 3*hdr.nNodes) return 1;
   if (!fread(xyz, sizeof(double)*xyz_size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_double(xyz+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 March 15, 2011 - Initial creation
*/
int rev0::avm_unstruc_read_faces(rev0_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
)
{
   CHECK_PTR(triFaces);
   CHECK_PTR(quadFaces);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) return 1;
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of face buffers
// (this is unstruc format revision 0)
   if (triFaces_size != (5*(hdr.nBndTriFaces+hdr.nTriFaces-hdr.nBndTriFaces))) return 1;
   if (quadFaces_size != (6*(hdr.nBndQuadFaces+hdr.nQuadFaces-hdr.nBndQuadFaces))) return 1;

// Tri patch faces
   if (hdr.nBndTriFaces > 0) {
      if (!fread(triFaces, sizeof(int)*5*hdr.nBndTriFaces, 1, avf->fp)) return 1;
   }

// Quad patch faces
   if (hdr.nBndQuadFaces > 0) {
      if (!fread(quadFaces, sizeof(int)*6*hdr.nBndQuadFaces, 1, avf->fp)) return 1;
   }

// Tri interior faces
   if (hdr.nTriFaces-hdr.nBndTriFaces > 0) {
      if (!fread(triFaces+(5*hdr.nBndTriFaces), sizeof(int)*5*(hdr.nTriFaces-hdr.nBndTriFaces), 1, avf->fp)) return 1;
   }

// Quad interior faces
   if (hdr.nQuadFaces-hdr.nBndQuadFaces > 0) {
      if (!fread(quadFaces+(6*hdr.nBndQuadFaces), sizeof(int)*6*(hdr.nQuadFaces-hdr.nBndQuadFaces), 1, avf->fp)) return 1;
   }

// byte swap
   if (avf->byte_swap) for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
   if (avf->byte_swap) for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 March 15, 2011 - Initial creation
*/
int rev0::avm_unstruc_write_nodes_r4(rev0_avmesh_file* avf, float* xyz, int xyz_size)
{
   CHECK_PTR(xyz);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) return 1;
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (xyz_size != 3*hdr.nNodes) return 1;

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_float(xyz+i);

   if (!fwrite(xyz, sizeof(float)*xyz_size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_float(xyz+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 March 15, 2011 - Initial creation
*/
int rev0::avm_unstruc_write_nodes_r8(rev0_avmesh_file* avf, double* xyz, int xyz_size)
{
   CHECK_PTR(xyz);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) return 1;
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

   if (xyz_size != 3*hdr.nNodes) return 1;

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_double(xyz+i);

   if (!fwrite(xyz, sizeof(double)*xyz_size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<xyz_size; ++i)
         byte_swap_double(xyz+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 March 15, 2011 - Initial creation
*/
int rev0::avm_unstruc_write_faces(rev0_avmesh_file* avf,
   int* triFaces,  int triFaces_size,
   int* quadFaces, int quadFaces_size
)
{
   CHECK_PTR(triFaces);
   CHECK_PTR(quadFaces);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("unstruc", avf->mesh_hdrs[meshid].meshType, strlen("unstruc"))) return 1;
      
   const unstruc_header& hdr = avf->unstruc[meshid].header;

// validate size of face buffers
// (this is unstruc format revision 0)
   if (triFaces_size != (5*(hdr.nBndTriFaces+hdr.nTriFaces-hdr.nBndTriFaces))) return 1;
   if (quadFaces_size != (6*(hdr.nBndQuadFaces+hdr.nQuadFaces-hdr.nBndQuadFaces))) return 1;

// byte swap
   if (avf->byte_swap) for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
   if (avf->byte_swap) for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);

// Tri patch faces
   for (int i=0; i<hdr.nTriFaces; ++i) {
      if (triFaces[(i*5)+4] < 0) {
         if (!fwrite(triFaces+(i*5), sizeof(int)*5, 1, avf->fp)) return 1;
      }
   }

// Quad patch faces
   for (int i=0; i<hdr.nQuadFaces; ++i) {
      if (quadFaces[(i*6)+5] < 0) {
         if (!fwrite(quadFaces+(i*6), sizeof(int)*6, 1, avf->fp)) return 1;
      }
   }

// Tri interior faces
   for (int i=0; i<hdr.nTriFaces; ++i) {
      if (triFaces[(i*5)+4] > 0) {
         if (!fwrite(triFaces+(i*5), sizeof(int)*5, 1, avf->fp)) return 1;
      }
   }

// Quad interior faces
   for (int i=0; i<hdr.nQuadFaces; ++i) {
      if (quadFaces[(i*6)+5] > 0) {
         if (!fwrite(quadFaces+(i*6), sizeof(int)*6, 1, avf->fp)) return 1;
      }
   }

// byte swap
   if (avf->byte_swap) for (int i=0; i<triFaces_size; ++i) byte_swap_int(triFaces+i);
   if (avf->byte_swap) for (int i=0; i<quadFaces_size; ++i) byte_swap_int(quadFaces+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 December 18, 2010 - Initial creation
*/
int rev0::avm_cart_read_iblank(rev0_avmesh_file* avf, int* iblanks, int size)
{
   CHECK_PTR(iblanks);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   int blockid = avf->selection_blockid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"))) return 1;
   if (!(0 <= blockid && blockid < avf->cart[meshid].header.nBlocks)) return 1; 
   int* d = avf->cart[meshid].blocks[blockid].iblDim;
   if (d[0]*d[1]*d[2] != size) return 1;

   if (!fread(iblanks, sizeof(int)*size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<size; ++i)
         byte_swap_int(iblanks+i);

   return 0;
}

/**
@author
 Josh Calahan
@history
 January 21, 2011 - Initial creation
*/
int rev0::avm_cart_write_iblank(rev0_avmesh_file* avf, int* iblanks, int size)
{
   CHECK_PTR(iblanks);
   if (!avf) return 1;

   int meshid = avf->selection_meshid-1; 
   int blockid = avf->selection_blockid-1; 
   if (!(0 <= meshid && meshid < avf->file_hdr.meshCount)) return 1; 
   if (0!=strncmp("cart", avf->mesh_hdrs[meshid].meshType, strlen("cart"))) return 1;
   if (!(0 <= blockid && blockid < avf->cart[meshid].header.nBlocks)) return 1; 
   int* d = avf->cart[meshid].blocks[blockid].iblDim;
   if (d[0]*d[1]*d[2] != size) return 1;

   if (avf->byte_swap)
      for (int i=0; i<size; ++i)
         byte_swap_int(iblanks+i);

   if (!fwrite(iblanks, sizeof(int)*size, 1, avf->fp)) return 1;

   if (avf->byte_swap)
      for (int i=0; i<size; ++i)
         byte_swap_int(iblanks+i);

   return 0;
}


