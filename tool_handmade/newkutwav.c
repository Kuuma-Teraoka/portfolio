
// FILE
#include <stdio.h>
// malloc()
#include <stdlib.h>
// myrealpath()
#include <file.h>
// str_cmp()
#include <bin.h>


struct wav
{
    unsigned int RIFF_size ;
    unsigned int block_size ;
    unsigned int data_size ;
    unsigned char *data ;
} ;

struct wav *read_wav(FILE *) ;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("argc != 2\nPlease input wav file name such as \"$ kutwav some.wav\"\n") ;
        return 1 ;
    }

    FILE *input_file ;
    char *input_path = myrealpath(argv[1]) ;
    if((input_file = fopen(input_path, "r")) == NULL)
    {
        printf("fopen returned NULL. file name is different, or file don't exist in current directory\n") ;
        return 1 ;
    }

    struct wav *input_data ;
    if((input_data = read_wav(input_file)) == NULL)
    {
        fclose(input_file) ;
        return 1 ;
    }

    free(input_data) ;
    fclose(input_file) ;

    return 0 ;
}

struct wav *read_wav(FILE *input)
{
    unsigned int i ;
    unsigned char memory[4] ;

    struct wav *output = (struct wav *)malloc(sizeof(struct wav)) ;

    output->block_size = 0 ;
    output->data_size = 0 ;

    if(read_four_byte(input, memory, "EOF -> error : file end. the size of this file less than 4 byte. its strange."))
        return NULL ;
    if(str_cmp((char *)memory, "RIFF", 4) == 0)
    {
        printf("-> error : First 4 bytes is not \"RIFF\". so this is not wav\n") ;
        return NULL ;  
    }
    printf("-> chunk name : %s\n", memory) ;

    if(read_four_byte(input, memory, "EOF -> error : file end. the size of this file less than 8 byte. its strange."))
        return NULL ;
    output->RIFF_size = memory[0] + memory[1] *16*16 + memory[2] *16*16*16*16 + memory[3] *16*16*16*16*16*16 ;
    printf("-> chunk size = 0x%uX+8 [bytes]\n", output->RIFF_size) ;

    if(read_four_byte(input, memory, "EOF -> error : file end. the size of this file less than 12 byte. its strange."))
        return NULL ;
    if(str_cmp((char *)memory, "WAVE", 4) == 0)
    {
        printf("First 4 bytes is not \"WAVE\". this may be AVI, but it is not supposed\n") ;
        return NULL ;  
    }
    printf("-> format name (supposed WAVE): %s\n", memory) ;

    // RIFF内のfmt, data, tagなどのチャンクを読み込むループ。
    while(1)
    {
        // chunk name
        printf("0x%lX-0x%lX : ", ftell(input) + 1, ftell(input) + 4) ;
        // バイト一つ目で終了したとき。つまり正常終了。ただし、data, fmtチャンクが存在しなければwavとして成り立たないのと、RIFF_sizeとあっているかどうかの確認を忘れずに。
        if((memory[0] = fgetc(input)) == EOF)
        {
            // data chunkとfmt chunkが存在し、data_sizeがfmt chunkで定義されたblock_sizeで割り切れるとき。つまりwavファイルとしての最低条件を満たすとき。
            if(output->data_size && output->block_size && !(output->data_size % output->block_size))
            {
                // ファイルの終了地点とRIFF_sizeが一致、つまり正常に終了した場合。
                if(output->RIFF_size + 8 == ftell(input))
                {
                    printf("EOF -> file end normally. Lets go to next step.\n") ;
                    break ;
                }
                // 最低条件は満たすものの、RIFF_sizeが間違っているとき。
                else
                {
                    printf("EOF -> file end normally. warning : file size RIFF indicate (0x%lX [bytes]) is differ from actual file size(0x%X [bytes]), so modify RIFF size.\n", output->RIFF_size + 8, ftell(input)) ;
                    break ;
                }
            }
            // 最低条件も満たさず、ファイルが終了したとき。つまりwavファイルとして成り立っていないとき。
            else
            {
                printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                return NULL ;                
            }
        }
        // 一つ目のバイトで終了するかどうかで場合分けしていて、もうfgetcしてしまっているので、ひとつめのprintと、2~4のfgetcとprintをここでする。
        else
        {
            printf("%X ", memory[i]) ;
            for(i = 1 ; i < 4 ; i++)
            {
                // 2~4byteで終了したとき。つまり、chunk nameに成りえない謎の1~3バイトが存在するとき。
                if((memory[i] = fgetc(input)) == EOF)
                {
                    // 最低条件を満たすとき。
                    if(output->data_size && output->block_size && !(output->data_size % output->block_size))
                    {
                        // ファイルの終了地点とRIFF_sizeに間違いはないとき。といっても謎の1~3バイトが存在していてそれを反映しているときなので、正常とは言えないかもしれない。
                        if(output->RIFF_size + 8 == ftell(input))
                        {
                            printf("EOF -> warning : mystery %d byte exists\n\n", i) ;
                            break ;
                        }
                        // RIFF_sizeが間違っているとき。
                        else
                        {
                            printf("EOF -> warning : mystery %d byte exists, and file size RIFF indicate (0x%lX [bytes]) is differ from actual file size(0x%X [bytes])\n", i, output->RIFF_size + 8, ftell(input)) ;
                            break ;
                        }
                    }
                    // 最低条件を満たさないとき。
                    else
                    {
                        printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                        return NULL ;                        
                    }
                }
                else
                    printf("%X ", memory[i]) ;
            }
        }
        // 4バイト読み終わったあと。
        printf("-> chunk name : %c%c%c%c\n", memory[0], memory[1], memory[2], memory[3]) ;


        // fmt
        if(str_cmp(memory, "fmt ", 4))
        {
            if(read_four_byte(input, memory, "EOF -> file end. fmt chunk is broken"))
                return NULL ;
            if(*(unsigned int *)memory != 16)
            {
                printf("-> error : fmt chunk size is not 16 bytes. option may exist, but its not supposed\n") ;
                return NULL ;  
            }
            printf("-> chunk size = 0x%X+8 [bytes]\n", str_to_ui(memory)) ;
        }

        if(ftell(input) > 100) break ;
    }


    return output ;
}

// 同じ部分がいくつか合ったので関数化して見やすくした。関数としては、標準出力しつつ、inputから、memoryに4バイト読み込み、EOF、つまりファイルが終了したらerror_messageを標準出力して1を返す関数、だろうか。
int read_four_byte(FILE *input, unsigned char *memory, char *error_message)
{
    unsigned int i ;

    printf("0x%lX-0x%lX : ", ftell(input) + 1, ftell(input) + 4) ;
    for(i = 0 ; i < 4 ; i++)
    {
        if((memory[i] = fgetc(input)) == EOF)
        {
            puts(error_message) ;
            return 1 ;
        }
        else
            printf("%X ", memory[i]) ;
    }

    return 0 ;
}

