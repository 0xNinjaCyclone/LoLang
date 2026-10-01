
#include "lolang.h"

static TreeNode *lol_expr_primary(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode = NULL;
    TreeNode *pChild;
    LoLNode *pLoLNode;
    LoLTokenType t;

    t = pParser->pCur->type;

    if ( t == TOK_NOTHING ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, N_LIT_NOTHING, OP_NOTHING );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        return pNode;
    }

    if ( t == TOK_STRING ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, N_LIT_STRING, OP_NOTHING );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        return pNode;
    }

    if ( t == TOK_INT || t == TOK_FLOAT ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, t == TOK_INT ? N_LIT_INT : N_LIT_FLOAT, OP_NOTHING );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        return pNode;
    }

    if ( t == TOK_IDENT ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, N_IDENT, OP_NOTHING );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        return pNode;
    }

    if ( t == TOK_TRUE || t == TOK_FALSE ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, N_LIT_BOOL, OP_NOTHING );
        pLoLNode->value = malloc( sizeof(LoLBool) );
        *( (LoLBool *) pLoLNode->value ) = ( t == TOK_TRUE );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        return pNode;
    }

    if ( t == TOK_LPAREN ) {  
        PARSER_NEXT( pParser );      
        pNode = lol_parser_expr( pParser, pLoLAst, NULL );
        if ( pParser->pCur->type != TOK_RPAREN ) {
            /* Expr must closed with ')' !! */
        }
        PARSER_NEXT( pParser );
        return pNode;
    }

    if ( t == TOK_LBRACKET ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, N_LIT_LIST, OP_NOTHING );
        pNode = tree_node_create( pLoLAst, &pLoLNode );

        if ( pParser->pCur->type != TOK_RBRACKET )
            while ( LOL_TRUE ) {
                lol_parser_skip( pParser );
                lol_parser_expr( pParser, pLoLAst, pNode );
                if ( pParser->pCur->type != TOK_COMMA )
                    break;
                PARSER_NEXT( pParser );
            }

        if ( pParser->pCur->type != TOK_RBRACKET ) {
            /* List should closed with ']' */
        }
        
        PARSER_NEXT( pParser );
        return pNode;
    }

    if ( t == TOK_LBRACE ) {
        PARSER_NEXT( pParser );
        pLoLNode = lol_parser_newnode( pParser->pPrev, N_LIT_MAP, OP_NOTHING );
        pNode = tree_node_create( pLoLAst, &pLoLNode );

        if ( pParser->pCur->type != TOK_RBRACE )
            while ( LOL_TRUE ) {
                lol_parser_skip( pParser );
                if ( !(pParser->pCur->type == TOK_STRING || pParser->pCur->type == TOK_IDENT) ) {
                    /* ?? */
                }

                pLoLNode = lol_parser_newnode( pParser->pCur, N_LIT_STRING, OP_NOTHING );
                pChild = tree_insert( pLoLAst, pNode, &pLoLNode );
                PARSER_NEXT( pParser );
                if ( pParser->pCur->type != TOK_COLON ) {
                    /* !!!! */
                }

                PARSER_NEXT( pParser );
                lol_parser_expr( pParser, pLoLAst, pChild );
                if ( pParser->pCur->type != TOK_COMMA )
                    break;
                    
                PARSER_NEXT( pParser );
            }

        if ( pParser->pCur->type != TOK_RBRACE ) {
            /* Map should closed with '}' */
        }

        PARSER_NEXT( pParser );
        return pNode;
    }

    PARSER_NEXT( pParser );
    pParser->bErr = LOL_TRUE;
    pLoLNode = lol_parser_newnode( pParser->pPrev, N_LIT_NOTHING, OP_NOTHING );
    return tree_node_create( pLoLAst, &pLoLNode );
}

