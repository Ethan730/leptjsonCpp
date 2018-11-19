受lept_json项目启发，学习别人的C++版本实现

## Day 2：2018/11/19

完成了number的解析

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

## Day 1：2018/11/18

完成了基本的设计，完成了null和boolean（true/false）两种类型的解析。

学了一些gtest的一些皮毛