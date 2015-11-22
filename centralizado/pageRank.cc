#include <iostream>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* abs */
#include <cmath>        // std::abs
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
	    //cout << "Stay Here " << source << " " << target <<endl;
	    result[source].push_back(target);
	    nodes.insert(source);
	    nodes.insert(target);
	    edges++;
	}
	//cout << "Edges: " << edges << endl;
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
			for(int k: nodes) 
				if (k != n)
					adj.push_back(k);
			g[n]=adj;
		}
	}
}


int main(){
  pair<Graph,NodeSet> g = readGraph("example.txt");
  Graph graph = g.first; 
  NodeSet nodes = g.second;  
  fixgraph(graph,nodes);
  int size_graph = graph.size();
  int size_nodes = nodes.size();  
  pair<AdjMat,Norm> m = toMatrix(g.first,g.second);
  
  double Lp[graph.size()];
  double prInitial[graph.size()];
  //printf("%f\n", d);
  int lpSuma=0;
/*______________________________________________________________________________________*/    
  for(int i=0; i<graph.size(); i++){
  	if(i== 0){ 
  		prInitial[i]=1; 
  	}else{ 
  		prInitial[i]=0; 
  	}
  }
/*______________________________________________________________________________________*/  
 
  for(int i=0; i<graph.size(); i++){
  	for (int j=0; j<nodes.size(); j++){
  		 lpSuma = lpSuma + m.first[i][j];
  		 //cout << m.first[i][j];
  	}
  	cout <<"\n";
  	Lp[i] = lpSuma;	
  	lpSuma = 0;
  }
/*
  for(int x=0; x<graph.size(); x++){
  	cout <<"Lp[" <<x<<"] " << Lp[x] <<endl;
  }

  for(int x=0; x<graph.size(); x++){
  	cout <<"prInitial[" <<x<<"] " << prInitial[x] <<endl;
  }  
*/

/*______________________________________________________________________________________*/  

double suma_interna=0.0;
double PrNew[graph.size()];
double d = 0.05;  
int ite=1;
double delta = 0.0001;
double converged; 

	do{

	  cout << "--------Iteracion # " << ite <<endl;	

	  for(int j=0; j<graph.size(); j++){
	  	for (int i=0; i<nodes.size(); i++){  		
	  		suma_interna = suma_interna + ((prInitial[i] / Lp[i]) * m.first[i][j]);  		  		
	  	}  		 	  	
	  	//cout <<"suma [" << j+1 << "]: "<< suma_interna <<endl;
	  	d = d + 0.85 * suma_interna;  	
	  	cout <<"nuevo pr[" << j+1 << "]: "<< d <<endl;
	  	PrNew[j] = d;	  	
	  	suma_interna = 0.0;
	  	d = 0.05;
	  } 	  	  

	  for(int x=0; x<graph.size(); x++){
  		//cout <<"pr_inicial[" <<x<<"] " << prInitial[x] <<endl;
  		converged = (abs(PrNew[x] - prInitial[x])) + converged;
  	  }
	  
	  cout << "converged " << converged <<endl;

	  for(int x=0; x<graph.size(); x++){
  		//cout <<"pr_nuevo[" <<x<<"] " << PrNew[x] <<endl;
  		prInitial[x] = PrNew[x];
  	  }	  

	  ite++;
	}while(delta < converged);

  return 0;
}