#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include "rapidjson/document.h"

using namespace std;

void foo(string& tmp){
	tmp = "lol";
}

int main()
{
	map<int, string> names;
	names[1]="batonix";
	names[2]="emilia88";
	map<string, int> scores;
	scores["batonix"]=123;
	scores["emilia88"]=321;
	string fileName="abc";
	//foo(fileName);
	//cout << fileName << endl;
	
	
	/*
	int pid = fork();
	if (pid==0) {
	execl("/bin/ls", "ls", "-r", "-t", "-l", (char *) 0);
	}else{
	
		char buffer[128];
		string result = "";
		FILE* pipe = popen("ls", "r");
		if (!pipe) throw runtime_error("popen() failed!");
		try {
			while (!feof(pipe)) {
				if (fgets(buffer, 128, pipe) != NULL)
					result += buffer;
			}
		} catch (...) {
			pclose(pipe);
		}
		pclose(pipe);
		cout << result << endl;;
	//}
	*/
	//remove("1.txt");
	return 0;
}