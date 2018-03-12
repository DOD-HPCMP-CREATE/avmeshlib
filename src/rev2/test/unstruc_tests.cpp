#include <cstdio>
using namespace std;
#include <gtest/gtest.h>
#include "avmesh.h"

class TestFixture : public ::testing::Test {
public:
   int i, fileid;
   double d, vec3[3], vec9[9];
   char str[AVM_STD_STRING_LENGTH];
   int descriptionSize;
   char* description;
   TestFixture() {
      EXPECT_EQ(0, avm_read_headers(&fileid, "unstruc.avm"));
      description = 0;
   }
   ~TestFixture() {
      EXPECT_EQ(0, avm_close(fileid));
      if (description) delete []description;
   }
};

TEST_F(TestFixture, FileHeader) {
   EXPECT_EQ(0, avm_select(fileid, "header", 0));

   EXPECT_EQ(0, avm_get_int(fileid, "formatRevision", &i));
   EXPECT_EQ(2, i);

   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(1, i);

   EXPECT_EQ(0, avm_get_str(fileid, "contactInfo", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("David McDaniel", str);

   EXPECT_EQ(0, avm_get_int(fileid, "precision", &i));
   EXPECT_EQ(2, i);

   EXPECT_EQ(0, avm_get_int(fileid, "dimensions", &i));
   EXPECT_EQ(3, i);

   EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
   description = new (nothrow) char[descriptionSize];

   EXPECT_EQ(0, avm_get_str(fileid, "description", description, descriptionSize));
   EXPECT_STREQ("An example AVMesh file with some unstruc grids.", description);
}

TEST_F(TestFixture, MeshHeaders) {
   int meshCount, patch_count;
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));

   for (int meshid=1; meshid<=meshCount; ++meshid) {
      char temp[AVM_STD_STRING_LENGTH];
      EXPECT_EQ(0, avm_select(fileid, "mesh", meshid));

      // generic mesh fields
      {
         sprintf(temp, "unstruc mesh # %d", meshid);

         EXPECT_EQ(0, avm_get_str(fileid, "meshName", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ(temp, str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("unstruc", str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshGenerator", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("unstruc_write", str);

         EXPECT_EQ(0, avm_get_str(fileid, "coordinateSystem", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("xByUzL", str);

         EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_str(fileid, "gridUnits", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("in", str);

         EXPECT_EQ(0, avm_get_real_array(fileid, "referenceLength", vec3, 3));
         EXPECT_DOUBLE_EQ(1.0, vec3[0]);
         EXPECT_DOUBLE_EQ(1.0, vec3[1]);
         EXPECT_DOUBLE_EQ(1.0, vec3[2]);

         EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", vec3, 3));
         EXPECT_DOUBLE_EQ(0.0, vec3[0]);
         EXPECT_DOUBLE_EQ(0.0, vec3[1]);
         EXPECT_DOUBLE_EQ(0.0, vec3[2]);

         EXPECT_EQ(0, avm_get_str(fileid, "referencePointDescription", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("origin", str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshDescription", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("A nonsensical unstruc mesh that just demonstrates the format.", str);
      }

      // unstruc mesh fields
      {
         EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &i));
         EXPECT_EQ(12, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nEdges", &i));
         EXPECT_EQ(12, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nFaces", &i));
         EXPECT_EQ(17, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nMaxNodesPerFace", &i));
         EXPECT_EQ(4, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &i));
         EXPECT_EQ(9, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &i));
         EXPECT_EQ(8, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nBndTriFaces", &i));
         EXPECT_EQ(8, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nBndQuadFaces", &i));
         EXPECT_EQ(6, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nCells", &i));
         EXPECT_EQ(4, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nMaxNodesPerCell", &i));
         EXPECT_EQ(8, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nMaxFacesPerCell", &i));
         EXPECT_EQ(6, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nHexCells", &i));
         EXPECT_EQ(1, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nTetCells", &i));
         EXPECT_EQ(2, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nPriCells", &i));
         EXPECT_EQ(1, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nPyrCells", &i));
         EXPECT_EQ(1, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nPatches", &patch_count));
         EXPECT_EQ(2, patch_count);
      }

      // patches
      for (int patchid=1; patchid<=patch_count; ++patchid) {
         EXPECT_EQ(0, avm_select(fileid, "patch", patchid));

         EXPECT_EQ(0, avm_get_str(fileid, "patchLabel", str, AVM_STD_STRING_LENGTH));
         sprintf(temp, "patch %d label", patchid);
         EXPECT_STREQ(temp, str);

         EXPECT_EQ(0, avm_get_str(fileid, "patchType", str, AVM_STD_STRING_LENGTH));
         sprintf(temp, "patch %d bctype", patchid);
         EXPECT_STREQ(temp, str);

         EXPECT_EQ(0, avm_get_int(fileid, "patchId", &i));
         EXPECT_EQ(-patchid, i);
      }
   }
}

TEST_F(TestFixture, DataReads) {
   int meshCount;
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));

   for (int meshid=1; meshid<=meshCount; ++meshid) {
      EXPECT_EQ(0, avm_select(fileid, "mesh", meshid));

      int p;
      int nNodes;
      int nTriFaces;
      int nQuadFaces;
      int nBndTriFaces;
      int nBndQuadFaces;

      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndTriFaces", &nBndTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndQuadFaces", &nBndQuadFaces));

      double* xyz = new (nothrow) double[3*nNodes];
      int* triFaces = new (nothrow) int[4*nTriFaces];
      int* quadFaces = new (nothrow) int[5*nQuadFaces];

      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);

      EXPECT_EQ(0, avm_unstruc_read_nodes_r8(fileid, xyz, 3*nNodes));
      EXPECT_EQ(0, avm_unstruc_read_faces(fileid, triFaces,  4*nTriFaces,
                                                  quadFaces, 5*nQuadFaces));

      // nodes
      if (0) {
         printf("meshid=%d\n",meshid);
         for (int i=0; i<nNodes; ++i) {
            int i0=(i*3)+0, i1=(i*3)+1, i2=(i*3)+2;
            printf("xyz[%02d] xyz[%02d] xyz[%02d] = "
                   "(%1.2f,%1.2f,%1.2f)\n", i0,i1,i2,
                   xyz[i0],xyz[i1],xyz[i2]);
         }
      } else {
         double tol=0.001; 
         // check the first couple and last node 
         // node 1
         p = (0*3);
         EXPECT_NEAR(0.0,xyz[p+0],tol);
         EXPECT_NEAR(0.0,xyz[p+1],tol);
         EXPECT_NEAR(0.0,xyz[p+2],tol);
         // node 2
         p = (1*3);
         EXPECT_NEAR(12.0,xyz[p+0],tol);
         EXPECT_NEAR(0.0,xyz[p+1],tol);
         EXPECT_NEAR(0.0,xyz[p+2],tol);
         // node ...
         // node 12
         p = (11*3);
         EXPECT_NEAR(-6.0,xyz[p+0],tol);
         EXPECT_NEAR(15.0,xyz[p+1],tol);
         EXPECT_NEAR(6.0,xyz[p+2],tol);
      }

      // check the first and last triFace 
      {
         int BUFF_LEN=4, FIRST_POINT=0, LAST_POINT=8;
         p = FIRST_POINT * BUFF_LEN;
         EXPECT_EQ(5, triFaces[p+0]);
         EXPECT_EQ(12, triFaces[p+1]);
         EXPECT_EQ(6, triFaces[p+2]);
         EXPECT_EQ(-1, triFaces[p+3]);
         p = LAST_POINT * BUFF_LEN;
         EXPECT_EQ(5, triFaces[p+0]);
         EXPECT_EQ(6, triFaces[p+1]);
         EXPECT_EQ(11, triFaces[p+2]);
         EXPECT_EQ(1, triFaces[p+3]);
      }

      // check the first and last quadFace 
      {
         int BUFF_LEN=5, FIRST_POINT=0, LAST_POINT=7;
         p = FIRST_POINT * BUFF_LEN;
         EXPECT_EQ(1, quadFaces[p+0]);
         EXPECT_EQ(4, quadFaces[p+1]);
         EXPECT_EQ(5, quadFaces[p+2]);
         EXPECT_EQ(6, quadFaces[p+3]);
         EXPECT_EQ(-1, quadFaces[p+4]);
         p = LAST_POINT * BUFF_LEN;
         EXPECT_EQ(5, quadFaces[p+0]);
         EXPECT_EQ(6, quadFaces[p+1]);
         EXPECT_EQ(7, quadFaces[p+2]);
         EXPECT_EQ(8, quadFaces[p+3]);
         EXPECT_EQ(1, quadFaces[p+4]);
      }

      delete []xyz;
      delete []quadFaces;
      delete []triFaces;
   }
}
