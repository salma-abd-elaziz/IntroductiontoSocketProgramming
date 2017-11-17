#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>

using namespace std;


vector<string> parse_file(string file_name);
vector<string> split (string s, string delimiter);
vector<string> get_request_vector(string client_request_string);

#endif

