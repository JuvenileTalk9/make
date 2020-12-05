# cmake

## cmakeとは

cmakeは、ビルドをOSやコンパイラに依存せず実行するためのツールです。[前章](https://github.com/JuvenileTalk9/make/blob/main/02_make/make.md)でmakeの使い方を説明しましたが、makeはLinux環境においてGCCコンパイラでビルドを実行するツールです。一方で、WidnowsではMicrosoft Visual C++コンパイラでビルドを実行します。Visual C++コンパイラはMakefileの代わりに*.slnファイルでビルドオプションなどを定義しています。同様にMacOSでは、、、というように、OSやコンパイラによってビルドのツールが異なることが移植性の面で課題となっておりました。

cmakeを使うと環境に応じて自動的にMakefileや*.slnファイルを生成することができます。CMakeLists.txtという設定ファイルにビルドオプションを記述することで、任意の環境でビルドの準備が完了します。

## CMakeLists.txtの基本的な書き方

[make](https://github.com/JuvenileTalk9/make/blob/main/02_make/make.md)のときと同じ以下のプログラムを実行することを考えます。

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

このプログラムをビルドするMakefileを生成するために以下のCMakeLists.txtを作成します。

```cmake
cmake_minimum_required(VERSION 2.8)
project(test_cmake CXX)

include_directories(/home/centos/boost_1_74_0/build/include)
link_directories(/home/centos/boost_1_74_0/build/lib)

add_library(file_check SHARED file_check.cpp)
target_link_libraries(file_check boost_filesystem)

add_executable(hello main.cpp)
target_link_libraries(hello boost_filesystem file_check)
```

- ```cmake_minimum_required```で、このCMakeLists.txtが前提とするcmakeの最低バージョンを指定します。今回は2.8以上を指定しました。

- ```project```でcmakeプロジェクトの名称をつけます（ここでつけた名前がどこで使われるのかが謎。なくてもOKだが。。。）

- ```include_directories```でインクルードディレクトリを指定します。この例ではboostライブラリを使用しているのでboostのインクルードディレクトリを指定しました。

- ```link_directories```で静的・動的ライブラリのパスを指定します。

- ```add_library```で静的・動的ライブラリをコンパイルします。第1引数で生成するライブラリ名を、第2引数でライブラリの種類を、第3引数でソース・ファイルを指定します。ライブラリの種類は静的ライブラリを生成する```STATIC```か、動的ライブラリを生成する```SHARED```を指定します。ここでは```SHARED```を指定しているので、```libfile_check.so```が生成されます。

- ```target_link_libraries```で指定したライブラリまたはプログラムのリンク先を指定します。ここでは1つ前で生成した```libfile_check.so```にboostのlibboost_filesystem.soをリンクさせています。

- ```add_executable```で実行プログラムを生成します。

- ```target_link_libraries```で生成した実行プログラム```hello```に、先程生成した```libfile_check.so```と```libboost_filesystem.so```をリンクさせています。

このプログラムを実行します。ビルドする場合は```build```ディレクトリなどを生成し、その中でビルド作業を実行します。こうすることでビルドに必要な中間ファイルをすべて```build```の中で閉じることになり。```build```を削除するだけで初期化することができるようになります。

```sh
$ mkdir build
$ cd build/
$ cmake ..
-- The CXX compiler identification is GNU 4.8.5
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- PROJECT_NAME=test_cmake
-- PROJECT_SOURCE_DIR=/home/centos/Git/make/03_cmake
-- PROJECT_BINARY_DIR=/home/centos/Git/make/03_cmake/build
-- Configuring done
-- Generating done
-- Build files have been written to: /home/centos/Git/make/03_cmake/build
$ make
Scanning dependencies of target file_check
[ 50%] Building CXX object CMakeFiles/file_check.dir/file_check.cpp.o
Linking CXX shared library libfile_check.so
[ 50%] Built target file_check
Scanning dependencies of target hello
[100%] Building CXX object CMakeFiles/hello.dir/main.cpp.o
Linking CXX executable hello
[100%] Built target hello
$ ./hello
main.cpp does not exist.
```

## cmake 逆引きあれこれ

### コメントを書きたい！

コメントを書くときは```#```を使います。

```cmake
# コメント
cmake_minimum_required(VERSION 2.8)
project(test_cmake CXX)
```

### 変数を使いたい！

CMakeLists.txtでは、変数はすべて文字列として処理されます。```1```も```hoge```も文字列です。変数を定義するときは```set```を使います。

```cmake
set(value1 1)
```

変数の中身を参照したい場合は```${}```で変数名を書き見ます。コンソールに文字列を出力したいときは```message```が使えます。CMakeLists.txtの変数名は大文字小文字を区別します。存在しない変数を使おうとすると、空文字として処理されます。

```cmake
message(value1=${value1})
# value1=1
message(VALUE1=${VALUE1})
# VALUIE1=
```

変数を削除したい場合は```unset```を使います。

```cmake
unset(value1)
message(value1=${value1})
# value1=
```

### キャッシュ変数を使いたい！

先程の変数はcmakeの実行とともに生成され、終了とともに消える変数です。しかし、ライブラリのパスなど実行のたびに変わらない値を毎回生成するのは無駄です。そこで、一度生成した変数をキャッシュファイルに保存しておき、次回起動時はキャッシュファイルから値を読み出すキャッシュ変数が利用できます。

キャッシュ変数を使うときも```set```を使いますが、引数が増えています。

```cmake
message(cache_value1=${cache_value1})
# cache_value1=
set(cache_value1 111 CACHE STRING "cache value")
```

第1、第2引数までは同じで、変数名と値です。キャッシュ変数を使うときは第3引数に```CHACHE```を指定します。第4、第5引数はcmakeのGUIツールcmake-guiなどで使用するものなので、CUIベースで操作する場合適当でOKですが、何も指定しないとエラーになります。

このcmakeを再度実行すると、2回目に```cache_value1```が設定されていることがわかります。

```sh
# 1回目
$ cmake ..
cache_value1=
# 2回目
$ cmake ..
cache_value1=111
```
