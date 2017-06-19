#ifndef CART_REV1_DEFS
#define CART_REV1_DEFS

#ifdef __cplusplus
#include <string>
#include <vector> 
#endif

#include <sys/types.h>

namespace rev1 {

/** @page cart_rev1 Cartesian Data Format

@section Overview

   This document describes the AVMesh block structured Cartesian grid format.

@section cart_header Cartesian Header Format

<pre>
   nLevels
   nBlocks
   nFringe
   nxc
   nyc
   nzc
   domXLo
   domYLo
   domZLo
   domXHi
   domYHi
   domZHi
</pre>

   For additional details see @ref cart_header_t.

   Following the main part of the Cartesian header are <code>nLevels</code>
   ratio parameters.

   Following the ratio parameters there are
   <code>nBlocks</code> blocks:

<pre>
   ilo(3)
   ihi(3)
   levNum
   iblFlag
   bdryFlag
   iblDim(3)
   blo(3)
   bhi(3)
</pre>

   If <code>iblFlag=2</code> then <code>iblDim</code> field will contain
   iblank dimensions and iblanks for the block will be stored in the
   mesh data section.

   If <code>bdryFlag=1</code> then <code>blo/bhi</code> fields will contain
   block boundary information.

   For additional details see @ref cart_block_t.

@section cart_mesh Cartesian Mesh Format

For each block with <code>iblFlag=2</code>, the iblank information
is stored as:

<pre>
   do b=1,cart_hdr%nBlocks
      if (cart_blks(b)%iblFlag==2) then
         ! ...
         write(10) (((iblanks(j,k,l),j=1,jd),k=1,kd),l=1,ld)
      end if
   end do</pre>
*/

/*! @struct cart_header_t
 * The Cartesian header record.
 */
typedef struct cart_header_t {
/*! number of levels */
   int nLevels;
/*! number of blocks */
   int nBlocks;
/*! number of fringe cells */
   int nFringe;
/*! number of coarse domain cells */
   int nxc;
/*! number of coarse domain cells */
   int nyc;
/*! number of coarse domain cells */
   int nzc;
/*! lower domain extent */
   double domXLo;
/*! lower domain extent */
   double domYLo;
/*! lower domain extent */
   double domZLo;
/*! upper domain extent */
   double domXHi;
/*! upper domain extent */
   double domYHi;
/*! upper domain extent */
   double domZHi;
#ifdef __cplusplus
   cart_header_t();
   int size() const;
   static int write(FILE* fp, bool swap, cart_header_t*);
   static int read(FILE* fp, bool swap, cart_header_t*);
#endif
} cart_header;

/*! @struct cart_block_t
 * Block data.
 */
typedef struct cart_block_t {
/*! lower block index in X,Y,Z */
   int ilo[3];
/*! upper block index in X,Y,Z */
   int ihi[3];
/*! level number */
   int levNum;
/*! specifies whether block is blanked.  Set to 0 if all points in 
   the block are blanked, 1 if all points are unblanked, and 2 if 
   it contains a mix of blanked and unblanked points*/
   int iblFlag;
/*! specifies whether block contains one or more farfield boundaries.
    0 if block is bordered by other blocks and does not contain a
    farfield boundary, 1 if it does.  */
   int bdryFlag;
/*! number of iblank points in X,Y,Z */
   int iblDim[3];
/*! lower block boundary type in X,Y,Z 
   (inflow, outflow, periodic, etc) */
   int blo[3];
/*! upper block boundary type in X,Y,Z 
   (inflow, outflow, periodic, etc) */
   int bhi[3];
#ifdef __cplusplus
   cart_block_t();
   int size() const;
   static int writen(FILE* fp, bool swap, std::vector<cart_block_t>&);
   static int readn(FILE* fp, bool swap, std::vector<cart_block_t>&);
#endif
} cart_block;

#ifdef __cplusplus
struct cart_info {
   cart_header header;
   std::vector<int> ratios;
   std::vector<cart_block> blocks;
   static int write(FILE* fp, bool swap, cart_info*);
   static int read(FILE* fp, bool swap, cart_info*);
   off_t hdrsize() const;
   off_t datasize(const file_header_t&, const mesh_header_t&) const;
};
#endif

} //namespace rev1

#endif  /* CART_REV1_DEFS */
