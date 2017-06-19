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
      EXPECT_EQ(0, avm_read_headers(&fileid, "strand.avm"));
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
   EXPECT_STREQ("Aaron Katz", str);

   EXPECT_EQ(0, avm_get_str(fileid, "contactOrg", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("Ames Education Associates", str);

   EXPECT_EQ(0, avm_get_str(fileid, "contactEmail", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("akatz@merlin.arc.nasa.gov", str);

   EXPECT_EQ(0, avm_get_int(fileid, "precision", &i));
   EXPECT_EQ(2, i);

   EXPECT_EQ(0, avm_get_int(fileid, "dimensions", &i));
   EXPECT_EQ(3, i);

   EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
   description = new (nothrow) char[descriptionSize];

   EXPECT_EQ(0, avm_get_str(fileid, "description", description, descriptionSize));
   EXPECT_STREQ("An example AVMesh file with some strand grids.", description);
}

TEST_F(TestFixture, MeshHeaders) {
   int meshCount;
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));

   for (int meshid=1; meshid<=meshCount; ++meshid) {
      char temp[AVM_STD_STRING_LENGTH];
      EXPECT_EQ(0, avm_select(fileid, "mesh", meshid));

      // generic mesh fields
      {
         sprintf(temp, "strand mesh # %d", meshid);

         EXPECT_EQ(0, avm_get_str(fileid, "meshName", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ(temp, str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("strand", str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshGenerator", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("strand_write", str);

         EXPECT_EQ(0, avm_get_str(fileid, "changedDate", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("May 6, 2010", str);

         EXPECT_EQ(0, avm_get_str(fileid, "coordinateSystem", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("xByUzL", str);

         EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_str(fileid, "gridUnits", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("m", str);

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
         EXPECT_STREQ("A nonsensical strand mesh that just demonstrates the format.", description);
      }

      // strand mesh fields
      int surf_patch_count, edge_patch_count;
      {
         EXPECT_EQ(0, avm_get_int(fileid, "nSurfNodes", &i));
         EXPECT_EQ(6, i);

         EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &i));
         EXPECT_EQ(2, i);

         EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &i));
         EXPECT_EQ(1, i);

         EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &i));
         EXPECT_EQ(6, i);

         EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &i));
         EXPECT_EQ(4, i);

         EXPECT_EQ(0, avm_get_int(fileid, "nSurfPatches", &surf_patch_count));
         EXPECT_EQ(2, surf_patch_count);

         EXPECT_EQ(0, avm_get_int(fileid, "nEdgePatches", &edge_patch_count));
         EXPECT_EQ(4, edge_patch_count);

         EXPECT_EQ(0, avm_get_real(fileid, "strandLength", &d));
         EXPECT_DOUBLE_EQ(2.0, d);

         EXPECT_EQ(0, avm_get_real(fileid, "stretchRatio", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_real(fileid, "smoothingThreshold", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_str(fileid, "strandDistribution", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("geometric", str);
      }

      // surface patches
      for (int surfpatchi=1; surfpatchi<=surf_patch_count; ++surfpatchi)
      {
         EXPECT_EQ(0, avm_select(fileid, "surfPatch", surfpatchi));
         EXPECT_EQ(0, avm_get_int(fileid, "surfPatchId", &i));          EXPECT_EQ(surfpatchi, i);
         EXPECT_EQ(0, avm_get_str(fileid, "surfPatchBody", str, 32));   EXPECT_STREQ("flat plate", str);
         EXPECT_EQ(0, avm_get_str(fileid, "surfPatchComp", str, 32));   EXPECT_STREQ("surface", str);
         EXPECT_EQ(0, avm_get_str(fileid, "surfPatchBCType", str, 32)); EXPECT_STREQ("no slip", str);
      }

      // edge patches
      for (int edgepatchi=1; edgepatchi<=edge_patch_count; ++edgepatchi)
      {
         EXPECT_EQ(0, avm_select(fileid, "edgePatch", edgepatchi));
         EXPECT_EQ(0, avm_get_int(fileid, "edgePatchId", &i));          EXPECT_EQ(edgepatchi, i);
         EXPECT_EQ(0, avm_get_str(fileid, "edgePatchBody", str, 32));   EXPECT_STREQ("plate boundary", str);
         EXPECT_EQ(0, avm_get_str(fileid, "edgePatchComp", str, 32));   EXPECT_STREQ("edge", str);
         EXPECT_EQ(0, avm_get_str(fileid, "edgePatchBCType", str, 32)); EXPECT_STREQ("symmetry", str);
         EXPECT_EQ(0, avm_get_real(fileid, "nx", &d));                  EXPECT_DOUBLE_EQ(1.0, d);
         EXPECT_EQ(0, avm_get_real(fileid, "ny", &d));                  EXPECT_DOUBLE_EQ(0.0, d);
         EXPECT_EQ(0, avm_get_real(fileid, "nz", &d));                  EXPECT_DOUBLE_EQ(0.0, d);
      }
   }
}

TEST_F(TestFixture, DataReads) {
   int meshCount;
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));

   for (int meshid=1; meshid<=meshCount; ++meshid) {
      EXPECT_EQ(0, avm_select(fileid, "mesh", meshid));
      
      int nSurfNodes;
      int nTriFaces;
      int nQuadFaces;
      int nBndEdges;
      int nPtsPerStrand;

      EXPECT_EQ(0, avm_get_int(fileid, "nSurfNodes", &nSurfNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &nBndEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &nPtsPerStrand));

      int* triConn = new (nothrow) int[3*nTriFaces];
      int* quadConn = new (nothrow) int[4*nQuadFaces];
      int* bndEdges = new (nothrow) int[2*nBndEdges];
      int* nodeClip = new (nothrow) int[nSurfNodes];
      int* faceClip = new (nothrow) int[nTriFaces+nQuadFaces];
      int* faceTag = new (nothrow) int[nTriFaces+nQuadFaces];
      int* edgeTag = new (nothrow) int[nBndEdges];
      double* xSurf = new (nothrow) double[3*nSurfNodes];
      double* pointingVec = new (nothrow) double[3*nSurfNodes];
      double* xStrand = new (nothrow) double[nPtsPerStrand];

      EXPECT_TRUE(0!=triConn);
      EXPECT_TRUE(0!=quadConn);
      EXPECT_TRUE(0!=bndEdges);
      EXPECT_TRUE(0!=nodeClip);
      EXPECT_TRUE(0!=faceClip);
      EXPECT_TRUE(0!=faceTag);
      EXPECT_TRUE(0!=edgeTag);
      EXPECT_TRUE(0!=xSurf);
      EXPECT_TRUE(0!=pointingVec);
      EXPECT_TRUE(0!=xStrand);

      EXPECT_EQ(0, avm_strand_read_r8(fileid,
         triConn, 3*nTriFaces,
         quadConn, 4*nQuadFaces,
         bndEdges, 2*nBndEdges,
         nodeClip, nSurfNodes,
         faceClip, nTriFaces+nQuadFaces,
         faceTag, nTriFaces+nQuadFaces,
         edgeTag, nBndEdges,
         xSurf, 3*nSurfNodes,
         pointingVec, 3*nSurfNodes,
         xStrand, nPtsPerStrand));

      for (int i=0; i<nTriFaces; ++i)
         for (int j=0; j<3; ++j)
            EXPECT_EQ(meshid+i+1+j+1, triConn[(i*3)+j]); 

      for (int i=0; i<nPtsPerStrand; ++i)
         EXPECT_DOUBLE_EQ(meshid+i, xStrand[i]); 

      delete []triConn;
      delete []quadConn;
      delete []bndEdges;
      delete []nodeClip;
      delete []faceClip;
      delete []faceTag;
      delete []edgeTag;
      delete []xSurf;
      delete []pointingVec;
      delete []xStrand;
   }
}
