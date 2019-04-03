/* 
Trivial socket-based web server, 
recommended only for classroom use.

Dr. Orion Lawlor, lawlor@alaska.edu, 2019-04-03 (Public Domain)
*/
#include <iostream>
#include <vector>
#include <string>
#include "PracticalSocket.h"
#include "PracticalSocket.cpp"

int main()
{
	// Listen as http://localhost:8080
	TCPServerSocket serv(8080);
	
	// Keep serving clients
	while (1) {
		std::cout<<"Waiting for client..."<<std::endl;
		TCPSocket *client=serv.accept();
		std::cout<<"Server got a client!"<<std::endl;
		
		// Grab their HTTP request header:
	  const int headerlen=10000;
	  char reqbuf[headerlen+1]; //+1 to make space for nul
	  int nbytes=client->recv(reqbuf,headerlen);
	  if (nbytes>=0) reqbuf[nbytes]=0; // nul terminator
	  std::cout<<"Client sent "<<nbytes<<" data:"<<reqbuf<<std::endl;
	  
	  // Make a page for this client
	  std::string page="<h1>It's a web page</h1> But that's about it.";
		
	  // Wrap the page in a simple HTTP server header
	  std::string resp=
		  "HTTP/1.1 200 OK\r\n"
		  "Content-Type: text/html\r\n"
		  "Content-Length: "+std::to_string(page.size())+"\r\n"
		  "\r\n"
		  +page;
	  
	  // Send the client the page
	  client->send(resp.data(),resp.size());
	}
	return 0;
}




