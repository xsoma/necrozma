#pragma once
#include <string>
#include "../duktate api/duk_config.h"
#include "../../Hooks.h"
#define ret_type static duk_ret_t
#define duk_ctx duk_context* ctx
class c_js_funcs
{
private:
	ret_type add_log(duk_ctx);
	ret_type add_rectfilled(duk_ctx);
	ret_type add_to_hook(duk_ctx);
public:
	//call in dllmain
	void setup_js();
	static std::string read_script_from_file(const std::string path);
	static std::vector<std::string> get_js_files();
private:
};

extern c_js_funcs js_funcs;