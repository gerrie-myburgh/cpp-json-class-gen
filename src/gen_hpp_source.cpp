#include <boost/format.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <stdlib.h>

#include "gen_hpp_source.hpp"
#include "utils.hpp"

/**
 * @brief Go through the list and if the content are
 *
 * @param obj
 * @param out
 */
string get_headers(const json::object obj)
{
    string result = "";
    for (auto it : obj)
    {
        string key = it.key();
        json::value val = it.value();

        if (val.is_object())
        {
            result += str(
                format("#include \"%1%_%2%.hpp\"\n") % mainName % key);
        }
        else if (val.is_array())
        {
            int counter = 0;
            for (auto val : val.as_array())
            {
                if (val.is_object())
                {
                    result += str(
                        format("#include \"%1%_%2%%3%.hpp\"\n") % mainName % key % counter);
                    counter++;
                }
            }
        }
    }
    return result;
}
/**
 * @brief Print the hpp prototype methods
 *
 * @param name
 * @param out
 */
string get_methods_hpp(string name)
{
    string result = "";
    result += str(format(
        "%1%%3%_%2%(json::value val);\n"
        "%1%json::object get_json();\n"
        "%1%~%3%_%2%();\n") % indent(2) % name % mainName);
    return result;
}
/**
 * @brief Get the Types object for an array of objects
 *
 * @param objName
 * @param array
 * @return string
 */
string get_hpp_types(string objName, json::array array)
{
    string result = "";
    int counter = 0;
    for (auto val : array)
    {
        if (result.length() > 0)
            result += ",";
        if (val.is_bool())
        {
            result += "bool";
        }
        else if (val.is_double())
        {
            result += "double";
        }
        else if (val.is_int64())
        {
            result += "long";
        }
        else if (val.is_uint64())
        {
            result += "unsigned long";
        }
        else if (val.is_string())
        {
            result += "json::string";
        }
        else if (val.is_object())
        {

            char buffer[10];
            sprintf(buffer, "%d", counter);

            result += mainName + "_" + objName + buffer;

            ofstream out;
            string fileName = hppDir + "/" + mainName + "_" + objName + buffer + ".hpp";
            out.open(fileName, ofstream::out);
            get_object_hpp(val.as_object(), objName + buffer, out);
            counter++;
        }
    }
    return result;
}
/**
 * @brief Get the attrs hpp object
 * 
 * @param obj 
 * @param out 
 * @return string 
 */
string get_attrs_hpp(const json::object obj)
{
    string result = "";
    for (auto it : obj)
    {
        string key = it.key();
        json::value val = it.value();

        if (val.is_string())
        {
            result += str(format("%1%json::string %2% = \"\";\n") % indent(2) % key);
        }
        else if (val.is_int64())
        {
            result += str(format("%1%long %2% = 0;\n") % indent(2) % key);
        }
        else if (val.is_uint64())
        {
            result += str(format("%1%unsigned long %2% = 0;\n") % indent(2) % key);
        }
        else if (val.is_double())
        {
            result += str(format("%1%double %2% = 0.0;\n") % indent(2) % key);
        }
        else if (val.is_bool())
        {
            result += str(format("%1%bool %2% = false;\n") % indent(2) % key);
        }
        else if (val.is_array() && val.as_array().size() > 0)
        {
            result += str(format("%1%typedef variant<%2%> %3%_variant;\n") % indent(2) % get_hpp_types(key, val.as_array()) % key);
            result += str(format("%1%typedef vector<%2%_variant> %2%_vector;\n") % indent(2) % key);
            result += str(format("%1%%2%_vector %2%;\n") % indent(2) % key);
        }
        else if (val.is_object())
        {
            result += str(format("%1%optional<%2%_%3%> %3% = {};\n") % indent(2) % mainName % key); 

            ofstream out;
            string fileName = hppDir + "/" + mainName + "_" + key + ".hpp";
            out.open(fileName, ofstream::out);
            get_object_hpp(val.as_object(), key, out);
            out.close();
        }
    }
    return result;
}
/**
 * @brief Print hpp prototype struct
 *
 * @param obj
 * @param name
 * @param out
 */
void get_object_hpp(const json::object obj, string name, ofstream &out)
{
    out << format(
        "#ifndef %3%_%4%_\n"
        "#define %3%_%4%_\n"
        "#include <boost/json.hpp>\n"
        "#include <vector>\n"
        "#include <optional>\n"
        "#include <variant>\n"
        "%2%"
        "using namespace std;\n"
        "using namespace boost;\n"
        "namespace %3% {\n"
        "%1%struct %3%_%4%{\n"
        "%1%    json::object val;\n") % indent(1) % get_headers(obj) % mainName % name;

    out << get_attrs_hpp(obj);
    out << get_methods_hpp(name);

    out << format(
        "%1%};\n"
        "};\n"
        "#endif") % indent(1);

    out.close();
}
