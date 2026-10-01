
#include "lolang.h"

LoLCStr lol_readfile(LoLCStr szFName, LoLQWord *pqwSize) {
    LoLStr szBuf;
    FILE *fp;
    LoLQWord qwSize;

    if ( !(fp = fopen(szFName, "rb")) )
        return LOL_NULL;

    fseek( fp, 0L, SEEK_END );
    qwSize = ftell( fp );
    fseek( fp, 0L, SEEK_SET );

    if ( szBuf = (LoLStr) malloc(qwSize) )
        fread( szBuf, qwSize, 1, fp );

    fclose( fp );
    
    if ( pqwSize )
        ( *pqwSize ) = qwSize;
        
    return szBuf;
}

LoLCStr op_name(LoLOpKind op) {
    switch (op) {
        case OP_ASSIGN: return "=";
        case OP_ADD_ASSIGN: return "+=";
        case OP_SUB_ASSIGN: return "-=";
        case OP_MUL_ASSIGN: return "*=";
        case OP_DIV_ASSIGN: return "/=";
        case OP_MOD_ASSIGN: return "%=";
        case OP_SHR_ASSIGN: return ">>=";
        case OP_SHL_ASSIGN: return "<<=";
        case OP_AND_ASSIGN: return "&=";
        case OP_XOR_ASSIGN: return "^=";
        case OP_OR_ASSIGN: return "|=";
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_POW: return "**";
        case OP_EQ:  return "==";
        case OP_NE:  return "!=";
        case OP_LT:  return "<";
        case OP_LE:  return "<=";
        case OP_GT:  return ">";
        case OP_GE:  return ">=";
        case OP_AND: return "and";
        case OP_OR:  return "or";
        case OP_NEG: return "unary-";
        case OP_NOT: return "not";
        case OP_POST_INC: return "()++";
        case OP_POST_DEC: return "()--";
        case OP_PRE_INC: return "++()";
        case OP_PRE_DEC: return "--()";
        case OP_BIT_AND: return "&";
        case OP_BIT_NOT: return "~";
        case OP_BIT_OR: return "|";
        case OP_BIT_SHL: return "<<";
        case OP_BIT_SHR: return ">>";
        case OP_BIT_XOR: return "^";
    }
    return "?";
}