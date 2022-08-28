#include "utils.hpp"

#include <iostream>
#include <stdlib.h>

using namespace boost;
using namespace std;

const int indent_depth = 4;
string cppDir;
string hppDir;
string mainName;

/**
 * @brief Read json string from istream
 *
 * @param is
 * @param ec
 * @return json::value
 */
json::value read_json(std::istream &is, json::error_code &ec) {
	json::stream_parser p;
	std::string line;
	while (std::getline(is, line)) {
		p.write(line, ec);
		if (ec.failed())
			return nullptr;
	}
	p.finish(ec);
	if (ec.failed())
		return nullptr;
	return p.release();
}
/**
 * @brief Create indent string
 *
 * @param current_indent
 * @return string
 */
string indent(int current_indent) {
    if (current_indent > 0)
        return string(current_indent * indent_depth, ' ');
    else
        return "";
}
