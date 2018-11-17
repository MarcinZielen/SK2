#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

using namespace std;


int main()
{
	cout << atoi("p2upa") << endl;
	
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