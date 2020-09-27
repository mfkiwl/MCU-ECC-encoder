#include <stdio.h>
#include <stdint.h>

#define CODE_LENGTH 8
#define MESSAGE_LENGTH 2
#define PARITY_DIM 5

/* A Reed-Solomon code is generated by a Vandermonde matrix over some field.
 * Because of the provided communication functions in the funkit API, it is easiest
 * to communicate data as bytes (ie by sending a char). Thus, it is best for the 
 * binary representation of the code to have length a multiple of 8.
 * 
 * In my case I have decided to work over the field F_8 which is isomorphic to (Z_2)^3.
 * Therefore every element of my field has a binary representation of length 3, and the
 * transmitted length of my code is 3 bytes. I have chosen a message length of k = 2,
 * which is 6 bits in binary transmission. Thus I have 64 possible code words in my code
 * and a distance of d = n - k + 1 = 7. This implies I can correct any transmission with
 * less than or equal to 37.5% bit error rate. */


/* I am generating my chosen field, F_8 with the irreducible polynomial x^3 + x^2 + 1 
 * over Z_2. This gives me the following equivalent representations for elements of F_8:
 * 
 * 
 *   Power:   |   Additive:   |  Binary:  | Decimal:
 * --------------------------------------------------------
 *      0     |       0       |  (0,0,0)  |    0
 *      1     |       1       |  (0,0,1)  |    1
 *      x     |       x       |  (0,1,0)  |    2
 *     x^2    |      x^2      |  (1,0,0)  |    4
 *     x^3    |    x^2 + 1    |  (1,0,1)  |    5
 *     x^4    |  x^2 + x + 1  |  (1,1,1)  |    7 
 *     x^5    |     x + 1     |  (0,1,1)  |    3
 *     x^6    |    x^2 + x    |  (1,1,0)  |    6 
 * 
 * 
 * Binary represents what these elements look like to the IR receiver. Power and Additive
 * forms are both useful for multiplying and adding elements respectively. The decimal
 * notation sheds little light on the behavior of the field, I merely use it as a convenient
 * representation for the coding side.
 */

//indexed by decimal representation of element. eg x^3 * x^6 = mutliplication_table[5][6] (or [6][5])
int multiplication_table[CODE_LENGTH][CODE_LENGTH] = {
    {0,0,0,0,0,0,0,0},
    {0,1,2,3,4,5,6,7},
    {0,2,4,6,5,7,1,3},
    {0,3,6,5,1,2,7,4},
    {0,4,5,1,7,3,2,6},
    {0,5,7,2,3,6,4,1},
    {0,6,1,7,2,4,3,5},
    {0,7,3,4,6,1,5,2}
};

int generator_matrix[MESSAGE_LENGTH][CODE_LENGTH] = {
    {1,1,1,1,1,1,1,1},
    {0,1,2,3,4,5,6,7}
};

int parity_check_matrix[PARITY_DIM][CODE_LENGTH] = {
    {1,1,1,1,1,1,1,1},
    {0,1,2,3,4,5,6,7},
    {0,1,4,5,7,6,3,2},
    {0,1,5,2,6,4,7,3},
    {0,1,7,6,2,3,5,4}
};

void multiply(int vector[], int matrix[][CODE_LENGTH], int result[], int vector_length)
{
    int sum;
    for (int i = 0; i < CODE_LENGTH; i++) {
        sum = 0;
        for (int j = 0; j < vector_length; j++) {
            sum += multiplication_table[vector[j]][matrix[j][i]];
        }
        result[i] = sum % CODE_LENGTH;
    }
}

void convert_to_ascii(int vector[], uint8_t string[])
{
    string[0] = (vector[0] << 5) | (vector[1] << 2) | (vector[2] >> 1);
    string[1] = ((vector[2] & 001) << 7) | (vector[3] << 4) | (vector[4] << 1) | (vector[5] >> 2);
    string[2] = ((vector[5] & 011) << 6) | (vector[6] << 3) | vector[7];
    string[3] = '\0';
}


void encode(int message, uint8_t string[]) 
{
    //split message into 2 3 bit sequences
    int a = message >> 3;
    int b = message & ~(111 << 3);
    
    int message_vector[] = {a, b};
    int codeword[CODE_LENGTH] = {0};
    
    multiply(message_vector, generator_matrix, codeword, MESSAGE_LENGTH);
    convert_to_ascii(codeword, string);
}

int main(void)
{
    uint8_t m[4];
    encode(33, m);
    for (int i = 0; i < 4; i++) {
        printf("%c\n", m[i]);
    }
}


