/** @mainpage
This document describes the AVMesh format and library.

@section Overview

An AVMesh file is a compact binary file that can be read or written with the
AVMesh library or your own routines. The library allows you to read or write
any AVMesh file (big or little endian, single or double precision) with one
set of easy-to-use calls. For examples on reading an AVMesh file see the @ref
quick_start section. For an understanding of the format, read on or see the
format spreadsheet that is included with the code.

Features of the AVMesh Format:
   - Support for multiple meshes
   - Support for meshes of different types in the same file
   - Extensive file-level meta-data
   - Mesh data stored in variable precision and dimensions
   - Boundary condition information stored alongside mesh data
   - Variable-length file-level description string
   - Variable-length mesh description strings for each mesh

Features of the AVMesh Library:
   - High-level routines for creating and reading AVMesh files
   - High-level routines for setting and getting AVMesh string, integer, and
     floating-point meta-data
   - Caching all file/mesh level meta data
   - Mesh type specific routines for reading and writing mesh data
   - Variable order mesh reads (i.e., read just the nth mesh in a file)
   - Mesh offset calculation routines for use by MPI/IO
   - C, Fortran, and Python APIs
   - Complete unit test coverage

@section format AVMesh Format

@image html format.png
@image latex format.eps

The AVMesh file format is this:
<pre>
   <file-header>
   <mesh-1-header>
   <mesh-2-header>
   <mesh...header>
   <mesh-n-header>
   <mesh-1-data>
   <mesh-2-data>
   <mesh...data>
   <mesh-n-data>
</pre>

The <code><file-header></code> is:
<pre>
   <file-id-prefix>
   <file-header>
   <file-description> (variable length)
</pre>

The <code><mesh-n-header></code>s are:
<pre>
   <generic-mesh-header>
   <mesh-description> (variable length)
   <specific-mesh-header>
</pre>

The <code><mesh-n-data></code> format is determined by the specific mesh
types.

See @ref avmhdrs for more details.

@section avmeshlib AVMesh Library

The AVMesh library is a simple way to read or create AVMesh files. Here is an
example of reading a body-fitted structured file with the Fortran API.

@subsection quick_start Quick Start 
<pre>
01: call avm_read_headersf(id, 'bfstruc.grid', ierr)
02: if (ierr.ne.0) stop 'ERROR: avm_read_headersf'
03:
04: call avm_get_intf(id, 'meshCount', nMesh, ierr)
05: call avm_get_intf(id, 'precision', precision, ierr)
06: ...
07:
08: do m = 1, nMesh
09:    call avm_selectf(id, 'mesh', m, ierr)
10:    call avm_get_strf(id, 'meshName', name, ierr)
11:    call avm_get_strf(id, 'meshType', type, ierr)
12:    ! assume that type is 'bfstruc'
13:    call avm_get_intf(id, 'jmax', jd, ierr)
14:    call avm_get_intf(id, 'kmax', kd, ierr)
15:    call avm_get_intf(id, 'lmax', ld, ierr)
16:
17:    if (precision == 1) then
18:       allocate(x4(jd,kd,ld), y4(jd,kd,ld), z4(jd,kd,ld))
19:       call avm_bfstruc_read_xyz_r4f(id, x4,y4,z4, jd*kd*ld, ierr)
20:       if (ierr.ne.0) stop 'ERROR: avm_bfstruc_read_xyz_r4f'
21:       ...
22:    elseif (precision == 2) then
23:       allocate(x8(jd,kd,ld), y8(jd,kd,ld), z8(jd,kd,ld))
24:       call avm_bfstruc_read_xyz_r8f(id, x8,y8,z8, jd*kd*ld, ierr)
25:       if (ierr.ne.0) stop 'ERROR: avm_bfstruc_read_xyz_r8f'
26:       ...
27:    endif
28: end do
29:
30: call avm_closef(id, ierr)
</pre>

On line #1 the AVMesh file is opened and all file and mesh-level headers are
cached into AVMesh managed-memory. Very little memory is allocated by the
library. All AVMesh routines begin with <code>avm_</code>. The library
supports reading multiple files at the same time, so the first argument to all
routines is an <code>id</code> argument. @ref avm_read_headers returns the
<code>id</code> for the current file being read. This is kind of like a file
handle and is passed to the other <code>avm_</code> routines.

All AVMesh routines return an error value; with the Fortran API an additional
argument is received that returns this error code. Return codes are zero on
success, non-zero on failure. Also with the Fortran API, the letter 'f' is
appended to each routine.

On lines #4-5, AVMesh header get routines are called to pick out file level
meta-data values that are cached by the library. On line #9 the
<code>n</code>th mesh is selected, then header values are picked out of the
AVMesh header cache. On lines #17-27 structured mesh data is read from the
file. AVMesh does not allocate any significant amount of memory; mesh data
memory is allocated by the caller. On line #20 @ref avm_close is called and
all header cache memory is reclaimed.

For more information including reading/writing AVMesh files and calling the
API from C, Fortran, and Python, see the @ref quick_example and @ref api.

@subsection install Installation 
To compile the library:
<pre>
   tar xzf avmeshlib-VERSION.tar.gz
   cd avmeshlib-VERSION
   ./configure
   make
</pre>

To exercise the included unit test suite, run <code>./configure
--gtest-dir=...</code> and then run <code>make test</code>.

See the @ref install_guide for more information.
*/ 

