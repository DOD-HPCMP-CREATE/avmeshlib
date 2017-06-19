
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "avmhdrs.h"
#include "bfstruc.h"
using namespace std;
using namespace rev1;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

bfstruc_header_t::bfstruc_header_t()
{
   jmax = kmax = lmax = 0;
   iblank = 0;
   nPatches = 0;
   nPatchIntParams = 0;
   nPatchR8Params = 0;
   nPatchC80Params = 0;
}

int bfstruc_header_t::size() const
{
   return sizeof(jmax) +
          sizeof(kmax) +
          sizeof(lmax) +
          sizeof(iblank) +
          sizeof(nPatches) +
          sizeof(nPatchIntParams) +
          sizeof(nPatchR8Params) +
          sizeof(nPatchC80Params);
}

int bfstruc_header_t::write(FILE* fp, bool swap, bfstruc_header_t* p)
{
   if (swap) byte_swap_int(&p->jmax);
   if (swap) byte_swap_int(&p->kmax);
   if (swap) byte_swap_int(&p->lmax);
   if (swap) byte_swap_int(&p->iblank);
   if (swap) byte_swap_int(&p->nPatches);
   if (swap) byte_swap_int(&p->nPatchIntParams);
   if (swap) byte_swap_int(&p->nPatchR8Params);
   if (swap) byte_swap_int(&p->nPatchC80Params);

   if (!fwrite(&p->jmax, sizeof(p->jmax), 1, fp)) return 0;
   if (!fwrite(&p->kmax, sizeof(p->kmax), 1, fp)) return 0;
   if (!fwrite(&p->lmax, sizeof(p->lmax), 1, fp)) return 0;
   if (!fwrite(&p->iblank, sizeof(p->iblank), 1, fp)) return 0;
   if (!fwrite(&p->nPatches, sizeof(p->nPatches), 1, fp)) return 0;
   if (!fwrite(&p->nPatchIntParams, sizeof(p->nPatchIntParams), 1, fp)) return 0;
   if (!fwrite(&p->nPatchR8Params, sizeof(p->nPatchR8Params), 1, fp)) return 0;
   if (!fwrite(&p->nPatchC80Params, sizeof(p->nPatchC80Params), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->jmax);
   if (swap) byte_swap_int(&p->kmax);
   if (swap) byte_swap_int(&p->lmax);
   if (swap) byte_swap_int(&p->iblank);
   if (swap) byte_swap_int(&p->nPatches);
   if (swap) byte_swap_int(&p->nPatchIntParams);
   if (swap) byte_swap_int(&p->nPatchR8Params);
   if (swap) byte_swap_int(&p->nPatchC80Params);

   return 1;
}

int bfstruc_header_t::read(FILE* fp, bool swap, bfstruc_header_t* p)
{
   if (!fread(&p->jmax, sizeof(p->jmax), 1, fp)) return 0;
   if (!fread(&p->kmax, sizeof(p->kmax), 1, fp)) return 0;
   if (!fread(&p->lmax, sizeof(p->lmax), 1, fp)) return 0;
   if (!fread(&p->iblank, sizeof(p->iblank), 1, fp)) return 0;
   if (!fread(&p->nPatches, sizeof(p->nPatches), 1, fp)) return 0;
   if (!fread(&p->nPatchIntParams, sizeof(p->nPatchIntParams), 1, fp)) return 0;
   if (!fread(&p->nPatchR8Params, sizeof(p->nPatchR8Params), 1, fp)) return 0;
   if (!fread(&p->nPatchC80Params, sizeof(p->nPatchC80Params), 1, fp)) return 0;

   if (swap) byte_swap_int(&p->jmax);
   if (swap) byte_swap_int(&p->kmax);
   if (swap) byte_swap_int(&p->lmax);
   if (swap) byte_swap_int(&p->iblank);
   if (swap) byte_swap_int(&p->nPatches);
   if (swap) byte_swap_int(&p->nPatchIntParams);
   if (swap) byte_swap_int(&p->nPatchR8Params);
   if (swap) byte_swap_int(&p->nPatchC80Params);

   return 1;
}

bfstruc_patch_t::bfstruc_patch_t()
{
   bctype = 0;
   direction = 0;
   jbegin = 0;
   jend = 0;
   kbegin = 0;
   kend = 0;
   lbegin = 0;
   lend = 0;
   strncpy(description, "", sizeof(description));
}

int bfstruc_patch_t::size() const
{
   return sizeof(bctype) +
          sizeof(direction) +
          sizeof(jbegin) +
          sizeof(jend) +
          sizeof(kbegin) +
          sizeof(kend) +
          sizeof(lbegin) +
          sizeof(lend) +
          sizeof(description);
}

int bfstruc_patch_t::writen(FILE* fp, bool swap, vector<bfstruc_patch_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (swap) byte_swap_int(&p[i].bctype);
      if (swap) byte_swap_int(&p[i].direction);
      if (swap) byte_swap_int(&p[i].jbegin);
      if (swap) byte_swap_int(&p[i].jend);
      if (swap) byte_swap_int(&p[i].kbegin);
      if (swap) byte_swap_int(&p[i].kend);
      if (swap) byte_swap_int(&p[i].lbegin);
      if (swap) byte_swap_int(&p[i].lend);

      if (!fwrite(&p[i].bctype, sizeof(p[i].bctype), 1, fp)) return 0;
      if (!fwrite(&p[i].direction, sizeof(p[i].direction), 1, fp)) return 0;
      if (!fwrite(&p[i].jbegin, sizeof(p[i].jbegin), 1, fp)) return 0;
      if (!fwrite(&p[i].jend, sizeof(p[i].jend), 1, fp)) return 0;
      if (!fwrite(&p[i].kbegin, sizeof(p[i].kbegin), 1, fp)) return 0;
      if (!fwrite(&p[i].kend, sizeof(p[i].kend), 1, fp)) return 0;
      if (!fwrite(&p[i].lbegin, sizeof(p[i].lbegin), 1, fp)) return 0;
      if (!fwrite(&p[i].lend, sizeof(p[i].lend), 1, fp)) return 0;
      if (!fwrite(&p[i].description, sizeof(p[i].description), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].bctype);
      if (swap) byte_swap_int(&p[i].direction);
      if (swap) byte_swap_int(&p[i].jbegin);
      if (swap) byte_swap_int(&p[i].jend);
      if (swap) byte_swap_int(&p[i].kbegin);
      if (swap) byte_swap_int(&p[i].kend);
      if (swap) byte_swap_int(&p[i].lbegin);
      if (swap) byte_swap_int(&p[i].lend);
   }

   return 1;
}

