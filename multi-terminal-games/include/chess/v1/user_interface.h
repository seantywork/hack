

#include "chess/v1/chess.h"

#ifndef _CHESS_V1_USER_INTERFACE_H_
#define _CHESS_V1_USER_INTERFACE_H_

void createNextMessage( string msg );
void appendToNextMessage( string msg );
void clearScreen( void );
void printLogo( void );
void printLogo( void );
void printMenu( void );
void printMessage( void );
void printLine( int iLine, int iColor1, int iColor2, Game& game );
void printSituation( Game& game );
void printBoard(Game& game);






#endif