/*Documentation
Name: Mohammed Finaz A
Date:21/11/24
DES:LSB Steganography
*/

#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc,char *argv[])
{
    if(argc==1)
    {
       printf("\033[0;31mINFO: Please pass the proper arguments\033[0m\n");
    }
    else if(argc<3)
    {
        printf("\033[0;31mINFO: ERROR Unable to perform operation.Arguments should be > 3\033[0m\n");
        printf("For Encoding Arguments should be: \n./a.out -e <sourcefile.bmp> <secretfile.txt> <outputfile.bmp>(outputfile is optional)\n");
        printf("For decoding Arguments should be: \n./a.out -d <outputfile.bmp>  <outputfile.txt>(outputfile is optional)\n");
        
    }
    else if(argc>=3)
    {
        if(check_operation_type(argv) == e_encode)
        {
            if(argc<4)
            {
                printf("\033[0;31mINFO: ERROR Unable to perform operation.Arguments should be > 3\033[0m\n");
                printf("For Encoding Arguments should be: \n./a.out -e <sourcefile.bmp> <secretfile.txt> <outputfile.bmp>(outputfile is optional)\n");
                printf("For decoding Arguments should be: \n./a.out -d <outputfile.bmp>  <outputfile.txt>(outputfile is optional)\n");
                return 0;
            }
            EncodeInfo encInfo;
            printf("INFO: Checking arguments started\n");

             if(read_and_validate_encode_args(argv,&encInfo)==e_success)
            {
                printf("INFO: Encoding Started\n");
                if(do_encoding(&encInfo) == e_success)
                 printf("INFO: ENCODING COMPLETED\n");
                
            }
           
        }
        else if(check_operation_type(argv) == e_decode)
        {
            DecodeInfo decInfo;
            printf("INFO: Checking arguments started\n");

            if(read_and_validate_decode_args(argv,&decInfo) == e_success)
            {
                printf("INFO: Decoding Started\n");

                if(do_decoding(&decInfo) == e_success)
                  printf("DECODING COMPLETED\n");

            }


        }
        else if(check_operation_type(argv) == e_unsupported)
        {
        printf("\033[0;31mINFO : Error Unable to proceed operation provide valid type(-e or -d).\033[0m\n");
        }
    }
    else
    {
        printf("\033[0;31mNo arguments are present for any operation\033[0m\n");
    }    

    return 0;
}