static TreeNode *lol_expr_postfix(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pChild;
    TreeNode *pNode;
    LoLNode *pLoLNode;

    pChild = lol_expr_primary( pParser, pLoLAst );

    while ( LOL_TRUE ) {

        if ( pParser->pCur->type == TOK_LPAREN ) {
            /* function call: ident(args...) */
            pLoLNode = *(LoLNode **) pChild->data;

            /* Is callable expr!? */
            if ( pLoLNode->kind == N_IDENT || pLoLNode->kind == N_MEMBER ||
                pLoLNode->kind == N_INDEX || pLoLNode->kind == N_CALL ) {

                pLoLNode = lol_parser_newnode( pParser->pCur, N_CALL, OP_NOTHING );
                pNode = tree_node_create( pLoLAst, &pLoLNode );
                pChild = tree_insert2( pLoLAst, pNode, pChild );
                PARSER_NEXT( pParser );

                /* Args */
                if ( pParser->pCur->type != TOK_RPAREN )
                    while ( LOL_TRUE ) {
                        lol_parser_expr( pParser, pLoLAst, pNode );
                        if ( pParser->pCur->type != TOK_COMMA )
                            break;
                        PARSER_NEXT( pParser );
                    }
                
                if ( pParser->pCur->type != TOK_RPAREN ) {
                    /* Err: function calls must end with ) */
                }

                pChild = pNode;
                PARSER_NEXT( pParser );

            } else {
                /* implicit multiplication: <expr> (<expr>)  e.g. 4 (a + b) */
                PARSER_NEXT( pParser );
                pLoLNode = lol_parser_newnode( pParser->pPrev, N_BINOP, OP_MUL );
                pNode = tree_node_create( pLoLAst, &pLoLNode );
                tree_insert2( pLoLAst, pNode, pChild );
                lol_parser_expr( pParser, pLoLAst, pNode );
                if ( pParser->pCur->type != TOK_RPAREN ) {
                    /* Err: expected ')' to close parenthesized factor */
                }
                pChild = pNode;
                PARSER_NEXT( pParser );
            } 
        }

        /* Index */
        else if ( pParser->pCur->type == TOK_LBRACKET ) {
            PARSER_NEXT( pParser );
            pLoLNode = lol_parser_newnode( pParser->pPrev, N_INDEX, OP_NOTHING );
            pNode = tree_node_create( pLoLAst, &pLoLNode );
            tree_insert2( pLoLAst, pNode, pChild );
            lol_parser_expr( pParser, pLoLAst, pNode );
            if ( pParser->pCur->type != TOK_RBRACKET ) {
                /* Err: expected ']' to close index */
            }
            pChild = pNode;
            PARSER_NEXT( pParser );
        }

        /* Member: obj.member */
        else if ( pParser->pCur->type == TOK_DOT ) {
            PARSER_NEXT( pParser );
            if ( pParser->pCur->type != TOK_IDENT ) {
                /* Err: WTF!? */
            }

            /* Member Object */
            pLoLNode = lol_parser_newnode( pParser->pCur, N_MEMBER, OP_NOTHING );
            pNode = tree_node_create( pLoLAst, &pLoLNode );
            tree_insert2( pLoLAst, pNode, pChild );
            /* Member */
            pLoLNode = lol_parser_newnode( pParser->pCur, N_IDENT, OP_NOTHING );
            pChild = tree_node_create( pLoLAst, &pLoLNode );
            tree_insert2( pLoLAst, pNode, pChild );
            pChild = pNode;
            PARSER_NEXT( pParser );
        }

        else if ( pParser->pCur->type == TOK_INC || pParser->pCur->type == TOK_DEC ) {
            pLoLNode = *(LoLNode **) pChild->data;
            if ( pLoLNode->kind != N_IDENT && pLoLNode->kind != N_MEMBER && pLoLNode->kind != N_INDEX ) {
    
                /* ++/-- requires something assignable. */
            }

            pLoLNode = lol_parser_newnode( pParser->pCur, N_UNOP, (pParser->pCur->type == TOK_INC) ? OP_POST_INC : OP_POST_DEC ); 
            pNode = tree_node_create( pLoLAst, &pLoLNode );
            tree_insert2( pLoLAst, pNode, pChild );
            pChild = pNode;
            PARSER_NEXT( pParser );
        }

        else 
            break;
    }

    return pChild;
}

static TreeNode *lol_expr_unary(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pChild;
    LoLNode *pLoLNode;
    LoLTokenType t;
    LoLOpKind op;

    t = pParser->pCur->type;

    if ( t == TOK_MINUS || t == TOK_NOT || t == TOK_BIT_NOT || t == TOK_DEC || t == TOK_INC ) {
        if ( t == TOK_DEC || t == TOK_INC )
            op = ( t == TOK_DEC ) ? OP_PRE_DEC : OP_PRE_INC;
        else
            op = ( t == TOK_MINUS ) ? OP_NEG : t == TOK_NOT ? OP_NOT : OP_BIT_NOT;
        pLoLNode = lol_parser_newnode( pParser->pCur, N_UNOP, op );
        PARSER_NEXT( pParser );
        pChild = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pChild, lol_expr_unary(pParser, pLoLAst) );
        return pChild;
    }

    return lol_expr_postfix( pParser, pLoLAst );
}

static TreeNode *lol_expr_pow(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;

    pLeft = lol_expr_unary( pParser, pLoLAst );

    if ( pParser->pCur->type == TOK_POW ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, OP_POW );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_pow( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
    }

    return pLeft;
}

