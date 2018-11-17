#include <iostream>
#include <fcntl.h>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdio>
#include <string>
#include <ctime>
#include <thread>
#include <regex>
#include <time.h>
#include <string>

using namespace std;

string host("default");
string user_agent("Unknown agent");
string method("UNKNOWN");
string fileName("default.html");
string line;
string content;
string resCode;
bool expectContinue;
bool notModified;
bool authorized;
bool noPreconditions;
bool underConstruction;
bool correct = true;
bool keepGoing;
int client;
int resNum=200;
int reqLength;
int nRequests;
int nHeaders;
int newLines;
int timeOut=10;
int n;
char *buffer = new char;
char *bufor = new char[100];

vector<string> methods = {"GET","PUT","HEAD","DELETE","OPTIONS","CONNECT","TRACE","PATCH","POST"};
vector<string> badMethods = {"CONNECT","TRACE","PATCH","POST"};
vector<string> charsets = {"utf-8", "*"};
vector<string> encodings = {"identity", "*"};
vector<string> languages = {"pl", "en", "en-us", "*"};
vector<string> MIMEtypes = {
	"text/html", "text/plain", "text/*",
	"image/gif", "image/png", "image/jpeg", "image/bmp", "image/webp", "image/svg+xml", "image/*",
	"audio/midi", "audio/mpeg", "audio/webm", "audio/wav", "audio/*",
	"video/webm","video/*",
	"application/javascript", "application/pdf", "application/*",
	"*/*"
};

map<string, string> extToType;
map<int, string> resCodes;
map<string, function<void(string)>> headerFunctions;
map<string, int> methodToResCode;

bool searchVector(string val, vector<string> v){
	size_t pos = 0;
	string token;
	while ((pos = val.find(',')) != string::npos) {
		token = val.substr(0, pos);
		if(find(v.begin(), v.end(), token) != v.end()) return true;
		val.erase(0, pos + 1);
	}
	if(find(v.begin(), v.end(), val) != v.end()) return true;
	return false;
}

void loadAll(){
	extToType["txt"]="text/plain";
	extToType["html"]="text/html";
	extToType["gif"]="image/gif";
	extToType["png"]="image/png";
	extToType["jpeg"]="image/jpeg";
	extToType["jpg"]="image/jpeg";
	extToType["bmp"]="image/bmp";
	extToType["webp"]="image/webp";
	extToType["svg"]="image/svg+xml";
	extToType["midi"]="audio/midi";
	extToType["mp3"]="audio/mpeg";
	extToType["wav"]="audio/wav";
	extToType["webm"]="video/webm";
	extToType["js"]="application/javascript";
	extToType["pdf"]="application/pdf";
	
	resCodes[100]="Continue";
	resCodes[101]="Switching Protocols";
	resCodes[110]="Connection Timed Out";
	resCodes[111]="Connection refused";
	resCodes[200]="OK";
	resCodes[201]="Created";
	resCodes[202]="Accepted";
	resCodes[203]="Non-Authoritative Information";
	resCodes[204]="No content";
	resCodes[205]="Reset Content";
	resCodes[206]="Partial Content";
	resCodes[300]="Multiple Choices";
	resCodes[301]="Moved Permanently";
	resCodes[302]="Found";
	resCodes[303]="See Other";
	resCodes[304]="Not Modified";
	resCodes[305]="Use Proxy";
	resCodes[306]="Switch Proxy";
	resCodes[307]="Too many redirects";
	resCodes[400]="Bad Request";
	resCodes[401]="Unauthorized";
	resCodes[402]="Payment Required";
	resCodes[403]="Forbidden";
	resCodes[404]="Not Found";
	resCodes[405]="Method Not Allowed";
	resCodes[406]="Not Acceptable";
	resCodes[407]="Proxy Authentication Required";
	resCodes[408]="Request Timeout";
	resCodes[409]="Conflict";
	resCodes[410]="Gone";
	resCodes[411]="Length required";
	resCodes[412]="Precondition Failed";
	resCodes[413]="Request Entity Too Large";
	resCodes[414]="Request-URI Too Long";
	resCodes[415]="Unsupported Media Type";
	resCodes[416]="Requested Range Not Satisfiable";
	resCodes[417]="Expectation Failed";
	resCodes[418]="I'm a teapot";
	resCodes[422]="Unprocessable Entity";
	resCodes[428]="Precondition Required";
	resCodes[429]="Too Many Requests";
	resCodes[431]="Request Header Fields Too Large";
	resCodes[451]="Unavailable For Legal Reasons";
	resCodes[500]="Internal Server Error";
	resCodes[501]="Not Implemented";
	resCodes[502]="Bad Gateway";
	resCodes[503]="Service Unavailable";
	resCodes[504]="Gateway Timeout";
	resCodes[505]="HTTP Version Not Supported";
	resCodes[506]="Variant Also Negotiates";
	resCodes[507]="Insufficient Storage";
	resCodes[508]="Loop Detected";
	resCodes[509]="Bandwidth Limit Exceeded";
	resCodes[510]="Not Extended";
	resCodes[511]="Network Authentication Required";
	
	headerFunctions["Accept"]=[](string val){ if(searchVector(val,MIMEtypes)==false) resNum = 406; };
	headerFunctions["Accept-Charset"]=[](string val){ if(searchVector(val,charsets)==false) resNum = 406; };
	headerFunctions["Accept-Encoding"]=[](string val){ if(searchVector(val,encodings)==false) resNum = 406; };
	headerFunctions["Accept-Language"]=[](string val){ if(searchVector(val,languages)==false) resNum = 406; };
	headerFunctions["Content-Type"]=[](string val){ if(searchVector(val,MIMEtypes)==false) resNum = 415; };
	headerFunctions["Content-Encoding"]=[](string val){ if(searchVector(val,encodings)==false) resNum = 406; };
	headerFunctions["Expect"]=[](string val){ if(val=="100-continue") expectContinue=true; };
	headerFunctions["Host"]=[](string val){ host=val; };
	headerFunctions["Content-length"]=[](string val){ reqLength=atoi(val.c_str()); };
	
	methodToResCode["GET"]=200;
	methodToResCode["HEAD"]=204;//no content,no refresh	
	methodToResCode["OPTIONS"]=204;//no content,no refresh	
	methodToResCode["DELETE"]=205;//no content, refresh
	//methodToResCode["PUT"]=200;//if put modifies,sends back resource
	//methodToResCode["PUT"]=201;//created,link to resource
	
	
}