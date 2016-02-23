/********************************* Klotski Solver **********************************
*
*
*    Author:                    Alfredo De la Fuente 
*                               Brian Giraldo 
*
*    Email:                     alfredo.delafuente.b@gmail.com
*
*    Description:               Code to solve the Klotski Puzzle in 
*                               minimum number of steps.
*
******************************************************************************/

#include <bits/stdc++.h>
using namespace std ;

int state[ 5 ][ 4 ] ; // Keep  the current state of the board based on pieces
int board[ 5 ][ 4 ] ; // Keep  the current full state of the board
map<string,bool> m ; // Keep track of visited states
map<string,int> depth; // Keep track of the depth of exploration
map<string,int> st ; // Map between states and integers
map<int,string> ts ; // Inverse map of the above mentioned
map<int,string> full ; // From integers position to full board state
int parent[ 30000 ] ; // Keep track of the parents in exploration
queue<string> q; // Queue to keep current visited state
long long c = 0 ; // Number of states visited so far
string code ;

struct piece
{
    int x ;
    int y ;
    int width ;
    int height ;
    int info ;

    piece(){};

    piece ( int width_ , int height_ , int info_ )
    {
        width = width_ ;
        height = height_ ;
        info = info_ ;
    }

    int getWidth ( ) { return width ; }

    int getHeight( ) { return height ; }

    int getInfo( ){ return info ; }

    int getNotation( )
    {
        /*
        Notation for pieces of the board
            width x height -> notation
            0x0 -> 0  ; 1x1 -> 1  ; 1x2 -> 2 ; 2x1 -> 3 ; 2x2 -> 4
        */
        if ( width == 1 && height == 1 ) return 1 ;
        if ( width == 1 && height == 2 ) return 2 ;
        if ( width == 2 && height == 1 ) return 3 ;
        if ( width == 2 && height == 2 ) return 4 ;
        return 0 ;
    }

    void setYX ( int y_ , int x_ )
    {
        x = x_ ;
        y = y_ ;

        // Set the state of the board based on the piece notation
        int n = getNotation() ;
        int m = getInfo() ;

        for ( int i = y ; i < y + height ; i++ )
            for ( int j = x ; j < x + width ; j++ )
                state[ i ][ j ] = n ;

        for ( int i = y ; i < y + height ; i++ )
            for ( int j = x ; j < x + width ; j++ )
                board[ i ][ j ] = m ;
    }

    int getX ( ) { return x ; }

    int getY ( ) { return y ; }

    bool left ( )
    {
        // Whether is possible to move the current piece to the left
        if ( x == 0 ) return false ; // If it is touching the left border
        // If it has height 1 or 2 blocks and there is a free space next to it
        if ( !state[ y ][ x-1 ] && !state[ y+height-1 ][ x-1 ] ) return true ;
        return false ; // Otherwise it's not possible to make such move
    }

    bool right ( )
    {
        // Whether is possible to move the current piece to the right
        if ( x + width - 1 == 3 ) return false ; // If it is touching the right border
        // If it has height 1 or 2 blocks and there is a free space next to it
        if ( !state[ y ][ x+width ] && !state[ y+height-1 ][ x+width ] ) return true ;
        return false ; // Otherwise it's not possible to make such move
    }

    bool up ( )
    {
        // Whether is possible to move the current piece up.
        if ( y == 0 ) return false ; // If it is touching the top border
        // If it has width 1 or 2 blocks and there is a free space next to it
        if ( !state[ y-1 ][ x ] && !state[ y-1 ][ x+width-1 ] ) return true ;
        return false ; // Otherwise it's not possible to make such move
    }

    bool down ( )
    {
        // Whether is possible to move the current piece up.
        if ( y + height - 1 == 4 ) return false ; // If it is touching the bottom border
        // If it has width 1 or 2 blocks and there is a free space next to it
        if ( !state[ y+height ][ x ] && !state[ y+height ][ x+width-1 ] ) return true ;
        return false ; // Otherwise it's not possible to make such move
    }

    void moveLeft ( )
    {
        if ( !left() ) return ;  // Check if possible to move
        // Make move to the left and actualize board
        state[ y ][ x + width - 1 ] = 0 ; state[ y + height - 1 ][ x + width - 1 ] = 0 ;
        board[ y ][ x + width - 1 ] = -1 ; board[ y + height - 1 ][ x + width - 1 ] = -1 ;
        state[ y ][ x - 1 ] = getNotation() ;  state[ y + height - 1 ][ x - 1 ] = getNotation() ;
        board[ y ][ x - 1 ] = getInfo() ;  board[ y + height - 1 ][ x - 1 ] = getInfo() ;
        x--;
    }

