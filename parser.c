
#include "lolang.h"


static LoLVoid lol_parser_printnode(LoLPtr ppNode) {
    LoLNode *pLoLNode;
    LoLChr cBuff[32];

    pLoLNode = *(LoLNode **) ppNode;
    
    // printf( "kind = %d, op = %d, line = %d, pos = %lu, val = %s", 
    //     pLoLNode->kind, pLoLNode->op_kind, pLoLNode->dwLine, pLoLNode->qwPos,
    //     pLoLNode->value ? (LoLStr) pLoLNode->value : "" // A fuckin bad thing, just for test
    // );
    snprintf( cBuff, sizeof(cBuff), "L%d:P%lld", pLoLNode->dwLine, pLoLNode->qwPos );
    printf( "%-15s", cBuff );

    switch ( pLoLNode->kind ) {
        case N_PROGRAM:
            printf( "Program" );
            break;
        case N_BLOCK:
            printf( "Block" );
            break;
        case N_ASSIGN:
            printf( "Assign (%s)", op_name(pLoLNode->op_kind) );
            break;
        case N_EXPR_STMT:
            printf( "ExprStmt" );
            break;
        case N_IF:
            printf( "If" );
            break;
        case N_NEXT:
            printf( "Next" );
            break;
        case N_STOP:
            printf( "Stop" );
            break;
        case N_FOR:
            printf( "For" );
            break;
        case N_WHILE:
            printf( "While" );
            break;
        case N_NEED:
            printf( "Need <%s>", (LoLStr) pLoLNode->value );
            break;
        case N_FUNC_DEF:
            printf( "FuncDef %s", (LoLStr) pLoLNode->value );
            break;
        case N_RET:
            printf( "RET" );
            break;
        case N_LIT_NOTHING:
            printf( "Nothing" );
            break;
        case N_LIT_BOOL:
            printf( "Bool %s", (*(LoLBool *) pLoLNode->value) ? "True" : "False" );
            break;
        case N_LIT_INT:
            printf( "Int %lld", *(LoLQWord *) pLoLNode->value );
            break;
        case N_LIT_FLOAT:
            printf( "Float %0.2f", (double)*(LoLQWord *) pLoLNode->value );
            break;
        case N_LIT_STRING:
            printf( "String \"%s\"", (LoLStr) pLoLNode->value );
            break;
        case N_LIT_LIST:
            printf( "List" );
            break;
        case N_LIT_MAP:
            printf( "Map" );
            break;
        case N_MAP_PAIR:
            printf( "Pair" );
            break;
        case N_IDENT:
            printf( "Ident %s", (LoLStr) pLoLNode->value );
            break;
        case N_BINOP:
            printf( "BinOp %s", op_name(pLoLNode->op_kind) );
            break;
        case N_UNOP:
            printf( "UnOp %s", op_name(pLoLNode->op_kind) );
            break;
        case N_CALL:
            printf( "Call" );
            break;
        case N_INDEX:
            printf( "Index" );
            break;
        case N_PATH:
            printf( "Path %s", (LoLStr) pLoLNode->value );
            break;
        case N_MEMBER:
            printf( "Member %s", (LoLStr) pLoLNode->value );
            break;
        default:
            printf( "<Unknown node %d>", pLoLNode->kind );
    }
}

static LoLVoid lol_parser_freenode(LoLPtr ppNode) {
    if ( (*(LoLNode **) ppNode)->value )
        free( (*(LoLNode **) ppNode)->value );
        
    free( *(LoLPtr **) ppNode );
    free( ppNode );
}

static LoLBool lol_parser_expect(LoLParser *pParser, LoLTokenType t, LoLCStr szErr) {
    if ( pParser->pCur->type != t ) {
        /* TODO Later */
        return LOL_FALSE;
    }

    PARSER_NEXT( pParser );
    return LOL_TRUE;
}

static TreeNode *lol_parser_block(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    LoLNode *pLoLNode;

    lol_parser_expect( pParser, TOK_LBRACE, "expected '{' to start a block" );
    pLoLNode = lol_parser_newnode( pParser->pPrev, N_BLOCK, OP_NOTHING );
    pNode = tree_insert( pLoLAst, pAstNode, &pLoLNode );

    while ( pParser->pCur->type != TOK_RBRACE && pParser->pCur->type != TOK_EOF ) {
        lol_parser_skip( pParser );
        if ( pParser->pCur->type == TOK_RBRACE || pParser->pCur->type == TOK_EOF )
            break;
        lol_parser_stmt( pParser, pLoLAst, pNode );
    }

    lol_parser_expect( pParser, TOK_RBRACE, "expected '}' to close block" );
    return pNode;
}

