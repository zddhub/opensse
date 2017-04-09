
#include "json_parser.h"

void parse(const string &jsonContent, map<string, string> &json)
{
	string keys_stack;
	string stack;
	stack.push_back('#');

	int i = 0;
	while(i < jsonContent.size())
	{
		char curChar = jsonContent[i];
		string value;
		switch (curChar) {
			case '"':
				while(jsonContent[++i] != '"' && i < jsonContent.size())
					keys_stack.push_back(jsonContent[i]);
				break;
			case ':':
				keys_stack.push_back('$');
				stack.push_back(curChar);
				break;
			case '}':
				while(stack.back() != '{')
					stack.pop_back();
				stack.pop_back();
				if (stack.back() == '#')
					break;
			case ',':
				if (keys_stack.back() != '$') {
					while(keys_stack.back() != '$') {
						value.insert(0, 1, keys_stack.back());
						keys_stack.pop_back();
					}
					keys_stack.pop_back();
					stack.pop_back();

					json.insert(make_pair(keys_stack, value));

					while(!keys_stack.empty() && keys_stack.back() != '$') {
						keys_stack.pop_back();
					}
				} else {
					keys_stack.pop_back();
					while(!keys_stack.empty() && keys_stack.back() != '$')
						keys_stack.pop_back();
				}
				break;
			default:
				stack.push_back(curChar);
		}
		i++;
	}
}

Json::Json(const string &filename)
{
	ifstream in(filename.c_str());

	string jsonContent;
    char c;
	while(in.get(c))
		if(c != ' ' && c != '\r' && c != '\n' && c != '\t')
			jsonContent.push_back(c);
    in.close();

    parse(jsonContent, values);
}

const string& Json::getValue(const string &key, const string &defaultValue)
{
	return values[key].empty() ? defaultValue : values[key];
}
