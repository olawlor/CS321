/* 
Trivial socket-based web server, 
making pages with if-then-else style.

CAUTION: recommended only for classroom use, likely to have
serious scalability and potential security issues.

Dr. Orion Lawlor, lawlor@alaska.edu, 2019-04-03 (Public Domain)
*/
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <sstream>

#include "PracticalSocket.h"
#include "PracticalSocket.cpp"


// Server threads can write to this data structure:
std::mutex tagginwall_lock;
std::string tagginwall="Nobody here yet. ";

// Given this client request, return the complete HTTP reply.
std::string handle_request(const char *client_headers)
{
	// Parse their HTTP request:
	std::istringstream req(client_headers);
	std::string httpMethod; req>>httpMethod;
	std::cout<<"Client method: "<<httpMethod<<std::endl;
	std::string path; req>>path;
	
	if (httpMethod!="GET") return "HTTP/1.1 400 Bad_method\r\n\r\n";
	
	std::string page="";
	
	if (path=="/fancy") {
	  // Prepare a page to send back to them.
	  page+="<h1>Fanciest page eva</h1>\n";
	  page+="Request path: "+path+"<br>\n";
	  page+="You sent me this request: <pre>";
	  page+=client_headers;
	  page+="</pre>  That was a nice request.\n";
	} 
	else if (strncmp(path.c_str(),"/dyn",4)==0) {
	  // Secure our multithread access to the shared wall
	  std::lock_guard<std::mutex> wall_lock(tagginwall_lock); 
	  
	  tagginwall += " <li>"+path+" ";
	  page += "Dynamic page: <ul> "+tagginwall+" </ul>";
	}
	else if (path=="/spartan") {
	  page += "This is a simple page.  It is all we need.";
	}
	else if (path=="/meta") {
	  page += "We have lots of pages: <ul> <li><a href='/spartan'>Simple page</a> <li><img src='/lawlorface.gif' /></ul>";
	}
	else if (path=="/script") {
	  page += "<script>console.log('Logging here'); for (var i=0;i<10;i++) document.write('This is dynamically generated!');</script>";
	}
	else if (path=="/script/annoying") {
	  page += "<script>while (true) { window.alert('Oh noes'); }</script>";
	}
	else /* unknown path? */ {
	  page += "Page not found.  Try /dyn, /meta, /script, etc";
	}
	
	// Wrap the page in a simple HTTP server header
	std::string resp=
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: "+std::to_string(page.size())+"\r\n"
		"\r\n"
		+page;
	
	return resp;
}

void service_client(TCPSocket *client) 
{
  // Grab their HTTP request header:
	const int headerlen=10000;
	char reqbuf[headerlen+1]; //+1 to make space for nul
	int nbytes=client->recv(reqbuf,headerlen);
	if (nbytes>=0) reqbuf[nbytes]=0; // nul terminator
	std::cout<<"Client sent "<<nbytes<<" data:"<<reqbuf<<std::endl;
	
	// Wrap the page in a simple HTTP server header
	std::string resp=handle_request(reqbuf);

	// Send the client the page
	client->send(resp.data(),resp.size());
	std::cout<<"Server send complete."<<std::endl;

	delete client; // close connection
}

int main()
{
	// Listen as http://137.229.25.241:8080
	TCPServerSocket serv(8080);
	
	std::vector<std::thread *> clients;
	
	// Keep serving clients
	while (1) {
		std::cout<<"Waiting for client..."<<std::endl;
		TCPSocket *client=serv.accept();
		std::cout<<"Server got a client!"<<std::endl;
		
		// Make a thread to handle this client:
		clients.push_back(new std::thread(service_client,client));
		
	  // FIXME: need to delete old client threads once they're done.
	}
	return 0;
}




