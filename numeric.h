_Pragma("once")
/* 这个文件包含mysql的数值算法 */
#include "iterator.h"
namespace mystl
{

// accumulate
// 版本1：以初值 init 对每个元素进行累加
// 版本2：以初值 init 对每个元素进行二元操作
template <class InputIter, class T>
T accumulate(InputIter first, InputIter last, T init)
{
  for (; first != last; ++first)
  {
    init += *first;
  }
  return init;
}
/* BinaryOp里会重载（），binary_op里面在调用的时候会设定累加的规则 */
template <class InputIter, class T, class BinaryOp>
T accumulate(InputIter first, InputIter last, T init, BinaryOp binary_op)
{
  for (; first != last; ++first)
  {
    init = binary_op(init, *first);
  }
  return init;
}
// adjacent_difference
// 版本1：计算相邻元素的差值，结果保存到以 result 为起始的区间上
// 版本2：自定义相邻元素的二元操作
/* 容器内容是1，4，6，8，11，那么得到的结果就是1，3，2，2，3 
注意这里指向的并非是最后一个元素，而是最后一个元素的下一个空间*/
template <class InputIter, class OutputIter>
OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result)
{
  if (first == last)  return result;
  *result = *first;  // 记录第一个元素
  auto value = *first;
  while (++first != last)
  {
    auto tmp = *first;
    *++result = tmp - value;
    value = tmp;
  }
  return ++result;
}
template <class InputIter, class OutputIter, class BinaryOp>
OutputIter adjacent_difference(InputIter first, InputIter last, OutputIter result,BinaryOp binary_op)
{
  if (first == last)  
    return result;
  *result = *first;  // 记录第一个元素
  auto value = *first;
  while (++first != last)
  {
    auto tmp = *first;
    *++result = binary_op(tmp, value);
    value = tmp;
  }
  return ++result;
}
// 版本1：以 init 为初值，计算两个区间的内积   
// 版本2：自定义 operator+ 和 operator*
template <class InputIter1, class InputIter2, class T>
T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init)
{
  for (; first1 != last1; ++first1, ++first2)
  {
    init = init + (*first1 * *first2);
  }
  return init;
}

template <class InputIter1, class InputIter2, class T, class BinaryOp1, class BinaryOp2>
T inner_product(InputIter1 first1, InputIter1 last1, InputIter2 first2, T init,BinaryOp1 binary_op1, BinaryOp2 binary_op2)
{
  for (; first1 != last1; ++first1, ++first2)
  {
    init = binary_op1(init, binary_op2(*first1, *first2));
  }
  return init;
}
//填充[first, last)，以 value 为初值开始递增
template <class ForwardIter, class T>
void iota(ForwardIter first, ForwardIter last, T value)
{
  /* while (first != last)
  {
    *first++ = value;
    ++value;
  } */

  /* int *arr=new int[10];
    while(i<20)
        *arr++=i++; */
    /* 第一个注释是原作者的代码，被我稍微修改，为了避免出现问题用第二个注释
    代码进行了检验，结果没有错误，在实际测试时候也并没有出现问题 */
  while(first!=last)
    *first++=value++;
}
// 版本1：计算局部累计求和，结果保存到以 result 为起始的区间上
// 版本2：进行局部进行自定义二元操作
template <class InputIter, class OutputIter>
OutputIter partial_sum(InputIter first, InputIter last, OutputIter result)
{
    /* 设容器内容为1，2，3，4，5，那么result的结果为
    1，3，6，10，15，也就是到第i个为止的元素的累加和，最后result指向
    15的下一个元素 */
  if (first == last)  return result;
  *result = *first;  // 记录第一个元素
  auto value = *first;
  while (++first != last)
  {
    value = value + *first;
    *++result = value;
  }
  return ++result;
}

// 版本2
template <class InputIter, class OutputIter, class BinaryOp>
OutputIter partial_sum(InputIter first, InputIter last, OutputIter result,
                       BinaryOp binary_op)
{
  if (first == last)  return result;
  *result = *first;  //记录第一个元素
  auto value = *first;
  while (++first != last)
  {
    value = binary_op(value, *first);
    *++result = value;
  }
  return ++result;
}

}