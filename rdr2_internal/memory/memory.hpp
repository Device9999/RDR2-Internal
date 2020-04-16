#pragma once

namespace memory
{
	static uintptr_t find_signature(const char* module, const char* pattern_, const char* mask) {
		const auto compare = [](const uint8_t * data, const uint8_t * pattern, const char* mask_) {
			for (; *mask_; ++mask_, ++data, ++pattern)
				if (*mask_ == 'x' && *data != *pattern)
					return false;

			return (*mask_) == 0;
		};


		MODULEINFO module_info = {};
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module), &module_info, sizeof MODULEINFO);

		auto module_start = uintptr_t(module_info.lpBaseOfDll);
		const uint8_t* pattern = reinterpret_cast<const uint8_t*>(pattern_);
		for (size_t i = 0; i < module_info.SizeOfImage; i++)
			if (compare(reinterpret_cast<uint8_t*>(module_start + i), pattern, mask))
				return module_start + i;

		return 0;
	}

}