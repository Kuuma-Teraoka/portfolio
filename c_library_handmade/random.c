#include <stdlib.h>
#include <time.h>

#include <random.h>

/*
　確率の配列を受け取って、生成した乱数を用いて抽選する関数。2, [0.3, 0.7]を受け取ったら30%の確率で0を返す。3, [0.5, 0.7, 1.5]などでも動きはするが2を返すことはない。2, [0.2, 0.3]のようなときはforをi=numで抜けて、i=0にして返す。検知といえるものではないが、numのまま返すと、呼び出し元でa[num]という配列の末尾はa[num-1]なのに、a[num]にrouletの値を入れてしまいオーバーフローしてしまう危険性があるので。
　>だと[0, 0.3], (0.3, 0.9], というように少し気持ちわるいが、>=だと[0, 0.3),[0.3,0.9)ときれいになるため。またそのためにrandomFrom0Untill1を作った。
　といっても[0,1]で、[0,0.3],(0.3,0.9],(0.9,1.0]というようになり0だけ含まれる形になるので十分かもしれない。
　後者のほうがいいきがしたので変えた。genrand64_real1()が[0,1]の乱数らしい。中身は理解できていない。
*/
int roulet(unsigned int num, double *probability)
{
    unsigned int i ;
    double random = genrand64_real1() ;

    for(i = 0 ; random > probability[i] && i < num ; i++) random -= probability[i] ;

    return (i == num ? 0 : i) ;
}

// この関数を何度呼び出しても同じ値になってしまう。現在時刻をシードに1, 3, 5, 2という乱数が生成されたとして、ある関数内で連続でrandを実行すると1, 3, 5, 2と生成されるが、この関数を連続で呼び出しても1, 1, 1, 1としか生成されない。呼び出しで先頭に戻ってしまうのだろう。
/*
double randomFrom0to1()
{
    srand((unsigned int)time(NULL)) ;

    return (double)rand() / RAND_MAX ;
}
*/

/*
　[0,1]から乱数をとる関数から、[0,1)から乱数をとる関数を作る。
(int)(myrand() * M)
　とすれば、0.0から4.99まで乱数をとって、intで0から4にして、0,1,2,3,4から乱数をとれる。

　なぜこれを作ったかというと、[0,1]で乱数をとったのときに、0以上0.5未満だったときに50%を引いたとするものの、はたして0以上0.5未満と0.5以上1"以下"で確率は同じなのだろうか。おそらく同じではあるはずだが、少し気持ち悪いので個人的に[0,1}の乱数が欲しかった。

　といっても、1がきたときに0にするだけで、上の理論では正しくない乱数であるのは変わらない。

　しかし、たとえば0.2,0.3,0.5で3つのスロットを引くときに、ルーレット式で、0以上0.2未満、0.2以上0.5未満、0.5以上1.0未満というように、プログラムが書きやすいので必要である。
*/
double randomFrom0untill1(void)
{
    double random = genrand64_real1() ;
    if(random == 1) return 0 ;
    else return (random) ;
}








// 以下、情報総合演習Bより。こっちは上のように関数呼び出しでリセットされることがない。中身は理解できていないが。



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
void init_by_array64(unsigned long long init_key[],
		     unsigned long long key_length)
{
    unsigned long long i, j, k;
    init_genrand64(19650218ULL);
    i=1; j=0;
    k = (NN>key_length ? NN : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 3935559000370003845ULL))
          + init_key[j] + j; /* non linear */
        i++; j++;
        if (i>=NN) { mt[0] = mt[NN-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=NN-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 62)) * 2862933555777941757ULL))
          - i; /* non linear */
        i++;
        if (i>=NN) { mt[0] = mt[NN-1]; i=1; }
    }

    mt[0] = 1ULL << 63; /* MSB is 1; assuring non-zero initial array */ 
}



/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void)
{
    int i;
    unsigned long long x;
    static unsigned long long mag01[2]={0ULL, MATRIX_A};

    if (mti >= NN) { /* generate NN words at one time */

        /* if init_genrand64() has not been called, */
        /* a default initial seed is used     */
        if (mti == NN+1) 
            init_genrand64(5489ULL); 

        for (i=0;i<NN-MM;i++) {
            x = (mt[i]&UM)|(mt[i+1]&LM);
            mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
        }
        for (;i<NN-1;i++) {
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
{
    return (long long)(genrand64_int64() >> 1);
}



/* generates a random number on [0,1]-real-interval */
double genrand64_real1(void)
{
    return (genrand64_int64() >> 11) * (1.0/9007199254740991.0);
}



/* generates a random number on [0,1)-real-interval */
double genrand64_real2(void)
{
    return (genrand64_int64() >> 11) * (1.0/9007199254740992.0);
}



/* generates a random number on (0,1)-real-interval */
double genrand64_real3(void)
{
    return ((genrand64_int64() >> 12) + 0.5) * (1.0/4503599627370496.0);
}