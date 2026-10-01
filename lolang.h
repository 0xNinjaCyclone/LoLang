#ifndef _LOLANG
#define _LOLANG

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define LOL_ZERO ( 0 )
#define LOL_NULL ( NULL )
#define LOL_TRUE ( true )
#define LOL_FALSE ( false )
#define LOL_SUCCESS ( EXIT_SUCCESS )
#define LOL_FAILURE ( EXIT_FAILURE ) 

typedef int LoLInt;
typedef char LoLChr;
typedef LoLChr* LoLStr;
typedef const LoLStr LoLCStr;
typedef bool LoLBool;
typedef uint8_t LoLByte;
typedef uint16_t LoLWord;
typedef uint32_t LoLDWord;
typedef uint64_t LoLQWord; 
typedef LoLByte* LoLBytes;
typedef LoLStr* LoLStrArr;
typedef int64_t LoLong;
typedef double LoLFloat;
typedef void LoLVoid;
typedef LoLVoid* LoLPtr;
typedef LoLPtr LoLValue;



#include "llist.h"
#include "lstack.h"
#include "tree.h"
#include "scanner.h"
#include "parser.h"
#include "expr.h"

#include "utils.h"

#endif