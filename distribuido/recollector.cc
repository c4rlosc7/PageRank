//Carlos Andres Martinez - {Client - recollector } ./recolector IP 
#include <iostream>  
#include <fstream>  
#include <string>
#include <list>
#include <map>
#include <cassert>
#include <map>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

int main(int argc, char **argv)         // 10.253.96.236 U, 192.168.1.12 CASA
{

  string ip;                        
  ip = argv[1];
  cout<<"running recollector, connect at ip " << ip << endl;
  
  context ctx;
  socket wr(ctx, socket_type::xrep);         // socket client-recollector
  wr.bind("tcp://*:6666");
  socket rc(ctx, socket_type::xreq);         // socket recollector-client
  rc.connect("tcp://"+ip+":6667");

  string idc;
  double d = 0.85;
  double N; 
  int size_g=0;
  map<int,double> suma_interna;
  int ite=1;
/*______________________________________________________________________________________*/    
	while(true)
	{      
			message crecollector;
			wr.receive(crecollector);

      crecollector >> idc;      
      crecollector >> N;
      crecollector >> size_g;      

      for (int i=0; i< size_g; i++){
        crecollector >> suma_interna[i];        
      }			
      cout << "ite " << ite <<endl;
      map<int,double> PrNew;      

      for(int p=0; p<size_g; p++){
        PrNew[p] = ((1 - d) / N) + (d * suma_interna[p]);        
      }      
/*______________________________________________________________________________________*/  
      message rclient;
      rclient << idc;

      for (int i=0; i<size_g; i++){
        rclient << PrNew[i];
      }

      ite++;

      rc.send(rclient);
  	}  

	return 0;
}
