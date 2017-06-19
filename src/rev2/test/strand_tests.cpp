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
   EXPECT_EQ(1, i);

   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(3, i);

   EXPECT_EQ(0, avm_get_str(fileid, "contactInfo", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("Aaron Katz", str);

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

         EXPECT_EQ(0, avm_get_str(fileid, "coordinateSystem", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("xByUzL", str);

         EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_str(fileid, "gridUnits", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("m", str);

         EXPECT_EQ(0, avm_get_real(fileid, "referenceLength", &d));
         EXPECT_DOUBLE_EQ(1.0, d);

         EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", vec3, 3));
         EXPECT_DOUBLE_EQ(0.0, vec3[0]);
         EXPECT_DOUBLE_EQ(0.0, vec3[1]);
         EXPECT_DOUBLE_EQ(0.0, vec3[2]);

         EXPECT_EQ(0, avm_get_str(fileid, "referencePointDescription", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("origin", str);

         EXPECT_EQ(0, avm_get_str(fileid, "meshDescription", str, AVM_STD_STRING_LENGTH));
         EXPECT_STREQ("A nonsensical strand mesh that just demonstrates the format.", str);
      }

      // strand mesh fields
      int surf_patch_count, edge_patch_count;
      {
         EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &i));
         EXPECT_EQ(6, i);

         EXPECT_EQ(0, avm_get_int(fileid, "nStrands", &i));
         EXPECT_EQ(8, i);

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
      
      int nNodes;
      int nStrands;
      int nTriFaces;
      int nQuadFaces;
      int nPolyFaces;
      int nBndEdges;
      int nPtsPerStrand;
      int polyFacesSize;

      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nStrands", &nStrands));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nPolyFaces", &nPolyFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &nBndEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &nPtsPerStrand));
      EXPECT_EQ(0, avm_get_int(fileid, "polyFacesSize", &polyFacesSize));

      int* triFaces = new (nothrow) int[3*nTriFaces];
      int* quadFaces = new (nothrow) int[4*nQuadFaces];
      int* polyFaces = new (nothrow) int[polyFacesSize];
      int* bndEdges = new (nothrow) int[2*nBndEdges];
      int* nodeID = new (nothrow) int[nStrands];
      int* nodeClip = new (nothrow) int[nStrands];
      int* faceTag = new (nothrow) int[nTriFaces+nQuadFaces+nPolyFaces];
      int* edgeTag = new (nothrow) int[nBndEdges];
      double* xyz = new (nothrow) double[3*nNodes];
      double* pointingVec = new (nothrow) double[3*nStrands];
      double* xStrand = new (nothrow) double[nPtsPerStrand];

      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);
      EXPECT_TRUE(0!=bndEdges);
      EXPECT_TRUE(0!=nodeID);
      EXPECT_TRUE(0!=nodeClip);
      EXPECT_TRUE(0!=faceTag);
      EXPECT_TRUE(0!=edgeTag);
      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=pointingVec);
      EXPECT_TRUE(0!=xStrand);

      EXPECT_EQ(0, avm_strand_read_nodes_r8(fileid,
         xyz, 3*nNodes,
         nodeID, nStrands,
         nodeClip, nStrands,
         pointingVec, 3*nStrands,
         xStrand, nPtsPerStrand));

      EXPECT_EQ(0, avm_strand_read_faces(fileid,
         triFaces, 3*nTriFaces,
         quadFaces, 4*nQuadFaces,
         polyFaces, polyFacesSize,
         faceTag, nTriFaces+nQuadFaces+nPolyFaces));

      EXPECT_EQ(0, avm_strand_read_edges(fileid,
         bndEdges, 2*nBndEdges,
         edgeTag, nBndEdges));

      for (int i=0; i<nTriFaces; ++i)
         for (int j=0; j<3; ++j)
            EXPECT_EQ(meshid+i+1+j+1, triFaces[(i*3)+j]); 

      for (int i=0; i<nPtsPerStrand; ++i)
         EXPECT_DOUBLE_EQ(meshid+i, xStrand[i]); 

      delete []triFaces;
      delete []quadFaces;
      delete []polyFaces;
      delete []bndEdges;
      delete []nodeID;
      delete []nodeClip;
      delete []faceTag;
      delete []edgeTag;
      delete []xyz;
      delete []pointingVec;
      delete []xStrand;
   }
}
