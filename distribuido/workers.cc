//Carlos Andres Martinez - {Client - workers - recollector }
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <stdio.h>      /* printf, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include <zmqpp/zmqpp.hpp>
#include <unordered_map>
#include <sstream>
#include <cmath>        // abs

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

pair<AdjMat,Norm> toMatrix(const Graph& g, const NodeSet& nodes) { 
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


/*____________________________________________________________________________*/
int main(int argc, char **argv)  // ./workers 192.168.1.12 5557 o 5558 o 5559
{
		string ip = argv[1];
  		cout<<"Running worker " <<ip<<endl;
		/*_____________________________pagerank initial_________________________________________*/    
		  for(int i=0; i<size_g; i++){
		  	if(i== 0){ 
		  		prInitial[i]=1; 
		  	}else{ 
		  		prInitial[i]=0; 
		  	}
		  }
		/*______________________________L(p)______________________________________________________*/  
		 
		  for(int i=0; i<size_g; i++){
		  	for (int j=0; j<size_n; j++){
		  		 lpSuma = lpSuma + m.first[i][j];
		  		 //cout << m.first[i][j];
		  	}
		  	//cout <<"\n";
		  	if(lpSuma == 0){ 
		  		Lp[i] = 1; 
		  	}else{
		  		Lp[i] = lpSuma;	
		  		lpSuma = 0;
		  	}
		  }

		/*______________________________________________________________________________________*/  

		double suma_interna=0.0;
		double PrNew[size_g];
		double d = 0.05;  
		int ite = 1;
		double delta = 0.0001;
		double converged = 0.0;

			do{

			  cout << "--------Iteracion # " << ite <<endl;	
			  converged = 0.0;

			  for(int j=0; j<size_g; j++){
			  	for (int i=0; i<size_n; i++){  		
			  		suma_interna = suma_interna + ( (prInitial[i] / Lp[i]) * m.first[i][j] );  		  		
			  	}  		 	  	
			  	d = d + 0.85 * suma_interna;  	
			  	//cout <<"nuevo pr[" << j+1 << "]: "<< d <<endl;
			  	PrNew[j] = d;	  	
			  	suma_interna = 0.0;
			  	d = 0.05;
			  } 	  	  

			  for(int x=0; x<size_g; x++){
		  		converged = (abs(PrNew[x] - prInitial[x])) + converged;
		  	  }
			  
			  cout << "converged " << converged <<endl;

			  for(int x=0; x<size_g; x++){
		  		prInitial[x] = PrNew[x];
		  	  }	  

			  ite++;
			}while(delta < converged);

		/*________________________________________________________________________*/
		context ctx;
		socket wx(ctx, socket_type::xrep);   // socket client-workers 5557
		wx.bind("tcp://*:5557");

		socket wr(ctx, socket_type::xreq);   // socket worked-recollector 6667
		wr.connect("tcp://"+ip+":6667");

		message cworkers;
  		string idc, ipc, msg_filename;
	  	while(true)
	  	{
				wx.receive(cworkers);                  // receive idc,k,ipc
				
				cout << " recibe " << cworkers.parts() << " partes " << endl;
				for(size_t i = 0; i < cworkers.parts(); i++) {
					cout << cworkers.get(i) << endl;
				}				
				
				cworkers >> idc >> msg_filename >> ipc;

				string filename = argv[1];
				pair<Graph,NodeSet> g = readGraph(msg_filename);

				Graph graph = g.first; 
				NodeSet nodes = g.second;  

				fixgraph(graph,nodes);
				int size_g = graph.size();
				int size_n = nodes.size();  
				pair<AdjMat,Norm> m = toMatrix(g.first,g.second);
				  
				double Lp[graph.size()];
				double prInitial[size_g];
				int lpSuma=0;				
				/*______________________________________________________________________*/
				message wrecollector;               // envia al recolector
				wrecollector << idc << msg_receive << ipc;
				cout << "idcliente " <<idc<<" cluster "<<msg_receive<<" ipcliente "<<ipc<<endl;
				/*______________________________________________________________________*/
				cout << " envio " << wrecollector.parts() << " partes " << endl;
				for(size_t i = 0; i < wrecollector.parts(); i++) {
					cout << wrecollector.get(i) << endl;
				}				
				wr.send(wrecollector);
	  	}
	return 0;
}
