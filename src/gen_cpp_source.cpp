#include "gen_cpp_source.hpp"
#include <boost/format.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <stdlib.h>

#include "utils.hpp"

using namespace boost;
using namespace std;
/**
 * @brief Get the Types object for an array of objects
 *
 * @param objName
 * @param array
 * @return string
 */
void get_cpp_types(string objName, json::array array)
{
    int counter = 0;
    for (auto val : array)
    {
        if (val.is_object())
        {

            char buffer[10];
            sprintf(buffer, "%d", counter);

            ofstream out;
            string fileName = cppDir + "/" + mainName + "_" + objName + buffer + ".cpp";
            out.open(fileName, ofstream::out);
            get_object_cpp(val.as_object(), objName + buffer, out);
            counter++;
            out.close();
        }
    }
}
/**
 * @brief Go through the list and generate the cpp include headers
 *
 * @param obj
 * @param out
 */
string get_cpp_headers(const json::object obj)
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
string get_obj_path(json::object obj)
{
    string result = "";
    auto count = 0;
    for (auto o : obj)
    {
        auto key = o.key();
        auto value = o.value();

        result += str(format("%2%element.find_pointer(\"/%1%\", ec)!=nullptr") % key % (count > 0 ? " && " : ""));
        count++;
    }
    return result;
}
/**
 * @brief Get the json write value object
 *
 * @param value
 * @param name
 * @param mainName
 * @return string
 */
string get_json_write_value(const json::value value, string name, string mainName)
{
    string result = "";
    int max_counter = value.as_array().size();
    for (int counter = 0; counter < max_counter; counter++)
    {
        json::value val = value.at(counter);

        if (val.is_int64())

            result += str(format("%1%if(holds_alternative<long>(element))\n"
                                 "%1%{\n"
                                 "%1%    val[\"%2%\"].as_array().push_back( get<long>(element) );\n"
                                 "%1%}\n") %
                          indent(3) % name);
        if (val.is_uint64())
            result += str(format("%1%if(holds_alternative<unsigned long>(element))\n"
                                 "%1%{\n"
                                 "%1%    val[\"%2%\"].as_array().push_back( get<unsigned long>(element) );\n"
                                 "%1%}\n") %
                          indent(3) % name);
        else if (val.is_string())
            result += str(format("%1%if(holds_alternative<json::string>(element))\n"
                                 "%1%{\n"
                                 "%1%    val[\"%2%\"].as_array().push_back( get<json::string>(element) );\n"
                                 "%1%}\n") %
                          indent(3) % name);
        else if (val.is_double())
            result += str(format("%1%if(holds_alternative<double>(element))\n"
                                 "%1%{\n"
                                 "%1%    val[\"%2%\"].as_array().push_back( get<double>(element) );\n"
                                 "%1%}\n") %
                          indent(3) % name);
        else if (val.is_bool())
            result += str(format("%1%if(holds_alternative<bool>(element))\n"
                                 "%1%{\n"
                                 "%1%    val[\"%2%\"].as_array().push_back( get<bool>(element) );\n"
                                 "%1%}\n") %
                          indent(3) % name);
        else if (val.is_object())
        {
            result +=
                str(format(
                        "%1%if(holds_alternative<%4%_%3%%5%>(element))\n"
                        "%1%{\n"
                        "%1%    val[\"%3%\"].as_array().push_back( (get<%4%_%3%%5%>(element)).get_json() );\n"
                        "%1%}\n") %
                    indent(3) % "key" % name % mainName % counter);
        }
    }
    return result;
}
/**
 * @brief Generate cpp get Json method
 *
 * @param obj
 * @param name
 * @param out
 */
string get_json_cpp(const json::object obj, string name)
{
    string result = "";

    result += str(format("%1%json::object %4%_%3%::get_json(){\n") %
                  indent(1) % indent(2) % name % mainName);

    for (auto it : obj)
    {
        string key = it.key();
        json::value val = it.value();

        if (val.is_string())
        {
            result += str(format("%1%val[\"%2%\"] = %2%;\n") % indent(2) % key);
        }
        else if (val.is_int64() || val.is_bool() || val.is_double() || val.is_number() || val.is_uint64() || val.is_bool())
        {
            result += str(format("%1%val[\"%2%\"] = %2%;\n") % indent(2) % key);
        }
        else if (val.is_object())
        {
            result += str(format(
                              "%1%if (%2%.has_value()) val[\"%2%\"] = %2%.value().get_json();\n") %
                          indent(2) % key);
        }
        else if (val.is_array())
        {
            result += str(format(
                              "%1%if(%3%.size() > 0) val[\"%3%\"] = json::array();\n"
                              "%1%for(auto element : %3%){\n"
                              "%2%"
                              "%1%}\n") %
                          indent(2) % get_json_write_value(val, key, mainName) % key);
        }
    }
    result += str(format("%1%return val;\n") % indent(2));
    result += str(format("%1%}\n") % indent(1));

    return result;
}
/**
 * @brief the set of path kept - used to prevet duplicate paths from traversed
 *
 */
set<string> objectPath;
/**
 * @brief Get the value object
 *
 * @param v
 * @param name
 * @param mainName
 * @param count
 * @return string
 */
