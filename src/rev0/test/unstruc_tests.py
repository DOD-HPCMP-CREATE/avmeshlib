import sys
sys.path.append('../..')
import avmeshlib as AVM
import unittest

class TestAVM(unittest.TestCase):

   def setUp(self):
      self.avmid = -1 # avmesh file id 
      err, self.avmid = AVM.read_headers('unstruc.avm')
      self.assertEqual(0, err)

   def tearDown(self):
      self.assertEqual(0, AVM.close(self.avmid))

   def test_fileHeader(self):
      err = AVM.select(self.avmid, 'header', 0)
      self.assertEqual(0, err)

      err, i = AVM.get_int(self.avmid, 'formatRevision')
      self.assertEqual(0, err)
      self.assertEqual(0, i)

      err, s = AVM.get_str(self.avmid, 'meshRevision', 64)
      self.assertEqual(0, err)
      self.assertEqual('1.0', s)

      err, i = AVM.get_int(self.avmid, 'meshCount')
      self.assertEqual(0, err)
      self.assertEqual(1, i)

      err, s = AVM.get_str(self.avmid, 'contactName', 64)
      self.assertEqual(0, err)
      self.assertEqual('David McDaniel', s)

      err, s = AVM.get_str(self.avmid, 'contactOrg', 64)
      self.assertEqual(0, err)
      self.assertEqual('UAB', s)

      err, s = AVM.get_str(self.avmid, 'contactEmail', 64)
      self.assertEqual(0, err)
      self.assertEqual('david.mcdaniel.ctr@eglin.af.mil', s)

      err, i = AVM.get_int(self.avmid, 'precision')
      self.assertEqual(0, err)
      self.assertEqual(1, i)

      err, i = AVM.get_int(self.avmid, 'dimensions')
      self.assertEqual(0, err)
      self.assertEqual(3, i)

      err, descriptionSize = AVM.get_int(self.avmid, 'descriptionSize')
      self.assertEqual(0, err)

      err, s = AVM.get_str(self.avmid, 'description', descriptionSize)
      self.assertEqual(0, err)
      self.assertEqual('An example AVMesh file with some unstruc grids.', s)

   def test_meshHeaders(self):
      err, meshCount = AVM.get_int(self.avmid, 'meshCount')

      for meshid in range(1, meshCount+1):

         err = AVM.select(self.avmid, 'mesh', meshid)
         self.assertEqual(0, err)

         # generic mesh fields
         err, s = AVM.get_str(self.avmid, 'meshName', 64)
         self.assertEqual(0, err)
         self.assertEqual('unstruc mesh # %d' % meshid, s)

         err, s = AVM.get_str(self.avmid, 'meshType', 64)
         self.assertEqual(0, err)
         self.assertEqual('unstruc', s)

         err, s = AVM.get_str(self.avmid, 'meshGenerator', 64)
         self.assertEqual(0, err)
         self.assertEqual('unstruc_write', s)

         err, s = AVM.get_str(self.avmid, 'changedDate', 64)
         self.assertEqual(0, err)
         self.assertEqual('June 15, 2010', s)

         err, s = AVM.get_str(self.avmid, 'coordinateSystem', 64)
         self.assertEqual(0, err)
         self.assertEqual('xByUzL', s)

         err, r = AVM.get_real(self.avmid, 'modelScale')
         self.assertEqual(0, err)
         self.assertEqual(1.0, r)

         err, s = AVM.get_str(self.avmid, 'gridUnits', 64)
         self.assertEqual(0, err)
         self.assertEqual('in', s)

         err, r = AVM.get_real(self.avmid, 'reynoldsNumber')
         self.assertEqual(0, err)
         self.assertEqual(1.0, r)

         err, r = AVM.get_real(self.avmid, 'referenceLength')
         self.assertEqual(0, err)
         self.assertEqual(1.0, r)

         err, r = AVM.get_real(self.avmid, 'wallDistance')
         self.assertEqual(0, err)
         self.assertEqual(1.0, r)

         vec3 = AVM.r8Array(3)
         err = AVM.get_real_array(self.avmid, 'referencePoint', vec3, 3)
         self.assertEqual(0, err)
         self.assertEqual(0.0, vec3[0])
         self.assertEqual(0.0, vec3[1])
         self.assertEqual(0.0, vec3[2])

         err, s = AVM.get_str(self.avmid, 'referencePointDescription', 64)
         self.assertEqual(0, err)
         self.assertEqual('origin', s)

         err, r = AVM.get_real(self.avmid, 'periodicity')
         self.assertEqual(0, err)
         self.assertEqual(1.0, r)

         err, s = AVM.get_str(self.avmid, 'periodicityDescription', 64)
         self.assertEqual(0, err)
         self.assertEqual('full-span', s)

         err, i = AVM.get_int(self.avmid, 'refinementLevel')
         self.assertEqual(0, err)
         self.assertEqual(1, i)

         err, descriptionSize = AVM.get_int(self.avmid, 'descriptionSize')
         self.assertEqual(0, err)

         err, s = AVM.get_str(self.avmid, 'description', descriptionSize)
         self.assertEqual(0, err)
         self.assertEqual('A nonsensical unstruc mesh that just demonstrates the format.', s)

         # unstruc mesh fields
         err, i = AVM.get_int(self.avmid, 'fullViscousLayerCount')
         self.assertEqual(0, err)
         self.assertEqual(0, i)

         err, i = AVM.get_int(self.avmid, 'partialViscousLayerCount')
         self.assertEqual(0, err)
         self.assertEqual(0, i)

         err, i = AVM.get_int(self.avmid, 'nNodes')
         self.assertEqual(0, err)
         self.assertEqual(12, i)

         err, i = AVM.get_int(self.avmid, 'nEdges')
         self.assertEqual(0, err)
         self.assertEqual(24, i)

         err, i = AVM.get_int(self.avmid, 'nFaces')
         self.assertEqual(0, err)
         self.assertEqual(17, i)

         err, i = AVM.get_int(self.avmid, 'nMaxNodesPerFace')
         self.assertEqual(0, err)
         self.assertEqual(4, i)

         err, i = AVM.get_int(self.avmid, 'nTriFaces')
         self.assertEqual(0, err)
         self.assertEqual(9, i)

         err, i = AVM.get_int(self.avmid, 'nQuadFaces')
         self.assertEqual(0, err)
         self.assertEqual(8, i)

         err, i = AVM.get_int(self.avmid, 'nBndTriFaces')
         self.assertEqual(0, err)
         self.assertEqual(8, i)

         err, i = AVM.get_int(self.avmid, 'nBndQuadFaces')
         self.assertEqual(0, err)
         self.assertEqual(6, i)

         err, i = AVM.get_int(self.avmid, 'nCells')
         self.assertEqual(0, err)
         self.assertEqual(4, i)

         err, i = AVM.get_int(self.avmid, 'nMaxNodesPerCell')
         self.assertEqual(0, err)
         self.assertEqual(8, i)

         err, i = AVM.get_int(self.avmid, 'nMaxFacesPerCell')
         self.assertEqual(0, err)
         self.assertEqual(6, i)

         err, i = AVM.get_int(self.avmid, 'nHexCells')
         self.assertEqual(0, err)
         self.assertEqual(1, i)

         err, i = AVM.get_int(self.avmid, 'nTetCells')
         self.assertEqual(0, err)
         self.assertEqual(1, i)

         err, i = AVM.get_int(self.avmid, 'nPriCells')
         self.assertEqual(0, err)
         self.assertEqual(1, i)

         err, i = AVM.get_int(self.avmid, 'nPyrCells')
         self.assertEqual(0, err)
         self.assertEqual(1, i)

         err, nPatches = AVM.get_int(self.avmid, 'nPatches')
         self.assertEqual(0, err)
         self.assertEqual(2, nPatches)

         err, i = AVM.get_int(self.avmid, 'nPatchNodes')
         self.assertEqual(0, err)
         self.assertEqual(12, i)

         err, i = AVM.get_int(self.avmid, 'nPatchFaces')
         self.assertEqual(0, err)
         self.assertEqual(14, i)

         for patchid in range(1, nPatches+1):
            err = AVM.select(self.avmid, 'patch', patchid)
            self.assertEqual(0, err)

            err, s = AVM.get_str(self.avmid, 'patchLabel', 64)
            self.assertEqual(0, err)
            self.assertEqual('patch %d label' % patchid, s)

            err, s = AVM.get_str(self.avmid, 'patchType', 64)
            self.assertEqual(0, err)
            self.assertEqual('patch %d bctype' % patchid, s)

            err, i = AVM.get_int(self.avmid, 'patchId')
            self.assertEqual(0, err)
            self.assertEqual(-patchid, i)

   def test_dataReads(self):
      err, meshCount = AVM.get_int(self.avmid, 'meshCount')

      for meshid in range(1, meshCount+1):

         err = AVM.select(self.avmid, 'mesh', meshid)
         self.assertEqual(0, err)

         err, nNodes = AVM.get_int(self.avmid, 'nNodes')
         err, nTriFaces = AVM.get_int(self.avmid, 'nTriFaces')
         err, nQuadFaces = AVM.get_int(self.avmid, 'nQuadFaces')
         err, nBndTriFaces = AVM.get_int(self.avmid, 'nBndTriFaces')
         err, nBndQuadFaces = AVM.get_int(self.avmid, 'nBndQuadFaces')

         xyz = AVM.r4Array(3*nNodes)
         triFaces = AVM.intArray(5*nTriFaces)
         quadFaces = AVM.intArray(6*nQuadFaces)

         err = AVM.unstruc_read_nodes_r4(self.avmid, xyz, 3*nNodes)
         self.assertEqual(0, err)
         err = AVM.unstruc_read_faces(self.avmid, triFaces, 5*nTriFaces,
                                                  quadFaces, 6*nQuadFaces)
         self.assertEqual(0, err)

         # nodes
         # check the first couple and last node 
         # node 1
         p = (0*3)
         self.assertAlmostEqual(0.0,xyz[p+0])
         self.assertAlmostEqual(0.0,xyz[p+1])
         self.assertAlmostEqual(0.0,xyz[p+2])
         # node 2
         p = (1*3)
         self.assertAlmostEqual(12.0,xyz[p+0])
         self.assertAlmostEqual(0.0,xyz[p+1])
         self.assertAlmostEqual(0.0,xyz[p+2])
         # node ...
         # node 12
         p = (11*3)
         self.assertAlmostEqual(-6.0,xyz[p+0])
         self.assertAlmostEqual(15.0,xyz[p+1])
         self.assertAlmostEqual(6.0,xyz[p+2])

         # check the first and last triFace 
         BUFF_LEN=5; FIRST_POINT=0; LAST_POINT=8
         p = FIRST_POINT * BUFF_LEN
         self.assertEqual(5, triFaces[p+0])
         self.assertEqual(12, triFaces[p+1])
         self.assertEqual(6, triFaces[p+2])
         self.assertEqual(3, triFaces[p+3])
         self.assertEqual(-1, triFaces[p+4])
         p = LAST_POINT * BUFF_LEN
         self.assertEqual(5, triFaces[p+0])
         self.assertEqual(6, triFaces[p+1])
         self.assertEqual(11, triFaces[p+2])
         self.assertEqual(3, triFaces[p+3])
         self.assertEqual(2, triFaces[p+4])

         # check the first and last quadFace 
         BUFF_LEN=6; FIRST_POINT=0; LAST_POINT=7
         p = FIRST_POINT * BUFF_LEN
         self.assertEqual(1, quadFaces[p+0])
         self.assertEqual(4, quadFaces[p+1])
         self.assertEqual(5, quadFaces[p+2])
         self.assertEqual(6, quadFaces[p+3])
         self.assertEqual(1, quadFaces[p+4])
         self.assertEqual(-1, quadFaces[p+5])
         p = LAST_POINT * BUFF_LEN
         self.assertEqual(5, quadFaces[p+0])
         self.assertEqual(6, quadFaces[p+1])
         self.assertEqual(7, quadFaces[p+2])
         self.assertEqual(8, quadFaces[p+3])
         self.assertEqual(1, quadFaces[p+4])
         self.assertEqual(2, quadFaces[p+5])

if __name__ == '__main__':
   unittest.main()
