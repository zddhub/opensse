#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
using namespace std;

class Json {
public:
	Json(const string &filename);
	const string& getValue(const string &key, const string &defaultValue);
private:
	map<string, string> values;
};

#endif // JSON_PARSER_H