
const char *help =
    "\n"
    "Usage: genrand [OPTIONS] FILE_PATH\n"
    "\n"
    "genrand: error: You must provide file path.\n" 
    "Opthions:\n"
    "\t-s: seed\n"
    "\t-n: size[byte]\n"
    "comment:\n"
    "　情報総合演習Bの乱数生成の関数をmainで呼び出して、ファイルに出力する関数。[0, 2^64-1]の乱数をバイナリにしているのでランダムバイナリになっている。\n";

#include <iostream> // std::cout

#include "str.hpp"
#include "file.hpp"

void init_genrand64(unsigned long long seed) ;
double genrand64_real1(void) ;
unsigned long long genrand64_int64(void) ;

int main(int argc, char **argv)
{ 
    int size = 1000 ;
    int seed = 1 ;
    int i ;
    for(i = 1 ; i < argc ; i++)
    {
        if(argv[i] == str("-s") && i + 1 < argc)
            seed = str(argv[++i]).int32() ;
        else if(argv[i] == str("-n") && i + 1 < argc)
            size = str(argv[++i]).int32() ;
        else
            break ;
    }
    if(argc - i != 1)
        {std::cout << help ; return 1 ;}


    if(size % sizeof(unsigned long long) != 0)
        throw std::invalid_argument("main(): size % sizeof(unsigned long long) != 0") ;
    init_genrand64(seed) ;
    file output(argv[i], "x") ;
    for(i = 0 ; i < size / sizeof(unsigned long long) ; ++i)
    {
        unsigned long long buffer = genrand64_int64() ;
        output.write(&buffer, sizeof(unsigned long long)) ;
    }


    return 0 ;
}










// for文の括弧など整えた。


#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */
/* The array for the state vector */
static unsigned long long mt[NN]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

/* initializes mt[NN] with a seed */
void init_genrand64(unsigned long long seed)
{
    mt[0] = seed;
    for (mti=1; mti<NN; mti++)
        mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(unsigned long long init_key[], unsigned long long key_length)
{
    unsigned long long i, j, k;
    init_genrand64(19650218ULL);
    i=1; j=0;
    k = (NN>key_length ? NN : key_length);
    for (; k; k--)
    {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 3935559000370003845ULL)) + init_key[j] + j; /* non linear */
        i++; j++;
        if(i>=NN)
            {mt[0] = mt[NN-1] ; i=1 ;}
        if(j>=key_length)
            j=0;
    }
    for (k=NN-1; k; k--)
    {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 2862933555777941757ULL)) - i; /* non linear */
        i++;
        if (i>=NN)
            {mt[0] = mt[NN-1] ; i=1 ;}
    }
    mt[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void)
{
    int i;
    unsigned long long x;
    static unsigned long long mag01[2]={0ULL, MATRIX_A};
    /* generate NN words at one time */
    if (mti >= NN)
    { 
        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
        if (mti == NN+1) 
            init_genrand64(5489ULL); 
        for (i=0;i<NN-MM;i++)
        {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        for (;i<NN-1;i++)
        {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        x = (mt[NN-1]&UM)|(mt[0]&LM);
        mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        mti = 0;
    }
    x = mt[mti++];
    x ^= (x >> 29) & 0x5555555555555555ULL;
    x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
    x ^= (x << 37) & 0xFFF7EEE000000000ULL;
    x ^= (x >> 43);
    return x;
}


/* generates a random number on [0, 2^63-1]-interval */
long long genrand64_int63(void)
    {return (long long)(genrand64_int64() >> 1) ;}

/* generates a random number on [0,1]-real-interval */
double genrand64_real1(void)
    {return (genrand64_int64() >> 11) * (1.0/9007199254740991.0) ;}

/* generates a random number on [0,1)-real-interval */
double genrand64_real2(void)
    {return (genrand64_int64() >> 11) * (1.0/9007199254740992.0) ;}

/* generates a random number on (0,1)-real-interval */
double genrand64_real3(void)
    {return ((genrand64_int64() >> 12) + 0.5) * (1.0/4503599627370496.0) ;}

