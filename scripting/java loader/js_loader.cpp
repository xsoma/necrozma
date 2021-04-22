#include "js_loader.h"

std::vector<c_js_script> c_js_loader::scripts = {};

bool c_js_loader::convert_v_string(std::vector<std::string> script_names)
{
	scripts.clear();
	for (auto name : script_names)
	{
		auto script = c_js_script{ name, none, false };
		scripts.push_back(script);
	}
	return true;
}

std::vector<c_js_script> c_js_loader::refresh_scripts(std::vector<std::string> script_names)
{
	bool resp = convert_v_string(script_names);
	return scripts;
}

void c_js_loader::execute_scripts(exec_locations loc)
{
	for (auto& js_script : scripts)
	{
		if (js_script.execution_location != loc)
			continue;

		if (!js_script.is_enabled)
			continue;

		if (js_script.execution_location == none) //just in case ...
			continue;

		js_script.execute();
	}
}
