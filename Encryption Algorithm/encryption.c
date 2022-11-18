#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define MAX_BUF  256
#define IV 0b11001011

//forward reference
unsigned char computeKey(unsigned char);
void encode(unsigned char*, unsigned char*, unsigned char);
void decode(unsigned char*, unsigned char*, unsigned char, int);

//forward reference
unsigned char encryptByte(unsigned char, unsigned char);
unsigned char decryptByte(unsigned char, unsigned char);

//forward reference
unsigned char getBit(unsigned char, unsigned char);
unsigned char setBit(unsigned char, unsigned char);
unsigned char clearBit(unsigned char, unsigned char);


int main() {
    // declaration od variables
    char str[8];
    int  choice;
    
    //declaration of variables
    unsigned char key;
    unsigned char plaintext[MAX_BUF];
    unsigned char ciphertext[MAX_BUF];
    unsigned char input[MAX_BUF];
    unsigned char enter;
    int numBytes;

    //ask user if they want to encrypt or decrypt 
    printf("\nYou may:\n");
    printf("  (1) Encrypt a message \n");
    printf("  (2) Decrypt a message \n");
    printf("\n  what is your selection: ");
    fgets(str, sizeof(str), stdin);
    sscanf(str, "%d", &choice);

    switch (choice) {
        // user selects enctyption:
        case 1:
            //prompt user to enter key
            printf("Enter a partial key between 1 and 15 \n");
            scanf("%hhu%c", &key, &enter);
            
            // reprompt until user enters a key between 1-15
            while (key < 1 || key > 15) {
              printf("Error: Partial Key Must Be Between 1 and 15\n");
              scanf("%hhu%c", &key, &enter);
            }
            
            //call compute key with users input as parameter
            key = computeKey(key);

            // promp user to enter plaintext then read
            printf("\nPlease Enter plaintext:\n");
            fgets(plaintext, MAX_BUF, stdin);
            
            //create a new list with values of plaintext
            numBytes = strlen(plaintext);
            for (int i = 0; i < numBytes; ++i) {
                input[i] = plaintext[i];
            }
           
            // run encryption
            encode(input, ciphertext, key);
            
            // display output
            printf("\nCiphertext:\n");
            for (int i = 0; i < numBytes; ++i) {
                printf("%03hhu ", ciphertext[i]);
            }
            
            printf("\nEncryption Complete!\n");

            break;
        
        // user selects decryption:
        case 2:
            //prompt user to enter key
            printf("Enter a partial key between 1 and 15 \n");
            scanf("%hhu%c", &key, &enter);
            
            // reprompt until user enters a key between 1-15
            while (key < 1 || key > 15) {
              printf("Error: Partial Key Must Be Between 1 and 15\n");
              scanf("%hhu%c", &key, &enter);
            }
            
            //call compute key with users input as parameter
            key = computeKey(key);

            // prompt user to enter ciphertext then read
            printf("\nPlease Enter ciphertext:\n");
            unsigned char c;
            int numBytes;
            for (int i = 0; i < sizeof(ciphertext); ++i) {
                scanf("%hhu", &c);
                if (c == 255) {
                    numBytes = i;
                    break;
                }
                else {
                    ciphertext[i] = c;
                }
            }

            // run decryption
            decode(ciphertext, plaintext, key, numBytes);

            // display output
            printf("\nPlaintext:\n");
            for (int i = 0; i < numBytes; ++i) {
                printf("%c", plaintext[i]);
            }
            
            printf("\nDecryption Complete!\n");

            break;

        default:
            // if not 1 or 2 break
            break;
    }

    return 0;
}


/*
  Function:  computeKey
  Purpose:   mirror bits of inputted key to to compute a new key value
       in:   user inputted value of key
   return:   updated value of key
*/
unsigned char computeKey(unsigned char partial) {
    for (int i = 0; i <= 3; ++i) {
        if (getBit(partial, i)) {
            partial = setBit(partial, 7-i);
        }
    }
    return partial;
}


