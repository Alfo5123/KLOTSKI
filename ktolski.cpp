/*
Author(s): 
    Alfredo De la Fuente <alfredo.delafuente.b@gmail.com>
    Brian Giraldo <briangiraldo123@gmail.com>

KLOTSKI

    X    X-X    X
    |    | |    |
    X    X-X    X

    X    X-X    X
    |           |
    X    X X    X

    X           x

Rules:

    Like other sliding-block puzzles, several different-sized block pieces are placed inside a box, 
    which is generally in 4×5 size. Among the blocks, there is a special one (usually the largest) 
    which must be moved to a special area designated by the game board. The player is not allowed 
    to remove blocks, and may only slide blocks horizontally and vertically. Common goals are 
    to solve the puzzle with a minimum number of moves or in a minimum amount of time.

Klotski Solver

*/

#include <bits/stdc++.h>
using namespace std ;

int state[ 5 ][ 4 ] ; // Keep  the current state of the board
map<string,bool> m ; // Keep track of visited states
map<string,int> depth; // Keep track of the depth of exploration
map<string,int> st ; // Map between states and integers
map<int,string> ts ; // Inverse map of the above mentioned
int parent[ 30000 ] ; // Keep track of the parents in exploration

struct piece
{
    int x ; 
    int y ; 

    int width ; 
    int height ; 

    piece(){};

    piece ( int width_ , int height_ )
    {
        width = width_ ; 
        height = height_ ; 
    }

    int getWidth ( ) { return width ; }

    int getHeight( ) { return height ; }

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

        for ( int i = y ; i < y + height ; i++ )
            for ( int j = x ; j < x + width ; j++ )
                state[ i ][ j ] = n ; 
    }

    int getX ( ) { return x ; }

    int getY ( ) { return y ; }

    bool left ( )
    {
        // Whether is possible to move the current piece to the left
        // If it is touching the left border
        if ( x == 0 ) return false ; 
        // If it has height 1 or 2 blocks and there is a free space next to it
        if ( !state[ y ][ x-1 ] && !state[ y+height-1 ][ x-1 ] ) return true ; 
        // Otherwise it's not possible to make such move
        return false ;
    }

    bool right ( )
    {
        // Whether is possible to move the current piece to the right
        // If it is touching the right border
        if ( x + width - 1 == 3 ) return false ; 
        // If it has height 1 or 2 blocks and there is a free space next to it
        if ( !state[ y ][ x+width ] && !state[ y+height-1 ][ x+width ] ) return true ; 
        // Otherwise it's not possible to make such move
        return false ;
    }

    bool up ( )
    {
        // Whether is possible to move the current piece up.
        // If it is touching the top border
        if ( y == 0 ) return false ; 
        // If it has width 1 or 2 blocks and there is a free space next to it
        if ( !state[ y-1 ][ x ] && !state[ y-1 ][ x+width-1 ] ) return true ; 
        // Otherwise it's not possible to make such move
        return false ;
    }

    bool down ( )
    {
        // Whether is possible to move the current piece up.
        // If it is touching the bottom border
        if ( y + height - 1 == 4 ) return false ; 
        // If it has width 1 or 2 blocks and there is a free space next to it
        if ( !state[ y+height ][ x ] && !state[ y+height ][ x+width-1 ] ) return true ; 
        // Otherwise it's not possible to make such move
        return false ;
    }

    void moveLeft ( )
    {
        if ( !left() ) return ;  // Check if possible to move
        // Make move to the left and actualize board
        state[ y ][ x + width - 1 ] = 0 ; state[ y + height - 1 ][ x + width - 1 ] = 0 ; 
        state[ y ][ x - 1 ] = getNotation() ;  state[ y + height - 1 ][ x - 1 ] = getNotation() ; 
        x--;
    }

    void moveRight( )
    {
        if ( !right() ) return ; // Check if possible to move
        // Make move to the right and actualize board
        state[ y ][ x ] = 0 ; state[ y + height - 1 ][ x ] = 0 ; 
        state[ y ][ x + width ] = getNotation() ; state[ y + height -1 ][ x + width ] = getNotation() ;
        x++;
    }

    void moveUp ( )
    {
        if ( !up() ) return ; // Check if possible move 
        // Make move up and actualize board
        state[ y + height - 1 ][ x ] = 0 ; state[ y + height - 1 ][ x + width - 1 ] = 0 ; 
        state[ y - 1 ][ x ] = getNotation() ; state[ y - 1 ][ x + width - 1 ] = getNotation() ; 
        y--;
    }

    void moveDown( )
    {
        if ( !down() ) return ; // Check if possible move
        // Make move down and actualize board 
        state[ y ][ x ] = 0 ; state[ y ][ x + width - 1 ] = 0 ; 
        state[ y + height ][ x ] = getNotation() ; state[ y + height ][ x + width - 1 ] = getNotation() ;
        y++;
    }

} ;

