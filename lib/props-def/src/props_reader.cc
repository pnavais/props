#include "props_reader.h"
#include <fstream>
#include <sstream>
#include <pcrecpp.h>  

string PropsReader::find_value(const string& key) {
	std::ifstream infile("/Projects/props/test.cfg");

	// Prepare regex
    std::stringstream regex;
	regex << "^" << key << "=(.+)";
    string value_r;
    pcrecpp::RE re(regex.str());
    pcrecpp::RE comm_re("^[\\s]*#");

	string line;
    while (std::getline(infile, line))
	{
		// Try to find the regex in line, and report results.
		if (!comm_re.PartialMatch(line)) {
			if (re.PartialMatch(line, &value_r)) {
				break;
			}
		} else {
			std::cout << "Skipping commented line " << line << std::endl;
		}
	}

	return value_r;
}
