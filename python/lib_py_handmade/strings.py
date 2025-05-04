#!/usr/bin/env python3


'''
-23103212345
　を、
-23,103,212,345
　に変換する関数。反転して、
grouped: ['543', '212', '301', '32']
　というlistをつくり、','.joinを使い、反転する。反転スライスについてはpython.txt参照。
'''
def comma(self):
    self = str(self)
    is_negative = self.startswith('-')
    if is_negative:
        number = self[1:]
    else:
        number = self
    
    number = number[::-1]
    grouped = [number[i:i+3] for i in range(0, len(number), 3)]
    result = ','.join(grouped)[::-1]

    if is_negative:
        result = '-' + result
    
    return result

'''
　print_matrixやhitogramで使う、"3:4"をslice(3,4,0)に変えて返す関数。slice("3:4")というようにコンストラクタは使えないので。詳しくはpython.txtのslice参照。str.splitでstr_lstにして、内包表現で各要素をintにしたint_strを生成して、sliceにint_lstをアンパックするだけでできた。強い。アンパックはCでは実装できないが、他はできる。
　エラーハンドリングについて。:以外にintにできない"aiueo"とか与えられてもint()でエラー。:は0~2までとれて、3つならslice()でslice expected at most 3 arguments, got 4とエラーを出してくれる。
　:に対応できてないので、ifで分岐した。
　print_matrix -s : 3 a.matとしたときに、3がslice(3)になるので、0,1,2列が表示されてしまう。a[:,3]としたいので、:がない場合はsliceではなく、intを返すようにする。
　やっぱりやめた。print_matrix.pyでmat[:,3]とすると1次元になってしまう。ndarra.printが対応していないのと、転地とかの処理がめんどくさいので、すべて2次元、つまり行列として扱う方がいい。mat[:,3]をprintしたいならmat[:,3:4]とすることにしよう。
'''
def __slice__(self):
    self = str(self)
    if self.find(":") == -1:
        return int(self)
    else:
        str_lst = self.split(":")
        result = []
        for i in str_lst:
            if i == '':
                result.append(None)
            else:
                result.append(int(i))
        return slice(*result)



if __name__ == "__main__":
    print(comma("-23103212345"))
    print(comma("asdfsdfd"))

    print(__slice__("3:5:5"))
    print(__slice__(":"))
    print(__slice__("3:"))

    a = [1,2,3,4,5]
    print(a[__slice__(":")])
    print(a[__slice__("2")])

