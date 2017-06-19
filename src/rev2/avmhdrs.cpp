
#include <stdio.h>
#include <string.h>
#include "avmhdrs.h"
using namespace std;
using namespace rev2;

extern "C" void byte_swap_int(int*);
extern "C" void byte_swap_float(float*);
extern "C" void byte_swap_double(double*);

file_header_t::file_header_t()
{
   meshCount         = 0;
   precision         = 1;
   dimensions        = 3;
   descriptionSize   = 0;

   strncpy(contactInfo, "", sizeof(contactInfo));
}

int file_header_t::size() const
{
   return sizeof(meshCount) +
          sizeof(contactInfo) +
          sizeof(precision) +
          sizeof(dimensions) +
          sizeof(descriptionSize);
}

int file_header_t::write(FILE* fp, bool swap, file_header_t* p)
{
   if (swap) {
      byte_swap_int(&p->meshCount);
      byte_swap_int(&p->precision);
      byte_swap_int(&p->dimensions);
      byte_swap_int(&p->descriptionSize);
   }

   if (!fwrite(&p->meshCount, sizeof(p->meshCount), 1, fp)) return 0;
   if (!fwrite(p->contactInfo, sizeof(p->contactInfo), 1, fp)) return 0;
   if (!fwrite(&p->precision, sizeof(p->precision), 1, fp)) return 0;
   if (!fwrite(&p->dimensions, sizeof(p->dimensions), 1, fp)) return 0;
   if (!fwrite(&p->descriptionSize, sizeof(p->descriptionSize), 1, fp)) return 0;

   if (swap) {
      byte_swap_int(&p->meshCount);
      byte_swap_int(&p->precision);
      byte_swap_int(&p->dimensions);
      byte_swap_int(&p->descriptionSize);
   }

   return 1;
}

int file_header_t::read(FILE* fp, bool swap, file_header_t* p)
{
   if (!fread(&p->meshCount, sizeof(p->meshCount), 1, fp)) return 0;
   if (!fread(p->contactInfo, sizeof(p->contactInfo), 1, fp)) return 0;
   if (!fread(&p->precision, sizeof(p->precision), 1, fp)) return 0;
   if (!fread(&p->dimensions, sizeof(p->dimensions), 1, fp)) return 0;
   if (!fread(&p->descriptionSize, sizeof(p->descriptionSize), 1, fp)) return 0;

   if (swap) {
      byte_swap_int(&p->meshCount);
      byte_swap_int(&p->precision);
      byte_swap_int(&p->dimensions);
      byte_swap_int(&p->descriptionSize);
   }

   return 1;
}

mesh_header_t::mesh_header_t()
{
   modelScale      = 1.0;
   referenceLength = 1.0;
   referenceArea   = 1.0;
   referencePoint[0]    = 0.0;
   referencePoint[1]    = 0.0;
   referencePoint[2]    = 0.0;

   strncpy(meshName, "", sizeof(meshName));
   strncpy(meshType, "", sizeof(meshType));
   strncpy(meshGenerator, "", sizeof(meshGenerator));
   strncpy(coordinateSystem, "", sizeof(coordinateSystem));
   strncpy(gridUnits, "", sizeof(gridUnits));
   strncpy(referencePointDescription, "", sizeof(referencePointDescription));
   strncpy(meshDescription, "", sizeof(meshDescription));
}

int mesh_header_t::size() const
{
   return sizeof(meshName) +
          sizeof(meshType) +
          sizeof(meshGenerator) +
          sizeof(coordinateSystem) +
          sizeof(modelScale) +
          sizeof(gridUnits) +
          sizeof(referenceLength) +
          sizeof(referenceArea) +
          sizeof(referencePoint) +
          sizeof(referencePointDescription) +
          sizeof(meshDescription);
}

int mesh_header_t::write(FILE* fp, bool swap, mesh_header_t* p)
{
   if (swap) {
      byte_swap_double(&p->modelScale);
      byte_swap_double(&p->referenceLength);
      byte_swap_double(&p->referenceArea);
      byte_swap_double(&p->referencePoint[0]);
      byte_swap_double(&p->referencePoint[1]);
      byte_swap_double(&p->referencePoint[2]);
   }

   if (!fwrite(p->meshName, sizeof(p->meshName), 1, fp)) return 0;
   if (!fwrite(p->meshType, sizeof(p->meshType), 1, fp)) return 0;
   if (!fwrite(p->meshGenerator, sizeof(p->meshGenerator), 1, fp)) return 0;
   if (!fwrite(p->coordinateSystem, sizeof(p->coordinateSystem), 1, fp)) return 0;
   if (!fwrite(&p->modelScale, sizeof(p->modelScale), 1, fp)) return 0;
   if (!fwrite(p->gridUnits, sizeof(p->gridUnits), 1, fp)) return 0;
   if (!fwrite(&p->referenceLength, sizeof(p->referenceLength), 1, fp)) return 0;
   if (!fwrite(&p->referenceArea, sizeof(p->referenceArea), 1, fp)) return 0;
   if (!fwrite(p->referencePoint, sizeof(p->referencePoint), 1, fp)) return 0;
   if (!fwrite(p->referencePointDescription, sizeof(p->referencePointDescription), 1, fp)) return 0;
   if (!fwrite(&p->meshDescription, sizeof(p->meshDescription), 1, fp)) return 0;

   if (swap) {
      byte_swap_double(&p->modelScale);
      byte_swap_double(&p->referenceLength);
      byte_swap_double(&p->referenceArea);
      byte_swap_double(&p->referencePoint[0]);
      byte_swap_double(&p->referencePoint[1]);
      byte_swap_double(&p->referencePoint[2]);
   }

   return 1;
}

int mesh_header_t::read(FILE* fp, bool swap, mesh_header_t* p)
{
   if (!fread(p->meshName, sizeof(p->meshName), 1, fp)) return 0;
   if (!fread(p->meshType, sizeof(p->meshType), 1, fp)) return 0;
   if (!fread(p->meshGenerator, sizeof(p->meshGenerator), 1, fp)) return 0;
   if (!fread(p->coordinateSystem, sizeof(p->coordinateSystem), 1, fp)) return 0;
   if (!fread(&p->modelScale, sizeof(p->modelScale), 1, fp)) return 0;
   if (!fread(p->gridUnits, sizeof(p->gridUnits), 1, fp)) return 0;
   if (!fread(&p->referenceLength, sizeof(p->referenceLength), 1, fp)) return 0;
   if (!fread(&p->referenceArea, sizeof(p->referenceArea), 1, fp)) return 0;
   if (!fread(p->referencePoint, sizeof(p->referencePoint), 1, fp)) return 0;
   if (!fread(p->referencePointDescription, sizeof(p->referencePointDescription), 1, fp)) return 0;
   if (!fread(&p->meshDescription, sizeof(p->meshDescription), 1, fp)) return 0;

   if (swap) {
      byte_swap_double(&p->modelScale);
      byte_swap_double(&p->referenceLength);
      byte_swap_double(&p->referenceArea);
      byte_swap_double(&p->referencePoint[0]);
      byte_swap_double(&p->referencePoint[1]);
      byte_swap_double(&p->referencePoint[2]);
   }

   return 1;
}
