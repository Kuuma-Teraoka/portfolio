
#include <stdio.h>
#include <stdlib.h>

#include <bin.h> // str_cmp()

#pragma pack(1)

typedef struct
{
    char name[4] ;
    unsigned int size ;
    char wave[4] ;
} RIFF_CHUNK ;

typedef struct
{
    char name[4] ;
    unsigned int size ;
    unsigned short format ;
    unsigned short channel ;
    unsigned int sampling_rate ;
    unsigned int bit_rate ;
    unsigned short block_size ;
    unsigned short sample_bit ; 
} FMT_CHUNK ;

typedef struct
{
    char name[4] ;
    unsigned int size ;
    void *data ;
} DATA_CHUNK ;

typedef struct
{
    RIFF_CHUNK *riff ;
    FMT_CHUNK *fmt ;
    DATA_CHUNK *data ;
} WAV ;

typedef struct
{
    char name[4] ;
    unsigned int size ;
} READ_CHUNK ;

WAV *read_wav(FILE *) ;
WAV *malloc_wav() ;
void free_wav(WAV *) ;