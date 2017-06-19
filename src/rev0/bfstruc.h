#ifndef BFSTRUC_REV0_DEFS
#define BFSTRUC_REV0_DEFS

/** @page bfstruc_rev0 Body-Fitted Structured Data Format

@section Overview

   This document describes the AVMesh body-fitted structured grid format.

@section bfstruc_header Body-Fitted Structured Header Format

<pre>
   jmax
   kmax
   lmax
   iblank                    
   nPatches
   nPatchIntParams
   nPatchR8Params
   nPatchC80Params
</pre>

   For additional details see @ref bfstruc_header_t.

   Following the main part of the bfstruc header are <code>nPatches</code>
   boundary conditions:

<pre>    
   bctype
   direction
   jbegin
   jend
   kbegin
   kend
   lbegin
   lend
   description
</pre>

   For additional details see @ref bfstruc_patch_t.

   Following the boundary conditions, patch parameters are listed.

   <code>nPatches * nPatchIntParams</code> integer parameters

   <code>nPatches * nPatchR8Params</code> floating-point parameters

   <code>nPatches * nPatchC80Params</code> string parameters

@section bfstruc_mesh Body-Fitted Structured Mesh Format

   In Fortran the mesh format is written as:
<pre>
   write(iu) &
     (((x(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax), &
     (((y(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax), &
     (((z(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax)</pre>

   Mesh data may be written as single or double-precision.

   If there are iblanks in the file, they are written as:
<pre>
   write(iu) &
     (((iblanks(j,k,l),j=1,jmax),k=1,kmax),l=1,lmax)</pre>
*/

#ifdef __cplusplus
#include <string>
#include <vector> 
#endif

#include <sys/types.h>

namespace rev0 {

/*! @struct bfstruc_header_t
 * The body-fitted structured header record.
 */
typedef struct bfstruc_header_t {
/*! maximum j index */
   int jmax;
/*! maximum k index */
   int kmax;
/*! maximum l index */
   int lmax;
/*! 0 - no iblank, 1 - iblank included for plotting */
   int iblank;
/*! number of boundary condition patches */
   int nPatches;
/*! number of integer patch parameters */
   int nPatchIntParams;
/*! number of real patch parameters */
   int nPatchR8Params;
/*! number of string patch parameters */
   int nPatchC80Params;
#ifdef __cplusplus
   bfstruc_header_t();
   int size() const;
   static int write(FILE* fp, bool swap, bfstruc_header_t*);
   static int read(FILE* fp, bool swap, bfstruc_header_t*);
#endif
} bfstruc_header;

/*! @struct bfstruc_patch_t
 * The body-fitted structured boundry condition record.
 */
typedef struct bfstruc_patch_t {
/*! BC type */
   int bctype;
/*! BC direction <code>[-3,-2,-1,+1,+2,+3]</code>*/
   int direction;
/*! J start index */
   int jbegin;
/*! J end index */
   int jend;
/*! K start index */
   int kbegin;
/*! K end index */
   int kend;
/*! L start index */
   int lbegin;
/*! L end index */
   int lend;
/*! BC description */
   char description[128];
#ifdef __cplusplus
   bfstruc_patch_t();
   int size() const;
   static int writen(FILE* fp, bool swap, std::vector<bfstruc_patch_t>&);
   static int readn(FILE* fp, bool swap, std::vector<bfstruc_patch_t>&);
#endif
} bfstruc_patch;

#ifdef __cplusplus
struct bfstruc_info {
   bfstruc_header header;
   std::vector<bfstruc_patch> patches;
   std::vector< std::vector<int> > patchIntParams;
   std::vector< std::vector<double> > patchR8Params;
   std::vector< std::vector<std::string> > patchC80Params;
   static int write(FILE* fp, bool swap, bfstruc_info*);
   static int read(FILE* fp, bool swap, bfstruc_info*);
   off_t hdrsize() const;
   off_t datasize(const file_header_t&, const mesh_header_t&) const;
};
#endif

} //namespace rev0

#endif  /* BFSTRUC_REV0_DEFS */
