#include <cstdio>
using namespace std;
#include <gtest/gtest.h>
#include "avmesh.h"

const char* filename = "mixed.avm";

TEST(Example, generic_header_interface_example) {
   int fileid;
   int meshCount;
   char contactInfo[AVM_STD_STRING_LENGTH];

   EXPECT_EQ(0, avm_read_headers(&fileid, filename));

   EXPECT_EQ(0, avm_select(fileid, "header", 0));
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));
   EXPECT_EQ(2, meshCount);
   EXPECT_EQ(0, avm_get_str(fileid, "contactInfo", contactInfo, AVM_STD_STRING_LENGTH));

   for (int i=1; i<=meshCount; ++i) {
      double modelScale, ref_point[3];
      char name[AVM_STD_STRING_LENGTH], type[AVM_STD_STRING_LENGTH];

      EXPECT_EQ(0, avm_select(fileid, "mesh", i));
      EXPECT_EQ(0, avm_get_str(fileid, "meshName", name, AVM_STD_STRING_LENGTH));
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", type, AVM_STD_STRING_LENGTH));
      EXPECT_EQ(0, avm_get_real(fileid, "modelScale", &modelScale));
      EXPECT_EQ(0, avm_get_real_array(fileid, "referencePoint", ref_point, 3));

      if (0==strcmp(type, "strand")) {
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
   const int genericMeshHeaderSize = 964;
   int precision = 0;
   int dimensions = 0;

   int fileHeaderSize = 158;
   // check some mesh expectations and then
   // add in file description size
   {
      int descriptionSize;
      int meshCount;
      EXPECT_EQ(0, avm_select(fileid, "header", 0));
      EXPECT_EQ(0, avm_get_int(fileid, "descriptionSize", &descriptionSize));
      EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));
      EXPECT_EQ(2, meshCount);
      EXPECT_EQ(0, avm_get_int(fileid, "precision", &precision));
      EXPECT_EQ(1, precision);
      EXPECT_EQ(0, avm_get_int(fileid, "dimensions", &dimensions));
      EXPECT_EQ(3, dimensions);

      // add in file description size
      fileHeaderSize += descriptionSize-1; // (negate null-terminator byte)

      //fprintf(stderr, "FYI: fileHeaderSize = %d\n", fileHeaderSize);
   }

   int mesh1headerSize = genericMeshHeaderSize;
   // add in strand header pieces
   {
      int nSurfPatches;
      int nEdgePatches;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
      EXPECT_EQ(0, avm_get_int(fileid, "nSurfPatches", &nSurfPatches));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdgePatches", &nEdgePatches));

      // add in fixed part of strand header
      mesh1headerSize += 112;

      // add in strand patches
      mesh1headerSize += 100 * nSurfPatches;
      mesh1headerSize += 124 * nEdgePatches;

      //fprintf(stderr, "FYI: mesh1headerSize = %d\n", mesh1headerSize);
   }

   int mesh2headerSize = genericMeshHeaderSize;
   // add in unstruc header pieces
   {
      int nPatches;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
      EXPECT_EQ(0, avm_get_int(fileid, "nPatches", &nPatches));

      // add in fixed part of unstruc header
      mesh2headerSize += 88 + 32;

      // add in unstruc patches
      mesh2headerSize += 52 * nPatches;

      //fprintf(stderr, "FYI: mesh2headerSize = %d\n", mesh2headerSize);
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

   int meshHeaderSize = mesh1headerSize + mesh2headerSize;

   // NOTE: request position of one past the last mesh header block.
   EXPECT_EQ(0, avm_mesh_header_offset(fileid, 3, &offset));
   EXPECT_EQ(fileHeaderSize + meshHeaderSize, int(offset));

   int mesh1dataSize = 0;
   {
      int nBndEdges;
      int nPtsPerStrand;
      int polyFacesSize;
      int nPolyFaces;
      int nQuadFaces;
      int nNodes;
      int nStrands;
      int nTriFaces;
      int nNodesOnGeometry, nEdgesOnGeometry, nFacesOnGeometry;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndEdges", &nBndEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nPtsPerStrand", &nPtsPerStrand));
      EXPECT_EQ(0, avm_get_int(fileid, "polyFacesSize", &polyFacesSize));
      EXPECT_EQ(0, avm_get_int(fileid, "nPolyFaces", &nPolyFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nStrands", &nStrands));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nNodesOnGeometry", &nNodesOnGeometry));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdgesOnGeometry", &nEdgesOnGeometry));
      EXPECT_EQ(0, avm_get_int(fileid, "nFacesOnGeometry", &nFacesOnGeometry));

      // and in strand mesh size
      mesh1dataSize = 3 * nTriFaces * 4 + //triFaces
                      4 * nQuadFaces * 4 + //quadFaces
                      polyFacesSize * 4 + //polyFaces
                      2 * nBndEdges * 4 + //bndEdges
                      nStrands * 4 + //nodeID
                      nStrands * 4 + //nodeClip
                      ( nTriFaces + nQuadFaces + nPolyFaces ) * 4 + //faceTag
                      nBndEdges * 4 + //edgeTag
                      3 * nNodes * (1==precision ? 4 : 8) + //xyz
                      3 * nStrands * (1==precision ? 4 : 8) + //pointingVec
                      nPtsPerStrand * (1==precision ? 4 : 8) + //xStrand
                      28 * nNodesOnGeometry + //nodesOnGeometry
                      3 * nEdgesOnGeometry * 4 + //edgesOnGeometry
                      3 * nFacesOnGeometry * 4; //facesOnGeometry

      //fprintf(stderr, "FYI: mesh1dataSize = %d\n", mesh1dataSize);
   }

   int mesh2dataSize = 0;
   {
      int nNodes;
      int nTriFaces;
      int nQuadFaces;
      int nBndTriFaces;
      int nBndQuadFaces;
      int nHexCells, nTetCells, nPriCells, nPyrCells;
      int nNodesOnGeometry, nEdgesOnGeometry, nFacesOnGeometry;
      int nEdges;
      EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndTriFaces", &nBndTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nBndQuadFaces", &nBndQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdges", &nEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nHexCells", &nHexCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nTetCells", &nTetCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nPriCells", &nPriCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nPyrCells", &nPyrCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nNodesOnGeometry", &nNodesOnGeometry));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdgesOnGeometry", &nEdgesOnGeometry));
      EXPECT_EQ(0, avm_get_int(fileid, "nFacesOnGeometry", &nFacesOnGeometry));

      // and in unstruc mesh size
      mesh2dataSize += 3 * nNodes * (1==precision ? 4 : 8) +
                       4 * nBndTriFaces * 4 +
                       5 * nBndQuadFaces * 4 +
                       4 * (nTriFaces - nBndTriFaces) * 4 +
                       5 * (nQuadFaces - nBndQuadFaces) * 4 +
                       8 * nHexCells * 4 +
                       4 * nTetCells * 4 +
                       6 * nPriCells * 4 +
                       5 * nPyrCells * 4 +
                       2 * nEdges * 4 +
                       sizeof(AMR_Node_Data) * nNodesOnGeometry +
                       3 * nEdgesOnGeometry * 4 +
                       3 * nFacesOnGeometry * 4 +
                       nHexCells * 4 +
                       nTetCells * 4 +
                       nPriCells * 4 +
                       nPyrCells * 4;

      //fprintf(stderr, "FYI: mesh2dataSize = %d\n", mesh2dataSize);
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

   // NOTE: request position of one past the last mesh data block.
   // This should be the file size, so check this.
   EXPECT_EQ(0, avm_mesh_data_offset(fileid, 3, &offset));
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
   char contactInfo[AVM_STD_STRING_LENGTH];

   EXPECT_EQ(0, avm_select(fileid, "header", 0));
   EXPECT_EQ(0, avm_get_str(fileid, "contactInfo", contactInfo, AVM_STD_STRING_LENGTH));

   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &meshCount));
   EXPECT_EQ(2, meshCount);


   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
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

   EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
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
}

