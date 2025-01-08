#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */



//function to check whether the operation is encode or decode.

OperationType check_operation_type(char *argv[])
{
    if((strcmp(argv[1],"-e"))==0)
    {
        
        return e_encode;
    }
    else if((strcmp(argv[1],"-d"))==0)
    {
        return e_decode;
    }
    else 
    {
        return e_unsupported;
    }
}

//function to check whether the i/p files are of supported types.
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    int i,len=0;


    //Checking whether the source file is of .bmp extension
    i=2;
    len=strlen(argv[i]); 

    if(strstr(argv[i],".bmp")!=NULL)
    {
        
        encInfo -> src_image_fname =malloc(len+1);
        strcpy( encInfo -> src_image_fname,argv[i]);   

    }
    else
    {
        printf("\033[0;31mINFO: Source File extension is not bmp format\033[0m\n");
        
        return e_failure;

    }



   //checking whether the secret file is of supported type 
    i=3;

    len=strlen(argv[i]); 

    char *ptr;

    if(strstr(argv[i],".txt")!=NULL)
    {
        ptr=strstr(argv[i],".txt");
    }
    else if(strstr(argv[i],".c")!=NULL)
    {
        ptr=strstr(argv[i],".c");
    }
    else if(strstr(argv[i],".sh"))
    {
        ptr=strstr(argv[i],".sh");
    }
    else
    {
        printf("\033[0;31mINFO: Secret file data is in unsupported format\033[0m\n");
        return e_failure;

    }



    if(strcmp(ptr,".txt")==0)
    {
        strcpy(encInfo -> extn_secret_file,ptr);

        encInfo -> secret_fname =malloc(strlen(argv[i])+1);
        strcpy( encInfo -> secret_fname,argv[i]);

        
    }
    else if (strcmp(ptr,".c")==0)
    {
        strcpy(encInfo -> extn_secret_file,strstr(argv[i],".c"));

        encInfo -> secret_fname =malloc(strlen(argv[i])+1);
        strcpy( encInfo -> secret_fname,argv[i]);

    }
    else if(strcmp(ptr,".sh")==0)
    {
        strcpy(encInfo -> extn_secret_file,strstr(argv[i],".sh"));

        encInfo -> secret_fname =malloc(strlen(argv[i])+1);
        strcpy( encInfo -> secret_fname,argv[i]);


    } 
     else
    {
        printf("\033[0;31mINFO: Secret file data is in unsupported format\033[0m\n");

        return e_failure;

    }
        
    

    //Checking whether o/p file name is passed else allocating a default name to the o/p file.
    i=4;

    if(argv[i]==NULL)
    {
        encInfo -> stego_image_fname =malloc(strlen("stego.bmp")+1);
        strcpy( encInfo -> stego_image_fname,"stego.bmp");
        
    }
    else if(strstr(argv[i],".bmp")!=NULL)
    {
        len=strlen(argv[i]);
        encInfo -> stego_image_fname =malloc(strlen(argv[i])+1);
        strcpy( encInfo -> stego_image_fname,argv[i]);
    }
    else
    {
        printf("\033[0;31mINFO: Error provide outputfile with .bmp extension\033[0m\n");
        return e_failure;
    }


    printf("INFO: Arguments Verified succesfully\n");

    return e_success;

    
}


//function to calculate the secret file size
uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret,0,SEEK_END);
    return ftell(fptr_secret);

}


//function to check whether the secret file can be encode into the source image
Status check_capacity(EncodeInfo *encInfo)
{
    

    encInfo -> size_secret_file=get_file_size(encInfo ->fptr_secret);
    

    encInfo-> image_capacity=get_image_size_for_bmp(encInfo -> fptr_src_image);

    
    if((encInfo-> image_capacity) > ((2 + 4 + 4 + 4 + encInfo -> size_secret_file ) * 8 + 54))
    {
        return e_success;

    }
    else
    {
        return e_failure;
    }
    
}


//function to copy the 54 bytes header file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[54];
    rewind(fptr_src_image);
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_stego_image);

    if(ftell(fptr_stego_image)==54 && ftell(fptr_src_image)==54)//checking whether the 54 bytes are read and copied properly.
    {
        return e_success;

    }
    else
    {
        return e_failure;
    }

}


//function to encode integer into the lsb of the image
Status encode_int_to_lsb(long data, char *int_buffer)
{
    int get=0;

    for(int i=31;i>=0;i--)
    {
        get=(data & (1<< i))>>i;
        int_buffer[31-i]=int_buffer[31-i]&(~1);
        int_buffer[31-i]=int_buffer[31-i] | get;

    }
    
    return e_success;

}

//function to encode character into the lsb of the integer
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    int get=0;

    for(int i=7;i>=0;i--)
    {
        get=(data & (1<< i))>>i;
        image_buffer[7-i]=image_buffer[7-i]&(~1);
        image_buffer[7-i]=image_buffer[7-i] | get;

    }
    
    return e_success;
   

}



//function to encode the data to the image
Status encode_data_to_image( const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,fptr_src_image);
        if(encode_byte_to_lsb(data[i],buffer) != e_success)
        {
            return e_failure;
        }
        fwrite(buffer,8,1,fptr_stego_image);
    }

    return e_success;

}