/*
  Function:  encode
  Purpose:   use the given key to encrypt the users inputted plaintext
       in:   plaintext
       in:   ciphertext
       in:   updated key
*/
void encode(unsigned char *pt, unsigned char *ct, unsigned char k) {
    *ct = 0;
    unsigned char temp;
    bool checker;
    bool switcher = false;
    
    if (*pt != '\0') {
        checker = true;
    }
    else {
        checker = false;
    }

    while (checker) {
        if (*pt == '\0') {
            checker = false;
        }
        if (switcher == true) {
            temp = *pt ^ *ct;
            ++ct;
        }
        else {
            temp = *pt ^ IV;
            switcher = true;
        }
        *ct = encryptByte(temp, k);
        ++pt;
    }
}

/*
  Function:  encryptByte
  Purpose:   user inputted key is used to encrypt byte
       in:   key
       in:   byte value
   return:   encypted byte
*/
unsigned char encryptByte(unsigned char src, unsigned char k) {
    
    unsigned char bitHolder = src;
    bitHolder = bitHolder << 2;
    
    for (int i = 0; i < 2; ++i) {
        if (!(getBit(src, (6 + i)))) {
            bitHolder = clearBit(bitHolder, i);
        }
        else {
            bitHolder = setBit(bitHolder, i);
        }
    }
    
    for (int i = 0; i < 8; ++i) {
        if (!(getBit(bitHolder, i) ^ getBit(k, 7-i))) {
            bitHolder = clearBit(bitHolder, i);
        }
        else {
            bitHolder = setBit(bitHolder, i);
        }
    }

    return bitHolder;
}

/*
  Function:  decode
  Purpose:   use the given key to decrypt the users inputted ciphertext
       in:   plaintext
       in:   ciphertext
       in:   updated key
*/

void decode(unsigned char *ct, unsigned char *pt, unsigned char k, int numBytes) {
    //code wont print out first letter 
    unsigned char src;
    for (int i = numBytes; i > 0; i--){
      src = decryptByte(ct[i],k);
      pt[i] = src ^ ct[i-1];
      if (i ==0){
          pt[i] = src ^ ct[0];
      }
    }
    src = decryptByte(IV,k);
}

/*
  Function:  decryptByte
  Purpose:   user inputted key is used to decrypt byte
       in:   ciphertext bytes
       in:   byte value
   return:   encypted byte
*/
unsigned char decryptByte(unsigned char ct, unsigned char k) {
    
    unsigned char bitHolder = ct;

    for (int i = 0; i < 8; ++i) {
        if (getBit(bitHolder, i) ^ getBit(k, 7-i)) {
            bitHolder = setBit(bitHolder, i);
        }
        else {
            bitHolder = clearBit(bitHolder, i);
        }
    }

    bitHolder = bitHolder >> 2;
    
    for (int i = 0; i < 2; ++i) {
        if (getBit(ct, (1 - i))) {
            bitHolder = setBit(bitHolder, 7-i);
        }
        else {
            bitHolder = clearBit(bitHolder, 7-i);
        }
    }
    return bitHolder;
}

/*
  Function:  getBit
  Purpose:   retrieve value of bit at specified position
       in:   character from which a bit will be returned
       in:   position of bit to be returned
   return:   value of bit n in character c (0 or 1)
*/
unsigned char getBit(unsigned char c, unsigned char n) {
    return (c & (1 << n)) >> n;
}

/*
  Function:  setBit
  Purpose:   set specified bit to 1
       in:   character in which a bit will be set to 1
       in:   position of bit to be set to 1
   return:   new value of character c with bit n set to 1
*/
unsigned char setBit(unsigned char c, unsigned char n) {
    return c | (1 << n);
}

/*
  Function:  clearBit
  Purpose:   set specified bit to 0
       in:   character in which a bit will be set to 0
       in:   position of bit to be set to 0
   return:   new value of character c with bit n set to 0
*/
unsigned char clearBit(unsigned char c, unsigned char n) {
    return c & (~(1 << n));
}
