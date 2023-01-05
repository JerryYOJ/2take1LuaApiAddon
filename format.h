#pragma once
#include "Include.h"

namespace std {
	template<typename ... Args>
	static std::wstring format(const std::wstring& format, Args ... args)
	{
		auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		std::unique_ptr<char[]> buf(new(std::nothrow) char[size_buf]);

		if (!buf)
			return std::wstring("");

		std::snprintf(buf.get(), size_buf, format.c_str(), args ...);
		return std::wstring(buf.get(), buf.get() + size_buf - 1);
	}


	template<typename ... Args>
	static std::string format(const std::string& format, Args ... args)
	{
		auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
		std::unique_ptr<char[]> buf(new(std::nothrow) char[size_buf]);

		if (!buf)
			return std::string("");

		std::snprintf(buf.get(), size_buf, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size_buf - 1);
	}
}

