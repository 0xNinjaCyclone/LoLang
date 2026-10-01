/*
    Author  => Abdallah Mohamed ( @0xNinjaCyclone )
    Email   => elsharifabdallah53@gmail.com
    Date    => August 27, 2026 - 03:28PM
    Why     => Just to revive my burnt-out mind and reclaim the skills that were all but killed off
                due to artificial intelligence and brain rot.
    Compile => gcc *.c -o lolang -DUSE_DALGO_STRUCTURES
*/

#include "lolang.h"

LoLInt main(LoLInt nArg, LoLStrArr cpArgArr) {
    LoLParser *pParser;
    Tree *pLoLAst;

    if ( nArg <= 1 ) {
        fprintf( stderr, "\tUsage: %s <source_file.lol>", cpArgArr[0] );
        return LOL_FAILURE;
    }

    pParser = lol_parser( cpArgArr[1] );
    if ( pParser ) {
        puts( "Parsing ..." );
        if ( pLoLAst = lol_parser_launch(pParser) ) {
            tree_print( pLoLAst );
            puts( "Don3 successfully -_-" );
        }
    }

    return LOL_SUCCESS;
}