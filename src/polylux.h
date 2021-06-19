#pragma once

#include <array>
#include <string_view>

namespace polylux {
class argument_list_wrapper;

class argument_wrapper {
   const argument_list_wrapper &args;
   size_t  offset;

 public:
   explicit argument_wrapper(const argument_list_wrapper &args, size_t offset)
       : args{args}, offset{offset} {}
   
   bool as_bool() const;
   long as_long() const;
   double as_double() const;
   std::string_view as_string() const; /* TODO: in Python we have to mind string types binary/unicode and in Node.js we might want to support uint8arrays etc. as binary string */

   // underlying PyObject, zval, napi_object etc. (consumer currently has no API to identify ...)
   void *raw() const;
};

class argument_list_wrapper {
  friend argument_wrapper;
public:
  virtual ~argument_list_wrapper() = default;

  virtual size_t count() const = 0;
  argument_wrapper operator[](size_t offset)  const {
    return argument_wrapper{*this, offset};
  }

  // They are here as that eases ownership handling. The consumer should go via
  // operator[]
  virtual bool as_bool(size_t offset) const = 0;
  virtual long as_long(size_t offset) const = 0;
  virtual double as_double(size_t offset) const = 0;
  virtual std::string_view as_string(size_t offset) const = 0;

  virtual void *raw(size_t offset) const = 0;
};

using function = void (*)(const argument_list_wrapper &args);
struct named_function {
  const char *name;
  function f;
   };
   template <size_t N> using function_table_t = std::array<named_function, N>;

} // namespace polylux
