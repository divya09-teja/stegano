#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

/* Function Definitions */ 
/*
 * Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);   // Move pointer to end
    long size = ftell(fptr);    // Get current position = file size
    rewind(fptr) ;              // Move pointer back to start
    return size;
}

/*
* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */


 /*( Function: get_image_size_for_bmp()
    Description: Finds image capacity = width * height * 3 bytes. )*/

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Validate source BMP File 
    if (strstr(argv[2],".bmp") == NULL)
    {
        printf("ERROR: Source Image must be a .bmp file.\n");
        return e_failure;
    }
    encInfo ->src_image_fname=argv[2];

    /*validate Secret TXT file
    if(strstr(argv[3],".txt")==NULL)
    {
        printf("ERROR: Secret file must be a .txt file\n");
        return e_failure;
    }
    encInfo->secret_fname = argv[3];
    strcpy(encInfo->extn_secret_file,".txt");                    //store extension for decoding */
    // Find last dot in file name → gets file extension
char *dot = strrchr(argv[3], '.');   // Finds extension starting from right

// If no dot → invalid file name
if (dot == NULL)                    // Checks if extension exists
{
    printf("ERROR: Secret file must have an extension\n");
    return e_failure;               // Return failure if no extension
}

// Allowed extensions list
char *allowed[] = { ".txt", ".c", ".h", ".sh" };   // List of valid extensions
int allowed_count = 4;                              // Number of valid extensions
int valid = 0;                                      // Flag to track validation

// Loop through allowed extensions
for (int i = 0; i < allowed_count; i++)             // Check each allowed extn
{
    if (strcmp(dot, allowed[i]) == 0)               // Compare file extn with allowed one
    {
        valid = 1;                                  // Mark as valid
        break;                                      // Exit loop
    }
}

// If no extension matched → error
if (!valid)                                         // If not valid
{
    printf("ERROR: Secret file must be .txt, .c, .h or .sh\n");  // Error message
    return e_failure;                               // Stop program
}

   // Store file name and extension for encoding
   encInfo->secret_fname = argv[3];                    // Save file name
  strcpy(encInfo->extn_secret_file, dot);             // Save extension


    //Destination BMP File (optional)
    if (argv[4] != NULL)
    {
        if (strstr(argv[4],".bmp") == NULL)
        {
            printf("ERROR: Destination file must be a .bmp file\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";                       // Default output file name
    }
    return e_success;
    
}

/* ( Function: open_files()
    Description: Opens source, secret, and destination files.)*/

Status open_files(EncodeInfo *encInfo)
{
    // open source BMP in binary read mode
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: cannot open source image file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // open secret txt file (text mode is fine)
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    //Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: cannot open secret file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // open stego image in write mode
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    //Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr,"ERROR: cannot open stego image file %s\n",encInfo->stego_image_fname);
        return e_failure;
    }
    
    return e_success;
}

    /*(Function: check_capacity()
    Description: Verifies if image has enough space for secret)*/

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // Required space = all encoded bits (magic + extn + size + data)
    long total_bits_needed = (strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + encInfo->size_secret_file) * 8;

    if (encInfo->image_capacity >= total_bits_needed)
    {
          return e_success;
    }
    else
    {
        printf("ERROR: Image too small! Need %ld bytes, available %u\n", total_bits_needed, encInfo->image_capacity);
        return e_failure;
    }
}

/*( Function: copy_bmp_header()
    Description: Copies first 54 bytes (header) unchanged.)*/

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char image_buffer[54];
    rewind(fptr_src_image);
    fread(image_buffer, 54, 1, fptr_src_image);
    fwrite(image_buffer, 54, 1, fptr_stego_image);
    if(ftell(fptr_src_image)!=ftell(fptr_stego_image))
   {
      printf("Both files offset is not in same position\n");
      return e_failure;
   }
   return e_success;

}

/*(Function: encode_magic_string()
    Description: Encodes predefined string (e.g., "#*") to verify decoding)*/

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("Encoding magic string...\n");

    //fseek(encInfo->fptr_src_image, 54, SEEK_SET);   // Skip BMP header

    char buffer[8];

    for (int i = 0; i <(int) strlen(magic_string); i++)
    {
        fread(buffer, 1,8, encInfo->fptr_src_image);       // Read 8 bytes
        encode_byte_to_lsb(magic_string[i], buffer);        // Encode char
        fwrite(buffer, 1, 8, encInfo->fptr_stego_image);    // Write modified bytes
    }

    printf("[INFO] Magic string encoded.\n");
    return e_success;  
}


