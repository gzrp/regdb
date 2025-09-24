# RegDB

扩展 RegDB

git clone --recurse-submodules https://github.com/gzrp/regdb.git

## 构建

### 管理依赖

DuckDB扩展使用VCPKG进行依赖管理。VCPKG 参考 [installation instructions](https://vcpkg.io/en/getting-started) 或者运行下述命令:

```shell
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh
export VCPKG_TOOLCHAIN_PATH=`pwd`/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 构建步骤

构建扩展运行 make 命令:
```sh
make
```

构建结果
```sh
./build/release/duckdb
./build/release/test/unittest
./build/release/extension/regdb/regdb.duckdb_extension
```

- duckdb：DuckDB 的 shell 可执行文件，已经自动加载了扩展代码。
- unittest：DuckDB 的测试运行程序，同样已经把扩展链接进去了。
- <extension_name>.duckdb_extension：可加载的扩展二进制文件，这就是最终要分发的扩展。

DuckDB 的扩展目前依赖 DuckDB 自带的构建系统来进行测试和分发。但是，这样做的缺点是：每次构建扩展时，都必须重新编译 DuckDB 和它的 unittest 二进制文件。

为缓解这个问题，强烈推荐安装 ccache 和 ninja： ccache 可以缓存编译结果，避免重复编译相同代码；
ninja 是一个更快的构建工具，比默认的 make 快很多。 这样一来，你只需要在第一次完整构建时编译核心 DuckDB，之后扩展的重新编译会非常迅速。

```
sudo apt-get install ccache ninja-build
GEN=ninja make
```

## 运行扩展

在 Shell 中运行 `./build/release/duckdb`.

现在我们可以直接在DuckDB中使用扩展的特性了。模板包含一个标量函数‘ regdb() ’，它接受一个字符串参数并返回一个字符串：

```
D select quack('Jane') as result;
┌───────────────┐
│    result     │
│    varchar    │
├───────────────┤
│ Regdb Jane 🐥 │
└───────────────┘
```

## 运行测试

可以为DuckDB扩展创建不同的测试。测试DuckDB扩展的主要方法应该是‘ ./test/ SQL ’中的SQL测试。可以使用以下命令运行这些SQL测试：

```sh
make test
```
