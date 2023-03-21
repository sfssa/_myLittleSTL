/* #ifndef MYTINYSTL_UTIL_H_
#define MYTINYSTL_UTIL_H_

#endif 
原作者使用的是ifndef,个人认为pragma更加简洁
*/
#pragma once//保证头文件只编译一次，也可以_pragma("once")
#include<cstddef>
#include"type_traits.h"
namespace mystl
{
    /* remove_reference针对不同类型（T、T&、T&&），分别重载，只返回不带引用符
    号的值类型 T */
    /* 
    move：不能移动任何东西，它唯一的功能是将一个左值强制转化为右值引用，继而可以
    通过右值引用使用该值，以用于移动语义，从实现上讲，std::move基本等同于一个类
    型转换：static_cast<T&&>(lvalue) */
    template <class T>
    typename std::remove_reference<T>::type&& move(T&& arg) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(arg);
    }
    /* 
    forward: 不转发任何东西，也是执行左值到右值的强制类型转换，只是仅在特定
    条件满足时才执行该转换典型使用场景：某个函数模板取用了万能引用类型为形参
    ，随后把它传递给了另一个函数 */
    template <class T>
    T&& forward(typename std::remove_reference<T>::type& arg) noexcept
    {
        return static_cast<T&&>(arg);
    }
    template <class T>
    T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
    {
        static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
        return static_cast<T&&>(arg);
    }

    template <class Tp>
    void swap(Tp& lhs, Tp& rhs)
    {
        /* 
        交换，用lhs初始化临时变量，然后将临时变量作为中间值
        交换两个对象 ，下面两个参考交换连个数据和交换两数组*/
        auto tmp(mystl::move(lhs));
        lhs = mystl::move(rhs);
        rhs = mystl::move(tmp);
    }

    template <class ForwardIter1, class ForwardIter2>
    ForwardIter2 swap_range(ForwardIter1 first1, ForwardIter1 last1, ForwardIter2 first2)
    {
        for (; first1 != last1; ++first1, (void) ++first2)
            mystl::swap(*first1, *first2);
        return first2;
    }

    template <class Tp, size_t N>
    void swap(Tp(&a)[N], Tp(&b)[N])
    {
        /* 将a中的N个数据复制到b开头的起始地址 */
        mystl::swap_range(a, a + N, b);
    }



    /* 结构体模板 : pair，两个模板参数分别表示两个数据的类型
    用 first 和 second 来分别取出第一个数据和第二个数据 */

    template <class Ty1, class Ty2>
    struct pair
    {
        typedef Ty1    first_type;
        typedef Ty2    second_type;

        first_type first;    // 保存第一个数据
        second_type second;  // 保存第二个数据

        // 默认的可构造函数
        /* is_default_constructible类用来测试是否有默认的构造函数
        enable_if表示满足条件时类型有效 */
        template <class Other1 = Ty1, class Other2 = Ty2,
            typename = typename std::enable_if
            <std::is_default_constructible<Other1>::value &&
            std::is_default_constructible<Other2>::value, void>::type>
            constexpr pair()
            : first(), second(){}

        // 隐式的构造函数
        template <class U1 = Ty1, class U2 = Ty2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::value &&
            std::is_convertible<const U1&, Ty1>::value &&
            std::is_convertible<const U2&, Ty2>::value, int>::type = 0>
            constexpr pair(const Ty1& a, const Ty2& b)
            : first(a), second(b){}

        //显示的构造函数
        template <class U1 = Ty1, class U2 = Ty2,
            typename std::enable_if<
            std::is_copy_constructible<U1>::value &&
            std::is_copy_constructible<U2>::value &&
            (!std::is_convertible<const U1&, Ty1>::value ||
            !std::is_convertible<const U2&, Ty2>::value), int>::type = 0>
            explicit constexpr pair(const Ty1& a, const Ty2& b)
            : first(a), second(b){}

        /* 对对象的赋值构造可能对对象造成改变，因此限制编译器不会生成默认的赋值构造 */
        pair(const pair& rhs) = default;
        pair(pair&& rhs) = default;


        //其他类型的饮隐士构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1&&, Ty1>::value &&
            std::is_convertible<Other2&&, Ty2>::value, int>::type = 0>
            constexpr pair(Other1&& a, Other2&& b)
            : first(mystl::forward<Other1>(a)),
            second(mystl::forward<Other2>(b)){}

        //其他类型的显示构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
            !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
            explicit constexpr pair(Other1&& a, Other2&& b)
            : first(mystl::forward<Other1>(a)),
            second(mystl::forward<Other2>(b)){}


        //对pair的隐式构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            std::is_convertible<const Other1&, Ty1>::value &&
            std::is_convertible<const Other2&, Ty2>::value, int>::type = 0>
            constexpr pair(const pair<Other1, Other2>& other)
            : first(other.first),
            second(other.second){}

    //对pair的显示构造
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, const Other1&>::value &&
            std::is_constructible<Ty2, const Other2&>::value &&
            (!std::is_convertible<const Other1&, Ty1>::value ||
            !std::is_convertible<const Other2&, Ty2>::value), int>::type = 0>
            explicit constexpr pair(const pair<Other1, Other2>& other)
            : first(other.first),
            second(other.second){}


            // implicit constructiable for other pair
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            std::is_convertible<Other1, Ty1>::value &&
            std::is_convertible<Other2, Ty2>::value, int>::type = 0>
            constexpr pair(pair<Other1, Other2>&& other)
            : first(mystl::forward<Other1>(other.first)),
            second(mystl::forward<Other2>(other.second))
        {
        }

        // explicit constructiable for other pair
        template <class Other1, class Other2,
            typename std::enable_if<
            std::is_constructible<Ty1, Other1>::value &&
            std::is_constructible<Ty2, Other2>::value &&
            (!std::is_convertible<Other1, Ty1>::value ||
            !std::is_convertible<Other2, Ty2>::value), int>::type = 0>
            explicit constexpr pair(pair<Other1, Other2>&& other)
            : first(mystl::forward<Other1>(other.first)),
            second(mystl::forward<Other2>(other.second))
        {
        }


        //重载赋值=操作符
        pair& operator=(const pair& rhs)
        {
            if (this != &rhs)
            {
            first = rhs.first;
            second = rhs.second;
            }
            return *this;
        }


        //右值引用来重载赋值操作符
        pair& operator=(pair&& rhs)
        {
            if (this != &rhs)
            {
            first = mystl::move(rhs.first);
            second = mystl::move(rhs.second);
            }
            return *this;
        }

        //对其他类型的重载=操作符
        template <class Other1, class Other2>
        pair& operator=(const pair<Other1, Other2>& other)
        {
            first = other.first;
            second = other.second;
            return *this;
        }

        template <class Other1, class Other2>
        pair& operator=(pair<Other1, Other2>&& other)
        {
            first = mystl::forward<Other1>(other.first);
            second = mystl::forward<Other2>(other.second);
            return *this;
        }

        ~pair() = default;//使编译器不默认生成pair的析构函数
        //交换两个pair
        void swap(pair& other)
        {
            if (this != &other)
            {
                mystl::swap(first, other.first);
                mystl::swap(second, other.second);
            }
        }
    };


    // 重载比较操作符 
    template <class Ty1, class Ty2>
    bool operator==(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
    //重载小于操作符，要么first小于要么second小
    template <class Ty1, class Ty2>
    bool operator<(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }
    //重载不等操作符
    template <class Ty1, class Ty2>
    bool operator!=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs == rhs);
    }
    //重载大于操作符
    template <class Ty1, class Ty2>
    bool operator>(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return rhs < lhs;
    }
    //重载<=
    template <class Ty1, class Ty2>
    bool operator<=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(rhs < lhs);
    }
    //重载>=
    template <class Ty1, class Ty2>
    bool operator>=(const pair<Ty1, Ty2>& lhs, const pair<Ty1, Ty2>& rhs)
    {
        return !(lhs < rhs);
    }

    // 重载 mystl 的 swap
    template <class Ty1, class Ty2>
    void swap(pair<Ty1, Ty2>& lhs, pair<Ty1, Ty2>& rhs)
    {
        lhs.swap(rhs);
    }

    // 全局函数，让两个数据成为一个 pair
    template <class Ty1, class Ty2>
    pair<Ty1, Ty2> make_pair(Ty1&& first, Ty2&& second)
    {
        return pair<Ty1, Ty2>(mystl::forward<Ty1>(first), mystl::forward<Ty2>(second));
    }

}

