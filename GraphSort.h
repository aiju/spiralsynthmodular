/*  Graph sort
 *  Copyleft (C) 2002 David Griffiths <dave@pawfal.org>
*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/ 

#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <map>

#ifndef GRAPH_SORT
#define GRAPH_SORT

#define GRAPHSORT_USE_TEST_SORT_BY_DEFAULT

using namespace std;

class GraphSort
{
public:
	#ifdef GRAPHSORT_USE_TEST_SORT_BY_DEFAULT
	GraphSort(bool UseTestSort=true);
	#else
	GraphSort(bool UseTestSort=false);
	#endif
	
	~GraphSort();
	const list<int> &GetSortedList();

	void Sort();
	void TestSort();
	void OrigSort();

	void AddConnection(int SID, bool STerminal, int DID, bool DTerminal);
	void RemoveConnection(int SID, int DID);
	void Clear();
	void Dump();
	
	struct Node
	{
		list<int> Inputs;
		list<int> Outputs;
		bool IsTerminal;
		// temporaries used during sort
		int UnsatisfiedOutputs;
		bool IsCandidate;
		bool IsSorted;
	};
	
private:
	void RecursiveWalk(int node);
	bool m_UseTestSort;

	map<int,Node> m_Graph;
	list<int> m_Sorted;
};

#endif

