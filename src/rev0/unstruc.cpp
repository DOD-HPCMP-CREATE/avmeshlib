
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <string>
#include "avmhdrs.h"
#include "unstruc.h"
using namespace std;
using namespace rev0;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

unstruc_header_t::unstruc_header_t()
{
   fullViscousLayerCount = 0;
   partialViscousLayerCount = 0;
   nNodes = 0;
   nEdges = 0;
   nFaces = 0;
   nMaxNodesPerFace = 0;
   nTriFaces = 0;
   nQuadFaces = 0;
   nBndTriFaces = 0;
   nBndQuadFaces = 0;
   nCells = 0;
   nMaxNodesPerCell = 0;
   nMaxFacesPerCell = 0;
   nHexCells = 0;
   nTetCells = 0;
   nPriCells = 0;
   nPyrCells = 0;
   nPatches = 0;
   nPatchNodes = 0;
   nPatchFaces = 0;
}

int unstruc_header_t::size() const
{
   return sizeof(fullViscousLayerCount) +
          sizeof(partialViscousLayerCount) +
          sizeof(nNodes) +
          sizeof(nEdges) +
          sizeof(nFaces) +
          sizeof(nMaxNodesPerFace) +
          sizeof(nTriFaces) +
          sizeof(nQuadFaces) +
          sizeof(nBndTriFaces) +
          sizeof(nBndQuadFaces) +
          sizeof(nCells) +
          sizeof(nMaxNodesPerCell) +
          sizeof(nMaxFacesPerCell) +
          sizeof(nHexCells) +
          sizeof(nTetCells) +
          sizeof(nPriCells) +
          sizeof(nPyrCells) +
          sizeof(nPatches) +
          sizeof(nPatchNodes) +
          sizeof(nPatchFaces);
}

