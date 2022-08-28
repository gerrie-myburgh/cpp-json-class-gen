#ifndef GEN_CPP_SOURCE_HPP_
#define GEN_CPP_SOURCE_HPP_

#include <boost/json.hpp>
#include <string>
#include "utils.hpp"

using namespace boost;
using namespace std; 

void get_object_cpp(const json::object obj, string name, ofstream &out);

#endif