#include <cstdio>
using namespace std;
#include <gtest/gtest.h>
#include "avmesh.h"

const char* filename = "mixed.avm";

TEST(Example, generic_header_interface_example) {
   int fileid;
   int meshCount;
   char contactName[AVM_STD_STRING_LENGTH];

   EXPECT_EQ(0, avm_read_headers(&fileid, filename));

   EXPECT_EQ(0, avm_select(fileid, "header", 0));
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));
   EXPECT_EQ(4, meshCount);
   EXPECT_EQ(0, avm_get_str(fileid, "contactName", contactName, AVM_STD_STRING_LENGTH));

   for (int i=1; i<=meshCount; ++i) {
      double modelScale, ref_point[3];
      char name[AVM_STD_STRING_LENGTH], type[AVM_STD_STRING_LENGTH];

      EXPECT_EQ(0, avm_select(fileid, "mesh", i));
      EXPECT_EQ(0, avm_get_str(fileid, "meshName", name, AVM_STD_STRING_LENGTH));
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", type, AVM_STD_STRING_LENGTH));
      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &modelScale));
      EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", ref_point, 3));

      if (0==strcmp(type, "bfstruc")) {
         int jmax,kmax,lmax;
         EXPECT_EQ(0, avm_get_int(fileid, "jmax", &jmax));
         EXPECT_EQ(0, avm_get_int(fileid, "kmax", &kmax));
         EXPECT_EQ(0, avm_get_int(fileid, "lmax", &lmax));
         /* ... */
      }
      else if (0==strcmp(type, "strand")) {
         int nBndEdges,nPtsPerStrand;
         EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &nBndEdges));
         EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &nPtsPerStrand));
         /* ... */
      }
      else if (0==strcmp(type, "unstruc")) {
         int nNodes,nEdges,nFaces;
         EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
         EXPECT_EQ(0, avm_get_int(fileid, "nEdges", &nEdges));
         EXPECT_EQ(0, avm_get_int(fileid, "nFaces", &nFaces));
         /* ... */
      }
      else if (0==strcmp(type, "cart")) {
         int nLevels,nBlocks,nFringe;
         EXPECT_EQ(0, avm_get_int(fileid, "nLevels", &nLevels));
         EXPECT_EQ(0, avm_get_int(fileid, "nBlocks", &nBlocks));
         EXPECT_EQ(0, avm_get_int(fileid, "nFringe", &nFringe));
         /* ... */
      }
   }

   EXPECT_EQ(0, avm_close(fileid));
}

class TestFixture : public ::testing::Test {
public:
   int i, fileid;
   double d, vec3[3], vec9[9];
   char str[AVM_STD_STRING_LENGTH], c[80];
   TestFixture() {
      EXPECT_EQ(0, avm_read_headers(&fileid, filename));
   }
   ~TestFixture() {
      EXPECT_EQ(0, avm_close(fileid));
   }
};

