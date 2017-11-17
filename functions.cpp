#include "functions.h"


#define HTTP "HTTP/1.1"
#define IMAGE "image"
#define HTML "html"
#define TXT "txt"
#define NEWLINE "/n"


// Parsing the command files.
vector<string> parse_file(string file_name) {
    vector<string> result;
    ifstream inFile;
    inFile.open(file_name.c_str());

    if (!inFile.is_open()) {
        cerr << "Unable to open file datafile.txt";
        // exit(1);   // call system to stop
    }
    string x;
    while (std::getline(inFile, x)) {
        result.push_back(x);
    }
    inFile.close();
    result.push_back(x);
    return result;
}  


vector<string> split (string s, string delimiter ) {
    size_t pos = 0;
    cout << "hi";
    vector<string> result;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        s.erase(0, pos + delimiter.length());
        result.push_back(token);
    }
    result.push_back(s);
    return result;
}

// Forward the request to formating function.
vector<string> get_request_vector(string client_request_string) {
    vector<string> result;
    string file_type;
    vector<string> client_request = split(client_request_string, SPACE);
    for (int i = 0; i < client_request.size() - 1; i++) {
        if (i == 1) { // Check file-name.
            vector<string> v = split(client_request.at(i), ".");
            file_type = v.at(v.size());
            if (file_type.compare(TXT) != 0 && file_type.compare(HTML) != 0) {
                file_type = IMAGE;
            }
        } else if (i == 2) {
            result.push_back(HTTP);
        }
        result.push_back(client_request.at(i));
    }
    // Add file type.
    result.push_back(file_type);
    return result;
}

string get_request(string request) {
  
    return request;
}