string get_value(json::value v, string name, string mainName, int count)
{
    string result = "";
    if (v.is_int64())
        result += str(format("%1%        if(element.is_int64())\n"
                             "%1%        {\n"
                             "%1%            %2%_variant val = (long)element.as_int64();\n"
                             "%1%            %2%.push_back(val);\n"
                             "%1%        }\n") %
                      indent(2) % name);
    else if (v.is_uint64())
        result += str(format("%1%        if(element.is_int64())\n"
                             "%1%        {\n"
                             "%1%            %2%_variant val = (unsigned long)element.as_int64();\n"
                             "%1%            %2%.push_back(val);\n"
                             "%1%        }\n") %
                      indent(2) % name);
    else if (v.is_string())
        result +=
            str(
                format(
                    "%1%        if(element.is_string())\n"
                    "%1%        {\n"
                    "%1%            %2%_variant val = (json::string)element.as_string();\n"
                    "%1%            %2%.push_back(val);\n"
                    "%1%        }\n") %
                indent(2) % name);
    else if (v.is_double())
        result +=
            str(
                format(
                    "%1%        if(element.is_double())\n"
                    "%1%        {\n"
                    "%1%            %2%_variant val = (double)element.as_double();\n"
                    "%1%            %2%.push_back(val);\n"
                    "%1%        }\n") %
                indent(2) % name);
    else if (v.is_bool())
        result += str(format("%1%        if(element.is_bool())\n"
                             "%1%        {\n"
                             "%1%            %2%_variant val = (bool)element.as_bool();\n"
                             "%1%            %2%.push_back(val);\n"
                             "%1%        }\n") %
                      indent(2) % name);
    else if (v.is_object())
    {
        auto obj = v.as_object();
        for_each(obj.begin(), obj.end(), [name, mainName, count, &obj, &result](auto o)
                 {
            auto key = o.key();
            auto value = o.value();
            auto pathValue = get_obj_path(obj);
            if(!objectPath.contains(pathValue))
            {
                objectPath.insert(pathValue);
                result +=
                    str(
                        format(
                            "%1%        if(%6%)\n"
                            "%1%        {\n"
                            "%1%            %3%_variant val = %4%_%3%%5%(element);\n"
                            "%1%            %3%.push_back(val);\n"
                            "%1%        }\n") %
                            indent(2) % key % name % mainName % count % pathValue); 
            } });
    }
    objectPath.clear();
    return result;
}
/**
 * @brief Get the array values object
 *
 * @param value
 * @param name
 * @param mainName
 * @return string
 */
string get_array_values(json::value value, string name, string mainName)
{
    auto val = value.as_array();

    string result = "";
    int max_count = val.size();
    for (int count = 0; count < max_count; count++)
    {
        auto v = value.at(count);
        result += get_value(v, name, mainName, count);
    }
    return result;
}
string get_destructor_cpp(string name)
{
    return str(format("%1%%3%_%2%::~%3%_%2%(){}\n") % indent(1) % name % mainName);
}
/**
 * @brief Generate cpp constructor method
 *
 * @param obj
 * @param name
 * @param out
 */
string get_constructor_cpp(const json::object obj, string name)
{
    string result = "";
    result += str(format("%1%%4%_%2%::%4%_%2%(json::value val){\n"
                         "%3%json::error_code ec;\n") %
                  indent(1) % name % indent(2) % mainName);
    json::error_code ec;
    for (auto it : obj)
    {
        auto key = it.key();
        auto val = it.value();

        if (val.is_string())
        {
            result += str(format("%1%%2% = val.at(\"%2%\").as_string();\n") % indent(2) % key);
        }
        else if (val.is_int64())
        {
            result += str(format("%1%%2% = val.at(\"%2%\").as_int64();\n") % indent(2) % key);
        }
        else if (val.is_uint64())
        {
            result += str(format("%1%%2% = val.at(\"%2%\").as_uint64();\n") % indent(2) % key);
        }
        else if (val.is_double())
        {
            result += str(format("%1%%2% = val.at(\"%2%\").as_double();\n") % indent(2) % key);
        }
        else if (val.is_bool())
        {
            result += str(format("%1%%2% = val.at(\"%2%\").as_bool();\n") % indent(2) % key);
        }
        else if (val.is_object())
        {
            result += str(format(
                              "%1%if (val.find_pointer(\"/%2%\", ec)!=nullptr) %2% = %3%_%2%(val.at_pointer(\"/%2%\")); else %2% = {};\n") %
                          indent(2) % key % mainName);
        }
        else if (val.is_array() && val.as_array().size() > 0)
        {
            get_cpp_types(key, val.as_array());
            result += str(format("%1%if (val.find_pointer(\"/%3%\", ec)!=nullptr)/*  %4%  */\n"
                                 "%1%{\n"
                                 "%1%    for(auto element : val.find_pointer(\"/%3%\", ec)->as_array())\n"
                                 "%1%    {\n"
                                 "%2%    "
                                 "%1%}\n"
                                 "%1%}\n") %
                          indent(2) % get_array_values(val, key, mainName) % key % val);
        }
    }
    result += str(format("%1%}\n") % indent(1));
    return result;
}
/**
 * @brief Generate the cpp file with
 *
 * @param obj
 * @param name
 * @param out
 */
void get_object_cpp(const json::object obj, string name, ofstream &out)
{
    out << format("#include <boost/json.hpp>\n"
                  "#include <optional>\n"
                  "#include \"%2%_%1%.hpp\"\n"
                  "%3%\n"
                  "using namespace std;\n"
                  "using namespace boost;\n\n"
                  "namespace %2% {\n") %
               name % mainName % get_cpp_headers(obj);

    out << get_constructor_cpp(obj, name);
    out << get_destructor_cpp(name);
    out << get_json_cpp(obj, name);

    out << format("}\n");

    for (auto it : obj)
    {
        string key = it.key();
        json::value val = it.value();
        if (val.is_object())
        {
            ofstream out;
            string fileName = cppDir + "/" + mainName + "_" + key + ".cpp";
            out.open(fileName, ofstream::out);
            get_object_cpp(val.as_object(), key, out);
            out.close();
        }
    }
}
