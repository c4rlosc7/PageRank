//Carlos Andres Martinez - {Client - recollector } ./client IP FILENAME
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <zmqpp/zmqpp.hpp>
#include <map>
#include <cmath>        // abs
#include <fstream>
#include <unordered_map>
#include <time.h>
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

  clock_t start = clock();	
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
	string msg;
	do{				
		converged = 0.0;

  		cworkers << N;
		cworkers << size_g;

		for(int j=0; j<size_g; j++){
			for (int i=0; i<size_n; i++){  		
		  		suma_interna = suma_interna + ( (prInitial[i] / Lp[i]) * m.first[i][j] );  		  		
		  	}  		 	  	
		  	//cout << suma_interna <<endl; 
		  	cworkers << suma_interna;
		  	suma_interna = 0.0;
		} 	 		
		wx.send(cworkers);		
/*_________________________________________________________________________*/
		message rclient;
		rc.receive(rclient);
		string idr, idc;
		rclient >> idr >> idc;
	      
	    for (int i=0; i< size_g; i++){
	       rclient >> aux_prInitial[i];
	    }			
	    for (int i=0; i< size_g; i++){
	       converged = (abs(aux_prInitial[i] - prInitial[i])) + converged;
	    }  
	    for (int i=0; i< size_g; i++){
	       prInitial[i] = aux_prInitial[i];
	    }	

	    cout << " Iteration # " << ite << " converged " << converged <<endl;
	    ite++;

	}while(delta < converged);

	double suma_total;
	cout <<"\n";
	for(int x=0; x<size_g; x++){
  		cout << "PR[" << x << "] " << prInitial[x] <<endl;
  		suma_total = suma_total + prInitial[x];
  	}	  
  	cout << "Suma " << suma_total <<endl;

	clock_t end = clock();
	double time = (double) (end-start) / CLOCKS_PER_SEC * 1000.0;
    cout <<"Time Execution: " << time << endl;  	

  return 0;
}
