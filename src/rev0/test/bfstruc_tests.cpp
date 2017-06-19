#include <cstdio>
using namespace std;
#include <gtest/gtest.h>
#include "avmesh.h"

class TestFixture : public ::testing::Test {
public:
   int i, fileid;
   double d, vec3[3], vec9[9];
   char str[AVM_STD_STRING_LENGTH], c[80];
   int descriptionSize;
   char* description;
   TestFixture() {
      EXPECT_EQ(0, avm_read_headers(&fileid, "bfstruc.avm"));
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
   EXPECT_EQ(0, i);

   EXPECT_EQ(0, avm_get_str(fileid, "meshRevision", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("1.0", str);

   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(3, i);

   EXPECT_EQ(0, avm_get_str(fileid, "contactName", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("Josh Calahan", str);

   EXPECT_EQ(0, avm_get_str(fileid, "contactOrg", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("Aerospace Testing Alliance", str);

   EXPECT_EQ(0, avm_get_str(fileid, "contactEmail", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("joshua.calahan@arnold.af.mil", str);

   EXPECT_EQ(0, avm_get_int(fileid, "precision", &i));
   EXPECT_EQ(1, i);

   EXPECT_EQ(0, avm_get_int(fileid, "dimensions", &i));
   EXPECT_EQ(3, i);

   EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
   description = new (nothrow) char[descriptionSize];

   EXPECT_EQ(0, avm_get_str(fileid, "description", description, descriptionSize));
   EXPECT_STREQ("An example AVMesh file with some bfstruc grids.", description);
}

TEST_F(TestFixture, MeshHeaders) {
   int meshCount;
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));

   for (int meshid=1; meshid<=meshCount; ++meshid) {
      char temp[AVM_STD_STRING_LENGTH];
      EXPECT_EQ(0, avm_select(fileid, "mesh", meshid));

      // generic mesh fields
      {
         sprintf(temp, "bfstruc mesh # %d", meshid);

         EXPECT_EQ(0, avm_get_str(fileid, "meshName", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ(temp, str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("bfstruc", str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshGenerator", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("write_test001", str);

         EXPECT_EQ(0, avm_get_str(fileid, "changedDate", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("April 22, 2010", str);

         EXPECT_EQ(0, avm_get_str(fileid, "coordinateSystem", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("xByUzL", str);

         EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_str(fileid, "gridUnits", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("ft", str);

         EXPECT_EQ(0, avm_get_real(fileid, "reynoldsNumber", &d));
         EXPECT_DOUBLE_EQ(5.0, d);

         EXPECT_EQ(0, avm_get_real(fileid, "referenceLength", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_real(fileid, "wallDistance", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", vec3, 3));
         EXPECT_DOUBLE_EQ(0.0, vec3[0]);
         EXPECT_DOUBLE_EQ(0.0, vec3[1]);
         EXPECT_DOUBLE_EQ(0.0, vec3[2]);

         EXPECT_EQ(0, avm_get_str(fileid, "referencePointDescription", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("origin", str);

         EXPECT_EQ(0, avm_get_real(fileid, "periodicity", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_str(fileid, "periodicityDescription", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("full-span", str);

         EXPECT_EQ(0, avm_get_int(fileid, "refinementLevel", &i));
         EXPECT_EQ(1, i);

         EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
         description = new (nothrow) char[descriptionSize];

         EXPECT_EQ(0, avm_get_str(fileid, "description", description, descriptionSize));
         EXPECT_STREQ("A nonsensical bfstruc mesh that just demonstrates the format.", description);
      }

      // bfstruc mesh fields
      int patch_count, patch_param_count;
      {
         EXPECT_EQ(0, avm_get_int(fileid, "jmax", &i));
         EXPECT_EQ(2, i);
         EXPECT_EQ(0, avm_get_int(fileid, "kmax", &i));
         EXPECT_EQ(3, i);
         EXPECT_EQ(0, avm_get_int(fileid, "lmax", &i));
         EXPECT_EQ(4, i);
         EXPECT_EQ(0, avm_get_int(fileid, "iblank", &i));
         EXPECT_EQ(1, i);
         EXPECT_EQ(0, avm_get_int(fileid, "nPatches", &patch_count));
         EXPECT_EQ(4, patch_count);
         EXPECT_EQ(0, avm_get_int(fileid, "nPatchIntParams", &patch_param_count));
         EXPECT_EQ(5, patch_param_count);
         EXPECT_EQ(0, avm_get_int(fileid, "nPatchR8Params", &patch_param_count));
         EXPECT_EQ(5, patch_param_count);
         EXPECT_EQ(0, avm_get_int(fileid, "nPatchC80Params", &patch_param_count));
         EXPECT_EQ(5, patch_param_count);
      }

      // bcs
      for (int patchid=1; patchid<=patch_count; ++patchid)
      {
         EXPECT_EQ(0, avm_select(fileid, "patch", patchid));
         {
            EXPECT_EQ(0, avm_get_int(fileid, "bctype", &i));     EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "direction", &i));  EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "jbegin", &i));     EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "jend", &i));       EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "kbegin", &i));     EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "kend", &i));       EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "lbegin", &i));     EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_int(fileid, "lend", &i));       EXPECT_EQ(patchid, i);
            EXPECT_EQ(0, avm_get_str(fileid, "patch_description", str, AVM_STD_STRING_LENGTH));
            sprintf(temp, "patch %d description", patchid);
            EXPECT_STREQ(temp, str);

            // bc parameters
            for (int patchparami=1; patchparami<=patch_param_count; ++patchparami)
            {
               EXPECT_EQ(0, avm_select(fileid, "patchParam", patchparami));
               EXPECT_EQ(0, avm_get_int(fileid, "patch_int_param", &i));
               EXPECT_EQ(patchparami, i);
               EXPECT_EQ(0, avm_get_real(fileid, "patch_r8_param", &d));
               EXPECT_DOUBLE_EQ(double(patchparami), d);
               EXPECT_EQ(0, avm_get_str(fileid, "patch_c80_param", c, 80));
               sprintf(temp, "bc character parameter (mesh=%d, bc=%d, param=%d)", meshid, patchid, patchparami);
               EXPECT_STREQ(temp, c);
            }
         }
      }
   }
}

TEST_F(TestFixture, DataReads) {
   int meshCount;
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));

   for (int meshid=1; meshid<=meshCount; ++meshid) {
      EXPECT_EQ(0, avm_select(fileid, "mesh", meshid));

      int jmax,kmax,lmax;
      EXPECT_EQ(0, avm_get_int(fileid, "jmax", &jmax));
      EXPECT_EQ(0, avm_get_int(fileid, "kmax", &kmax));
      EXPECT_EQ(0, avm_get_int(fileid, "lmax", &lmax));

      int size = jmax*kmax*lmax;
      float* x = new (nothrow) float[size];
      ASSERT_TRUE(0 != x);
      float* y = new (nothrow) float[size];
      ASSERT_TRUE(0 != y);
      float* z = new (nothrow) float[size];
      ASSERT_TRUE(0 != z);
      int* iblanks = new (nothrow) int[size];
      ASSERT_TRUE(0 != iblanks);

      EXPECT_EQ(0, avm_bfstruc_read_xyz_r4(fileid, x,y,z, size));
      EXPECT_EQ(0, avm_bfstruc_read_iblank(fileid, iblanks, size));

      delete []x;
      delete []y;
      delete []z;
      delete []iblanks;
   }
}
