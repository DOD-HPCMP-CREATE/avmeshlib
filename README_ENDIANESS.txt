Endianess Notes
===============

This document describes how AVMesh library handless endianess for numeric data
in AVMesh file I/O.

Reading
-------

@ref avm_read_headers automatically handles detecting endianess of an AVMesh
file and then converting, if necessary, to native machine endianess.  @ref
avm_read_headers does this by checking the file header magic number field.  If
the magic number is one then the file contents is assumed to match the native
machine endianess.  If the magic number is not one, then the magic number is
byte swapped and then checked to be one, then the remaining file contents are
automatically swapped.  If the magic number is not one even after byte swapping
then the file is assumed not to be a valid AVMesh file.

Writing
-------

It is possible to write an AVMesh file in a different endianness than a host
machine.  To write in a different endianness, set the "AVM_WRITE_ENDIAN" flag
after the @ref avm_new_file call and before the @ref avm_write_headers call.
Set the "AVM_WRITE_ENDIAN" with @ref avm_set_str.  The valid options are
"little", "big", and "native".

Testing
-------

./configure --test-bigendian ...
make
make test
