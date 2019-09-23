#pragma once

#define mimpl_concat(a, b) a ## b

#define mimpl_cpp_nocopy(classname) \
  classname(const classname& that) = delete; \
  classname& operator=(const classname& that) = delete;

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
  friend void swap(classname& a, classname& b) noexcept

#define mimpl_any_const_getter(rettype, classname, method, ...) \
  rettype method(__VA_ARGS__) { \
    return const_cast<rettype>(const_cast<const classname*>(this)->method(__VA_ARGS__)); \
  } \
  \
  rettype method(__VA_ARGS__) const
