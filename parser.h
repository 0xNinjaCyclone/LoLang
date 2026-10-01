#ifndef _LOL_PARSER
#define _LOL_PARSER


#include "lolang.h"

#define PARSER_NEXT(p) \
    lol_parser_next( p, lol_lexer_scan )

#define PARSER_NEXT_PATH(p) \
    lol_parser_next( p, lol_lexer_scanpath );

typedef enum {
    N_PROGRAM,
    N_ASSIGN,        /* x = expr                */
    N_EXPR_STMT,     /* bare expression as stmt */
    N_IF,
    N_FOR,
    N_WHILE,
    N_NEXT,
    N_STOP,
    N_NEED,
    N_FUNC_DEF,
    N_RET,
    N_BLOCK,

    N_LIT_NOTHING,
    N_LIT_INT,
    N_LIT_FLOAT,
    N_LIT_BOOL,
    N_LIT_STRING,
    N_LIT_LIST,
    N_LIT_MAP,

    N_IDENT,
    N_PATH,
    N_BINOP,
    N_UNOP,
    N_CALL,
    N_MEMBER,        /* target.member */
    N_INDEX,         /* target[index] */
    N_MAP_PAIR       /* key: value, used inside N_LIT_MAP */
} LoLNodeKind;

typedef enum {
    OP_NOTHING = 0,
    OP_ASSIGN,
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD, OP_POW,
    OP_ADD_ASSIGN, OP_SUB_ASSIGN, OP_MUL_ASSIGN, OP_DIV_ASSIGN, OP_MOD_ASSIGN,
    OP_SHL_ASSIGN, OP_SHR_ASSIGN, OP_AND_ASSIGN, OP_OR_ASSIGN, OP_XOR_ASSIGN,
    OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE,
    OP_AND, OP_OR,
    OP_NEG, OP_NOT,
    OP_POST_INC, OP_POST_DEC, OP_PRE_INC, OP_PRE_DEC,
    OP_BIT_AND, OP_BIT_OR, OP_BIT_NOT, OP_BIT_XOR,
    OP_BIT_SHL, OP_BIT_SHR
    
} LoLOpKind;

typedef struct _LoLNode {
    LoLNodeKind kind;
    LoLOpKind op_kind;
    LoLValue value;
    LoLDWord dwLine;
    LoLQWord qwPos;
} LoLNode;

typedef struct _LoLParser {
    LoLexer *pLex;
    LoLToken *pCur;
    LoLToken *pPrev;
    LoLBool bErr;
    LoLStr szFName;
    LoLStr sSrc;
    LoLQWord qwSize;
    LoLDWord dwLoopDepth;
    LoLDWord dwFuncDepth;
} LoLParser;



LoLParser *lol_parser(LoLStr szFName);
Tree *lol_parser_launch(LoLParser *pParser);
LoLNode *lol_parser_newnode(LoLToken *pTok, LoLNodeKind kind, LoLOpKind op_kind);
LoLVoid lol_parser_next(LoLParser *pParser, LoLToken *(*scan)(LoLexer*));
LoLVoid lol_parser_skip(LoLParser *pParser);
LoLVoid lol_parser_done(LoLParser **ppParser);

static TreeNode *lol_parser_stmt(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode);

#endif