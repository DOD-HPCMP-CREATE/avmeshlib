# This is an example of using the avmesh library with Python.
# Error checking is ignored for brevity.

import sys
sys.path.append('../..')
import avmeshlib as avmesh

retval, avmid = avmesh.read_headers('mixed.avm')
if 0 != retval:
   fprintf(stderr, 'ERROR: not an AVMesh file\n')
   sys.exit(1)

err, meshCount = avmesh.get_int(avmid, 'meshCount')
err, modelScale = avmesh.get_real(avmid, 'modelScale')
err, contactName = avmesh.get_str(avmid, 'contactName', 128)

ref_point = avmesh.r8Array(3)
avmesh.get_real_array(avmid, 'referencePoint', ref_point, 3)

print 'Mesh Count is %d' % meshCount

for i in xrange(1, meshCount+1):
   avmesh.select(avmid, 'mesh', i)
   err, name = avmesh.get_str(avmid, 'meshName', 128)
   err, type = avmesh.get_str(avmid, 'meshType', 128)
   err, modelScale = avmesh.get_real(avmid, 'modelScale')
   avmesh.get_real_array(avmid, 'referencePoint', ref_point, 3)

   print  "Mesh #%d named '%s' is type '%s'" % (i, name, type)
   print  'Model Scale is %f' % modelScale
   print  'Reference Point is %f, %f, %f' \
          % (ref_point[0], ref_point[1], ref_point[2])

   if type == 'strand':
      err, nBndEdges = avmesh.get_int(avmid, 'nBndEdges')
      err, nPtsPerStrand = avmesh.get_int(avmid, 'nPtsPerStrand')
      # ...
   elif type == 'unstruc':
      err, nNodes = avmesh.get_int(avmid, 'nNodes')
      err, nEdges = avmesh.get_int(avmid, 'nEdges')
      err, nFaces = avmesh.get_int(avmid, 'nFaces')
      # ...

avmesh.close(avmid)