/** @page quick_example Library Guide

@section overview Overview

The AVMesh library contains routines for creating and reading AVMesh files.

All AVMesh library routines begin with <code>avm_</code>. The AVMesh library
supports creating or reading multiple files at the same time. The first
argument of all routines is an '<code>avmid</code>' argument.  The following
routines return new <code>avmids</code>: @ref avm_new_file and @ref
avm_read_headers.

All routines return an error value; C routines as the value of the function
and Fortran routines in the last argument. Return codes are zero on success,
non-zero on failure.

@subsection reading Reading Files with AVMesh

The general sequence of calls for reading is:
   - @ref avm_read_headers
   - @ref avm_select
   - <code>avm_get_*</code> routines (such as @ref avm_get_int, @ref
     avm_get_real, et al)
   - <code>avm_<type>_read_*</code> routines (such as @ref
     avm_bfstruc_read_xyz_r4, @ref avm_unstruc_read_nodes_r4, et al)
   - @ref avm_close

@ref avm_read_headers opens a file and checks its validity. It then caches
all file and mesh header information.

@ref avm_select picks cached information with a name and id, which enables the
<code>avm_get_*</code> routines.

<code>avm_get_*</code> picks out information from the file and mesh header
cache.

<code>avm_<type>_read_*</code> routines read specific mesh data from the file.
Note that the caller is responsible for allocating arrays passed into these
routines. The size of these arrays are passed in as well and these sizes are
checked against the sizes in the mesh headers. <code>avm_select(avmid, "mesh",
<meshnumber>)</code> needs to be called prior to calling these read routines.

@ref avm_close closes the file and reclaims file and mesh header memory.

@subsection writing Writing Files with AVMesh

   - @ref avm_new_file
   - @ref avm_select
   - <code>avm_set_*</code> routines (such as @ref avm_set_int, @ref
     avm_set_real, et al)
   - @ref avm_write_headers
   - <code>avm_<type>_write_*</code> routines (such as @ref
     avm_bfstruc_write_xyz_r4, @ref avm_unstruc_write_nodes_r4, et al)
   - @ref avm_close

@ref avm_new_file initializes a new file and mesh header cache. Following this
call <code>avm_set_*</code> calls can be made to populate file header cache.

@ref avm_select picks cached information with a name and id, which enables the
<code>avm_set_*</code> routines for the mesh header cache.

<code>avm_set_*</code> applies information to the file and mesh header cache.

@ref avm_write_headers commits file and mesh header information to disk.

<code>avm_<type>_write_*</code> routines writes specific mesh data to the file.

@ref avm_close closes the file and reclaims file and mesh header memory.

@subsection select avm_select

@ref avm_select is used to pick cached information with a name and id, which
enables the <code>avm_get_*</code> and <code>avm_set_*</code> routines to be
called. For a detailed listing of name/id inputs see the API Reference
documentation at @ref avm_select.

@subsection get_and_set avm_get_* and avm_set_*

<code>avm_get_*</code> and <code>avm_set_*</code> routines are used to query
and apply information to and from file and mesh header caches. The names that
are used for these routines are from the @ref avmhdrs. A listing of these get
and set routines are found in the @ref api reference.

@subsection Mesh Data I/O Routines

<code>avm_<type>_read</code> and <code>avm_<type>_write</code> routines are
used to read and write mesh data to and from AVMesh files. In all cases the
memory that is used by these routines is allocated by the caller. A listing of
these routines are found in the @ref api reference.

The routines prefixed with <code>_r4</code> are for single-precision data I/O.
The routines prefixed with <code>_r8</code> are for double-precision data I/O.

@subsection offsets Variable Order Mesh Reading and Mesh Offsets 

It is possible to read data in a variable order from which it is listed in a
file. That is, if you want to read mesh 3's data without allocating memory for
mesh 1 or mesh 2, then @ref avm_seek_to_mesh is for you. For example:
<pre>
   avm_select(avmid, "mesh", 3);
   avm_seek_to_mesh(avmid, 3); 
   avm_<type>_read_*(...);
</pre>

It is also possible to grab the mesh offset values to use the AVMesh library
to read header fields and your own routines to read mesh data. There routines
are:
   - @ref avm_mesh_header_offset
   - @ref avm_mesh_data_offset

Here is an example of their use:
@include example_offset.c

@section reader Reader Samples 

@subsection example_reader_c C Example
This sample demonstrates reading an AVMesh file with C.
@include example_reader.c

@subsection example_reader_f90 Fortran Example
This sample demonstrates reading an AVMesh file with Fortran.  All routines in
Fortran have an additional argument (<code>istatus</code>) at the end of the
argument list.  <code>istatus</code> is an integer and has the same meaning as
the return value of the routine in C. Also with the Fortran API, the letter
'f' is appended to each routine.
@include example_reader.f90

@subsection example_reader_py Python Example
This sample demonstrates reading an AVMesh file with Python.
@include example_reader.py

@section writer Writer Samples 

@subsection example_writer_f90 Fortran Example
This sample demonstrates writing an AVMesh file with Fortran.
@include strand_write.f90

Examples of writing other formats are in the
<code>avmeshlib-VERSION/src/rev1/test</code> directory.
*/
