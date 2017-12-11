#include "functions.h"

// Parsing the command files.
vector<string> parse_file(string file_name) {
    vector<string> result;
    ifstream inFile;
    inFile.open(file_name.c_str());

    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        // exit(1);   // call system to stop
    }
    string x;
    while (std::getline(inFile, x)) {  
        // getline doesn't read new line :3, so no need to check for newlines.
        result.push_back(x);
    }
    inFile.close();
    return result;
}  

vector<string> split (string s, string delimiter) {
    size_t pos = 0;
    vector<string> result;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
        result.push_back(token);
    }
    result.push_back(s); //check that 
    return result;
}

// get strings of a request 
string get_client_request(vector<string> v, int index) {
    string delimiter = CARRiAGERETURN;
    delimiter += NEWLINE;
    delimiter += CARRiAGERETURN;
    delimiter += NEWLINE;
    string result;
    for (int i = index; i < v.size(); i++) {
        result += v.at(i);
        if ((v.at(i)).find(delimiter) != string::npos) { //found, then end of a request.
            result += SEPERATOR;
            result += to_string(i); //concatenate index in the end. 
            break;
        } 
    }
    return result;
}

// Returns one HTTP request.
string get_server_request(string request) {
    string result;
    string port;
    cout << endl;
    vector<string> client_request = split(request,SPACE);
    for (int i = 0; i < client_request.size(); i++) {
        if (i == 2) { 
            result.append(HTTP);
            result += CARRiAGERETURN;
            result += NEWLINE;
            result += HOST;
            result += SPACE; 
        } 
        result.append(client_request.at(i));
        result += SPACE;
    }
    return result;
}



// Called after get_request
vector<string> parse_request(string request) {
    if (request.find(POST) != string::npos) 
        cout << "Request : "<< request.substr(0, request.size()-5) << endl;
    else 
        cout << "Request : "<< request << endl;
    vector<string> result;
    //  Assuming the request ends with \r\n, split at them.
    string delimiter = CARRiAGERETURN;
    delimiter += NEWLINE;
    
    vector<string> request_lines = split(request, delimiter);
    for (int i = 0; i < request_lines.size(); i++) {
        vector<string> s = split(request_lines.at(i), SPACE);
        result.insert(result.end(), s.begin(), s.end());
    }   
     return result;
}



string getFileType(vector<string> request){
    string fileName = request[FILENAME];
    vector<string> fileNameTokens = split(fileName, ".");
    string fileExtension = fileNameTokens[1];
    string imgFileType = "img";
    
    if(fileExtension.compare("txt") != 0 && fileExtension.compare("html") != 0)
        return imgFileType;
    else
        return fileExtension;
}

void recv(int new_fd, std::string fileName){
    char buffer[MAXDATASIZE];
    recv(new_fd, buffer, MAXDATASIZE, 0);
    int length = stoi(string(buffer).substr(16, 20));
    ofstream myfile (fileName.c_str(), ofstream::binary);
    
    int count = 0;
    int received_length = 0;
    
    while (received_length < length){
        count = recv(new_fd, buffer, MAXDATASIZE, 0);        
        received_length += count;
        buffer[count] = '\0';
        myfile.write(buffer, count);
    }
    myfile.close();
}

int send(int new_fd, std::vector<std::string> curRequest){

    char file_size[MAXDATASIZE];
    struct stat file_stat;
    int offset;
    int remain_data;
    ssize_t len;
    int fd;
    int sent_bytes = 0;

    string fileName = curRequest[1];

    char *FILE_TO_SEND = new char[fileName.length() + 1]; // or
    // char y[100];
    strcpy(FILE_TO_SEND, fileName.c_str());

    fd = open(FILE_TO_SEND, O_RDONLY);

    if (fd == -1){
        fprintf(stderr, "Error opening file --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Get file stats */
    if (fstat(fd, &file_stat) < 0){
        fprintf(stderr, "Error fstat --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    sprintf(file_size, "Content-Length: %20.d", file_stat.st_size);
    
    /* Sending file size */
    len = send(new_fd, file_size, strlen(file_size), 0);
    if (len < 0){
        fprintf(stderr, "Error on sending greetings --> %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    usleep(100);

    offset = 0;
    remain_data = file_stat.st_size;
    off_t ot = (off_t) offset;
          /* Sending file data */
    while (((sent_bytes = sendfile(new_fd, fd, &ot, BUFSIZ)) > 0) && (remain_data > 0)){
           remain_data -= sent_bytes;
    }
    // const char* end_msg = END_MSG;  
    // send(new_fd , end_msg , strlen(end_msg) , 0);

    close(new_fd);           
}
