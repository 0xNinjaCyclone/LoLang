#ifndef _LOLEXER
#define _LOLEXER

#include "lolang.h"


#define LEXER_MAX_IDENTIFIER_LEN 64

#define LEXER_SQUOTE '\''
#define LEXER_DQUOTE '"'
#define LEXER_USCORE '_'
#define LEXER_WHITESPACE ' '
#define LEXER_BACKSPACE '\b'
#define LEXER_FORMAT '\f'
#define LEXER_CR '\r'
#define LEXER_LF '\n'
#define LEXER_TAB '\t'
#define LEXER_NUL '\0'
#define LEXER_HASHTAG '#'
#define LEXER_SLASH '/'
#define LEXER_PERCENT '%'
#define LEXER_BACKSLASH '\\'
#define LEXER_COMMA ','
#define LEXER_COLON ':'
#define LEXER_SEMICOLON ';'
#define LEXER_DOT '.'
#define LEXER_NEGSIGN '-'
#define LEXER_POSSIGN '+'
#define LEXER_STAR '*'
#define LEXER_EQUAL '='
#define LEXER_LT '<'
#define LEXER_GT '>'
#define LEXER_PIPE '|'
#define LEXER_CARET '^'
#define LEXER_TILDE '~'
#define LEXER_EXCLAMATION '!'
#define LEXER_AMPERSAND '&'
#define LEXER_LPAREN '('
#define LEXER_RPAREN ')'
#define LEXER_OPENBRACKET '['
#define LEXER_CLOSEBRACKET ']'
#define LEXER_OPENCBRACKET '{'
#define LEXER_CLOSECBRACKET '}'

#define LEXER_ISCOMMENT(pLexer) ( *pLexer->sBuff == LEXER_HASHTAG )
#define LEXER_ISNEWLINE(pLexer) ( *pLexer->sBuff == LEXER_LF || *pLexer->sBuff == LEXER_SEMICOLON )
#define LEXER_ISWS(pLexer) ( *pLexer->sBuff <= 0x20 && !LEXER_ISNEWLINE(pLexer) )
#define LEXER_CANREAD(pLexer) ( pLexer->qwPos < pLexer->qwSize )
#define LEXER_READ(pLexer) ( *pLexer->sBuff )

typedef struct _LoLexer {
    LoLQWord qwSize;
    LoLQWord qwPos;
    LoLDWord dwLine;
    LoLStr sBuff;
} LoLexer;

typedef enum {
    TOK_EOF = 0,

    /* literals */
    TOK_IDENT, TOK_INT, TOK_FLOAT, TOK_STRING, TOK_PATH,

    /* LoLKeywords */
    TOK_NOTHING, TOK_IF, TOK_ELSE, TOK_FOR, TOK_IN, TOK_WHILE,
    TOK_NEED, TOK_DEFINE, TOK_RET, TOK_AND, TOK_OR,
    TOK_KW_EQ, TOK_KW_NE, TOK_KW_LT, TOK_KW_LE, TOK_KW_GT, TOK_KW_GE,
    TOK_NEXT, TOK_STOP, TOK_TRUE, TOK_FALSE,

    /* operators */
    TOK_ASSIGN,      /* =  */
    TOK_EQ,        /* == */
    TOK_NEQ,         /* != */
    TOK_LT, TOK_LE, TOK_GT, TOK_GE,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT, TOK_POW,
    TOK_NOT,          /* ! */
    TOK_INC,          /* ++ */
    TOK_DEC,          /* -- */
    TOK_ADD_ASSIGN,   /* += */
    TOK_SUB_ASSIGN,   /* -= */
    TOK_MUL_ASSIGN,   /* *= */
    TOK_DIV_ASSIGN,   /* /= */
    TOK_MOD_ASSIGN,   /* %= */
    TOK_SHL_ASSIGN,   /* <<= */
    TOK_SHR_ASSIGN,   /* >>= */
    TOK_AND_ASSIGN,   /* &= */
    TOK_OR_ASSIGN,    /* |= */
    TOK_XOR_ASSIGN,   /* ^= */
    TOK_BIT_AND,      /* & */
    TOK_BIT_OR,       /* | */
    TOK_BIT_XOR,      /* ^ */
    TOK_BIT_NOT,      /* ~ */
    TOK_BIT_SHL,      /* << */
    TOK_BIT_SHR,      /* >> */

    /* punctuation */
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET, TOK_COMMA, TOK_COLON, TOK_DOT,
    TOK_NEWLINE,

    TOK_ERROR
} LoLTokenType;

typedef struct _LoLToken {
    LoLTokenType type;
    LoLValue value;
    LoLQWord qwPos;
    LoLDWord dwLine;
} LoLToken;

typedef struct _LoLKeyword {
    LoLCStr szWord;
    LoLTokenType tok_type;
} LoLKeyword;

LoLexer *lol_lexer(LoLStr sLoLSrc, LoLQWord qwSrcSize);
LoLToken *lol_lexer_scan(LoLexer *pLexer);
LoLToken *lol_lexer_scanpath(LoLexer *pLexer);
LoLVoid lol_lexer_dntok(LoLToken **ppTok);
LoLVoid lol_lexer_done(LoLexer **ppLexer);

#endif