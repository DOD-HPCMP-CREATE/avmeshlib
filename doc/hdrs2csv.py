import sys

class TypeDef:
   def __init__(self, string):
      bytesPerType = {'int':4,'double':8,'char':1}
      self.fields = []
      doneWithIfdef = True 
      for line in string.split('\n'):

         if line.startswith('typedef'):
            continue

         if line.startswith('/*!'):
            field = {}
            field['description'] = line.strip('/*! ')
            doneWithDesc = line.endswith('*/')
            continue

         if not doneWithDesc:
            field['description'] += ' ' + line.strip('/*! ')
            doneWithDesc = line.endswith('*/')
            continue

         if line.startswith('#ifdef'):
            doneWithIfdef = False 
            continue

         if not doneWithIfdef:
            doneWithIfdef = line.startswith('#endif')
            continue

         # done with description, now parse the field
         line = line.strip(' ];')
         words = line.split(' ')
         if len(words)>0:
            field['type'] = words[0]
            if field['type'] in bytesPerType:
               field['bytes'] = bytesPerType[field['type']]
         if len(words)>1:
            words = words[1].split('[')
            field['name'] = words[0]
            if len(words)>1 and 'bytes' in field:
               field['bytes'] *= int(words[1])
         if 'description' in field:
            self.fields.append(field)
            field = {}

      self.name = line.strip('} ;\n')

#==============================================================================
import unittest
class TestTypeDef(unittest.TestCase):
   def test(self):
      td = TypeDef('''typedef booya_t {
/*! field 1 description
Multi-line
*/
   int field1;
/*! field 2 description */
   double field2;
/*! field 3 description */
   char field3[32];
#ifdef __cplusplus
   file_id_prefix_t();
   static int write(FILE* fp, const file_id_prefix_t*);
   static int read(FILE* fp, bool* swap, file_id_prefix_t*);
#endif
} booya;''')

      self.assertEqual("booya", td.name)
      self.assertEqual(3, len(td.fields))
      # 1st field
      self.assertEqual('''field 1 description Multi-line ''', td.fields[0]["description"])
      self.assertEqual("int", td.fields[0]["type"])
      self.assertEqual("field1", td.fields[0]["name"])
      self.assertEqual(4, td.fields[0]["bytes"])
      # 2nd field
      self.assertEqual("field 2 description", td.fields[1]["description"])
      self.assertEqual("double", td.fields[1]["type"])
      self.assertEqual("field2", td.fields[1]["name"])
      self.assertEqual(8, td.fields[1]["bytes"])
      # 3rd field
      self.assertEqual("field 3 description", td.fields[2]["description"])
      self.assertEqual("char", td.fields[2]["type"])
      self.assertEqual("field3", td.fields[2]["name"])
      self.assertEqual(32, td.fields[2]["bytes"])

#if __name__ == '__main__':
#   unittest.main()
#   sys.exit(0)
#==============================================================================

tds = []

for filename in sys.argv[1:]:
   f = open(filename,"r")
   doneWithParse = True
   for line in f.readlines():
      line = line.strip()
      if line.startswith('typedef') and line.endswith('{'):
         tdstr = line + '\n'
         doneWithParse = False
         continue
      if not doneWithParse:
         tdstr += line + '\n'
         doneWithParse = line.startswith('}') and line.endswith(';')
         if doneWithParse:
            tdstr = tdstr.strip()
            tds.append(TypeDef(tdstr)) 

print('Name,Bytes,Type,Default,Description')

typeMap = {'int':'Integer','double':'Float','char':'String'}

for td in tds:
   print(td.name)
   for field in td.fields:
      print('%s,%s,%s,...,%s' % (field['name'], field['bytes'], 
      typeMap[field['type']], field['description'].replace(',','')))
   print()
