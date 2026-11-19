#ifndef _LOL_EXPR
#define _LOL_EXPR

#include "lolang.h"

#define LOL_MAX_EXPR_DEPTH 128

TreeNode *lol_parser_expr(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode);

#endif