int unstruc_header_t::write(FILE* fp, bool swap, unstruc_header_t* p)
{
   if (swap) byte_swap_int(&p->fullViscousLayerCount);
   if (swap) byte_swap_int(&p->partialViscousLayerCount);
   if (swap) byte_swap_int(&p->nNodes);
   if (swap) byte_swap_int(&p->nEdges);
   if (swap) byte_swap_int(&p->nFaces);
   if (swap) byte_swap_int(&p->nMaxNodesPerFace);
   if (swap) byte_swap_int(&p->nTriFaces);
   if (swap) byte_swap_int(&p->nQuadFaces);
   if (swap) byte_swap_int(&p->nBndTriFaces);
   if (swap) byte_swap_int(&p->nBndQuadFaces);
   if (swap) byte_swap_int(&p->nCells);
   if (swap) byte_swap_int(&p->nMaxNodesPerCell);
   if (swap) byte_swap_int(&p->nMaxFacesPerCell);
   if (swap) byte_swap_int(&p->nHexCells);
   if (swap) byte_swap_int(&p->nTetCells);
   if (swap) byte_swap_int(&p->nPriCells);
   if (swap) byte_swap_int(&p->nPyrCells);
   if (swap) byte_swap_int(&p->nPatches);
   if (swap) byte_swap_int(&p->nPatchNodes);
   if (swap) byte_swap_int(&p->nPatchFaces);

   if (!fwrite(&p->fullViscousLayerCount, sizeof(p->fullViscousLayerCount), 1, fp)) return 0;
   if (!fwrite(&p->partialViscousLayerCount, sizeof(p->partialViscousLayerCount), 1, fp)) return 0;
   if (!fwrite(&p->nNodes, sizeof(p->nNodes), 1, fp)) return 0;
   if (!fwrite(&p->nEdges, sizeof(p->nEdges), 1, fp)) return 0;
   if (!fwrite(&p->nFaces, sizeof(p->nFaces), 1, fp)) return 0;
   if (!fwrite(&p->nMaxNodesPerFace, sizeof(p->nMaxNodesPerFace), 1, fp)) return 0;
   if (!fwrite(&p->nTriFaces, sizeof(p->nTriFaces), 1, fp)) return 0;
   if (!fwrite(&p->nQuadFaces, sizeof(p->nQuadFaces), 1, fp)) return 0;
   if (!fwrite(&p->nBndTriFaces, sizeof(p->nBndTriFaces), 1, fp)) return 0;
   if (!fwrite(&p->nBndQuadFaces, sizeof(p->nBndQuadFaces), 1, fp)) return 0;
   if (!fwrite(&p->nCells, sizeof(p->nCells), 1, fp)) return 0;
   if (!fwrite(&p->nMaxNodesPerCell, sizeof(p->nMaxNodesPerCell), 1, fp)) return 0;
   if (!fwrite(&p->nMaxFacesPerCell, sizeof(p->nMaxFacesPerCell), 1, fp)) return 0;
   if (!fwrite(&p->nHexCells, sizeof(p->nHexCells), 1, fp)) return 0;
   if (!fwrite(&p->nTetCells, sizeof(p->nTetCells), 1, fp)) return 0;
   if (!fwrite(&p->nPriCells, sizeof(p->nPriCells), 1, fp)) return 0;
   if (!fwrite(&p->nPyrCells, sizeof(p->nPyrCells), 1, fp)) return 0;
   if (!fwrite(&p->nPatches, sizeof(p->nPatches), 1, fp)) return 0;
   if (!fwrite(&p->nPatchNodes, sizeof(p->nPatchNodes), 1, fp)) return 0;
   if (!fwrite(&p->nPatchFaces, sizeof(p->nPatchFaces), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->fullViscousLayerCount);
   if (swap) byte_swap_int(&p->partialViscousLayerCount);
   if (swap) byte_swap_int(&p->nNodes);
   if (swap) byte_swap_int(&p->nEdges);
   if (swap) byte_swap_int(&p->nFaces);
   if (swap) byte_swap_int(&p->nMaxNodesPerFace);
   if (swap) byte_swap_int(&p->nTriFaces);
   if (swap) byte_swap_int(&p->nQuadFaces);
   if (swap) byte_swap_int(&p->nBndTriFaces);
   if (swap) byte_swap_int(&p->nBndQuadFaces);
   if (swap) byte_swap_int(&p->nCells);
   if (swap) byte_swap_int(&p->nMaxNodesPerCell);
   if (swap) byte_swap_int(&p->nMaxFacesPerCell);
   if (swap) byte_swap_int(&p->nHexCells);
   if (swap) byte_swap_int(&p->nTetCells);
   if (swap) byte_swap_int(&p->nPriCells);
   if (swap) byte_swap_int(&p->nPyrCells);
   if (swap) byte_swap_int(&p->nPatches);
   if (swap) byte_swap_int(&p->nPatchNodes);
   if (swap) byte_swap_int(&p->nPatchFaces);

   return 1;
}

int unstruc_header_t::read(FILE* fp, bool swap, unstruc_header_t* p)
{
   if (!fread(&p->fullViscousLayerCount, sizeof(p->fullViscousLayerCount), 1, fp)) return 0;
   if (!fread(&p->partialViscousLayerCount, sizeof(p->partialViscousLayerCount), 1, fp)) return 0;
   if (!fread(&p->nNodes, sizeof(p->nNodes), 1, fp)) return 0;
   if (!fread(&p->nEdges, sizeof(p->nEdges), 1, fp)) return 0;
   if (!fread(&p->nFaces, sizeof(p->nFaces), 1, fp)) return 0;
   if (!fread(&p->nMaxNodesPerFace, sizeof(p->nMaxNodesPerFace), 1, fp)) return 0;
   if (!fread(&p->nTriFaces, sizeof(p->nTriFaces), 1, fp)) return 0;
   if (!fread(&p->nQuadFaces, sizeof(p->nQuadFaces), 1, fp)) return 0;
   if (!fread(&p->nBndTriFaces, sizeof(p->nBndTriFaces), 1, fp)) return 0;
   if (!fread(&p->nBndQuadFaces, sizeof(p->nBndQuadFaces), 1, fp)) return 0;
   if (!fread(&p->nCells, sizeof(p->nCells), 1, fp)) return 0;
   if (!fread(&p->nMaxNodesPerCell, sizeof(p->nMaxNodesPerCell), 1, fp)) return 0;
   if (!fread(&p->nMaxFacesPerCell, sizeof(p->nMaxFacesPerCell), 1, fp)) return 0;
   if (!fread(&p->nHexCells, sizeof(p->nHexCells), 1, fp)) return 0;
   if (!fread(&p->nTetCells, sizeof(p->nTetCells), 1, fp)) return 0;
   if (!fread(&p->nPriCells, sizeof(p->nPriCells), 1, fp)) return 0;
   if (!fread(&p->nPyrCells, sizeof(p->nPyrCells), 1, fp)) return 0;
   if (!fread(&p->nPatches, sizeof(p->nPatches), 1, fp)) return 0;
   if (!fread(&p->nPatchNodes, sizeof(p->nPatchNodes), 1, fp)) return 0;
   if (!fread(&p->nPatchFaces, sizeof(p->nPatchFaces), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->fullViscousLayerCount);
   if (swap) byte_swap_int(&p->partialViscousLayerCount);
   if (swap) byte_swap_int(&p->nNodes);
   if (swap) byte_swap_int(&p->nEdges);
   if (swap) byte_swap_int(&p->nFaces);
   if (swap) byte_swap_int(&p->nMaxNodesPerFace);
   if (swap) byte_swap_int(&p->nTriFaces);
   if (swap) byte_swap_int(&p->nQuadFaces);
   if (swap) byte_swap_int(&p->nBndTriFaces);
   if (swap) byte_swap_int(&p->nBndQuadFaces);
   if (swap) byte_swap_int(&p->nCells);
   if (swap) byte_swap_int(&p->nMaxNodesPerCell);
   if (swap) byte_swap_int(&p->nMaxFacesPerCell);
   if (swap) byte_swap_int(&p->nHexCells);
   if (swap) byte_swap_int(&p->nTetCells);
   if (swap) byte_swap_int(&p->nPriCells);
   if (swap) byte_swap_int(&p->nPyrCells);
   if (swap) byte_swap_int(&p->nPatches);
   if (swap) byte_swap_int(&p->nPatchNodes);
   if (swap) byte_swap_int(&p->nPatchFaces);

   return 1;
}

unstruc_patchheader_t::unstruc_patchheader_t()
{
   strncpy(patchLabel, "", sizeof(patchLabel));
   strncpy(patchType, "", sizeof(patchType));
   patchId = 0;
}

int unstruc_patchheader_t::size() const
{
   return sizeof(patchLabel) +
          sizeof(patchType) +
          sizeof(patchId);
}

int unstruc_patchheader_t::writen(FILE* fp, bool swap, vector<unstruc_patchheader_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (swap) byte_swap_int(&p[i].patchId);

      if (!fwrite(p[i].patchLabel, sizeof(p[i].patchLabel), 1, fp)) return 0;
      if (!fwrite(p[i].patchType, sizeof(p[i].patchType), 1, fp)) return 0;
      if (!fwrite(&p[i].patchId, sizeof(p[i].patchId), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].patchId);
   }

   return 1;
}

int unstruc_patchheader_t::readn(FILE* fp, bool swap, vector<unstruc_patchheader_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (!fread(p[i].patchLabel, sizeof(p[i].patchLabel), 1, fp)) return 0;
      if (!fread(p[i].patchType, sizeof(p[i].patchType), 1, fp)) return 0;
      if (!fread(&p[i].patchId, sizeof(p[i].patchId), 1, fp)) return 0;
      
      if (swap) byte_swap_int(&p[i].patchId);
   }

   return 1;
}