piece pieces [ 10 ] ; // Array of the different pieces to locate in board

void initBoard ()
{
    // Empty cells of the board 
    memset ( state , 0 , sizeof( state ) ) ;
    // Fill the board with pieces positions
    pieces[ 0 ] = piece ( 1 , 2 ) ; pieces[ 0 ].setYX ( 0 , 0 ) ; 
    pieces[ 1 ] = piece ( 2 , 2 ) ; pieces[ 1 ].setYX ( 0 , 1 ) ; 
    pieces[ 2 ] = piece ( 1 , 2 ) ; pieces[ 2 ].setYX ( 0 , 3 ) ; 
    pieces[ 3 ] = piece ( 1 , 2 ) ; pieces[ 3 ].setYX ( 2 , 0 ) ;
    pieces[ 4 ] = piece ( 2 , 1 ) ; pieces[ 4 ].setYX ( 2 , 1 ) ;
    pieces[ 5 ] = piece ( 1 , 1 ) ; pieces[ 5 ].setYX ( 3 , 1 ) ; 
    pieces[ 6 ] = piece ( 1 , 1 ) ; pieces[ 6 ].setYX ( 3 , 2 ) ; 
    pieces[ 7 ] = piece ( 1 , 2 ) ; pieces[ 7 ].setYX ( 2 , 3 ) ; 
    pieces[ 8 ] = piece ( 1 , 1 ) ; pieces[ 8 ].setYX ( 4 , 0 ) ;
    pieces[ 9 ] = piece ( 1 , 1 ) ; pieces[ 9 ].setYX ( 4 , 3 ) ; 
}

string encode( )
{
    // This method encodes the state of the board in a string
    
    string s = "";
    for ( int i = 0 ; i < 5 ; i++ )
        for ( int j = 0 ; j < 4 ; j++ )
            s += ( char ) ( '0' + state[ i ][ j ] ) ; 

    return s ; 
}

void setBoard( string s )
{
    // This method reconstructs a board base on it's encoding

    // Empty cells of the board
    memset ( state , 0 , sizeof( state ) ) ;
    // Fill the board with pieces positions
    int p = 0 ; 
    int c = 0 ; 

    for ( int i = 0 ; i < 5 ; i++ )
    {
        for ( int j = 0 ; j < 4 ; j++ )
        {
            if ( s[ c ] == '1' )
            {
                 pieces[ p ] = piece ( 1 , 1 ) ; 
                 pieces[ p ].setYX ( i , j ) ; 
                 p++;
            }
            else if ( s[ c ] == '2' )
            {
                 pieces[ p ] = piece ( 1 , 2 ) ; 
                 pieces[ p ].setYX ( i , j ) ; 
                 s[ c+4 ] = '*' ;
                 p++;   
            }
            else if ( s[ c ] == '3' )
            {
                pieces[ p ] = piece ( 2 , 1 ) ; 
                pieces[ p ].setYX ( i , j ) ; 
                s[ c+1 ] = '*';
                p++;
            }
            else if ( s[ c ] == '4' )
            {
                pieces[ p ] = piece ( 2 , 2 ) ; 
                pieces[ p ].setYX ( i , j ) ; 
                s[ c + 1 ] = s[ c + 4 ] = s[ c + 5 ] = '*' ;
                p++;
            }

            c++;
        }
    }
}

