
#include "lolang.h"

LoLKeyword kwds[] = {
    { "nothing", TOK_NOTHING },
    { "if",      TOK_IF },
    { "else",    TOK_ELSE },
    { "for",     TOK_FOR },
    { "in",      TOK_IN },
    { "while",   TOK_WHILE },
    { "need",    TOK_NEED },
    { "define",  TOK_DEFINE },
    { "return",  TOK_RET },
    { "stop",    TOK_STOP },
    { "next",    TOK_NEXT },
    { "true",    TOK_TRUE},
    { "false",   TOK_FALSE},
    { "and",     TOK_AND },
    { "or",      TOK_OR },
    { "eq",      TOK_KW_EQ },
    { "ne",      TOK_KW_NE },
    { "lt",      TOK_KW_LT },
    { "le",      TOK_KW_LE },
    { "gt",      TOK_KW_GT },
    { "ge",      TOK_KW_GE },
    { LOL_NULL, TOK_EOF }
};

static LoLVoid lol_lexer_walk(LoLexer *pLexer) {
    LoLChr c;

    if ( LEXER_CANREAD(pLexer) ) {
        c = LEXER_READ( pLexer );
        if ( c == LEXER_LF )
            pLexer->dwLine++;
        pLexer->qwPos++;
        pLexer->sBuff++;
    }
}

static LoLVoid lol_lexer_skip(LoLexer *pLexer) {
    LoLBool b = LOL_FALSE;
    LoLChr c;

    while ( LEXER_CANREAD(pLexer) && (LEXER_ISWS(pLexer) || (b = LEXER_ISCOMMENT(pLexer))) ) {
        lol_lexer_walk( pLexer );

        if ( b ) {
            while ( LEXER_CANREAD(pLexer) && ((c = LEXER_READ(pLexer)) && c != LEXER_LF) )
                lol_lexer_walk( pLexer );

            b = LOL_FALSE;
        }
    }
}

static LoLToken *lol_lexer_mktok(LoLexer *pLexer, LoLTokenType tok_type, LoLValue value, LoLDWord dwLine, LoLQWord qwPos) {
    LoLToken *pTok;

    if ( pTok = (LoLToken *) malloc(sizeof(LoLToken)) ) {
        pTok->type = tok_type;
        pTok->value = value;
        pTok->dwLine = dwLine;
        pTok->qwPos = qwPos;
    }

    return pTok;
}

static LoLToken *lol_lexer_identifier(LoLexer *pLexer, LoLDWord dwLine, LoLQWord qwPos) {
    LoLStr szKwd;
    LoLStr szPtr;
    LoLWord wLen;
    LoLChr c;

    szPtr = pLexer->sBuff;

    while ( LEXER_CANREAD(pLexer) && (isalnum(c = LEXER_READ(pLexer)) || c == LEXER_USCORE) ) 
        lol_lexer_walk( pLexer );

    wLen = pLexer->qwPos - qwPos;
    
    if ( wLen > LEXER_MAX_IDENTIFIER_LEN )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLValue) "Too Long Identifier", dwLine, qwPos );
    
    if ( !(szKwd = (LoLStr) malloc(wLen + 1)) )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLValue) "Out Of Memory", dwLine, qwPos );

    memcpy( szKwd, szPtr, wLen );
    *( szKwd + wLen ) = LEXER_NUL;
    
    for ( LoLKeyword *pKwd = kwds; pKwd->szWord; pKwd++ ) 
        if ( wLen == strlen(pKwd->szWord) && strncmp(pKwd->szWord, szKwd, wLen) == 0 )
            return lol_lexer_mktok( pLexer, pKwd->tok_type, (LoLValue) szKwd, dwLine, qwPos);

    return lol_lexer_mktok( pLexer, TOK_IDENT, (LoLValue) szKwd, dwLine, qwPos );
}

static LoLToken *lol_lexer_string(LoLexer *pLexer, LoLDWord dwLine, LoLQWord qwPos) {
    LoLStr szStr;
    LoLStr szPtr;
    LoLValue value;
    LoLQWord qwStrLen;
    LoLChr c, q;

    q = LEXER_READ( pLexer );

    if ( !(q == LEXER_DQUOTE || q == LEXER_SQUOTE) )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLPtr) "Invalid string (no openning quote)", dwLine, qwPos );

    lol_lexer_walk( pLexer );
    szPtr = pLexer->sBuff;
    qwStrLen = 0;

    while ( LEXER_CANREAD(pLexer) && (c = LEXER_READ(pLexer)) != q ) {
        if ( c == LEXER_BACKSLASH && (pLexer->qwSize - pLexer->qwPos) > 2 )
            lol_lexer_walk( pLexer );

        qwStrLen++;
        lol_lexer_walk( pLexer );
    }

    if ( c != q )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLPtr) "Invalid string (no closing quote)", dwLine, qwPos );

    if ( !(szStr = (LoLStr) malloc(qwStrLen + 1)) )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLPtr) "Out Of Memory", dwLine, qwPos );

    value = (LoLValue) szStr;

    while ( szPtr < pLexer->sBuff ) {
        if ( *szPtr == LEXER_BACKSLASH ) {
            c = *( ++szPtr );

            switch ( c ) {
                case 'b':
                    *szStr++ = LEXER_BACKSPACE;
                    break;
                case 'f':
                    *szStr++ = LEXER_FORMAT;
                    break;
                case 'n':
                    *szStr++ = LEXER_LF;
                    break;
                case 'r':
                    *szStr++ = LEXER_CR;
                    break;
                case 't':
                    *szStr++ = LEXER_TAB;
                    break;
                case LEXER_DQUOTE:
                case LEXER_BACKSLASH:
                case LEXER_SLASH:
                    *szStr++ = c;
                    break;
            }
        }

        else {
            *szStr++ = *szPtr;
        }

        szPtr++;
    }

    *szStr = LEXER_NUL;

    // Skip closing quote
    lol_lexer_walk( pLexer );

    return lol_lexer_mktok( pLexer, TOK_STRING, value, dwLine, qwPos );
}


