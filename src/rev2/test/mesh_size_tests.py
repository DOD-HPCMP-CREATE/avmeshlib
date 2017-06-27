import subprocess
import unittest

class TestFileSize(unittest.TestCase):

    def setUp(self):
        self.actualSizes = []
        out = subprocess.Popen(["./rev2_example_offset.exe", "mixed.avm"],
                    stdout=subprocess.PIPE).communicate()[0]
        for line in out.splitlines():
            self.actualSizes += [int(s) for s in line.split() if s.isdigit()]

        # File prefix
        ###############################################################
        #file_id_prefix: 'AVMESH' + int(1) + int(formatRevision)
        size = 6 + 4*2

        # File header
        ###############################################################

        #file_header: 4*4 + 128 + desc
        size += 4*4 + 128 + 128

        self.expectedMesh1HeaderSize = size

        # Generic mesh header
        ###############################################################
        #mesh header: 128*7 + 8*8
        size += 128*7 + 8*8

        # Strand mesh header
        ###############################################################
        #strand header: 4*14 + 8*3 + 32
        size += 4*14 + 8*3 + 32
        #2x strand_surf_patch: 4 + 32*3
        size += 2*(4 + 32*3)
        #4x strand_edge_patch: 4 + 32*3 + 8*3
        size += 4*(4 + 32*3 + 8*3)

        self.expectedMesh2HeaderSize = size

        # Generic mesh header
        ###############################################################
        #mesh header: 128*7 + 8*8
        size += 128*7 + 8*8

        # Unstruc mesh header
        ###############################################################
        #unstruc header: 4*20
        size += 4*22
        #2x unstruc patch_header: 32 + 16 + 4
        size += 2*(32 + 16 + 4)

        self.expectedMesh1DataSize = size

        # Strand mesh data
        ###############################################################
        #3*4*nNodes
        size += 3*4*6
        #4*nStrands (ID)
        size += 4*8
        #4*nStrands (Clip)
        size += 4*8
        #3*4*nStrands (PointingVec)
        size += 3*4*8
        #4*nPtsPerStrand (xStrand)
        size += 4*4
        #4*3*nTriFaces
        size += 4*3*2
        #4*4*nQuadFaces
        size += 4*4*1
        #4*polyFacesSize
        size += 4*10
        #4*nTriFaces+nQuadFaces+nPolyFaces (faceTag)
        size += 4*(2+1+1)
        #4*2*nBndEdges
        size += 4*2*6
        #4*nBndEdges (edgeTag)
        size += 4*6

        self.expectedMesh2DataSize = size

        # Unstruc mesh data
        ###############################################################
        #3*4*nNodes (because single-precision mesh)
        size += 3*4*12
        #5*4*triFaces
        size += 5*4*9
        #6*4*quadFaces
        size += 6*4*8
        #8*4*nHexCells
        size += 8*4*1
        #4*4*nTetCells
        size += 4*4*1
        #6*4*nPriCells
        size += 6*4*1
        #5*4*nPyrCells
        size += 5*4*1
        #4*2*nEdges
        size += 4*2*12
        #(4*4+8*2)*nNodesOnGeometry
        size += (4*4+8*2)*4
        #4*3*nEdgesOnGeometry
        size += 4*3*1
        #4*3*nFacesOnGeometry
        size += 4*3*2
        #(Next 4 arrays are AMR Volume IDs)
        #4*nHexCells
        size += 4*1
        #4*nTetCells
        size += 4*1
        #4*nPriCells
        size += 4*1
        #4*nPyrCells
        size += 4*1

        self.expectedFileSize = size

    def test_meshCount(self):
        self.assertEqual(2,self.actualSizes[0])

    def test_fileHeaderSize(self):
        self.assertEqual(self.expectedMesh1HeaderSize,self.actualSizes[1])

    def test_strandHeaderSize(self):
        self.assertEqual(self.expectedMesh2HeaderSize,self.actualSizes[2])

    def test_unstrucHeaderSize(self):
        self.assertEqual(self.expectedMesh1DataSize,self.actualSizes[3])

    def test_StrandDataSize(self):
        self.assertEqual(self.expectedMesh2DataSize,self.actualSizes[4])

    def test_UnstrucDataSize(self):
        self.assertEqual(self.expectedFileSize,self.actualSizes[5])

if __name__ == '__main__':
   unittest.main()
