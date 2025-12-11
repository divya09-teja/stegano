 #include <stdio.h>                     // Includes standard input-output functions
#include <string.h>                    // Includes string handling functions (strcmp, strlen)
#include "decode.h"                    // Includes decode-related structure & function declarations
#include "types.h"                     // Includes Status enum (e_success, e_failure)
#include "common.h"

/* Open all required files */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    decInfo->stego_image_fname = argv[2];        // Store the stego image name from command-line argument
    decInfo->fptr_stego_image = fopen(argv[2], "r");     // Open stego BMP file in read mode
    if (decInfo->fptr_stego_image == NULL)               // Check if file opening failed
    {
        printf("ERROR: Unable to open stego image file\n");  // Print error message
        return e_failure;                                   // Return failure if file not opened
    }
    decInfo->dest_fname = "decoded_output";      // Set default output filename for decoded data
    return e_success;                            // Return success
}

/* Main decoding workflow */
Status do_decoding(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);      // Move file pointer to skip 54-byte BMP header
    char magic_str[3];                                   // Array to store decoded 2-char magic string
    if (decode_magic_string(magic_str, decInfo) == e_failure)   // Decode magic string
    {
        return e_failure;                                       // Stop if failed
    }
    if (strcmp(magic_str, "#*") != 0)                     // Compare decoded magic string with "#*"
    {
        printf("ERROR: Magic string mismatch\n");         // Print error if mismatch
        return e_failure;                                 // Stop decoding
    }
    if (decode_size(&decInfo->extn_size, decInfo) == e_failure) // Decode size of file extension
    {
       return e_failure;                                       // Stop if failed
    }
    if (decode_secret_file_extn(decInfo->extn_size, decInfo) == e_failure) // Decode extension (.txt/.c)
    {
       return e_failure;                                                    // Stop if failed
    }
    if (decode_secret_file_data_size(&decInfo->size_secret_file, decInfo) == e_failure) // Decode secret file size
    {
        return e_failure;                                                                // Stop if failed
    }
    if (decode_secret_file_data(decInfo) == e_failure)        // Decode actual file data
    {
        return e_failure;                                     // Stop if failed
    }
    return e_success;                                         // If all steps OK → success
}

/* Decode magic string (#*) */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8];                                      // Temporary buffer to read 8 bytes per character

    for (int i = 0; i < 2; i++)                          // Loop to decode 2 characters
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);  // Read 8 bytes from stego image
        magic_string[i] = decode_byte_from_lsb(buffer);  // Extract 1 hidden character from 8 LSBs
    }
    magic_string[2] = '\0';                              // Null terminate the string
    printf("INFO: Magic string decoded: %s\n", magic_string); // Print decoded magic
    return e_success;                                    // Return success
}

/* Decode 32-bit integer (size) */
Status decode_size(int *size, DecodeInfo *decInfo)
{
    char buffer[32];                                     // Buffer to read 32 bytes (32 bits)
    fread(buffer, 1, 32, decInfo->fptr_stego_image);     // Read 32 bytes from image
    *size = decode_size_from_lsb(buffer);                // Convert 32 LSB bits to integer
    printf("INFO: Size decoded: %d\n", *size);           // Print decoded size
    return e_success;                                    // Return success
}

/* Decode secret file extension */
Status decode_secret_file_extn(int extn_size, DecodeInfo *decInfo)
{
    char buffer[8];                                      // Buffer to read 8 bytes for each character
    for (int i = 0; i < extn_size; i++)                  // Loop through extension length
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);  // Read 8 bytes for 1 character
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(buffer); // Extract 1 character
    }
    decInfo->extn_secret_file[extn_size] = '\0';         // Null terminate extension string
    printf("INFO: Secret extension decoded: %s\n", decInfo->extn_secret_file); // Display extension
    return e_success;                                    // Return success
}

/* Decode secret file data size */
Status decode_secret_file_data_size(int *data_size, DecodeInfo *decInfo)
{
    char buffer[32];                                     // Buffer to read 32 bytes
    fread(buffer, 1, 32, decInfo->fptr_stego_image);     // Read 32 bytes containing data size
    *data_size = decode_size_from_lsb(buffer);           // Convert 32 bits to integer
    printf("INFO: Secret file size decoded: %d bytes\n", *data_size); // Print size
    return e_success;                                    // Return success
}

/* Decode secret file data and write to output */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    FILE *out = fopen("decoded_output.txt", "w");        // Open output file to write decoded data
    if (out == NULL)                                     // Check if file failed to open
    {
         return e_failure;                                // Return failure
    }
    char buffer[8];                                      // Buffer to read 8 bytes per data byte
    for (int i = 0; i < decInfo->size_secret_file; i++)  // Loop through total number of bytes to decode
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);  // Read 8 bytes
        char ch = decode_byte_from_lsb(buffer);          // Extract 1 character from LSBs
        fputc(ch, out);                                  // Write character into output file
    }
    fclose(out);                                         // Close output file
    printf("INFO: Secret file decoded successfully.\n"); // Print success message
    return e_success;                                    // Return success
}

/* Extract 1 byte from 8 LSBs */
char decode_byte_from_lsb(char *buffer)
{
    char ch = 0;                                         // Initialize character to 0
    for (int i = 0; i < 8; i++)                          // Loop over 8 bytes (8 bits)
    {
        ch |= (buffer[i] & 1) << i;                      // Take LSB of each byte & build character
    }
    return ch;                                           // Return decoded byte
}

/* Extract 32-bit integer from 32 LSBs */
int decode_size_from_lsb(char *buffer)
{
    int size = 0;                                        // Initialize size to 0
    for (int i = 0; i < 32; i++)                         // Loop 32 bytes
    {
        size |= (buffer[i] & 1) << i;                   // Build integer using LSB bits
    }
    return size;                                         // Return decoded integer
}
