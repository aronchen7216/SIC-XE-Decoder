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

// Getst the operand value based on PC/Base/Direct bits
const char *getOperand(const char *chunk, int format, int LOC, long lastBASE, int lastX, const char *oat, const char *taam, int isEndOfTLine, int nextLoc, int nextNewStartingAddress, int tLineCounter) {
    if (format == 3){
        char *operandChar = (char*) malloc(3);
        int operand = strtol(chunk, NULL, 16);
        operand = operand & 0xFFF;

        // skeleton for operand calculation
        if (!strcmp(oat, "simple") || !strcmp(oat, "indirect")) {
            // ta = disp + (pc or b)
            if (!strcmp(taam, "pc") || !strcmp(taam, "pc_indexed")) {
                operand = (0xFFF - operand) + 1;
                // if the program has reached the end of a T line, use the starting address of the following line for the PC value
                if (isEndOfTLine == 0 || nextNewStartingAddress > tLineCounter){
                    operand = (LOC + 3) - operand; // LOC hasn't been incremented when it is passed to this function
                } else {
                    operand = nextLoc - operand;
                }
                operand = operand & 0xFFF;
            } else if (!strcmp(taam, "base") || !strcmp(taam, "base_indexed")) {
                printf("BASE: %lX\n", lastBASE);
                operand += lastBASE;
                printf("AFTER %x\n", operand);
            } else {
                // direct, ta = disp
            }
        } else if (!strcmp(oat, "immediate")) {
            // ta = ta
        } else {
            // indirect, memory access, ((TA))
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
    FILE *symptr = fopen(argv[2], "r");
    // creating file output
    FILE *out_ptr = fopen("out.lst", "w");
    int line;
    int k = 0;
    int symbolRowCountTop = 0;
    int symbolRowCountBottom = 0;
//********************************************************************************************

// Declaring for main function to run
//********************************************************************************************    
    char locString[5];
    int LOC = 0x0;
    int nextLoc = 0x0;
    char label[6] = "     ";
    char operand;
    long lastBASE = 0x0;
    long lastX = 0x0;
    int nextNewStartingAddress = 1;
    int isEndOfTLine = 0;
    // Allocate space for the mnemonic 
    char mnemonic[7]; 
//********************************************************************************************

// THIS IS TO READ/RUN THE "MAIN" PROGAM
//********************************************************************************************

    // Count the number of T lines to know how large to make the array to store the data
    char firstLine[100];
    int tLineCounter = 0;
    while(fgets(firstLine, sizeof(firstLine), ptr)){
        char *to = (char*) malloc(1);
        if (!strcmp(to, "T")){
            tLineCounter += 1;
        }
    }
    fclose(ptr);

    // Add the starting address of each T line to an array for use with PC-relative calculations
    char allStartingAddresses[tLineCounter][5];
    ptr = fopen(argv[1], "r");
    int addrAddedCounter = 0;
    while(fgets(firstLine, sizeof(firstLine), ptr)){
        char *to = (char*) malloc(1);
		strncpy(to, firstLine, 1);
        if (!strcmp(to, "T")){
		    strncpy(allStartingAddresses[addrAddedCounter], firstLine + 3, 4);
            allStartingAddresses[addrAddedCounter][4] = *"\0";
            addrAddedCounter += 1;
        }
    }

    fclose(ptr);
    ptr = fopen(argv[1], "r");

    // Counts the number of rows in the symbol table so it can "dynamically" allocate the correct resources for the arrays
    int section = 0;
    while ((line = fgetc(symptr)) != EOF) {
        if (section == 0){
            if (line == *"\n"){
                symbolRowCountTop += 1;
            } else if (line == *"N"){
                section += 1;
            }
        } else {
            if (line == *"\n"){
                symbolRowCountBottom += 1;
            }
        }
    }

    fclose(symptr);
    symptr = fopen(argv[2], "r");

    // Row count correction for --- and header lines
    symbolRowCountTop -= 3;
    symbolRowCountBottom -= 2;

    // So many arrays for each column of the symbol table, separated by the first and section sections
    char symbolsTopChars[symbolRowCountTop][10];
    char symbolsTopAddress[symbolRowCountTop][10];

    char symbolsBottomChars[symbolRowCountBottom][10];
    char symbolsBottomConst[symbolRowCountBottom][10];
    char symbolsBottomLength[symbolRowCountBottom][10];
    char symbolsBottomAddress[symbolRowCountBottom][10];

    section = 0;
    int rowCounter = 0;
    char symLine[100];
    char LOC_char[10];

    // Adds the symbol table data to the associated arrays
    while(fgets(symLine, sizeof(symLine), symptr)){
        char *to = (char*) malloc(6);
		strncpy(to, symLine, 6);
        if (strcmp(to, "Symbol") && strcmp(to, "------") && strcmp(to, "\n")){
            if (!strcmp(to, "Name  ")) {
                section += 1;
                rowCounter = 0;
            } else if (section == 0) {
                char* token = strtok(symLine, " ");
                strcpy(symbolsTopChars[rowCounter], token);
                token = strtok(NULL, " ");
                strcpy(symbolsTopAddress[rowCounter], token);
                rowCounter += 1;
            } else if (section == 1) {
                char* token = strtok(symLine, " ");
                strcpy(symbolsBottomChars[rowCounter], token);
                token = strtok(NULL, " ");
                strcpy(symbolsBottomConst[rowCounter], token);
                token = strtok(NULL, " ");
                strcpy(symbolsBottomLength[rowCounter], token);
                token = strtok(NULL, " ");
                strcpy(symbolsBottomAddress[rowCounter], token);
                symbolsBottomAddress[rowCounter][ 6 ] = *"\0";
                rowCounter += 1;
            }
        }
        
    }

    // for (int t=0; t < symbolRowCountBottom; t++){
    //     printf("addr %s\n", symbolsBottomAddress[t]);
    // }


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

            int i = 0;
            // This while loop gets the menmonics first, determines if it's format 3,4 or 2, and then calls
            // the funcitons above, then prints the results out
            while (i < strlen(test)) {

                if ((strlen(test) - i) < 7){
                    nextLoc = (int)strtol(allStartingAddresses[nextNewStartingAddress], NULL, 16);
                    nextNewStartingAddress += 1;
                    isEndOfTLine = 1;
                } else {
                    isEndOfTLine = 0;
                }

                // printf("%d\n", (strlen(test) - i));
        
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

    			snprintf(LOC_char, sizeof(LOC_char), "%06X", LOC);

                for (int i = 0; i < symbolRowCountTop; i++){
                    if (!(strcmp(symbolsTopAddress[i], LOC_char))){
                        snprintf(LOC_char, sizeof(LOC_char), "%06X", LOC);
                        strcpy(label, symbolsTopChars[i]);
                    }
                }

                int isConst = 0;
                // Check for location in symbol table, output accordingly from earlier defined arrays
                snprintf(LOC_char, sizeof(LOC_char), "%06X", LOC);
                for (int x = 0; x < symbolRowCountBottom; x++){
                    // printf("addr %s   LOC %s \n", symbolsBottomAddress[x], LOC_char);
                    if (!(strcmp(symbolsBottomAddress[x], LOC_char))){
                        isConst = 1;
                        char *address = (char*) malloc(6);
                        strncpy(address, symbolsBottomConst[x] + 2, strlen(symbolsBottomConst[x]) - 3);
                        fprintf(out_ptr, "%04X %s%-12s%-12s%-12s%-12s\n", LOC, "", symbolsBottomChars[x], "BYTE", symbolsBottomConst[x], address);
                        if (isEndOfTLine == 0){
                            LOC += atoi(symbolsBottomLength[x]) / 2;
                        }
                        i += atoi(symbolsBottomLength[x]);
                    }
                }

                if (isConst == 0){

                    // It's format 2, so print only mnemonic and 4 hex digits
                    if (isFormat2) {
                        // Create a  array for 4 hex digits, then copy the first 4 char from chunk
                        char format2Chunk[5]; 
                        strncpy(format2Chunk, chunk, 4);
                        format2Chunk[4] = '\0';
                        
                        format = 2;

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
                        printf("%d\n", format);
                        // LOC += 2;
            
                    } else {
                        // Calls the function if it's format 3 and 4 and print out the result
                        const char *oat = getOAT(formatChunk);
                        const char *taam = getTAAM(formatChunk);
                        operand = getOperand(chunk, format, LOC, lastBASE, lastX, oat, taam, isEndOfTLine, nextLoc, nextNewStartingAddress, tLineCounter);

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
                        // LOC += (format == 3) ? 3 : 4;
                    }

                    // Store base register and index values
                    if (!strcmp("LDB", mnemonic)) {
                        lastBASE = strtol(operand, NULL, 16);
                        fprintf(out_ptr, "%17s%-12s%-12s\n", "", "BASE", operand);
                    } else if (!strcmp("LDX", mnemonic)) {
                        lastX = strtol(operand, NULL, 16);
                    }



                    LOC += format;

                    // reset label variable
                    strcpy(label, "     ");
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
    fclose(symptr);
    return 0;
}