static LoLToken *lol_lexer_number(LoLexer *pLexer, LoLDWord dwLine, LoLQWord qwPos) {
    LoLStr szNum;
    LoLStr szPtr;
    LoLQWord value;
    LoLQWord qwLen = LOL_ZERO;
    LoLBool bFloat = LOL_FALSE;
    LoLChr c;

    szPtr = pLexer->sBuff;

    while ( LEXER_CANREAD(pLexer) && (c = LEXER_READ(pLexer)) && (isdigit(c) || c == LEXER_DOT) ) {
        if ( c == LEXER_DOT )
            bFloat = LOL_TRUE;

        qwLen++;
        lol_lexer_walk( pLexer );
    }

    if ( !(szNum = (LoLStr) malloc(qwLen + 1)) )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLValue) "Out Of Memory", dwLine, qwPos );

    memcpy( szNum, szPtr, qwLen );
    *( szNum + qwLen ) = LEXER_NUL;

    if ( !(value = (LoLQWord) malloc(sizeof(LoLQWord))) )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLValue) "Out Of Memory", dwLine, qwPos );

    *( (LoLQWord *) value ) = atof( szNum );
    return lol_lexer_mktok( pLexer, bFloat ? TOK_FLOAT : TOK_INT, (LoLValue) value, dwLine, qwPos );
}


LoLexer *lol_lexer(LoLStr sLoLSrc, LoLQWord qwSrcSize) {
    LoLexer *pLex;

    if ( pLex = (LoLexer *) malloc(sizeof(LoLexer)) ) {
        pLex->qwPos = 0;
        pLex->dwLine = 1;
        pLex->sBuff = sLoLSrc;
        pLex->qwSize = qwSrcSize;
    }

    return pLex;
}

