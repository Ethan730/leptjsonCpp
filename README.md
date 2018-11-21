受lept_json项目启发，学习别人的C++版本实现

## 2018/11/21

完成了unicode的解析

注意的地方：如果字符串中本来有`'\0'`，初始化string的时候需要指定长度，即使用下面的构造函数。

```c++
string (const char* s, size_t n);
```

不然遇到`'\0'`初始化会停止，得到的string是不对的。

## 2018/11/19

1. 完成了number的解析

遇到的问题：

最初写成了这样，number_too_big的测试用例怎么都通不过。。

```c++
double n = strtod(p, nullptr);
errno = 0;
```

感觉自己宛如一个智障。。转换完了把errno重新赋值怎么可能检测到ERANGE

正确的应该是下面的写法

```c++
errno = 0;
double n = strtod(p, nullptr);
```
2. string除了unicode的解析

切记！**每次switch加了一个case先加break**

学到的点：

- 匿名union
- 手动调用析构函数
- 如果对栈区的数据重新调用构造函数

## 2018/11/18

完成了基本的设计，完成了null和boolean（true/false）两种类型的解析。

学了一些gtest的一些皮毛