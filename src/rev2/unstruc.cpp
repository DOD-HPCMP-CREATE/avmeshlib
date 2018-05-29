
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <string>

#include "avmhdrs.h"
#include "unstruc.h"
#include "../avmesh.h"

#include "../avmesh_file.h"
#include "avmesh_rev2.h"

using namespace std;
using namespace rev2;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_int64(int64_t*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

unstruc_header_t::unstruc_header_t()
{
   nNodes = 0;
   nFaces = 0;
   nCells = 0;
   nMaxNodesPerFace = 0;
   nMaxNodesPerCell = 0;
   nMaxFacesPerCell = 0;
   strncpy(elementScheme, "uniform", sizeof(elementScheme));
   facePolyOrder = 1;
   cellPolyOrder = 1;
   nPatches = 0;
   nHexCells = 0;
   nTetCells = 0;
   nPriCells = 0;
   nPyrCells = 0;
   nBndTriFaces = 0;
   nTriFaces = 0;
   nBndQuadFaces = 0;
   nQuadFaces = 0;
   nEdges = 0;
   nNodesOnGeometry = 0;
   nEdgesOnGeometry = 0;
   nFacesOnGeometry = 0;
   geomRegionId = 0;
}

int unstruc_header_t::size() const
{
   return sizeof(nNodes) +
          sizeof(nFaces) +
          sizeof(nCells) +
          sizeof(nMaxNodesPerFace) +
          sizeof(nMaxNodesPerCell) +
          sizeof(nMaxFacesPerCell) +
          sizeof(elementScheme) +
          sizeof(facePolyOrder) +
          sizeof(cellPolyOrder) +
          sizeof(nPatches) +
          sizeof(nHexCells) +
          sizeof(nTetCells) +
          sizeof(nPriCells) +
          sizeof(nPyrCells) +
          sizeof(nBndTriFaces) +
          sizeof(nTriFaces) +
          sizeof(nBndQuadFaces) +
          sizeof(nQuadFaces) +
          sizeof(nEdges) +
          sizeof(nNodesOnGeometry) +
          sizeof(nEdgesOnGeometry) +
          sizeof(nFacesOnGeometry) +
          sizeof(geomRegionId);
}

void unstruc_byte_swap_header(unstruc_header_t* p)
{
   byte_swap_int(&p->nNodes);
   byte_swap_int(&p->nFaces);
   byte_swap_int(&p->nCells);
   byte_swap_int(&p->nMaxNodesPerFace);
   byte_swap_int(&p->nMaxNodesPerCell);
   byte_swap_int(&p->nMaxFacesPerCell);
   byte_swap_int(&p->facePolyOrder);
   byte_swap_int(&p->cellPolyOrder);
   byte_swap_int(&p->nPatches);
   byte_swap_int(&p->nHexCells);
   byte_swap_int(&p->nTetCells);
   byte_swap_int(&p->nPriCells);
   byte_swap_int(&p->nPyrCells);
   byte_swap_int(&p->nBndTriFaces);
   byte_swap_int(&p->nTriFaces);
   byte_swap_int(&p->nBndQuadFaces);
   byte_swap_int(&p->nQuadFaces);
   byte_swap_int(&p->nEdges);
   byte_swap_int(&p->nNodesOnGeometry);
   byte_swap_int(&p->nEdgesOnGeometry);
   byte_swap_int(&p->nFacesOnGeometry);
   byte_swap_int(&p->geomRegionId);
}

int unstruc_header_t::write(FILE* fp, bool swap, unstruc_header_t* p)
{
   if (swap) unstruc_byte_swap_header(p);

   if (!fwrite(&p->nNodes, sizeof(p->nNodes), 1, fp)) return 0;
   if (!fwrite(&p->nFaces, sizeof(p->nFaces), 1, fp)) return 0;
   if (!fwrite(&p->nCells, sizeof(p->nCells), 1, fp)) return 0;
   if (!fwrite(&p->nMaxNodesPerFace, sizeof(p->nMaxNodesPerFace), 1, fp)) return 0;
   if (!fwrite(&p->nMaxNodesPerCell, sizeof(p->nMaxNodesPerCell), 1, fp)) return 0;
   if (!fwrite(&p->nMaxFacesPerCell, sizeof(p->nMaxFacesPerCell), 1, fp)) return 0;
   if (!fwrite(&p->elementScheme, sizeof(p->elementScheme), 1, fp)) return 0;
   if (!fwrite(&p->facePolyOrder, sizeof(p->facePolyOrder), 1, fp)) return 0;
   if (!fwrite(&p->cellPolyOrder, sizeof(p->cellPolyOrder), 1, fp)) return 0;
   if (!fwrite(&p->nPatches, sizeof(p->nPatches), 1, fp)) return 0;
   if (!fwrite(&p->nHexCells, sizeof(p->nHexCells), 1, fp)) return 0;
   if (!fwrite(&p->nTetCells, sizeof(p->nTetCells), 1, fp)) return 0;
   if (!fwrite(&p->nPriCells, sizeof(p->nPriCells), 1, fp)) return 0;
   if (!fwrite(&p->nPyrCells, sizeof(p->nPyrCells), 1, fp)) return 0;
   if (!fwrite(&p->nBndTriFaces, sizeof(p->nBndTriFaces), 1, fp)) return 0;
   if (!fwrite(&p->nTriFaces, sizeof(p->nTriFaces), 1, fp)) return 0;
   if (!fwrite(&p->nBndQuadFaces, sizeof(p->nBndQuadFaces), 1, fp)) return 0;
   if (!fwrite(&p->nQuadFaces, sizeof(p->nQuadFaces), 1, fp)) return 0;
   if (!fwrite(&p->nEdges, sizeof(p->nEdges), 1, fp)) return 0;
   if (!fwrite(&p->nNodesOnGeometry, sizeof(p->nNodesOnGeometry), 1, fp)) return 0;
   if (!fwrite(&p->nEdgesOnGeometry, sizeof(p->nEdgesOnGeometry), 1, fp)) return 0;
   if (!fwrite(&p->nFacesOnGeometry, sizeof(p->nFacesOnGeometry), 1, fp)) return 0;
   if (!fwrite(&p->geomRegionId, sizeof(p->geomRegionId), 1, fp)) return 0;

   if (swap) unstruc_byte_swap_header(p);

   return 1;
}

int unstruc_header_t::read(FILE* fp, bool swap, unstruc_header_t* p)
{
   if (!fread(&p->nNodes, sizeof(p->nNodes), 1, fp)) return 0;
   if (!fread(&p->nFaces, sizeof(p->nFaces), 1, fp)) return 0;
   if (!fread(&p->nCells, sizeof(p->nCells), 1, fp)) return 0;
   if (!fread(&p->nMaxNodesPerFace, sizeof(p->nMaxNodesPerFace), 1, fp)) return 0;
   if (!fread(&p->nMaxNodesPerCell, sizeof(p->nMaxNodesPerCell), 1, fp)) return 0;
   if (!fread(&p->nMaxFacesPerCell, sizeof(p->nMaxFacesPerCell), 1, fp)) return 0;
   if (!fread(&p->elementScheme, sizeof(p->elementScheme), 1, fp)) return 0;
   if (!fread(&p->facePolyOrder, sizeof(p->facePolyOrder), 1, fp)) return 0;
   if (!fread(&p->cellPolyOrder, sizeof(p->cellPolyOrder), 1, fp)) return 0;
   if (!fread(&p->nPatches, sizeof(p->nPatches), 1, fp)) return 0;
   if (!fread(&p->nHexCells, sizeof(p->nHexCells), 1, fp)) return 0;
   if (!fread(&p->nTetCells, sizeof(p->nTetCells), 1, fp)) return 0;
   if (!fread(&p->nPriCells, sizeof(p->nPriCells), 1, fp)) return 0;
   if (!fread(&p->nPyrCells, sizeof(p->nPyrCells), 1, fp)) return 0;
   if (!fread(&p->nBndTriFaces, sizeof(p->nBndTriFaces), 1, fp)) return 0;
   if (!fread(&p->nTriFaces, sizeof(p->nTriFaces), 1, fp)) return 0;
   if (!fread(&p->nBndQuadFaces, sizeof(p->nBndQuadFaces), 1, fp)) return 0;
   if (!fread(&p->nQuadFaces, sizeof(p->nQuadFaces), 1, fp)) return 0;
   if (!fread(&p->nEdges, sizeof(p->nEdges), 1, fp)) return 0;
   if (!fread(&p->nNodesOnGeometry, sizeof(p->nNodesOnGeometry), 1, fp)) return 0;
   if (!fread(&p->nEdgesOnGeometry, sizeof(p->nEdgesOnGeometry), 1, fp)) return 0;
   if (!fread(&p->nFacesOnGeometry, sizeof(p->nFacesOnGeometry), 1, fp)) return 0;
   if (!fread(&p->geomRegionId, sizeof(p->geomRegionId), 1, fp)) return 0;

   if (swap) unstruc_byte_swap_header(p);

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

   int nodesPerTri = rev2::avm_nodes_per_tri(header.facePolyOrder) + 1;
   int nodesPerQuad = rev2::avm_nodes_per_quad(header.facePolyOrder) + 1;
   int nodesPerHex = rev2::avm_nodes_per_hex(header.cellPolyOrder);
   int nodesPerTet = rev2::avm_nodes_per_tet(header.cellPolyOrder);
   int nodesPerPri = rev2::avm_nodes_per_pri(header.cellPolyOrder);
   int nodesPerPyr = rev2::avm_nodes_per_pyr(header.cellPolyOrder);

   off_t offset = 0;
   offset += 3 * header.nNodes *
             (1==precision ? sizeof(float) : sizeof(double));
   offset += nodesPerTri * header.nBndTriFaces * sizeof(int);
   offset += nodesPerQuad * header.nBndQuadFaces * sizeof(int);
   offset += nodesPerTri * (header.nTriFaces - header.nBndTriFaces) * sizeof(int);
   offset += nodesPerQuad * (header.nQuadFaces - header.nBndQuadFaces) * sizeof(int);
   offset += nodesPerHex * header.nHexCells * sizeof(int);
   offset += nodesPerTet * header.nTetCells * sizeof(int);
   offset += nodesPerPri * header.nPriCells * sizeof(int);
   offset += nodesPerPyr * header.nPyrCells * sizeof(int);
   offset += 2 * header.nEdges * sizeof(int);
   offset += sizeof(AMR_Node_Data) * header.nNodesOnGeometry;
   offset += 3 * header.nEdgesOnGeometry * sizeof(int);
   offset += 3 * header.nFacesOnGeometry * sizeof(int);
   offset += header.nHexCells * sizeof(int);
   offset += header.nTetCells * sizeof(int);
   offset += header.nPriCells * sizeof(int);
   offset += header.nPyrCells * sizeof(int);
   return offset;
}
