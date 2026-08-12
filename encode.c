#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_failure)
    {
        printf("FILE OPENING NOT SUCCESSFUL\n");
        return e_failure;
    }
    printf("FILE OPENED SUCCESSFULLY\n");

    if (check_capacity(encInfo) == e_failure)
    {
        printf("INSUFFICIENT IMAGE CAPACITY\n");
        return e_failure;
    }
    printf("IMAGE CAPACITY VALIDATED SUCCESSFULLY\n");

    if (copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_failure)
    {
        printf("FAILED TO COPY BMP HEADER\n");
        return e_failure;
    }
    printf("BMP HEADER COPIED SUCCESSFULLY\n");

    if (encode_magic_string(MAGIC_STRING,encInfo) == e_failure)
    {
        printf("FAILED TO ENCODE MAGIC STRING\n");
        return e_failure;
    }
    printf("MAGIC STRING ENCODED SUCCESSFULLY\n");

    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_failure)
    {
        printf("FAILED TO ENCODE EXTENSION SIZE\n");
        return e_failure;
    }
    printf("EXTENSION SIZE ENCODED SUCCESSFULLY\n");
    if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_failure)
    {
        printf("FAILED TO ENCODE SECRET FILE EXTENSION\n");
        return e_failure;
    }
    printf("SECRET FILE EXTENSION ENCODED SUCCESSFULLY\n");
            
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_failure)
    {
        printf("FAILED TO ENCODE SECRET FILE EXTENSION\n");
        return e_failure;
    }
    printf("SECRET FILE ENCODED SUCCESSFULLY\n");
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("FAILED TO ENCODE SECRET DATA\n");
        return e_failure;
    }
    printf("SECRET FILE DATA ENCODED SUCCESSFULLY\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf("FAILED TO COPY REMAINING IMAGE DATA\n");
        return e_failure;
    }
    printf("REMAINING IMAGE DATA COPIED SUCCESSFULLY\n");

    return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        printf("Unable to open source image\n");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        printf("Unable to open secret file\n");
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        printf("Unable to open stego image\n");
        return e_failure;
    }

    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);

    uint size = ftell(fptr);

    rewind(fptr);

    return size;
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity =
        get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file =
        get_file_size(encInfo->fptr_secret);

    if (encInfo->image_capacity >=(54+16 + 32 + 32 + 32 +(encInfo->size_secret_file * 8)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char header[54];

    rewind(fptr_src_image);

    if (fread(header, 54, 1, fptr_src_image) != 1)
    {
        return e_failure;
    }

    if (fwrite(header, 54, 1, fptr_dest_image) != 1)
    {
        return e_failure;
    }

    return e_success;
}
Status encode_byte_to_lsb(char data,
                          char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] =(image_buffer[i] & 0xFE)| ((data >> (7 - i)) & 1);
    }

    return e_success;
}
Status encode_size_to_lsb(int size, char *imageBuffer)
{
   
   for (int i = 0; i < 32; i++)
    {
        imageBuffer[i]=(imageBuffer[i]&0xFE)|((size>>(31-i))&1);
    }
    return e_success;
}
Status encode_magic_string(const char *magic_string,EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; magic_string[i] != '\0'; i++)
    {
        fread(buffer, 8, 1,encInfo->fptr_src_image);

        encode_byte_to_lsb(magic_string[i],buffer);

        fwrite(buffer, 8, 1,encInfo->fptr_stego_image);
    }

    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0;file_extn[i]!=0;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{   char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char data_buffer;
    char buffer[8];
    for(int i=0;i<encInfo->size_secret_file;i++)
    {
        fread(&data_buffer,1,1,encInfo->fptr_secret);
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(data_buffer,buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer;
    while(fread(&buffer,1,1,fptr_src)==1)
    {
        fwrite(&buffer,1,1,fptr_dest);
    }
    return e_success;
}