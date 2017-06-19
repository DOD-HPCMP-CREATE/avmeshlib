
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <string>
#include "avmhdrs.h"
#include "cart.h"
using namespace std;
using namespace rev1;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

cart_header_t::cart_header_t()
{
   nLevels = 0;
   nBlocks = 0;
   nFringe = 0;
   nxc = 0;
   nyc = 0;
   nzc = 0;
   domXLo = 0.0;
   domYLo = 0.0;
   domZLo = 0.0;
   domXHi = 0.0;
   domYHi = 0.0;
   domZHi = 0.0;
}

int cart_header_t::size() const
{
   return sizeof(nLevels) +
          sizeof(nBlocks) +
          sizeof(nFringe) +
          sizeof(nxc) +
          sizeof(nyc) +
          sizeof(nzc) +
          sizeof(domXLo) +
          sizeof(domYLo) +
          sizeof(domZLo) +
          sizeof(domXHi) +
          sizeof(domYHi) +
          sizeof(domZHi);
}

int cart_header_t::write(FILE* fp, bool swap, cart_header_t* p)
{
   if (swap) byte_swap_int(&p->nLevels);
   if (swap) byte_swap_int(&p->nBlocks);
   if (swap) byte_swap_int(&p->nFringe);
   if (swap) byte_swap_int(&p->nxc);
   if (swap) byte_swap_int(&p->nyc);
   if (swap) byte_swap_int(&p->nzc);
   if (swap) byte_swap_double(&p->domXLo);
   if (swap) byte_swap_double(&p->domYLo);
   if (swap) byte_swap_double(&p->domZLo);
   if (swap) byte_swap_double(&p->domXHi);
   if (swap) byte_swap_double(&p->domYHi);
   if (swap) byte_swap_double(&p->domZHi);

   if (!fwrite(&p->nLevels, sizeof(p->nLevels), 1, fp)) return 0;
   if (!fwrite(&p->nBlocks, sizeof(p->nBlocks), 1, fp)) return 0;
   if (!fwrite(&p->nFringe, sizeof(p->nFringe), 1, fp)) return 0;
   if (!fwrite(&p->nxc, sizeof(p->nxc), 1, fp)) return 0;
   if (!fwrite(&p->nyc, sizeof(p->nyc), 1, fp)) return 0;
   if (!fwrite(&p->nzc, sizeof(p->nzc), 1, fp)) return 0;
   if (!fwrite(&p->domXLo, sizeof(p->domXLo), 1, fp)) return 0;
   if (!fwrite(&p->domYLo, sizeof(p->domYLo), 1, fp)) return 0;
   if (!fwrite(&p->domZLo, sizeof(p->domZLo), 1, fp)) return 0;
   if (!fwrite(&p->domXHi, sizeof(p->domXHi), 1, fp)) return 0;
   if (!fwrite(&p->domYHi, sizeof(p->domYHi), 1, fp)) return 0;
   if (!fwrite(&p->domZHi, sizeof(p->domZHi), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->nLevels);
   if (swap) byte_swap_int(&p->nBlocks);
   if (swap) byte_swap_int(&p->nFringe);
   if (swap) byte_swap_int(&p->nxc);
   if (swap) byte_swap_int(&p->nyc);
   if (swap) byte_swap_int(&p->nzc);
   if (swap) byte_swap_double(&p->domXLo);
   if (swap) byte_swap_double(&p->domYLo);
   if (swap) byte_swap_double(&p->domZLo);
   if (swap) byte_swap_double(&p->domXHi);
   if (swap) byte_swap_double(&p->domYHi);
   if (swap) byte_swap_double(&p->domZHi);

   return 1;
}

int cart_header_t::read(FILE* fp, bool swap, cart_header_t* p)
{
   if (!fread(&p->nLevels, sizeof(p->nLevels), 1, fp)) return 0;
   if (!fread(&p->nBlocks, sizeof(p->nBlocks), 1, fp)) return 0;
   if (!fread(&p->nFringe, sizeof(p->nFringe), 1, fp)) return 0;
   if (!fread(&p->nxc, sizeof(p->nxc), 1, fp)) return 0;
   if (!fread(&p->nyc, sizeof(p->nyc), 1, fp)) return 0;
   if (!fread(&p->nzc, sizeof(p->nzc), 1, fp)) return 0;
   if (!fread(&p->domXLo, sizeof(p->domXLo), 1, fp)) return 0;
   if (!fread(&p->domYLo, sizeof(p->domYLo), 1, fp)) return 0;
   if (!fread(&p->domZLo, sizeof(p->domZLo), 1, fp)) return 0;
   if (!fread(&p->domXHi, sizeof(p->domXHi), 1, fp)) return 0;
   if (!fread(&p->domYHi, sizeof(p->domYHi), 1, fp)) return 0;
   if (!fread(&p->domZHi, sizeof(p->domZHi), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->nLevels);
   if (swap) byte_swap_int(&p->nBlocks);
   if (swap) byte_swap_int(&p->nFringe);
   if (swap) byte_swap_int(&p->nxc);
   if (swap) byte_swap_int(&p->nyc);
   if (swap) byte_swap_int(&p->nzc);
   if (swap) byte_swap_double(&p->domXLo);
   if (swap) byte_swap_double(&p->domYLo);
   if (swap) byte_swap_double(&p->domZLo);
   if (swap) byte_swap_double(&p->domXHi);
   if (swap) byte_swap_double(&p->domYHi);
   if (swap) byte_swap_double(&p->domZHi);

   return 1;
}

cart_block_t::cart_block_t()
{
   ilo[0] = 0;
   ilo[1] = 0;
   ilo[2] = 0;
   ihi[0] = 0;
   ihi[1] = 0;
   ihi[2] = 0;
   levNum = 0;
   iblFlag = 0;
   bdryFlag = 0;
   iblDim[0] = 0;
   iblDim[1] = 0;
   iblDim[2] = 0;
   blo[0] = 0;
   blo[1] = 0;
   blo[2] = 0;
   bhi[0] = 0;
   bhi[1] = 0;
   bhi[2] = 0;
}

int cart_block_t::size() const
{
   return sizeof(ilo) +
          sizeof(ihi) +
          sizeof(levNum) +
          sizeof(iblFlag) +
          sizeof(bdryFlag) +
          sizeof(iblDim) +
          sizeof(blo) +
          sizeof(bhi);
}

int cart_block_t::writen(FILE* fp, bool swap, vector<cart_block_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (swap) byte_swap_int(&p[i].levNum);
      if (swap) byte_swap_int(&p[i].iblFlag);
      if (swap) byte_swap_int(&p[i].bdryFlag);
      if (swap) for (int j=0; j<3; ++j) {
         byte_swap_int(&p[i].ilo[j]);
         byte_swap_int(&p[i].ihi[j]);
         byte_swap_int(&p[i].iblDim[j]);
         byte_swap_int(&p[i].blo[j]);
         byte_swap_int(&p[i].bhi[j]);
      }

      if (!fwrite(p[i].ilo, sizeof(p[i].ilo), 1, fp)) return 0;
      if (!fwrite(p[i].ihi, sizeof(p[i].ihi), 1, fp)) return 0;
      if (!fwrite(&p[i].levNum, sizeof(p[i].levNum), 1, fp)) return 0;
      if (!fwrite(&p[i].iblFlag, sizeof(p[i].iblFlag), 1, fp)) return 0;
      if (!fwrite(&p[i].bdryFlag, sizeof(p[i].bdryFlag), 1, fp)) return 0;
      if (!fwrite(p[i].iblDim, sizeof(p[i].iblDim), 1, fp)) return 0;
      if (!fwrite(p[i].blo, sizeof(p[i].blo), 1, fp)) return 0;
      if (!fwrite(p[i].bhi, sizeof(p[i].bhi), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].levNum);
      if (swap) byte_swap_int(&p[i].iblFlag);
      if (swap) byte_swap_int(&p[i].bdryFlag);
      if (swap) for (int j=0; j<3; ++j) {
         byte_swap_int(&p[i].ilo[j]);
         byte_swap_int(&p[i].ihi[j]);
         byte_swap_int(&p[i].iblDim[j]);
         byte_swap_int(&p[i].blo[j]);
         byte_swap_int(&p[i].bhi[j]);
      }
   }

   return 1;
}

int cart_block_t::readn(FILE* fp, bool swap, vector<cart_block_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (!fread(p[i].ilo, sizeof(p[i].ilo), 1, fp)) return 0;
      if (!fread(p[i].ihi, sizeof(p[i].ihi), 1, fp)) return 0;
      if (!fread(&p[i].levNum, sizeof(p[i].levNum), 1, fp)) return 0;
      if (!fread(&p[i].iblFlag, sizeof(p[i].iblFlag), 1, fp)) return 0;
      if (!fread(&p[i].bdryFlag, sizeof(p[i].bdryFlag), 1, fp)) return 0;
      if (!fread(p[i].iblDim, sizeof(p[i].iblDim), 1, fp)) return 0;
      if (!fread(p[i].blo, sizeof(p[i].blo), 1, fp)) return 0;
      if (!fread(p[i].bhi, sizeof(p[i].bhi), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].levNum);
      if (swap) byte_swap_int(&p[i].iblFlag);
      if (swap) byte_swap_int(&p[i].bdryFlag);
      if (swap) for (int j=0; j<3; ++j) {
         byte_swap_int(&p[i].ilo[j]);
         byte_swap_int(&p[i].ihi[j]);
         byte_swap_int(&p[i].iblDim[j]);
         byte_swap_int(&p[i].blo[j]);
         byte_swap_int(&p[i].bhi[j]);
      }
   }

   return 1;
}

