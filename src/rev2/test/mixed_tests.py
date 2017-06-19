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
      self.assertEqual(2, meshCount)

      err, contactInfo = AVM.get_str(self.avmid, 'contactInfo', 128)
      self.assertEqual(0, err)
      self.assertEqual('Josh Calahan: joshua.calahan@arnold.af.mil', contactInfo)

      err, formatRevision = AVM.get_int(self.avmid, 'formatRevision')
      self.assertEqual(0, err)
      self.assertEqual(2, formatRevision)

   def test_strand(self):

      meshNumber = 1
      err = AVM.select(self.avmid, 'mesh', meshNumber)
      self.assertEqual(0, err)
      err = AVM.seek_to_mesh(self.avmid, meshNumber)
      self.assertEqual(0, err)

      err, meshType = AVM.get_str(self.avmid, 'meshType', 16)
      self.assertEqual(0, err)
      self.assertEqual('strand', meshType)

      err, nNodes = AVM.get_int(self.avmid, 'nNodes')
      self.assertEqual(0, err)
      err, nStrands = AVM.get_int(self.avmid, 'nStrands')
      self.assertEqual(8, nStrands)
      self.assertEqual(0, err)
      err, nTriFaces = AVM.get_int(self.avmid, 'nTriFaces')
      self.assertEqual(0, err)
      err, nQuadFaces = AVM.get_int(self.avmid, 'nQuadFaces')
      self.assertEqual(0, err)
      err, nPolyFaces = AVM.get_int(self.avmid, 'nPolyFaces')
      self.assertEqual(0, err)
      err, polyFacesSize = AVM.get_int(self.avmid, 'polyFacesSize')
      self.assertEqual(0, err)
      err, nBndEdges = AVM.get_int(self.avmid, 'nBndEdges')
      self.assertEqual(0, err)
      err, nPtsPerStrand = AVM.get_int(self.avmid, 'nPtsPerStrand')
      self.assertEqual(4, nPtsPerStrand)
      self.assertEqual(0, err)

      triFaces = AVM.intArray(3*nTriFaces)
      quadFaces = AVM.intArray(4*nQuadFaces)
      polyFaces = AVM.intArray(polyFacesSize)
      bndEdges = AVM.intArray(2*nBndEdges)
      nodeID = AVM.intArray(nStrands)
      nodeClip = AVM.intArray(nStrands)
      faceTag = AVM.intArray(nTriFaces+nQuadFaces+nPolyFaces)
      edgeTag = AVM.intArray(nBndEdges)
      xyz = AVM.r4Array(3*nNodes)
      pointingVec = AVM.r4Array(3*nStrands)
      xStrand = AVM.r4Array(nPtsPerStrand)

      err = AVM.strand_read_nodes_r4(self.avmid,
         xyz, 3*nNodes,
         nodeID, nStrands,
         nodeClip, nStrands,
         pointingVec, 3*nStrands,
         xStrand, nPtsPerStrand)
      self.assertEqual(0, err)

      err = AVM.strand_read_faces(self.avmid,
         triFaces, 3*nTriFaces,
         quadFaces, 4*nQuadFaces,
         polyFaces, polyFacesSize,
         faceTag, nTriFaces+nQuadFaces+nPolyFaces)
      self.assertEqual(0, err)

      err = AVM.strand_read_edges(self.avmid,
         bndEdges, 2*nBndEdges,
         edgeTag, nBndEdges)
      self.assertEqual(0, err)

      for i in xrange(nTriFaces):
         for j in xrange(3):
            self.assertEqual(i+1+j+1, triFaces[(i*3)+j])

      for i in xrange(nPtsPerStrand):
         self.assertEqual(i, xStrand[i])

   def test_unstruc(self):

      meshNumber = 2
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
      err, polyFacesSize = AVM.get_int(self.avmid, 'polyFacesSize')
      self.assertEqual(0, err)
      err, nHexCells = AVM.get_int(self.avmid, 'nHexCells')
      self.assertEqual(0, err)
      err, nTetCells = AVM.get_int(self.avmid, 'nTetCells')
      self.assertEqual(0, err)
      err, nPriCells = AVM.get_int(self.avmid, 'nPriCells')
      self.assertEqual(0, err)
      err, nPyrCells = AVM.get_int(self.avmid, 'nPyrCells')
      self.assertEqual(0, err)
      err, nEdges = AVM.get_int(self.avmid, 'nEdges')
      self.assertEqual(0, err)
      err, nNodesOnGeometry = AVM.get_int(self.avmid, 'nNodesOnGeometry')
      self.assertEqual(0, err)
      err, nEdgesOnGeometry = AVM.get_int(self.avmid, 'nEdgesOnGeometry')
      self.assertEqual(0, err)
      err, nFacesOnGeometry = AVM.get_int(self.avmid, 'nFacesOnGeometry')
      self.assertEqual(0, err)

      xyz = AVM.r4Array(3*nNodes)
      triFaces = AVM.intArray(5*nTriFaces)
      quadFaces = AVM.intArray(6*nQuadFaces)
      polyFaces = AVM.intArray(polyFacesSize)
      hexCells = AVM.intArray(8*nHexCells)
      tetCells = AVM.intArray(4*nTetCells)
      priCells = AVM.intArray(6*nPriCells)
      pyrCells = AVM.intArray(5*nPyrCells)
      edges = AVM.intArray(2*nEdges)
      nodesOnGeometry = AVM.new_AMR_Node_Data_Array(nNodesOnGeometry)
      edgesOnGeometry = AVM.intArray(3*nEdgesOnGeometry)
      facesOnGeometry = AVM.intArray(3*nFacesOnGeometry)
      hexGeomIds = AVM.intArray(nHexCells)
      tetGeomIds = AVM.intArray(nTetCells)
      priGeomIds = AVM.intArray(nPriCells)
      pyrGeomIds = AVM.intArray(nPyrCells)

      err = AVM.unstruc_read_nodes_r4(self.avmid, xyz, 3*nNodes)
      self.assertEqual(0, err)

      err = AVM.unstruc_read_faces(self.avmid, triFaces,  5*nTriFaces,
                                               quadFaces, 6*nQuadFaces,
                                               polyFaces, polyFacesSize)
      self.assertEqual(0, err)

      err = AVM.unstruc_read_cells(self.avmid, hexCells, 8*nHexCells,
                                               tetCells, 4*nTetCells,
                                               priCells, 6*nPriCells,
                                               pyrCells, 5*nPyrCells)
      self.assertEqual(0, err)

      err = AVM.unstruc_read_edges(self.avmid, edges,  2*nEdges)
      self.assertEqual(0, err)

      err = AVM.unstruc_read_amr(self.avmid, nodesOnGeometry, nNodesOnGeometry,
                                             edgesOnGeometry, 3*nEdgesOnGeometry,
                                             facesOnGeometry, 3*nFacesOnGeometry)
      self.assertEqual(0, err)

      #first_node = AVM.AMR_Node_Data_Array_getitem(nodesOnGeometry,0)
      #print first_node.u, first_node.v, first_node.nodeIndex, first_node.geomType, first_node.geomTopoId

      err = AVM.unstruc_read_amr_volumeids(self.avmid, hexGeomIds, nHexCells,
                                                       tetGeomIds, nTetCells,
                                                       priGeomIds, nPriCells,
                                                       pyrGeomIds, nPyrCells)
      self.assertEqual(0, err)

      AVM.delete_AMR_Node_Data_Array(nodesOnGeometry)

if __name__ == '__main__':
   unittest.main()
