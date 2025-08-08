| hostname itbc-usr-l3s{} | | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 |
| version 15.2 ~ |
| hostname itbc-usr-l3s{} | | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 |
| enable secret 5 $1${} | | y1EX$xifUDIX9by4ehIRTbUDjz/ | ryFA$xcmbK27Dlz5s4ireOK/Bw. | kER.$wG0dqRmsE.wcs3cD1jrTu0 | glvd$ySOPnBmD/qMGoJgeJCRfQ1 | uP8e$TRz/fBCFOTGN1r.7J3dGP0 | kI/b$cv0C8wBXcsK86RNs1aaEB/ | R51D$av.Ss9AgCYVBUXAZGKcM5. | PRIR$a6yayK9y8svpOlDUaAXXe1 | nRr8$7cy0P0IORDfOaOE0iGSe4/ | 5BCS$pBEv8sbxNxdw57YhATIm4. | yCxi$nnPNYjKCA6n5dsCtoRUWh0 | W1GB$OGDMDFSEKAwAAndaQYlD31 | aJ.L$Y3icejLUgFWCz/cPpJX2.0 | NUav$HLJ9jK0Q3TG6YsJ1Ifnaa0 |
| interface Loopback0 ~ interface Vlan1 |
| router ospf 1 | router-id 10.128.128.{} | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 |
| router ospf 1 | redistribute connected metric {} metric-type 1 subnets | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 |
| router ospf 1 | redistribute static metric {} metric-type 1 subnets | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 | 100 | 1000 |
| router ospf 1 | network 10.128.1.{} 0.0.0.3 area 0.0.0.0 | 44 | 48 | 52 | 56 | 60 | 64 | 68 | 72 | 76 | 80 | 84 | 88 | 92 | 96 |
| router ospf 1 | network 10.128.1.{} 0.0.0.3 area 0.0.0.0 | 124 | 128 | 132 | 136 | 140 | 144 | 148 | 152 | 156 | 160 | 164 | 168 | 172 | 176 |
| router ospf 1 | network 10.128.128.{} 0.0.0.0 area 0.0.0.0 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | none | 18 |
| line con 0 | password 7 {} | 121700035F04020178 | 030A5E1F4B002F491C | 06080A350141071C57 | 09424B1D540A191759 | 000A1612495405035D | 121700035F04020178 | 121700035F04020178 | 030A5E1F4B002F491C | 04550E12422E424B5B | 030A5E1F4B002F491C | 0505031B6C43400C4B | 09424B1D540A191759 | 082F495A44160B1240 | 011D03101604080A73 |
| line vty 0 4 | password 7 {} | 0505031B6C43400C4B | 030A5E1F4B002F491C | 06080A350141071C57 | 09424B1D540A191759 | 000A1612495405035D | 121700035F04020178 | 121700035F04020178 | 030A5E1F4B002F491C | 04550E12422E424B5B | 030A5E1F4B002F491C | 0505031B6C43400C4B | 09424B1D540A191759 | 082F495A44160B1240 | 011D03101604080A73 |
| other | | `+ ip access-list standard acl_snmp_in::permit 10.124.235.15 log <br> + ip access-list standard acl_snmp_in::permit 10.124.235.14 log
+ ip access-list standard acl_snmp_in::permit 10.124.235.54 log
+ ip access-list standard acl_snmp_in::permit 10.124.235.51 log
+ ip access-list standard acl_snmp_in::permit 10.124.235.50 log
- ip access-list standard acl_snmp_in::permit 10.124.235.61 log
- snmp-server enable traps bfd
- snmp-server enable traps trustsec-sxp conn-srcaddr-err msg-parse-err conn-config-err binding-
- snmp-server enable traps ospfv3 state-change
- snmp-server enable traps ospfv3 errors
- snmp-server enable traps stackwise
- snmp-server enable traps bulkstat collection transfer
+ snmp-server host 10.124.235.50 version 2c Net_RO_28!47
+ snmp-server host 10.124.235.51 version 2c Net_RO_28!47
+ snmp-server host 10.124.235.54 version 2c Net_RO_28!47" | "- snmp-server enable traps bfd
- snmp-server enable traps trustsec-sxp conn-srcaddr-err msg-parse-err conn-config-err binding-
- snmp-server enable traps ospfv3 state-change
- snmp-server enable traps ospfv3 errors
- snmp-server enable traps stackwise
- snmp-server enable traps bulkstat collection transfer ` | | | "- snmp-server enable *
+ snmp-server enable traps snmp authentication linkdown linkup coldstart warmstart" | "- snmp-server enable *
+ snmp-server enable traps snmp authentication linkdown linkup coldstart warmstart" | | | "+ ip route 202.122.142.128 255.255.255.224 GigabitEthernet0/1 202.122.136.234
- line con 0::logging synchronous" | "+ ip route 202.122.142.128 255.255.255.224 GigabitEthernet0/1 202.122.136.234
- line con 0::logging synchronous
- line vty 0 4::logging synchronous" | | "+ router ospf 1 | network 10.0.255.17 0.0.0.0 area 0.0.0.0" | "+ ip route 202.122.142.32 255.255.255.224 GigabitEthernet0/1 202.122.136.238"
| ~ end |



  
   
