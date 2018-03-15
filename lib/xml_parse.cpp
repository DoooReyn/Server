#include "xml_parse.h"
#include "tinyxml2.h"
using namespace tinyxml2;

XMLParse::XMLParse(const string& filename) : m_strFile(filename)
{
	parseFile();
}

XMLParse::~XMLParse()
{
	m_mapConfig.clear();
}

void XMLParse::parseFile()
{
	XMLDocument doc;
	XMLError xml_ret = doc.LoadFile(m_strFile.c_str());
	assert(XML_SUCCESS == xml_ret);

	XMLElement* rootNode = doc.FirstChildElement();
	assert(rootNode != NULL);
	XMLElement* node = rootNode->FirstChildElement();
	while (node)
	{
		XMLElement* subNode = node->FirstChildElement();
		assert(subNode != NULL);
		string strName(node->Name());

		while (subNode)
		{
			m_mapConfig[strName + subNode->Name()] = subNode->GetText();
			subNode = subNode->NextSiblingElement();
		}
		node = node->NextSiblingElement();
	}

	//for (std::map<string, string>::iterator it = m_mapConfig.begin(); it != m_mapConfig.end(); ++it)
	//{
	//	printf("%s -> %s \n", it->first.c_str(), it->second.c_str());
	//}

}

string XMLParse::GetNode(const string& node, const string& name)
{
	return m_mapConfig[node + name];
}
