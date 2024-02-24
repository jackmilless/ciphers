// when calling function from terminal, ignore msg; msg is just the the first 
// argument in terminal or an already altered version thereof
//
// inverse functions have identical arguments; using the same arguments on 
// an encoded message will return the original message (except for some cases
// where a small amount of information is lost)
//
// ordered roughly by type and complexity:
// substitution ciphers: atbash, caesar, vigenere, polybius_square,
//						 four_square, playfair
// transposition ciphers: scytale, railfence, spiral, columnar_transposition
// substitition + transposition ciphers: bifid, trifid, adfgx, adfgvx

/* 
 * atbash: switch a with z, b with y, etc.
 * identical to decryption; "deatbash" calls this function as well
 * takes no additional arguments
 * Example with msg hello:
 * atbash : svool
 */
void atbash(char *msg);

/*
 * caesar: shifts each letter in_key places to the right in the alphabet
 * passes to vigenere cipher using letter associated with in_key
 * key: number of places to shift each letter to the right
 * Example with msg hello:
 * caesar 3 : abcd -> defg
 */
void caesar(int key, char *msg);

// decaesar: inverse of caesar cipher
void decaesar(int key, char *msg);

/*
 * vigenere: shifts each letter to the right according to current letter in key
 * starts with first letter of key and moves one along for each letter in message
 * key: string of length >= 1 determining how far each letter is shifted to the 
 * right, where a = 0, b = 1, etc.
 * Example with msg dog:
 * vigenere abc : dog -> dpi
 */
void vigenere(char *key, char *msg);

// devigenere: inverse of vigenere cipher
void devigenere(char *key, char *msg);

/*
 * polybius_square: creates a polybius square using key. A polybius square is a 5x5 grid of all letters in the
 * alphabet (except j, which is treated as i). Unique letters of key placed in grid from left to right, top to
 * bottom, after which remaining letters are placed in alphabetical order. Letters in message then converted
 * to integer indices (starting at 1, standard (row, col) format) of the location of the letter in the square.
 * key: alphabetical string of any length used to create polybius square. 
 * Example with msg helloworld:
 * polybius_square polybiussquare
 *   1 2 3 4 5
 * 1 p o l y b
 * 2 i u s q a
 * 3 r e c d f  ->  42321313125312311334
 * 4 g h k m n
 * 5 t v w x z
 */
void polybius_square(char *key, char *msg);

// depolybius: inverse of polybius_square
void depolybius(char *key, char *msg);

/*
 * foursquare: four polybius squares are made. "upper left" and "lower right" are just the alphabet 
 * (excluding j), while the "upper right" and "lower left" are made with key_one and key_two, respectively,
 * as in polybius_square. Message is encoded in pairs: finds first letter in upper left matrix, and second 
 * letter in lower right. In the upper right matrix, encodes the letter at the row of the first input letter 
 * and the column of the second. Then, in the lower left matrix, encodes the letter at at the column of the
 * first input letter and the row of the second. If odd number of letters, last letter is encoded unchanged.
 * key_one: alphabetical string of any length used to create upper right polybius square
 * key_two: alphabetical string of any length used to create lower left polybius square
 * Example with msg helloworld:
 * foursquare four square
 * a b c d e    f o u r a
 * f g h i k    b c d e g
 * l m n o p    h i k l m
 * q r s t u    n p q s t
 * v w x y z    v w x y z
 *                         ->  guhgiyipls
 * s q u a r    a b c d e
 * e b c d f    f g h i k
 * g h i k l    l m n o p
 * m n o p t    q r s t u
 * v w x y z    v w x y z
 */
void foursquare(char *key_one, char *key_two, char *msg);

// defoursquare: inverse of four_square
void defoursquare(char *key_one, char *key_two, char *msg);

/*
 * playfair: creates a polybius square using key. Message is encoded in pairs; find letters in square. 
 * If the same letter is repeated, or 1 letter is left at the end, insert an x and apply relevant rule:
 * If on the same column, replace each letter with the letter below it (wrapping around if needed). 
 * If on the same row, replace each letter with the letter to its right (wrapping around if needed).
 * Otherwise, find opposite "corners" of a rectangle created by the indices of the two letters, 
 * replacing each letter with the letter of the "corner" that is on the same row.
 * key: alphabetical string of any length used to create polybius square
 * Example with msg helloworld:
 * playfair playfair
 *   1 2 3 4 5
 * 1 p l a y f
 * 2 i r b c d
 * 3 e g h k m  -> kgyvrvvqgrcz
 * 4 n o q s t
 * 5 u v w x z
 */