### 概要  
　githubでよくみるREADME.mdはmarkdown。つまり拡張子は.md。chatGPTのテキストもmarkdown。ただ、markdownといっても表示するビューワによって変わるような気がする。  
　cli viewerにはmdcat, glow, batがある。見比べてみたところ、デフォルトで入ってるmdcatがいいかもしれない。ただ、$$によるlatexブロックがだめだった。  
　discordは斜体や画像、tableもダメで、latexの$a$は斜体になる。コードは行けた。  
  
  
### 見出し  

# 見出し1  
## 見出し2  
### 見出し3  
#### 見出し4  
　#が多いほど小さい、レベルの低い見出しになる。githubを参考に、flexmark-javaのREADME.mdでは###をsectionくらい, ####をsubsectionくらいを使っていた。  
　あと、nvimでmdであれば####といった記号は表示されるものの、太文字になる。  
  
　いままで、txtでは、  
　python。  
　や、  
────python  
　というように見出しを書いていた。markdownを学んだときに、  
# python  
　に変えようと思ったが、読みづらかったのでやめてしまった。しかし、  
### python  
　であれば、読みやすい。1つ目はわかりづらいのでなしにするとして、###に乗り換えてもいいかもしれない。  
　まぁ、カスタムmdのようなものを自分で作って、────を### に変換するプログラムを作ればいいだけなので、対応付けが可能な書き方がいいと思う。  
  
  
---  
　区切り線。`*`,`_`でもいいらしい。  
  
　githubでは、  
  
flexmark-java  
====================================  
　というようにタイトルをつくる。===は区切り線になる。nvimでは===は変化なしだが、その上の文字は太字になる。githubでは#1つの見出しと表示が同じだった。  
  
  
  
  
  
### 斜体  
*斜体* または _斜体_  
　nvimで反映された。  
  
### 太字  
**太字** または __太字__  
　nvimで反映された。  
  
  
  
### 箇条書き  
- アイテム1  
- アイテム2  
- アイテム3  
　で箇条書きになる。  
* a  
* b  
* c  
　*でもいける。nvimは*であれば*をハイライトしてくれる。-はだめ。+はハイライトあり。  
  
1. 最初のアイテム  
2. 次のアイテム  
3. 最後のアイテム  
　で番号付きリスト。nvim有効。  
  
  
### リンク  
[Google](https://www.google.com)  
　でリンク。埋め込みなのでgoogleしか表示されない。nvimではurlももちろん表示されるが、下線がひかれて、おそらくリンクも踏める。  
  
![Markdownのロゴ](https://markdown-here.com/img/icon256.png)  
　画像。代替テキストなので[]は通常表示されないと思う。  
  
  
### コードブロック  
`kutwav a.wav`  
　一行はこう囲む。  
  
```C  
int a ;  
int a = 30 ;  
```  
　でコードブロックになる。  
  
### 引用  
> これは引用です。  
> 複数行にわたることもできます。  
  
　引用。空行\n\nをいれないと飲み込まれる。  
  
  
  
  
#### 表  
  
　ただ、discordでは反映されない。gptだけかもしれない。mdcatは表示できた。  
| 見出し1 | 見出し2 |  
| -------- | -------- |  
| 内容1   | 内容2   |  
| 内容3   | 内容4   |  
| B | C |  
| D | C |  
|A|b|  
  
　箇条書きを表の中にいれるにはこのようにする。gptはこれで表示できていたが、discordもmdcatも上手く表示できない。  
| 項目 | 内容 |  
|----|----|  
| タスク | - 調査  <br> - 実装  <br> - テスト |  
| 備考 | 特になし |  
  
　また、私のtxtではよく、  
CX33K, panasonic, 325kWh  
CX33M, panasonic, 50kWh  
　のようなことを書く。これは表に対応している。とにかく、頭に"　"で末尾に"。"がくる文を普通の文として、それ以外において,区切りがあれば表として表示するようなカスタムmdを定義してもいいかもしれない。  
　いや、"',"区切りを" | "区切りにしてもいいかも。  
  
　csvなら',', tsvなら'\t', mdなら'|'を区切り文字として表のようなデータを記述する。どれがいいかなかなか難しい。,は文章でも使うのと、\tは空白と区別がしづらい。\tはフォントやvimの設定で表示すればいいかとも思う。terminalでは\tならcatするだけで自動で幅を調節してくれるのも大きい。  
　だが、結局|区切りがいいのではないかとも思う。  
  
$ pip install csvkit  
$ csvlook -t port.tsv  
　でmdの|区切りで出力してくれるので、  
$ csvlook -t port.tsv | mdcat  
　で表示できる。  
  
　exelへの変換は、exelのcsvインポートで区切り文字を,なり\tなり|なりを選べばいいらしい。  
  
  
  
  
  
  
### チェックリスト  
- [ ] タスク1  
- [x] タスク2（完了）  
  
  
  
### 数式  
$x = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}$  
　でできる。  
  
$$$  
x = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}  
y = 3  
\int_{a}^{b} f(x) \, dx = F(b) - F(a)  
$$$  
　で数式ブロック。latexが使える。これは便利。githubでもこれ有効じゃなかった。chatgptだけかもしれない。  
  
  
  
  
  
