#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"

// Structure for Decode
typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *output_fname;
    FILE *fptr_output;

    char secret_fname[50];      
    FILE *fptr_secret;          

    int extn_size;
    char extn_secret_file[20];

    int size_secret_file;

} DecodeInfo;


// Function Prototypes 

// Validation 
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// Main controller 
Status do_decoding(DecodeInfo *decInfo);

// File handling 
Status open_decode_files(DecodeInfo *decInfo);

// Bit extraction helpers
Status decode_byte_from_lsb(char *buffer, char *data);
Status decode_size_from_lsb(char *buffer, int *size);

// Decoding steps 
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);


#endif