TEST_F(TestFixture, offsets) {
   off_t offset = 0;
   const int genericMeshHeaderSize = 1096;
   int precision = 0;
   int dimensions = 0;

   int fileHeaderSize = 542;
   // check some mesh expectations and then
   // add in file description size
   {
      int descriptionSize;
      int meshCount;
      EXPECT_EQ(0, avm_select(fileid, "header", 0));
      EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
      EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));
      EXPECT_EQ(4, meshCount);
      EXPECT_EQ(0, avm_get_int(fileid, "precision", &precision));
      EXPECT_EQ(1, precision);
      EXPECT_EQ(0, avm_get_int(fileid, "dimensions", &dimensions));
      EXPECT_EQ(3, dimensions);

      // add in file description size
      fileHeaderSize += descriptionSize-1; // (negate null-terminator byte)

      //fprintf(stderr, "FYI: fileHeaderSize = %d\n", fileHeaderSize);
   }

   int mesh1headerSize = genericMeshHeaderSize;
   // add in bfstruc header pieces
   {
      int descriptionSize;
      int nPatches;
      int nPatchIntParams;
      int nPatchR8Params;
      int nPatchC80Params;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
      EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
      EXPECT_EQ(0, avm_get_int(fileid, "nPatches", &nPatches));
      EXPECT_EQ(0, avm_get_int(fileid, "nPatchIntParams", &nPatchIntParams));
      EXPECT_EQ(0, avm_get_int(fileid, "nPatchR8Params", &nPatchR8Params));
      EXPECT_EQ(0, avm_get_int(fileid, "nPatchC80Params", &nPatchC80Params));

      // add in mesh description size
      mesh1headerSize += descriptionSize-1; // (negate null-terminator byte)

      // add in fixed part of bfstruc header
      mesh1headerSize += 32;

      // add in bfstruc patches
      mesh1headerSize += 160 * nPatches;

      // add in bfstruc patch parameters
      mesh1headerSize += nPatches * nPatchIntParams * 4;
      mesh1headerSize += nPatches * nPatchR8Params * 8;
      mesh1headerSize += nPatches * nPatchC80Params * 80;

      //fprintf(stderr, "FYI: mesh1headerSize = %d\n", mesh1headerSize);
   }

   int mesh2headerSize = genericMeshHeaderSize;
   // add in strand header pieces
   {
      int descriptionSize;
      int nSurfPatches;
      int nEdgePatches;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
      EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
      EXPECT_EQ(0, avm_get_int(fileid, "nSurfPatches", &nSurfPatches));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdgePatches", &nEdgePatches));

      // add in mesh description size
      mesh2headerSize += descriptionSize-1; // (negate null-terminator byte)

      // add in fixed part of strand header
      mesh2headerSize += 88;

      // add in strand patches
      mesh2headerSize += 100 * nSurfPatches;
      mesh2headerSize += 124 * nEdgePatches;

      //fprintf(stderr, "FYI: mesh2headerSize = %d\n", mesh2headerSize);
   }

   int mesh3headerSize = genericMeshHeaderSize;
   // add in unstruc header pieces
   {
      int descriptionSize;
      int nPatches;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 3));
      EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
      EXPECT_EQ(0, avm_get_int(fileid, "nPatches", &nPatches));

      // add in mesh description size
      mesh3headerSize += descriptionSize-1; // (negate null-terminator byte)

      // add in fixed part of unstruc header
      mesh3headerSize += 80;

      // add in unstruc patches
      mesh3headerSize += 52 * nPatches;

      //fprintf(stderr, "FYI: mesh3headerSize = %d\n", mesh3headerSize);
   }

   int mesh4headerSize = genericMeshHeaderSize;
   // add in cart header pieces
   {
      int descriptionSize;
      int nLevels;
      int nBlocks;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 4));
      EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
      EXPECT_EQ(0, avm_get_int(fileid, "nLevels", &nLevels));
      EXPECT_EQ(0, avm_get_int(fileid, "nBlocks", &nBlocks));

      // add in mesh description size
      mesh4headerSize += descriptionSize-1; // (negate null-terminator byte)

      // add in fixed part of cart header
      mesh4headerSize += 72;

      // add in ratios
      mesh4headerSize += 4 * nLevels;

      // add in blocks
      mesh4headerSize += 72 * nBlocks;

      //fprintf(stderr, "FYI: mesh4headerSize = %d\n", mesh4headerSize);
   }

   //
   // check avm_mesh_header_offset for each mesh
   //

   EXPECT_EQ(0, avm_mesh_header_offset(fileid, 1, &offset));
   EXPECT_EQ(fileHeaderSize, 
             int(offset));

   EXPECT_EQ(0, avm_mesh_header_offset(fileid, 2, &offset));
   EXPECT_EQ(fileHeaderSize + mesh1headerSize, 
             int(offset));

   EXPECT_EQ(0, avm_mesh_header_offset(fileid, 3, &offset));
   EXPECT_EQ(fileHeaderSize + mesh1headerSize + 
             mesh2headerSize, 
             int(offset));

   EXPECT_EQ(0, avm_mesh_header_offset(fileid, 4, &offset));
   EXPECT_EQ(fileHeaderSize + mesh1headerSize + 
             mesh2headerSize + mesh3headerSize, 
             int(offset));

   int meshHeaderSize = mesh1headerSize + mesh2headerSize + 
                        mesh3headerSize + mesh4headerSize; 

   // NOTE: request position of one past the last mesh header block.
   EXPECT_EQ(0, avm_mesh_header_offset(fileid, 5, &offset));
   EXPECT_EQ(fileHeaderSize + meshHeaderSize, int(offset));

   int mesh1dataSize = 0;
   {
      int jmax;
      int kmax;
      int lmax;
      int iblank;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
      EXPECT_EQ(0, avm_get_int(fileid, "jmax", &jmax));
      EXPECT_EQ(0, avm_get_int(fileid, "kmax", &kmax));
      EXPECT_EQ(0, avm_get_int(fileid, "lmax", &lmax));
      EXPECT_EQ(0, avm_get_int(fileid, "iblank", &iblank));

      // add in xyz size
      mesh1dataSize += jmax * kmax * lmax * (1==precision ? 4 : 8) * 3;

      // add in iblank size
      if (iblank) mesh1dataSize += jmax * kmax * lmax * 4;

      //fprintf(stderr, "FYI: mesh1dataSize = %d\n", mesh1dataSize);
   }

   int mesh2dataSize = 0;
   {
      int nBndEdges;
      int nPtsPerStrand;
      int nQuadFaces;
      int nSurfNodes;
      int nTriFaces;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &nBndEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &nPtsPerStrand));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nSurfNodes", &nSurfNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));

      // and in strand mesh size
      mesh2dataSize = 3 * nTriFaces * 4 +
                      4 * nQuadFaces * 4 +
                      2 * nBndEdges * 4 +
                      nSurfNodes * 4 +
                      ( nTriFaces + nQuadFaces ) * 4 +
                      ( nTriFaces + nQuadFaces ) * 4 +
                      nBndEdges * 4 +
                      3 * nSurfNodes * (1==precision ? 4 : 8) +
                      3 * nSurfNodes * (1==precision ? 4 : 8) +
                      nPtsPerStrand * (1==precision ? 4 : 8);

      //fprintf(stderr, "FYI: mesh2dataSize = %d\n", mesh2dataSize);
   }

   int mesh3dataSize = 0;
   {
      int nNodes;
      int nTriFaces;
      int nQuadFaces;
      int nBndTriFaces;
      int nBndQuadFaces;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 3));
      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndTriFaces", &nBndTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndQuadFaces", &nBndQuadFaces));

      // and in unstruc mesh size
      mesh3dataSize += 3 * nNodes * (1==precision ? 4 : 8) +
                       5 * nBndTriFaces * 4 +
                       6 * nBndQuadFaces * 4 +
                       5 * (nTriFaces - nBndTriFaces) * 4 +
                       6 * (nQuadFaces - nBndQuadFaces) * 4;

      //fprintf(stderr, "FYI: mesh3dataSize = %d\n", mesh3dataSize);
   }

   int mesh4dataSize = 0;
   {
      int nBlocks;
      int iblFlag;
      int iblDim[3];
      EXPECT_EQ(0, avm_select(fileid, "mesh", 4));
      EXPECT_EQ(0, avm_get_int(fileid, "nBlocks", &nBlocks));

      // add in iblanks that are stored in the mesh data section (if any)
      for (int i=1; i<=nBlocks; ++i)
      {
         EXPECT_EQ(0, avm_select(fileid, "block", i));
         EXPECT_EQ(0, avm_get_int(fileid, "iblFlag", &iblFlag));
         EXPECT_EQ(0, avm_get_int_array(fileid, "iblDim", iblDim, 3));
         if (iblFlag==2) mesh4dataSize += iblDim[0] * iblDim[1] * iblDim[2] * 4;
      }

      //fprintf(stderr, "FYI: mesh4dataSize = %d\n", mesh4dataSize);
   }

   //
   // check avm_mesh_data_offset for each mesh
   //

   EXPECT_EQ(0, avm_mesh_data_offset(fileid, 1, &offset));
   EXPECT_EQ(fileHeaderSize + meshHeaderSize,
             int(offset));

   EXPECT_EQ(0, avm_mesh_data_offset(fileid, 2, &offset));
   EXPECT_EQ(fileHeaderSize + meshHeaderSize +
             mesh1dataSize,
             int(offset));

   EXPECT_EQ(0, avm_mesh_data_offset(fileid, 3, &offset));
   EXPECT_EQ(fileHeaderSize + meshHeaderSize +
             mesh1dataSize + mesh2dataSize,
             int(offset));

   EXPECT_EQ(0, avm_mesh_data_offset(fileid, 4, &offset));
   EXPECT_EQ(fileHeaderSize + meshHeaderSize +
             mesh1dataSize + mesh2dataSize +
             mesh3dataSize,
             int(offset));

   // NOTE: request position of one past the last mesh data block.
   // This should be the file size, so check this.
   EXPECT_EQ(0, avm_mesh_data_offset(fileid, 5, &offset));
   {
      FILE* fp=fopen(filename, "rb");
      fseek(fp, 0L, SEEK_END);
      EXPECT_EQ(int(ftello(fp)), offset);
      fclose(fp);
   }
}

