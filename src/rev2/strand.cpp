
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <string>
#include "avmhdrs.h"
#include "strand.h"
#include "../avmesh.h"

using namespace std;
using namespace rev2;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

strand_header_t::strand_header_t()
{
   surfaceOnly = 0;
   nNodes = 0;
   nStrands = 0;
   nTriFaces = 0;
   nQuadFaces = 0;
   nBndEdges = 0;
   nPtsPerStrand = 0;
   nSurfPatches = 0;
   nEdgePatches = 0;
   strandLength = 0.0;
   stretchRatio = 0.0;
   smoothingThreshold = 0.0;
   strcpy(strandDistribution, "");
}

int strand_header_t::size() const
{
   return sizeof(surfaceOnly) +
          sizeof(nNodes) +
          sizeof(nStrands) +
          sizeof(nTriFaces) +
          sizeof(nQuadFaces) +
          sizeof(nPolyFaces) +
          sizeof(polyFacesSize) +
          sizeof(nBndEdges) +
          sizeof(nPtsPerStrand) +
          sizeof(nSurfPatches) +
          sizeof(nEdgePatches) +
          sizeof(strandLength) +
          sizeof(stretchRatio) +
          sizeof(smoothingThreshold) +
          sizeof(strandDistribution) +
          sizeof(nNodesOnGeometry) +
          sizeof(nEdgesOnGeometry) +
          sizeof(nFacesOnGeometry);
;
}

