#ifndef _LOLUTILS
#define _LOLUTILS

#include "lolang.h"

LoLBytes lol_readfile(LoLCStr szFName, LoLQWord *pqwSize);
LoLCStr lol_op_name(LoLOpKind op);
LoLStr lol_strdup(LoLStr sSrc, LoLQWord qwSize);
LoLBytes lol_memdup(LoLBytes pSrc, LoLQWord qwSize);

#endif