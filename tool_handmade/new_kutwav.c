/*
07/18/1100: カット位置のときに毎回静寂ブロック数を表示するようにした。
07181114 345[block]を345 [block]に
*/

#include <stdio.h>
#include <unistd.h>
#define CUT 64
//#define PATH_SIZE 128

#include <file.h>

struct wav
{
    int a ;

    void *a ;

} ;

void header(int, int, int, int, int, int, FILE *, int) ;
void putIntFile(int, int, FILE *) ;

void sumStrings(char *, char *, char *) ;
int nullNum(char *, int) ;

int equalModify(char *) ;


int main(int argc, char *argv[])
{
    FILE *fileWav ;
    FILE *output ;

    /*
    1byteを格納するだけなので、1byteでいい。
    しかし、char型にするとchunkSize=c[0]+c[1]*16*16...あたりでなぜか値が小さくなってしまう。
    これは16をかけて0x0016を0x1600にしているが、ビット演算子で簡略化できるかもしれない。
    他にも4bytesのint1つだけを使ってビット演算子を使って上手くやりくりするという方法もある。
    charにしろintでやりくりするにしろ、1バイトの16進数を格納するのに整数型変数を一つ使うというほうが本来の方法であるはずだ。
    たとえ、ひつようなメモリ数が1*4で4bytes無駄に増えることになってもshortでいいと思う。
    */
    unsigned c[4] ;

    

    // 1ブロックから5ブロックまで00が続いたときに、3という数字を格納する配列。
    // カット位置がわかるたびに定義するのもできないので、CUTの数だけ先に定義しておく。
    unsigned cut[CUT] ;
    unsigned cutEmptyBlock[CUT] ;
    short cutOff[CUT] ;
    // 配列に値を入れたら、cotCouter++をして、次にcut[cutCounter]に入れる。
    // cut[0]=0でcutCounter=1から始め、dataの終わり+1を最後に入れ、cutCounterはそのままにする。forで簡単になるので。
    unsigned cutCounter ; 

    // blocksize分読み取って全て0だったら、のためにforでflagを使う
    unsigned forIfFlag ;

    /*
    intは2^31-1 = 2,147,483,647 > 2GBなので、かなり余裕はある。
    unsignedに変えた。2^32-1 = 4,294,967,295 = 2^2 * (2^10)^3 - 1 = 4[GB]-1[byte]で余裕はある。
    というかそもそもwavのヘッダ情報が4byteだからwavの限界だと思う。
    filesizeは8バイトのヘッダ情報を含めていて、chunkSizeとdataSizeは含めていない。
    */
    unsigned emptyBlockBorder, emptyBlockCounter, skipAdSecond ;
    unsigned pcmStart0, tagStart0 ;
    unsigned long long chunkSize, fileSize, dataSize, fakeSize ;
    unsigned format, ch, Hz, ps, blockSize, bit ;
    
    register int i, j, byteBlockCounter ;



    // つまるのあんまり好きじゃない。
    printf("\n") ;

    // this.c some.wav、のように引数が2つ必要なので、そうでない場合にエラーを出力。
    // argv[1]がないと、pathの設定でsegmentation faultになるため、先にこっち。
    // まあ、segmentation faultがでるか、自分で設定したこのエラーが出るかの違いだけだが。
    if(argc != 2)
    {
        printf("argc != 2\nPlease input wav file name such as \"$ kutwav some.wav\"\n\n") ;
        return 0 ;
    }

    // modified.wavを入力してしまうと書き込みモードと読み込みモードが同時起動してしまう。
    // 01 sound.wavはそもそも空白によって、argc=3となりはじかれる。
    if(equalModify(argv[1]))
    {
        printf("argv[1]=\"modified.wav\". if this name, file may be edit\n\n") ;
        return 0 ;
    }


    // fopenのための"sound01"のような文字列を作り格納するための配列。
    char sound[13] = "00 sound.wav" ;


    char *realpath = myrealpath(argv[1]) ;

    if((fileWav = fopen(realpath, "r")) == 0)
    {
        printf("fopen returned NULL. file name is different, or file don't exist in current directory\n\n") ;
        return 0 ;
    }


    // start analyze
    printf("start analyze wav file\n\n") ;
    

    // RIFF
    // chunk name = RIFF
    byteBlockCounter = 0 ;
    printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
    for(i = 0 ; i < 4 ; i++)
    {
        if((c[i]=fgetc(fileWav)) == EOF)
        {
            fclose(fileWav) ;
            printf("EOF -> error : file end. the size of this file less than 4 byte. its strange.\n\n") ;// changed
            return 0 ;
        }
        else
        {
            printf("%X ", c[i]) ;
        }
    }
    if(!(c[0]=='R' && c[1]=='I' && c[2]=='F' && c[3]=='F'))
    {
      printf("-> error : First 4 bytes is not \"RIFF\". so this is not wav\n\n") ;
      return 0 ;
    }
    printf("-> chunk name : %c%c%c%c\n", c[0], c[1], c[2], c[3]) ;
    byteBlockCounter += 4 ;


    // chunk size
    printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
    for(i = 0 ; i < 4 ; i++)
    {
        if((c[i]=fgetc(fileWav)) == EOF)
        {
            fclose(fileWav) ;
            printf("EOF -> file end. the size of this file less than 8 byte. its strange.\n\n") ;// changed
            return 0 ;
        }
        else
        {
            printf("%X ", c[i]) ;
        }
    }
    chunkSize = c[0] + c[1] *16*16 + c[2] *16*16*16*16 + c[3] *16*16*16*16*16*16 ;
    printf("-> chunk size = 0x%llX+8 [bytes]\n", chunkSize) ;
    fileSize = chunkSize + 8 ;
    byteBlockCounter += 4 ;


    // WAVE
    printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
    for(i = 0 ; i < 4 ; i++)
    {
        if((c[i]=fgetc(fileWav)) == EOF)
        {
            fclose(fileWav) ;
            printf("EOF -> error : file end. the size of this file less than 12 byte. its strange.\n\n") ;// changed
            return 0 ;
        }
        else
        {
            printf("%X ", c[i]) ;
        }
    }
    if(!(c[0]=='W' && c[1]=='A' && c[2]=='V' && c[3]=='E'))
    {
      printf("First 4 bytes is not \"WAVE\". this may be AVI, but it is not supposed\n\n") ;
      return 0 ;
    }
    printf("-> format name (supposed WAVE): %c%c%c%c\n\n", c[0], c[1], c[2], c[3]) ;
    byteBlockCounter += 4 ;


    // RIFFの後にあるさまざまなチャンクをwhileでよんでいる。whileループはfmt、data、other、これはタグなど、で3, 4回くらいだろう。

    dataSize = 0 ;
    blockSize = 0 ;

    while(1)
    {
        // chunk name
        printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
        if((c[0]=fgetc(fileWav)) == EOF)
        {
            fclose(fileWav) ;
            if(dataSize > 0 && blockSize > 0 && !(dataSize % blockSize))
            {
                if(fileSize == byteBlockCounter)
                {
                    printf("EOF -> file end normally. Lets go to next step.\n\n") ;
                    goto breakLabel ;
                }
                else
                {
                    printf("EOF -> file end normally.\n\n") ;
                    printf("warning : file size RIFF indicate (0x%llX [bytes]) is differ from actual file size(0x%X [bytes])\n\n", fileSize, byteBlockCounter) ;
                    printf("do you want to make new wav file modified for RIFF chunk ? (yes : 1 no : 0) : ") ;
                    scanf("%d", &c[0]) ; // c = &c[0]?
                    while(c[0] < 0 || 1 < c[0])
                    {
                        printf("please input 0 or 1 : ") ;
                        scanf("%d", &c[0]) ;
                    }
                    if(c[0])
                    {
                        fileWav = fopen(inputWavPath, "r") ;
                        output = fopen(modifiedWavPath, "w") ;
                        for(i = 0 ; i < 4 ; i++) putc(fgetc(fileWav), output) ; //wavファイルはこの時点でRIFF,fmt,dataとちゃんとあるはずなのでEOFにはならない。
                        for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // RIFFのsize4bytesを捨てる。
                        putIntFile(byteBlockCounter-8, 4, output) ;
                        for(i = 0 ; i < byteBlockCounter-8 ; i++) fputc(fgetc(fileWav), output) ;
                        printf("\nnew wav file \"modified.wav\" is generated\n\n") ;
                        fclose(fileWav) ;
                        fclose(output) ;
                        goto breakLabel ;
                    }

                    // RIFFのsizeが間違っていてもこのプログラムは問題なく動く。 
                    else
                    {
                        printf("\n") ;
                        goto breakLabel ;
                    }
                }   
            }
            else
            {
                printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                return 0 ;
            }
        }
        else printf("%X ", c[0]) ;
        for(i = 1 ; i < 4 ; i++)
        {
            if((c[i]=fgetc(fileWav)) == EOF)
            {
                fclose(fileWav) ;
                if(dataSize > 0 && blockSize > 0 && !(dataSize % blockSize))
                {
                    if(fileSize == byteBlockCounter) printf("EOF -> warning : mystery %d byte exists\n\n", i) ;
                    else printf("EOF -> warning : mystery %d byte exists, and file size RIFF indicate (0x%llX [bytes]) is differ from actual file size(0x%X [bytes])\n\n", i, fileSize, byteBlockCounter) ;
                    printf("do you want to make new wav file modified? (yes : 1 no : 0) : ") ;
                    scanf("%d", &c[0]) ;
                    while(c[0] < 0 || 1 < c[0])
                    {
                        printf("please input 0 or 1 : ") ;
                        scanf("%d", &c[0]) ;
                    }
                    if(c[0])
                    {
                        // byteが謎のバイトを含まない形式的に正しいサイズなのでそれを使う。
                        // なので、謎数バイトがあっても、謎の数バイトを含まないfileSizeをRIFFに書いて、byteまでコピーするだけなので、結局正常終了の訂正と同じ。
                        fileWav = fopen(inputWavPath, "r") ;
                        output = fopen(modifiedWavPath, "w") ;
                        for(i = 0 ; i < 4 ; i++) putc(fgetc(fileWav), output) ; //wavファイルはこの時点でRIFF,fmt,dataとちゃんとあるはずなのでEOFにはならない。
                        for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // RIFFのsize4bytesを捨てる。
                        putIntFile(byteBlockCounter-8, 4, output) ;
                        for(i = 0 ; i < byteBlockCounter-8 ; i++) fputc(fgetc(fileWav), output) ;
                        printf("\nnew wav file \"modified.wav\" is generated\n\n") ;
                        fclose(fileWav) ;
                        fclose(output) ;
                        goto breakLabel ;
                    }
                    else
                    {
                        printf("\n") ;
                        goto breakLabel ;
                    }
                }
                else
                {
                    printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                    return 0 ;
                }
            }
            else printf("%X ", c[i]) ;
        }
        printf("-> chunk name : %c%c%c%c\n", c[0], c[1], c[2], c[3]) ;
        byteBlockCounter += 4 ;


        // fmt
        if(c[0]=='f' && c[1]=='m' && c[2]=='t' && c[3]==' ')
        {
            // chunk size
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    printf("EOF -> file end. fmt chunk is broken\n\n") ;
                    return 0 ;
                }
                else
                {
                    printf("%X ", c[i]) ;
                }
            }
            chunkSize = c[0] + c[1] *16*16 + c[2] *16*16*16*16 + c[3] *16*16*16*16*16*16 ;
            if(chunkSize != 16)
            {
            printf("-> error : fmt chunk size is not 16 bytes. option may exist, but its not supposed\n\n") ;
            return 0 ;  // ちなみにこれはfor内ではなくwhile内で多重ループではないため、break;でもよい。
            }
            printf("-> chunk size = 0x%llX+8 [bytes]\n", chunkSize) ;
            byteBlockCounter += 4 ;

            // format and chanel
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    printf("EOF -> file end. fmt chunk is broken\n\n") ;
                    return 0 ;
                }
                else
                {
                    printf("%X ", c[i]) ;
                }
            }
            format = c[0] ; // ch参照。
            ch = c[2] ;  // ch数がc[3]まで及ぶことはないと思うので、c[2]の値で代入。formatも同様で、pcmの1以外も6や7らしい。
            printf("-> format : %d (if 1, PCM. other is not supposed) chanel : %d (2 is supposed. 1 is okay but cheep)\n", c[0], ch) ;
            byteBlockCounter += 4 ;

            // kHz
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    printf("EOF -> file end. fmt chunk is broken\n\n") ;
                    return 0 ;
                }
                else
                {
                    printf("%X ", c[i]) ;
                }
            }
            Hz = c[0] + c[1] *16*16 + c[2] *16*16*16*16 + c[3] *16*16*16*16*16*16 ;
            printf("-> %d [Hz]\n", Hz) ;
            byteBlockCounter += 4 ;

            // byte/s
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    printf("EOF -> file end. fmt chunk is broken\n\n") ;
                    return 0 ;
                }
                else
                {
                    printf("%X ", c[i]) ;
                }
            }
            ps = c[0] + c[1] *16*16 + c[2] *16*16*16*16 + c[3] *16*16*16*16*16*16 ;
            printf("-> %d [bytes / sec]\n", ps) ;
            byteBlockCounter += 4 ;

            // block and bit
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    printf("EOF -> file end. fmt chunk is broken\n\n") ;
                    return 0 ;
                }
                else
                {
                    printf("%X ", c[i]) ;
                }
            }
            blockSize = c[0] + c[1] *16*16 ;
            bit = c[2] + c[3] *16*16 ;
            printf("-> block : %d [bytes], sample : %d [bit]\n\n", blockSize, bit) ;
            byteBlockCounter += 4 ;
        }

        // data
        else if(c[0]=='d' && c[1]=='a' && c[2]=='t' && c[3]=='a')
        {
            pcmStart0 = byteBlockCounter + 4 ;
            // chunk size
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    printf("EOF -> file end. data chunk is broken\n\n") ;
                    return 0 ;
                }
                else
                {
                    printf("%X ", c[i]) ;
                }
            }
            chunkSize = c[0] + c[1] *16*16 + c[2] *16*16*16*16 + c[3] *16*16*16*16*16*16 ;
            dataSize = chunkSize ;
            printf("-> chunk size = 0x%llX+8 [bytes]\n", chunkSize) ;
            byteBlockCounter += 4 ;

            // skip chunk data
            printf("0x%X-0x%llX : skip\n\n", byteBlockCounter + 1, byteBlockCounter + chunkSize) ;
            for(i = 0 ; i < chunkSize; i++)
            {
                if(fgetc(fileWav) == EOF)
                {
                    fclose(fileWav) ;
                    // fakeSizeに、warning用の間違ったdataSizeを保存しておき、正しいbyteをdataSizeに。
                    fakeSize = dataSize ;
                    dataSize = byteBlockCounter - pcmStart0 ;
                    if(dataSize > 0 && blockSize > 0 && !(dataSize % blockSize))
                    {
                        // dataSizeを実際の値で示せなかった。byteから他のchunkSize+8を引けばいいがotherが難しい。と思ったができた。
                        if(fileSize == byteBlockCounter) printf("warning : file end in skip time. 0x%X = EOF\n\ndata size data indicate (0x%llX [bytes]) is more than actual data size (0x%llX [bytes]) \n\n", byteBlockCounter + 1, fakeSize, dataSize) ;
                        else printf("warning : file end in skip time. 0x%X = EOF\n\nfile size RIFF indicate (0x%llX [bytes]) is differ from actual file size (0x%X [bytes]) , and data size data indicate (0x%llX [bytes]) is more than actual data size (0x%llX [bytes]) \n\n", byteBlockCounter + 1, fileSize, byteBlockCounter, fakeSize, dataSize) ;

                        printf("do you want to make new wav file modified? (yes : 1 no : 0) : ") ;
                        scanf("%d", &c[0]) ;
                        while(c[0] < 0 || 1 < c[0])
                        {
                            printf("please input 0 or 1 : ") ;
                            scanf("%d", &c[0]) ;
                        }
                        if(c[0])
                        {
                            fileWav = fopen(inputWavPath, "r") ;
                            output = fopen(modifiedWavPath, "w") ;

                            // RIFFが正しかったとしても、正しく書き直しても同じで分岐の必要はないのだが、正しいなら訂正しないというのが本質だろう。
                            if(fileSize == byteBlockCounter)
                            {
                                for(i = 0 ; i < pcmStart0-4 ; i++) fputc(fgetc(fileWav), output) ; // RIFFは正しいので、最初からdataChunkのnameまでコピー
                            }
                            else
                            {
                                for(i = 0 ; i < 4 ; i++) putc(fgetc(fileWav), output) ; //wavファイルはこの時点でRIFF,fmt,dataとちゃんとあるはずなのでEOFにはならない。
                                for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // RIFFのsize4bytesを捨てる。
                                putIntFile(byteBlockCounter-8, 4, output) ;
                                for(i = 0 ; i < pcmStart0-4-8 ; i++) putc(fgetc(fileWav), output) ; // 8バイト書き込んでからdataChunkのnameまで
                            }

                            for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // dataのsize4bytesを捨てる。
                            putIntFile(byteBlockCounter - pcmStart0, 4, output) ; 
                            for(i = 0 ; i < byteBlockCounter - pcmStart0 ; i++) putc(fgetc(fileWav), output) ;

                            printf("\nnew wav file \"modified.wav\" is generated\n\n") ;
                            fclose(fileWav) ;
                            fclose(output) ;
                            goto breakLabel ;
                        }
                        else
                        {
                            printf("\n") ;
                            goto breakLabel ;
                        }
                    }
                    else
                    {
                        printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                        return 0 ;
                    }
                }
                byteBlockCounter++ ;
            }
        }

        // other such as tag
        else
        {
            tagStart0 = byteBlockCounter + 4 ;

            // chunk size
            printf("0x%X-0x%X : ", byteBlockCounter + 1, byteBlockCounter + 4) ;
            for(i = 0 ; i < 4 ; i++)
            {
                if((c[i]=fgetc(fileWav)) == EOF)
                {
                    fclose(fileWav) ;
                    if(dataSize > 0 && blockSize > 0 && !(dataSize % blockSize))
                    {
                        if(fileSize == byteBlockCounter) printf("EOF -> warning : mystery %d byte exists\n\n", i + 4) ;  // chunkNameの4バイト分加える
                        else printf("EOF -> warning : mystery %d byte exists, and file size RIFF indicate (0x%llX [bytes]) is differ from actual file size(0x%X [bytes])\n\n", i, fileSize, byteBlockCounter) ;
                        printf("do you want to make new wav file modified? (yes : 1 no : 0) : ") ;
                        scanf("%d", &c[0]) ;
                        while(c[0] < 0 || 1 < c[0])
                        {
                            printf("please input 0 or 1 : ") ;
                            scanf("%d", &c[0]) ;
                        }
                        if(c[0])
                        {
                            // 謎の数バイトがi+4バイトあり、byteBlockCounterは4バイト進んでいるので、byteBlockCounter-4が正常なファイルのサイズ。
                            fileWav = fopen(inputWavPath, "r") ;
                            output = fopen(modifiedWavPath, "w") ;
                            for(i = 0 ; i < 4 ; i++) putc(fgetc(fileWav), output) ; //wavファイルはこの時点でRIFF,fmt,dataとちゃんとあるはずなのでEOFにはならない。
                            for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // RIFFのsize4bytesを捨てる。
                            putIntFile(byteBlockCounter-4-8, 4, output) ;
                            for(i = 0 ; i < byteBlockCounter-4-8 ; i++) fputc(fgetc(fileWav), output) ;
                            printf("\nnew wav file \"modified.wav\" is generated\n\n") ;
                            fclose(fileWav) ;
                            fclose(output) ;
                            goto breakLabel ;
                        }
                        else
                        {
                            printf("\n") ;
                            goto breakLabel ;
                        }
                    }
                    else
                    {
                        printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                        return 0 ;
                    }
                }
                else printf("%X ", c[i]) ;
            }
            chunkSize = c[0] + c[1] *16*16 + c[2] *16*16*16*16 + c[3] *16*16*16*16*16*16 ;
            printf("-> chunk size = 0x%llX+8 [bytes]\n", chunkSize) ;
            byteBlockCounter += 4 ;

            // skip chunk data
            printf("0x%X-0x%llX : skip\n\n", byteBlockCounter + 1, byteBlockCounter + chunkSize) ;
            for(i = 0 ; i < chunkSize; i++)
            {
                if(fgetc(fileWav) == EOF)
                {
                    fclose(fileWav) ;
                    if(dataSize > 0 && blockSize > 0 && !(dataSize % blockSize))
                    {
                        // dataとは違って、実際のchunksizeが示せないので示さない。と思ったが、消去機能でtagStart0を作ったためいける。                  
                        if(fileSize == byteBlockCounter) printf("warning : file end in skip time. 0x%X = EOF\n\nchunk size header indicate (0x%llX [bytes]) is more than actual chunk size (0x%X [bytes]) \n\n", byteBlockCounter + 1, chunkSize, byteBlockCounter - tagStart0) ;
                        else printf("warning : file end in skip time. 0x%X = EOF\n\nfile size RIFF indicate (0x%llX [bytes]) is differ from actual file size (0x%X [bytes]) , and chunk size header indicate (0x%llX [bytes]) is more than actual chunk size (0x%X [bytes]) \n\n", byteBlockCounter + 1, fileSize, byteBlockCounter, chunkSize, byteBlockCounter - tagStart0) ;

                        printf("do you want to make new wav file modified, that is modified header or deleted this chunk (deleted : 2 modified : 1 no : 0) : ") ;
                        scanf("%d", &c[0]) ;
                        while(c[0] < 0 || 2 < c[0])
                        {
                            printf("please input from 0 to 2 : ") ;
                            scanf("%d", &c[0]) ;
                        }
                        if(c[0] == 1)
                        {
                            fileWav = fopen(inputWavPath, "r") ;
                            output = fopen(modifiedWavPath, "w") ;

                            // RIFFが正しかったとしても、正しく書き直しても同じで分岐の必要はないのだが、正しいなら訂正しないというのが本質だろう。
                            if(fileSize == byteBlockCounter)
                            {
                                for(i = 0 ; i < pcmStart0-4 ; i++) fputc(fgetc(fileWav), output) ; // RIFFは正しいので、最初からdataChunkのnameまでコピー
                            }
                            else
                            {
                                for(i = 0 ; i < 4 ; i++) putc(fgetc(fileWav), output) ; //wavファイルはこの時点でRIFF,fmt,dataとちゃんとあるはずなのでEOFにはならない。
                                for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // RIFFのsize4bytesを捨てる。
                                putIntFile(byteBlockCounter-8, 4, output) ;
                                for(i = 0 ; i < pcmStart0-4-8 ; i++) putc(fgetc(fileWav), output) ; // 8バイト書き込んでからdataChunkのnameまで
                            }

                            for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // dataのsize4bytesを捨てる。
                            putIntFile(byteBlockCounter - pcmStart0, 4, output) ; 
                            for(i = 0 ; i < byteBlockCounter - pcmStart0 ; i++) putc(fgetc(fileWav), output) ;

                            printf("\nnew wav file \"modified.wav\" is generated\n\n") ;
                            fclose(fileWav) ;
                            fclose(output) ;
                            goto breakLabel ;
                        }
                        else
                        {
                            printf("\n") ;
                            goto breakLabel ;
                        }
                        if(c[0] == 2)
                        {
                            fileWav = fopen(inputWavPath, "r") ;
                            output = fopen(modifiedWavPath, "w") ;
                            for(i = 0 ; i < 4 ; i++) putc(fgetc(fileWav), output) ; //wavファイルはこの時点でRIFF,fmt,dataとちゃんとあるはずなのでEOFにはならない。
                            for(i = 0 ; i < 4 ; i++) fgetc(fileWav) ; // RIFFのsize4bytesを捨てる。
                            putIntFile(tagStart0-8-8, 4, output) ;  // tagStart0-8でtagの前。さらにRIFFの8を引く。
                            for(i = 0 ; i < tagStart0-8-8 ; i++) fputc(fgetc(fileWav), output) ;
                            printf("\nnew wav file \"modified.wav\" is generated\n\n") ;
                            fclose(fileWav) ;
                            fclose(output) ;
                            goto breakLabel ;
                        }
                    }
                    else
                    {
                        printf("EOF -> error fmt or data chunk dont exist or dataSize %% blockSize != 0. this wav file is broken\n\n") ;
                        return 0 ;
                    }
                }
                byteBlockCounter++ ;
            }
        }
    }
    breakLabel :






















    printf("---------------------\n\n") ;

    printf("sample : %d[bit] = %d[bytes]\n", bit, bit/8) ;
    printf("block size : %d[bytes] * %d[ch] = %d[bytes]\n", bit/8, ch, blockSize) ; // blockSize = bit/8 * chなのでどちらでもよい
    printf("rate : %d[bytes] * %u[Hz] = %u[bytes/sec]\n", blockSize, Hz, ps) ;
    printf("\n") ;
    printf("time : 0x%llX[bytes] / %d[bytes/sec] = %llds +%lld[bytes](%lld%%) ~= %lldm%llds\n", dataSize, ps, dataSize / ps, dataSize % ps, (dataSize % ps) * 100 / ps, dataSize / ps / 60, dataSize / ps % 60) ;
    printf("last block : 0x%llX[bytes] / %d[bytes] = 0x%llX[block]\n", dataSize, blockSize, dataSize/blockSize) ;
    printf("\n") ;











    
    printf("---------------------\n\n") ;

    // ここからはc[1]-[4]は使わないので適宜カウンタなどに使っていく。

    printf("How make new cut wav file reading wav file? (0: dont use, 1: automatic, 2: input cut time) : ") ;
    scanf("%d", &c[0]) ;
    while(c[0] < 0 || 2 < c[0])
    {
        printf("please input from 0 to 2 : ") ;
        scanf("%d", &c[0]) ;
    }











    // 00 sound.wavにtagをリセットしたものを出力。
    if(c[0] == 0)
    {
        printf("\ndo you make new 00 sound.wav, which is tag deleted (yes : 1 no : 0) : ") ;
        scanf("%d", &c[0]) ;
        while(c[0] < 0 || 1 < c[0])
        {
            printf("please input 0 or 1 : ") ;
            scanf("%d", &c[0]) ;
        }
        if(c[0])
        {
            fileWav = fopen(inputWavPath, "r") ;
            for(i = 0 ; i < pcmStart0 ; i++) fgetc(fileWav) ;  // pcmのdataが始まる前まで捨てる
            
            char *realpath ;
            realpath = myrealpath(sound) ;
            // soundは00 sound.wavに初期化されてる。
            //sumStrings(path_name, sound, soundPath) ;
            output = fopen(realpath, "w") ;
            free(realpath) ;
            header(format, ch, Hz, ps, blockSize, bit, output, dataSize) ;  // header関数でヘッダ情報を書き込む。
            for(i = 0 ; i < dataSize ; i++) putc(fgetc(fileWav), output) ; // 該当箇所のpcmを書き込む

            printf("\nnew wav file \"00 sound.wav\" is generated\n\nthank you..\n\n") ;
            fclose(output) ;
            fclose(fileWav) ;

            return 0 ;
        }
        else
        {
            printf("\nthank you.\n\n") ;
            return 0 ;
        }
    }












    else if(c[0] == 1)
    {
        printf("\ncut block memorized is %d. this duo to wav file. so I define it before start program\n\n", CUT) ;
        printf("How much empty block continue, do you want cut more than in int bytes (1[second] = %d[block]): ", Hz) ;
        scanf("%d", &emptyBlockBorder) ;
        // emptyBlockBorderをunsignedにしたため、意味がなくなった。-1を入れたら0xffffffff以上の空白を探し、もちろん見つからない。>= 0x80000000で上手くいった。ちなみに-2147483649とマイナスの下限を超えると0x0fffffffとなりプラスになる。
        while(emptyBlockBorder >= 0x80000000)
        {
            printf("please input more than 0 : ") ;
            scanf("%d", &emptyBlockBorder) ;
        }
        printf("\nEmpty Block more than %u [block] is ...\n\n", emptyBlockBorder) ;

        
        // ここからはblock単位で進めていく。
        byteBlockCounter = 0 ;
        emptyBlockCounter = 0 ;
        cut[0] = 0 ;
        cutCounter = 1 ;

        

        fileWav = fopen(inputWavPath, "r") ;

        for(i = 0 ; i < pcmStart0 ; i++) fgetc(fileWav) ;  // pcmのdataが始まる前まで捨てる
        while(byteBlockCounter * blockSize < dataSize)
        {
            forIfFlag = 1 ;
            for(i = 0 ; i < blockSize ; i++) if(fgetc(fileWav) != 0) forIfFlag = 0 ; 

            // 00だけのブロックだったとき
            if(forIfFlag) emptyBlockCounter++ ;
            // 00だけのブロックを抜けたとき
            else
            {
                if(emptyBlockCounter > emptyBlockBorder)
                {
                    // 11-20blockであれば、11-15,16-20で分けて15を記録。20-10/2。5-9であれば、5-7,8-9で分けて7を記録。9-5/2。
                    cut[cutCounter] = byteBlockCounter - emptyBlockCounter/2 ;
                    cutEmptyBlock[cutCounter] = emptyBlockCounter ;

                    printf("%d: 0x%X[block](~=%dm%ds) : only 00 - 0x%X[block](~=%dm%ds) : only 00 in %d [block] -> cut in 0x%X [block](~=%dm%ds)\n", cutCounter, byteBlockCounter-emptyBlockCounter+1, (byteBlockCounter-emptyBlockCounter+1) / Hz / 60, (byteBlockCounter-emptyBlockCounter+1) / Hz % 60, byteBlockCounter, byteBlockCounter / Hz / 60, byteBlockCounter / Hz % 60, emptyBlockCounter, cut[cutCounter], cut[cutCounter] / Hz / 60, cut[cutCounter] / Hz % 60) ;

                    cutCounter++ ;
                }
                emptyBlockCounter = 0 ;
            }
            byteBlockCounter++ ;
        }

        fclose(fileWav) ;

        cut[cutCounter] = dataSize / blockSize ;
        // 始まりの0と終わりのcutCounterは静寂ブロックによるカットではないので適当に0を入れておく。
        cutEmptyBlock[0] = 0 ;
        cutEmptyBlock[cutCounter] = 0 ;




        // cut[]を0にすると、表示が0になるが、なんかわざわざわかりづらくする必要ないと思った。
        for(i = 0 ; i < CUT ; i++) cutOff[i] = 1 ;


        // cut[0]は0で、cut[cutCounter]はdataの最後のブロックで、検出された静寂、つまりカット位置として出力されてない。
        while(1)
        {
            printf("\nif exist some where you dont cut, input cut number (if dout use or enough, input 0 to go next) : ") ;
            // 余ったc[0]。下でも初期化して使ってる。
            scanf("%d", &c[0]) ;
            while(c[0] < 0 || cutCounter <= c[0])
            {
                printf("please more than 0, and more less than %d. input : ", cutCounter) ;
                scanf("%d", &c[0]) ;
            }

            if(c[0] == 0) break ;
            cutOff[c[0]] = 0 ;

            printf("\n") ;
            for(i = 0 ; i <= cutCounter ; i++) printf("%d: %d [block] -> cut in 0x%X [block] (~=%dm%ds) = %c\n", i, cutEmptyBlock[i], cut[i], cut[i] / Hz / 60, cut[i] / Hz % 60, (cutOff[i] ? 'o' : 'x')) ;
            printf("\n") ;
        }










        printf("\nskip making new sound that is less than how much seconds (if 30s, 26 second sound is delete. if no use, input 0s) : ") ;
        scanf("%d", &skipAdSecond) ;
        while(skipAdSecond >= 0x80000000)
        {
            printf("please input more than 0 : ") ;
            scanf("%d", &skipAdSecond) ;
        }
        printf("\n") ;


        // sound00.wavのところの00のカウンタ。余ったc[0]を使ってる。
        c[0] = 1 ;

        fileWav = fopen(inputWavPath, "r") ;

        for(i = 0 ; i < pcmStart0 ; i++) fgetc(fileWav) ;  // pcmのdataが始まる前まで捨てる

        for(i = 0 ; i < cutCounter ; i += c[2])
        {
            // カット位置を無効にするためのカウンタ。c[i+1]が0だったらプラスして、その分iもプラスする。
            c[2] = 1 ;
            // i=40, 0, 0, 50であれば、whileが2回行われて、c[2]=3になり、cut[i]とcut[i+c[2]]の間でカットを行う。最後にi+=c[2]をして次は50とその次の間のカット処理へ。
            while(cutOff[i+c[2]] == 0) c[2]++ ;
            printf("0x%X[block](~=%dm%ds) - 0x%X[block](~=%dm%ds) -> ", cut[i]+1, (cut[i]+1) / Hz / 60, (cut[i]+1) / Hz % 60, cut[i+c[2]], cut[i+c[2]] / Hz / 60, cut[i+c[2]] / Hz % 60) ;

            if(cut[i+c[2]]-cut[i] < skipAdSecond * Hz)
            {
                for(j = 0 ; j < (cut[i+c[2]]-cut[i]) *blockSize ; j++) fgetc(fileWav) ;  // 捨てる
                printf("skip\n") ;
            }
            else
            {
                // "01 sound.wav"のような文字列を作る。
                // 定義の時に"00 sound.wav\0"と入れているのでsound[5],[6]だけ書き換える。
                // c[0]は余ってるから使ってるだけで初期値は1。
                sound[0] = c[0] / 10 + 0x30 ;
                sound[1] = c[0] % 10 + 0x30 ;
                //sumStrings(path_name, sound, soundPath) ;
                char *realpath ;
                realpath = myreal(sound) ;
                c[0]++ ;

                output = fopen(realpath, "w") ;
                free(realpath) ;
                header(format, ch, Hz, ps, blockSize, bit, output, (cut[i+c[2]]-cut[i]) *blockSize) ;  // header関数でヘッダ情報を書き込む。
                for(j = 0 ; j < (cut[i+c[2]]-cut[i]) *blockSize ; j++) putc(fgetc(fileWav), output) ; // 該当箇所のpcmを書き込む

                fclose(output) ;

                printf("%s\n", sound) ;
            }
        }

        fclose(fileWav) ;

        printf("\nthank you\n\n") ;

        return 0 ;
    }

    else if(c[0] == 2)
    {

        printf("\nsorry. I making this now. please wait.\n\n") ;
        return 0 ;
    }

}


