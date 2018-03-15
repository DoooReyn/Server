#ifndef __XML_PARSE_H__
#define __XML_PARSE_H__

#include "type_define.h"

class XMLParse
{
public:
	explicit XMLParse(const string& filename);
	~XMLParse();
	string GetNode(const string& node, const string& name);

private:
	void parseFile();

	string m_strFile;
	std::map<string, string> m_mapConfig;

};


#endif