    void moveRight( )
    {
        if ( !right() ) return ; // Check if possible to move
        // Make move to the right and actualize board
        state[ y ][ x ] = 0 ; state[ y + height - 1 ][ x ] = 0 ;
        board[ y ][ x ] = -1 ; board[ y + height - 1 ][ x ] = -1 ;
        state[ y ][ x + width ] = getNotation() ; state[ y + height -1 ][ x + width ] = getNotation() ;
        board[ y ][ x + width ] = getInfo() ; board[ y + height -1 ][ x + width ] = getInfo() ;
        x++;
    }

    void moveUp ( )
    {
        if ( !up() ) return ; // Check if possible move
        // Make move up and actualize board
        state[ y + height - 1 ][ x ] = 0 ; state[ y + height - 1 ][ x + width - 1 ] = 0 ;
        board[ y + height - 1 ][ x ] = -1 ; board[ y + height - 1 ][ x + width - 1 ] = -1 ;
        state[ y - 1 ][ x ] = getNotation() ; state[ y - 1 ][ x + width - 1 ] = getNotation() ;
        board[ y - 1 ][ x ] = getInfo() ; board[ y - 1 ][ x + width - 1 ] = getInfo() ;
        y--;
    }

    void moveDown( )
    {
        if ( !down() ) return ; // Check if possible move
        // Make move down and actualize board
        state[ y ][ x ] = 0 ; state[ y ][ x + width - 1 ] = 0 ;
        board[ y ][ x ] = -1 ; board[ y ][ x + width - 1 ] = -1 ;
        state[ y + height ][ x ] = getNotation() ; state[ y + height ][ x + width - 1 ] = getNotation() ;
        board[ y + height ][ x ] = getInfo() ; board[ y + height ][ x + width - 1 ] = getInfo() ;
        y++;
    }

} ;

piece pieces [ 10 ] ; // Array of the different pieces to locate in board

void initBoard ()
{
    // Empty cells of the board
    memset ( state , 0 , sizeof( state ) ) ;
    memset ( board , -1 , sizeof ( board ) ) ;
    // Fill the board with pieces positions
    pieces[ 0 ] = piece ( 1 , 2 , 0 ) ; pieces[ 0 ].setYX ( 0 , 0 ) ;
    pieces[ 1 ] = piece ( 2 , 2 , 1 ) ; pieces[ 1 ].setYX ( 0 , 1 ) ;
    pieces[ 2 ] = piece ( 1 , 2 , 2 ) ; pieces[ 2 ].setYX ( 0 , 3 ) ;
    pieces[ 3 ] = piece ( 1 , 2 , 3 ) ; pieces[ 3 ].setYX ( 2 , 0 ) ;
    pieces[ 4 ] = piece ( 2 , 1 , 4 ) ; pieces[ 4 ].setYX ( 2 , 1 ) ;
    pieces[ 5 ] = piece ( 1 , 1 , 5 ) ; pieces[ 5 ].setYX ( 3 , 1 ) ;
    pieces[ 6 ] = piece ( 1 , 1 , 6 ) ; pieces[ 6 ].setYX ( 3 , 2 ) ;
    pieces[ 7 ] = piece ( 1 , 2 , 7 ) ; pieces[ 7 ].setYX ( 2 , 3 ) ;
    pieces[ 8 ] = piece ( 1 , 1 , 8 ) ; pieces[ 8 ].setYX ( 4 , 0 ) ;
    pieces[ 9 ] = piece ( 1 , 1 , 9 ) ; pieces[ 9 ].setYX ( 4 , 3 ) ;
}

void encode( )
{
    // This method encodes the state of the board in a string
    code = "";
    for ( int i = 0 ; i < 5 ; i++ )
        for ( int j = 0 ; j < 4 ; j++ )
        {
            code += ( char ) ( '0' + state[ i ][ j ] ) ;
            code +=  ( board[ i ][ j ] < 0 ? '0' : ( char ) ( '0' + board[ i ][ j ] ) ) ;
        }
}

string getCode( )
{
    string s = "" ; for ( int i = 0 ; i < code.size() ; i+=2 ) s+=code[ i ] ;
    return s ;
}

