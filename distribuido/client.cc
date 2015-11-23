//Carlos Andres Martinez - {Client - workers - recollector }
#include <iostream>
#include <string>
#include <cassert>
#include <fstream>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace zmqpp;

/*____________________________________________________________________________*/

int main(int argc, char **argv){                     // .client 192.168.1.12 hola
  string ip; 		// 10.253.96.236 U, 192.168.1.12 CASA
  ip = argv[1];
  cout<<"running client, connect at ip " << ip << endl;
/*__________________________________________________________________*/
	context ctx;
  socket wx(ctx, socket_type::xreq);  // socket (wx) cliente - workers (5557)
  wx.connect("tcp://"+ip+":5557");

  socket rc(ctx, socket_type::xrep); // socket recollector to client
  rc.bind("tcp://*:6666");

  string ipc = "192.168.1.12";
  message cworkers;                 // mensaje cworkers

	string msg;
	while(true)
	{		
		cout << "Enter filename : ";
		cin >> msg;
		cworkers << msg << ipc;
		wx.send(cworkers);		
/*_________________________________________________________________________*/
		message rclient;
		rc.receive(rclient);
		string idr, idc;
		rclient >> idr >> idc;
		string msg_receive;
		rclient >> msg_receive;
		cout << "Message: " << msg_receive <<endl;
	}
  return 0;
}
