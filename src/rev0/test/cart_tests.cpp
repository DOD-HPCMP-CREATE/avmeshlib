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
      EXPECT_EQ(0, avm_read_headers(&fileid, "cart.avm"));
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
   EXPECT_EQ(1, i);

   EXPECT_EQ(0, avm_get_str(fileid, "contactName", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("Andy Wissink", str);

   EXPECT_EQ(0, avm_get_str(fileid, "contactOrg", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("NASA Ames", str);

   EXPECT_EQ(0, avm_get_str(fileid, "contactEmail", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("awissink@mail.arc.nasa.gov", str);

   EXPECT_EQ(0, avm_get_int(fileid, "precision", &i));
   EXPECT_EQ(2, i);

   EXPECT_EQ(0, avm_get_int(fileid, "dimensions", &i));
   EXPECT_EQ(3, i);

   EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
   description = new (nothrow) char[descriptionSize];

   EXPECT_EQ(0, avm_get_str(fileid, "description", description, descriptionSize));
   EXPECT_STREQ("An example AVMesh file with a cartesian grid.", description);
}

TEST_F(TestFixture, MeshHeader) {

   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));

   // generic mesh fields
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshName", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("cart mesh # 1", str);

      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("cart", str);

      EXPECT_EQ(0, avm_get_str(fileid, "meshGenerator", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("cart_write", str);

      EXPECT_EQ(0, avm_get_str(fileid, "changedDate", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("May 7, 2010", str);

      EXPECT_EQ(0, avm_get_str(fileid, "coordinateSystem", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("xByUzL", str);

      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &d));
      EXPECT_DOUBLE_EQ(1.0, d);

      EXPECT_EQ(0, avm_get_str(fileid, "gridUnits", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("m", str);

      EXPECT_EQ(0, avm_get_real(fileid, "reynoldsNumber", &d));
      EXPECT_DOUBLE_EQ(1.0, d);

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
      EXPECT_STREQ("A nonsensical cart mesh that just demonstrates the format.", description);
   }

   // cart mesh fields
   int nlevels, nblocks;
   {
      EXPECT_EQ(0, avm_get_int(fileid, "nLevels", &nlevels));
      EXPECT_EQ(3, nlevels);
      EXPECT_EQ(0, avm_get_int(fileid, "nBlocks", &nblocks));
      EXPECT_EQ(6, nblocks);
      EXPECT_EQ(0, avm_get_int(fileid, "nFringe", &i));
      EXPECT_EQ(3, i);
      EXPECT_EQ(0, avm_get_int(fileid, "nxc", &i));
      EXPECT_EQ(10, i);
      EXPECT_EQ(0, avm_get_int(fileid, "nyc", &i));
      EXPECT_EQ(10, i);
      EXPECT_EQ(0, avm_get_int(fileid, "nzc", &i));
      EXPECT_EQ(10, i);
      EXPECT_EQ(0, avm_get_real(fileid, "domXLo", &d));
      EXPECT_DOUBLE_EQ(0.0, d);
      EXPECT_EQ(0, avm_get_real(fileid, "domYLo", &d));
      EXPECT_DOUBLE_EQ(0.0, d);
      EXPECT_EQ(0, avm_get_real(fileid, "domZLo", &d));
      EXPECT_DOUBLE_EQ(0.0, d);
      EXPECT_EQ(0, avm_get_real(fileid, "domXHi", &d));
      EXPECT_DOUBLE_EQ(10.0, d);
      EXPECT_EQ(0, avm_get_real(fileid, "domYHi", &d));
      EXPECT_DOUBLE_EQ(10.0, d);
      EXPECT_EQ(0, avm_get_real(fileid, "domZHi", &d));
      EXPECT_DOUBLE_EQ(10.0, d);
   }

   // levels
   EXPECT_EQ(0, avm_select(fileid, "level", 1));
   EXPECT_EQ(0, avm_get_int(fileid, "iRatio", &i));
   EXPECT_EQ(1, i);
   EXPECT_EQ(0, avm_select(fileid, "level", 2));
   EXPECT_EQ(0, avm_get_int(fileid, "iRatio", &i));
   EXPECT_EQ(2, i);
   EXPECT_EQ(0, avm_select(fileid, "level", 3));
   EXPECT_EQ(0, avm_get_int(fileid, "iRatio", &i));
   EXPECT_EQ(4, i);

   // blocks
   for (int blockid=1; blockid<=nblocks; ++blockid) {
      EXPECT_EQ(0, avm_select(fileid, "block", blockid));

      int ilo[3], ihi[3]; // ndimensions
      int levNum, iblFlag, bdryFlag;
      int iblDim[3], blo[3], bhi[3];
      EXPECT_EQ(0, avm_get_int_array(fileid, "ilo", ilo, 3));
      EXPECT_EQ(0, avm_get_int_array(fileid, "ihi", ihi, 3));
      EXPECT_EQ(0, avm_get_int(fileid, "levNum", &levNum));
      EXPECT_EQ(0, avm_get_int(fileid, "iblFlag", &iblFlag));
      EXPECT_EQ(0, avm_get_int(fileid, "bdryFlag", &bdryFlag));
      EXPECT_EQ(0, avm_get_int_array(fileid, "iblDim", iblDim, 3));
      EXPECT_EQ(0, avm_get_int_array(fileid, "blo", blo, 3));
      EXPECT_EQ(0, avm_get_int_array(fileid, "bhi", bhi, 3));

      switch(blockid)
      {
      case 1:
         EXPECT_EQ(1, ilo[0]);
         EXPECT_EQ(1, ilo[1]);
         EXPECT_EQ(1, ilo[2]);
         EXPECT_EQ(10, ihi[0]);
         EXPECT_EQ(10, ihi[1]);
         EXPECT_EQ(10, ihi[2]);
         EXPECT_EQ(1, levNum);
         EXPECT_EQ(1, iblFlag);
         EXPECT_EQ(1, bdryFlag);
         // this block has bdry data
         EXPECT_EQ(1, blo[0]);
         EXPECT_EQ(1, blo[1]);
         EXPECT_EQ(1, blo[2]);
         EXPECT_EQ(1, bhi[0]);
         EXPECT_EQ(1, bhi[1]);
         EXPECT_EQ(1, bhi[2]);
         break;
      case 2:
         EXPECT_EQ(7, ilo[0]);
         EXPECT_EQ(7, ilo[1]);
         EXPECT_EQ(7, ilo[2]);
         EXPECT_EQ(14, ihi[0]);
         EXPECT_EQ(14, ihi[1]);
         EXPECT_EQ(14, ihi[2]);
         EXPECT_EQ(2, levNum);
         EXPECT_EQ(2, iblFlag);
         EXPECT_EQ(0, bdryFlag);
         // this block has iblank data
         EXPECT_EQ(4, iblDim[0]);
         EXPECT_EQ(6, iblDim[1]);
         EXPECT_EQ(9, iblDim[2]);
         break;
      case 3:
         EXPECT_EQ(17, ilo[0]);
         EXPECT_EQ(17, ilo[1]);
         EXPECT_EQ(17, ilo[2]);
         EXPECT_EQ(20, ihi[0]);
         EXPECT_EQ(20, ihi[1]);
         EXPECT_EQ(24, ihi[2]);
         EXPECT_EQ(3, levNum);
         EXPECT_EQ(1, iblFlag);
         EXPECT_EQ(0, bdryFlag);
         break;
      case 4:
         EXPECT_EQ(21, ilo[0]);
         EXPECT_EQ(17, ilo[1]);
         EXPECT_EQ(17, ilo[2]);
         EXPECT_EQ(24, ihi[0]);
         EXPECT_EQ(20, ihi[1]);
         EXPECT_EQ(24, ihi[2]);
         EXPECT_EQ(3, levNum);
         EXPECT_EQ(1, iblFlag);
         EXPECT_EQ(0, bdryFlag);
         break;
      case 5:
         EXPECT_EQ(17, ilo[0]);
         EXPECT_EQ(21, ilo[1]);
         EXPECT_EQ(17, ilo[2]);
         EXPECT_EQ(20, ihi[0]);
         EXPECT_EQ(24, ihi[1]);
         EXPECT_EQ(24, ihi[2]);
         EXPECT_EQ(3, levNum);
         EXPECT_EQ(1, iblFlag);
         EXPECT_EQ(0, bdryFlag);
         break;
      case 6:
         EXPECT_EQ(21, ilo[0]);
         EXPECT_EQ(21, ilo[1]);
         EXPECT_EQ(17, ilo[2]);
         EXPECT_EQ(24, ihi[0]);
         EXPECT_EQ(24, ihi[1]);
         EXPECT_EQ(24, ihi[2]);
         EXPECT_EQ(3, levNum);
         EXPECT_EQ(2, iblFlag);
         EXPECT_EQ(0, bdryFlag);
         // this block has iblank data
         EXPECT_EQ(8, iblDim[0]);
         EXPECT_EQ(9, iblDim[1]);
         EXPECT_EQ(10, iblDim[2]);
         break;
      default: FAIL() << "We shouldn't get here.";
      }
   }
}

TEST_F(TestFixture, DataReads) {
   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));

   int nblocks, iblFlag, iblDim[3];
   EXPECT_EQ(0, avm_get_int(fileid, "nBlocks", &nblocks));
   for (int blockid=1; blockid<=nblocks; ++blockid) {
      EXPECT_EQ(0, avm_select(fileid, "block", blockid));
      EXPECT_EQ(0, avm_get_int(fileid, "iblFlag", &iblFlag));
      if (iblFlag == 2) {
         EXPECT_EQ(0, avm_get_int_array(fileid, "iblDim", iblDim, 3));
         int jd = iblDim[0], kd = iblDim[1], ld = iblDim[2];
         int* iblanks = new (nothrow) int[jd*kd*ld];
         EXPECT_TRUE(0!=iblanks);
         EXPECT_EQ(0, avm_cart_read_iblank(fileid, iblanks, jd*kd*ld));
         if (0) {
            for (int i=0; i<jd*kd*ld; ++i) {
               printf("[%05d] = %d\n", i, iblanks[i]);
            }
         } else {
            if (blockid==2) {
               for (int j=0; j<jd; ++j) {
                  for (int k=0; k<kd; ++k) {
                     for (int l=0; l<ld; ++l) {
                        int n = jd * kd * l + jd * k + j;
                             if (j+1==1  && k+1==1  && l+1==1) EXPECT_EQ(0,iblanks[n]);
                        else if (j+1==3 && k+1==5 && l+1==7) EXPECT_EQ(0,iblanks[n]);
                        else if (j+1==4 && k+1==6 && l+1==9) EXPECT_EQ(0,iblanks[n]);
                        else EXPECT_EQ(1,iblanks[n]);
                     }
                  }
               }
            } else if (blockid==6) {
               for (int j=0; j<jd; ++j) {
                  for (int k=0; k<kd; ++k) {
                     for (int l=0; l<ld; ++l) {
                        int n = jd * kd * l + jd * k + j;
                             if (j+1==1 && k+1==1 && l+1==1)  EXPECT_EQ(0,iblanks[n]);
                        else if (j+1==2 && k+1==2 && l+1==8)  EXPECT_EQ(0,iblanks[n]);
                        else if (j+1==8 && k+1==9 && l+1==10) EXPECT_EQ(0,iblanks[n]);
                        else EXPECT_EQ(1,iblanks[n]);
                     }
                  }
               }
            }
         }
         delete []iblanks;
      }
   }
}