void setBoard( string s )
{
    // This method reconstructs a board base on it's encoding
    string s_ = "";
    for ( int i = 0 ; i < s.size() ; i += 2 ) s_ += s[ i ] ;

    // Empty cells of the board
    memset ( board , -1 , sizeof ( board ) ) ;
    memset ( state , 0 , sizeof( state ) ) ;
    // Fill the board with pieces positions
    int p = 0 ;
    int c = 0 ;

    for ( int i = 0 ; i < 5 ; i++ )
    {
        for ( int j = 0 ; j < 4 ; j++ )
        {
            if ( s_[ c ] == '1' )
            {
                 pieces[ p ] = piece ( 1 , 1 , ( s[ 2*c + 1 ] -'0' ) ) ;
                 pieces[ p ].setYX ( i , j ) ;
                 p++;
            }
            else if ( s_[ c ] == '2' )
            {
                 pieces[ p ] = piece ( 1 , 2 , ( s[ 2*c + 1 ] -'0' ) ) ;
                 pieces[ p ].setYX ( i , j ) ;
                 s_[ c+4 ] = '*' ;
                 p++;
            }
            else if ( s_[ c ] == '3' )
            {
                pieces[ p ] = piece ( 2 , 1 , ( s[ 2*c + 1 ] -'0' ) ) ;
                pieces[ p ].setYX ( i , j ) ;
                s_[ c+1 ] = '*';
                p++;
            }
            else if ( s_[ c ] == '4' )
            {
                pieces[ p ] = piece ( 2 , 2 , ( s[ 2*c + 1 ] -'0' ) ) ;
                pieces[ p ].setYX ( i , j ) ;
                s_[ c + 1 ] = s_[ c + 4 ] = s_[ c + 5 ] = '*' ;
                p++;
            }

            c++;
        }
    }
}

void printBoard ()
{
    for ( int i = 0 ; i < 5 ; i++ )
    {
        for ( int j = 0 ; j < 4 ; j++ )
        {
            cout << ( board[ i ][ j ] >= 0 ? ( char ) ( board[ i ][ j ] + 'A' ) : ' ' ) << " " ;
        }
        cout << endl;
    }
    cout << endl;
}

bool checkEnd ( )
{
    // Check if final state ( solution board ) has been reached
    return ( state[ 3 ][ 1 ] == state[ 3 ][ 2 ] &&  state[ 3 ][ 2 ] == state[ 4 ][ 1 ] &&
             state[ 4 ][ 1 ] == state[ 4 ][ 2 ] &&  state[ 4 ][ 1 ] == 4 ) ;
}

void update ( string aux , string cur )
{
    q.push( aux ) ;
    m[ aux ] = true ;
    depth[ aux ] = depth[ cur ] + 1 ;
    ts[ c ] = aux ;
    full[ c ] = code ;
    st[ aux ] = c++;
    parent[ st[ aux ] ] = st[ cur ] ;
}

void printSolution ( string s )
{
    // Recursively printing optimal path
    if ( st[ s ] == 0 )
    {
        cout << 0 << endl;
        setBoard( full[ st[ s ] ] ) ;
        printBoard( ) ;
        return ;
    }

    printSolution ( ts[ parent[ st[ s ] ] ] ) ;
    cout << depth[ s ] << endl;
    setBoard( full[ st[ s ] ]  ) ;
    printBoard( ) ;
}

bool moveSpecific ( piece p, string cur, int direction )
{ 
    //direction 0-left, 1-right,2-up,3-down
    if( direction == 0 and p.left() ) p.moveLeft();
    else if( direction==1 and p.right() ) p.moveRight();
    else if( direction==2 and p.up() ) p.moveUp();
    else if( direction==3 and p.down() ) p.moveDown();
    else return 0 ; 

    encode() ; string aux = getCode() ;
    if ( !m[ aux ] )
    {
        update ( aux , cur ) ;
        // Check if reached final state
        if ( checkEnd() )
        {
            printSolution( aux ) ;
            return 1;
        }
        if ( moveSpecific(p,cur,0) ) return 1;
        if ( moveSpecific(p,cur,1) ) return 1;
        if ( moveSpecific(p,cur,2) ) return 1;
        if ( moveSpecific(p,cur,3) ) return 1;
    }

    if ( direction == 0 ) p.moveRight();
    else if (direction == 1) p.moveLeft();
    else if (direction == 2) p.moveDown();
    else if (direction == 3) p.moveUp();
    return 0;
}

int main()
{
    // Initialize Board
    initBoard() ;
    // Start BFS
    encode() ; string s = getCode() ;
    q.push(s) ;
    m[s] = true ;
    depth[s] = 0 ;
    ts[ c ] = s ;
    full[ c ] = code ;
    st[ s ] = c++ ;
    parent[ 0 ] = 0 ;

    while ( !q.empty() )
    {

        string cur = q.front() ; q.pop() ;
        // Reconstruct board
        setBoard ( full[ st[ cur ] ] ) ;
        encode() ;
        // Check if any piece can be moved
        for ( int i = 0 ; i < 10 ; i++ )
        {
            if ( moveSpecific ( pieces[i], cur, 0 ) ) return 0;
            if ( moveSpecific ( pieces[i], cur, 1 ) ) return 0;
            if ( moveSpecific ( pieces[i], cur, 2 ) ) return 0;
            if ( moveSpecific ( pieces[i], cur, 3 ) ) return 0;
        }

    }

    return 0;
}
