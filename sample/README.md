# Sample
基本構成のsample
Makefileについて、基本https://www.partow.net/programming/makefile/index.html をそのまま参考にしているが、テストコードも実行できるようにしている。

```
├── Makefile
├── README.md
├── build
│   ├── apps
│   │   └── program
│   ├── gtest
│   │   ├── libgmock.a
│   │   ├── libgmock_main.a
│   │   ├── libgtest.a
│   │   └── libgtest_main.a
│   ├── objects
│   │   ├── src
│   │   │   ├── main.d
│   │   │   ├── main.o
│   │   │   ├── module1
│   │   │   │   ├── incl.d
│   │   │   │   └── incl.o
│   │   │   └── module2
│   │   │       ├── hoo.d
│   │   │       └── hoo.o
│   │   └── test
│   │       ├── module1
│   │       │   ├── incl_test.d
│   │       │   └── incl_test.o
│   │       ├── module2
│   │       │   └── hoo_test.d
│   │       ├── test_sample.d
│   │       └── test_sample.o
│   └── tests
│       ├── module1
│       │   └── incl_test
│       ├── module2
│       │   └── hoo_test
│       └── test_sample
├── gtest
│   └── googletest-release-1.11.0
│       ├── BUILD.bazel
│       ├── CMakeLists.txt
│       ├── CONTRIBUTING.md
│       ├── CONTRIBUTORS
│       ├── LICENSE
│       ├── README.md
│       ├── WORKSPACE
│       ├── build
│       │   ├── CMakeCache.txt
│       │   ├── CMakeFiles
│       │   ├── ...
│       ├── googletest
│       │   ├── CMakeLists.txt
│       │   ├── README.md
│       │   ├── ...
├── include
│   ├── module1
│   │   └── incl.hpp
│   └── module2
│       └── hoo.hpp
├── src
│   ├── main.cpp
│   ├── module1
│   │   └── incl.cpp
│   └── module2
│       └── hoo.cpp
└── test
    ├── module1
    │   └── incl_test.cpp
    ├── module2
    │   └── hoo_test.cpp
    └── test_sample.cpp
```