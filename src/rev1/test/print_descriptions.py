import sys
sys.path.append('../..')
import avmeshlib as AVM

filename = sys.argv[1]

retval, avmid = AVM.read_headers(filename)
if 0 != retval:
   fprintf(stderr, 'ERROR: not an AVM file\n')
   sys.exit(1)

err = AVM.select(avmid, 'header', 0)

err, meshCount = AVM.get_int(avmid, 'meshCount')
print 'meshCount =', meshCount

err, descriptionSize = AVM.get_int(avmid, 'descriptionSize')
print 'descriptionSize =', descriptionSize

err, s = AVM.get_str(avmid, 'description', descriptionSize)
print "description = '%s'" % s

for meshid in range(1, meshCount+1):
   err = AVM.select(avmid, 'mesh', meshid)

   err, name = AVM.get_str(avmid, 'meshName', 64)
   print "mesh: '%s'" % name

   err, s = AVM.get_str(avmid, 'meshDescription', 128)
   print " - description = '%s'" % s
