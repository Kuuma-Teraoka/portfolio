
#include <wav.h>

WAV *read_wav(FILE *input)
{
    int rest ;
    WAV *wav = malloc_wav() ;
    READ_CHUNK *read = (READ_CHUNK *)malloc(sizeof(READ_CHUNK)) ;

    if(fread(wav->riff, sizeof(RIFF_CHUNK), 1, input) != 1)
    {
        fprintf(stderr, "error : file end. the size of this file less than 4 byte. its strange.\n") ;
        free_wav(wav) ;
        free(read) ;
        return NULL ;
    }

    if(!(str_cmp(wav->riff->name, "RIFF", 4) && str_cmp(wav->riff->wave, "WAVE", 4)))
    {
        fprintf(stderr, "error : First 4 bytes is not \"RIFF\", so this is not wav. or 9 ~ 12 bytes is not \"WAVE\". this may be AVI, but it is not supposed\n") ;
        free_wav(wav) ;
        free(read) ;
        return NULL ;
    }

    printf("0x%lX-0x%lX: %X %X %X %X -> chunk name: RIFF\n", ftell(input) - 11, ftell(input) - 8, wav->riff->name[0], wav->riff->name[1], wav->riff->name[2], wav->riff->name[3]) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> chunk size = 0x%X+8[byte]\n", ftell(input) - 7, ftell(input) - 4, ((unsigned char *)&(wav->riff->size))[0], ((unsigned char *)&(wav->riff->size))[1], ((unsigned char *)&(wav->riff->size))[02], ((unsigned char *)&(wav->riff->size))[3], wav->riff->size) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> format name (supposed WAVE): WAVE\n", ftell(input) - 3, ftell(input), wav->riff->wave[0], wav->riff->wave[1], wav->riff->wave[2], wav->riff->wave[3]) ;

    while(1)
    {
        rest = fread(read, 1, sizeof(READ_CHUNK), input) ;

        // バイト一つ目で終了したとき。つまり正常終了。ただし、data, fmtチャンクが存在しなければwavとして成り立たないのと、RIFF_sizeとあっているかどうかの確認を忘れずに。
        if(rest == 0)
        {
            printf("\n0x%lX: ", ftell(input) + 1) ;

            // 最低条件を満たすとき。outputにdataやfmtが紐づけされているかで判別しているのでoutput->を忘れずにつける。
            if(wav->data->data && wav->fmt->name[0] != '\0' && (wav->data->size % wav->fmt->block_size) == 0)
            {
                // ファイルの終了地点とRIFF_sizeが一致、つまり正常に終了した場合。
                if(wav->riff->size + 8 == ftell(input))
                {
                    fprintf(stdout, "EOF -> file end normally. Lets go to next step.\n") ;
                    break ;
                }
                // 最低条件は満たすものの、RIFF_sizeが間違っているとき。ここでは修正などはせずエラーだけ出力して正常終了。
                else
                {
                    fprintf(stdout, "EOF -> file end normally. warning : file size RIFF indicate (0x%X [bytes]) is differ from actual file size(0x%lX [bytes]), so modify RIFF size.\n", wav->riff->size + 8, ftell(input)) ;
                    break ;
                }
            }
            // 最低条件も満たさず、ファイルが終了したとき。つまりwavファイルとして成り立っていないとき。
            else
            {
                fprintf(stderr, "EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n") ;
                free_wav(wav) ;
                free(read) ;
                return NULL ;
            }
        }

        // chunkを構成しない謎の数バイトがあったときの処理。
        else if(1 <= rest && rest <= 7)
        {
            print_bin(read, 8) ;
            printf("\n0x%lX-0x%lX: ", ftell(input) + 1 - rest, ftell(input) + 8 - rest) ;

            // 最低条件を満たすとき
            if(wav->data->data && wav->fmt->name[0] != '\0' && (wav->data->size % wav->fmt->block_size) == 0)
            {
                // ファイルの終了地点とRIFF_sizeが一致、つまり正常に終了した場合。
                if(wav->riff->size + 8 == ftell(input))
                {
                    fprintf(stdout, "EOF -> warning : mystery %d byte exists\nf", rest) ;
                    break ;
                }
                // 最低条件は満たすものの、RIFF_sizeが間違っているとき。ここでは修正などはせずエラーだけ出力して正常終了。
                else
                {
                    fprintf(stdout, "EOF -> warning : mystery %d byte exists, and file size RIFF indicate (0x%X [bytes]) is differ from actual file size(0x%lX [bytes])\n", rest, wav->riff->size + 8, ftell(input)) ;
                    break ;
                }
            }
            // 最低条件も満たさず、ファイルが終了したとき。つまりwavファイルとして成り立っていないとき。
            else
            {
                fprintf(stderr, "EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n") ;
                free_wav(wav) ;
                free(read) ;
                return NULL ;
            }
        }

        // rest == 8。チャンクがあったときの処理。
        else
        {
            // fmt
            if(str_cmp(read->name, "fmt ", 4))
            {
                memory_copy(read, wav->fmt, sizeof(READ_CHUNK)) ;
                if(fread(&(wav->fmt->format), sizeof(FMT_CHUNK) - 8, 1, input) != 1)
                {
                    fprintf(stderr, "EOF -> file end. fmt chunk is broken\n") ;
                    free_wav(wav) ;
                    free(read) ;
                    return NULL ;
                }

                print_fmt(wav->fmt, ftell(input)) ;
            }

            // data
            else if(str_cmp(read->name, "data", 4))
            {
                memory_copy(read, wav->data, sizeof(READ_CHUNK)) ;
                wav->data->data = (void *)malloc(wav->data->size) ;
                if(fread(wav->data->data, wav->data->size, 1, input) != 1)
                {
                    fprintf(stderr, "error : file end. the size of this file less than 4 byte. its strange.\n") ;
                    free_wav(wav) ;
                    free(read) ;
                    return NULL ;
                }

                printf("\n") ;
                printf("0x%lX-0x%lX: %X %X %X %X -> chunk name: data\n", ftell(input) - 7 - wav->data->size, ftell(input) - 4 - wav->data->size, wav->data->name[0], wav->data->name[1], wav->data->name[2], wav->data->name[3]) ;
                printf("0x%lX-0x%lX: %X %X %X %X -> chunk size = 0x%X+8[byte]\n", ftell(input) - 3 - wav->data->size, ftell(input) - wav->data->size, ((unsigned char *)&(wav->data->size))[0], ((unsigned char *)&(wav->data->size))[1], ((unsigned char *)&(wav->data->size))[2], ((unsigned char *)&(wav->data->size))[3], wav->data->size) ;
                printf("0x%lX-0x%lX: skip\n", ftell(input) + 1 - wav->data->size, ftell(input)) ;
            }

            // fmtでもdataでもないチャンク。
            else
            {
                if(fseek(input, read->size, SEEK_CUR))
                {
                    fprintf(stderr, "error : file end. the size of this file less than 4 byte. its strange.\n") ;
                    free_wav(wav) ;
                    free(read) ;
                    return NULL ;
                }
                
                printf("\n") ;
                printf("0x%lX-0x%lX: %X %X %X %X -> chunk name: %c%c%c%c\n", ftell(input) - 7 - read->size, ftell(input) - 4 - read->size, read->name[0], read->name[1], read->name[2], read->name[3], read->name[0], read->name[1], read->name[2], read->name[3]) ;
                printf("0x%lX-0x%lX: %X %X %X %X -> chunk size = 0x%X+8[byte]\n", ftell(input) - 3 - read->size, ftell(input) - read->size, ((unsigned char *)&(read->size))[0], ((unsigned char *)&(read->size))[1], ((unsigned char *)&(read->size))[2], ((unsigned char *)&(read->size))[3], read->size) ;
                printf("0x%lX-0x%lX: skip\n", ftell(input) + 1 - read->size, ftell(input)) ;
            }
        }
    }

    free(read) ;
    return wav ;
}

