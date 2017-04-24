#pragma once

#include <boost/regex.hpp>

#include <cstddef>
#include <cctype>
#include <cstring>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <deque>
#include <string>

#define D_FUNCTION_DECL(ClassName) ClassName##Private* d_function() const

#define D_FUNCTION_IMPL(ClassName) ClassName##Private* ClassName::d_function() const \
{ \
	static ClassName##Private d_object; \
	return &d_object; \
}

#define D_FUNCTION(ClassName) ClassName##Private* const m_d = d_function()