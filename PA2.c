#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Dual Programmer Affidavit
// I the undersigned promise that the attached assignment is my own work. 
// While I was free to discuss ideas with others, the work contained is my own. 
// I recognize that should this not be the case, I will be subject to penalties as outlined in the course syllabus. 
// Your Name: Aron Chen 
// Red ID: 825066119
// James Scheiber
// 825617332


// Define the data type INSTR
typedef unsigned int INSTR;


const static bool format2[] = {
    false,false,true,false,true,false,
    false,true,false,false,true,false,
    false,false,false,false,false,false,
    false,false,false,false,false,false,
    false,false,false,false,false,false,
    true,false,false,false,true,false,
    true,true,false,false,false,false,
    false,false,false,false,false,false,
    false,false,false,false,true,true,
    false,false,false,true,false
};


// getINTR uses bitwise masking to the first two bit then finds the corresponding value index in
// *[]ops then returns the same index in *mnemonics[], it also returns true is it's format 2
INSTR getINSTR(const char *test, const char *ops[], const char *mnemonics[], int arraySize, char *mnemonic, bool *isFormat2) {
   // Convert the string into integer
   unsigned int testValue = (unsigned int)strtol(test, NULL, 16);

   // Bitwise AND with FC0 then shift right by 4 bits
   INSTR INSTRValue = (testValue & 0xFC0) >> 4;

   // Search for INSTR value in ops[] and retrieve the corresponding mnemonic
    for (int i = 0; i < arraySize; i++) {
        if (INSTRValue == (INSTR)strtol(ops[i], NULL, 16)) {
         strcpy(mnemonic, mnemonics[i]);
        
         // Determine if it's format 2
         *isFormat2 = format2[i];
         
         return INSTRValue;
        }
    }

    return 0;
}

// Finds OAT value by using bitwise maksing and returning simple, immediate, indirect, and unknown
// depending on switch case
char *getOAT(const char *test) {
    // Convert the 4-digit hexadecimal string to an integer
    unsigned int testValue = (unsigned int)strtol(test, NULL, 16);

    // bitwise AND to get the last two bits
    char oat = (char)(testValue & 0x30);
   
   // Chang OAT values to corresponding strings
    switch (oat) {
        case 0x00:
            return "simple";
        case 0x10:
            return "immediate";
        case 0x20:
            return "indirect";
        case 0x30:
            return "simple";
        default:
            return "unknown";
    }
}

// uses bitwise masking to get the E flag
int getFORMAT(const char *test) {

    unsigned int testValue = (unsigned int)strtol(test, NULL, 16);

    // Bitwise operation and checks the last bit
    if (testValue & 0x01) {
        return 4;
    } else {
        return 3;
    }
}

// Bitwise operation to get the TAAM, and returning depending on the switch case
const char *getTAAM(const char *test) {
   // Convert string into integer
   unsigned int testValue = (unsigned int)strtol(test, NULL, 16);

   // Bitwise masking with 0x0E
   unsigned int result = (unsigned int)(testValue & 0x0E);

    switch (result) {
        case 0x00:
            return "absolute";
        case 0x02:
            return "pc";
        case 0x04:
            return "base";
        case 0x0A:
            return "pc_indexed";
        case 0x0C:
            return "base_indexed";
        default:
            return "unknown";
    }
}


const static char *ops[] = {
    "18", "58", "90", "40", "B4", "28",
    "88", "A0", "24", "64", "9C", "C4",
    "C0", "F4", "3C", "30", "34", "38",
    "48", "00", "68", "50", "70", "08",
    "6C", "74", "04", "D0", "20", "60",
    "98", "C8", "44", "D8", "AC", "4C",
    "A4", "A8", "F0", "EC", "0C", "78",
    "54", "80", "D4", "14", "7C", "E8",
    "84", "10", "1C", "5C", "94", "B0",
    "E0", "F8", "2C", "B8", "DC"
};


const static char *mnemonics[] = {
    "ADD", "ADDF", "ADDR", "AND", "CLEAR", "COMP",
    "COMPF", "COMPR", "DIV", "DIVF", "DIVR", "FIX",
    "FLOAT", "HIO", "J", "JEQ", "JGT", "JLT",
    "JSUB", "LDA", "LDB", "LDCH", "LDF", "LDL",
    "LDS", "LDT", "LDX", "LPS", "MUL", "MULF",
    "MULR", "NORM", "OR", "RD", "RMO", "RSUB",
    "SHIFTL", "SHIFTR", "SIO", "SSK", "STA", "STB",
    "STCH", "STF", "STI", "STL","STS", "STSW",
    "STT", "STX", "SUB", "SUBF", "SUBR", "SVC",
    "TD", "TIO", "TIX", "TIXR", "WD"
};


