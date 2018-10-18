#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex>
#include <time.h>

using namespace std;

int main(int argc, char **argv) {
    string host,user_agent;
    string fileName;
    int req=open("req.txt", O_RDONLY);
    if(req<0){
        printf("File error\n");
        return 0;
    }
    int resNum = 200;
    string resCode;
    //int fileOut=open("fileOut.txt", O_CREAT|O_WRONLY);
    char *buffer = new char;
    string line("");
    //int size;
    int it = 0;
    while(read(req,buffer,1)>0){
        //write(fileOut,buffer,1);
        line += buffer;

        if((int)buffer[0]==10){
                /*
            regex r("(.+?): ([^ ]+)"); // entire match will be 2 numbers
            auto words_begin = sregex_iterator(line.begin(), line.end(), r);
            auto words_end = sregex_iterator();
            //smatch m;
            //regex_search(line, m, r);
            //for(auto v: m) cout << v << endl;
            for (sregex_iterator i = words_begin; i != words_end; ++i) {
                smatch match = *i;
                string match_str = match.str();
                cout << match_str << '\n';
            }
            */
            int deli = line.find(": ");
            if(deli > -1){
                //cout << line.substr(0, line.find(": ")) << ":" << line.substr(line.find(":")+2,line.length()-line.find(":")-3) << endl;
                string key = line.substr(0, line.find(": "));
                string val = line.substr(line.find(":")+2,line.length()-line.find(":")-3);
                if(key=="Host") { fileName = val+fileName; }
            }
            else{
                istringstream iss(line);
                iss >> fileName;
                iss >> fileName;
            }
            line = "";
        }
        //printf("%s %d\n",buffer,buffer[0]);
    }
    //cout << host << endl;
    close(req);
    switch(resNum){
        case 200:
            resCode = "OK";
    }
    cout << "------------------------------------" << endl;
    cout << "HTTP/1.1 " << resNum << " " << resCode << endl;
    time_t now;
    time(&now);
    struct tm *t = gmtime(&now);
    char godzina[ 80 ];
    strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
    cout << "Date: " << godzina << endl;
    cout << "Server: CustomServer2000" << endl;
    t->tm_year -= 1;
    strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
    cout << "Expires: " << godzina << endl;
    cout << "Cache-Control: no-store, no-cache, must-revalidate" << endl;
    cout << "Keep-Alive: timeout=15, max=100" << endl;
    cout << "Connection: Keep-Alive" << endl;
    cout << "Transfer-Encoding: chunked" << endl;
    cout << "Content-Type: text/html; charset=utf-8" << endl;
    //cout << endl;
    printf("%c%c",(char)13,(char)10);

    int res=open(fileName.c_str(), O_RDONLY);
    if(res<0){
        printf("File error\n");
        return 0;
    }
    while(read(res,buffer,1)>0){
        printf("%s",buffer);
    }
    close(res);


    return 0;
}