void printBoard ()
{
    char c[5][4] ; memset ( c , ' ' , sizeof ( c ) ) ;
    for ( int k = 0 ; k < 10 ; k++ )
    {
        int sy = pieces[ k ].getY() ; 
        int sx = pieces[ k ].getX() ;
        int w = pieces[ k ].getWidth() ;
        int h = pieces[ k ].getHeight() ; 

        for ( int i = sy ; i < sy + h ; i++ )
            for ( int j = sx ; j < sx + w ; j++ )
                    c[ i ][ j ] = ( char )( pieces[k].getNotation() + '0' ) ;
    }

    cout << endl;

    for ( int i = 0 ; i < 5 ; i++ )
    {
        for ( int j = 0 ; j < 4 ; j++ )
        {
            cout << c[ i ][ j ] << " " ;
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

void printSolution ( string s )
{
    // Recursively printing optimal path
    if ( st[ s ] == 0 )
    {
        cout << 0 << endl; 
        setBoard( s ) ; 
        printBoard( ) ; 
        return ; 
    }

    printSolution ( ts[ parent[ st[ s ] ] ] ) ; 
    cout << depth[ s ] << endl; 
    setBoard( s ) ; 
    printBoard( ) ;  
}


int main()
{
    // Initialize Board
    initBoard() ; 

    // Start BFS
    queue<string> q; // Queue to keep current visited state
    long long c = 0 ; // Number of states visited so far

    string s = encode() ; 
    q.push(s) ; 
    m[s] = true ; 
    depth[s] = 0 ; 
    ts[ c ] = s ; 
    st[ s ] = c++ ; 
    parent[ 0 ] = 0 ; 

    while ( !q.empty() )
    {
        string cur = q.front() ; q.pop() ; 
        // Reconstruct board
        setBoard ( cur ) ; 
        //cout << sofar << endl;
        //printBoard() ; 

        // Check if any piece can be moved
        for ( int i = 0 ; i < 10 ; i++ )
        {
            if ( pieces[ i ].left() )
            {
                 pieces[ i ].moveLeft() ; 
                 string aux = encode() ;
                 if ( !m[ aux ] ) 
                 {
                    q.push( aux ) ;
                    m[ aux ] = true ;
                    depth[ aux ] = depth[ cur ] + 1 ;
                    ts[ c ] = aux ; 
                    st[ aux ] = c++;
                    parent[ st[ aux ] ] = st[ cur ] ;  

                    // Check if reached final state
                    if ( checkEnd() )
                    {
                        printSolution( aux ) ; 
                        return 0 ; 
                    }
                 }

                 // Check if another possibility to move in the same direction
                 if ( pieces[ i ].left() )
                 {
                     pieces[ i ].moveLeft() ; 
                     string aux = encode() ;
                     if ( !m[ aux ] ) 
                     {
                        q.push( aux ) ;
                        m[ aux ] = true ;
                        depth[ aux ] = depth[ cur ] + 1 ;
                        ts[ c ] = aux ; 
                        st[ aux ] = c++;
                        parent[ st[ aux ] ] = st[ cur ] ;  

                        // Check if reached final state
                        if ( checkEnd() )
                        {
                            printSolution( aux ) ; 
                            return 0 ; 
                        }
                     }
                     pieces[ i ].moveRight() ; 
                 }

                 pieces[ i ].moveRight() ; 
            }

            if ( pieces[ i ].right() )
            {
                 pieces[ i ].moveRight() ; 
                 string aux = encode() ;
                 if ( !m[ aux ] ) 
                 {
                    q.push( aux ) ;
                    m[ aux ] = true ;
                    depth[ aux ] = depth[ cur ] + 1 ;
                    ts[ c ] = aux ; 
                    st[ aux ] = c++;
                    parent[ st[ aux ] ] = st[ cur ] ;  

                    // Check if reached final state
                    if ( checkEnd() )
                    {    
                        printSolution( aux ) ; 
                        return 0 ; 
                    }
                 }
                 // Check if move again in the same direction
                 if ( pieces[ i ].right() )
                 {
                     pieces[ i ].moveRight() ; 
                     string aux = encode() ;
                     if ( !m[ aux ] ) 
                     {
                        q.push( aux ) ;
                        m[ aux ] = true ;
                        depth[ aux ] = depth[ cur ] + 1 ;
                        ts[ c ] = aux ; 
                        st[ aux ] = c++;
                        parent[ st[ aux ] ] = st[ cur ] ;  

                        // Check if reached final state
                        if ( checkEnd() )
                        {    
                            printSolution( aux ) ; 
                            return 0 ; 
                        }
                     }

                     pieces[ i ].moveLeft() ; 
                 }

                 pieces[ i ].moveLeft() ; 
            }

            if ( pieces[ i ].up() )
            {
                 pieces[ i ].moveUp() ; 
                 string aux = encode() ;
                 if ( !m[ aux ] ) 
                 {
                    q.push( aux ) ;
                    m[ aux ] = true ;
                    depth[ aux ] = depth[ cur ] + 1 ;
                    ts[ c ] = aux ; 
                    st[ aux ] = c++;
                    parent[ st[ aux ] ] = st[ cur ] ;  

                    // Check if reached final state
                    if ( checkEnd() )
                    {
                        printSolution( aux ) ;
                        return 0 ; 
                    }
                 }

                 // Check if move in the same direction
                  if ( pieces[ i ].up() )
                 {
                     pieces[ i ].moveUp() ; 
                     string aux = encode() ;
                     if ( !m[ aux ] ) 
                     {
                        q.push( aux ) ;
                        m[ aux ] = true ;
                        depth[ aux ] = depth[ cur ] + 1 ;
                        ts[ c ] = aux ; 
                        st[ aux ] = c++;
                        parent[ st[ aux ] ] = st[ cur ] ;  

                        // Check if reached final state
                        if ( checkEnd() )
                        {
                            printSolution( aux ) ;
                            return 0 ; 
                        }
                     }

                     pieces[ i ].moveDown() ; 
                 }

                 pieces[ i ].moveDown() ; 
            }

            if ( pieces[ i ].down() )
            {
                 pieces[ i ].moveDown() ; 
                 string aux = encode() ;
                 if ( !m[ aux ] ) 
                 {
                    q.push( aux ) ;
                    m[ aux ] = true ;
                    depth[ aux ] = depth[ cur ] + 1 ;
                    ts[ c ] = aux ; 
                    st[ aux ] = c++;
                    parent[ st[ aux ] ] = st[ cur ] ;  

                    // Check if reached final state
                    if ( checkEnd() )
                    {
                        printSolution( aux ) ;
                        return 0 ; 
                    }
                 }
                 // Check if move in the same direction
                 if ( pieces[ i ].down() )
                 {
                     pieces[ i ].moveDown() ; 
                     string aux = encode() ;
                     if ( !m[ aux ] ) 
                     {
                        q.push( aux ) ;
                        m[ aux ] = true ;
                        depth[ aux ] = depth[ cur ] + 1 ;
                        ts[ c ] = aux ; 
                        st[ aux ] = c++;
                        parent[ st[ aux ] ] = st[ cur ] ;  

                        // Check if reached final state
                        if ( checkEnd() )
                        {
                            printSolution( aux ) ;
                            return 0 ; 
                        }
                     }

                     pieces[ i ].moveUp() ; 
                 }
                 
                 pieces[ i ].moveUp() ; 
            }

        }

    }

    return 0;
}
