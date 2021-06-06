Polylux - One Binary Multiple Languages
======================================

Polylux is a framework for building C++ extensions for common scripting
languages, where a single binary can be loaded into different languages.

This software is purely experimental toy software and not suggested for 
any form of serious usage.

Example
-------

Given an extension source like this:

```c++
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
    polylux::named_function{"world", world}
};

POLYLUX_ENTRY(polylux_demo, "1.0.0", function_table)
```

And compiled like this:

```sh
clang++  --std=c++i17  -fvisibility=hidden -Wextra -g -ldl -fPIC   -o polylux_demo.so -shared src/entry/entry_php.cc src/entry/entry_python.cc  sample/sample.cc
```

The resulting binary can be called from Python:

```
$ python3 -c "import polylux_demo; print(polylux_demo); polylux_demo.hello(); polylux_demo.world();"                           
<module 'polylux_demo' from '/home/johannes/src/polylux/polylux_demo.so'>
hello world
```

Or from PHP:

```
$ php -n -d extension_dir=. -d extension=./polylux_demo.so -r 'echo new ReflectionExtension("polylux_demo"); hello(); world();'                                    
Extension [ <persistent> extension #15 polylux_demo version 1.0.0 ] {  - Functions {
    Function [ <internal:polylux_demo> function hello ] {
    }
    Function [ <internal:polylux_demo> function world ] {
    }
  }
}
hello world
```  

What is missing?
---------------

* Everything!
* Handling of function arguments and return values
* a build system
* handlign of common APIs
* really everything I didn'T implelement in a Sunday hacking session

License
------

Polylux itself is licensed under MIT license. It also contains 
elemtens from PHP any Python. See LICENSE File for attribution.

About the name
-------------

Naming is hard ... polylux as word came to my head somehow.
Much light, depending which runtime shines on it.