//function to encode the magic string to the image.
Status encode_magic_string(const char* magic_string, EncodeInfo *encInfo)
{
    
    if(encode_data_to_image(magic_string,strlen(MAGIC_STRING),encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    {
       
       return e_success;
    }
    else
    {
        return e_failure;
    }


}


//function to encode the secret file extnsion size.
Status encode_secret_file_extn_size(long extn_size, EncodeInfo *encInfo)
{
    char buffer[32];

        fread(buffer,32,1,encInfo -> fptr_src_image);
        if(encode_int_to_lsb(extn_size,buffer) == e_failure)
        {
            return e_failure;
        }
        fwrite(buffer,32,1,encInfo -> fptr_stego_image);
    
     return e_success;

}


//function to encode secret file extension.
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if(encode_data_to_image(file_extn,strlen(encInfo -> extn_secret_file),encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


//function to encode secret file size.
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];

        fread(buffer,32,1,encInfo -> fptr_src_image);
        if(encode_int_to_lsb(file_size,buffer) == e_failure)
        {
            return e_failure;
        }
        fwrite(buffer,32,1,encInfo -> fptr_stego_image);
    
     return e_success;
}


//function to encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);//rewinding the file pointer to the begining after finding the size to encode the data.
    char secret_data[encInfo -> size_secret_file];
    fread(secret_data,encInfo -> size_secret_file,1,encInfo -> fptr_secret);
    if(encode_data_to_image(secret_data,encInfo -> size_secret_file,encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


//function to copy the remaining the data from the source image.
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{

    char ch;

    while(fread(&ch,1,1,fptr_src)>0)
    {
       fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
    
}









//function to do the encoding.
Status do_encoding(EncodeInfo *encInfo)
{
   printf("INFO: Open Files process started.\n");
   printf("INFO: Encoding file %s\n",encInfo -> src_image_fname);
   if(open_files(encInfo) == e_success)
   {

     printf("INFO: Files opened successfully\n");

     printf("INFO: Checking capacity to encode message.\n");
     
     if(check_capacity(encInfo) == e_success)
     {
        printf("INFO: Capacity of source file is more than the secret file.\n");
        printf("INFO: Header is copying.\n");
        if(copy_bmp_header(encInfo -> fptr_src_image,encInfo -> fptr_stego_image) == e_success)
        {
            printf("INFO: Header is copied successfully.\n");
            printf("INFO: Encoding magic string.\n");
            
            
            if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
            {
                printf("INFO: Magic String encoded successfully.\n");
                printf("INFO: Encoding secret file extension.\n");
                

                if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file ),encInfo) == e_success)
                {
                    printf("INFO: Secret file extension size encoded succesfully.\n");
                    printf("INFO: Encoding secret file extension.\n");

                   
                   if(encode_secret_file_extn(encInfo -> extn_secret_file,encInfo) == e_success)
                   {
                     printf("INFO: Secret file extension encoded succesfully.\n");
                     printf("INFO: Encoding Secret file size.\n");
                       

                       if(encode_secret_file_size(encInfo -> size_secret_file,encInfo) == e_success)
                       {
                        printf("INFO: Secret file size encoded successfully.\n");
                        printf("INFO: Encoding secret file data.\n");

                          

                          if(encode_secret_file_data(encInfo) == e_success)
                          {
                            printf("INFO: Secret file data encoded successfully\n");
                            printf("INFO: Encoding remaining data started.\n");

                            
                            if(copy_remaining_img_data(encInfo -> fptr_src_image,encInfo -> fptr_stego_image)== e_success)
                            {
                                printf("INFO: Remaining data encoded successfully.\n");
                                
                                
                                return e_success;
                            }
                            else
                            {
                                printf("\033[0;31mINFO: Remaing data encoding failed.\033[0m\n");
                                return e_failure;
                            }
                              
                          }
                          else
                          {
                            printf("\033[0;31mINFO: Secret file data encoding failed.\033[0m\n");
                            return e_failure;
                          }
  
                       }
                       else
                       {
                        printf("\033[0;31mINFO: Secret file size encoding failed.\033[0m\n");
                        return e_failure;
                       }
                   }

                   else
                    {
                        printf("\033[0;31mINFO: Secret file extension encoding failed.\033[0m\n");
                        return e_failure;
                    }
            
                }
                else
                {
                    printf("\033[0;31mINFO: Secret file extension size encoding failed\033[0m\n");
                    return e_failure;
                }
            }
            else
            {
                printf("\033[0;31mINFO: Magic String copying failed\033[0m\n");
                return e_failure;
            }  

        }
        else
        {
            printf("\033[0;31mINFO: Header copying failed\033[0m\n");
            return e_failure;
        }
   
     }
     else
     {
        printf("\033[0;31mINFO: Capacity of source file less than the secret file.\033[0m\n");
        return e_failure;
     }
   }
   else
   {
    printf("\033[0;31mINFO: Failed to open files.\033[0m\n");
    return e_failure;
   }
} 




//function to get the size of the source image
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */


//function to open the files or the operations.
Status open_files(EncodeInfo *encInfo)
{
    
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    //printf("INFO: Opened %s\n",encInfo->secret_fname);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    

    // No failure return e_success
    return e_success;
}
