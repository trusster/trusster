#include "alu_model.h"


unsigned int alu_model (unsigned int opa, unsigned int opb, unsigned char op)
{
  switch (op) {
    case 0: return opa + opb;
    case 1: return opa - opb;
    case 2: return opa & opb;
    case 3: return opa | opb;
  default: return 0;
  }
  return 0;
}
