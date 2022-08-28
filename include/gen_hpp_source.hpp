#ifndef GEN_HPP_SOURCE_HPP
#define GEN_HPP_SOURCE_HPP

#include <boost/json.hpp>
#include <string>

using namespace boost;
using namespace std;

extern string cppDir;
extern string hppDir;
extern string mainName;

void get_object_hpp(const json::object obj, string name, ofstream& out);

#endif
