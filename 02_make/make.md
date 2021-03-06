# make

## makeとは

ソースファイルが分割される大規模プログラムを作る場合、コマンドで逐一コンパイルすることは面倒です。makeは、Makefileというファイル名のテキストファイルにビルドの手順を記述し、```make```コマンドを実行することで、自動的にコンパイルを実行することができます。

## Makefileの基本的な書き方

Makefileは以下のような構造で記述します。

```Makefile
[変数名] = [値]

[作りたいもの]: [必要な材料]
    [作り方]
```

[作り方]の前はTabを挿入します。スペース4つなどではエラーとなるので注意してください。makeを実行するときは、Makefileの記述に合わせて、```make [作りたいもの]```と実行します。```[作りたいもの]```を省略した場合、一番最初に記述されている```[作りたいもの]```が実行されます。

例として、以下のプログラムがあるとします。

```c++
// main.cpp
#include <iostream>
#include <string>

int file_check(std::string);

int main(int argc, char** argv) {
    std::string file_path = "main.cpp";
    if (file_check(file_path)) {
        std::cout << file_path << " exists." << std::endl;
    } else {
        std::cout << file_path << " does not exist." << std::endl;
    }
    return 0;
}

```

```c++
// file_check.cpp
#include <string>
#include <boost/filesystem.hpp>

bool file_check(std::string file_path) {
    const boost::filesystem::path path(file_path);

    boost::system::error_code error;
    const bool result = boost::filesystem::exists(path, error);
    
    return result && !error;
}
```

このプログラムをビルドインストールするMakefileは以下のように作成します。

```Makefile
CXX      = g++
RM       = rm -rf
OBJS     = main.o file_check.o
PROGRAM  = hello
CXXFLAGS = -O4 -Wall -I/home/centos/boost_1_74_0/build/include
LDFLAGS  = -L/home/centos/boost_1_74_0/build/lib
LIBS     = -lboost_filesystem
DEST     = /usr/local/bin

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:
	$(RM) *.o $(PROGRAM)

install: $(PROGRAM)
	install -d $(DEST)
	install -s $(PROGRAM) $(DEST)

```

先頭に、以降で使用する変数を定義します。

- ```CXX```はg++コンパイラを指定する変数です。慣習的に、Cであれば```CC = gcc```を、C++であれば```CXX = g++```を指定します。

- ```RM```はmakeが失敗したときに不要なファイルを削除するときに削除コマンドを定義する変数です。

- ```OBJS```は生成するオブジェクトファイルを記述します。

- ```PROGRAM```は最終的に生成するプログラムファイルを記述します。

- ```CXXFLAGS```はオブジェクトファイル生成時に必要なパラメータを設定します。以降の記述で```CXXFLAGS```が使われていませんが、ソースファイルのコンパイル時に自動的にこの値が設定されます。

- ```LDFLAGS```と```LIBS```はリンクされるライブラリファイルのファイルパスとライブラリ名をリンク時のパラメータと同様に指定します。

- ```DEST```は```make install```を実行したときのインストール先を記述します。

次に、作りたいものと作り方を記述していきます。

- ```all```は作り方が書かれていませんが、必要な材料として```$(PROGRAM)```が指定されています。つまり、これは```$(PROGRAM)```を実行しなさいという意味のみを持ち、慣習的に記述されていることが多いです。通常、```all```はすべての作りたいものの先頭に記述し、```make all```を実行しても、```make```を実行しても、ビルドしたいものが生成できるようにします。

- ```$(PROGRAM)```はこのMakefileがビルドしたいプログラムのビルド方法を記述します。ここで、```$(PBJS)```が必要なものとして記述されていますが、```$(OBJS)```を生成するコードが定義されていません。これは、makeが暗黙のうちに```$(OBJS)```を生成する処理を行うため、通常記述を省略するためです。ただし、オブジェクトファイルの生成時に必要なインクルードファイルの指定や、最適化などのコンパイルオプションを指定するタイミングがないので、ここで指定する必要があります。

- ```clean```は必要な材料が記述されていません。これはビルドで生成されるファイルを削除して初期状態に戻すコードであり、慣習的にcleanの名前がよく使われます。

- ```install```はビルドしたアプリケーションをインストールするコードが記述されています。必要な材料として```$(PROGRAM)```が指定されているので、```make install```とするだけでビルドとインストールが実行されるようになっています。

cleanもinstallも、cleanやinstallというものを作りたいわけではありませんが、makeの仕組みを利用してこのような使われ方がされています。

このMakefileを実行したいときは、Makefileと同じ階層のディレクトリで以下のように実行します。

```sh
# 普通の実行方法
$ make hello
g++ -O4 -Wall -I/home/centos/boost_1_74_0/build/include   -c -o main.o main.cpp
g++ -O4 -Wall -I/home/centos/boost_1_74_0/build/include   -c -o file_check.o file_check.cpp
g++ main.o file_check.o -L/home/centos/boost_1_74_0/build/lib -lboost_filesystem -o hello

# makeを省略してもOK（make allも同じ）
$ make
g++ -O4 -Wall -I/home/centos/boost_1_74_0/build/include   -c -o main.o main.cpp
g++ -O4 -Wall -I/home/centos/boost_1_74_0/build/include   -c -o file_check.o file_check.cpp
g++ main.o file_check.o -L/home/centos/boost_1_74_0/build/lib -lboost_filesystem -o hello

# makeに失敗するなどで要らなくなったファイルはcleanする。
$ make clean
rm -f *.o hello

# ビルドに成功したらインストールする
$ sudo make install
install -d /usr/local/bin
install -s hello /usr/local/bin
```

## makeを使った小技

### インストール先を調べる

```-n```オプションをつけることで、実際に実行せず、実行するコマンドの表示だけを行うことができる。これを利用して、```make install```した対象のインストール先を調べることができる。

```sh
$ make -n install
install -d /usr/local/bin
install -s hello /usr/local/bin
```

### 並列実行

```-j```オプションをつけることで、ビルド作業の並列処理の数を指定できます。数を省略することで無制限に並列処理が可能ですが、CPUのコア数に合わせて制限するほうが、メモリ溢れのリスクもなくなります。

```sh
# 4並列でビルドする例
$ make -j4
```

[目次へ](https://github.com/JuvenileTalk9/make)
