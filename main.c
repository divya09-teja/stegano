#include <stdio.h>                      // Includes standard input/output functions
#include <string.h>                     // Includes string functions like strcmp()
#include "encode.h"                     // Includes encode-related declarations
#include "decode.h"                     // Includes decode-related declarations
#include "types.h"                      // Includes custom types like Status and OperationType

OperationType check_operation_type(char *);   // Function prototype to check -e or -d

int main(int argc, char *argv[])        // Main function: argc = count of arguments, argv = array of arguments
{
    /* At least 4 args for encode, 3 args for decode */
    if (argc >= 3)                       // Check if enough arguments are provided
    {
        if (check_operation_type(argv[1]) == e_encode)  // If user gave -e (encode)
        {
            printf("*---------------------------------------*\n");
            printf("LSB IMAGE STEGANOGRAPHY - ENCODER\n");
            printf("*-----------------------------------------*\n");
            EncodeInfo encInfo;          // Declare structure to hold encoding information

            if (read_and_validate_encode_args(argv, &encInfo) == e_failure)  // Validate encode inputs
                return 0;                // Stop program if validation failed

            if (do_encoding(&encInfo) == e_failure)      // Perform encoding process
                return 0;                // Stop if encoding failed

            return 0;                    // Successful encode end of program
        }
        else if (check_operation_type(argv[1]) == e_decode)  // If user gave -d (decode)
        {
            printf("*-----------------------------------------*\n");
            printf("LSB IMAGE STEGANOGRAPHY - DECODER\n");
            printf("*-----------------------------------------*\n");
            DecodeInfo decInfo;          // Declare structure to hold decoding information

            if (read_and_validate_decode_args(argv, &decInfo) == e_failure)  // Validate decode inputs
            {
                printf("ERROR: ARGUMENT VALIDATION FAILED. PLEASE CHECK FILE NAMES AND EXTENSIONS.\n");
                 return 0;                // Stop if validation failed
            }
               

            if (do_decoding(&decInfo) == e_failure)       // Perform decoding
            {
                printf("ERROR: DECODING FAILED. PROCESS COULD NOT BE COMPLETED.\n");
                return 0;                // Stop if decode failed
            }
    
            return 0;                    // Successful decode end of program
        }
        else
        {
            printf("Error: Unsupported operation! Use -e for encode, -d for decode.\n");  // Print error if user didn't use -e or -d
            return 0;                      // Stop program
        }
    }

    /* If arguments are too few */
    printf("Error: Insufficient arguments!\n");           // Message for not enough arguments
    printf("Usage:\n");                                   // Display correct usage format
    printf("  Encode : ./a.out -e <input.bmp> <secret.txt> [output.bmp]\n");     // Example encode usage
    printf("  Decode : ./a.out -d <stego.bmp> <output.txt>\n");              // Example decode usage
    return 0;                                             // End of program
}

OperationType check_operation_type(char *symbol)   // Function to check operation type
{
    if (!strcmp("-e", symbol))                     // Compare user input with "-e"
    {
         return e_encode;                           // Return encode type
    }

    else if (!strcmp("-d", symbol))                // Compare user input with "-d"
    {
        return e_decode;                           // Return decode type
    } 
    else
    {
      return e_unsupported;                      // Return unsupported type for anything else
    }
}
        







