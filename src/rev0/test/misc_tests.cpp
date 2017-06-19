#include <cstdio>
using namespace std;
#include <gtest/gtest.h>
#include "avmesh.h"

TEST(Misc, TypeChecks) {
   EXPECT_EQ(sizeof(long), sizeof(off_t)) << "If this test fails, then the"
   " fortran interfaces to avm_mesh_header_offset() and"
   " avm_mesh_header_offset() will not work";
}

TEST(Misc, Endianess_API_Checks) {
   int id;
   char str[AVM_STD_STRING_LENGTH];
   EXPECT_EQ(0, avm_new_file(&id, "/dev/null",0));
   EXPECT_EQ(0, avm_get_str(id, "AVM_WRITE_ENDIAN", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("native", str);
   EXPECT_EQ(0, avm_set_str(id, "AVM_WRITE_ENDIAN", "big", strlen("big")));
   EXPECT_EQ(0, avm_get_str(id, "AVM_WRITE_ENDIAN", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("big", str);
   EXPECT_EQ(0, avm_set_str(id, "AVM_WRITE_ENDIAN", "little", strlen("little")));
   EXPECT_EQ(0, avm_get_str(id, "AVM_WRITE_ENDIAN", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("little", str);
   EXPECT_EQ(0, avm_set_str(id, "AVM_WRITE_ENDIAN", "native", strlen("native")));
   EXPECT_EQ(0, avm_get_str(id, "AVM_WRITE_ENDIAN", str, AVM_STD_STRING_LENGTH));
   EXPECT_STREQ("native", str);
   EXPECT_EQ(0, avm_close(id));
}