int bfstruc_patch_t::readn(FILE* fp, bool swap, vector<bfstruc_patch_t>& p)
{
   for (unsigned int i=0; i<p.size(); ++i) {
      if (!fread(&p[i].bctype, sizeof(p[i].bctype), 1, fp)) return 0;
      if (!fread(&p[i].direction, sizeof(p[i].direction), 1, fp)) return 0;
      if (!fread(&p[i].jbegin, sizeof(p[i].jbegin), 1, fp)) return 0;
      if (!fread(&p[i].jend, sizeof(p[i].jend), 1, fp)) return 0;
      if (!fread(&p[i].kbegin, sizeof(p[i].kbegin), 1, fp)) return 0;
      if (!fread(&p[i].kend, sizeof(p[i].kend), 1, fp)) return 0;
      if (!fread(&p[i].lbegin, sizeof(p[i].lbegin), 1, fp)) return 0;
      if (!fread(&p[i].lend, sizeof(p[i].lend), 1, fp)) return 0;
      if (!fread(&p[i].description, sizeof(p[i].description), 1, fp)) return 0;

      if (swap) byte_swap_int(&p[i].bctype);
      if (swap) byte_swap_int(&p[i].direction);
      if (swap) byte_swap_int(&p[i].jbegin);
      if (swap) byte_swap_int(&p[i].jend);
      if (swap) byte_swap_int(&p[i].kbegin);
      if (swap) byte_swap_int(&p[i].kend);
      if (swap) byte_swap_int(&p[i].lbegin);
      if (swap) byte_swap_int(&p[i].lend);
   }

   return 1;
}

int bfstruc_info::write(FILE* fp, bool swap, bfstruc_info* p)
{
   if(!bfstruc_header_t::write(fp, swap, &p->header)) return 0;

   int nPatches    = p->header.nPatches,
       n_int_params = p->header.nPatchIntParams,
       n_r8_params  = p->header.nPatchR8Params,
       n_c80_params = p->header.nPatchC80Params;

   // write patches
   if(!bfstruc_patch_t::writen(fp, swap, p->patches)) return 0;

   // read patchIntParams
   if (nPatches>0 && n_int_params>0)
   {
      int* int_params_buffer = new int[nPatches*n_int_params];
      if (!int_params_buffer) return 0;
      for(int i=0; i<nPatches; ++i) {
         for(int j=0; j<n_int_params; ++j) {
            int_params_buffer[i+(j*nPatches)] = p->patchIntParams[i][j];
         }
      }   
      if (swap) for(int i=0; i<nPatches*n_int_params; ++i) byte_swap_int(int_params_buffer+i);
      if (!fwrite(int_params_buffer, sizeof(int)*nPatches*n_int_params, 1, fp)) return 0;
      delete []int_params_buffer;
   }

   // read patchR8Params
   if (nPatches>0 && n_r8_params>0)
   {
      double* r8_params_buffer = new double[nPatches*n_r8_params];
      if (!r8_params_buffer) return 0;
      for(int i=0; i<nPatches; ++i) {
         for(int j=0; j<n_r8_params; ++j) {
            r8_params_buffer[i+(j*nPatches)] = p->patchR8Params[i][j];
         }
      }   
      if (swap) for(int i=0; i<nPatches*n_r8_params; ++i) byte_swap_double(r8_params_buffer+i);
      if (!fwrite(r8_params_buffer, sizeof(double)*nPatches*n_r8_params, 1, fp)) return 0;
      delete []r8_params_buffer;
   }

   // read patchC80Params
   if (nPatches>0 && n_c80_params>0)
   {
      char* c80_params_buffer = new char[80*nPatches*n_c80_params];
      if (!c80_params_buffer) return 0;
      for(int i=0; i<nPatches; ++i) {
         for(int j=0; j<n_c80_params; ++j) {
            for(int k=0; k<80; ++k) {
               c80_params_buffer[((i+(j*nPatches))*80)+k] = p->patchC80Params[i][j][k];
            }
         }
      }
      if (!fwrite(c80_params_buffer, 80*nPatches*n_c80_params, 1, fp)) return 0;
      delete []c80_params_buffer;
   }

   return 1;
}

