#ifndef UTILS_HPP
#define UTILS_HPP

#include <boost/json.hpp>
#include <string>

using namespace boost;
using namespace std;

extern string cppDir;
extern string hppDir;
extern string mainName;
extern const int indent_depth;

json::value read_json(std::istream& is, json::error_code& ec);
string indent(int current_indent);

#endif
