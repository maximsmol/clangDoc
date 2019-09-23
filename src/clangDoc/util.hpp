#pragma once

#define mimpl_concat(a, b) a ## b

#define mimpl_cpp_nocopy(classname) \
  classname(const classname& that) = delete; \
  classname& operator=(const classname& that) = delete;

#define mimpl_cpp_swap(classname) \
  friend void swap(classname& a, classname& b) noexcept

#define mimpl_cpp_copy_and_swap(classname) \
  classname(classname&& that) noexcept : \
    classname() \
  { \
    swap(*this, that); \
  } \
\
  classname& operator=(classname&& that) noexcept { \
    swap(*this, that); \
    return *this; \
  } \
\
  mimpl_cpp_swap(classname)

#define mimpl_any_const_getter_args(rettype, classname, method, args_list, call_list) \
  rettype method args_list { \
    return const_cast<rettype>(const_cast<const classname*>(this)->method call_list); \
  } \
  \
  rettype method args_list const

  #define mimpl_any_const_getter(rettype, classname, method) \
  rettype method() { \
    return const_cast<rettype>(const_cast<const classname*>(this)->method()); \
  } \
  \
  rettype method() const
