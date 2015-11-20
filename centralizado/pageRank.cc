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
  pair<Graph,NodeSet> g = readGraph("example.txt");
  Graph graph = g.first; 
  NodeSet nodes = g.second;  
  cout << "nodos relacionados " << graph.size() << endl;
  cout << "numero de nodos " << nodes.size() << endl;
  fixgraph(graph,nodes);
  //cout << graph.size() << endl;
  //cout << nodes.size() << endl;
  
  pair<AdjMat,Norm> m = toMatrix(g.first,g.second);
  int sum=0, ii=0;
  double Lp[graph.size()];
  double PrNew[graph.size()];
  double d = 0.05;
  double prI[graph.size()];
  printf("%f\n", d);
/*______________________________________________________________________________________*/    
  for(int i=0; i<graph.size(); i++){
  	if(i== 2){ prI[i]=1; }else{ prI[i]=0; }
  }
/*______________________________________________________________________________________*/  
  for(int i=0; i<graph.size(); i++){
  	for (int j=0; j<nodes.size(); j++){
  		 sum = sum + m.first[i][j];
  		 cout << m.first[i][j];
  	}
  	cout <<"\n";
  	Lp[ii] = sum;	
  	sum=0;
  	ii++;
  }

  for(int x=0; x<graph.size(); x++){
  	cout << Lp[x] <<endl;
  }
/*______________________________________________________________________________________*/  
double aux;
int ite=0;

do{

  for(int j=0; j<graph.size(); j++){
  	for (int i=0; i<nodes.size(); i++){
  		aux = (prI[i] / Lp[i]) * m.first[i][j];  	
  		//printf("aux %f\n", aux);  		
  	}  		 
  	d = d + 0.85 * aux;
  	PrNew[j]=d;
  	d = 0.05;
  	cout << PrNew[j] << endl;
  	//prI[j] = PrNew[j];
  	aux = 0.0;
  }
  for (int j = 0; j < 3; j++) { prI[j] = PrNew[j]; }  
  ite++;
}while(ite < 2);

  return 0;
}