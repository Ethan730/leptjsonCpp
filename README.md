受lept_json项目启发，学习别人的C++版本实现

## 2018/12/04

尝试了把项目迁移到Linux，并修复了几个bug。

- unionFree()里忘记写break，不知道为什么windows下还能跑通
- sprintf_s在Linux库中没有，换成了sprintf

最近Linux看到人头大，任重而道远啊，加油，打算改改简历？

## 2018/11/28

完成了比较操作符operator==和operator!=

本来想用C++ 17的std::variant替换union，发现自己的vs15并不支持。。还是算了吧。开始新一篇，学学Linux吧。

## 2018/11/27

完成了序列化

double类型的格式化为"%g"，“%.17g”表示最宽可以有17位，保证精度。

另外C++库函数to_string的精度是不够的，不能保证最大浮点数的转换。

## 2018/11/26

完成了object的解析

今天没有遇到太多问题。

去刷leetcode啦。

## 2018/11/25

完成了array的解析

今天遇到了挺多的问题。

因为涉及到vector的操作，需要为类添加拷贝构造函数和拷贝赋值运算符。

问题出现在拷贝构造函数里，一开始写成了

```c++
this->s=v.s
```

但是由于s并不存在，因此不能赋值，而是应该直接调用构造函数。

在这里，可以直接调用set_系列的函数，因为函数里处理了析构和重新构造。

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