TEST_F(TestFixture, headers) {
   int meshCount;
   double modelScale;
   double ref_point[3];
   char type[AVM_STD_STRING_LENGTH];
   char contactName[AVM_STD_STRING_LENGTH];

   EXPECT_EQ(0, avm_select(fileid, "header", 0));
   EXPECT_EQ(0, avm_get_str(fileid, "contactName", contactName, AVM_STD_STRING_LENGTH));

   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));
   EXPECT_EQ(4, meshCount);

   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", type, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("bfstruc", type);

      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &modelScale));
      EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", ref_point, 3));
      EXPECT_DOUBLE_EQ(1.0, ref_point[0]);
      EXPECT_DOUBLE_EQ(2.0, ref_point[1]);
      EXPECT_DOUBLE_EQ(3.0, ref_point[2]);

      int jmax,kmax,lmax;
      EXPECT_EQ(0, avm_get_int(fileid, "jmax", &jmax));
      EXPECT_EQ(0, avm_get_int(fileid, "kmax", &kmax));
      EXPECT_EQ(0, avm_get_int(fileid, "lmax", &lmax));
   }

   EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", type, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("strand", type);

      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &modelScale));
      EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", ref_point, 3));
      EXPECT_DOUBLE_EQ(4.0, ref_point[0]);
      EXPECT_DOUBLE_EQ(5.0, ref_point[1]);
      EXPECT_DOUBLE_EQ(6.0, ref_point[2]);

      int nBndEdges,nPtsPerStrand;
      EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &nBndEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &nPtsPerStrand));
   }

   EXPECT_EQ(0, avm_select(fileid, "mesh", 3));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", type, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("unstruc", type);

      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &modelScale));
      EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", ref_point, 3));
      EXPECT_DOUBLE_EQ(7.0, ref_point[0]);
      EXPECT_DOUBLE_EQ(8.0, ref_point[1]);
      EXPECT_DOUBLE_EQ(9.0, ref_point[2]);

      int nNodes,nEdges,nFaces;
      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdges", &nEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nFaces", &nFaces));
   }

   EXPECT_EQ(0, avm_select(fileid, "mesh", 4));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", type, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("cart", type);

      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &modelScale));
      EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", ref_point, 3));
      EXPECT_DOUBLE_EQ(10.0, ref_point[0]);
      EXPECT_DOUBLE_EQ(11.0, ref_point[1]);
      EXPECT_DOUBLE_EQ(12.0, ref_point[2]);

      int nLevels,nBlocks,nFringe;
      EXPECT_EQ(0, avm_get_int(fileid, "nLevels", &nLevels));
      EXPECT_EQ(0, avm_get_int(fileid, "nBlocks", &nBlocks));
      EXPECT_EQ(0, avm_get_int(fileid, "nFringe", &nFringe));
   }
}