void playfair(char *key, char *msg);

// deplayfair: inverse of playfair
void deplayfair(char *key, char *msg);

/*
 * scytale: writes message across rows of length message, moving height
 * spaces between letters. Starts at position 0 on row 0, 1 on row 1, etc. 
 * writes down the resulting "stairs" to encode.
 * height: number of rows used to make the "staircases"
 * Example with msg abcdefghij:
 * scytale 3
 * a . . b . . c . . d
 * . e . . f . . g . .   ->   aehbficgjd
 * . . h . . i . . j .
 */
void scytale(int height, char *msg);

// descytale: inverse of scytale
void descytale(int height, char *msg);

/*
 * railfence: writes message alternating between up and down directions
 * then writes out resulting "rails" (left to right, top to bottom) to encode.
 * height: number of rails, height at which direction switches
 * direction: start at the top (1) or bottom (0)
 * Example with msg abcdefghij:
 * railfence 3 1
 * a       e       i
 *   b   d   f   h   j   ->   aeibdfhjcg
 *     c       g
 */
void railfence(int height, int direction, char *msg);

// derailfence: inverse of railfence
void derailfence(int height, int direction, char *msg);

/*
 * spiral: lays out message in a grid, then "spirals" gridded message to encode
 * rows: number of rows in grid
 * cols: number of cols in grid
 * layout_direction: direction message is laid out in grid:
 * 		0: top to bottom, left to right, 1: top to bottom, right to left
 * 		2: bottom to top, left to right, 3: bottom to top, right to left
 * corner: corner in grid where spiraling starts
 * 		0: upper left, 1: upper right, 2: lower right, 3: lower left
 * clockwise: direction of spiral: 1 if clockwise, 0 if counterclockwise
 * Example with msg helloworld:
 * spiral 2 5 0 2 1
 * h l o o l 
 * e l w r d  ->  drwlehlool
 */
void spiral(int rows, int cols, int layout_direction, int corner, int clockwise, char *msg);

// despiral: inverse of spiral
void despiral(int rows, int cols, int layout_direction, int corner, int clockwise, char *msg);

/* 
 * columnar_transposition: lays out message left to right (in rows), top to bottom in a grid. length of key
 * determines length of rows. Encodes by writing out resulting columns using alphabetical order of letters in key.
 * (e.g. in "hello" the column under "e" is written first). Repeated letters in key are normally treated as if
 * letters further to the left come first unless is_myszkowski. Entire process can be repeated using different keys
 * and gaps can be added when writing out letters in grid.
 * keys: alphabetical strings determine number and order of columns based on alphabetical order of letters.
 * multiple keys can be passed using spaces, surrounding complete string with quotes. Will be cycled through
 * if num_transpositions > 1. num_transpositions can be greater than number of keys; keys will just be repeated
 * num_transpositions: number of times columnar_transposition is applied, potentially with different keys
 * is_myszkowski: if 1, columns under repeat letters in keys are transcribed horizontally before vertically
 * disruption_key: string of length >=1; adds gaps when filling grid. gaps are spaced based on alphabetical
 * order of letters in distruption_key; the earliest letter in the alphabet found in the word gives a space 
 * between gaps of 0 (two gaps in a row), the second earliest a space of 1, etc. gaps skipped when encoding.
 * single letter disruption_key treated by adding no gaps (no disruption)
 * Example with msg helloworld:
 * columnar_transposition column 1 0 hello 
 * c o l u m n
 * h * * e l *  ->  hllwlorode
 * l o w * o r 
 * l d
 */
void columnar_transposition(char *keys, int num_transpositions,
							int is_myszkowski, char *disruption_key, char *msg);

// decolumnar: inverse of columnar transposition
void decolumnar(char* keys, int num_transpositions, int is_myszkowski, 
				char* disruption_key, char* msg);

