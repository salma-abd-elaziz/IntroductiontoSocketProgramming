/*
** server.cpp 
*/
#include "functions.h"

#define GET_TYPE 1
#define POST_TYPE 0
#define VEC_MAX_SIZE 5000
#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define OK_MSG "HTTP/1.0 200 OK\r\n"
#define ERR_MSG "HTTP/1.0 404 Not Found\r\n"
//
#define REQUESTTYPE 0
#define FILENAME 1
#define HOSTNAME 4

void sigchld_handler(int s)
{
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void* get_in_addr(struct sockaddr* sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool checkFileExisting(std::string fileName){
    return true;
}

void recvTxt(int new_fd){

}

void recvHTML(int new_fd){

}

void recvImg(int new_fd){
	
}

int sendTxt(int new_fd, std::vector<std::string> curRequest){

}

int sendHTML(int new_fd, std::vector<std::string> curRequest){

}

int sendImg(int new_fd, std::vector<std::string> curRequest){
	
}

void sendResponse(int new_fd, char* response){
    if (send(new_fd, response, strlen(response), 0) == -1)
    	perror("send response");
}

void handlePostRequest(int new_fd, std::vector<std::string> curRequest){
	sendResponse(new_fd, OK_MSG);

    std::string fileType = curRequest[4];
    if(fileType.compare("txt") == 0){
    	recvTxt(new_fd);
    }else if(fileType.compare("html") == 0){
		recvHTML(new_fd);
    }else if(fileType.compare("img") == 0){
    	recvImg(new_fd);
    }else{
		printf("(POST request) file type not recognised\n");
    }
}

void handleGetRequest(int new_fd, std::vector<std::string> curRequest){
	cout << "lalala" << endl;
	std::string fileType;// = curRequest[4];
	int result;
	char* response;
	std::string fileName;// = curRequest[1];

    
    cout << "ahah" << endl;
	if(!checkFileExisting(fileName)){
		 response = ERR_MSG;	
	}else{
		response = OK_MSG;
	}
	cout << "hamada" << endl;
    
	sendResponse(new_fd, response);
    
    cout << "hamada 2" << endl;

    if(fileType.compare("txt") == 0){
    	result = sendTxt(new_fd, curRequest);
    }else if(fileType.compare("html") == 0){
		result = sendHTML(new_fd, curRequest);
    }else if(fileType.compare("img") == 0){
    	result = sendImg(new_fd, curRequest);
    }else{
		printf("(GET request) file type not recognised\n");
    }
    
    cout << "ahah" << endl;
	if(result == -1){
		printf("file not found");
	}
}



int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;  // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  // use my IP
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo);  // all done with this structure
    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    sa.sa_handler = sigchld_handler;  // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }
    printf("server: waiting for connections...\n");
    while (1)    // main accept() loop
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);
        if (new_fd == -1)
        {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        if (!fork())      // this is the child process
        {
            
            close(sockfd);  // child doesn't need the listener
            char buf[MAXDATASIZE];
            int numbytes;
            
			char* buffer;
			int result = recv(new_fd,buffer, 255, 0);
			if (result <= -1) {
                perror("recv");
                exit(1);
            }

            // Request.
            cout << buffer << endl;

            // parse request.
            string str(buffer);
            vector<string> request = parse_request(str);

            //check Get or Post.

            // Get
            // GET => ttt.txt => HTTP/1.1 => Host: => www.tutorialspoint.com
            if (request.at(REQUESTTYPE).compare(GET) == 0) {
                cout << "get" << endl;
                cout << "hi" << endl;
                string file_name = request.at(FILENAME);
                string line;
                ifstream infile;
                cout <<  file_name << endl;
                // use file name to open file and send it. 
                 infile.open(file_name.c_str()); // Get file name from request
                // File doesn't exits.
                if (!infile){
                    //error file doesn't exitst
                }
                cout << "228 " << endl;
                while(!infile.eof()) {
                    cout << "230" << endl;
                    getline(infile,line); 
                    const char *STRING_mod = line.c_str();
                    send(new_fd , STRING_mod , strlen(STRING_mod) , 0);
                }
                cout << "235" << endl;
                char *end_msg="end";
                send(new_fd , end_msg , strlen(end_msg) , 0);
                infile.close();

            } else if (request.at(REQUESTTYPE).compare(POST) == 0) {

            // Post, send OK, then receive the file to update.


                cout << "post" << endl;

            }    
            /*if(curRequest[0].compare("GET") == 0)
                handleGetRequest(new_fd, curRequest);
            else
	            handlePostRequest(new_fd, curRequest);
*/
            cout << "finished" << endl;
            close(new_fd);
            exit(0);
        }
    }
    return 0;
}