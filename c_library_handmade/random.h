// ここからが乱数生成関数3つのヘッダ情報。stdio.hはすでにあるので消した。元のファイルを見ればわかるがなぜか二回記述していた。

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */


/* The array for the state vector */
static unsigned long long mt[NN]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

void init_genrand64(unsigned long long seed); // initialzes mt
double genrand64_real1(void); // generates a random number on [0,1]-real-interval
unsigned long long genrand64_int64(void) ;

// ここまでが乱数生成関数3つのヘッダ情報。関数本体は一番下にまとめておく。

int roulet(unsigned int, double *) ;
double randomFrom0untill1(void) ;
//double randomFrom0to1() ;