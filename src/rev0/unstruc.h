#ifndef UNSTRUC_REV0_DEFS
#define UNSTRUC_REV0_DEFS

/** @page unstruc_rev0 Unstructured Data Format

@section Overview

   This document describes the AVMesh unstructured grid format.

@section unstruc_header Unstructured Header Format

<pre>
   fullViscousLayerCount
   partialViscousLayerCount
   nNodes
   nEdges
   nFaces
   nMaxNodesPerFace
   nTriFaces
   nQuadFaces
   nBndTriFaces
   nBndQuadFaces
   nCells
   nMaxNodesPerCell
   nMaxFacesPerCell
   nHexCells
   nTetCells
   nPriCells
   nPyrCells
   nPatches
   nPatchNodes
   nPatchFaces
</pre>

   For additional details see @ref unstruc_header_t.

   Following the main part of the unstruc header are <code>nPatches</code> patch
   headers:

<pre>
   patchLabel
   patchType
   patchId
   nUniqueNodes
   nUniqueFaces
</pre>

   For additional details see @ref unstruc_patchheader_t.

@section unstruc_mesh Unstructured Mesh Format

   In Fortran the mesh format is written as:
<pre>
   write(ui) ((xyz(i,j),i=1,3),j=1,nNodes)
   write(ui) ((quadFaces(i,j),i=1,6),j=1,nQuadFaces)
   write(ui) ((triFaces(i,j),i=1,5),j=1,nTriFaces)
   write(ui) ((hexCells(i,j),i=1,8),j=1,nHexCells)
   write(ui) ((tetCells(i,j),i=1,4),j=1,nTetCells)
   write(ui) ((priCells(i,j),i=1,6),j=1,nPriCells)
   write(ui) ((pyrCells(i,j),i=1,5),j=1,nPyrCells)</pre>

   Node data may be written as single or double-precision.
*/

#ifdef __cplusplus
#include <string>
#include <vector> 
#endif

#include <sys/types.h>

namespace rev0 {

/*! @struct unstruc_header_t
 * The unstructured header record.
 */
typedef struct unstruc_header_t {
/*! number of full cell layers on the no-slip solid wall boundaries */
   int fullViscousLayerCount;
/*! number of partial cell layers on the no-slip solid wall boundaries */
   int partialViscousLayerCount;
/*! number of nodes */
   int nNodes;
/*! number of edges */
   int nEdges;
/*! number of faces */
   int nFaces;
/*! max number of nodes in any face */
   int nMaxNodesPerFace;
/*! total number of tri faces */
   int nTriFaces;
/*! total number of quad faces */
   int nQuadFaces;
/*! number of tri faces on boundaries */
   int nBndTriFaces;
/*! number of quad faces on boundaries */
   int nBndQuadFaces;
/*! total number of cells */
   int nCells;
/*! max number of nodes in any cell */
   int nMaxNodesPerCell;
/*! max number of faces in any cell */
   int nMaxFacesPerCell;
/*! number of hex cells */
   int nHexCells;
/*! number of tet cells */
   int nTetCells;
/*! number of prism cells */
   int nPriCells;
/*! number of pryamid cells */
   int nPyrCells;
/*! number of domain boundary patches */
   int nPatches;
/*! total number of patch nodes */
   int nPatchNodes;
/*! total number of patch nodes */
   int nPatchFaces;
#ifdef __cplusplus
   unstruc_header_t();
   int size() const;
   static int write(FILE* fp, bool swap, unstruc_header_t*);
   static int read(FILE* fp, bool swap, unstruc_header_t*);
#endif
} unstruc_header;

/*! @struct unstruc_patchheader_t
 * The unstructured patch header record.
 */
typedef struct unstruc_patchheader_t {
/*! name of the boundary patch */
   char patchLabel[32];
/*! boundary condition type for the boundary patch */
   char patchType[16];
/*! id number in connectivity data for the boundary patch */
   int patchId;
#ifdef __cplusplus
   unstruc_patchheader_t();
   int size() const;
   static int writen(FILE* fp, bool swap, std::vector<unstruc_patchheader_t>&);
   static int readn(FILE* fp, bool swap, std::vector<unstruc_patchheader_t>&);
#endif
} unstruc_patchheader;

struct unstruc_info {
   int a;
   unstruc_header header;
   std::vector<unstruc_patchheader> patches;
   static int write(FILE* fp, bool swap, unstruc_info*);
   static int read(FILE* fp, bool swap, unstruc_info*);
   off_t hdrsize() const;
   off_t datasize(const file_header_t&, const mesh_header_t&) const;
};

} //namespace rev0

#endif  /* UNSTRUC_REV0_DEFS */
