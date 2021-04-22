#include "DLL_MAIN.h"
#include "Utils.h"

//
//uint8_t* cUtils::FindPatternIDA(/*const char**/HMODULE hModule, const char* szSignature)
//{
//	static auto pattern_to_byte = [](const char* pattern) {
//		auto bytes = std::vector<int>{};
//		auto start = const_cast<char*>(pattern);
//		auto end = const_cast<char*>(pattern) + strlen(pattern);
//
//		for (auto current = start; current < end; ++current) {
//			if (*current == '?') {
//				++current;
//				if (*current == '?')
//					++current;
//				bytes.push_back(-1);
//			}
//			else {
//				bytes.push_back(strtoul(current, &current, 16));
//			}
//		}
//		return bytes;
//	};
//
//	//auto Module = GetModuleHandleA(szModule);
//
//	auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
//	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);
//
//	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
//	auto patternBytes = pattern_to_byte(szSignature);
//	auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);
//
//	auto s = patternBytes.size();
//	auto d = patternBytes.data();
//
//	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
//		bool found = true;
//		for (auto j = 0ul; j < s; ++j) {
//			if (scanBytes[i + j] != d[j] && d[j] != -1) {
//				found = false;
//				break;
//			}
//		}
//		if (found) {
//			return &scanBytes[i];
//		}
//	}
//	return nullptr;
//
//}