/*(Function: encode_secret_file_extn_size()
    Description: Encodes the length of secret file extension (.txt).)*/

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{   
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    printf("The buffer data successfuly copied into destination file\n");
     if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

/*(Function: encode_secret_file_extn()
    Description: Encodes file extension (.txt).)*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("Sucessfully copied the buffer into destination file\n");
     if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

/*(Function: encode_secret_file_size()
    Description: Encodes total secret file size)*/
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    printf("The buffer data successfuly copied into destination file\n");
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}

/*(Function: encode_secret_file_data()
    Description: Encodes actual secret text file data.)*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);       // Move to beginning of secret file

    char ch;                            // Character read from secret file
    char buffer[8];                     // Buffer for 8 image bytes
    char secret_buffer;                 // Temporary variable to store one secret char

    // Read each character from secret file
    while (fread(&secret_buffer, 1, 1, encInfo->fptr_secret))
    {
        ch = secret_buffer;

        // Read 8 bytes from the source image
        fread(buffer, 8, 1, encInfo->fptr_src_image);

        // Encode 1 byte (character) of secret data into 8 bytes of image data
        encode_byte_to_lsb(ch, buffer);

        // Write modified 8 bytes into stego image
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    printf("Total Secret data successfully copied into destination file");
    if(ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        printf("Both file offsets are equal\n");
        return e_success;
    }
    else
    {
        printf("Both file offsets are not equal\n");
        return e_failure;
    }
}


/*(Function: copy_remaining_img_data()
    Description: Copies remaining unused image data.)*/
    /* Copy remaining image bytes after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;                                            // Temporary variable to hold each byte
    while (fread(&ch, 1, 1, fptr_src))                  // Read 1 byte from the source image
    {
        fwrite(&ch, 1, 1, fptr_dest);                   // Write that byte to the destination image
    }
    return e_success;                                   // Return success when all bytes copied
}


/*( Function: encode_byte_to_lsb()
    Description: Stores each bit of a character into 8 bytes of image)*/
Status encode_byte_to_lsb(char data, char *image_buffer)
{  
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);                    // Clear LSB and insert bit
        // (1) &0xFE = clears the LSB (sets last bit to 0)
       // (2) (data >> i) & 1 = extracts the ith bit from data
       // (3) | = combines cleared byte with that bit (stores bit into LSB)
    }
    return e_success;

    /*for(int i=0;i<8;i++)
   {
    int clear_bit = image_buffer[i] & (~1);    // Clear the last bit (same as &0xFE)
    int get_bit = (data >> i) & 1;             // Extract ith bit of data
    image_buffer[i] = clear_bit | get_bit;     // Combine both to set new LSB
    }*/
}

/*(    Function: encode_size_to_lsb()
    Description: Stores 32-bit integer into 32 bytes of image)*/
    Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 0; i < 32; i++)
    {
        imageBuffer[i] = (imageBuffer[i] & 0xFE) | ((size >> i) & 1);
        // (1) &0xFE → clears the last bit (LSB) of each image byte
        // (2) (size >> i) & 1 → extracts the ith bit from 'size'
        // (3) | → sets that extracted bit into the image byte’s LSB
    }
    return e_success;

    /*for (int i = 0; i < 32; i++)
    {
    int clear_bit = image_Buffer[i] & (~1);   // Clear last bit of image byte (make it 0)
    int get_bit = (size >> i) & 1;            // Extract the ith bit of 'size'
    image_Buffer[i] = clear_bit | get_bit;    // Insert that bit into image byte
    }*/
}

/*(Function: do_encoding()
    Description: Main control function for encoding process.)*/
Status do_encoding(EncodeInfo *encInfo)
{
     if(open_files(encInfo) == e_success)
    {
        printf("Successfully files opened\n");
    }
    else
    {
       return e_failure;
    }
    if(check_capacity(encInfo) == e_success)
    {
        printf("image_capacity is greaterthan the hidden file data\n");
    }
    else
    {
        return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        printf("Source file header is successfully copyed into destination file\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Magic_string successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
    {
        printf("Secret file extension size successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("Secret file extension  successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)== e_success)
    {
        printf("Secret file data size  successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Secret file data  successfully encoded\n");
    }
    else
    {
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("All operation completed successfully!\n");
    }
    else
    {
        return e_failure;
    }
    return e_success;
}