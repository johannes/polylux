#include <dlfcn.h>
#include <iostream>

#include "entry.h"
#include "../backend/backend_php.h"

struct zend_module_entry {
	unsigned short size;
	unsigned int zend_api;
	unsigned char zend_debug;
	unsigned char zts;
	const struct _zend_ini_entry *ini_entry;
	const struct _zend_module_dep *deps;
	const char *name;
	const void /*struct _zend_function_entry*/ *functions;
	void *module_startup; //int (*module_startup_func)(INIT_FUNC_ARGS);
	void *module_shutdown; //int (*module_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	void *request_startup; //int (*request_startup_func)(INIT_FUNC_ARGS);
	void *request_shutdown; //int (*request_shutdown_func)(SHUTDOWN_FUNC_ARGS);
	void *info_func; //void (*info_func)(ZEND_MODULE_INFO_FUNC_ARGS);
	const char *version;
	size_t globals_size;
#ifdef ZTS
	unsupported
	ts_rsrc_id* globals_id_ptr;
#else
	void* globals_ptr;
#endif
	void *globals_ctor; //void (*globals_ctor)(void *global);
	void *globals_dtor; //void (*globals_dtor)(void *global);
	void *post_deactivate; //int (*post_deactivate_func)(void);
	int module_started;
	unsigned char type;
	void *handle;
	int module_number;
	const char *build_id;
};

namespace {
static zend_module_entry module_entry{};

zend_module_entry *get_some_other_module_entry() {
  // We are not relying on RT_LD_LAZY here, but are using dlsym to get a nicer
  // error message
  typedef zend_module_entry **(*zend_hash_str_find_func)(void *, const char *, size_t);
  zend_hash_str_find_func zend_hash_str_find = reinterpret_cast<zend_hash_str_find_func>(dlsym(nullptr, "zend_hash_str_find"));
  auto module_registry = dlsym(nullptr, "module_registry");
  if (!zend_hash_str_find || !module_registry) {
    return nullptr;
  }
  return *zend_hash_str_find(module_registry, "standard",
                            sizeof("standard") - 1);
}
}

namespace polylux {
namespace entry {
namespace php {
/*extern "C" __attribute__((visibility("default"))) */
zend_module_entry *get_module(zend_function_entry *function_table) {
  module_entry.size = sizeof(zend_module_entry);

  module_entry.name = polylux_info.name;
  module_entry.version = polylux_info.version_str;
  module_entry.functions = function_table;

  auto other_module = get_some_other_module_entry();
  if (!other_module || other_module->size != sizeof(zend_module_entry)) {
    std::cerr << "get_module() invoked, but caller not a known version of PHP. "
                 "Aborting."
              << std::endl;
    std::abort();
  }

  if (other_module->zend_api < 2010'01'01 ||
      other_module->zend_api >= 2022'00'00) {
    // We know we are in some version of PHP, but don't know which. Let PHP
    // handle this.
    // TODO: The API versions are more or less randomly picked. Need to define
    // supported versions
    std::cerr << "PHP reported Zend API " << other_module->zend_api
              << " we don't know." << std::endl;
    return &module_entry;
  }

  module_entry.zend_api = other_module->zend_api;
  module_entry.build_id = other_module->build_id;
  module_entry.zend_debug = other_module->zend_debug;
  module_entry.zts = other_module->zts;

  return &module_entry;
}

} // namespace php
} // namespace entry
} // namespace polylux
