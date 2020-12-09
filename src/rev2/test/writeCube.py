import sys
import os
#sys.path.append('../..')
sys.path.append('/home/bpittman/src/avmeshlib.git/INSTALL/python')
import avmeshlib as AVM
import unittest

class TestAVM(unittest.TestCase):
    def setUp(self):
        self.avmid = -1 # avmesh file id
        self.filename = "largeWrite.avm"
        err, self.avmid = AVM.new_file(self.filename, 2)

    def test_largeCube(self):
        #go up to 648x648x648 to overflow int
        self.writeCube(894)
        #self.readMesh()

    def tearDown(self):
        pass
        #os.remove(self.filename)

    def writeCube(self,size):
        #file header
        self.assertEqual(AVM.select(self.avmid, 'header', 0), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'meshCount', 1), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'contactInfo', 'David McDaniel', 128), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'precision', 2), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'dimensions', 3), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'description', 'cube', 128) , 0)

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
        self.assertEqual(AVM.set_str(self.avmid, 'meshGenerator', 'largeWrite.py', 128), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'coordinateSystem', 'xByUzL', 128), 0)
        self.assertEqual(AVM.set_real(self.avmid, 'modelScale', 1.0), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'gridUnits', 'in', 128), 0)
        self.assertEqual(AVM.set_real_array(self.avmid, 'referenceLength', refLength, 3), 0)
        self.assertEqual(AVM.set_real(self.avmid, 'referenceArea', 1.0), 0)
        self.assertEqual(AVM.set_real_array(self.avmid, 'referencePoint', refPt, 3), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'referencePointDescription', 'origin', 128), 0)
        self.assertEqual(AVM.set_str(self.avmid, 'meshDescription', 'large mesh', 128), 0)

        #unstruc mesh header
        nNodes = size**3
        nCells = (size-1)**3
        nHexCells = nCells
        nBndQuadFaces = ((size-1)**2) * 6
        nFaces = (nBndQuadFaces + (nHexCells * 6)) / 2
        #print nFaces, nCells
        nMaxNodesPerFace = 4
        nMaxNodesPerCell = 8
        nMaxFacesPerCell = 6
        nTetCells = 0
        nPriCells = 0
        nPyrCells = 0
        nBndTriFaces = 0
        nTriFaces = 0
        nQuadFaces = ((size-1)**2) * 6
        self.assertEqual(AVM.set_int(self.avmid, 'nNodes', nNodes), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nFaces', nFaces), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nCells', nCells), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nMaxNodesPerFace', nMaxNodesPerFace), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nMaxNodesPerCell', nMaxNodesPerCell), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'nMaxFacesPerCell', nMaxFacesPerCell), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'facePolyOrder', 1), 0)
        self.assertEqual(AVM.set_int(self.avmid, 'cellPolyOrder', 1), 0)
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
        nodesPerQuad = AVM.nodes_per_quad(self.avmid, 1)
        nodesPerHex = AVM.nodes_per_hex(self.avmid, 1)

        xyz = AVM.r8Array(nNodes*3)
        triFaces = None
        quadFaces = AVM.intArray(nQuadFaces*5)
        hexCells = AVM.intArray(nHexCells*8)
        tetCells = None
        priCells = None
        pyrCells = None

        print("computing nodes...")
        for z in range(size):
            for y in range(size):
                for x in range(size):
                    i = (x + (y*size) + (z*size**2)) * 3
                    xyz[i]   = x
                    xyz[i+1] = y
                    xyz[i+2] = z

        print("writing nodes...")
        self.assertEqual(AVM.unstruc_write_nodes_r8(self.avmid, xyz, nNodes*3),0)

        print("computing faces...")
        for y in range(size-1):
            for x in range(size-1):
                #add the z=0 faces
                i = (x + y*(size-1)) * 5
                quadFaces[i]   = x     + (y     * size) + 1
                quadFaces[i+1] = (x+1) + (y     * size) + 1
                quadFaces[i+2] = (x+1) + ((y+1) * size) + 1
                quadFaces[i+3] = x     + ((y+1) * size) + 1
                quadFaces[i+4] = -1                     #patchID
                #add the z=size faces
                z=size-1
                i = (x + y*(size-1) + z**2) * 5
                quadFaces[i]   = x     + (y     * size) + (z*size**2) + 1
                quadFaces[i+1] = x     + ((y+1) * size) + (z*size**2) + 1
                quadFaces[i+2] = (x+1) + ((y+1) * size) + (z*size**2) + 1
                quadFaces[i+3] = (x+1) + (y     * size) + (z*size**2) + 1
                quadFaces[i+4] = -1                     #patchID
        for z in range(size-1):
            for y in range(size-1):
                #add the x=0 faces
                i = (y + z*(size-1) + 2*((size-1)**2)) * 5
                quadFaces[i]   = y*size     + (z     * size**2) + 1
                quadFaces[i+1] = (y+1)*size + (z     * size**2) + 1
                quadFaces[i+2] = (y+1)*size + ((z+1) * size**2) + 1
                quadFaces[i+3] = y*size     + ((z+1) * size**2) + 1
                quadFaces[i+4] = -1                     #patchID
                #add the x=size faces
                x=size-1
                i = (y + z*(size-1) + x**2 + 2*((size-1)**2)) * 5
                quadFaces[i]   = y*size     + (z     * size**2) + (size-1) + 1
                quadFaces[i+1] = y*size     + ((z+1) * size**2) + (size-1) + 1
                quadFaces[i+2] = (y+1)*size + ((z+1) * size**2) + (size-1) + 1
                quadFaces[i+3] = (y+1)*size + (z     * size**2) + (size-1) + 1
                quadFaces[i+4] = -1                     #patchID
        for z in range(size-1):
            for x in range(size-1):
                #add the y=0 faces
                i = (x + z*(size-1) + 4*((size-1)**2)) * 5
                quadFaces[i]   = x     + (z     * size**2) + 1
                quadFaces[i+1] = x     + ((z+1) * size**2) + 1
                quadFaces[i+2] = (x+1) + ((z+1) * size**2) + 1
                quadFaces[i+3] = (x+1) + (z     * size**2) + 1
                quadFaces[i+4] = -1                     #patchID
                #add the y=size faces
                y=size-1
                i = (x + z*(size-1) + y**2 + 4*((size-1)**2)) * 5
                quadFaces[i]   = x     + (z     * size**2) + (size*(size-1)) + 1
                quadFaces[i+1] = (x+1) + (z     * size**2) + (size*(size-1)) + 1
                quadFaces[i+2] = (x+1) + ((z+1) * size**2) + (size*(size-1)) + 1
                quadFaces[i+3] = x     + ((z+1) * size**2) + (size*(size-1)) + 1
                quadFaces[i+4] = -1                     #patchID

        print("writing faces...")
        self.assertEqual(AVM.unstruc_write_faces(self.avmid,
                                                 None, 0,
                                                 quadFaces, nQuadFaces*5),0)

        #print "nodes: ",
        #for z in range(size):
        #    print
        #    print
        #    for y in range(size):
        #        print
        #        for x in range(size):
        #           i = (x + (y*size) + (z*size**2)) * 3
        #           print xyz[i],xyz[i+1],xyz[i+2], " ",
        #print
        #print

        #print "faces: ",
        #for i in range(nQuadFaces):
        #    print quadFaces[i*5],quadFaces[i*5+1],quadFaces[i*5+2],quadFaces[i*5+3],quadFaces[i*5+4]

        #print

        print("computing cells...")
        for z in range(size-1):
            for y in range(size-1):
                for x in range(size-1):
                    i = (x + y*(size-1) + z*((size-1)**2)) * 8
                    hexCells[i]   = x     + y*size     + (z+1)*(size**2) + 1
                    hexCells[i+1] = x     + (y+1)*size + (z+1)*(size**2) + 1
                    hexCells[i+2] = (x+1) + (y+1)*size + (z+1)*(size**2) + 1
                    hexCells[i+3] = (x+1) + y*size     + (z+1)*(size**2) + 1

                    hexCells[i+4] = x     + y*size     + z*(size**2) + 1
                    hexCells[i+5] = x     + (y+1)*size + z*(size**2) + 1
                    hexCells[i+6] = (x+1) + (y+1)*size + z*(size**2) + 1
                    hexCells[i+7] = (x+1) + y*size     + z*(size**2) + 1
 

        #print "cells: ",
        #for i in range(nHexCells*8):
        #    if i % 8 == 0: print
        #    print hexCells[i],
        #print

        print("writing cells...")
        self.assertEqual(AVM.unstruc_write_cells_nosize(self.avmid,
                                                 hexCells,
                                                 None,
                                                 None,
                                                 None),0)
        #AVM.unstruc_write_cells_nosize(self.avmid,
        #                                         hexCells,
        #                                         None,
        #                                         None,
        #                                         None)
        #errorMsg = AVM.get_error_str()
        #print "ERROR MSG:", errorMsg

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
        nBndTriFaces = 4
        nTriFaces = 5
        nBndQuadFaces = 4
        nQuadFaces = 5
        err, facePolyOrder = AVM.get_int(self.avmid, 'facePolyOrder')
        self.assertEqual(0,err)
        self.assertEqual(p,facePolyOrder)
        err, cellPolyOrder = AVM.get_int(self.avmid, 'cellPolyOrder')
        self.assertEqual(0,err)
        self.assertEqual(p,cellPolyOrder)

        #heavy data
        nodesPerQuad = AVM.nodes_per_quad(self.avmid, 1)
        nodesPerHex = AVM.nodes_per_hex(self.avmid, 1)

        xyz = AVM.r8Array(20*3)
        quadFaces = AVM.intArray(nQuadFaces*(nodesPerQuad+1))
        bndQuadFaces = AVM.intArray(nBndQuadFaces*(nodesPerQuad+1))
        hexCells = AVM.intArray(nHexCells*nodesPerHex)

        #nodes
        self.assertEqual(AVM.unstruc_read_nodes_r8(self.avmid, xyz, 20*3),0)

        for i in range(0,20*3):
            self.assertEqual(xyz[i], i)

        #all faces
        self.assertEqual(AVM.unstruc_read_faces(self.avmid,
                                                 triFaces, nTriFaces*(nodesPerTri+1),
                                                 quadFaces, nQuadFaces*(nodesPerQuad+1)),0)

        for i in range(0,nTriFaces):
            base = i*(nodesPerTri+1)
            for j in range(0,nodesPerTri):
                self.assertEqual(triFaces[base+j], p*(i+j))
            if i == nTriFaces-1:
                self.assertEqual(triFaces[base+nodesPerTri], 1)
            else:
                self.assertEqual(triFaces[base+nodesPerTri], -1)

        for i in range(0,nQuadFaces):
            base = i*(nodesPerQuad+1)
            for j in range(0,nodesPerQuad):
                self.assertEqual(quadFaces[base+j], p*(i+j))
            if i == nQuadFaces-1:
                self.assertEqual(quadFaces[base+nodesPerQuad], 1)
            else:
                self.assertEqual(quadFaces[base+nodesPerQuad], -1)

        #bnd faces
        self.assertEqual(AVM.unstruc_read_bnd_faces(self.avmid,
                                                 bndTriFaces, nBndTriFaces*(nodesPerTri+1),
                                                 bndQuadFaces, nBndQuadFaces*(nodesPerQuad+1)),0)

        for i in range(0,nBndTriFaces):
            base = i*(nodesPerTri+1)
            for j in range(0,nodesPerTri):
                self.assertEqual(bndTriFaces[base+j], p*(i+j))
            self.assertEqual(bndTriFaces[base+nodesPerTri], -1)

        for i in range(0,nBndQuadFaces):
            base = i*(nodesPerQuad+1)
            for j in range(0,nodesPerQuad):
                self.assertEqual(bndQuadFaces[base+j], p*(i+j))
            self.assertEqual(bndQuadFaces[base+nodesPerQuad], -1)

        #partial faces
        self.assertEqual(AVM.unstruc_read_partial_faces(self.avmid,
                                                 partialTriFaces, 2*(nodesPerTri+1), 2, 3,
                                                 partialQuadFaces, 2*(nodesPerQuad+1), 2, 3),0)

        for i in range(0,2):
            base = i*(nodesPerTri+1)
            for j in range(0,nodesPerTri):
                self.assertEqual(partialTriFaces[base+j], p*((i+1)+j))
            self.assertEqual(partialTriFaces[base+nodesPerTri], -1)

        for i in range(0,2):
            base = i*(nodesPerQuad+1)
            for j in range(0,nodesPerQuad):
                self.assertEqual(partialQuadFaces[base+j], p*((i+1)+j))
            self.assertEqual(partialQuadFaces[base+nodesPerQuad], -1)

        #cells
        self.assertEqual(AVM.unstruc_read_cells(self.avmid,
                                                 hexCells, nHexCells*nodesPerHex,
                                                 tetCells, nTetCells*nodesPerTet,
                                                 priCells, nPriCells*nodesPerPri,
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