int strand_header_t::write(FILE* fp, bool swap, strand_header_t* p)
{
   if (swap) byte_swap_int(&p->surfaceOnly);
   if (swap) byte_swap_int(&p->nNodes);
   if (swap) byte_swap_int(&p->nStrands);
   if (swap) byte_swap_int(&p->nTriFaces);
   if (swap) byte_swap_int(&p->nQuadFaces);
   if (swap) byte_swap_int(&p->nPolyFaces);
   if (swap) byte_swap_int(&p->polyFacesSize);
   if (swap) byte_swap_int(&p->nBndEdges);
   if (swap) byte_swap_int(&p->nPtsPerStrand);
   if (swap) byte_swap_int(&p->nSurfPatches);
   if (swap) byte_swap_int(&p->nEdgePatches);
   if (swap) byte_swap_double(&p->strandLength);
   if (swap) byte_swap_double(&p->stretchRatio);
   if (swap) byte_swap_double(&p->smoothingThreshold);
   if (swap) byte_swap_int(&p->nNodesOnGeometry);
   if (swap) byte_swap_int(&p->nEdgesOnGeometry);
   if (swap) byte_swap_int(&p->nFacesOnGeometry);

   if (!fwrite(&p->surfaceOnly, sizeof(p->surfaceOnly), 1, fp)) return 0;
   if (!fwrite(&p->nNodes, sizeof(p->nNodes), 1, fp)) return 0;
   if (!fwrite(&p->nStrands, sizeof(p->nStrands), 1, fp)) return 0;
   if (!fwrite(&p->nTriFaces, sizeof(p->nTriFaces), 1, fp)) return 0;
   if (!fwrite(&p->nQuadFaces, sizeof(p->nQuadFaces), 1, fp)) return 0;
   if (!fwrite(&p->nPolyFaces, sizeof(p->nPolyFaces), 1, fp)) return 0;
   if (!fwrite(&p->polyFacesSize, sizeof(p->polyFacesSize), 1, fp)) return 0;
   if (!fwrite(&p->nBndEdges, sizeof(p->nBndEdges), 1, fp)) return 0;
   if (!fwrite(&p->nPtsPerStrand, sizeof(p->nPtsPerStrand), 1, fp)) return 0;
   if (!fwrite(&p->nSurfPatches, sizeof(p->nSurfPatches), 1, fp)) return 0;
   if (!fwrite(&p->nEdgePatches, sizeof(p->nEdgePatches), 1, fp)) return 0;
   if (!fwrite(&p->strandLength, sizeof(p->strandLength), 1, fp)) return 0;
   if (!fwrite(&p->stretchRatio, sizeof(p->stretchRatio), 1, fp)) return 0;
   if (!fwrite(&p->smoothingThreshold, sizeof(p->smoothingThreshold), 1, fp)) return 0;
   if (!fwrite(&p->strandDistribution, sizeof(p->strandDistribution), 1, fp)) return 0;
   if (!fwrite(&p->nNodesOnGeometry, sizeof(p->nNodesOnGeometry), 1, fp)) return 0;
   if (!fwrite(&p->nEdgesOnGeometry, sizeof(p->nEdgesOnGeometry), 1, fp)) return 0;
   if (!fwrite(&p->nFacesOnGeometry, sizeof(p->nFacesOnGeometry), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->surfaceOnly);
   if (swap) byte_swap_int(&p->nNodes);
   if (swap) byte_swap_int(&p->nStrands);
   if (swap) byte_swap_int(&p->nTriFaces);
   if (swap) byte_swap_int(&p->nQuadFaces);
   if (swap) byte_swap_int(&p->nPolyFaces);
   if (swap) byte_swap_int(&p->polyFacesSize);
   if (swap) byte_swap_int(&p->nBndEdges);
   if (swap) byte_swap_int(&p->nPtsPerStrand);
   if (swap) byte_swap_int(&p->nSurfPatches);
   if (swap) byte_swap_int(&p->nEdgePatches);
   if (swap) byte_swap_double(&p->strandLength);
   if (swap) byte_swap_double(&p->stretchRatio);
   if (swap) byte_swap_double(&p->smoothingThreshold);
   if (swap) byte_swap_int(&p->nNodesOnGeometry);
   if (swap) byte_swap_int(&p->nEdgesOnGeometry);
   if (swap) byte_swap_int(&p->nFacesOnGeometry);

   return 1;
}

int strand_header_t::read(FILE* fp, bool swap, strand_header_t* p)
{
   if (!fread(&p->surfaceOnly, sizeof(p->surfaceOnly), 1, fp)) return 0;
   if (!fread(&p->nNodes, sizeof(p->nNodes), 1, fp)) return 0;
   if (!fread(&p->nStrands, sizeof(p->nStrands), 1, fp)) return 0;
   if (!fread(&p->nTriFaces, sizeof(p->nTriFaces), 1, fp)) return 0;
   if (!fread(&p->nQuadFaces, sizeof(p->nQuadFaces), 1, fp)) return 0;
   if (!fread(&p->nPolyFaces, sizeof(p->nPolyFaces), 1, fp)) return 0;
   if (!fread(&p->polyFacesSize, sizeof(p->polyFacesSize), 1, fp)) return 0;
   if (!fread(&p->nBndEdges, sizeof(p->nBndEdges), 1, fp)) return 0;
   if (!fread(&p->nPtsPerStrand, sizeof(p->nPtsPerStrand), 1, fp)) return 0;
   if (!fread(&p->nSurfPatches, sizeof(p->nSurfPatches), 1, fp)) return 0;
   if (!fread(&p->nEdgePatches, sizeof(p->nEdgePatches), 1, fp)) return 0;
   if (!fread(&p->strandLength, sizeof(p->strandLength), 1, fp)) return 0;
   if (!fread(&p->stretchRatio, sizeof(p->stretchRatio), 1, fp)) return 0;
   if (!fread(&p->smoothingThreshold, sizeof(p->smoothingThreshold), 1, fp)) return 0;
   if (!fread(&p->strandDistribution, sizeof(p->strandDistribution), 1, fp)) return 0;
   if (!fread(&p->nNodesOnGeometry, sizeof(p->nNodesOnGeometry), 1, fp)) return 0;
   if (!fread(&p->nEdgesOnGeometry, sizeof(p->nEdgesOnGeometry), 1, fp)) return 0;
   if (!fread(&p->nFacesOnGeometry, sizeof(p->nFacesOnGeometry), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->surfaceOnly);
   if (swap) byte_swap_int(&p->nStrands);
   if (swap) byte_swap_int(&p->nTriFaces);
   if (swap) byte_swap_int(&p->nQuadFaces);
   if (swap) byte_swap_int(&p->nPolyFaces);
   if (swap) byte_swap_int(&p->polyFacesSize);
   if (swap) byte_swap_int(&p->nBndEdges);
   if (swap) byte_swap_int(&p->nPtsPerStrand);
   if (swap) byte_swap_int(&p->nSurfPatches);
   if (swap) byte_swap_int(&p->nEdgePatches);
   if (swap) byte_swap_double(&p->strandLength);
   if (swap) byte_swap_double(&p->stretchRatio);
   if (swap) byte_swap_double(&p->smoothingThreshold);
   if (swap) byte_swap_int(&p->nNodesOnGeometry);
   if (swap) byte_swap_int(&p->nEdgesOnGeometry);
   if (swap) byte_swap_int(&p->nFacesOnGeometry);

   return 1;
}

strand_surf_patch_t::strand_surf_patch_t()
{
   surfPatchId = 0;
   strncpy(surfPatchBody, "", sizeof(surfPatchBody));
   strncpy(surfPatchComp, "", sizeof(surfPatchComp));
   strncpy(surfPatchBCType, "", sizeof(surfPatchBCType));
}

int strand_surf_patch_t::size() const
{
   return sizeof(surfPatchId) +
          sizeof(surfPatchBody) +
          sizeof(surfPatchComp) +
          sizeof(surfPatchComp);
}

int strand_surf_patch_t::writen(FILE* fp, bool swap, vector<strand_surf_patch_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (swap) byte_swap_int(&p[i].surfPatchId);

      if (!fwrite(&p[i].surfPatchId, sizeof(p[i].surfPatchId), 1, fp)) return 0;
      if (!fwrite(&p[i].surfPatchBody, sizeof(p[i].surfPatchBody), 1, fp)) return 0;
      if (!fwrite(&p[i].surfPatchComp, sizeof(p[i].surfPatchComp), 1, fp)) return 0;
      if (!fwrite(&p[i].surfPatchBCType, sizeof(p[i].surfPatchComp), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].surfPatchId);
   }

   return 1;
}

int strand_surf_patch_t::readn(FILE* fp, bool swap, vector<strand_surf_patch_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (!fread(&p[i].surfPatchId, sizeof(p[i].surfPatchId), 1, fp)) return 0;
      if (!fread(&p[i].surfPatchBody, sizeof(p[i].surfPatchBody), 1, fp)) return 0;
      if (!fread(&p[i].surfPatchComp, sizeof(p[i].surfPatchComp), 1, fp)) return 0;
      if (!fread(&p[i].surfPatchBCType, sizeof(p[i].surfPatchComp), 1, fp)) return 0;
      
      if (swap) byte_swap_int(&p[i].surfPatchId);
   }

   return 1;
}