static TreeNode *lol_parser_loopctrl(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    LoLNode *pLoLNode;

    PARSER_NEXT( pParser );

    if ( pParser->pCur->type != TOK_NEWLINE ) {
        /* control statements should be ends the block */
    }

    pLoLNode = lol_parser_newnode( pParser->pPrev, pParser->pPrev->type == TOK_NEXT ? N_NEXT : N_STOP, OP_NOTHING );
    return tree_insert( pLoLAst, pAstNode, &pLoLNode );
}

static TreeNode *lol_parser_if(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    LoLNode *pLoLNode;

    PARSER_NEXT( pParser );
    pLoLNode = lol_parser_newnode( pParser->pPrev, N_IF, OP_NOTHING );
    pNode = tree_node_create( pLoLAst, &pLoLNode );
    lol_parser_expr( pParser, pLoLAst, pNode );
    lol_parser_block( pParser, pLoLAst, pNode );
    lol_parser_skip( pParser );
    /* Check else and else if */
    if ( pParser->pCur->type == TOK_ELSE ) {
        PARSER_NEXT( pParser );
        if ( pParser->pCur->type == TOK_LBRACE )
            lol_parser_block( pParser, pLoLAst, pNode );

        else if ( pParser->pCur->type == TOK_IF )
            lol_parser_if( pParser, pLoLAst, pNode );

        else {
            /* ?? */
        }
    }

    return tree_insert2( pLoLAst, pAstNode, pNode );
}

static TreeNode *lol_parser_for(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    TreeNode *pChild;
    LoLNode *pLoLNode;

    PARSER_NEXT( pParser );

    if ( pParser->pCur->type != TOK_LPAREN )
        return NULL;

    PARSER_NEXT( pParser );    
    if ( pParser->pCur->type != TOK_IDENT ) {
        /* Loop var name? */
        return NULL;
    }
    pLoLNode = lol_parser_newnode( pParser->pPrev, N_FOR, OP_NOTHING );
    pNode = tree_node_create( pLoLAst, &pLoLNode );
    pLoLNode = lol_parser_newnode( pParser->pCur, N_IDENT, OP_NOTHING );
    tree_insert( pLoLAst, pNode, &pLoLNode );
    PARSER_NEXT( pParser );

    if ( !lol_parser_expect(pParser, TOK_IN, "in keyword expected!") ) {
        /* We've to do somthing here! */
        return NULL;
    }

    lol_parser_expr( pParser, pLoLAst, pNode );

    if ( !lol_parser_expect(pParser, TOK_RPAREN, "')' expected befor for block") ) {
        /* We've to do somthing here! */
        return NULL;
    }

    if ( pParser->pCur->type != TOK_LBRACE ) {
        /* Where is the block? */
        return NULL;
    }

    pParser->dwLoopDepth++;
    lol_parser_block( pParser, pLoLAst, pNode );
    pParser->dwLoopDepth--;
    return tree_insert2( pLoLAst, pAstNode, pNode );
}

static TreeNode *lol_parser_while(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    LoLNode *pLoLNode;

    PARSER_NEXT( pParser );

    if ( pParser->pCur->type != TOK_LPAREN )
        return NULL;

    pLoLNode = lol_parser_newnode( pParser->pPrev, N_WHILE, OP_NOTHING );
    pNode = tree_node_create( pLoLAst, &pLoLNode );
    lol_parser_expr( pParser, pLoLAst, pNode );

    if ( pParser->pCur->type != TOK_LBRACE )
        return NULL;

    pParser->dwLoopDepth++;
    lol_parser_block( pParser, pLoLAst, pNode );
    pParser->dwLoopDepth--;
    return tree_insert2( pLoLAst, pAstNode, pNode );
}

static TreeNode *lol_parser_need(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pChild;
    LoLNode *pNode;

    PARSER_NEXT_PATH( pParser );

    if ( pParser->pCur->type != TOK_PATH ) // WTF??
        return NULL;

    pNode = lol_parser_newnode( pParser->pCur, N_NEED, OP_NOTHING );
    if ( !(pChild = tree_insert(pLoLAst, pAstNode, &pNode)) )
        return NULL;

    pNode = lol_parser_newnode( pParser->pCur, N_PATH, OP_NOTHING );
    if ( !tree_insert(pLoLAst, pChild, &pNode) )
        return NULL;

    PARSER_NEXT( pParser );
    return pChild;
}