void header(int format, int ch, int Hz, int ps, int blockSize, int bit, FILE *sound, int cutDataSize)
{
    register int i ;
    fputs("RIFF", sound) ;
    putIntFile(4 + 16+8 + cutDataSize+8, 4, sound) ;
    fputs("WAVE", sound) ;

    fputs("fmt ", sound) ;
    putIntFile(16, 4, sound) ;
    putIntFile(format, 2, sound) ;
    putIntFile(ch, 2, sound) ;
    putIntFile(Hz, 4, sound) ;
    putIntFile(ps, 4, sound) ;
    putIntFile(blockSize, 2, sound) ;
    putIntFile(bit, 2, sound) ;
    
    fputs("data", sound) ;
    putIntFile(cutDataSize, 4, sound) ;
}

void putIntFile(int data, int byte, FILE *file)
{
    register int i ;
    for(i = 0 ; i < byte ; i++) putc((((data) >> (8*i)) & 0x000000ff), file) ;  //0x12345678 (>> 8*1) 0x00123456 (&0x000000ff) 0x00000056
}



// sumに、cwdPath、/、relativePath、\0を入れる関数。
void sumStrings(char *cwdPath, char *relativePath, char *sum)
{
    register int i ;
    // 毎回nullNumを呼び出すとそのたびに処理が行われるので、cpuよりメモリを使おう。
    int longCwdPath = nullNum(cwdPath, 0) ;
    int longRelativePath = nullNum(relativePath, 0) ;

    if(relativePath[0] == '.' && relativePath[1] == '.' && relativePath[2] == '/') printf("sorry\n") ;
    else if(relativePath[0] == '.' && relativePath[1] == '/')
    {    
        // cwdPathいれる
        for(i = 0 ; i < longCwdPath ; i++) sum[i] = cwdPath[i] ;
        // .入れず、/から入れる。ちゃんとヌルコードまで入れる。sum[longCwd + 1 - 1]から。
        for(i = 1 ; i < longRelativePath + 1 ; i++) sum[longCwdPath + i - 1] = relativePath[i] ;
    }
    else
    {
        // cwdPathいれる
        for(i = 0 ; i < longCwdPath ; i++) sum[i] = cwdPath[i] ;
        // /いれる
        sum[longCwdPath] = '/' ;
        // relativePath入れる、ちゃんとヌルコードまで入れる。
        for(i = 0 ; i < longRelativePath +1 ; i++) sum[longCwdPath + 1 + i] = relativePath[i] ;
    }
}

