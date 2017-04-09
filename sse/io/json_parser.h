#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;

enum VALUE_TYPE {
	UINT
};

template <class T>
T convert(const string &value, VALUE_TYPE type)
{
	if (type == UINT) {
		char *end;
		return static_cast<T>(strtol(value.c_str(), &end, 10));
	}
	return T(0);
};

class Json {
public:
	Json(const string &filename);
	const string& getValue(const string &key, const string &defaultValue);
private:
	map<string, string> values;
};

#endif // JSON_PARSER_H