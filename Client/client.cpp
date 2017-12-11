/*
** client.cpp
*/
#include "functions.h"

#define MAXDATASIZE 100 // max number of bytes we can get at once


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	vector<string> result = parse_file("read.txt");


    for (int i = 0; i < result.size(); i++) {
    	// get strings of a request 

    	string client_request = get_client_request(result, i);
    	int pos  = client_request.find(SEPERATOR) + 2;
    	string index = client_request.substr(pos , client_request.size() - pos);
    	
    	i = atoi(index.c_str());
    	client_request.erase(pos - 2, client_request.size() - pos - 2);

    	string request = get_server_request(client_request);
    	vector<string> requestVector = parse_request(request);
    	
	    if ((rv = getaddrinfo((requestVector.at(HOSTNAME)).c_str(), (requestVector.at(HOSTNAME + 1)).c_str(), &hints, &servinfo)) != 0)
	    {
	        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	        return 1;
	    }
    	
	   // loop through all the results and connect to the first we can
	    for(p = servinfo; p != NULL; p = p->ai_next) {
	        if ((sockfd = socket(p->ai_family, p->ai_socktype,
	                             p->ai_protocol)) == -1)
	        {
	            perror("client: socket");
	            continue;
	        }
	        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
	        {
	            close(sockfd);
	            perror("client: connect");
	            continue;
	        }
	        break;
	    }
	    if (p == NULL) {
	        fprintf(stderr, "client: failed to connect\n");
	        return 2;
	    }
	    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	              s, sizeof s);
	    printf("client: connecting to %s\n", s);
	    freeaddrinfo(servinfo); // all done with this structure
       
        string fileType = getFileType(requestVector);

        if (send(sockfd, request.c_str(), request.size(), 0) == -1) perror("send");
        // check if get or post.
        if (request.find(GET) != string::npos) { // get
            // receive file (download).
            char buffer[MAXDATASIZE];
            // response to check.
            int result = recv(sockfd, buffer, MAXDATASIZE, 0);
            if (result <= -1) {
                perror("recv");
                exit(1);
            }
            
            cout << "response : " << string(buffer).substr(0, result) << endl; 
            if (string(buffer).find("OK") != string::npos) {
                // char* buffer;
                if(fileType.compare("txt") == 0){
                    recv(sockfd, requestVector[FILENAME]);
                }else if(fileType.compare("img") == 0){
                    recv(sockfd, requestVector[FILENAME]);
                }
            }

        } else { // Post
            // Wait for OK message then sent file according to file type.
            char buffer[MAXDATASIZE];
            // Response to check.
            int result = recv(sockfd, buffer, MAXDATASIZE, 0);
            if (result <= -1) {
                perror("recv");
                exit(1);
            }
            cout << "response : " << string(buffer).substr(0, result) << endl; 
            if (string(buffer).find("OK") != string::npos) {
            	// send file to server.
            	int result = send(sockfd, requestVector);
            }

        }

        cout << "===============================================" <<endl;
    }
    

     
    /*if (send(sockfd, "hamada!", 13, 0) == -1) perror("send");
    printf("client: received '%s'\n",buf);*/
    close(sockfd);
    return 0;
}