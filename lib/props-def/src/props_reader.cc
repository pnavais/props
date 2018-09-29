#include "props_reader.h"
#include <fstream>
#include <sstream>
#include <pcrecpp.h>  

string PropsReader::find_value(const string& key) {
	std::cout << "Finding [" << key << "]" << std::endl;
	std::ifstream infile("/Projects/props/test.cfg");

	// Prepare regex
    std::stringstream regex;
	regex << "^" << key << "(.+)";
    string key_r;
    string value_r;
    pcrecpp::RE re(regex.str());
    pcrecpp::RE comm_re("^[\\s]*#");

	string line;
    while (std::getline(infile, line))
	{
		std::cout << "Line [" << line <<"]" << std::endl;

		// Try to find the regex in line, and report results.
		if (!comm_re.PartialMatch(line)) {
			re.FullMatch(line, &key_r, &value_r);
			std::cout << "VALUE => [" << value_r << "]" << endl;
		} else {
			std::cout << "Skipping commented line " << line << std::endl;
		}

	}

	return "";
}