strand_edge_patch_t::strand_edge_patch_t()
{
   edgePatchId = 0;
   strncpy(edgePatchBody, "", sizeof(edgePatchBody));
   strncpy(edgePatchComp, "", sizeof(edgePatchComp));
   strncpy(edgePatchBCType, "", sizeof(edgePatchBCType));
   nx = 0.0;
   ny = 0.0;
   nz = 0.0;
}

int strand_edge_patch_t::size() const
{
   return sizeof(edgePatchId) +
          sizeof(edgePatchBody) +
          sizeof(edgePatchComp) +
          sizeof(edgePatchBCType) +
          sizeof(nx) +
          sizeof(ny) +
          sizeof(nz);
}

int strand_edge_patch_t::writen(FILE* fp, bool swap, vector<strand_edge_patch_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (swap) byte_swap_int(&p[i].edgePatchId);
      if (swap) byte_swap_double(&p[i].nx);
      if (swap) byte_swap_double(&p[i].ny);
      if (swap) byte_swap_double(&p[i].nz);

      if (!fwrite(&p[i].edgePatchId, sizeof(p[i].edgePatchId), 1, fp)) return 0;
      if (!fwrite(&p[i].edgePatchBody, sizeof(p[i].edgePatchBody), 1, fp)) return 0;
      if (!fwrite(&p[i].edgePatchComp, sizeof(p[i].edgePatchComp), 1, fp)) return 0;
      if (!fwrite(&p[i].edgePatchBCType, sizeof(p[i].edgePatchBCType), 1, fp)) return 0;
      if (!fwrite(&p[i].nx, sizeof(p[i].nx), 1, fp)) return 0;
      if (!fwrite(&p[i].ny, sizeof(p[i].ny), 1, fp)) return 0;
      if (!fwrite(&p[i].nz, sizeof(p[i].nz), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].edgePatchId);
      if (swap) byte_swap_double(&p[i].nx);
      if (swap) byte_swap_double(&p[i].ny);
      if (swap) byte_swap_double(&p[i].nz);
   }

   return 1;
}

