#ifndef AVMESH_FILE
#define AVMESH_FILE

#include "rev0/avmhdrs.h"
#include "rev0/bfstruc.h"
#include "rev0/cart.h"
#include "rev0/strand.h"
#include "rev0/unstruc.h"

#include "rev1/avmhdrs.h"
#include "rev1/bfstruc.h"
#include "rev1/cart.h"
#include "rev1/strand.h"
#include "rev1/unstruc.h"

#ifdef __cplusplus
#include <string>
#include <vector>
#endif

/*! @struct file_id_prefix_t
 * The first record of a AVMesh File.
 */
typedef struct file_id_prefix_t {
/*! magicString is "AVMESH" */
   char magicString[6];
/*! magicNumber is 1 */
   int magicNumber;
/*! formatRevision number */
   int formatRevision;

#ifdef __cplusplus
   file_id_prefix_t();
   int size() const;
   static int write(FILE* fp, bool swap, file_id_prefix_t*);
   static int read(FILE* fp, bool* swap, file_id_prefix_t*);
#endif
} file_id_prefix;

struct rev0_avmesh_file {
   FILE* fp;
   std::string filename;
   bool byte_swap;

   file_id_prefix file_prefix;
   rev0::file_header file_hdr;
   std::string description;

   std::vector<rev0::mesh_header> mesh_hdrs;
   std::vector<std::string> mesh_descriptions;

   std::vector<rev0::bfstruc_info> bfstruc;
   std::vector<rev0::strand_info> strand;
   std::vector<rev0::unstruc_info> unstruc;
   std::vector<rev0::cart_info> cart;

   int selection_meshid;
   int selection_patchid;
   int selection_patchparam;
   int selection_surfpatchid;
   int selection_edgepatchid;
   int selection_levelid;
   int selection_blockid;

   enum Endian { ENDIAN_NATIVE,
                 ENDIAN_LITTLE,
                 ENDIAN_BIG } write_endian;
};


struct rev1_avmesh_file {
   FILE* fp;
   std::string filename;
   bool byte_swap;

   file_id_prefix file_prefix;
   rev1::file_header file_hdr;
   std::string description;

   std::vector<rev1::mesh_header> mesh_hdrs;

   std::vector<rev1::bfstruc_info> bfstruc;
   std::vector<rev1::strand_info> strand;
   std::vector<rev1::unstruc_info> unstruc;
   std::vector<rev1::cart_info> cart;

   int selection_meshid;
   int selection_patchid;
   int selection_patchparam;
   int selection_surfpatchid;
   int selection_edgepatchid;
   int selection_levelid;
   int selection_blockid;

   enum Endian { ENDIAN_NATIVE,
                 ENDIAN_LITTLE,
                 ENDIAN_BIG } write_endian;
};

struct avmesh_file {
   int formatRevision;
   rev0_avmesh_file* rev0;
   rev1_avmesh_file* rev1;
};

#endif /* AVMESH_FILE */
