_Pragma("once")
/* 简介一下散列表和解决哈希冲突的方法-开链法
    散列表的构造简单来说就是一个数据a通过一个加工处理变成了b
        直接定址法：用一个函数y=a*x+b，输入x，通过函数得到对应的y
        除留余数法：将直接定址法的函数固定位取余，也就是函数为：y=x/b
        数字分析：例如存电话号，前几位都是1111111那么就可以用后四位来作为真正的用户
        还有折叠法、平方去中法、随机数法等
    但是上述的方法会产生一个问题，我们散列表的构建都是输入一个x加工处理后得到一个y
    但是我们无法确定得到的y是独一无二的，那么当产生一样的y时我们就说这个时冲突

    解决方法：
        线性探测：例如得到的y应该放到6号内存，而6号已经被占用，那么我们就检查相邻的下一个内存
        二次探测：和线性探测类似，但是下一次不会检查相邻内存，第一次检查2^0,第二次2^1...以此类推
        开链法：当有冲突产生时，将产生冲突的位置上的元素用链表的形式串起来

 */

#include <initializer_list>

#include "algo.h"
#include "functional.h"
#include "memory.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"

namespace mystl
{

// hashtable 的节点定义
template <class T>
struct hashtable_node
{
    hashtable_node* next;   // 指向下一个节点
    T               value;  // 储存实值

    //这里取消了默认构造的生成
    hashtable_node() = default;
    hashtable_node(const T& n) :next(nullptr), value(n) {}
    hashtable_node(const hashtable_node& node) :next(node.next), value(node.value) {}
    hashtable_node(hashtable_node&& node) :next(node.next), value(mystl::move(node.value))
    {
        node.next = nullptr;
    }
};

// value traits
template <class T, bool>
struct ht_value_traits_imp
{
    typedef T key_type;
    typedef T mapped_type;
    typedef T value_type;

    //下面两个函数第一次我不是很懂，返回的都是value，并没有想通意义在哪，
    //后来发现value只是形参，是一个泛型，就像函数名字一样，返回值类型和key类型
    template <class Ty>
    static const key_type& get_key(const Ty& value)
    {
        return value;
    } 

    template <class Ty>
    static const value_type& get_value(const Ty& value)
    {
        return value;
    }
};


template <class T>
struct ht_value_traits_imp<T, true>
{
    typedef typename std::remove_cv<typename T::first_type>::type key_type;
    typedef typename T::second_type                               mapped_type;
    typedef T                                                     value_type;

    template <class Ty>
    static const key_type& get_key(const Ty& value)
    {
        return value.first;
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value)
    {
        return value;
    }
};


template <class T>
struct ht_value_traits
{
    static constexpr bool is_map = mystl::is_pair<T>::value;

    typedef ht_value_traits_imp<T, is_map> value_traits_type;

    typedef typename value_traits_type::key_type    key_type;
    typedef typename value_traits_type::mapped_type mapped_type;
    typedef typename value_traits_type::value_type  value_type;

    template <class Ty>
    static const key_type& get_key(const Ty& value)
    {
        return value_traits_type::get_key(value);
    }

    template <class Ty>
    static const value_type& get_value(const Ty& value)
    {
        return value_traits_type::get_value(value);
    }
};
}