LoLToken *lol_lexer_scan(LoLexer *pLexer) {
    LoLDWord dwLine;
    LoLQWord qwPos;
    LoLChr c;
    
    lol_lexer_skip( pLexer );

    if ( !LEXER_CANREAD(pLexer) )
        return lol_lexer_mktok( pLexer, TOK_EOF, LOL_NULL, pLexer->dwLine, pLexer->qwPos );

    c = LEXER_READ( pLexer );

    if ( isalpha(c) || c == LEXER_USCORE )
        return lol_lexer_identifier( pLexer, pLexer->dwLine, pLexer->qwPos );

    else if ( isdigit(c) )
        return lol_lexer_number( pLexer, pLexer->dwLine, pLexer->qwPos );

    else if ( c == LEXER_DQUOTE || c == LEXER_SQUOTE )
        return lol_lexer_string( pLexer, pLexer->dwLine, pLexer->qwPos );

    else {
        dwLine = pLexer->dwLine;
        qwPos = pLexer->qwPos;
        lol_lexer_walk( pLexer );

        switch ( c ) {

        case LEXER_SEMICOLON:
        case LEXER_LF:
            return lol_lexer_mktok( pLexer, TOK_NEWLINE, LOL_NULL, dwLine, qwPos );

        case LEXER_SLASH:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_DIV_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_SLASH, LOL_NULL, dwLine, qwPos );

        case LEXER_PERCENT:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_MOD_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_PERCENT, LOL_NULL, dwLine, qwPos );

        case LEXER_COMMA:
            return lol_lexer_mktok( pLexer, TOK_COMMA, LOL_NULL, dwLine, qwPos );

        case LEXER_COLON:
            return lol_lexer_mktok( pLexer, TOK_COLON, LOL_NULL, dwLine, qwPos );

        case LEXER_NEGSIGN:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_NEGSIGN ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_DEC, LOL_NULL, dwLine, qwPos );
            } 

            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_SUB_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_MINUS, LOL_NULL, dwLine, qwPos );

        case LEXER_POSSIGN:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_POSSIGN ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_INC, LOL_NULL, dwLine, qwPos );
            } 

            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_ADD_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_PLUS, LOL_NULL, dwLine, qwPos );

        case LEXER_LPAREN:
            return lol_lexer_mktok( pLexer, TOK_LPAREN, LOL_NULL, dwLine, qwPos );

        case LEXER_RPAREN:
            return lol_lexer_mktok( pLexer, TOK_RPAREN, LOL_NULL, dwLine, qwPos );

        case LEXER_OPENBRACKET:
            return lol_lexer_mktok( pLexer, TOK_LBRACKET, LOL_NULL, dwLine, qwPos );

        case LEXER_CLOSEBRACKET:
            return lol_lexer_mktok( pLexer, TOK_RBRACKET, LOL_NULL, dwLine, qwPos );

        case LEXER_OPENCBRACKET:
            return lol_lexer_mktok( pLexer, TOK_LBRACE, LOL_NULL, dwLine, qwPos );

        case LEXER_CLOSECBRACKET:
            return lol_lexer_mktok( pLexer, TOK_RBRACE, LOL_NULL, dwLine, qwPos );

        case LEXER_DOT:
            return lol_lexer_mktok( pLexer, TOK_DOT, LOL_NULL, dwLine, qwPos );

        case LEXER_PIPE:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_OR_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_BIT_OR, LOL_NULL, dwLine, qwPos );

        case LEXER_CARET:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_XOR_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_BIT_XOR, LOL_NULL, dwLine, qwPos );

        case LEXER_TILDE:
            return lol_lexer_mktok( pLexer, TOK_BIT_NOT, LOL_NULL, dwLine, qwPos );

        case LEXER_AMPERSAND:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_AND_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_BIT_AND, LOL_NULL, dwLine, qwPos );
            
        case LEXER_STAR:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_STAR ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_POW, LOL_NULL, dwLine, qwPos );
            } 

            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_MUL_ASSIGN, LOL_NULL, dwLine, qwPos );
            } 

            return lol_lexer_mktok( pLexer, TOK_STAR, LOL_NULL, dwLine, qwPos );

        case LEXER_EQUAL:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_EQ, LOL_NULL, dwLine, qwPos );
            } 
            return lol_lexer_mktok( pLexer, TOK_ASSIGN, LOL_NULL, dwLine, qwPos );

        case LEXER_LT:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_LT ) {
                lol_lexer_walk( pLexer );
                if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                    lol_lexer_walk( pLexer );
                    return lol_lexer_mktok( pLexer, TOK_SHL_ASSIGN, LOL_NULL, dwLine, qwPos );
                }
                return lol_lexer_mktok( pLexer, TOK_BIT_SHL, LOL_NULL, dwLine, qwPos );
            } 

            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_LE, LOL_NULL, dwLine, qwPos );
            }

            return lol_lexer_mktok( pLexer, TOK_LT, LOL_NULL, dwLine, qwPos );

        case LEXER_GT:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_GT ) {
                lol_lexer_walk( pLexer );
                if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                    lol_lexer_walk( pLexer );
                    return lol_lexer_mktok( pLexer, TOK_SHR_ASSIGN, LOL_NULL, dwLine, qwPos );
                }

                return lol_lexer_mktok( pLexer, TOK_BIT_SHR, LOL_NULL, dwLine, qwPos );
            } 

            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_GE, LOL_NULL, dwLine, qwPos );
            }

            return lol_lexer_mktok( pLexer, TOK_GT, LOL_NULL, dwLine, qwPos );

        case LEXER_EXCLAMATION:
            if ( LEXER_CANREAD(pLexer) && LEXER_READ(pLexer) == LEXER_EQUAL ) {
                lol_lexer_walk( pLexer );
                return lol_lexer_mktok( pLexer, TOK_NEQ, LOL_NULL, dwLine, qwPos );
            } 
            return lol_lexer_mktok( pLexer, TOK_NOT, LOL_NULL, dwLine, qwPos );

        default:
            break;
        }
    }

    return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLValue) "Invalid value!", dwLine, qwPos );
}

LoLToken *lol_lexer_scanpath(LoLexer *pLexer) {
    LoLStr szPtr;
    LoLStr szPath;
    LoLChr c;
    LoLDWord dwLine;
    LoLQWord qwPos;
    LoLWord wLen = 0;
    
    lol_lexer_skip( pLexer );
    szPtr = pLexer->sBuff;
    dwLine = pLexer->dwLine;
    qwPos = pLexer->qwPos;
    
    while ( 
        LEXER_CANREAD(pLexer) && 
        ( c = LEXER_READ(pLexer) ) && (
            isalnum(c) || c == '_' ||
           c == '.' || c == '/' || c == '-'
        )
    ) {
        lol_lexer_walk( pLexer );
        wLen++;
    }

    if ( !(szPath = (LoLStr) malloc(wLen + 1)) )
        return lol_lexer_mktok( pLexer, TOK_ERROR, (LoLValue) "OOM!", dwLine, qwPos );

    memcpy( szPath, szPtr, wLen );
    *( szPath + wLen ) = LEXER_NUL;
    return lol_lexer_mktok( pLexer, TOK_PATH, szPath, dwLine, qwPos );
}

LoLVoid lol_lexer_dntok(LoLToken **ppTok) {

}

LoLVoid lol_lexer_done(LoLexer **ppLexer) {
    free( *ppLexer );
    ( *ppLexer ) = LOL_NULL;
}