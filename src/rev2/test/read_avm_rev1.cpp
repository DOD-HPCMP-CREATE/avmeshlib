//----------------------------------------------------------------------------80
// Author: James S Masters
// Date: 11Sep2015
// Discription:
//   Code for reading in an AVMesh Rev1 file using AVMeshlib functions
//----------------------------------------------------------------------------80
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

#include "read_avm_rev1.h"
#include "avmesh.h"
#include "checkpoint.h"
#include "structures.h"

void read_avm_rev1(char*                     meshfile,
                   vector< vector<double> >  &xyzVector, // [nNodes][3]
                   vector< vector<int> >     &hex,       // [nHexCells][8]
                   vector< vector<int> >     &tet,       // [nTetCells][4]
                   vector< vector<int> >     &pri,       // [nPriCells][6]
                   vector< vector<int> >     &pyr,       // [nPyrCells][5]
                   vector< vector<int> >     &bndTri,    // [nBndTri][5]
                   vector< vector<int> >     &bndQuad,   // [nBndQuad][6]
                   avmesh_metadata           &avmData)
{
  printf("\n --- Reading in information from Rev1 AVMesh file <%s> ---\n",meshfile);

  char contactInfo[AVM_STD_STRING_LENGTH];
  char meshName[AVM_STD_STRING_LENGTH];
  char meshType[AVM_STD_STRING_LENGTH];
  char meshGenerator[AVM_STD_STRING_LENGTH];
  char gridUnits[AVM_STD_STRING_LENGTH];
  char patchLabel[33];      // Note: AVMesh lib reqires an extra NULL character byte 
  char patchType[17];          

  int i,j;                  // counters
  int callStatus;           // return value for an avm function call
  int avmid;                // mesh file ID number
  int formatRevision;       // mesh revision number
  int meshCount;            // number of meshes in file
  int descriptionSize;      // size of description field
  int nNodes;               // number of nodes 
  int nFaces;               // total number of faces, both tri and quad
  int nCells;               // total number of cells
  int nTriFaces;            // number of tri faces
  int nQuadFaces;           // number of quad faces
  int nPolyFaces;           // number of poly faces
  int polyFacesSize;        // numbero of edges of a poly-face
  int nHexCells;            // number of hexahedral cells
  int nTetCells;            // number of tetrahedral cells
  int nPriCells;            // number of prism cells
  int nPyrCells;            // number of pyramid cells
  int nEdges;               // number of edges
  int nNodesOnGeometry;     // number nodes on geometry
  int nEdgesOnGeometry;     // number of edges on geometry 
  int nFacesOnGeometry;     // number of faces on geometry 
  int nPatches;             // number of patches (i.e. boundaries)
  int nMaxNodesPerFace;     // maximum nodes per face
  int nMaxNodesPerCell;     // maximum nodes per cell
  int nMaxFacesPerCell;     // maximum faces per cell
  int nBndTriFaces;         // total number of quad faces on boundaries
  int nBndQuadFaces;        // total number of quad faces on boundaries

  double modelScale;        // Model Scale from mesh file
  double referenceLength;   // Reference length
  double referenceArea;     // Reference area

  callStatus = avm_read_headers(&avmid, meshfile);
  if(callStatus != 0){CHECKPT; exit(1);}

  //--- Read in General File and Header information from mesh file -----------80
  callStatus = avm_get_int(avmid, "formatRevision", &formatRevision);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "meshCount", &meshCount);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "descriptionSize", &descriptionSize);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_str(avmid, "contactInfo",   contactInfo, AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  char description[descriptionSize];
  callStatus = avm_get_str(avmid, "description",   description, descriptionSize);
  if(callStatus != 0){CHECKPT; exit(1);}

  avm_select(avmid, "mesh", 1);

  callStatus = avm_get_str(avmid, "meshName", meshName, AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_str(avmid, "meshType", meshType, AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_str(avmid, "meshGenerator", meshGenerator, AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_str(avmid, "gridUnits", gridUnits, AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_real(avmid, "modelScale", &modelScale);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_real(avmid, "referenceLength", &referenceLength);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_real(avmid, "referenceArea", &referenceArea);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nNodes", &nNodes);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nFaces", &nFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nTriFaces", &nTriFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nQuadFaces", &nQuadFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nPolyFaces", &nPolyFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nCells", &nCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nHexCells", &nHexCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nTetCells", &nTetCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nPriCells", &nPriCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nPyrCells", &nPyrCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nEdges", &nEdges);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nPatches", &nPatches);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nMaxNodesPerFace", &nMaxNodesPerFace);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nMaxNodesPerCell", &nMaxNodesPerCell);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nMaxFacesPerCell", &nMaxFacesPerCell);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nBndTriFaces", &nBndTriFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nBndQuadFaces", &nBndQuadFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nNodesOnGeometry", &nNodesOnGeometry);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nEdgesOnGeometry", &nEdgesOnGeometry);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "nFacesOnGeometry", &nFacesOnGeometry);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_get_int(avmid, "polyFacesSize", &polyFacesSize);
  if(callStatus != 0){CHECKPT; exit(1);}
  if(polyFacesSize != 0){CHECKPT; exit(1);}

  printf("  --- Header information ---\n");
  printf("  contact name:        %s\n",contactInfo);
  printf("  description:         %s\n",description);
  printf("  Mesh Name:           %s\n",meshName);
  printf("  Mesh Type:           %s\n",meshType);
  printf("  Mesh Generator:      %s\n",meshGenerator);
  printf("  formatRevision:      %d\n",formatRevision);
  printf("  meshCount:           %d\n",meshCount);
  printf("  nNodes:              %d\n",nNodes);
  printf("  nFaces:              %d\n",nFaces);
  printf("  nCells:              %d\n",nCells);
  printf("  nHexCells:           %d\n",nHexCells);
  printf("  nTetCells:           %d\n",nTetCells);
  printf("  nPriCells:           %d\n",nPriCells);
  printf("  nPyrCells:           %d\n",nPyrCells);
  printf("  nPatches:            %d\n",nPatches);
  printf("  nBndTriFaces:        %d\n",nBndTriFaces);
  printf("  nBndQuadFaces:       %d\n",nBndQuadFaces);
  printf("  nNodesOnGeometry:    %d\n",nNodesOnGeometry);
  printf("  nEdgesOnGeometry:    %d\n",nEdgesOnGeometry);
  printf("  nFacesOnGeometry:    %d\n",nFacesOnGeometry);

  //--- Read in Node (coordinate) data and create Vectors --------------------80
  double* xyz       = new (nothrow) double[3*nNodes];

  callStatus = avm_unstruc_read_nodes_r8(avmid, xyz, 3*nNodes);
  if(callStatus != 0){CHECKPT; exit(1);}

  xyzVector.resize(nNodes);
  for(i=0; i<nNodes; i++)
    xyzVector[i].resize(3);

  for(i=0; i<nNodes; i++) 
    for(j=0; j<3; j++) 
      xyzVector[i][j] = xyz[i*3 + j];

  //--- Read in Cell data and create Vectors ---------------------------------80
  int*    hexCells  = new (nothrow) int[8*nHexCells];
  int*    tetCells  = new (nothrow) int[4*nTetCells];
  int*    priCells  = new (nothrow) int[6*nPriCells];
  int*    pyrCells  = new (nothrow) int[5*nPyrCells];

  callStatus = avm_unstruc_read_cells(avmid,
                                      hexCells,  8*nHexCells,
                                      tetCells,  4*nTetCells,
                                      priCells,  6*nPriCells,
                                      pyrCells,  5*nPyrCells);
  if(callStatus != 0){CHECKPT; exit(1);}
 
  hex.resize(nHexCells);
  for(i=0; i<nHexCells; i++)
    hex[i].resize(8);
  tet.resize(nTetCells);
  for(i=0; i<nTetCells; i++)
    tet[i].resize(4);
  pri.resize(nHexCells);
  for(i=0; i<nPriCells; i++)
    pri[i].resize(6);
  pyr.resize(nPyrCells);
  for(i=0; i<nPyrCells; i++)
    pyr[i].resize(5);

  //--- create zero-based vectors ---
  for(i=0; i<nHexCells; i++) 
    for(j=0; j<8; j++) 
      hex[i][j] = hexCells[i*8 + j] - 1;

  for(i=0; i<nTetCells; i++) 
    for(j=0; j<4; j++) 
      tet[i][j] = tetCells[i*4 + j] - 1;

  for(i=0; i<nPriCells; i++) 
    for(j=0; j<6; j++) 
      pri[i][j] = priCells[i*6 + j] - 1;

  for(i=0; i<nPyrCells; i++) 
    for(j=0; j<5; j++) 
      pyr[i][j] = pyrCells[i*5 + j] - 1;

  //--- Read in Boundary Face Data and create Vectors and Structures ---------80
  printf(" \n --- boundary patch information ---\n");
  int*    triFaces  = new (nothrow) int[5*nTriFaces];
  int*    quadFaces = new (nothrow) int[6*nQuadFaces];
  int*    polyFaces = new (nothrow) int[polyFacesSize*nPolyFaces];

  avmData.patch.resize(nPatches);

  callStatus = avm_unstruc_read_faces(avmid, 
                                      triFaces,  5*nTriFaces,
                                      quadFaces, 6*nQuadFaces,
                                      polyFaces, polyFacesSize);
  if(callStatus != 0){CHECKPT; exit(1);}

  bndTri.resize(nTriFaces);
  for(i=0; i<nTriFaces; i++)
    bndTri[i].resize(5);
  bndQuad.resize(nQuadFaces);
  for(i=0; i<nQuadFaces; i++)
    bndQuad[i].resize(6);

  for(i=0; i<nTriFaces; i++) 
  {
    for(j=0; j<5; j++) 
    {
      if(j<4)
        bndTri[i][j] = triFaces[i*5 + j] - 1;
      else
      {
        bndTri[i][j] = triFaces[i*5 + j];
        avmData.patch[-bndTri[i][j] -1].nBndTriFaces++;
      }
    }
  }

  for(i=0; i<nQuadFaces; i++) 
  {
    for(j=0; j<6; j++) 
    {
      if(j<5)
        bndQuad[i][j] = quadFaces[i*6 + j] - 1;
      else
      {
        bndQuad[i][j] = quadFaces[i*6 + j];
        avmData.patch[-bndQuad[i][j] -1].nBndQuadFaces++;
      }
    }
  }

  for(i=0; i<nPatches; i++)
  {
    avmData.patch[i].patchID = i+1;

    avm_select(avmid, "patch", i+1);
    callStatus = avm_get_str(avmid, "patchLabel",  patchLabel, 32);
    if(callStatus != 0){CHECKPT; exit(1);}
    callStatus = avm_get_str(avmid, "patchType",   patchType, 16);
    if(callStatus != 0){CHECKPT; exit(1);}
  
    strncpy( avmData.patch[i].patchLabel, patchLabel, 32);
    strncpy( avmData.patch[i].patchType, patchType, 16);
  }

  for(i=0; i<nPatches; i++)
  {
    printf("  --- patch # %d ---\n",i);
    printf("   patchID:       %d\n",avmData.patch[i].patchID);
    printf("   patchLabel:    %s\n",avmData.patch[i].patchLabel);
    printf("   patchType:     %s\n",avmData.patch[i].patchType);
    printf("   nBndTriFaces:  %d\n",avmData.patch[i].nBndTriFaces);
    printf("   nBndQuadFaces: %d\n",avmData.patch[i].nBndQuadFaces);
  }

  //--- Populate AVMesh data structure ---------------------------------------80
  avmData.formatRevision = formatRevision;
  avmData.nNodes = nNodes;
  avmData.nEdges = nEdges;
  avmData.nFaces = nFaces;
  avmData.nTriFaces = nTriFaces;
  avmData.nQuadFaces = nQuadFaces;
  avmData.nBndTriFaces = nBndTriFaces;
  avmData.nBndQuadFaces = nBndQuadFaces;
  avmData.nCells = nCells;
  avmData.nHexCells = nHexCells;
  avmData.nTetCells = nTetCells;
  avmData.nPriCells = nPriCells;
  avmData.nPyrCells = nPyrCells;
  avmData.patch_count = nPatches;
  avmData.nMaxNodesPerFace = nMaxNodesPerFace;
  avmData.nMaxNodesPerCell = nMaxNodesPerCell;
  avmData.nMaxFacesPerCell = nMaxFacesPerCell;
  avmData.totalPatchFaces = nBndTriFaces+nBndQuadFaces;
  avmData.modelScale = modelScale;
  avmData.referenceLength = referenceLength;
  avmData.referenceArea = referenceArea;
  strncpy(avmData.gridUnits, gridUnits, AVM_STD_STRING_LENGTH);
  
  delete xyz;
  delete hexCells;
  delete tetCells;
  delete priCells;
  delete pyrCells;
  delete triFaces;
  delete quadFaces;
  delete polyFaces;
}
