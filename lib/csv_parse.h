#ifndef __CAV_PARSE_H__
#define __CAV_PARSE_H__

#include "type_define.h"
#include <fstream>
#include "var_type.h"
static VarType VAR_NULL;
class CSVRow
{
public:
	CSVRow(const vector<string>& header): m_header(header)
	{

	}
	~CSVRow()
	{
	}

	uint32 Size() const
	{
		return m_values.size();
	}
	void Push(const string& value)
	{
		m_values.push_back(value);
	}

	VarType operator[](uint32 pos) const
	{
		assert(pos < m_values.size());
		VarType res;
		res.put(m_values[pos]);
		return res;
	}

	VarType operator[](const std::string& key) const
	{
		uint32 pos = 0;
		for (auto it = m_header.begin(); it != m_header.end(); ++it)
		{
			if (key == *it)
			{
				assert(pos < m_values.size());
				VarType res;
				res.put(m_values[pos]);
				return res;
			}
			pos++;
		}
		return VAR_NULL;
	}


	friend std::ostream& operator<<(std::ostream& os, const CSVRow& row);
	friend std::ofstream& operator<<(std::ofstream& os, const CSVRow& row);

private:
	const vector<string> m_header;
	vector<string> m_values;
};



class CSVParser
{
public:
	CSVParser(const string& filename, char sep = ',');
	~CSVParser();

	CSVRow& GetRow(uint32 row) const;
	uint32 RowCount() const;
	uint32 ColumnCount() const;
	vector<string> GetHeader() const;
	const string GetHeaderElement(uint32 pos) const;
	const string& GetFileName() const;
	CSVRow& operator[](uint32 row) const;

private:
	void parseHeader();
	void parseContent();


private:
	string m_file;
	const char m_sep = ',';
	vector<string>  m_orgFile;
	vector<string>  m_header;
	vector<CSVRow*> m_content;

};

#endif