int bfstruc_info::read(FILE* fp, bool swap, bfstruc_info* p)
{
   if(!bfstruc_header_t::read(fp, swap, &p->header)) return 0;

   int nPatches    = p->header.nPatches,
       n_int_params = p->header.nPatchIntParams,
       n_r8_params  = p->header.nPatchR8Params,
       n_c80_params = p->header.nPatchC80Params;

   p->patches.resize(nPatches);
   vector<int> int_params(n_int_params);
   vector<double> r8_params(n_r8_params);
   vector<string> c80_params(n_c80_params);

   // read patches
   if(!bfstruc_patch_t::readn(fp, swap, p->patches)) return 0;

   // read patchIntParams
   if (nPatches>0 && n_int_params>0)
   {
      int* int_params_buffer = new int[nPatches*n_int_params];
      if (!int_params_buffer) return 0;
      if (!fread(int_params_buffer, sizeof(int)*nPatches*n_int_params, 1, fp)) return 0;
      if (swap) for(int i=0; i<nPatches*n_int_params; ++i) byte_swap_int(int_params_buffer+i);
      for(int i=0; i<nPatches; ++i) {
         for(int j=0; j<n_int_params; ++j) {
            int_params[j] = int_params_buffer[i+(j*nPatches)];
         }
         p->patchIntParams.push_back(int_params);
      }   
      delete []int_params_buffer;
   }

   // read patchR8Params
   if (nPatches>0 && n_r8_params>0)
   {
      double* r8_params_buffer = new double[nPatches*n_r8_params];
      if (!r8_params_buffer) return 0;
      if (!fread(r8_params_buffer, sizeof(double)*nPatches*n_r8_params, 1, fp)) return 0;
      if (swap) for(int i=0; i<nPatches*n_r8_params; ++i) byte_swap_double(r8_params_buffer+i);
      for(int i=0; i<nPatches; ++i) {
         for(int j=0; j<n_r8_params; ++j) {
            r8_params[j] = r8_params_buffer[i+(j*nPatches)];
         }
         p->patchR8Params.push_back(r8_params);
      }   
      delete []r8_params_buffer;
   }

   // read patchC80Params
   if (nPatches>0 && n_c80_params>0)
   {
      char* c80_params_buffer = new char[80*nPatches*n_c80_params];
      if (!c80_params_buffer) return 0;
      if (!fread(c80_params_buffer, 80*nPatches*n_c80_params, 1, fp)) return 0;
      for(int i=0; i<nPatches; ++i) {
         for(int j=0; j<n_c80_params; ++j) {
            const char* str = c80_params_buffer+((i+(j*nPatches))*80);
            c80_params[j] = string(str, str+80);
         }
         p->patchC80Params.push_back(c80_params);
      }
      delete []c80_params_buffer;
   }

   return 1;
}

off_t bfstruc_info::hdrsize() const
{
   off_t offset = 0;
   offset += header.size();
   offset += header.nPatches * bfstruc_patch_t().size();
   offset += header.nPatches * header.nPatchIntParams * sizeof(int);
   offset += header.nPatches * header.nPatchR8Params * sizeof(double);
   offset += header.nPatches * header.nPatchC80Params * sizeof(char)*80;
   return offset;
}

off_t bfstruc_info::datasize(const file_header_t& filehdr,
                             const mesh_header_t& meshhdr) const
{
   int precision = filehdr.precision;

   off_t offset = 0;
   // add in xyz size
   offset += header.jmax * header.kmax * header.lmax * 
             (1==precision ? sizeof(float) : sizeof(double)) * 
             filehdr.dimensions;
   // add in iblank size
   if (header.iblank) offset += header.jmax * header.kmax * header.lmax *
                                 sizeof(int);
   return offset;
}
