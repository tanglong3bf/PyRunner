# PyRunner

更方便的使用C++语言调用Python的代码。

用到了一点点的C++17特性。

## 已经支持的功能

执行单一语句
调用Python模块中的函数，参数列表支持int, double类型，返回值支持int, double, void类型。

## 例子

**执行单一语句**

```cpp
PyRunner().SimpleString("print('Hello world')");
```

**调用Python模块中的函数**

```cpp
PyRunner runner("./scripts/testAdd.py");
//PyRunner runner("./scripts/testAdd");
//PyRunner runner("testAdd.py");
//PyRunner runner("testAdd");
std::cout << runner.CallFunc<int>("add")(123, 312) << std::endl;
std::cout << runner.CallFunc<double>("add")(1.23, 312) << std::endl;
```

## 后续可能完善的功能

- 参数列表与返回值添加字符串类型
- 调用Python模块中的类
