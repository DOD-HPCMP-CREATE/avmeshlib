/* 
   This example demonstrates avm_mesh_*_offset.
   Error checking is ignored for brevity.
*/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include "avmesh.h"
int main(int argc, char** argv)
{
   off_t offset;
   int i, avmid, meshCount=-1;

   if(argc < 2) {
      printf("Usage: ./example_offset.exe <avm file>\n");
      return 1;
   }
   avm_read_headers(&avmid, argv[1]);
   avm_get_int(avmid, "meshCount", &meshCount);
   printf("meshCount: %d\n",meshCount);
   for (i=1; i<=meshCount; ++i) {
      avm_mesh_header_offset(avmid, i, &offset);
      printf("Mesh %d's header starts at %ld\n", i, offset);
   }
   for (i=1; i<=meshCount; ++i) {
      avm_mesh_data_offset(avmid, i, &offset);
      printf("Mesh %d's data starts at %ld\n", i, offset);
   }
   avm_mesh_data_offset(avmid, meshCount+1, &offset);
   printf("File size is %ld\n", offset);
   avm_close(avmid);
   return 0;
}