static TreeNode *lol_parser_ret(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    LoLNode *pLoLNode;

    pLoLNode = lol_parser_newnode( pParser->pCur, N_RET, OP_NOTHING );
    pNode = tree_insert( pLoLAst, pAstNode, &pLoLNode );
    PARSER_NEXT( pParser );
    if ( !(pParser->pCur->type == TOK_EOF || pParser->pCur->type == TOK_NEWLINE || pParser->pCur->type == TOK_RBRACE) )
        lol_parser_expr( pParser, pLoLAst, pNode );
    return pNode;
}

static TreeNode *lol_parser_define(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    TreeNode *pChild;
    LoLNode *pLoLNode;

    PARSER_NEXT( pParser );
    if ( pParser->pCur->type != TOK_IDENT ) {
        /* ????? */
    }

    pLoLNode = lol_parser_newnode( pParser->pPrev, N_FUNC_DEF, OP_NOTHING );
    pChild = tree_insert( pLoLAst, pAstNode, &pLoLNode );
    pLoLNode = lol_parser_newnode( pParser->pCur, N_IDENT, OP_NOTHING );
    tree_insert( pLoLAst, pChild, &pLoLNode );
    PARSER_NEXT( pParser );
    lol_parser_expect( pParser, TOK_LPAREN, "Func should has (), even if takes no params" );

    if ( pParser->pCur->type != TOK_RPAREN )
        while ( LOL_TRUE ) {
            if ( pParser->pCur->type != TOK_IDENT ) {
                /* Err: expect param name */
            }
            pLoLNode = lol_parser_newnode( pParser->pCur, N_IDENT, OP_NOTHING );
            tree_insert( pLoLAst, pChild, &pLoLNode );
            PARSER_NEXT( pParser );
            if ( pParser->pCur->type != TOK_COMMA )
                break;
            PARSER_NEXT( pParser );
        }
    
    lol_parser_expect( pParser, TOK_RPAREN, "expected ')' after params" );
    pParser->dwFuncDepth++;
    lol_parser_block( pParser, pLoLAst, pChild );
    pParser->dwFuncDepth--;
    return pChild;
}

