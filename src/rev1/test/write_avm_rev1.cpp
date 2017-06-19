//----------------------------------------------------------------------------80
// Author: James S Masters
// Date: Sep2015
// Discription:
//  Code for writing AVMesh Rev-1 file using AVMeshlib functions
//----------------------------------------------------------------------------80
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

#include "write_avm_rev1.h"
#include "checkpoint.h"
#include "avmesh.h"
#include "structures.h"

void write_avm_rev1(const char*               meshfile,
                    vector< vector<double> >  &xyzVector,
                    vector< vector<int> >     &hex,       // [nHexCells][8]
                    vector< vector<int> >     &tet,       // [nTetCells][4]
                    vector< vector<int> >     &pri,       // [nPriCells][6]
                    vector< vector<int> >     &pyr,       // [nPyrCells][5]
                    vector< vector<int> >     &bndTri,    // [nBndTri][5]
                    vector< vector<int> >     &bndQuad,   // [nBndQuad][6]
                    avmesh_metadata           &avmData)
{
  printf("\n --- Writing out modified AVMesh Rev-1 mesh file <%s> ---\n",meshfile);
  //--- Variables ------------------------------------------------------------80
  int i,j;          // Counters
  int avmid;        // AVMesh ID
  int callStatus;   // return value for an avm function calls

  double refPt[3];  // reference point
  refPt[0] = 0.0; refPt[1] = 0.0; refPt[2] = 0.0;
 
  //--- initialize File Header Info ------------------------------------------80
  callStatus = avm_new_file(&avmid, meshfile, avmData.formatRevision);
  if(callStatus != 0){CHECKPT; exit(1);}

  callStatus = avm_select(avmid, "header", 0);
  if(callStatus != 0)
  {
    printf("\n ERROR: %s\n", avm_get_error_str() );
    CHECKPT; 
    exit(1);
  }

  callStatus = avm_set_str(avmid, "AVM_WRITE_ENDIAN", "native", AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "meshCount", 1);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "contactInfo", "James Masters", AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "precision", 2);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "dimensions", 3);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "description", "smoothed Rev-1 AVMesh", AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}

  printf("  --- File Header info ---\n");
  printf("   formatRevision:      %d\n",avmData.formatRevision);
  printf("   AVM_WRITE_ENDIAN:    native\n");
  printf("   meshCount:           1\n");
  printf("   contactInfo:         James Masters\n");
  printf("   precision:           2\n");
  printf("   dimensions:          3\n");
  printf("   description:         smoothed Rev-1 AVMesh\n");

  //--- Initialize Mesh1 Header Info (currently only one mesh is supported) --80
  callStatus = avm_select(avmid,  "mesh", 1);
  if(callStatus != 0){CHECKPT; exit(1);}

  callStatus = avm_set_str(avmid, "meshName",                 "mesh1",            AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "meshType",                 "unstruc",          AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "meshGenerator",            "write_avm_rev1",   AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "coordinateSystem",         "xByRzU",           AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "meshDescription",          "mesh numero uno",  AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "gridUnits",                 avmData.gridUnits, AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_real(avmid, "modelScale",               avmData.modelScale);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_real(avmid, "referenceLength",          avmData.referenceLength);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_real(avmid, "referenceArea",            avmData.referenceLength);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_real_array(avmid, "referencePoint",     refPt, 3);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_str(avmid, "referencePointDescription", "origin",        AVM_STD_STRING_LENGTH);
  if(callStatus != 0){CHECKPT; exit(1);}

  printf("\n  --- Mesh1 Header info ---\n");
  printf("   Mesh Name:           %s\n","mesh1");
  printf("   Mesh Type:           %s\n","unstruc");
  printf("   Mesh Generator:      %s\n","write_avm_rev1");
  printf("   coordinateSystem:    %s\n","xByRzU");
  printf("   meshDescription:     %s\n","mesh numero uno");
  printf("   gridUnits:           %s\n",avmData.gridUnits);
  printf("   modelScale:          %f\n",avmData.modelScale);
  printf("   referenceLength:     %f\n",1.0);
  printf("   referenceArea:       %f\n",1.0);
  printf("   referencePoint:      (%f,%f,%f)\n",refPt[0],refPt[1],refPt[0]);
  printf("   refPt description:   %s\n","origin");

  //--- Initialize Mesh1 Mesh Info -------------------------------------------80
  callStatus = avm_set_int(avmid, "nNodes",           avmData.nNodes);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nEdges",           avmData.nEdges);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nFaces",           avmData.nFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nTriFaces",        avmData.nTriFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nQuadFaces",       avmData.nQuadFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nBndTriFaces",     avmData.nBndTriFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nBndQuadFaces",    avmData.nBndQuadFaces);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nCells",           avmData.nCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nHexCells",        avmData.nHexCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nTetCells",        avmData.nTetCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nPriCells",        avmData.nPriCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nPyrCells",        avmData.nPyrCells);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nPatches",         avmData.patch_count);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nMaxNodesPerFace", avmData.nMaxNodesPerFace);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nMaxNodesPerCell", avmData.nMaxNodesPerCell);
  if(callStatus != 0){CHECKPT; exit(1);}
  callStatus = avm_set_int(avmid, "nMaxFacesPerCell", avmData.nMaxFacesPerCell);
  if(callStatus != 0){CHECKPT; exit(1);}

  printf("\n  --- Mesh1 Mesh info ---\n");
  printf("   nNodes:              %d\n",avmData.nNodes);
  printf("   nFaces:              %d\n",avmData.nFaces);
  printf("   nBndTriFaces:        %d\n",avmData.nBndTriFaces);
  printf("   nBndQuadFaces:       %d\n",avmData.nBndQuadFaces);
  printf("   nCells:              %d\n",avmData.nCells);
  printf("   nHexCells:           %d\n",avmData.nHexCells);
  printf("   nTetCells:           %d\n",avmData.nTetCells);
  printf("   nPriCells:           %d\n",avmData.nPriCells);
  printf("   nPyrCells:           %d\n",avmData.nPyrCells);
  printf("   nPatches:            %d\n",avmData.patch_count);

  //--- Initialize Boundary Patch Header Info --------------------------------80
  printf("\n  --- Patch Header info ---");
  for(j=0; j<avmData.patch_count; j++)
  {
    avm_select(avmid, "patch", avmData.patch[j].patchID);

    callStatus = avm_set_int(avmid, "patchId",   -avmData.patch[j].patchID);
    if(callStatus != 0){CHECKPT; exit(1);}
    callStatus = avm_set_str(avmid, "patchLabel", avmData.patch[j].patchLabel, 32);
    if(callStatus != 0){CHECKPT; exit(1);}
    callStatus = avm_set_str(avmid, "patchType",  avmData.patch[j].patchType, 16);
    if(callStatus != 0){CHECKPT; exit(1);}

    printf("\n  patch ID: %d\n",avmData.patch[j].patchID);
    printf("   Label:     %s\n",avmData.patch[j].patchLabel);
    printf("   Type:      %s\n",avmData.patch[j].patchType);
    printf("   TriFaces:  %d\n",avmData.patch[j].nBndTriFaces);
    printf("   QuadFaces: %d\n",avmData.patch[j].nBndQuadFaces);
  }

  //--- Write All Header Info to File ----------------------------------------80
  callStatus = avm_write_headers(avmid); 
  if(callStatus != 0){CHECKPT; exit(1);}

  //--- Write Node (coordinate) Info to File ---------------------------------80
  double* xyz = new (nothrow) double[3*avmData.nNodes];

  callStatus = avm_select(avmid, "mesh", 1);
  if(callStatus != 0){CHECKPT; exit(1);}

  for(i=0; i<avmData.nNodes; i++)
  {
    xyz[i*3]     = xyzVector[i][0];
    xyz[i*3 + 1] = xyzVector[i][1];
    xyz[i*3 + 2] = xyzVector[i][2];
  }

  callStatus = avm_unstruc_write_nodes_r8(avmid, xyz, 3*avmData.nNodes);
  if(callStatus != 0){CHECKPT; exit(1);}

  //--- Write (base-1 indexed) cell Info to File ----------------------------80
  int* hexCells = new (nothrow) int[8*avmData.nHexCells];
  int* tetCells = new (nothrow) int[4*avmData.nTetCells];
  int* priCells = new (nothrow) int[6*avmData.nPriCells];
  int* pyrCells = new (nothrow) int[5*avmData.nPyrCells];

  for(i=0; i<avmData.nHexCells; i++)
    for(j=0; j<8; j++)
      hexCells[i*8 + j] = hex[i][j] + 1;

  for(i=0; i<avmData.nTetCells; i++)
    for(j=0; j<4; j++)
      tetCells[i*4 + j] = tet[i][j] + 1;

  for(i=0; i<avmData.nPriCells; i++)
    for(j=0; j<6; j++)
      priCells[i*6 + j] = pri[i][j] + 1;

  for(i=0; i<avmData.nPyrCells; i++)
    for(j=0; j<5; j++)
      pyrCells[i*5 + j] = pyr[i][j] + 1;

  callStatus = avm_unstruc_write_cells(avmid, 
                                       hexCells, 8*avmData.nHexCells,
                                       tetCells, 4*avmData.nTetCells,
                                       priCells, 6*avmData.nPriCells,
                                       pyrCells, 5*avmData.nPyrCells);
                             
  if(callStatus != 0){CHECKPT; exit(1);}


  //--- Write (base-1 indexed) Boundary Face Info to File -------------------80
  int* triFaces  = new (nothrow) int[5*avmData.nTriFaces];
  int* quadFaces = new (nothrow) int[6*avmData.nQuadFaces];
  int* polyFaces = NULL;

  for(i=0; i<avmData.nTriFaces; i++)
  {
    for(j=0; j<5; j++)
    {
    if(j<4)  
      triFaces[i*5 + j] = bndTri[i][j] + 1;
    else
      triFaces[i*5 + j] = bndTri[i][j];
    }
  }

  for(i=0; i<avmData.nQuadFaces; i++)
  {
    for(j=0; j<6; j++)
    {
    if(j<5)  
      quadFaces[i*6 + j] = bndQuad[i][j] + 1;
    else
      quadFaces[i*6 + j] = bndQuad[i][j];
    }
  }

  callStatus = avm_unstruc_write_faces(avmid,
                                       triFaces,  5*avmData.nTriFaces,
                                       quadFaces, 6*avmData.nQuadFaces,
                                       polyFaces, 0);
  if(callStatus != 0){CHECKPT; exit(1);}

  callStatus = avm_close(avmid);
  if(callStatus != 0){CHECKPT; exit(1);}

  delete xyz;
  delete hexCells;
  delete tetCells;
  delete priCells;
  delete pyrCells;
  delete triFaces;
  delete quadFaces;
  delete polyFaces;

  printf("\n");
}
