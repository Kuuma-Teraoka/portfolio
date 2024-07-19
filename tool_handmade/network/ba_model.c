
#include <stdio.h>

#include <bin.h>
#include <network.h>

int main(int argc, char **argv)
{
    // counter
    unsigned int i ;

    // option
    unsigned int m, m0, n ;
    m = m0 = n = 0 ;


    // 適当な文字列をオプションに入れても、いくつかのうち定義されたものだけ取り出して読み込むので関係ない。++iのところはi+1でもいいのだが、-n 23の23が引っかかることはないので飛ばしているだけ。++iで+1してからその値を代入している。
    for(i = 0 ; i < argc ; i++)
    {
        if(str_cmp(argv[i], "-m", length_string(argv[i]) + 1) && i + 1 < argc) m = str_to_ui(argv[++i]) ;
        else if(str_cmp(argv[i], "-m0", length_string(argv[i]) + 1) && i + 1 < argc) m0 = str_to_ui(argv[++i]) ;
        else if(str_cmp(argv[i], "-n", length_string(argv[i]) + 1) && i + 1 < argc) n = str_to_ui(argv[++i]) ; 

        else if(str_cmp(argv[i], "--help", length_string(argv[i] + 1)))
        {
            printf("use like \"ba_model -m 3 -m0 4 -n 10\"\n") ;
            return 0 ;
        } 
    }

    struct edges *head ;

    head = ba_model(m, m0, n) ;
    print_edges_list(head) ;
    free_list_edges(&head) ;

    return 0 ;
}
