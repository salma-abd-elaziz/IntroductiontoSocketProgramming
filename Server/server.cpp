/*
** server.cpp 
*/
#include "functions.h"

#define GET_TYPE 1
#define POST_TYPE 0
#define VEC_MAX_SIZE 5000
#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
//


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
    ifstream file;
    file.open(fileName.c_str());
    if(!file) {
        file.close();
        return false;
    }
    file.close();
    return true;
}


void sendResponse(int new_fd, char* response){
    int yy; 
    if ((yy = send(new_fd, response, strlen(response), 0)) == -1)
        perror("send response");
}

void handlePostRequest(int new_fd, std::vector<std::string> request){
    
    char* response;
    std::string fileName = request[1];    
    response = OK_MSG;
    
    sendResponse(new_fd, response);

    std::string fileType = request[5];
    if(fileType.compare("txt") == 0){
        recv(new_fd, request[FILENAME]);
    }else if(fileType.compare("html") == 0){
        recv(new_fd, request[FILENAME]);
    }else if(fileType.compare("img") == 0){
        recv(new_fd, request[FILENAME]);
    }else{
        printf("(POST request) file type not recognised\n");
    }
}

void handleGetRequest(int new_fd, std::vector<std::string> curRequest){
    std::string fileType = curRequest[5];
    int result;
    char* response;
    std::string fileName = curRequest[1];    
    if(!checkFileExisting(fileName)){
         response = ERR_MSG;    
    }else{
        response = OK_MSG;
    }
    
     sendResponse(new_fd, response);

    if(fileType.compare("txt") == 0){
        result = send(new_fd, curRequest);
    }else if(fileType.compare("html") == 0){
        result = send(new_fd, curRequest);
    }else if(fileType.compare("img") == 0){
        result = send(new_fd, curRequest);
    }else{
        printf("(GET request) file type not recognised\n");
    }
    
    if(result == -1){
        printf("file not found");
    }
}



int main(int argc, char *argv[])
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

    if (argc != 2)
    {
        fprintf(stderr,"Host name and port missing.\n");
        exit(1);
    }
    if ((rv = getaddrinfo(NULL,argv[1], &hints, &servinfo)) != 0)
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
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        if (setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout)) < 0) {
            perror("setsockopt failed\n");
        }


        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr*)&their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);
        if (!fork())      // this is the child process
        {
            
            // close(sockfd);  // child doesn't need the listener
            char buffer[MAXDATASIZE];

            // char* buffer;
            int result = recv(new_fd, buffer, 255, 0);
            if (result <= -1) {
                perror("recv");
                exit(1);
            }

            // Request.

            // parse request.
            string str(buffer);
            std::vector<std::string> request = parse_request(str);
            
            if(request[FILENAME].at(0) == '/'){
                string fileName = request[FILENAME].substr(1, (request[FILENAME]).size() -1);    
                request[FILENAME] = fileName;
            }

            cout << "FILENAME " << request[FILENAME] << endl;
            
            string fileType = getFileType(request);
            request[5] = fileType;

            // GET => ttt.txt => HTTP/1.1 => Host: => www.tutorialspoint.com
            if (request.at(REQUESTTYPE).compare(GET) == 0)
                handleGetRequest(new_fd, request);
            else if (request.at(REQUESTTYPE).compare(POST) == 0) 
                handlePostRequest(new_fd, request);
            else
                perror("request type");

            close(new_fd);
            exit(0);
        }
    }
    return 0;
}