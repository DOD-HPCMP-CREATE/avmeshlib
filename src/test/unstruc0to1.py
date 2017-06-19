import sys
sys.path.append('..')
import avmeshlib as AVM

def checkError(err,msg):
   if err != 0:
      print msg
      sys.exit(1)

if __name__ == '__main__':
   if len(sys.argv) < 3:
      print "USAGE: python unstruc0to1.py <input_mesh> <output_mesh>"
      sys.exit(1)

   err, rev0id = AVM.read_headers(sys.argv[1])
   checkError(err,"read_headers failed")

   err, rev1id = AVM.new_file(sys.argv[2], 1)
   checkError(err,"new_file failed")

   #read rev0 file header
   err = AVM.select(rev0id, 'header', 0)
   checkError(err,"select header failed")
   
   err, meshCount = AVM.get_int(rev0id, 'meshCount')
   checkError(err,"get meshCount failed")
   print "meshCount", meshCount

   err, contactName = AVM.get_str(rev0id, 'contactName', 128)
   checkError(err,"get contactName failed")
   err, contactOrg = AVM.get_str(rev0id, 'contactOrg', 128)
   checkError(err,"get contactOrg failed")
   err, contactEmail = AVM.get_str(rev0id, 'contactEmail', 128)
   checkError(err,"get contactEmail failed")
   print "contactName", contactName
   print "contactOrg", contactOrg
   print "contactEmail", contactEmail

   err, precision = AVM.get_int(rev0id, 'precision')
   checkError(err,"get precision failed")
   print "precision", precision
   err, dimensions = AVM.get_int(rev0id, 'dimensions')
   checkError(err,"get dimensions failed")
   print "dimensions", dimensions
   err, descriptionSize = AVM.get_int(rev0id, 'descriptionSize')
   checkError(err,"get descriptionSize failed")
   print "descriptionSize", descriptionSize
   err, description = AVM.get_str(rev0id, 'description', descriptionSize)
   checkError(err,"get description failed")
   print "description", description

   #write rev1 file header
   err = AVM.select(rev1id, 'header', 0)
   checkError(err,"select header failed")
   
   err = AVM.set_int(rev1id, 'meshCount', meshCount)
   checkError(err,"get meshCount failed")

   contactInfo = contactName + " " + contactOrg + " " + contactEmail
   err = AVM.set_str(rev1id, 'contactInfo', contactInfo, len(contactInfo))
   checkError(err,"set contactInfo failed")

   err = AVM.set_int(rev1id, 'precision', precision)
   checkError(err,"set precision failed")
   err = AVM.set_int(rev1id, 'dimensions', dimensions)
   checkError(err,"set dimensions failed")
   err = AVM.set_str(rev1id, 'description', description, descriptionSize)
   checkError(err,"set description failed")

   for meshNum in range(1,meshCount+1):
      #read rev0 mesh headers
      err = AVM.select(rev0id, 'mesh', meshNum)
      checkError(err,"select mesh failed")

      err, meshName = AVM.get_str(rev0id, 'meshName', 128)
      checkError(err,"get meshName failed")
      err, meshType = AVM.get_str(rev0id, 'meshType', 128)
      checkError(err,"get meshType failed")
      err, meshGenerator = AVM.get_str(rev0id, 'meshGenerator', 128)
      checkError(err,"get meshGenerator failed")
      err, coordinateSystem = AVM.get_str(rev0id, 'coordinateSystem', 128)
      checkError(err,"get coordinateSystem failed")
      err, modelScale = AVM.get_real(rev0id, 'modelScale')
      checkError(err,"get modelScale failed")
      err, gridUnits = AVM.get_str(rev0id, 'gridUnits', 128)
      checkError(err,"get gridUnits failed")
      err, referenceLength = AVM.get_real(rev0id, 'referenceLength')
      checkError(err,"get referenceLength failed")
      err, referencePointDescription = AVM.get_str(rev0id, 'referencePointDescription', 128)
      checkError(err,"get referencePointDescription failed")
      err, descriptionSize = AVM.get_int(rev0id, 'descriptionSize')
      checkError(err,"get descriptionSize failed")
      err, meshDescription = AVM.get_str(rev0id, 'description', descriptionSize)
      checkError(err,"get meshDescription failed")

      err, nNodes = AVM.get_int(rev0id, 'nNodes')
      checkError(err,"get nNodes failed")
      print "nNodes", nNodes
      err, nFaces = AVM.get_int(rev0id, 'nFaces')
      checkError(err,"get nFaces failed")
      err, nMaxNodesPerFace = AVM.get_int(rev0id, 'nMaxNodesPerFace')
      checkError(err,"get nMaxNodesPerFace failed")
      err, nTriFaces = AVM.get_int(rev0id, 'nTriFaces')
      checkError(err,"get nTriFaces failed")
      err, nQuadFaces = AVM.get_int(rev0id, 'nQuadFaces')
      checkError(err,"get nQuadFaces failed")
      err, nBndTriFaces = AVM.get_int(rev0id, 'nBndTriFaces')
      checkError(err,"get nBndTriFaces failed")
      err, nBndQuadFaces = AVM.get_int(rev0id, 'nBndQuadFaces')
      checkError(err,"get nBndQuadFaces failed")
      err, nCells = AVM.get_int(rev0id, 'nCells')
      checkError(err,"get nCells failed")
      err, nMaxNodesPerCell = AVM.get_int(rev0id, 'nMaxNodesPerCell')
      checkError(err,"get nMaxNodesPerCell failed")
      err, nMaxFacesPerCell = AVM.get_int(rev0id, 'nMaxFacesPerCell')
      checkError(err,"get nMaxFacesPerCell failed")
      err, nPatches = AVM.get_int(rev0id, 'nPatches')
      checkError(err,"get nPatches failed")
      print "nPatches", nPatches

      #write rev1 mesh headers
      err = AVM.select(rev1id, 'mesh', meshNum)

      err = AVM.set_str(rev1id, 'meshName', meshName, len(meshName))
      checkError(err,"set meshName failed")
      err = AVM.set_str(rev1id, 'meshType', meshType, len(meshType))
      checkError(err,"set meshType failed")
      err = AVM.set_str(rev1id, 'meshGenerator', meshGenerator, len(meshGenerator))
      checkError(err,"set meshGenerator failed")
      err = AVM.set_str(rev1id, 'coordinateSystem', coordinateSystem, len(coordinateSystem))
      checkError(err,"set coordinateSystem failed")
      err = AVM.set_real(rev1id, 'modelScale', modelScale)
      checkError(err,"set modelScale failed")
      err = AVM.set_str(rev1id, 'gridUnits', gridUnits, len(gridUnits))
      checkError(err,"set gridUnits failed")
      err = AVM.set_real(rev1id, 'referenceLength', referenceLength)
      checkError(err,"set referenceLength failed")
      err = AVM.set_str(rev1id, 'referencePointDescription', referencePointDescription, len(referencePointDescription))
      checkError(err,"set referencePointDescription failed")
      err = AVM.set_str(rev1id, 'meshDescription', meshDescription, len(meshDescription))
      checkError(err,"set meshDescription failed")
      #FIXME: add new in rev1 header fields

      err = AVM.set_int(rev1id, 'nNodes', nNodes)
      checkError(err,"set nNodes failed")
      err = AVM.set_int(rev1id, 'nFaces', nFaces)
      checkError(err,"set nFaces failed")
      err = AVM.set_int(rev1id, 'nMaxNodesPerFace', nMaxNodesPerFace)
      checkError(err,"set nMaxNodesPerFace failed")
      err = AVM.set_int(rev1id, 'nTriFaces', nTriFaces)
      checkError(err,"set nTriFaces failed")
      err = AVM.set_int(rev1id, 'nQuadFaces', nQuadFaces)
      checkError(err,"set nQuadFaces failed")
      err = AVM.set_int(rev1id, 'nBndTriFaces', nBndTriFaces)
      checkError(err,"set nBndTriFaces failed")
      err = AVM.set_int(rev1id, 'nBndQuadFaces', nBndQuadFaces)
      checkError(err,"set nBndQuadFaces failed")
      err = AVM.set_int(rev1id, 'nCells', nCells)
      checkError(err,"set nCells failed")
      err = AVM.set_int(rev1id, 'nMaxNodesPerCell', nMaxNodesPerCell)
      checkError(err,"set nMaxNodesPerCell failed")
      err = AVM.set_int(rev1id, 'nMaxFacesPerCell', nMaxFacesPerCell)
      checkError(err,"set nMaxFacesPerCell failed")
      err = AVM.set_int(rev1id, 'nPatches', nPatches)
      checkError(err,"set nPatches failed")

      print "nPatches", nPatches
      for patchNum in range(1,nPatches+1):
         #read rev0 patch headers
         err = AVM.select(rev0id, 'patch', patchNum)
         checkError(err,"select patch failed")
         err, patchLabel = AVM.get_str(rev0id, 'patchLabel', 32)
         checkError(err,"get patchLabel failed")
         err, patchType = AVM.get_str(rev0id, 'patchType', 32)
         checkError(err,"get patchType failed")
         err, patchId = AVM.get_int(rev0id, 'patchId')
         checkError(err,"get patchId failed")
         print patchLabel, patchType, patchId

         #write rev1 patch headers
         err = AVM.select(rev1id, 'patch', patchNum)
         checkError(err,"select patch failed")
         err = AVM.set_str(rev1id, 'patchLabel', patchLabel, len(patchLabel))
         checkError(err,"set patchLabel failed")
         err = AVM.set_str(rev1id, 'patchType', patchType, len(patchType))
         checkError(err,"set patchType failed")
         err = AVM.set_int(rev1id, 'patchId', patchId)
         checkError(err,"set patchId failed")

   err = AVM.write_headers(rev1id)

   for meshNum in range(1,meshCount+1):
      #read rev0 mesh data
      err = AVM.select(rev0id, 'mesh', meshNum)
      checkError(err,"select mesh failed")
      if precision == 1:
         xyz = AVM.r4Array(3*nNodes)
      elif precision == 2:
         xyz = AVM.r8Array(3*nNodes)
      triFaces = AVM.intArray(5*nTriFaces)
      quadFaces = AVM.intArray(6*nQuadFaces)

      if precision == 1:
         err = AVM.unstruc_read_nodes_r4(rev0id, xyz, 3*nNodes)
         checkError(err,"read_nodes failed")
      elif precision == 2:
         err = AVM.unstruc_read_nodes_r8(rev0id, xyz, 3*nNodes)
         checkError(err,"read_nodes failed")

      err = AVM.unstruc_read_faces(rev0id, triFaces,  5*nTriFaces,
                                           quadFaces, 6*nQuadFaces,
                                           None, 0)
      checkError(err,"read_faces failed")

      #print [xyz[i] for i in range(0,3*nNodes)]
      #print [triFaces[i] for i in range(0,5*nTriFaces)]
      #print [quadFaces[i] for i in range(0,6*nQuadFaces)]

      #write rev1 mesh data
      err = AVM.select(rev1id, 'mesh', meshNum)
      checkError(err,"select mesh failed")

      if precision == 1:
         err = AVM.unstruc_write_nodes_r4(rev1id, xyz, 3*nNodes)
         checkError(err,"write_nodes failed")
      elif precision == 2:
         err = AVM.unstruc_write_nodes_r8(rev1id, xyz, 3*nNodes)
         checkError(err,"write_nodes failed")

      err = AVM.unstruc_write_faces(rev1id, triFaces,  5*nTriFaces,
                                            quadFaces, 6*nQuadFaces,
                                            None, 0)
      checkError(err,"write_faces failed")

   AVM.close(rev0id)
   AVM.close(rev1id)