/*
 * bifid: starts with polybius square cipher, returning list of digits. Digits are placed vertically
 * "under" the letters of the message in pairs. The resulting horizontal rows are concatenated,
 * and the altered pairs of integers are re-encoded as letters in the square (using depolybius).
 * key: alphabetical string of any length used to create polybius square
 * Example with msg helloworld:
 * bifid bifid
 *   1 2 3 4 5
 * 1 b i f d a
 * 2 c e g h k      h e l l o w o r l d
 * 3 l m n o p  ->  2 2 3 3 3 5 3 4 3 1  ->  22333534314211424214  ->  enpolrbrrd
 * 4 q r s t u      4 2 1 1 4 2 4 2 1 4
 * 5 v w x y z
 */
void bifid(char *key, char *msg);

// debifid: inverse of bifid
void debifid(char *key, char *msg);

/*
 * trifid: creates 3 3x3 squares using a key, in the same manner as a polybius square, filling one blank at
 * a time until all letters of key are used, and then all positions in all squares are filled with unique
 * letters, with an unused '+' in the final position. Letters are then encoded into 3 digits, the first
 * being the number of the square (starting with 1, in fill order), the 2nd and 3rd being the (row, col)
 * coordinates. As in bifid, the digits are written vertically below the corresponding letters. Digits are
 * concatenated horizontally, then vertically, in groups, with horizontal length determined by group_size.
 * key: alphabetical string of any length used to create the 3 3x3 squares.
 * group_size: an integer > 1 that determines how many letters of the message are re-encoded from digit
 * into new letters in groups.
 * Example with msg helloworld:
 * trifid trifid 5                                                           
 * 1         2         3
 *   1 2 3     1 2 3     1 2 3      h e l l o   w o r l d
 * 1 t r i   1 g h j   1 q s u      2 1 2 2 2   3 2 1 2 1
 * 2 f d a   2 k l m   2 v w x  ->  1 3 2 2 3   2 3 1 2 2  ->  212 221 322 323 222  ->  hkwxlvhsll
 * 3 b c e   3 n o p   3 y z +      2 3 2 2 2   2 2 2 2 2      321 212 312 222 222      
 */
void trifid(char *key, int group_size, char *msg);

// detrifid: inverse of trifid
void detrifid(char *key, int group_size, char *msg);

/*
 * adfgx: a polybius square is made and the message is encoded as in polybius_square, except indices 1-5
 * are replaced with adfgx. This encoding is then passed to a simple columnar_transposition
 * (not is_myszkowski, only 1 transposition, no disruption) with transposition_key as keys.
 * polybius_key: alphabetical string of any length used to create polybius square
 * transposition_key: alphabetical string passed to columnar_transposition (only one, no spaces)
 * Example with msg helloworld:
 * adfgx xgfda edcba
 *    a d f g x                                columnar_transposition
 * a  x g f d a
 * d  b c e h i
 * f  k l m n o  ->  dgdffdfdfxxffxgffdag  ->  fxggffxaddfdgfffddxf
 * g  p q r s t
 * x  u v w y z
 */
void adfgx(char *polybius_key, char *transposition_key, char *msg);

// deadfgx: inverse of adfgx
void deadfgx(char *polybius_key, char *transposition_key, char *msg);

/*
 * adfgvx: a 6x6 square is made as in polybius square, except the numbers 0-9 are added after specific 
 * letters: 1 after a, 2 after b... 0 after 9. Then, like with adfgx, indices are adfgvx and polybius_square 
 * encoding is applied, and a simple columnar_transposition is called with transposition_key as keys
 * polybius_key: alphabetical string of any length used to create 6x6 square with letters and digits
 * transposition_key: alphabetical string passed to columnar_transposition (only one, no spaces)
 * Example with msg hello123:
 * adfgvx xvgfda edcba
 *    a d f g v x                            columnar_transposition
 * a  x v g 7 f 6
 * d  d 4 a 1 b 2 
 * f  c 3 e 5 h 8  ->  fvffgxgxvfdgdxfd  ->  gfffvxfxdvggfxdd
 * g  i 9 j 0 k l
 * v  m n o p q r
 * x  s t u w y z
 */
void adfgvx(char *polybius_key, char *transposition_key, char *msg);

// deadfgvx: inverse of adfgvx
void deadfgvx(char *polybius_key, char *transposition_key, char *msg);