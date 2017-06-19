#include <stdio.h>
#include <string.h>
#include "avmesh.h"

int main(int argc, char** argv)
{
   int i, avmid, meshCount, descriptionSize;
   char* description;

   if (argc<2) {
      fprintf(stderr, "ERROR: argc<2\n");
      return 1;
   }

   int retval = avm_read_headers(&avmid, argv[1]);
   if (0 != retval) {
      fprintf(stderr, "ERROR: not an AVMesh file\n");
      return 1;
   }

   retval = avm_get_int(avmid, "meshCount", &meshCount);
   if (0 != retval) {
      fprintf(stderr, "ERROR: avm_get_int('meshCount'), line=%d\n", __LINE__);
      return 1;
   }
   printf("meshCount = %d\n", meshCount);

   retval = avm_get_int(avmid, "descriptionSize", &descriptionSize);
   if (0 != retval) {
      fprintf(stderr, "ERROR: avm_get_int('descriptionSize'), line=%d\n", __LINE__);
      return 1;
   }
   printf("descriptionSize = %d\n", descriptionSize);
   
   description = new char[descriptionSize];
   retval = avm_get_str(avmid, "description", description, descriptionSize);
   if (0 != retval) {
      fprintf(stderr, "ERROR: avm_get_str('description'), line=%d\n", __LINE__);
      return 1;
   }
   printf("description = '%s'\n", description);
   delete []description;

   for (i=1; i<=meshCount; ++i) {
      char name[AVM_STD_STRING_LENGTH];

      retval = avm_select(avmid, "mesh", i);
      if (0 != retval) {
         fprintf(stderr, "ERROR: avm_select('mesh'), line=%d\n", __LINE__);
         return 1;
      }
      retval = avm_get_str(avmid, "meshName", name, AVM_STD_STRING_LENGTH);
      if (0 != retval) {
         fprintf(stderr, "ERROR: avm_get_str('meshName'), line=%d\n", __LINE__);
         return 1;
      }

      printf("mesh: '%s'\n", name);

      retval = avm_get_int(avmid, "descriptionSize", &descriptionSize);
      if (0 != retval) {
         fprintf(stderr, "ERROR: avm_get_int('descriptionSize'), line=%d\n", __LINE__);
         return 1;
      }
      printf("descriptionSize = %d\n", descriptionSize);

      description = new char[descriptionSize];
      retval = avm_get_str(avmid, "description", description, descriptionSize);
      if (0 != retval) {
         fprintf(stderr, "ERROR: avm_get_str('description'), line=%d\n", __LINE__);
         return 1;
      }
      printf(" - description = '%s'\n", description);
      delete []description;
   }

   retval = avm_close(avmid);
   if (0 != retval) {
      fprintf(stderr, "ERROR: avm_close(), line=%d\n", __LINE__);
      return 1;
   }

   return 0;
}