static TreeNode *lol_expr_term(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;
    LoLTokenType t;

    pLeft = lol_expr_pow( pParser, pLoLAst );
    t = pParser->pCur->type;

    while ( t == TOK_STAR || t == TOK_SLASH || t == TOK_PERCENT ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, (t == TOK_STAR) ? OP_MUL : (t == TOK_SLASH) ? OP_DIV : OP_MOD );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_pow( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
        t = pParser->pCur->type;
    }

    return pLeft;
}

static TreeNode *lol_expr_arithmetic(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;
    LoLTokenType t;

    pLeft = lol_expr_term( pParser, pLoLAst );
    t = pParser->pCur->type;

    while ( t == TOK_MINUS || t == TOK_PLUS ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, (t == TOK_PLUS) ? OP_ADD : OP_SUB );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_term( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
        t = pParser->pCur->type;
    }

    return pLeft;
}

static TreeNode *lol_expr_b_shift(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;
    LoLTokenType t;

    pLeft = lol_expr_arithmetic( pParser, pLoLAst );
    t = pParser->pCur->type;

    while ( t == TOK_BIT_SHL || t == TOK_BIT_SHR ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, (t == TOK_BIT_SHL) ? OP_BIT_SHL : OP_BIT_SHR );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_arithmetic( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
        t = pParser->pCur->type;
    }

    return pLeft;
}

static TreeNode *lol_expr_b_and(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;

    pLeft = lol_expr_b_shift( pParser, pLoLAst );

    while ( pParser->pCur->type == TOK_BIT_AND ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, OP_BIT_AND );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_b_shift( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
    }

    return pLeft;
}

static TreeNode *lol_expr_b_orxor(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;
    LoLTokenType t;

    pLeft = lol_expr_b_and( pParser, pLoLAst );
    t = pParser->pCur->type;

    while ( t == TOK_BIT_OR || t == TOK_BIT_XOR ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, (t == TOK_BIT_OR) ? OP_BIT_OR : OP_BIT_XOR );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_b_and( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
        t = pParser->pCur->type;
    }

    return pLeft;
}

static TreeNode *lol_expr_relational(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;
    LoLTokenType t;
    LoLOpKind k;

    pLeft = lol_expr_b_orxor( pParser, pLoLAst );
    t = pParser->pCur->type;

    while ( t == TOK_LT || t == TOK_LE || t == TOK_GT || t == TOK_GE ||
            t == TOK_KW_LT || t == TOK_KW_LE || t == TOK_KW_GT || t == TOK_KW_GE ) {

        if ( t == TOK_LT || t == TOK_KW_LT )
            k = OP_LT;
        else if ( t == TOK_LE || t == TOK_KW_LE )
            k = OP_LE;
        else if ( t == TOK_GT || t == TOK_KW_GT )
            k = OP_GT;
        else
            k = OP_GE;

        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, k );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_b_orxor( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
        t = pParser->pCur->type;
    }

    return pLeft;
}

static TreeNode *lol_expr_eq(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;
    LoLTokenType t;

    pLeft = lol_expr_relational( pParser, pLoLAst );
    t = pParser->pCur->type;

    while ( t == TOK_EQ || t == TOK_NEQ || t == TOK_KW_EQ || t == TOK_KW_NE ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, (t == TOK_EQ || t == TOK_KW_EQ) ? OP_EQ : OP_NE );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_relational( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
        t = pParser->pCur->type;

    }

    return pLeft;
}

static TreeNode *lol_expr_and(LoLParser *pParser, Tree *pLoLAst) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;

    pLeft = lol_expr_eq( pParser, pLoLAst );

    while ( pParser->pCur->type == TOK_AND ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, OP_AND );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_eq( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
    }

    return pLeft;
}

TreeNode *lol_parser_expr(LoLParser *pParser, Tree *pLoLAst, TreeNode *pAstNode) {
    TreeNode *pNode;
    TreeNode *pLeft;
    TreeNode *pRight;
    LoLNode *pLoLNode;

    pLeft = lol_expr_and( pParser, pLoLAst );

    while ( pParser->pCur->type == TOK_OR ) {
        pLoLNode = lol_parser_newnode( pParser->pCur, N_BINOP, OP_OR );
        PARSER_NEXT( pParser );
        pNode = tree_node_create( pLoLAst, &pLoLNode );
        tree_insert2( pLoLAst, pNode, pLeft );
        pRight = lol_expr_and( pParser, pLoLAst );
        tree_insert2( pLoLAst, pNode, pRight );
        pLeft = pNode;
    }

    if ( pAstNode )
        pLeft = tree_insert2( pLoLAst, pAstNode, pLeft );

    return pLeft;
}