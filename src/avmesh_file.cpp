#include <stdio.h>
#include <string.h>
#include "avmesh_file.h"
//using namespace std;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

file_id_prefix_t::file_id_prefix_t()
{
   strncpy(magicString, "AVMESH", sizeof(magicString));
   magicNumber = 1;
   formatRevision = 0;
}

int file_id_prefix_t::size() const
{
   return sizeof(magicString) +
          sizeof(magicNumber) +
          sizeof(formatRevision);
}

int file_id_prefix_t::write(FILE* fp, bool swap, file_id_prefix_t* p)
{
   if (swap) {
      byte_swap_int(&p->magicNumber);
      byte_swap_int(&p->formatRevision);
   }

   if (!fwrite(p->magicString, sizeof(p->magicString), 1, fp)) return 0;
   if (!fwrite(&p->magicNumber, sizeof(p->magicNumber), 1, fp)) return 0;
   if (!fwrite(&p->formatRevision, sizeof(p->formatRevision), 1, fp)) return 0;

   if (swap) {
      byte_swap_int(&p->magicNumber);
      byte_swap_int(&p->formatRevision);
   }

   return 1;
}

int file_id_prefix_t::read(FILE* fp, bool* swap, file_id_prefix_t* p)
{
   if (!fread(p->magicString, sizeof(p->magicString), 1, fp)) return 0;
   if (!fread(&p->magicNumber, sizeof(p->magicNumber), 1, fp)) return 0;
   if (!fread(&p->formatRevision, sizeof(p->formatRevision), 1, fp)) return 0;

   // check to see if the magic string is what we expect
   if (0!=strncmp("AVMESH", p->magicString, sizeof(p->magicString))) return 0;

   // see if we need to byte swap file contents
   *swap = (p->magicNumber != 1);

   // check to see if swapping will give us the
   // correct magic number we are looking for
   if (*swap) {
      byte_swap_int(&p->magicNumber);
      if (p->magicNumber != 1) return 0;
      byte_swap_int(&p->formatRevision);
   }

   return 1;
}