TEST_F(TestFixture, data_reads) {
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(2, i);

   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("strand", str);

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
      float* xyz = new (nothrow) float[3*nNodes];
      float* pointingVec = new (nothrow) float[3*nStrands];
      float* xStrand = new (nothrow) float[nPtsPerStrand];

      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);
      EXPECT_TRUE(0!=polyFaces);
      EXPECT_TRUE(0!=bndEdges);
      EXPECT_TRUE(0!=nodeID);
      EXPECT_TRUE(0!=nodeClip);
      EXPECT_TRUE(0!=faceTag);
      EXPECT_TRUE(0!=edgeTag);
      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=pointingVec);
      EXPECT_TRUE(0!=xStrand);
      EXPECT_TRUE(0!=polyFacesSize);

      EXPECT_EQ(0, avm_strand_read_nodes_r4(fileid,
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
         for (int j=0; j<3; ++j) {
            EXPECT_EQ(i+1+j+1, triFaces[(i*3)+j]); 
         }

      for (int i=0; i<nPtsPerStrand; ++i)
         EXPECT_DOUBLE_EQ(i, xStrand[i]); 

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

   EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("unstruc", str);

      int p;
      int nNodes;
      int nTriFaces, nQuadFaces;
      int nHexCells, nTetCells, nPriCells, nPyrCells;
      int nEdges;
      int nNodesOnGeometry, nEdgesOnGeometry, nFacesOnGeometry;

      EXPECT_EQ(0, avm_get_int(fileid, "nNodes", &nNodes));
      EXPECT_EQ(0, avm_get_int(fileid, "nTriFaces", &nTriFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nQuadFaces", &nQuadFaces));
      EXPECT_EQ(0, avm_get_int(fileid, "nHexCells", &nHexCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nTetCells", &nTetCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nPriCells", &nPriCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nPyrCells", &nPyrCells));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdges", &nEdges));
      EXPECT_EQ(0, avm_get_int(fileid, "nNodesOnGeometry", &nNodesOnGeometry));
      EXPECT_EQ(0, avm_get_int(fileid, "nEdgesOnGeometry", &nEdgesOnGeometry));
      EXPECT_EQ(0, avm_get_int(fileid, "nFacesOnGeometry", &nFacesOnGeometry));

      float* xyz = new (nothrow) float[3*nNodes];
      int* triFaces = new (nothrow) int[4*nTriFaces];
      int* quadFaces = new (nothrow) int[5*nQuadFaces];
      int* hexCells = new (nothrow) int[8*nHexCells];
      int* tetCells = new (nothrow) int[4*nTetCells];
      int* priCells = new (nothrow) int[6*nPriCells];
      int* pyrCells = new (nothrow) int[5*nPyrCells];
      int* edges = new (nothrow) int[2*nEdges];
      AMR_Node_Data* nodesOnGeometry = new (nothrow) AMR_Node_Data[nNodesOnGeometry];
      int* edgesOnGeometry = new (nothrow) int[3*nEdgesOnGeometry];
      int* facesOnGeometry = new (nothrow) int[3*nFacesOnGeometry];
      int* hexGeomIds = new (nothrow) int[nHexCells];
      int* tetGeomIds = new (nothrow) int[nTetCells];
      int* priGeomIds = new (nothrow) int[nPriCells];
      int* pyrGeomIds = new (nothrow) int[nPyrCells];

      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);

      EXPECT_EQ(0, avm_unstruc_read_nodes_r4(fileid, xyz, 3*nNodes));
      EXPECT_EQ(0, avm_unstruc_read_faces(fileid, triFaces,  4*nTriFaces,
                                                  quadFaces, 5*nQuadFaces));
      EXPECT_EQ(0, avm_unstruc_read_cells(fileid, hexCells,  8*nHexCells,
                                                  tetCells, 4*nTetCells,
                                                  priCells, 6*nPriCells,
                                                  pyrCells, 5*nPyrCells));
      EXPECT_EQ(0, avm_unstruc_read_edges(fileid, edges,  2*nEdges));
      EXPECT_EQ(0, avm_unstruc_read_amr(fileid, nodesOnGeometry,  nNodesOnGeometry,
                                                edgesOnGeometry, 3*nEdgesOnGeometry,
                                                facesOnGeometry, 3*nFacesOnGeometry));
      EXPECT_EQ(0, avm_unstruc_read_amr_volumeids(fileid, hexGeomIds, nHexCells,
                                                          tetGeomIds, nTetCells,
                                                          priGeomIds, nPriCells,
                                                          pyrGeomIds, nPyrCells));

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

      // check the first and last nodesOnGeometry
      {
         double tol=0.001;
         EXPECT_EQ(1, nodesOnGeometry[0].nodeIndex);
         EXPECT_EQ(0, nodesOnGeometry[0].geomType);
         EXPECT_EQ(1, nodesOnGeometry[0].geomTopoId);
         EXPECT_NEAR(0.0, nodesOnGeometry[0].u,tol);
         EXPECT_NEAR(0.0, nodesOnGeometry[0].v,tol);
         EXPECT_EQ(12, nodesOnGeometry[3].nodeIndex);
         EXPECT_EQ(1, nodesOnGeometry[3].geomType);
         EXPECT_EQ(7, nodesOnGeometry[3].geomTopoId);
         EXPECT_NEAR(0.6, nodesOnGeometry[3].u,tol);
         EXPECT_NEAR(0.0, nodesOnGeometry[3].v,tol);
      }

      delete []xyz;
      delete []triFaces;
      delete []quadFaces;
      delete []hexCells;
      delete []tetCells;
      delete []priCells;
      delete []pyrCells;
      delete []edges;
      delete []nodesOnGeometry;
      delete []edgesOnGeometry;
      delete []facesOnGeometry;
      delete []hexGeomIds;
      delete []tetGeomIds;
      delete []priGeomIds;
      delete []pyrGeomIds;
   }

}

TEST_F(TestFixture, data_reads_in_opposite_order) {
   EXPECT_EQ(0, avm_get_int(fileid, "meshCount", &i));
   EXPECT_EQ(2, i);


   EXPECT_EQ(0, avm_select(fileid, "mesh", 2));
   EXPECT_EQ(0, avm_seek_to_mesh(fileid, 2));
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
      int* triFaces = new (nothrow) int[4*nTriFaces];
      int* quadFaces = new (nothrow) int[5*nQuadFaces];

      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);

      EXPECT_EQ(0, avm_unstruc_read_nodes_r4(fileid, xyz, 3*nNodes));
      EXPECT_EQ(0, avm_unstruc_read_faces(fileid, triFaces,  4*nTriFaces,
                                                  quadFaces, 5*nQuadFaces));

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
      delete []triFaces;
      delete []quadFaces;
   }

   EXPECT_EQ(0, avm_select(fileid, "mesh", 1));
   EXPECT_EQ(0, avm_seek_to_mesh(fileid, 1));
   {
      EXPECT_EQ(0, avm_get_str(fileid, "meshType", str, AVM_STD_STRING_LENGTH));
      EXPECT_STREQ("strand", str);

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
      float* xyz = new (nothrow) float[3*nNodes];
      float* pointingVec = new (nothrow) float[3*nStrands];
      float* xStrand = new (nothrow) float[nPtsPerStrand];

      EXPECT_TRUE(0!=triFaces);
      EXPECT_TRUE(0!=quadFaces);
      EXPECT_TRUE(0!=polyFaces);
      EXPECT_TRUE(0!=bndEdges);
      EXPECT_TRUE(0!=nodeID);
      EXPECT_TRUE(0!=nodeClip);
      EXPECT_TRUE(0!=faceTag);
      EXPECT_TRUE(0!=edgeTag);
      EXPECT_TRUE(0!=xyz);
      EXPECT_TRUE(0!=pointingVec);
      EXPECT_TRUE(0!=xStrand);

      EXPECT_EQ(0, avm_strand_read_nodes_r4(fileid,
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
            EXPECT_EQ(i+1+j+1, triFaces[(i*3)+j]); 

      for (int i=0; i<nPtsPerStrand; ++i)
         EXPECT_DOUBLE_EQ(i, xStrand[i]); 

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
