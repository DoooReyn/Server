#include "csv_parse.h"
#include "logger.h"

void TestCSVParse()
{
	CSVParser file = CSVParser("files/simple.csv");

	int32 rownow = file.RowCount();

	for(int32 i = 0; i < rownow; i++)
	{
		CSVRow& Row = file[i];
		cout << Row << endl;
		int32 year = Row["Year"];
		string make = Row["Make"];
		string model = Row["Model"];
		cout << year << " " << make << " " << model <<  endl;
		int32 year2 = Row[0];
		string make2 = Row[1];
		string model2 = Row[2];
		cout << year2 << " " << make2 << " " << model2 <<  endl;
	}

	//std::cout << file[0][0] << std::endl; // display : 1997
	//std::cout << file[0] << std::endl; // display : 1997 | Ford | E350

	//std::cout << file[1]["Model"] << std::endl; // display : Cougar

	//std::cout << file.RowCount() << std::endl; // display : 2
	//std::cout << file.ColumnCount() << std::endl; // display : 3

	//std::cout << file.GetHeaderElement(2) << std::endl; // display : Model
}

int main(int argc, char const* argv[])
{
	InitLogger("./log/test.log", "debug");
	TestCSVParse();
	return 0;
}