int cart_info::write(FILE* fp, bool swap, cart_info* p)
{
   // write header
   if(!cart_header_t::write(fp, swap, &p->header)) return 0;

   // write ratios
   if (int nLevels = p->header.nLevels) {
      int* ratios = new (nothrow) int[nLevels];
      if (!ratios) return 0;
      for (int i=0; i<nLevels; ++i) {
         ratios[i] = p->ratios[i];
      }
      if (swap) for (int i=0; i<nLevels; ++i) byte_swap_int(ratios+i);
      if (!fwrite(ratios, sizeof(int)*nLevels, 1, fp)) return 0;
      delete []ratios;
   }

   // write blocks
   if(!cart_block_t::writen(fp, swap, p->blocks)) return 0;

   return 1;
}

int cart_info::read(FILE* fp, bool swap, cart_info* p)
{
   // read header
   if(!cart_header_t::read(fp, swap, &p->header)) return 0;

   // read ratios
   if (int nLevels = p->header.nLevels) {
      p->ratios.resize(nLevels);
      int* ratios = new (nothrow) int[nLevels];
      if (!ratios) return 0;
      if (!fread(ratios, sizeof(int)*nLevels, 1, fp)) return 0;
      for (int i=0; i<nLevels; ++i) {
         if (swap) byte_swap_int(ratios+i);
         p->ratios[i] = ratios[i];
      }
      delete []ratios;
   }

   // read blocks
   if (int nBlocks = p->header.nBlocks) {
      p->blocks.resize(nBlocks);
      if(!cart_block_t::readn(fp, swap, p->blocks)) return 0;
   }

   return 1;
}

off_t cart_info::hdrsize() const
{
   off_t offset = 0;
   offset += header.size();
   offset += header.nLevels * sizeof(int);
   offset += header.nBlocks * cart_block_t().size();
   return offset;
}

off_t cart_info::datasize(const file_header_t& filehdr,
                          const mesh_header_t& meshhdr) const
{
   off_t offset = 0;
   for (unsigned int i=0; i<blocks.size(); ++i) {
      if (blocks[i].iblFlag==2) {
         offset += blocks[i].iblDim[0] * blocks[i].iblDim[1] * 
                   blocks[i].iblDim[2] * sizeof(int);
      }
   }
   return offset;
}