WAV *malloc_wav(void)
{
    WAV *wav = (WAV *)malloc(sizeof(WAV)) ;
    wav->riff = (RIFF_CHUNK *)malloc(sizeof(RIFF_CHUNK)) ;
    wav->fmt = (FMT_CHUNK *)malloc(sizeof(FMT_CHUNK)) ;
    wav->data = (DATA_CHUNK *)malloc(sizeof(DATA_CHUNK)) ;
    wav->data->data = NULL ;
    wav->fmt->name[0] = '\0' ;

    return wav ;
}

void free_wav(WAV *wav)
{
    if(wav)
    {
        free(wav->riff) ;
        free(wav->fmt) ;
        if(wav->data->data)
            free(wav->data->data) ;
        free(wav->data) ;
        free(wav) ;
    }
}

void print_fmt(FMT_CHUNK *fmt, unsigned long ftell)
{
    printf("\n") ;
    printf("0x%lX-0x%lX: %X %X %X %X -> chunk name: fmt \n", ftell - 23, ftell - 20, fmt->name[0], fmt->name[1], fmt->name[2], fmt->name[3]) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> chunk size = 0x%X+8[byte]\n", ftell - 19, ftell - 16, ((unsigned char *)&(fmt->size))[0], ((unsigned char *)&(fmt->size))[1], ((unsigned char *)&(fmt->size))[2], ((unsigned char *)&(fmt->size))[3], fmt->size) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> format : %hu (if 1, PCM. other is not supposed) chanel : %hu (2 is supposed. 1 is okay but cheep)\n", ftell - 15, ftell - 12, ((unsigned char *)&(fmt->format))[0], ((unsigned char *)&(fmt->format))[1], ((unsigned char *)&(fmt->channel))[0], ((unsigned char *)&(fmt->channel))[1], fmt->format, fmt->channel) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> %u [Hz]\n", ftell - 11, ftell - 8, ((unsigned char *)&(fmt->sampling_rate))[0], ((unsigned char *)&(fmt->sampling_rate))[1], ((unsigned char *)&(fmt->sampling_rate))[2], ((unsigned char *)&(fmt->sampling_rate))[3], fmt->sampling_rate) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> %u [bytes / sec]\n", ftell - 7, ftell - 4, ((unsigned char *)&(fmt->bit_rate))[0], ((unsigned char *)&(fmt->bit_rate))[1], ((unsigned char *)&(fmt->bit_rate))[2], ((unsigned char *)&(fmt->sampling_rate))[3], fmt->bit_rate) ;
    printf("0x%lX-0x%lX: %X %X %X %X -> block: %hu [bytes], sample: %hu [bit]\n", ftell - 3, ftell, ((unsigned char *)&(fmt->block_size))[0], ((unsigned char *)&(fmt->block_size))[1], ((unsigned char *)&(fmt->sample_bit))[0], ((unsigned char *)&(fmt->sample_bit))[1], fmt->block_size, fmt->sample_bit) ;

}