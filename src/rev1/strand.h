#ifndef STRAND_REV1_DEFS
#define STRAND_REV1_DEFS

#ifdef __cplusplus
#include <string>
#include <vector> 
#endif

#include <sys/types.h>

namespace rev1 {

/** @page strand_rev1 Strand Data Format

@section Overview

   This document describes the AVMesh strand grid format.

@section strand_header Strand Header Format

<pre>
   surfaceOnly
   nNodes
   nStrands
   nTriFaces
   nQuadFaces
   nPolyFaces
   polyFacesSize
   nBndEdges
   nPtsPerStrand
   nSurfPatches
   nEdgePatches
   strandLength
   stretchRatio
   smoothingThreshold
   strandDistribution
   nNodesOnGeometry
   nEdgesOnGeometry
   nFacesOnGeometry
</pre>

   For additional details see @ref strand_header_t.

   Following the main part of the strand header are <code>nSurfPatches</code>
   surface boundary conditions:

<pre>
   surfPatchId
   surfPatchBody
   surfPatchComp
   surfPatchBCType
</pre>

   For additional details see @ref strand_surf_patch_t.

   Following the surface patch data are
   <code>nEdgePatches</code> edge boundary conditions. Edge boundary
   conditions are needed in the case of open (non-watertight) surface
   geometry (e.g. symmetry boundary conditions):

<pre>
   edgePatchId
   edgePatchBody
   edgePatchComp
   edgePatchBCType
   nx
   ny
   nz
</pre>

   For additional details see @ref strand_edge_patch_t.

@section strand_mesh Strand Mesh Format

In Fortran the 3D strand mesh format is written as:

<pre>
   write(iu) ((xyz(k,n),k=1,3),n=1,nNodes)
   write(iu) (nodeID(n),n=1,nStrands)
   write(iu) (nodeClip(n),n=1,nStrands)
   write(iu) ((pointingVec(k,n),k=1,3),n=1,nStrands)
   write(iu) (xStrand(n),n=0,nPtsPerStrand-1)
   write(iu) ((triFaces(k,n),k=1,3),n=1,nTriFaces)
   write(iu) ((quadFaces(k,n),k=1,4),n=1,nQuadFaces)
   write(iu) ((polyFaces(k,n),k=1,4),n=1,polyFacesSize)
   write(iu) (faceTag(n),n=1,nTriFaces+nQuadFaces)
   write(iu) ((bndEdges(k,n),k=1,2),n=1,nBndEdges)
   write(iu) (edgeTag(n),n=1,nBndEdges)
   write(ui) (nodesOnGeometry(i),i=1,nNodesOnGeometry)
   write(ui) ((edgesOnGeometry(i,j),i=1,3),j=1,nEdgesOnGeometry)
   write(ui) ((facesOnGeometry(i,j),i=1,3),j=1,nFacesOnGeometry)

</pre>
*/

/*! @struct strand_header_t
 * The strand header record.
 */
typedef struct strand_header_t {
/*! determines if the mesh is surface only or the entire volume description. */
   int surfaceOnly;
/*! number of surface nodes */
   int nNodes;
/*! number of strands ( >= nNodes ) */
   int nStrands;
/*! number of triangular surface faces. In 2D, the
    number of surface edges */
   int nTriFaces;
/*! number of quadrilateral surface faces. Set to zero for 2D meshes*/
   int nQuadFaces;
/*! number of polyhedral surface faces. Set to zero for 2D meshes*/
   int nPolyFaces;
/*! Number of elements in the polyhedral faces array */
   int polyFacesSize;
/*! number of boundary edges. Note this is only the
    number of edges lying on an open boundary, such as a symmetry plane, and
    not internal edges of the surface description. In 2D, this number is the
    number of boundary nodes (which is either 0 for a closed set of edges
    like an airfoil, or 2 for an open set of edges like a flat plate. */
   int nBndEdges;
/*! number of points along each strand. This
    number is used to describe all strands */
   int nPtsPerStrand;
/*! number of surface patches */
   int nSurfPatches;
/*! number of edge patches */
   int nEdgePatches;
/*! strand length */
   double strandLength;
/*! maximum stretching ratio between successive strand nodes */
   double stretchRatio;
/*! threshold to determine when to stop
    strand pointing vector smoothing iterations */
   double smoothingThreshold;
/*! description of the strand stretching
    method. Currently either "geometric" or "hypTan" */
   char strandDistribution[32];
/*! number of nodes classified against geometry */
   int nNodesOnGeometry;
/*! number of edges classified against geometry */
   int nEdgesOnGeometry;
/*! number of faces classified against geometry */
   int nFacesOnGeometry;
#ifdef __cplusplus
   strand_header_t();
   int size() const;
   static int write(FILE* fp, bool swap, strand_header_t*);
   static int read(FILE* fp, bool swap, strand_header_t*);
#endif
} strand_header;

/*! @struct strand_surf_patch_t
 * The surface boundary patch header record.
 */
typedef struct strand_surf_patch_t {
/*! number of the surface patch appearing in <code>faceTag</code> array */
   int surfPatchId;
/*! body to which the patch belongs */
   char surfPatchBody[32];
/*! comp to which the patch belongs */
   char surfPatchComp[32];
/*! boundary condition type for the surface patch */
   char surfPatchBCType[32];
#ifdef __cplusplus
   strand_surf_patch_t();
   int size() const;
   static int writen(FILE* fp, bool swap, std::vector<strand_surf_patch_t>&);
   static int readn(FILE* fp, bool swap, std::vector<strand_surf_patch_t>&);
#endif
} strand_surf_patch;

/*! @struct strand_edge_patch_t
 * The edge boundary patch header record.
 */
typedef struct strand_edge_patch_t {
/*! number of the edge patch appearing in <code>edgeTag</code> array */
   int edgePatchId;
/*! body to which the patch belongs */
   char edgePatchBody[32];
/*! comp to which the patch belongs */
   char edgePatchComp[32];
/*! boundary condition type for the edge patch */
   char edgePatchBCType[32];
/*! x-component of outward normal to the face formed at this
    boundary edge */
   double nx;
/*! y-component of outward normal to the face formed at this
    boundary edge */
   double ny;
/*! z-component of outward normal to the face formed at this
    boundary edge. For 2D, nz is set to zero. */
   double nz;
#ifdef __cplusplus
   strand_edge_patch_t();
   int size() const;
   static int writen(FILE* fp, bool swap, std::vector<strand_edge_patch_t>&);
   static int readn(FILE* fp, bool swap, std::vector<strand_edge_patch_t>&);
#endif
} strand_edge_patch;

#ifdef __cplusplus
struct strand_info {
   strand_header header;
   std::vector<strand_surf_patch> surf_patches;
   std::vector<strand_edge_patch> edge_patches;
   static int write(FILE* fp, bool swap, strand_info*);
   static int read(FILE* fp, bool swap, strand_info*);
   off_t hdrsize() const;
   off_t datasize(const file_header_t&, const mesh_header_t&) const;
};
#endif

} //namespace rev1

#endif  /* STRAND_REV1_DEFS */
