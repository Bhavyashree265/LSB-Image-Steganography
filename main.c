#include <stdio.h>
#include <string.h>
#include "types.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    OperationType ret;

    if (argc < 3)
    {
        printf("Usage:\n");
        printf("./a.out -e <src.bmp> <secret.txt> [stego.bmp]\n");
        printf("./a.out -d <stego.bmp>\n");
        return 1;
    }

    ret = check_operation_type(argv);

    if (ret == e_encode)
    {
        printf("Selected Encoding.\n");

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validate encode args is successful.\n");
        }
        else
        {
            printf("Read and validate encode args is failure.\n");
            return e_failure;
        }

        if (do_encoding(&encInfo) == e_failure)
        {
            printf("ENCODING FAILURE\n");
            return e_failure;
        }

        printf("ENCODED SUCCESSFULLY\n");
        return e_success;
    }
    else if (ret == e_decode)
    {
        printf("Selected Decoding.\n");
        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validate decode args is successful.\n");
        }
        else
        {
            printf("Read and validate decode args is failure.\n");
            return e_failure;
        }
        if (do_decoding(&decInfo) == e_failure)
        {
            printf("DECODING FAILURE\n");
            return e_failure;
        }

        printf("DECODED SUCCESSFULLY\n");
        return e_success;
    }
    else
    {
        printf("Unsupported operation.\n");
        return e_failure;
    }

    return e_success;
}

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        printf("Source image file not supported\n");
        return e_failure;
    }

    if (strstr(argv[3], ".txt") != NULL)
    {
        encInfo->secret_fname = argv[3];
        strcpy(encInfo->extn_secret_file, ".txt");
    }
    else
    {
        printf("Secret file not supported\n");
        return e_failure;
    }

    if (argv[4] != NULL)
    {
        if (strstr(argv[4], ".bmp") != NULL)
        {
            encInfo->stego_image_fname = argv[4];
        }
        else
        {
            printf("Stego image file not supported\n");
            return e_failure;
        }
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    return e_success;
}
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)                        
{
    if(strstr(argv[2],".bmp")!=NULL)                             
    {
        decInfo->stego_image_fname=argv[2];
    }
    else
    {
        printf("Source image file not supported\n");
        return e_failure;
    } 
    if (argv[3] != NULL)
    {
        if (strstr(argv[3], ".txt") != NULL)
        {
            decInfo->output_fname = argv[3];
        }
        else
        {
            printf("Output file not supported\n");
            return e_failure;
        }
    }
    else
    {
    decInfo->output_fname="output";                                               
    }
return e_success;                                                                
}
