//Carlos Andres Martinez - {Client - workers - recollector }
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <zmqpp/zmqpp.hpp>
#include <cmath>        // abs
#include <fstream>
#include <unordered_map>
#include <sstream>

using namespace std;
using namespace zmqpp;

using Graph = unordered_map<int,vector<int>>;
using NodeSet = unordered_set<int>; 
using AdjMat = vector<vector<int>>;
using Norm = unordered_map<int,int>;

/*____________________________________________________________________________*/
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
/*____________________________________________________________________________*/
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
/*____________________________________________________________________________*/
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
/*_____________________________pagerank initial_________________________________________*/ 
vector<double> PR(int size_g){

  vector<double> prInitial(size_g);
  for(int i=0; i<size_g; i++){
  	if(i== 0){ 
  		prInitial[i]=1; 
  	}else{ 
  		prInitial[i]=0; 
  	}
  }
  return prInitial;	
}
/*____________________________________________________________________________*/

int main(int argc, char **argv){                     // .client 192.168.1.12 filename
  string ip, filename; 		// 10.253.96.236 U, 192.168.1.12 CASA
  ip = argv[1];
  filename = argv[2];
  cout<<"running client, connect at ip " << ip << endl;
/*____________________________________________________________________________*/
  pair<Graph,NodeSet> g = readGraph(filename);
  Graph graph = g.first; 
  NodeSet nodes = g.second;  

  fixgraph(graph,nodes);
  int size_g = graph.size();
  int size_n = nodes.size();  
  double N = size_g; 
  cout << "N = " << N <<endl; 
  pair<AdjMat,Norm> m = toMatrix(g.first,g.second);
  
  double Lp[size_g];
  vector<double>  prInitial(size_g);

  prInitial = PR(size_g);
  double aux_prInitial[size_g];
  int lpSuma=0;
/*______________________________L(p)____________________________________________________*/  
 
  for(int i=0; i<size_g; i++){
  	for (int j=0; j<size_n; j++){
  		 lpSuma = lpSuma + m.first[i][j];
  	}
  	if(lpSuma == 0){ 
  		Lp[i] = 1; 
  	}else{
  		Lp[i] = lpSuma;	
  		lpSuma = 0;
  	}
  }

/*______________________________________________________________________________________*/    
/*______________________________________________________________________________________*/  

double suma_interna=0.0;
double converged = 0.0;
double delta = 0.0001;
int ite = 1;
/*_______________________________________________________________________________*/  	  
  context ctx;
  socket wx(ctx, socket_type::xreq);  // socket (wx) cliente - recollector (6666)
  wx.connect("tcp://"+ip+":6666");

  socket rc(ctx, socket_type::xrep); // socket recollector to client
  rc.bind("tcp://*:6667");
/*_________________________________________________________________________*/
  	message cworkers;                 // mensaje cworkers
  	cworkers << N;
	cworkers << size_g;
	string msg;
	do{		
		suma_interna = 0.0;
		for(int j=0; j<size_g; j++){
			for (int i=0; i<size_n; i++){  		
		  		suma_interna = suma_interna + ( (prInitial[i] / Lp[i]) * m.first[i][j] );  		  		
		  	}  		 	  	
		  	cout << suma_interna <<endl; 
		  	cworkers << suma_interna;
		} 	 		
		wx.send(cworkers);		
/*_________________________________________________________________________*/
		message rclient;
		rc.receive(rclient);
		string idr, idc;
		rclient >> idr >> idc;
	      
	    for (int i=0; i< 3; i++){
	       rclient >> aux_prInitial[i];
	    }			
	    for (int i=0; i< 3; i++){
	       converged = (abs(aux_prInitial[i] - prInitial[i])) + converged;
	    }  
	    for (int i=0; i< 3; i++){
	       prInitial[i] = aux_prInitial[i];
	    }	
	    cout << " Iteration # " << ite << " converged " << converged <<endl;
	    ite++;
	}while(delta < converged);

  return 0;
}
