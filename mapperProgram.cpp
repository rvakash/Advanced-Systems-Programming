//============================================================================
// Name        : mapperProgram.cpp
// Author      : Akash R Vasishta
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <string.h>

using namespace std;

int main() {
	char data[100];
	char outputTuple[100];
	std::tuple <int, string, int> output;
	char *token;
	int score;
	int index = 1;
	map<char *,int> rules = {{'P',50},{'L',20},{'D',-10},{'C',30},{'S',40}};
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//	cout << "Value of key rules['D'] = " << rules.at('D') << endl;
	ifstream inputFile;
	inputFile.open("/home/rvakash/asp/assignments/inputForMapper");
//	inputFile >> data;
	while (inputFile >> data) {
/////////////////////////////////////////////////////////////////////
		token = strtok(data," ,()");
		/* walk through other tokens */
		outputTuple = "(" + token;
		while( token != NULL ) {
			printf( " %s\n", token );
			token = strtok(NULL, " ,()");
			if (index)
				score = rules.at(token);
			else{
				index = 0;
				outputTuple = outputTuple + "," + token + ",";
			}
			}
			outputTuple = outputTuple + score + "),";
		}
//		inputFile >> data;
		cout << outputTuple;
		printf("here\n");
	}
	return 0;
}
