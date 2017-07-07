import sys
import os
sys.path.append('../..')
import avmeshlib as AVM
import unittest

class TestAVM(unittest.TestCase):
    def setUp(self):
        self.avmid = -1 # avmesh file id
        self.filename = "higherOrder.avm"
        err, self.avmid = AVM.new_file(self.filename, 2)

    def test_p1(self):
        self.writeMesh(1)
        self.readMesh(1)

    def test_p2(self):
        self.writeMesh(2)
        self.readMesh(2)

    def test_p3(self):
        self.writeMesh(3)
        self.readMesh(3)

    def test_p4(self):
        self.writeMesh(4)
        self.readMesh(4)

    def tearDown(self):
        os.remove(self.filename)

    def writeMesh(self, p):
        #file header
        self.assertEqual(AVM.select(self.avmid, 'header', 0), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'meshCount', 1), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'contactInfo', 'David McDaniel', 128), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'precision', 2), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'dimensions', 3), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'description', 'p='+str(p)+'mesh', 128) , 0)

        #generic mesh header
        refLength = AVM.r8Array(3)
        refLength[0] = 1.0
        refLength[1] = 1.0
        refLength[2] = 1.0
        refPt = AVM.r8Array(3)
        refPt[0] = 0.0
        refPt[1] = 0.0
        refPt[2] = 0.0
        self.assertEqual(AVM.select(self.avmid, 'mesh', 1), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'meshName', 'mesh1', 128), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'meshType', 'unstruc', 128), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'meshGenerator', 'higherOrder.py', 128), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'coordinateSystem', 'xByUzL', 128), 0)
        self.assertEqual(AVM.set_real(self.avmid, 'modelScale', 1.0), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'gridUnits', 'in', 128), 0)
        self.assertEqual(AVM.set_real_array(self.avmid, 'referenceLength', refLength, 3), 0)
        self.assertEqual(AVM.set_real(self.avmid, 'referenceArea', 1.0), 0)
        self.assertEqual(AVM.set_real_array(self.avmid, 'referencePoint', refPt, 3), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'referencePointDescription', 'origin', 128), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'meshDescription', 'p='+str(p)+'mesh', 128), 0)

        #unstruc mesh header
        nNodes = 20
        nFaces = 12
        nCells = 8
        nMaxNodesPerFace = AVM.nodes_per_quad(self.avmid, p)
        nMaxNodesPerCell = AVM.nodes_per_hex(self.avmid, p)
        nMaxFacesPerCell = 6
        nHexCells = 2
        nTetCells = 2
        nPriCells = 2
        nPyrCells = 2
        nBndTriFaces = 5
        nTriFaces = 5
        nBndQuadFaces = 5
        nQuadFaces = 5
        self.assertEqual(AVM.set_int(self.avmid, 'nNodes', nNodes), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nFaces', nFaces), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nCells', nCells), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nMaxNodesPerFace', nMaxNodesPerFace), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nMaxNodesPerCell', nMaxNodesPerCell), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nMaxFacesPerCell', nMaxFacesPerCell), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'facePolyOrder', p), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'cellPolyOrder', p), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nPatches', 1), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nHexCells', nHexCells), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nTetCells', nTetCells), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nPriCells', nPriCells), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nPyrCells', nPyrCells), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nBndTriFaces', nBndTriFaces), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nTriFaces', nTriFaces), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nBndQuadFaces', nBndQuadFaces), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nQuadFaces', nQuadFaces), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nEdges', 0), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nNodesOnGeometry', 0), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nEdgesOnGeometry', 0), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nFacesOnGeometry', 0), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'geomRegionId', 0), 0)

        #patch
        self.assertEqual(AVM.select(self.avmid, 'patch', 1), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'patchLabel', 'patch1', 16), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'patchType', 'farfield', 16), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'patchId', -1), 0)

        #write headers
        self.assertEqual(AVM.write_headers(self.avmid),0)

        #heavy data
        nodesPerTri = AVM.nodes_per_tri(self.avmid, p)
        nodesPerQuad = AVM.nodes_per_quad(self.avmid, p)
        nodesPerHex = AVM.nodes_per_hex(self.avmid, p)
        nodesPerTet = AVM.nodes_per_tet(self.avmid, p)
        nodesPerPri = AVM.nodes_per_pri(self.avmid, p)
        nodesPerPyr = AVM.nodes_per_pyr(self.avmid, p)

        xyz = AVM.r8Array(20*3)
        triFaces = AVM.intArray(nTriFaces*(nodesPerTri+1))
        quadFaces = AVM.intArray(nQuadFaces*(nodesPerQuad+1))
        hexCells = AVM.intArray(nHexCells*nodesPerHex)
        tetCells = AVM.intArray(nTetCells*nodesPerTet)
        priCells = AVM.intArray(nPriCells*nodesPerPri)
        pyrCells = AVM.intArray(nPyrCells*nodesPerPyr)

        for i in range(0,20*3):
            xyz[i] = i

        self.assertEqual(AVM.unstruc_write_nodes_r8(self.avmid, xyz, 20*3),0)

        for i in range(0,nTriFaces):
            base = i*(nodesPerTri+1)
            for j in range(0,nodesPerTri):
                triFaces[base+j] = p*(i+j)
            triFaces[base+nodesPerTri] = -1

        for i in range(0,nQuadFaces):
            base = i*(nodesPerQuad+1)
            for j in range(0,nodesPerQuad):
                quadFaces[base+j] = p*(i+j)
            quadFaces[base+nodesPerQuad] = -1

        self.assertEqual(AVM.unstruc_write_faces(self.avmid,
                                                 triFaces, nTriFaces*(nodesPerTri+1), \
                                                 quadFaces, nQuadFaces*(nodesPerQuad+1)),0)

        for i in range(0,nHexCells):
            for j in range(0,nodesPerHex):
                hexCells[i*nodesPerHex+j] = p*(i+j)

        for i in range(0,nTetCells):
            for j in range(0,nodesPerTet):
                tetCells[i*nodesPerTet+j] = p*(i+j)

        for i in range(0,nPriCells):
            for j in range(0,nodesPerPri):
                priCells[i*nodesPerPri+j] = p*(i+j)

        for i in range(0,nPyrCells):
            for j in range(0,nodesPerPyr):
                pyrCells[i*nodesPerPyr+j] = p*(i+j)

        self.assertEqual(AVM.unstruc_write_cells(self.avmid,
                                                 hexCells, nHexCells*nodesPerHex, \
                                                 tetCells, nTetCells*nodesPerTet, \
                                                 priCells, nPriCells*nodesPerPri, \
                                                 pyrCells, nPyrCells*nodesPerPyr),0)
        self.assertEqual(AVM.close(self.avmid),0)

    def readMesh(self, p):
        #file header
        err, self.avmid = AVM.read_headers(self.filename)
        self.assertEqual(0,err)
        self.assertEqual(AVM.select(self.avmid, 'header', 0), 0)
        err, meshCount = AVM.get_int(self.avmid, 'meshCount')
        self.assertEqual(0,err)
        self.assertEqual(1,meshCount)

        #unstruc mesh header
        self.assertEqual(AVM.select(self.avmid, 'mesh', 1), 0)
        nNodes = 20
        nFaces = 12
        nCells = 8
        nHexCells = 2
        nTetCells = 2
        nPriCells = 2
        nPyrCells = 2
        nBndTriFaces = 5
        nTriFaces = 5
        nBndQuadFaces = 5
        nQuadFaces = 5
        err, facePolyOrder = AVM.get_int(self.avmid, 'facePolyOrder')
        self.assertEqual(0,err)
        self.assertEqual(p,facePolyOrder)
        err, cellPolyOrder = AVM.get_int(self.avmid, 'cellPolyOrder')
        self.assertEqual(0,err)
        self.assertEqual(p,cellPolyOrder)

        #heavy data
        nodesPerTri = AVM.nodes_per_tri(self.avmid, p)
        nodesPerQuad = AVM.nodes_per_quad(self.avmid, p)
        nodesPerHex = AVM.nodes_per_hex(self.avmid, p)
        nodesPerTet = AVM.nodes_per_tet(self.avmid, p)
        nodesPerPri = AVM.nodes_per_pri(self.avmid, p)
        nodesPerPyr = AVM.nodes_per_pyr(self.avmid, p)

        xyz = AVM.r8Array(20*3)
        triFaces = AVM.intArray(nTriFaces*(nodesPerTri+1))
        quadFaces = AVM.intArray(nQuadFaces*(nodesPerQuad+1))
        hexCells = AVM.intArray(nHexCells*nodesPerHex)
        tetCells = AVM.intArray(nTetCells*nodesPerTet)
        priCells = AVM.intArray(nPriCells*nodesPerPri)
        pyrCells = AVM.intArray(nPyrCells*nodesPerPyr)

        self.assertEqual(AVM.unstruc_read_nodes_r8(self.avmid, xyz, 20*3),0)

        for i in range(0,20*3):
            self.assertEqual(xyz[i], i)

        self.assertEqual(AVM.unstruc_read_faces(self.avmid,
                                                 triFaces, nTriFaces*(nodesPerTri+1), \
                                                 quadFaces, nQuadFaces*(nodesPerQuad+1)),0)

        for i in range(0,nTriFaces):
            base = i*(nodesPerTri+1)
            for j in range(0,nodesPerTri):
                self.assertEqual(triFaces[base+j], p*(i+j))
            self.assertEqual(triFaces[base+nodesPerTri], -1)

        for i in range(0,nQuadFaces):
            base = i*(nodesPerQuad+1)
            for j in range(0,nodesPerQuad):
                self.assertEqual(quadFaces[base+j], p*(i+j))
            self.assertEqual(quadFaces[base+nodesPerQuad], -1)

        self.assertEqual(AVM.unstruc_read_cells(self.avmid,
                                                 hexCells, nHexCells*nodesPerHex, \
                                                 tetCells, nTetCells*nodesPerTet, \
                                                 priCells, nPriCells*nodesPerPri, \
                                                 pyrCells, nPyrCells*nodesPerPyr),0)

        for i in range(0,nHexCells):
            for j in range(0,nodesPerHex):
                self.assertEqual(hexCells[i*nodesPerHex+j], p*(i+j))

        for i in range(0,nTetCells):
            for j in range(0,nodesPerTet):
                self.assertEqual(tetCells[i*nodesPerTet+j], p*(i+j))

        for i in range(0,nPriCells):
            for j in range(0,nodesPerPri):
                self.assertEqual(priCells[i*nodesPerPri+j], p*(i+j))

        for i in range(0,nPyrCells):
            for j in range(0,nodesPerPyr):
                self.assertEqual(pyrCells[i*nodesPerPyr+j], p*(i+j))

        self.assertEqual(AVM.close(self.avmid),0)

if __name__ == '__main__':
   unittest.main()

