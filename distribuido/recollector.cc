//Carlos Andres Martinez - {Client - workers - recollector }
#include <iostream>  // std::cout
#include <fstream>  //std::ifstream
#include <string>
#include <list>
#include <cassert>
#include <map>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

int main(int argc, char **argv)
{
  string ip;                        // 10.253.96.236 U, 192.168.1.12 CASA
  ip = argv[1];
  cout<<"running recollector, connect at ip " << ip << endl;
  
  context ctx;
  socket wr(ctx, socket_type::xrep);         // socket client-recollector
  wr.bind("tcp://*:6666");
  socket rc(ctx, socket_type::xreq);         // socket recollector to client
  rc.connect("tcp://"+ip+":6667");

  string idc;
  double suma_interna[3];                  // id, message
  double d = 0.85;
  double N; 
  int size_g;

	while(true)
	{
			message crecollector;
			wr.receive(crecollector);

      crecollector >> idc;      
      crecollector >> N;
      crecollector >> size_g;      
      cout << "N= " << N << " Size " <<size_g<<endl;

			cout << " recibe " << crecollector.parts() << " partes " << endl;
      for (int i=0; i< 3; i++){
        crecollector >> suma_interna[i];        
      }			
      for (int i=0; i< 3; i++){
        cout << suma_interna[i] <<endl;
      }            
      double PrNew[size_g];      

      for(int p=0; p<size_g; p++){
        PrNew[p] = ((1 - d) / N) + (d * suma_interna[p]);
      }      
/*______________________________________________________________________________________*/  
      message rclient;
      rclient << idc;

      for (int i=0; i<3; i++){
        rclient << PrNew[i];
        cout << PrNew[i]<<endl;
      }
      rc.send(rclient);
  	}
	return 0;
}
