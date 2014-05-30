/*************************************************************************
 * Copyright (c) 2014 Zhang Dongdong
 * All rights reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**************************************************************************/
#ifndef STRUTIL_H
#define STRUTIL_H
/*
Szymon Rusinkiewicz
Princeton University

strutil.h
Miscellaneous string-manipulation utilities

Usage:
	std::string s("foo.bar");
	std::string s2 = replace_ext(s, "baz");  // "foo.baz"
	begins_with("Foobar", "foo") // true
	ends_with("foobar", "baz") // false
*/


#include <string>
#include <cstring>

#ifdef _WIN32
# ifndef strncasecmp
#  define strncasecmp _strnicmp
# endif
#endif


// Replace the extension of a filename, else add one if none present
static inline std::string replace_ext(const std::string &filename,
	const std::string &ext)
{
	std::string x = filename;
	std::string::size_type dot = x.rfind(".", x.length());
	if (dot != std::string::npos)
		x.erase(dot);
	return x + std::string(".") + ext;
}


// Does string s1 begin/end with s2?  (Case-insensitive)
static inline bool begins_with(const char *s1, const char *s2)
{
	using namespace std;
	return !strncasecmp(s1, s2, strlen(s2));
}

static inline bool begins_with(const std::string &s1, const std::string &s2)
{
	return begins_with(s1.c_str(), s2.c_str());
}

static inline bool ends_with(const char *s1, const char *s2)
{
	using namespace std;
	size_t l1 = strlen(s1), l2 = strlen(s2);
	return (l1 >= l2) && !strncasecmp(s1 + l1 - l2, s2, l2);
}

static inline bool ends_with(const std::string &s1, const std::string &s2)
{
	return ends_with(s1.c_str(), s2.c_str());
}

#endif
