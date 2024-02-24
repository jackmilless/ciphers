# Ciphers

## Project description
14 ciphers are implemented to encode messages. Inverse functions for each cipher are also provided for decoding. Several functions can be called successively with appropriate arguments to chain multiple encoding and/or decoding functions. 

Compile: `gcc ciphers.c -o ciphers`


## How to run
```
./ciphers <msg> <is_capitalized> ...<cipher_name> [args]...

<msg>: the message you would like to encode or decode, alphabetic characters allowed; other characters (including spaces)
       will be removed if present before encoding/decoding.
<is_capitalized>: a boolean (0 or 1) indicating whether the output is in all caps
<cipher_name> [args]: encoding or decoding function followed by a list of arguments depending on the function
Valid functions and their arguments, as well as a brief description and example of how each one works, are found in ciphers.h.
Chain functions by alternating between ciphers and their arguments.
```

## Examples
```
./ciphers helloworld 1 atbash
SVOOLDLIOW
./ciphers helloworld 0 polybius_square polybiuskey
41251313125312451333
./ciphers helloworld 0 railfence 3 0
loelwrdhol
./ciphers helloworld 1 columnar_transposition "colum nar" 3 1 transpose
WLOEOHDRLL
./ciphers WLOEOHDRLL 0 decolumnar "colum nar" 3 1 transpose
helloworld
./ciphers helloworld 0 caesar 3 foursquare keyone keytwo adfgx polybiuskey columnarkey
dafdaxdfaaffgdxaffgd
```