TEST_F(TestFixture, data_reads) {
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(4, i);

   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("bfstruc", str);

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

   EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("strand", str);

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
      float* xSurf = new (nothrow) float[3*nSurfNodes];
      float* pointingVec = new (nothrow) float[3*nSurfNodes];
      float* xStrand = new (nothrow) float[nPtsPerStrand];

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

      EXPECT_EQ(0, avm_strand_read_r4(fileid,
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
            EXPECT_EQ(i+1+j+1, triConn[(i*3)+j]); 

      for (int i=0; i<nPtsPerStrand; ++i)
         EXPECT_DOUBLE_EQ(i, xStrand[i]); 

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

   EXPECT_EQ(0, avm_select(fileid, "mesh", 3));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("unstruc", str);

      int p;
      int nNodes;
      int nTriFaces;
      int nQuadFaces;

      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));

      float* xyz = new (nothrow) float[3*nNodes];
      int* triFaces = new (nothrow) int[5*nTriFaces];
      int* quadFaces = new (nothrow) int[6*nQuadFaces];

      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);

      EXPECT_EQ(0, avm_unstruc_read_nodes_r4(fileid, xyz, 3*nNodes));
      EXPECT_EQ(0, avm_unstruc_read_faces(fileid, triFaces,  5*nTriFaces,
                                                  quadFaces, 6*nQuadFaces));

      // nodes
      if (0) {
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
         int BUFF_LEN=5, FIRST_POINT=0, LAST_POINT=8;
         p = FIRST_POINT * BUFF_LEN;
         EXPECT_EQ(5, triFaces[p+0]);
         EXPECT_EQ(12, triFaces[p+1]);
         EXPECT_EQ(6, triFaces[p+2]);
         EXPECT_EQ(3, triFaces[p+3]);
         EXPECT_EQ(-1, triFaces[p+4]);
         p = LAST_POINT * BUFF_LEN;
         EXPECT_EQ(5, triFaces[p+0]);
         EXPECT_EQ(6, triFaces[p+1]);
         EXPECT_EQ(11, triFaces[p+2]);
         EXPECT_EQ(3, triFaces[p+3]);
         EXPECT_EQ(2, triFaces[p+4]);
      }

      // check the first and last quadFace 
      {
         int BUFF_LEN=6, FIRST_POINT=0, LAST_POINT=7;
         p = FIRST_POINT * BUFF_LEN;
         EXPECT_EQ(1, quadFaces[p+0]);
         EXPECT_EQ(4, quadFaces[p+1]);
         EXPECT_EQ(5, quadFaces[p+2]);
         EXPECT_EQ(6, quadFaces[p+3]);
         EXPECT_EQ(1, quadFaces[p+4]);
         EXPECT_EQ(-1, quadFaces[p+5]);
         p = LAST_POINT * BUFF_LEN;
         EXPECT_EQ(5, quadFaces[p+0]);
         EXPECT_EQ(6, quadFaces[p+1]);
         EXPECT_EQ(7, quadFaces[p+2]);
         EXPECT_EQ(8, quadFaces[p+3]);
         EXPECT_EQ(1, quadFaces[p+4]);
         EXPECT_EQ(2, quadFaces[p+5]);
      }

      delete []xyz;
      delete []triFaces;
      delete []quadFaces;
   }

   EXPECT_EQ(0, avm_select(fileid, "mesh", 4));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("cart", str);

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
}

