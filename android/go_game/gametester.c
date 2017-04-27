#include <stdio.h>
#include <string.h>
///
/// Main function to run the program
///

#define OUT_SIZE 8192
#define MAX_FILENAME 256
extern int game_run(const char *filename, char *out_results, int resultsLen);

///
/// Writes to output file
///
void writeOutputFile(
    char*outputFile,
    char*output,
    int outputSize)
{
    FILE *fptr;
    fptr = fopen(outputFile, "w");
    if (fptr == NULL)
    {
        printf("Error opening file %s\n", outputFile);
        return;
    }

    fprintf(fptr, "%s", output);
    fclose(fptr);
}

void performTests(
    const char* input, 
    char*outputFile, 
    char*output, 
    int outputSize) 
{
    int status = 0;
    memset(output, 0, OUT_SIZE);
    status = game_run(input, output, OUT_SIZE);

    writeOutputFile(outputFile, output, outputSize);
}

int main(int argc, char *argv[]) { 

    int status = 0;
    
    if (argc == 2) {
        char output[OUT_SIZE];
        memset(output, 0, OUT_SIZE);
        game_run(argv[1], output, OUT_SIZE);
        
        if (strlen(output) > 0) {
            char outFile[MAX_FILENAME];
            memset(outFile, 0, MAX_FILENAME);
            strcpy(outFile, argv[1]);
            strcat(outFile, ".out");

            writeOutputFile(outFile, output, OUT_SIZE);
        }
    }
    else {
        printf("%s", "Usage: game <input file>\n");
    }
    
    return status;
}

