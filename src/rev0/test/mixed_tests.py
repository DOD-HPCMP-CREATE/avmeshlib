import sys
sys.path.append('../..')
import avmeshlib as AVM
import unittest

class TestAVM(unittest.TestCase):

   def setUp(self):
      self.avmid = -1 # avmesh file id 
      err, self.avmid = AVM.read_headers('mixed.avm')
      self.assertEqual(0, err)

   def tearDown(self):
      self.assertEqual(0, AVM.close(self.avmid))

   def test_fileHeader(self):
      err = AVM.select(self.avmid, 'header', 0)
      self.assertEqual(0, err)
      
      err, meshCount = AVM.get_int(self.avmid, 'meshCount')
      self.assertEqual(0, err)
      self.assertEqual(4, meshCount)

      err, contactName = AVM.get_str(self.avmid, 'contactName', 64)
      self.assertEqual(0, err)
      self.assertEqual('Josh Calahan', contactName)

   def test_bfstruc(self):

      meshNumber = 1
      err = AVM.select(self.avmid, 'mesh', meshNumber)
      self.assertEqual(0, err)
      err = AVM.seek_to_mesh(self.avmid, meshNumber)
      self.assertEqual(0, err)

      err, meshType = AVM.get_str(self.avmid, 'meshType', 16)
      self.assertEqual(0, err)
      self.assertEqual('bfstruc', meshType)

      err, jmax = AVM.get_int(self.avmid, 'jmax')
      self.assertEqual(0, err)
      err, kmax = AVM.get_int(self.avmid, 'kmax')
      self.assertEqual(0, err)
      err, lmax = AVM.get_int(self.avmid, 'lmax')
      self.assertEqual(0, err)

      err, iblank = AVM.get_int(self.avmid, 'iblank')
      self.assertEqual(0, err)
      self.assertEqual(1, iblank)

      size = jmax*kmax*lmax;
      x = AVM.r4Array(size)
      y = AVM.r4Array(size)
      z = AVM.r4Array(size)
      iblanks = AVM.intArray(size)

      err = AVM.bfstruc_read_xyz_r4(self.avmid, x,y,z, size)
      err = AVM.bfstruc_read_iblank(self.avmid, iblanks, size)

   def test_strand(self):

      meshNumber = 2
      err = AVM.select(self.avmid, 'mesh', meshNumber)
      self.assertEqual(0, err)
      err = AVM.seek_to_mesh(self.avmid, meshNumber)
      self.assertEqual(0, err)

      err, meshType = AVM.get_str(self.avmid, 'meshType', 16)
      self.assertEqual(0, err)
      self.assertEqual('strand', meshType)

      err, nSurfNodes = AVM.get_int(self.avmid, 'nSurfNodes')
      self.assertEqual(0, err)
      err, nTriFaces = AVM.get_int(self.avmid, 'nTriFaces')
      self.assertEqual(0, err)
      err, nQuadFaces = AVM.get_int(self.avmid, 'nQuadFaces')
      self.assertEqual(0, err)
      err, nBndEdges = AVM.get_int(self.avmid, 'nBndEdges')
      self.assertEqual(0, err)
      err, nPtsPerStrand = AVM.get_int(self.avmid, 'nPtsPerStrand')
      self.assertEqual(0, err)

      triConn = AVM.intArray(3*nTriFaces)
      quadConn = AVM.intArray(4*nQuadFaces)
      bndEdges = AVM.intArray(2*nBndEdges)
      nodeClip = AVM.intArray(nSurfNodes)
      faceClip = AVM.intArray(nTriFaces+nQuadFaces)
      faceTag = AVM.intArray(nTriFaces+nQuadFaces)
      edgeTag = AVM.intArray(nBndEdges)
      xSurf = AVM.r4Array(3*nSurfNodes)
      pointingVec = AVM.r4Array(3*nSurfNodes)
      xStrand = AVM.r4Array(nPtsPerStrand)

      err = AVM.strand_read_r4(self.avmid,
         triConn, 3*nTriFaces,
         quadConn, 4*nQuadFaces,
         bndEdges, 2*nBndEdges,
         nodeClip, nSurfNodes,
         faceClip, nTriFaces+nQuadFaces,
         faceTag, nTriFaces+nQuadFaces,
         edgeTag, nBndEdges,
         xSurf, 3*nSurfNodes,
         pointingVec, 3*nSurfNodes,
         xStrand, nPtsPerStrand)
      self.assertEqual(0, err)

      for i in xrange(nTriFaces):
         for j in xrange(3):
            self.assertEqual(i+1+j+1, triConn[(i*3)+j])

      for i in xrange(nPtsPerStrand):
         self.assertEqual(i, xStrand[i])

   def test_unstruc(self):

      meshNumber = 3
      err = AVM.select(self.avmid, 'mesh', meshNumber)
      self.assertEqual(0, err)
      err = AVM.seek_to_mesh(self.avmid, meshNumber)
      self.assertEqual(0, err)

      err, meshType = AVM.get_str(self.avmid, 'meshType', 16)
      self.assertEqual(0, err)
      self.assertEqual('unstruc', meshType)

      err, nNodes = AVM.get_int(self.avmid, 'nNodes')
      self.assertEqual(0, err)
      err, nTriFaces = AVM.get_int(self.avmid, 'nTriFaces')
      self.assertEqual(0, err)
      err, nQuadFaces = AVM.get_int(self.avmid, 'nQuadFaces')
      self.assertEqual(0, err)

      xyz = AVM.r4Array(3*nNodes)
      triFaces = AVM.intArray(5*nTriFaces)
      quadFaces = AVM.intArray(6*nQuadFaces)

      err = AVM.unstruc_read_nodes_r4(self.avmid, xyz, 3*nNodes)
      self.assertEqual(0, err)

      err = AVM.unstruc_read_faces(self.avmid, triFaces,  5*nTriFaces,
                                               quadFaces, 6*nQuadFaces)
      self.assertEqual(0, err)

   def test_cart(self):

      meshNumber = 4
      err = AVM.select(self.avmid, 'mesh', meshNumber)
      self.assertEqual(0, err)
      err = AVM.seek_to_mesh(self.avmid, meshNumber)
      self.assertEqual(0, err)

      err, meshType = AVM.get_str(self.avmid, 'meshType', 16)
      self.assertEqual(0, err)
      self.assertEqual('cart', meshType)

      err, nBlocks = AVM.get_int(self.avmid, 'nBlocks')
      self.assertEqual(0, err)
      
      for blockid in xrange(1, nBlocks+1):
         err = AVM.select(self.avmid, 'block', blockid)
         self.assertEqual(0, err)
         err, iblFlag = AVM.get_int(self.avmid, 'iblFlag')
         self.assertEqual(0, err)
         if iblFlag == 2:
            iblDim = AVM.intArray(3)
            err = AVM.get_int_array(self.avmid, 'iblDim', iblDim, 3)
            self.assertEqual(0, err)
            size = iblDim[0] * iblDim[1] * iblDim[2]
            iblanks = AVM.intArray(size)
            err = AVM.cart_read_iblank(self.avmid, iblanks, size) 

if __name__ == '__main__':
   unittest.main()
