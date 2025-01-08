#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "decode.h"
#include "common.h"



//function to decode the byte from the lsb.
 char decode_byte_from_lsb(char *buffer)
{
    int get;
    char ch;
    for(int i=0;i<=7;i++)
    {
        get=buffer[i]&1;
        ch=(get<< (7-i)) | ch;
    }
    return ch;
   
}

//function to decode the integer value from the lsb.
int decode_int_from_lsb(char *buffer)
{
    int get;
    int ch=0;
    for(int i=0;i<=31;i++)
    {
        get=buffer[i]&1;
        ch=(get<< (31-i)) | ch;
    }
    return ch;
   
}


//function to read and validate the passed arguments.
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    int i,len=0;

    //Checking whether the i/p file is of .bmp format.
    i=2;
    len=strlen(argv[i]); 

    if(strstr(argv[i],".bmp")!=NULL)
    {
        
        decInfo -> stego_image_fname =malloc(len+1);
        strcpy( decInfo -> stego_image_fname,argv[i]);   

    }
    else
    {
        printf("\033[0;31mINFO: Image File extension is not bmp format.\033[0m\n");
        return e_failure;

    }


    //Checking Whether the o/p file name is available and if not a default name is allocated.
    i=3;

    if(argv[i]==NULL)
    {
       
        strcpy( decInfo -> secret_fname,"stego");
        
    }
    else if(strstr(argv[i],".txt") !=NULL  || strstr(argv[i],".ch") !=NULL || strstr(argv[i],".c") !=NULL)
    {
   
        strcpy(decInfo -> secret_fname,strtok(argv[i],"."));

    }

    else
    {
        printf("\033[0;31mINFO: The output file is not in supported format.\033[0m\n");
        return e_failure;

    }


    return e_success;

}


//function to skip the bmp header as no secret file data is stored in the header.
Status skip_bmp_header( FILE *fptr_stego_image)
{
    fseek(fptr_stego_image,54,SEEK_SET);

    return e_success;

}

//function to decode the magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char buffer[strlen(magic_string)+1];
    char ch[8];

    for(int i=0;i<strlen(magic_string);i++)
    {
        fread(ch,8,1,decInfo -> fptr_stego_image);
        buffer[i]=decode_byte_from_lsb(ch);
    }

    buffer[strlen(magic_string)]='\0';

    //printf("%s\n",buffer);
      

    if(strcmp(buffer,magic_string)!=0)
    {
        printf("MAGIC STRING NOT FOUND!!!\n");
        return e_failure;
    }
    

}

//function to decode the secret file extension size
Status decode_secret_file_extn_size( DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer,32,1,decInfo -> fptr_stego_image);
    decInfo -> secret_file_extn_size =decode_int_from_lsb(buffer);

   // printf("%ld\n",decInfo -> secret_file_extn_size);

    return e_success;
}

//function to decode secret file extension
Status decode_secret_file_extn( DecodeInfo *decInfo)
{
    char buffer[decInfo -> secret_file_extn_size+1];
    char ch[8];

    for(int i=0;i<decInfo -> secret_file_extn_size;i++)
    {
        fread(ch,8,1,decInfo -> fptr_stego_image);
        buffer[i]=decode_byte_from_lsb(ch);
    }
    buffer[decInfo -> secret_file_extn_size]='\0';

    strcpy(decInfo -> extn_secret_file,buffer);

    //printf("%s\n",decInfo -> extn_secret_file);

    return e_success;

}


//function to decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];

    fread(buffer,32,1,decInfo -> fptr_stego_image);
    decInfo -> size_secret_file =decode_int_from_lsb(buffer);

    //printf("%ld\n",decInfo -> size_secret_file);

    return e_success;

}

//function to decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[decInfo -> size_secret_file];
    char ch[8];


    for(int i=0;i < decInfo -> size_secret_file;i++)
    {
        fread(ch,8,1,decInfo -> fptr_stego_image);
        buffer[i]=decode_byte_from_lsb(ch);
     
    }
       buffer[decInfo->size_secret_file]='\0';
       //printf("%s\n",buffer);
   

      fprintf(decInfo -> fptr_secret,"%s",buffer);


    return e_success;

}









//function to decode the encoded image.
Status do_decoding(DecodeInfo *decInfo)
{
    printf("INFO: Opening Files for decoding\n");
    if(open_files_for_decode(decInfo) == e_success)
    {
        printf("INFO: Opened File Successfully\n"); 
        
        
         if(skip_bmp_header(decInfo ->fptr_stego_image) == e_success)
        {
            printf("INFO: Decoding MAGIC STRING.\n");

            if(decode_magic_string(MAGIC_STRING,decInfo) == e_success)
            {
                printf("INFO: MAGIC STRING decoded successfully\n");
                printf("INFO: Decoding secret file extension size\n");
                if(decode_secret_file_extn_size(decInfo) == e_success)
                {
                    printf("INFO: Secret File extension size decoded successfully.\n");
                    printf("INFO: Decoding secret file extension.\n");
                    if(decode_secret_file_extn(decInfo) == e_success)
                    {
                        printf("INFO: Secret file extension decoded successfully\n");
                        printf("INFO: Decoding secret file size.\n");
                        if(decode_secret_file_size(decInfo) == e_success)
                        {
                            printf("INFO: Decoded secret file size successfully\n");

                            if(open_files_for_writing(decInfo) == e_success)
                            {
                                if(decode_secret_file_data(decInfo) == e_success)
                                {
                                    printf("INFO: Decoded secret file data successfully\n");
                                    return e_success;
                                }
                                else
                                {
                                    printf("\033[0;31mINFO: Secret file decoding failed\033[0m\n");
                                    return e_failure;
                                }
                                
                            }
                            else
                            {
                                return e_failure;
                            }

                        }
                        else
                        {
                            printf("\033[0;31mINFO: Secret file size decoding failed.\033[0m\n");
                            return e_failure;
                        }

                    }
                    else
                    {
                        printf("\033[0;31mSecret file extension size decoding failed.\033[0m\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("\033[0;31mINFO: Secret file extension size decoding failed.\033[0m\n");
                    return e_failure;
                }

            }
            else
            {
                return e_failure;
            }


        }
        else
        {
            return e_failure;
        } 
        
    }
    else
    {
        printf("\033[0;31mINFO: Not able to open the file.\033[0m\n");
        return e_failure;
    }

    return e_success;
}





//function to open the output file for writing the secret data for writing
Status open_files_for_writing(DecodeInfo *decInfo)
{

    strcat(decInfo ->secret_fname,decInfo -> extn_secret_file);

    decInfo -> secret_fname[strlen(decInfo->secret_fname)]='\0';

    //printf("%s\n",decInfo -> secret_fname);

    decInfo-> fptr_secret = fopen(decInfo -> secret_fname, "w");


    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo -> secret_fname );

    	return e_failure;
    }

    return e_success;
}


//function to open the image file for decoding
Status open_files_for_decode(DecodeInfo *decInfo)
{

    decInfo->fptr_stego_image = fopen(decInfo -> stego_image_fname, "r");

    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }

    return e_success;
}



