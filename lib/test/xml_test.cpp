#include "tinyxml2.h"
#include "type_define.h"
using namespace tinyxml2;

void TestXMLParse()
{
	XMLDocument doc;
	doc.LoadFile("../config/test.xml");
	XMLElement* scene = doc.RootElement();
	XMLElement* surface = scene->FirstChildElement("node");
	while (surface)
	{
		XMLElement* surfaceChild = surface->FirstChildElement();
		const char* content;
		const char* name ;
		const XMLAttribute* attributeOfSurface = surface->FirstAttribute();
		cout << attributeOfSurface->Name() << ":" << attributeOfSurface->Value() << endl;
		while (surfaceChild)
		{
			name = surfaceChild->Name();
			content = surfaceChild->GetText();
			surfaceChild = surfaceChild->NextSiblingElement();
			cout << name << ":" << content << endl;
		}
		surface = surface->NextSiblingElement();
	}
}

int main(int argc, char const* argv[])
{
	TestXMLParse();
	return 0;
}
