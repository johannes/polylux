#pragma once

#include <array>
#include <string_view>

namespace polylux {

// TODO: return_value_wrapper and argument_wrapper have to be merged in order to
// be able to support arrays/lists etc. and doing anything interesting.
class return_value_wrapper {
public:
  virtual ~return_value_wrapper() = default;

  // TODO: Do I /really/ want to use operator= here?
  virtual bool operator=(bool value) = 0;
  virtual long operator=(long value) = 0;
  virtual double operator=(double value) = 0;
  virtual std::string_view operator=(std::string_view value) = 0;
};

class argument_list_wrapper;

class argument_wrapper final {
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

using function = void (*)(return_value_wrapper &return_value,
                          const argument_list_wrapper &args);

struct named_function {
  const char *name;
  function f;
   };
   template <size_t N> using function_table_t = std::array<named_function, N>;

struct info {
  const char *name;
  const char *version_str;
};

} // namespace polylux

extern const polylux::info polylux_info;
