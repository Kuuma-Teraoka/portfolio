
#ifndef _list_h
#define _list_h

/*
　ポインタ配列は静的確保をベースにしたオブジェクト指向が本質であるcppでは扱いづらい。forループでlist.addをしても、そのポインタが指す先は静的変数のポインタなので、スコープを抜ければ解放されてしまう。matrixのlistに使おうとしたがいまいちで、結局、(20, 20*100)の行列を作って、mat({0,20})というようにアクセスするようにした。
*/

#include <stdio.h> // NULL
#include <stdlib.h> // malloc(), EXIT_FAILURE=1

class list
{
public:
    void **data ; // void *の配列
    int len ;
    int cap ;
    
    // 可変長引数。void &型。あとでやる。いや、list lst(mat_a, str("abcde"))としたとしても、lst[0]はvoid *を返すしかないので、*(* matrix)lst[0]とする必要がある。いや、void &を返せば、(matrix &)lst[0]にできるか。うむむ。pythonのように、objectというクラスをつくり、すべてそれでラップすればいいかも。void *dataとchar *class_nameで構成。object a = matrix(3, 4)とすると、いや、任意の型に対して=を定義するのはめんどくさい。それに仮にoperator=(object, matrix)を定義したとして、object型の配列でlistを作って、lst[0]としても、[]でいちいち、
    // if(object data[i].class_name == "matrix") return *(* matrix)data[i].data ;とする必要がある。いや、返すのをdata[i]、つまりobjectにして、mainではobjectとして扱い、呼び出す関数の引数もobjectにすればいける。引数の型のところはobject aという感じになるので、pythonのように、a.class_name == "matrix"というように場合分けを関数内で書く必要があるが。
    //list()
//} ;

// initではlen=0, cap=1とする。本当なら、list(1,2,3)で1,2,3を保有できるようにしたいが。可変長引数...
    list(): data((void **)malloc(sizeof(void *) * 1)), len(0), cap(1) {} 

    // void *の先がfreeされていることが前提。forループでfreeしてから、lst_freeする。
    void lst_free(LIST *lst)
    {
        free(lst->data) ;
        free(lst) ;
    }

// pythonでいうlst.append(3)
void lst_append(LIST *lst, void *node)
{
    if(lst->len == lst->cap)
    {
        lst->cap = lst_next_cap(lst->cap) ;
        lst->data = (void **)realloc(lst->data, sizeof(void *) * lst->cap) ; 
    }

    lst->data[lst->len] = node ;
    lst->len++ ;
}

// capの上昇幅を定義する関数。最初は2倍で、1024あたりから1.1にする。1024は1024 * 8 = 8192[byte]。
unsigned int lst_next_cap(unsigned int cap)
{
    if(cap < 1024)
        return cap * 2 ;
    else
        return (unsigned int)(cap * 1.1) ;
}

/*
　__getitem__を略してget。*(double *)(lst_get(lst, 13))と*(double *)lst->data[12]、つまり、lst_get(lst, 13)とlst->data[12]だと後者のほうが楽かも。ただ、ないところにアクセスするとSegmentation faultになるが、lst_getはエラーを出力してくれる。
*/
void *lst_get(LIST *lst, unsigned int index)
{
    if(index >= lst->len)
        throw std::out_of_range("lst_get(): index >= lst->len") ;
    
    return *(lst->data + index) ;


#endif
