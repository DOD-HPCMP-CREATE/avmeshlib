import sys
sys.path.append('..')
import avmeshlib as AVM
import unittest

class TestAVM(unittest.TestCase):

   def setUp(self):
      self.rev0id = -1
      self.rev1id = -1
      err, self.rev0id = AVM.read_headers(sys.argv[1])
      err, self.rev1id = AVM.read_headers(sys.argv[2])

   def test_compare0to1(self):
      #read rev0 file header
      err = AVM.select(self.rev0id, 'header', 0)
      self.assertEqual(err, 0)
      err = AVM.select(self.rev1id, 'header', 0)
      self.assertEqual(err, 0)
   
      err, meshCount1 = AVM.get_int(self.rev0id, 'meshCount')
      self.assertEqual(err, 0)
      err, meshCount2 = AVM.get_int(self.rev1id, 'meshCount')
      self.assertEqual(err, 0)
      self.assertEqual(meshCount1, meshCount2)

      err, contactName = AVM.get_str(self.rev0id, 'contactName', 128)
      self.assertEqual(err, 0)
      err, contactOrg = AVM.get_str(self.rev0id, 'contactOrg', 128)
      self.assertEqual(err, 0)
      err, contactEmail = AVM.get_str(self.rev0id, 'contactEmail', 128)
      self.assertEqual(err, 0)
      err, contactInfo = AVM.get_str(self.rev1id, 'contactInfo', 128)
      self.assertEqual(err, 0)
      self.assertEqual(contactInfo, contactName + " " + contactOrg + " " + contactEmail)

      err, precision1 = AVM.get_int(self.rev0id, 'precision')
      self.assertEqual(err, 0)
      err, precision2 = AVM.get_int(self.rev1id, 'precision')
      self.assertEqual(err, 0)
      self.assertEqual(precision1, precision2)

      err, dimensions1 = AVM.get_int(self.rev0id, 'dimensions')
      self.assertEqual(err, 0)
      err, dimensions2 = AVM.get_int(self.rev1id, 'dimensions')
      self.assertEqual(err, 0)
      self.assertEqual(dimensions1, dimensions2)

      err, descriptionSize1 = AVM.get_int(self.rev0id, 'descriptionSize')
      self.assertEqual(err, 0)
      err, descriptionSize2 = AVM.get_int(self.rev1id, 'descriptionSize')
      self.assertEqual(err, 0)
      #self.assertEqual(descriptionSize1, descriptionSize2)

      err, description1 = AVM.get_str(self.rev0id, 'description', descriptionSize1)
      self.assertEqual(err, 0)
      err, description2 = AVM.get_str(self.rev1id, 'description', descriptionSize2)
      self.assertEqual(err, 0)
      self.assertEqual(description1, description2)

      for meshNum in range(1,meshCount1+1):
         #read rev0 mesh headers
         err = AVM.select(self.rev0id, 'mesh', meshNum)
         self.assertEqual(err, 0)
         err = AVM.select(self.rev1id, 'mesh', meshNum)
         self.assertEqual(err, 0)

         err, meshName1 = AVM.get_str(self.rev0id, 'meshName', 128)
         self.assertEqual(err, 0)
         err, meshName2 = AVM.get_str(self.rev1id, 'meshName', 128)
         self.assertEqual(err, 0)
         self.assertEqual(meshName1, meshName2)

         err, meshType1 = AVM.get_str(self.rev0id, 'meshType', 128)
         self.assertEqual(err, 0)
         err, meshType2 = AVM.get_str(self.rev1id, 'meshType', 128)
         self.assertEqual(err, 0)
         #only test unstruc meshes
         self.assertEqual(meshType1, "unstruc")
         self.assertEqual(meshType2, "unstruc")

         err, meshGenerator1 = AVM.get_str(self.rev0id, 'meshGenerator', 128)
         self.assertEqual(err, 0)
         err, meshGenerator2 = AVM.get_str(self.rev1id, 'meshGenerator', 128)
         self.assertEqual(err, 0)
         self.assertEqual(meshGenerator1, meshGenerator2)

         err, coordinateSystem1 = AVM.get_str(self.rev0id, 'coordinateSystem', 128)
         self.assertEqual(err, 0)
         err, coordinateSystem2 = AVM.get_str(self.rev1id, 'coordinateSystem', 128)
         self.assertEqual(err, 0)
         self.assertEqual(coordinateSystem1, coordinateSystem2)

         err, modelScale1 = AVM.get_real(self.rev0id, 'modelScale')
         self.assertEqual(err, 0)
         err, modelScale2 = AVM.get_real(self.rev1id, 'modelScale')
         self.assertEqual(err, 0)
         self.assertEqual(modelScale1, modelScale2)

         err, gridUnits1 = AVM.get_str(self.rev0id, 'gridUnits', 128)
         self.assertEqual(err, 0)
         err, gridUnits2 = AVM.get_str(self.rev1id, 'gridUnits', 128)
         self.assertEqual(err, 0)
         self.assertEqual(gridUnits1, gridUnits2)

         err, referenceLength1 = AVM.get_real(self.rev0id, 'referenceLength')
         self.assertEqual(err, 0)
         err, referenceLength2 = AVM.get_real(self.rev1id, 'referenceLength')
         self.assertEqual(err, 0)
         self.assertEqual(referenceLength1, referenceLength2)

         err, referencePointDescription1 = AVM.get_str(self.rev0id, 'referencePointDescription', 128)
         self.assertEqual(err, 0)
         err, referencePointDescription2 = AVM.get_str(self.rev1id, 'referencePointDescription', 128)
         self.assertEqual(err, 0)
         self.assertEqual(referencePointDescription1, referencePointDescription2)

         #mesh description size is fixed in rev1
         #err, descriptionSize1 = AVM.get_int(self.rev0id, 'descriptionSize')
         #self.assertEqual(err, 0)
         #err, descriptionSize2 = AVM.get_int(self.rev1id, 'descriptionSize')
         #self.assertEqual(err, 0)
         #self.assertEqual(descriptionSize1, descriptionSize2)

         err, description1 = AVM.get_str(self.rev0id, 'description', 128)
         self.assertEqual(err, 0)
         err, description2 = AVM.get_str(self.rev1id, 'meshDescription', 128)
         self.assertEqual(err, 0)
         self.assertEqual(description1, description2)

         err, meshDescription1 = AVM.get_str(self.rev0id, 'description', 128)
         self.assertEqual(err, 0)
         err, meshDescription2 = AVM.get_str(self.rev1id, 'meshDescription', 128)
         self.assertEqual(err, 0)
         self.assertEqual(meshDescription1, meshDescription2)

         err, nNodes1 = AVM.get_int(self.rev0id, 'nNodes')
         self.assertEqual(err, 0)
         err, nNodes2 = AVM.get_int(self.rev1id, 'nNodes')
         self.assertEqual(err, 0)
         self.assertEqual(nNodes1, nNodes2)

         err, nFaces1 = AVM.get_int(self.rev0id, 'nFaces')
         self.assertEqual(err, 0)
         err, nFaces2 = AVM.get_int(self.rev1id, 'nFaces')
         self.assertEqual(err, 0)
         self.assertEqual(nFaces1, nFaces2)

         err, nMaxNodesPerFace1 = AVM.get_int(self.rev0id, 'nMaxNodesPerFace')
         self.assertEqual(err, 0)
         err, nMaxNodesPerFace2 = AVM.get_int(self.rev1id, 'nMaxNodesPerFace')
         self.assertEqual(err, 0)
         self.assertEqual(nMaxNodesPerFace1, nMaxNodesPerFace2)

         err, nTriFaces1 = AVM.get_int(self.rev0id, 'nTriFaces')
         self.assertEqual(err, 0)
         err, nTriFaces2 = AVM.get_int(self.rev1id, 'nTriFaces')
         self.assertEqual(err, 0)
         self.assertEqual(nTriFaces1, nTriFaces2)

         err, nQuadFaces1 = AVM.get_int(self.rev0id, 'nQuadFaces')
         self.assertEqual(err, 0)
         err, nQuadFaces2 = AVM.get_int(self.rev1id, 'nQuadFaces')
         self.assertEqual(err, 0)
         self.assertEqual(nQuadFaces1, nQuadFaces2)

         err, nBndTriFaces1 = AVM.get_int(self.rev0id, 'nBndTriFaces')
         self.assertEqual(err, 0)
         err, nBndTriFaces2 = AVM.get_int(self.rev1id, 'nBndTriFaces')
         self.assertEqual(err, 0)
         self.assertEqual(nBndTriFaces1, nBndTriFaces2)

         err, nBndQuadFaces1 = AVM.get_int(self.rev0id, 'nBndQuadFaces')
         self.assertEqual(err, 0)
         err, nBndQuadFaces2 = AVM.get_int(self.rev1id, 'nBndQuadFaces')
         self.assertEqual(err, 0)
         self.assertEqual(nBndTriFaces1, nBndTriFaces2)

         err, nCells1 = AVM.get_int(self.rev0id, 'nCells')
         self.assertEqual(err, 0)
         err, nCells2 = AVM.get_int(self.rev1id, 'nCells')
         self.assertEqual(err, 0)
         self.assertEqual(nCells1, nCells2)

         err, nMaxNodesPerCell1 = AVM.get_int(self.rev0id, 'nMaxNodesPerCell')
         self.assertEqual(err, 0)
         err, nMaxNodesPerCell2 = AVM.get_int(self.rev1id, 'nMaxNodesPerCell')
         self.assertEqual(err, 0)
         self.assertEqual(nMaxNodesPerCell1, nMaxNodesPerCell2)

         err, nMaxFacesPerCell1 = AVM.get_int(self.rev0id, 'nMaxFacesPerCell')
         self.assertEqual(err, 0)
         err, nMaxFacesPerCell2 = AVM.get_int(self.rev1id, 'nMaxFacesPerCell')
         self.assertEqual(err, 0)
         self.assertEqual(nMaxFacesPerCell1, nMaxFacesPerCell2)

         err, nPatches1 = AVM.get_int(self.rev0id, 'nPatches')
         self.assertEqual(err, 0)
         err, nPatches2 = AVM.get_int(self.rev1id, 'nPatches')
         self.assertEqual(err, 0)
         self.assertEqual(nPatches1, nPatches2)

         for patchNum in range(1,nPatches1+1):
            #read rev0 patch headers
            err = AVM.select(self.rev0id, 'patch', patchNum)
            self.assertEqual(err, 0)
            err = AVM.select(self.rev1id, 'patch', patchNum)
            self.assertEqual(err, 0)

            err, patchLabel1 = AVM.get_str(self.rev0id, 'patchLabel', 32)
            self.assertEqual(err, 0)
            err, patchLabel2 = AVM.get_str(self.rev1id, 'patchLabel', 32)
            self.assertEqual(err, 0)
            self.assertEqual(patchLabel1, patchLabel2)

            err, patchType1 = AVM.get_str(self.rev0id, 'patchType', 32)
            self.assertEqual(err, 0)
            err, patchType2 = AVM.get_str(self.rev1id, 'patchType', 32)
            self.assertEqual(err, 0)
            self.assertEqual(patchType1, patchType2)

            err, patchId1 = AVM.get_int(self.rev0id, 'patchId')
            self.assertEqual(err, 0)
            err, patchId2 = AVM.get_int(self.rev1id, 'patchId')
            self.assertEqual(err, 0)
            self.assertEqual(patchId1, patchId2)



      for meshNum in range(1,meshCount1+1):
         #read rev0 mesh data
         err = AVM.select(self.rev0id, 'mesh', meshNum)
         self.assertEqual(err, 0)
         err = AVM.select(self.rev1id, 'mesh', meshNum)
         self.assertEqual(err, 0)

         if precision1 == 1:
            xyz1 = AVM.r4Array(3*nNodes1)
            xyz2 = AVM.r4Array(3*nNodes1)
         elif precision1 == 2:
            xyz1 = AVM.r8Array(3*nNodes1)
            xyz2 = AVM.r8Array(3*nNodes1)
         triFaces1 = AVM.intArray(5*nTriFaces1)
         triFaces2 = AVM.intArray(5*nTriFaces1)
         quadFaces1 = AVM.intArray(6*nQuadFaces1)
         quadFaces2 = AVM.intArray(6*nQuadFaces1)

         if precision1 == 1:
            err = AVM.unstruc_read_nodes_r4(self.rev0id, xyz1, 3*nNodes1)
            self.assertEqual(err, 0)
            err = AVM.unstruc_read_nodes_r4(self.rev1id, xyz2, 3*nNodes1)
            self.assertEqual(err, 0)
         elif precision1 == 2:
            err = AVM.unstruc_read_nodes_r8(self.rev0id, xyz1, 3*nNodes1)
            self.assertEqual(err, 0)
            err = AVM.unstruc_read_nodes_r8(self.rev1id, xyz2, 3*nNodes1)
            self.assertEqual(err, 0)

         for i in range(0,3*nNodes1):
            self.assertEqual(xyz1[i],xyz2[i])

         err = AVM.unstruc_read_faces(self.rev0id, triFaces1,  5*nTriFaces1,
                                                   quadFaces1, 6*nQuadFaces1,
                                                   None, 0)
         self.assertEqual(err, 0)
         err = AVM.unstruc_read_faces(self.rev1id, triFaces2,  5*nTriFaces1,
                                                   quadFaces2, 6*nQuadFaces1,
                                                   None, 0)
         self.assertEqual(err, 0)

         for i in range(0,3*nNodes1):
            self.assertEqual(triFaces1[i],triFaces2[i])

         for i in range(0,3*nNodes1):
            self.assertEqual(quadFaces1[i],quadFaces2[i])

      AVM.close(self.rev0id)
      AVM.close(self.rev1id)

if __name__ == '__main__':
   if len(sys.argv) < 3:
      print("USAGE: python compare0to1.py <rev0_mesh> <rev1_mesh>")
      sys.exit(1)

   unittest.main(argv=sys.argv[:1])