int unstruc_info::write(FILE* fp, bool swap, unstruc_info* p)
{
   // write header
   if(!unstruc_header_t::write(fp, swap, &p->header)) return 0;

   // write patches
   if(!unstruc_patchheader_t::writen(fp, swap, p->patches)) return 0;

   return 1;
}

int unstruc_info::read(FILE* fp, bool swap, unstruc_info* p)
{
   // read header
   if(!unstruc_header_t::read(fp, swap, &p->header)) return 0;

   // read patches
   if (int nPatches = p->header.nPatches) {
      p->patches.resize(nPatches);
      if(!unstruc_patchheader_t::readn(fp, swap, p->patches)) return 0;
   }

   return 1;
}

off_t unstruc_info::hdrsize() const
{
   off_t offset = 0;
   offset += header.size();
   offset += header.nPatches * unstruc_patchheader_t().size();
   return offset;
}

off_t unstruc_info::datasize(const file_header_t& filehdr,
                             const mesh_header_t& meshhdr) const
{
   int precision = filehdr.precision;

   off_t offset = 0;
   offset += 3 * header.nNodes * 
             (1==precision ? sizeof(float) : sizeof(double));
   offset += 5 * header.nBndTriFaces * sizeof(int);
   offset += 6 * header.nBndQuadFaces * sizeof(int);
   offset += 5 * (header.nTriFaces - header.nBndTriFaces) * sizeof(int);
   offset += 6 * (header.nQuadFaces - header.nBndQuadFaces) * sizeof(int);
   return offset;
}
