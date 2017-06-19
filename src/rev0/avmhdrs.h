#ifndef AVDEFS_REV0
#define AVDEFS_REV0

namespace rev0 {

/** @page avmhdrs_rev0 AVMesh File and Generic Mesh Header Format 

@section Overview

This document describes the AVMesh file-level and generic mesh header formats.
While the AVMesh format supports mesh data as single or double-precision, big-
or little-endian, and 2- or 3-dimensions, all header floating-point information
is stored as double-precision and all arrays are stored as 3-dimensions.

@section file_header File Header
The first 14 bytes of a AVMesh File are:
<pre>    
   AVMESH (AVMesh id character sequence)
   1      (AVMesh id integer)
   0      (AVMesh format revision)
</pre>

For additional details see @ref file_id_prefix_t.

The next section of the header is:
<pre>    
   meshRevision             
   meshCount                
   contactName              
   contactOrg               
   contactEmail             
   precision                
   dimensions               
   descriptionSize          
   description (variable length)
</pre>

For additional details see @ref file_header_t.

@section mesh_n_hdr Generic Mesh Header
Each mesh header is composed of a generic mesh header followed by a mesh-type
specific header.

Here is the generic mesh header:
<pre>
   meshName                  
   meshType                  
   meshGenerator             
   changedDate               
   coordinateSystem          
   modelScale                
   gridUnits                 
   reynoldsNumber            
   referenceLength           
   wallDistance              
   referencePoint(3)        
   referencePointDescription
   periodicity               
   periodicityDescription    
   refinementLevel           
   descriptionSize           
   description (variable length)
</pre>

For additional details see @ref mesh_header_t.

Following the mesh description is the mesh-specific header. Here is a list of
specific headers:
   - @ref bfstruc
   - @ref cart
   - @ref strand
   - @ref unstruc
*/

/*! @struct file_header_t
 * The AVMesh file header record.
 */
typedef struct file_header_t {
/*! File revision string */
   char meshRevision[128];
/*! Number of meshes appearing in the file */
   int meshCount;
/*! Name of the person maintaining the file */
   char contactName[128];
/*! Organization of maintainer */
   char contactOrg[128];
/*! Email address of maintainer */
   char contactEmail[128];
/*! 1 - single, 2-double, 4-quad */
   int precision;
/*! 2 = 2D or axisymetric, 3 = 3D */
   int dimensions;
/*! Size of the description field in bytes */
   int descriptionSize;
#ifdef __cplusplus
   file_header_t();
   int size() const;
   static int write(FILE* fp, bool swap, file_header_t*);
   static int read(FILE* fp, bool swap, file_header_t*);
#endif
} file_header;

/*! @struct mesh_header_t
 * The generic mesh-n header record.
 */
typedef struct mesh_header_t {
/*! Name/title of this mesh */
   char meshName[128];
/*! bfstruc, strand, unstruc, cart */
   char meshType[128];
/*! Name/version of the tool that generated the mesh */
   char meshGenerator[128];
/*! Date that the mesh was created or changed */
   char changedDate[128];
/*! Coordinate system of this mesh
<pre>
U/D = up/down
F/B = front/back
L/R = left/right 
xByUzL --- this is FLO57
xByRzU --- PANAIR
xFyRzD --- traditional S&C system 
</pre>
*/
   char coordinateSystem[128];
/*! Scale of model for the mesh */
   double modelScale;
/*! Description of units of the mesh */
   char gridUnits[128];
/*! Intended Reynolds number of the mesh */
   double reynoldsNumber;
/*! Reference length for Reynolds number */
   double referenceLength;
/*! Wall distance of the mesh in grid units */
   double wallDistance;
/*! XYZ coordinate of the reference point */
   double referencePoint[3];
/*! Description of the reference point */
   char referencePointDescription[128];
/*! 1.0=no periodicity, 2.0=180 degree periodicity */
   double periodicity;
/*! Description of what is meant by periodicity */
   char periodicityDescription[128];
/*! Refinement level of the mesh */
   int refinementLevel;
/*! Size of the description field in bytes */
   int descriptionSize;
#ifdef __cplusplus
   mesh_header_t();
   int size() const;
   static int write(FILE* fp, bool swap, mesh_header_t*);
   static int read(FILE* fp, bool swap, mesh_header_t*);
#endif
} mesh_header;

} //namespace rev0

#endif  /* AVDEFS_REV0 */
