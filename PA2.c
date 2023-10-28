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


const char *getOperand(const char *chunk, int format, int LOC, long lastBASE, int lastX, const char *oat, const char *taam) {
    if (format == 3){
        char *operandChar = (char*) malloc(3);
        int operand = strtol(chunk, NULL, 16);
        operand = operand & 0xFFF;

        // skeleton for operand calculation
        if (!strcmp(oat, "simple")) {
            // ta = disp + (pc or b)
            if (!strcmp(taam, "pc") || !strcmp(taam, "pc_indexed")) {
                // ta = disp + pc
                operand += (LOC + 3); // LOC hasn't been incremented when it is passed to this function
            } else if (!strcmp(taam, "base") || !strcmp(taam, "base_indexed")) {
                operand += lastBASE;
            } else {
                // direct, ta = disp
            }
        } else if (!strcmp(oat, "immediate")) {
            // ta = ta
        } else {
            // indirect, memory access, ((TA))
        }

        if (strlen(taam) > 9) {
            // indexed so add lastx
            operand += lastX;
        }
        
        snprintf(operandChar, sizeof(operandChar), "%04X", operand);
        return operandChar;
    } else {
        // copy last 4 hex digits from value for TA
        char *operand = (char*) malloc(4);
        strncpy(operand, chunk + 4, 4);
        return operand;
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

// Declaring for I/O
//********************************************************************************************
    // reads in the file
    char test[100];
    // Characters from columns 2 to 7 and a null terminator
    char pName[7];
    // Characters from columns 8 to 13 and a null terminator       
    char startAddress[7];
    // Opens the file 
    FILE *ptr = fopen(argv[1], "r");
    // creating file output
    FILE *out_ptr = fopen("out.lst", "w");
    int line;
    int k = 0;
//********************************************************************************************

// Declaring for main function to run
//********************************************************************************************    
    char locString[5];
    int LOC = 0x0;
    char label[6] = "     ";
    char operand;
    long lastBASE = 0x0;
    long lastX = 0x0;
    // Allocate space for the mnemonic 
    char mnemonic[7]; 
//********************************************************************************************

// THIS IS TO READ/RUN THE "MAIN" PROGAM
//********************************************************************************************
    // This while loop goes to the end of the file, processing lines based on their first character
    while ((line = fgetc(ptr)) != EOF) {
        if (line == 'H') {
            // Read columns 2 to 7 into pName
            fread(pName, sizeof(char), 6, ptr);
            // Null-terminate to make it a valid string
            pName[6] = '\0'; 
            
            // Read columns 8 to 13 into startAddress
            fread(startAddress, sizeof(char), 6, ptr);
            // Null-terminate to make it a valid string
            startAddress[6] = '\0'; 
            
            // Skip the rest of the line
            while ((line = fgetc(ptr)) != '\n') {
                if (line == EOF) {
                    break; 
                }
            }
        // Prints out the Start File thing
        fprintf(out_ptr, "%04lX %-12s%-12s%-12lX\n", strtol(startAddress, NULL, 16), pName, "START", strtol(startAddress, NULL, 16));
        } else if (line == 'T') { 
            k = 0;
            int c;
            // Getting the LOC from coulumn 4 to 7
            //**********************************************************************
            fseek(ptr, 2, SEEK_CUR);
            fread(locString, sizeof(char), 4, ptr);
            locString[4] = '\0';
            LOC = (int)strtol(locString, NULL, 16);
            // Getting the Object Code starting coulumn 10
            //********************************************************************** 
            fseek(ptr, 2, SEEK_CUR);   
            while ((c = fgetc(ptr)) != '\n') { 
                if (c == EOF) {
                    break; 
                }
                test[k] = c;
                ++k;

            }
            // Start of "main" execution
            //******************************************************************************************************************
            // need this to ensure that it is null terminated, so its a valid String 
            test[k] = '\0';
            printf("THIS IS WHAT IS IN TEST: %s\n", test);

            int i = 0;
            // This while loop gets the menmonics first, determines if it's format 3,4 or 2, and then calls
            // the funcitons above, then prints the results out
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

                const char *operand;
        
                // It's format 2, so print only mnemonic and 4 hex digits
                if (isFormat2) {
                    // Create a  array for 4 hex digits, then copy the first 4 char from chunk
                    char format2Chunk[5]; 
                    strncpy(format2Chunk, chunk, 4);
                    format2Chunk[4] = '\0';
                    

                    // Extract the relevant hex digit
                    // Third hex digit represents bits 9 to 12
                    char hexDigit = format2Chunk[2]; 

                    // Map the hex digit to register names
                    const char *registerName = "";
                    switch (hexDigit) {
                        case '0': registerName = "A"; break;
                        case '1': registerName = "X"; break;
                        case '2': registerName = "L"; break;
                        case '3': registerName = "B"; break;
                        case '4': registerName = "S"; break;
                        case '5': registerName = "T"; break;
                        case '6': registerName = "F"; break;
                        case '7': registerName = "PC"; break;
                        case '8': registerName = "SW"; break;
                        // You might want to handle invalid cases as well
                        default:  registerName = "Unknown"; break;
                    }


                    //Prints out the with the format 2
                    fprintf(out_ptr, "%04X %-12s%-12s%-12s%-12s\n", LOC, label, mnemonic, registerName, format2Chunk);
                    // Increment i by 4 and LOC by 2 (4 half bytes)
                    i += 4;
                    LOC += 2;
        
                } else {
                    // Calls the function if it's format 3 and 4 and print out the result
                    const char *oat = getOAT(formatChunk);
                    const char *taam = getTAAM(formatChunk);
                    operand = getOperand(chunk, format, LOC, lastBASE, lastX, oat, taam);

                    // Operand Sign
                    char operandSign[2] = ""; 
            
                    // Checking what the Operand Sign should be by checkign the OAT "" by default
                    if (strcmp(oat, "immediate") == 0) {
                        strcpy(operandSign, "#");
                    } else if (strcmp(oat, "indirect") == 0) {
                        strcpy(operandSign, "@");
                    }  

                    if (format == 4) {
                        fprintf(out_ptr, "%04X %-12s+%-11s%s%-11s%-12s\n", LOC, label, mnemonic, operandSign, operand, chunk);
                    } else {
                        fprintf(out_ptr, "%04X %-12s%-12s%s%-12s%-12s\n", LOC, label, mnemonic, operandSign, operand, chunk);
                    }
                    // Increment by 6 or 8 depending on the format, as well as LOC by as many half bytes
                    i += (format == 3) ? 6 : 8; 
                    LOC += (format == 3) ? 3 : 4; 
                }


                // TODO: may need to alter if + is part of string at this point
                if (!strcmp("LDB", mnemonic)) {
                    lastBASE = strtol(operand, NULL, 16);
                    fprintf(out_ptr, "%17s%-12s%-12s\n", "", "BASE", operand);
                } else if (!strcmp("LDX", mnemonic)) {
                    lastX = strtol(operand, NULL, 16);
                    // printf("%x\n", lastX);
                }
            }
            memset(test, 0, sizeof(test));
            // END OF "main" Execution
            //******************************************************************************************************************
        }
    }
//********************************************************************************************
    


    fprintf(out_ptr, "%17s%-12s%-12s\n", "", "END", pName);

    // clsoe file
    fclose(out_ptr);
    fclose(ptr);
    return 0;
}