#include "functions.h"

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
        if (x.compare(NEWLINE) == 0) continue; // not working :V. because getline doesn't read new line :3.
        result.push_back(x);
    }
    inFile.close();
    return result;
}  




vector<string> split (string s, string delimiter ) {
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

// Returns one HTTP request.



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
    string result;
    string file_type;
    vector<string> client_request = split(request,SPACE);
    for (int i = 0; i < client_request.size() - 1; i++) { // size 1 => port number not used
        if (i == 2) { 
            result.append(HTTP);
            result += NEWLINE;
            result += HOST;
            result += SPACE; 
        }
        result.append(client_request.at(i));
        result += SPACE;
    }
    // add carriage return and new line at end of request.
    result += CARRiAGERETURN;  
    result += NEWLINE;
    return result;
}