### github  
　大きな特徴が、latexのように改行が改行として表示されない。  
全角スペース, 文章, \n, 全角スペース, 文章, \n  
　とすると、  
全角スペース, 文章, 全角スペース, 文章  
　となってしまう。latexのように空行をいれることで異なる段落として改行が入るが、段落の間で行間が空いてしまう。  
　ここで、  
全角スペース, 文章, 半角スペース2つ, \n, 全角スペース, 文章, 半角スペース2つ, \n  
　とすると、  
全角スペース, 文章, \n, 全角スペース, 文章, \n  
　として表示される。:%s/\(\n\)/  \1/gで\nを'  \n'に一括で変換してgithubにアップロードするとそのままの表示になった。  
  
　文章の書き方として、段落を変えるときしか改行をいれないべきなのかもしれない。私の書き方は改行がありすぎるのだろうか。  
　いや、私の改行は段落分けであるのでいいと思う。githubの段落の行間や、プレーンテキストの空行くらいのスペースはどちらかというと、章わけに近い気がする。  
  
  
  
  
  
  
  
  
  
  
　以下、androidのスペックの文。chatGPTは見出しとして###。これはlatexでのsectionだと思えばいい。そして、subsectionを##ではなく、**a**という太字を使っている。参考にしたい。  
  
### Samsung Galaxy SC-53B (Galaxy A52 5G)の仕様  
  
**ディスプレイ**:  
- サイズ: 6.5インチ  
- 解像度: 2400×1080ピクセル  
- 種類: Super AMOLED, 120Hzリフレッシュレート  
  
**CPU**:  
- Qualcomm Snapdragon 750G 5G  
- ベンチマークスコア: シングルコア約700、マルチコア約1900（推定）  
  
**カメラ**:  
1. **メインカメラ**:  
   - 解像度: 64MP  
   - 絞り値: f/1.8  
   - OIS（光学手ブレ補正）  
   - 4Kビデオ撮影最大30fps  
  
2. **超広角カメラ**:  
   - 解像度: 12MP  
   - 絞り値: f/2.2  
  
### 参考資料  
- [PhoneDB](https://phonedb.net)  
- [GSMArena](https://www.gsmarena.com)  
- [Welectronics](https://www.welectronics.com)  
  
これらの情報に基づき、Samsung Galaxy SC-53B（Galaxy A52 5G）はミドルレンジのスマートフォンであり、優れたディスプレイ、カメラ性能、5G対応を特徴としています。  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
────excel(.xlsx)  
　exelのメモ。正直exelを使うことは今後ないだろうが。  
  
　なぜエクセルを使うしかなかったのかを考えたい。まず、私が欲しいのは参加者情報のcsvファイルだ。  
　一つの方法として、webページを作り、それぞれの代表者にアカウントを配り、そこで参加者の情報を登録してもらう。悪くない考えだと思う。  
　そして、次に閃いたのが、参加者情報の登録を、csv形式で入力してもらい、checkを押すと、名前に変な記号が含まれています、というようなエラーが出て、全て正すとsubmitができるというもの。一応エラーが出て提出できない場合に下にもうひとつ提出ボックスを作り、何かあったら送ってくださいとする。  
　あと、オートセーブとかがめんどくさいので、やはりcheckとsaveにしよう。いや、save, check, submitで、submitを押したら、ありがとうございました、以下のものが提出されました、もし訂正がある場合はこちらで訂正してください、rewiteとするか。  
　それならエクセルを自作して、save, check, submit, rewiteをつけるか。  
  
　データ、データの入力規則、リストと進み、  
a,b,c,d  
　とコンマ区切りで入力すれば、abcdしか入れられなくなる。任意の範囲を選択してこれをすれば、規則がない枠にも適用しますかと出るので、これで規則をコピーできる。  
　校閲で保護をオンにして、保護を解除する範囲をクリックして、選択すれば、その範囲のみ変更可能になる。エクセル配って入力してもらう場合に便利。  
  
  
  
  
  