int main(int argc, char **argv) {
// reads in the file
char test[100];
char pName[7];       // Array to store the characters from columns 2 to 7 and a null terminator
char startAddress[7]; // Array to store the characters from columns 8 to 13 and a null terminator
FILE *ptr = fopen(argv[1], "r");
int line;
int k = 0;

int LOC = 0x0;
char spacing[] = "        ";
char label[6] = "     ";
char operand;

// This while loop goes to the end of the file, processing lines based on their first character
while ((line = fgetc(ptr)) != EOF) {
    if (line == 'H') {
        // Read columns 2 to 7 into pName
        fread(pName, sizeof(char), 6, ptr);
        pName[6] = '\0'; // Null-terminate to make it a valid string
        
        // Read columns 8 to 13 into startAddress
        fread(startAddress, sizeof(char), 6, ptr);
        startAddress[6] = '\0'; // Null-terminate to make it a valid string
        
        // Skip the rest of the line
        while ((line = fgetc(ptr)) != '\n') {
            if (line == EOF) {
                break; 
            }
        }
    }
    else if (line == 'T') { 
        fseek(ptr, 8, SEEK_CUR);
        int c;
        while ((c = fgetc(ptr)) != '\n') { 
            if (c == EOF) {
                break; 
            }
            test[k] = c;
            ++k;
        }  
    }
}

    // creating file output
    FILE *out_ptr = fopen("out.lst", "w");


    // TODO: remove the header line, it's not in the final output, but probably helps for prototyping :)

    fprintf(out_ptr, "%-12s%-12s%-12s%-12s%-12s\n", "LOC", "Label", "Opcode", "Operand", "ObjectCode");
    fprintf(out_ptr, "%04X%s%-12s%-12s%-12X\n", strtol(startAddress, NULL, 16), spacing, pName, "START", strtol(startAddress, NULL, 16));



    // Allocate space for the mnemonic 
    char mnemonic[7]; 

    // This while loop gets the menmonics first, determines if it's format 3,4 or 2, and then calls
    // the funcitons above, then prints the results out
    int i = 0;
    while (i < strlen(test)) {
        
        // Create a character array for 3 hex digits, then copy 3 char into formatChunk
        char formatChunk[4]; 
        strncpy(formatChunk, &test[i], 3); 
        formatChunk[3] = '\0'; 

        // Pass the formatChunk to getFORMAT
        int format = getFORMAT(formatChunk); 

        // Extract the chunk based on the format
        char chunk[10];

        // Putting either 6 or 8 depending on the format
        strncpy(chunk, &test[i], (format == 3) ? 6 : 8);
        chunk[(format == 3) ? 6 : 8] = '\0';

        // calls getINSTR 
        INSTR result;
        bool isFormat2 = false;
        result = getINSTR(formatChunk, ops, mnemonics, sizeof(ops) / sizeof(ops[0]), mnemonic, &isFormat2);
        
        // It's format 2, so print only mnemonic and 4 hex digits
        if (isFormat2) {
            // Create a  array for 4 hex digits, then copy the first 4 char from chunk
            char format2Chunk[5]; 
            strncpy(format2Chunk, chunk, 4);
            format2Chunk[4] = '\0'; 
            
            //Prints out the with the format 2
            fprintf(out_ptr, "%04X%s%-12s%-12s%-12s%-12s\n", LOC, spacing, label, mnemonic, operand, format2Chunk);
            // Increment i by 4 and LOC by 2 (4 half bytes)
            i += 4;
            LOC += 2;
        
        } else {
            // Calls the function if it's format 3 and 4 and print out the result
            const char *oat = getOAT(formatChunk); 
            const char *taam = getTAAM(formatChunk); 

            fprintf(out_ptr, "%04X%s%-12s%-12s%-12s%-12s\n", LOC, spacing, label, mnemonic, operand, chunk);
            // Increment by 6 or 8 depending on the format, as well as LOC by as many half bytes
            i += (format == 3) ? 6 : 8; 
            LOC += (format == 3) ? 3 : 4; 
        }

        if (!strcmp("LDB", mnemonic)) {
            fprintf(out_ptr, "%24s%-12s%-12s\n", "", "BASE", operand);
        }
    }

    fprintf(out_ptr, "%24s%-12s%-12s\n", "", "END", pName);

    // clsoe file
    fclose(out_ptr);
    fclose(ptr);
    return 0;
}