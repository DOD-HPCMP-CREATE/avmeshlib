#ifndef UNSTRUC_REV1_DEFS
#define UNSTRUC_REV1_DEFS

#ifdef __cplusplus
#include <string>
#include <vector> 
#endif

#include <sys/types.h>

namespace rev1 {

/** @page unstruc_rev1 Unstructured Data Format

@section Overview

   This document describes the AVMesh unstructured grid format.

@section unstruc_header Unstructured Header Format

<pre>
   nNodes
   nFaces
   nCells
   nMaxNodesPerFace
   nMaxNodesPerCell
   nMaxFacesPerCell
   nPatches
   nHexCells
   nTetCells
   nPriCells
   nPyrCells
   nPolyCells
   nBndTriFaces
   nTriFaces
   nBndQuadFaces
   nQuadFaces
   nBndPolyFaces
   nPolyFaces
   bndPolyFacesSize
   polyFacesSize
   nEdges
   nNodesOnGeometry
   nEdgesOnGeometry
   nFacesOnGeometry
   geomRegionId
</pre>

   For additional details see @ref unstruc_header_t.

   Following the main part of the unstruc header are <code>nPatches</code> patch
   headers:

<pre>
   patchLabel
   patchType
   patchId
</pre>

   For additional details see @ref unstruc_patchheader_t.

@section unstruc_mesh Unstructured Mesh Format

   In Fortran the mesh format is written as:
<pre>
   write(ui) ((xyz(i,j),i=1,3),j=1,nNodes)
   write(ui) ((triFaces(i,j),i=1,5),j=1,nTriFaces)
   write(ui) ((quadFaces(i,j),i=1,6),j=1,nQuadFaces)
   write(ui) (polyFaces(i,j),i=1,polyFacesSize)
   write(ui) ((hexCells(i,j),i=1,8),j=1,nHexCells)
   write(ui) ((tetCells(i,j),i=1,4),j=1,nTetCells)
   write(ui) ((priCells(i,j),i=1,6),j=1,nPriCells)
   write(ui) ((pyrCells(i,j),i=1,5),j=1,nPyrCells)
   write(ui) ((edges(i,j),i=1,2),j=1,nEdges)
   write(ui) (nodesOnGeometry(i),i=1,nNodesOnGeometry)
   write(ui) ((edgesOnGeometry(i,j),i=1,3),j=1,nEdgesOnGeometry)
   write(ui) ((facesOnGeometry(i,j),i=1,3),j=1,nFacesOnGeometry)
   write(ui) (hexGeomIds(ij),i=1,nHexCells)
   write(ui) (tetGeomIds(ij),i=1,nTetCells)
   write(ui) (priGeomIds(ij),i=1,nPriCells)
   write(ui) (pyrGeomIds(ij),i=1,nPyrCells)
</pre>

   Node data may be written as single or double-precision.
*/

/*! @struct unstruc_header_t
 * The unstructured header record.
 */
typedef struct unstruc_header_t {
/*! number of nodes */
   int nNodes;
/*! number of faces */
   int nFaces;
/*! total number of cells */
   int nCells;
/*! max number of nodes in any face */
   int nMaxNodesPerFace;
/*! max number of nodes in any cell */
   int nMaxNodesPerCell;
/*! max number of faces in any cell */
   int nMaxFacesPerCell;
/*! number of domain boundary patches */
   int nPatches;
/*! number of hex cells */
   int nHexCells;
/*! number of tet cells */
   int nTetCells;
/*! number of prism cells */
   int nPriCells;
/*! number of pryamid cells */
   int nPyrCells;
/*! number of polyhedral cells */
   int nPolyCells;
/*! number of tri faces on boundaries */
   int nBndTriFaces;
/*! total number of tri faces */
   int nTriFaces;
/*! number of quad faces on boundaries */
   int nBndQuadFaces;
/*! total number of quad faces */
   int nQuadFaces;
/*! number of polyhedral faces on boundaries */
   int nBndPolyFaces;
/*! total number of polyhedral faces */
   int nPolyFaces;
/*! Number of elements in the boundary faces portion of the polyhedral faces array */
   int bndPolyFacesSize;
/*! Number of elements in the polyhedral faces array */
   int polyFacesSize;
/*! number of edges */
   int nEdges;
/*! number of nodes classified against geometry */
   int nNodesOnGeometry;
/*! number of edges classified against geometry */
   int nEdgesOnGeometry;
/*! number of faces classified against geometry */
   int nFacesOnGeometry;
/*! geometry region ID for the entire volume mesh.
    If <0, ignored and avm_unstruc_amr_volumeids can be used */
   int geomRegionId;
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

#ifdef __cplusplus
struct unstruc_info {
   unstruc_header header;
   std::vector<unstruc_patchheader> patches;
   std::vector<int> faces_reordering;
   static int write(FILE* fp, bool swap, unstruc_info*);
   static int read(FILE* fp, bool swap, unstruc_info*);
   off_t hdrsize() const;
   off_t datasize(const file_header_t&, const mesh_header_t&) const;
};
#endif

} //namespace rev1

#endif  /* UNSTRUC_REV1_DEFS */
