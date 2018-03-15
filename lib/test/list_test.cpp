#include "timer.h"
#include "helper.h"
#include <list>

struct RankNode
{
	int32 roleid = 0;
	int32 rank = 0;

	bool operator< (const RankNode& rhs)
	{
		return rank < rhs.rank;
	}
};


void ListInsert(list<RankNode>& rankList, RankNode& node)
{
	if (rankList.size() < 50)
	{
		rankList.push_back(node);
	}
	else
	{
		RankNode& temp = rankList.back();
		if (node.rank < temp.rank)
		{
			temp = node;
		}
	}
	rankList.sort();
}


void ListInsert2(list<RankNode>& rankList, RankNode& node)
{
	for (list<RankNode>::iterator it = rankList.begin(); it != rankList.end(); ++it)
	{
		if ((*it).rank > node.rank)
		{
			rankList.insert(it, node);
			break;
		}
	}
	if (rankList.size() > 50)
	{
		rankList.pop_back();
	}
}



int main(int argc, char const* argv[])
{
	list<RankNode> listRank;

	std::vector<int32> rands;

	double totol = 0;
	for(int32 i = 0; i < 10000; i++)
	{
		auto start = Timestamp::Now().GetMicroSeconds();
		rands.clear();
		listRank.clear();
		RandNumInRegion(60, 30, rands);

		for (auto it : rands)
		{
			RankNode node;
			node.roleid = it;
			node.rank = it;
			listRank.push_back(node);
		}
		listRank.sort();


		rands.clear();
		RandNumInRegion(50, 50, rands);


		for (auto it : rands)
		{
			RankNode node;
			node.roleid = it;
			node.rank = it;
			//ListInsert(listRank, node);
			ListInsert2(listRank, node);
		}
		auto end = Timestamp::Now().GetMicroSeconds();

		totol += end - start;
	}

	for (auto it : listRank)
	{
		cout << "roleid:" << it.roleid << " rank:" << it.rank << endl;
	}
	//cout << "size:" << listRank.size() << endl;
	cout << "size:" << listRank.size() << " average:" << totol / 10000 << endl;
	cout << "======================================" << endl;



	return 0;
}