int strand_edge_patch_t::readn(FILE* fp, bool swap, vector<strand_edge_patch_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (!fread(&p[i].edgePatchId, sizeof(p[i].edgePatchId), 1, fp)) return 0;
      if (!fread(&p[i].edgePatchBody, sizeof(p[i].edgePatchBody), 1, fp)) return 0;
      if (!fread(&p[i].edgePatchComp, sizeof(p[i].edgePatchComp), 1, fp)) return 0;
      if (!fread(&p[i].edgePatchBCType, sizeof(p[i].edgePatchBCType), 1, fp)) return 0;
      if (!fread(&p[i].nx, sizeof(p[i].nx), 1, fp)) return 0;
      if (!fread(&p[i].ny, sizeof(p[i].ny), 1, fp)) return 0;
      if (!fread(&p[i].nz, sizeof(p[i].nz), 1, fp)) return 0;
      
      if (swap) byte_swap_int(&p[i].edgePatchId);
      if (swap) byte_swap_double(&p[i].nx);
      if (swap) byte_swap_double(&p[i].ny);
      if (swap) byte_swap_double(&p[i].nz);
   }

   return 1;
}

int strand_info::write(FILE* fp, bool swap, strand_info* p)
{
   // write surf header
   if(!strand_header_t::write(fp, swap, &p->header)) return 0;

   // write surf patches
   if(!strand_surf_patch_t::writen(fp, swap, p->surf_patches)) return 0;

   // write edge patches
   if(!strand_edge_patch_t::writen(fp, swap, p->edge_patches)) return 0;

   return 1;
}

int strand_info::read(FILE* fp, bool swap, strand_info* p)
{
   // read surf header
   if(!strand_header_t::read(fp, swap, &p->header)) return 0;

   // read surf patches
   if (int nSurfPatches = p->header.nSurfPatches) {
      p->surf_patches.resize(nSurfPatches);
      if(!strand_surf_patch_t::readn(fp, swap, p->surf_patches)) return 0;
   }

   // read edge patches
   if (int nEdgePatches = p->header.nEdgePatches) {
      p->edge_patches.resize(nEdgePatches);
      if(!strand_edge_patch_t::readn(fp, swap, p->edge_patches)) return 0;
   }

   return 1;
}

off_t strand_info::hdrsize() const
{
   off_t offset = 0;
   offset += header.size();
   offset += header.nSurfPatches * strand_surf_patch_t().size();
   offset += header.nEdgePatches * strand_edge_patch_t().size();
   return offset;
}

off_t strand_info::datasize(const file_header_t& filehdr,
                            const mesh_header_t& meshhdr) const
{
   int precision = filehdr.precision;

   off_t offset = 0;
   offset += 3 * header.nNodes * 
             (1==precision ? sizeof(float) : sizeof(double)); //xyz
   offset += header.nStrands * sizeof(int); //nodeID
   offset += header.nStrands * sizeof(int); //nodeClip
   offset += header.nPtsPerStrand *
             (1==precision ? sizeof(float) : sizeof(double)); //xStrand
   offset += 3 * header.nStrands *
             (1==precision ? sizeof(float) : sizeof(double)); //pointingVec
   offset += 3 * header.nTriFaces * sizeof(int);
   offset += 4 * header.nQuadFaces * sizeof(int);
   offset += header.polyFacesSize * sizeof(int);
   offset += ( header.nTriFaces + header.nQuadFaces + header.nPolyFaces ) * sizeof(int); //faceTag
   offset += header.nBndEdges * sizeof(int); //edgeTag
   offset += 2 *header.nBndEdges * sizeof(int); //bndEdges
   offset += sizeof(AMR_Node_Data) * header.nNodesOnGeometry;
   offset += 3 * header.nEdgesOnGeometry * sizeof(int);
   offset += 3 * header.nFacesOnGeometry * sizeof(int);
   return offset;
}
