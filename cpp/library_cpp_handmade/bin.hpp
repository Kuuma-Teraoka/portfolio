
#ifndef _bin_hpp
#define _bin_hpp

#include <cstddef> // size_t

namespace bin
{
    /*
    malloc: memory allocate
    realloc: reallocate
    calloc: contiguous allocate
    　という意味らしい。bin::allocという名前でもいいが、mallocのエラーハンドリングバージョンなのでこれでいいと思う。
    　mallocを使うと衝突するので、std::mallocつかう。
    int *shape = bin::malloc<int>(2)
    　というように使う。
    　でもライブラリができてしまえばmallocでsizeを間違えるとかはなくなるので、mallocのままでいいかも。これ使わないかも。
    　bad_allocはbad_alloc("...")というようなコンストラクタを持たず、ようはエラーメッセージの表示ができないので、runtime_errorにした。
    */
    template <typename T>
    T *malloc(size_t size)
    {
        T *result = (T *)std::malloc(sizeof(T) * size) ;
        if(result == NULL)
            throw std::runtime_error
                ("bin::malloc<T>(): result == NULL. malloc is failed or size is 0.") ;
        return result ;
    }

    void *copy(void *input, unsigned int size) ;
    void print(const void *input, const unsigned int size) ; 
}

#endif

