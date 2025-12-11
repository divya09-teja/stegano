#ifndef DECODE_H                 // Prevent multiple inclusion of this header file
#define DECODE_H

#include <stdio.h>               // For FILE operations
#include "types.h"               // For custom Status type

/* Structure to store decode information */
typedef struct _DecodeInfo
{
    char *stego_image_fname;     // Name of the stego BMP file to decode
    FILE *fptr_stego_image;      // File pointer for the stego BMP file

    char *dest_fname;            // Output file name where secret is written
    FILE *fptr_dest;             // File pointer for output secret file

    char extn_secret_file[10];   // Decoded secret file extension (.txt/.c/.sh)
    
    int extn_size;               // Number of characters in file extension
    int size_secret_file;        // Size of secret file data in bytes

} DecodeInfo;

/* Function declarations */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);     // Validate decode CLI arguments
Status do_decoding(DecodeInfo *decInfo);                                     // Perform entire decoding process
Status decode_magic_string(char * magic_string, DecodeInfo* decInfo);        // Decode and verify magic string
Status decode_size(int* size, DecodeInfo* decInfo);                          // Decode size value from image
Status decode_secret_file_extn(int extn_size, DecodeInfo* decInfo);          // Decode secret file extension
Status decode_secret_file_data_size(int* data_size, DecodeInfo* decInfo);    // Decode size of secret file data
Status decode_secret_file_data(DecodeInfo* decInfo);                         // Decode actual secret data
char decode_byte_from_lsb(char *buffer);                                     // Extract 1 character from 8 LSBs
int decode_size_from_lsb(char *buffer);                                      // Extract integer from 32 LSBs

#endif                                  // End of DECODE_H
