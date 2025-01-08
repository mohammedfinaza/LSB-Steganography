#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>
#include "types.h"
#include "common.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4


typedef struct _DecodeInfo
{
    
    /* Secret File Info */
    char secret_fname[100];//character array to store secret file name;
    FILE *fptr_secret;//File pointer
    long size_secret_file;//long int to store secret file size.
    long secret_file_extn_size;//variable to store secret file extension size.
    char extn_secret_file[MAX_FILE_SUFFIX+1];//character array to store the secret file extension



    /* Stego Image Info */
    char *stego_image_fname;//to store o/p file name
    FILE *fptr_stego_image;//to access the o/p file

} DecodeInfo;



Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_for_decode(DecodeInfo *decInfo);

/* Skip bmp image header */
Status skip_bmp_header( FILE *fptr_stego_image);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string ,DecodeInfo *decInfo);

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Opening o/p file for writing secret data*/
Status open_files_for_writing(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
char decode_byte_from_lsb(char *buffer);

/* Decode int to LSB of image data array*/
int decode_int_from_lsb(char *buffer);


#endif


