#include <iostream>
#include <string>
#include <cassert>
#include <zmqpp/zmqpp.hpp>
#include <fstream>
#include <unordered_map>
#include <sstream>

using namespace std;
using namespace zmqpp;

using Graph = unordered_map<int,vector<int>>;
using NodeSet = unordered_set<int>; 
using AdjMat = vector<vector<int>>;
using Norm = unordered_map<int,int>;

pair<Graph,NodeSet> readGraph(const string& filename) {
	Graph result;
	NodeSet nodes;
	ifstream is(filename);
	string line;
	int edges = 0;
	while(getline(is,line)) {
		if(line[0]=='#') continue;
	    stringstream ss(line);
	    int source;
	    int target;
	    ss >> source;
	    ss >> target;
	    result[source].push_back(target);
	    nodes.insert(source);
	    nodes.insert(target);
	    edges++;
	}
	cout << "Edges: " << edges << endl;
	return {result,nodes};
}

pair<AdjMat,Norm> toMatrix(const Graph& g, const NodeSet& nodes) { // pair
	Norm norm;
	int i = 0;
	for (int n : nodes) {
		norm[n] = i;
		i++;
	}
	
	AdjMat mat(nodes.size(),vector<int>(nodes.size(),0));
	for(const auto& adj : g) {
		int src = adj.first;
		for(const int& tgt : adj.second){
			//cout << src << ".." << tgt << endl;
			int nsrc = norm[src];
			int ntgt = norm[tgt];
			mat[nsrc][ntgt] = 1;
		}
	}
	return {mat,norm};
}

void fixgraph (Graph& g, NodeSet nodes){
	for (int n: nodes)
	{
		if (g.count(n)==0)
		{
			vector <int>adj;
			for(int k:nodes) 
				if (k!=n)
					adj.push_back(k);
			g[n]=adj;
		}
	}
}

int main(){
  pair<Graph,NodeSet> g = readGraph("wikivote.txt");
  Graph graph = g.first; 
  NodeSet nodes = g.second;  
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  fixgraph(graph,nodes);
  cout << graph.size() << endl;
  cout << nodes.size() << endl;
  
  pair<AdjMat,Norm> m = toMatrix(g.first,g.second);
  int i = m.second[3];
  int j = m.second[604];
  cout << m.first[i][j] << endl;
  return 0;
}