// 1なら内容を表示、ついでに末尾のヌルコードとその後のメモリ2byte分も表示。[5]がnullなら5を返す、0-4で文字数も5。
int nullNum(char *strings, int mode)
{
    register int i = 0 ;
    for(i = 0 ; strings[i] != 0 ; i++) if(mode) printf("%d:%c\n", strings[i], strings[i]) ;
    if(mode)
    {
        printf("%d:%c\n", strings[i], strings[i]) ;
        printf("%d:%c\n", strings[i+1], strings[i+1]) ;
        printf("%d:%c\n", strings[i+2], strings[i+2]) ;
    }
    return i ;
}

// 引数の文字列がmodified.wavであるかどうか、そうであれば1、ちがければ0を返す。
int equalModify(char *strings)
{
    if(strings[0] != 'm') return 0 ;
    if(strings[1] != 'o') return 0 ;
    if(strings[2] != 'd') return 0 ;
    if(strings[3] != 'i') return 0 ;
    if(strings[4] != 'f') return 0 ;
    if(strings[5] != 'i') return 0 ;
    if(strings[6] != 'e') return 0 ;
    if(strings[7] != 'd') return 0 ;
    if(strings[8] != '.') return 0 ;
    if(strings[9] != 'w') return 0 ;
    if(strings[10] != 'a') return 0 ;
    if(strings[11] != 'v') return 0 ;
    if(strings[12] != '\0') return 0 ;
    return 1 ;
}
