#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "common.h"

Status do_decoding(DecodeInfo *decInfo)                                                         
{
    if(open_decode_files(decInfo)==e_failure)                                                   
    {
        printf("FILE OPENING FAILED\n"); 
        return e_failure;                                                                       
    }
    printf("FILE OPENED SUCCESSFULLY\n");

    if(decode_magic_string(decInfo)==e_failure)                                                 
    {
        printf("FAILED TO DECODE MAGIC STRING\n");
        return e_failure;                                                                       
    }
    printf("MAGIC STRING DECODED SUCCESSFULLY\n");

    if(decode_secret_file_extn_size(decInfo)==e_failure)                                         
    {
        printf("FAILED TO DECODE EXTENSION SIZE\n");
        return e_failure;                                                                        
    }
    printf("EXTENSION SIZE DECODED SUCCESSFULLY\n");

    if(decode_secret_file_extn(decInfo)==e_failure)                                                                 
    {
        printf("FAILED TO DECODE FILE EXTENSION\n");
        return e_failure;                                                                        
    }
    printf("FILE EXTENSION DECODED SUCCESSFULLY\n");

    if(decode_secret_file_size(decInfo)==e_failure)                                                
    {
        printf("FAILED TO DECODE FILE SIZE\n");
        return e_failure;                                                                          
    }
    printf("FILE SIZE DECODED SUCCESSFULLY\n");

    if(decode_secret_file_data(decInfo)==e_failure)                                                
    {
        printf("FAILED TO DECODE FILE DATA\n");
        return e_failure;                                                                           
    }
    printf("FILE DATA DECODED SUCCESSFULLY\n");
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)                                     
{
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");              

    if(decInfo->fptr_stego_image==NULL)                                            
    {
        printf("unable to open the file %s\n",decInfo->stego_image_fname);
        return e_failure;                                                          
    }

    decInfo->fptr_output=fopen(decInfo->output_fname,"w");                          
    
    if(decInfo->fptr_output==NULL)                                                  
    {
        printf("unable to open the file %s\n",decInfo->output_fname);
        return e_failure;                                                            
    }
        return e_success;                                                            
}

Status decode_byte_from_lsb(char *buffer, char *data)                                        
{
    *data = 0;             
    for (int i=0;i<8;i++)                                                              
    {
        *data=(*data<<1) | (buffer[i]&1);                                               
    }
    return e_success;                                                                         
}

Status decode_size_from_lsb(char *buffer, int *size)                                           
{
    *size = 0;
    for (int i = 0; i < 32; i++)                                                                
    {
        *size=(*size<<1) | (buffer[i]&1);                                                 
    }
    return e_success;                                                                           
}

Status decode_magic_string(DecodeInfo *decInfo)                                      
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);                                  

    char buffer[8];                                                                  
    char decoded_char;                                                               
    char magic[10];                                                                   

    int len=strlen(MAGIC_STRING);                                                    
    for (int i=0;i<len;i++)                                                          
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);                             
        decode_byte_from_lsb(buffer, &decoded_char);                                 
        magic[i] = decoded_char;                                                      
    }

    magic[len]='\0';                                                                   

    if (strcmp(magic, MAGIC_STRING) != 0)                                              
    {
        return e_failure;                                                               
    }
    return e_success;                                                                   
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)                                                              
{
    char buffer[32];

    fread(buffer, 32, 1, decInfo->fptr_stego_image);                                   
    decode_size_from_lsb(buffer, &decInfo->extn_size);                                 

    return e_success;                                                                   
}

Status decode_secret_file_extn(DecodeInfo *decInfo)                                                                  
{
    char buffer[8];                                                                     
    char decoded_char;                                                                  
    char extn[20];                                                                      

    for (int i = 0; i < decInfo->extn_size; i++)                                        
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);                                  
        decode_byte_from_lsb(buffer, &decoded_char);                                     
        extn[i] = decoded_char;                                                          
    }
    extn[decInfo->extn_size] = '\0';                                                     

    strcpy(decInfo->secret_fname, decInfo->output_fname);

    if (strstr(decInfo->output_fname, extn) == NULL)
    {
        strcat(decInfo->secret_fname, extn);
    }                                                  

    decInfo->fptr_secret = fopen(decInfo->secret_fname, "wb");                          

    if (decInfo->fptr_secret == NULL)                                                       
    {
        printf("Unable to create output file\n");
        return e_failure;                                                                  
    }
    return e_success;                                                                       
}

Status decode_secret_file_size(DecodeInfo *decInfo)                                         
{
    char buffer[32];                                                                        
    fread(buffer, 32, 1, decInfo->fptr_stego_image);                                         
    decode_size_from_lsb(buffer, &decInfo->size_secret_file);                                
    return e_success;                                                                        
}

Status decode_secret_file_data(DecodeInfo *decInfo)                                          
{
    char buffer[8];                                                                           
    char decoded_char;                                                                       

    for (int i=0;i<decInfo->size_secret_file;i++)                                       
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);                                      
        decode_byte_from_lsb(buffer, &decoded_char);                                         
        fwrite(&decoded_char, 1, 1, decInfo->fptr_secret);                                   
    }
    return e_success;                                                                        
}
