#ifndef _LOLUTILS
#define _LOLUTILS

#include "lolang.h"

LoLCStr lol_readfile(LoLCStr szFName, LoLQWord *pqwSize);
LoLCStr op_name(LoLOpKind op);

#endif