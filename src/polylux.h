#include <array>

namespace polylux {
   using function = void(*)();
   struct named_function {
	   const char *name;
	   function f;
   };
   template <size_t N> using function_table_t = std::array<named_function, N>;

namespace inner {
} // namespace inner

} // namespace polylux