TEST_F(TestFixture, data_reads_in_opposite_order) {
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(4, i);

   EXPECT_EQ(0, avm_select(fileid, "mesh", 4));
   EXPECT_EQ(0, avm_seek_to_mesh(fileid, 4));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("cart", str);

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

   EXPECT_EQ(0, avm_select(fileid, "mesh", 3));
   EXPECT_EQ(0, avm_seek_to_mesh(fileid, 3));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("unstruc", str);

      int p;
      int nNodes;
      int nTriFaces;
      int nQuadFaces;

      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));

      float* xyz = new (nothrow) float[3*nNodes];
      int* triFaces = new (nothrow) int[5*nTriFaces];
      int* quadFaces = new (nothrow) int[6*nQuadFaces];

      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);

      EXPECT_EQ(0, avm_unstruc_read_nodes_r4(fileid, xyz, 3*nNodes));
      EXPECT_EQ(0, avm_unstruc_read_faces(fileid, triFaces,  5*nTriFaces,
                                                  quadFaces, 6*nQuadFaces));

      // nodes
      if (0) {
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
         int BUFF_LEN=5, FIRST_POINT=0, LAST_POINT=8;
         p = FIRST_POINT * BUFF_LEN;
         EXPECT_EQ(5, triFaces[p+0]);
         EXPECT_EQ(12, triFaces[p+1]);
         EXPECT_EQ(6, triFaces[p+2]);
         EXPECT_EQ(3, triFaces[p+3]);
         EXPECT_EQ(-1, triFaces[p+4]);
         p = LAST_POINT * BUFF_LEN;
         EXPECT_EQ(5, triFaces[p+0]);
         EXPECT_EQ(6, triFaces[p+1]);
         EXPECT_EQ(11, triFaces[p+2]);
         EXPECT_EQ(3, triFaces[p+3]);
         EXPECT_EQ(2, triFaces[p+4]);
      }

      // check the first and last quadFace 
      {
         int BUFF_LEN=6, FIRST_POINT=0, LAST_POINT=7;
         p = FIRST_POINT * BUFF_LEN;
         EXPECT_EQ(1, quadFaces[p+0]);
         EXPECT_EQ(4, quadFaces[p+1]);
         EXPECT_EQ(5, quadFaces[p+2]);
         EXPECT_EQ(6, quadFaces[p+3]);
         EXPECT_EQ(1, quadFaces[p+4]);
         EXPECT_EQ(-1, quadFaces[p+5]);
         p = LAST_POINT * BUFF_LEN;
         EXPECT_EQ(5, quadFaces[p+0]);
         EXPECT_EQ(6, quadFaces[p+1]);
         EXPECT_EQ(7, quadFaces[p+2]);
         EXPECT_EQ(8, quadFaces[p+3]);
         EXPECT_EQ(1, quadFaces[p+4]);
         EXPECT_EQ(2, quadFaces[p+5]);
      }

      delete []xyz;
      delete []triFaces;
      delete []quadFaces;
   }

   EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
   EXPECT_EQ(0, avm_seek_to_mesh(fileid, 2));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("strand", str);

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
      float* xSurf = new (nothrow) float[3*nSurfNodes];
      float* pointingVec = new (nothrow) float[3*nSurfNodes];
      float* xStrand = new (nothrow) float[nPtsPerStrand];

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

      EXPECT_EQ(0, avm_strand_read_r4(fileid,
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
            EXPECT_EQ(i+1+j+1, triConn[(i*3)+j]); 

      for (int i=0; i<nPtsPerStrand; ++i)
         EXPECT_DOUBLE_EQ(i, xStrand[i]); 

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

   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
   EXPECT_EQ(0, avm_seek_to_mesh(fileid, 1));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("bfstruc", str);

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
