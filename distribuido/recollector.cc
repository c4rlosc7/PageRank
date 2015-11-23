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

int main()
{
  cout << "Running recollector ";
  context ctx;
  socket wr(ctx, socket_type::xrep);         // socket workers-recollector
  wr.bind("tcp://*:6667");

  string idc, idw, ipc, msg_receive;        // id
	while(true)
	{
			message wrecollector;
			wr.receive(wrecollector);
			cout << " recibe " << wrecollector.parts() << " partes " << endl;
			for(size_t i = 0; i < wrecollector.parts(); i++) {
				cout << wrecollector.get(i) << endl;
			}
			wrecollector >> idw >> idc;
			//cout <<"idworkers "<<idw<<" idcliente "<<idc<<endl;
			wrecollector >> msg_receive >> ipc;
			//cout <<" message " << msg_receive <<" ipcliente "<<ipc<<endl;
      
      socket rc(ctx, socket_type::xreq);         // socket recollector to client
      rc.connect("tcp://"+ipc+":6666");

      message rclient;
      rclient << idc;
      rclient << msg_receive;
      cout << "envio" << rclient.parts() << "partes" << endl;
      for(size_t i = 0; i < wrecollector.parts(); i++) {
        cout << wrecollector.get(i) << endl;
      }      
      rc.send(rclient);

  	}
	return 0;
}
