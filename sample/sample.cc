#include "../src/entry/entry.h"

#include "../src/polylux.h"

#include <iostream>

namespace {
	void hello() {
		std::cout << "hello ";
	}
	void world() {
		std::cout << "world\n";
	}
}

static polylux::function_table_t<2> function_table{
    polylux::named_function{"hello", hello},
    polylux::named_function{"world", world}};

POLYLUX_ENTRY(polylux_demo, "1.0.0", function_table)
