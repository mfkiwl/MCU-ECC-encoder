# ATMega32U2-communications-encoder
An implementation of a Reed-Solomon code to enable error detection and correction in IR transmissions between ATMega32u2 microcontrollers used in UC funkits.

Code is specifically designed with limited program memory and communication in mind, so there are just 16 codewords, encoded over the field on 4 elements, which translates nicely into binary. Keeping the code small means that syndrome decoding is a viable decoding option, which is the fastest option, but requires the storage of each syndrome in memory. The code can correct any number of errors less than or equal to a 25% error rate per codeword.
