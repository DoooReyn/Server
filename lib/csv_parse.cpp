
#include "csv_parse.h"
#include "logger.h"



std::ostream& operator<<(std::ostream& os, const CSVRow& row)
{
	for (uint32 i = 0; i != row.m_values.size(); i++)
	{
		os << row.m_values[i] << " | ";
	}

	return os;
}

std::ofstream& operator<<(std::ofstream& os, const CSVRow& row)
{
	for (uint32 i = 0; i != row.m_values.size(); i++)
	{
		os << row.m_values[i];
		if (i < row.m_values.size() - 1)
		{
			os << ",";
		}
	}
	return os;
}


CSVParser::CSVParser(const string& filename, char sep /*= ','*/)
	: m_sep(sep)
{
	m_file = filename;
	ifstream ifile(m_file.c_str());

	if (ifile.is_open())
	{
		string line;
		while (ifile.good())
		{
			getline(ifile, line);
			if (line.size() > 0)
			{
				m_orgFile.push_back(line);
			}
		}
		ifile.close();

		if (m_orgFile.size() == 0)
		{
			WARN("CSVParser file:%s is empty", m_file.c_str());
			return;
		}
		parseHeader();
		parseContent();

	}
	else
	{
		ERROR("CSVParser open file:%s error", m_file.c_str());
	}
}

CSVParser::~CSVParser()
{
	for (auto it = m_content.begin(); it != m_content.end(); ++it)
	{
		delete *it;
	}
	m_content.clear();
}

CSVRow& CSVParser::GetRow(uint32 row) const
{
	assert(row < m_content.size());
	return *(m_content[row]);
}


uint32 CSVParser::RowCount() const
{
	return m_content.size();
}


uint32 CSVParser::ColumnCount() const
{
	return m_header.size();
}


vector<string> CSVParser::GetHeader() const
{
	return m_header;
}


const string CSVParser::GetHeaderElement(uint32 pos) const
{
	assert(pos < m_header.size());
	return m_header[pos];
}


const std::string& CSVParser::GetFileName() const
{
	return m_file;
}

CSVRow& CSVParser::operator[](uint32 row) const
{
	return CSVParser::GetRow(row);
}

void CSVParser::parseHeader(void)
{
	stringstream ss(m_orgFile[0]);
	string item;
	while (getline(ss, item, m_sep))
	{
		m_header.push_back(item);
	}
}

void CSVParser::parseContent()
{
	vector<string>::iterator it = m_orgFile.begin();
	it++;

	for (; it !=  m_orgFile.end(); ++it)
	{
		bool quoted = false;
		int32 tokenStart = 0;
		uint32 i = 0;
		CSVRow* row = new CSVRow(m_header);

		for (; i < it->length(); i++)
		{
			if (it->at(i) == '"')
			{
				quoted = !quoted;
			}
			else if (it->at(i) == ',' && !quoted)
			{
				row->Push(it->substr(tokenStart, i - tokenStart));
				tokenStart = i + 1;
			}
		}
		row->Push(it->substr(tokenStart, it->length() - tokenStart));
		assert(row->Size() == m_header.size());
		m_content.push_back(row);
	}
}