static TreeNode *lol_parser_stmt(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {

    TreeNode *pNode;
    TreeNode *pChild;
    LoLNode *pLoLNode;
    LoLOpKind op;
    
    switch ( pParser->pCur->type ) {
    case TOK_STOP:
    case TOK_NEXT:
        if ( !pParser->dwLoopDepth ) {
            /* stop/next outside a loop!? */
        }
        return lol_parser_loopctrl( pParser, pLoLAst, pAstNode );

    case TOK_IF:
        return lol_parser_if( pParser, pLoLAst, pAstNode );
    
    case TOK_FOR:
        return lol_parser_for( pParser, pLoLAst, pAstNode );

    case TOK_WHILE:
        return lol_parser_while( pParser, pLoLAst, pAstNode );

    case TOK_NEED:
        return lol_parser_need( pParser, pLoLAst, pAstNode );

    case TOK_DEFINE:
        return lol_parser_define( pParser, pLoLAst, pAstNode );

    case TOK_RET:
        if ( !pParser->dwFuncDepth ) {
            /* ret outside a func!? */
        }

        return lol_parser_ret( pParser, pLoLAst, pAstNode );

    default:
        break;
    }

    if ( !(pNode = lol_parser_expr(pParser, pLoLAst, NULL)) )
        return NULL;
        
    // if ( pParser->pCur->type == TOK_ASSIGN ) {
    //     PARSER_NEXT( pParser );

    //     if ( (*(LoLNode **) pNode->data)->kind != N_IDENT )
    //         return NULL;

    //     pLoLNode = lol_parser_newnode( pParser->pPrev, N_ASSIGN, OP_NOTHING );
    //     pChild = tree_node_create( pLoLAst, &pLoLNode );
    //     tree_insert2( pLoLAst, pChild, pNode );
    //     lol_parser_expr( pParser, pLoLAst, pChild );
    //     pNode = pChild;
    // }

    switch (pParser->pCur->type) {

    case TOK_ASSIGN:
        op = OP_ASSIGN;
        break;

    case TOK_ADD_ASSIGN:
        op = OP_ADD_ASSIGN;
        break;

    case TOK_SUB_ASSIGN:
        op = OP_SUB_ASSIGN;
        break;

    case TOK_MUL_ASSIGN:
        op = OP_MUL_ASSIGN;
        break;

    case TOK_DIV_ASSIGN:
        op = OP_DIV_ASSIGN;
        break;

    case TOK_MOD_ASSIGN:
        op = OP_MOD_ASSIGN;
        break;

    case TOK_SHL_ASSIGN:
        op = OP_SHL_ASSIGN;
        break;

    case TOK_SHR_ASSIGN:
        op = OP_SHR_ASSIGN;
        break;

    case TOK_AND_ASSIGN:
        op = OP_AND_ASSIGN;
        break;

    case TOK_OR_ASSIGN:
        op = OP_OR_ASSIGN;
        break;

    case TOK_XOR_ASSIGN:
        op = OP_XOR_ASSIGN;
        break;

    default:
        return pNode;
    }

    pLoLNode = *(LoLNode **) pNode->data;

    if (pLoLNode->kind != N_IDENT &&
        pLoLNode->kind != N_MEMBER &&
        pLoLNode->kind != N_INDEX) {

        pParser->bErr = LOL_TRUE;
        return NULL;
    }

    /*
     * Consume assignment operator.
     */
    PARSER_NEXT(pParser);

    pLoLNode = lol_parser_newnode(
        pParser->pPrev,
        N_ASSIGN,
        op
    );

    pChild = tree_node_create(pLoLAst, &pLoLNode);

    if (!pChild) {
        pParser->bErr = LOL_TRUE;
        return NULL;
    }

    tree_insert2(pLoLAst, pChild, pNode);

    if (!lol_parser_expr(pParser, pLoLAst, pChild)) {
        pParser->bErr = LOL_TRUE;
        return NULL;
    }

    return tree_insert2( pLoLAst, pAstNode, pChild );
}

LoLParser *lol_parser(LoLStr szFName) {
    LoLParser *pParser;

    if ( pParser = (LoLParser *) malloc(sizeof(LoLParser)) ) {
        pParser->dwLoopDepth = 0;
        pParser->dwFuncDepth = 0;
        pParser->pPrev = LOL_NULL;
        pParser->bErr = LOL_FALSE;
        pParser->szFName = szFName;
        pParser->sSrc = lol_readfile( pParser->szFName, &pParser->qwSize );
        pParser->pLex = lol_lexer( pParser->sSrc, pParser->qwSize );
        pParser->pCur = lol_lexer_scan( pParser->pLex );
    }

    return pParser;
}

Tree *lol_parser_launch(LoLParser *pParser) {
    Tree *pLoLAst;
    LoLNode *pLoLNode;
    TreeNode *pNode;

    if ( !(pLoLAst = tree_init(sizeof(LoLNode *), malloc, lol_parser_freenode, lol_parser_printnode, LOL_NULL)) )
        return LOL_NULL;

    pLoLNode = lol_parser_newnode( pParser->pCur, N_PROGRAM, OP_NOTHING );
    pLoLNode->dwLine = pLoLNode->qwPos = 0;
    pLoLNode->value = strdup( "LoLProgram" );
    pNode = tree_insert( pLoLAst, LOL_NULL, &pLoLNode );

    while ( pParser->pCur->type != TOK_EOF ) {
        lol_parser_skip( pParser );
        if ( pParser->pCur->type == TOK_EOF )
            break;
        
        if ( !lol_parser_stmt(pParser, pLoLAst, pNode) && pParser->pCur->type == TOK_ERROR )
            return NULL;
        
    }

    return pLoLAst;
}

LoLNode *lol_parser_newnode(LoLToken *pTok, LoLNodeKind kind, LoLOpKind op_kind) {
    LoLNode *pNode; 

    if ( pNode = (LoLNode *) malloc(sizeof(LoLNode)) ) {
        pNode->kind = kind;
        pNode->op_kind = op_kind;
        pNode->dwLine = pTok->dwLine;
        pNode->qwPos = pTok->qwPos;
        pNode->value = pTok->value;
    }

    return pNode;
}

LoLVoid lol_parser_next(LoLParser *pParser, LoLToken *(*scan)(LoLexer*)) {
    lol_lexer_dntok( &pParser->pPrev );
    pParser->pPrev = pParser->pCur;
    pParser->pCur = scan( pParser->pLex );
}

LoLVoid lol_parser_skip(LoLParser *pParser) {
    while ( pParser->pCur->type == TOK_NEWLINE )
        PARSER_NEXT( pParser );
}

LoLVoid lol_parser_done(LoLParser **ppParser) {
    lol_lexer_done( (LoLexer **) ppParser );
    free( (*ppParser)->sSrc );
    free( *ppParser );
    ( *ppParser ) = LOL_NULL;
}