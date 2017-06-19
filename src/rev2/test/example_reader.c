/* 
   This is an example of using the avmesh library with C.
   Error checking is ignored for brevity.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "avmesh.h"

int main()
{
   int i, avmid, meshCount, descriptionSize;
   double modelScale, ref_point[3], ref_area;
   char *fileDescription;
   char contactInfo[AVM_STD_STRING_LENGTH];

   int retval = avm_read_headers(&avmid, "mixed.avm");
   if (0 != retval) {
      fprintf(stderr, "ERROR: not an AVMesh file\n");
      return 1;
   }

   avm_get_int(avmid, "meshCount", &meshCount);
   avm_get_real(avmid, "modelScale", &modelScale);
   avm_get_str(avmid, "contactInfo", contactInfo, AVM_STD_STRING_LENGTH);
   avm_get_real_array(avmid, "referencePoint", ref_point, 3);
   avm_get_real(avmid, "referenceArea", &ref_area);
   avm_get_int(avmid, "descriptionSize", &descriptionSize);

   fileDescription = malloc(sizeof(char)*descriptionSize);

   avm_get_str(avmid, "description", fileDescription, descriptionSize);
   printf("file description is: '%s'\n", fileDescription);
   printf("Mesh Count is %d\n", meshCount);

   for (i=1; i<=meshCount; ++i) {
      char name[AVM_STD_STRING_LENGTH],
           type[AVM_STD_STRING_LENGTH];

      avm_select(avmid, "mesh", i);
      avm_get_str(avmid, "meshName", name, AVM_STD_STRING_LENGTH);
      avm_get_str(avmid, "meshType", type, AVM_STD_STRING_LENGTH);
      avm_get_real(avmid, "modelScale", &modelScale);
      avm_get_real_array(avmid, "referencePoint", ref_point, 3);

      printf("Mesh #%d named '%s' is type '%s'\n", i, name, type);
      printf("Model Scale is %f\n", modelScale);
      printf("Reference Point is %f, %f, %f\n",
             ref_point[0], ref_point[1], ref_point[2]);

      if (0==strcmp(type, "strand")) {
         int nBndEdges,nPtsPerStrand;
         avm_get_int(avmid, "nBndEdges", &nBndEdges);
         avm_get_int(avmid, "nPtsPerStrand", &nPtsPerStrand);
         /* ... */
      }
      else if (0==strcmp(type, "unstruc")) {
         int nNodes,nEdges,nFaces;
         avm_get_int(avmid, "nNodes", &nNodes);
         avm_get_int(avmid, "nEdges", &nEdges);
         avm_get_int(avmid, "nFaces", &nFaces);
         /* ... */
      }
   }

   avm_close(avmid);
   free(fileDescription);

   return 0;
}
