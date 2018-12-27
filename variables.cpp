#include <iostream>
#include <fcntl.h>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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
#include <fstream>

using namespace std;

string host("default");
string user_agent("Unknown agent");
string method("UNKNOWN");
string fileName("default.html");
string line;
string body;
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
int contentLength=0;
int n;
char *buffer = new char;
char *bufor = new char[100];

vector<string> methods = {"GET","PUT","HEAD","DELETE","OPTIONS","CONNECT","TRACE","PATCH","POST"};
vector<string> badMethods = {"CONNECT","TRACE","PATCH","POST"};
vector<string> charsets = {"utf-8", "*"};
vector<string> encodings = {"identity", "*"};
vector<string> languages = {"pl", "en", "en-us", "*"};
vector<string> MIMEtypes = {
	"text/html", "text/css", "text/plain", "text/xml", "text/*",
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
		//cout << "token="<<token<<endl;
		//cout << "val="<<val<<endl;
	}
	if(find(v.begin(), v.end(), val) != v.end()) return true;
	return false;
}

void loadAll(){
	extToType["txt"]="text/plain";
	extToType["xml"]="text/xml";
	extToType["html"]="text/html";
	extToType["css"]="text/css";
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
	headerFunctions["Content-Length"]=[](string val){ reqLength=stoi(val); };
	
	methodToResCode["GET"]=200;
	methodToResCode["HEAD"]=204;//no content,no refresh	
	methodToResCode["OPTIONS"]=204;//no content,no refresh	
	methodToResCode["DELETE"]=205;//no content, refresh
	//methodToResCode["PUT"]=200;//if put modifies,sends back resource
	//methodToResCode["PUT"]=201;//created,link to resource	
}

string extension(string filename){
	int deli = filename.find_last_of(".");
	if(deli != string::npos){
		string ext = filename.substr(deli+1,filename.length()-deli);
		return ext;			
	}
}

string readBody(int client){
	cout << reqLength << endl;
	int length=0;
	char *myChar = new char;
	string body;
	time_t  start = time(NULL);//start timer/keep alive value/how long am i willing to listen for the whole body
	//while(true){	
		while(read(client,myChar,1)>0){
			cout << myChar[0];
			body += myChar;
			if(++length==reqLength) break;
		}
		cout << "\nskonczylem\n";
		
	//IMPORTANT! DELETE THIS HARDCODED LINE BELOW
		//if(length==10) break;
	//hardcoded timeOut, send msg before closing
		if(difftime(time(NULL),start)>timeOut){//check the timer
			//tell client times up and im done with waiting
			//close(client);
		}
	//}
	return body;
}

int readHeaders(int client){
	time_t start = time(NULL);//start timer(how long am i willing to listen for all headers)
	int count = 0; 
	while(true){
		while(read(client,buffer,1)>0){
			count++;
			line += buffer;
			try{
				//cout << buffer[0];
				if((int)buffer[0]==10){
					if(line.length()>1000){
						resNum = 414;//uri too long
						return 1;
						//break;
					}				
					if(++newLines==4){
						keepGoing=true;
						return 1;
						//break; 
					}
					int deli = line.find(": ");
					if(deli != string::npos){
						string key = line.substr(0, deli);
						string val = line.substr(deli+2,line.length()-deli-3);
						//cout << key << ": ";
						
						if(headerFunctions.find(key) != headerFunctions.end()){
							headerFunctions[key](val);
							//if(resNum>0) cout << resNum <<endl;
						}
						//cout << val << endl;
						nHeaders++;				
					}
					else{
						method = line.substr(0,line.find(" "));
						if(line.find("HTTP/1.")!=string::npos){
							fileName = line.substr(line.find("/")+1,line.find("HTTP/1.")-line.find("/")-2); 
							if(fileName.length()==0) fileName = "default.html";
							cout << client << " " << method << " " << fileName << endl;
							nRequests++;
						}else{
							resNum=505;//wrong http version
							return 1;
							//break; 
						}
					}
					line = "";
				}else if((int)buffer[0]==13){
					if(++newLines==4){
						keepGoing=true;
						return 1;
						//break; 
					}
				}
				else newLines = 0;				
			}catch(...){ resNum = 400; }//generic client error
		}
		if(count<1) return -1;
		if(keepGoing){
			keepGoing=false;
			return 1;
			//break; 
		}
//hardcoded timeOut, send msg before closing
		if(difftime(time(NULL),start)>timeOut){//check the timer
			//tell client times up and im done with waiting
			//close(client);
		}
	}
}

char* getTime(){
	time_t now;
	time(&now);
	struct tm *t = gmtime(&now);
	char* godzina = new char[30];
	strftime( godzina, 30, "%a, %d %b %Y %X GMT", t );
	return godzina;
}

void writeHeaders(int client){
//HARDCODED TO WORK ALWAYS
		if(fileName=="favicon.ico"){
			n = sprintf(bufor, "404 Not Found\n");
			write(client, bufor, n);
			write(1, bufor, n);
		}
		else{
			n = sprintf(bufor, "HTTP/1.1 200 OK\n");
			write(client, bufor, n);
			write(1, bufor, n);
		}
//NOT HARDCODED TO WORK	ALWAYS
		//n = sprintf(bufor, "HTTP/1.1 %d %s\n",resNum,resCode.c_str());
		//write(client, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Date: %s\n", getTime());
		write(client, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Server: CustomServer2000\n");
		write(client, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Content-Language: en\n");
		write(client, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Access-Control-Allow-Origin: *\n");
		write(client, bufor, n);
		//write(1, bufor, n);		

		string ext = extension(fileName);
		if(extToType.find(ext) != extToType.end() ){
			string type = extToType[ext];
			printf("%s\n", type.c_str() );
			n = sprintf(bufor, "Content-Type: %s; charset=utf-8\n", type.c_str());
			write(client, bufor, n);	
			//write(1, bufor, n);
		}

		struct stat stat_buf;
		if( stat(fileName.c_str(), &stat_buf) == 0){
			n = sprintf(bufor, "Content-Length: %d\n", stat_buf.st_size);
			write(client, bufor, n);	
			//write(1, bufor, n);
		}

		n = sprintf(bufor, "\n");
		write(client, bufor, n);
		//write(1, bufor, n);

		if(method.rfind("GET"==0)){
			//host=".";//TEMPORARY
			string wholePath=("./"+fileName);
			cout << wholePath << endl;
			char *threadBuffer = new char;  
			int res=open(wholePath.c_str(), O_RDONLY);
			if(res<0){
				printf("File error\n");
				printf("%s\n",wholePath.c_str());
			}else{
				while(read(res,threadBuffer,1)>0){
					write(client, threadBuffer, 1);
					//write(1, threadBuffer, 1);
				}
			}
			close(res);
		}
}

int checkConditions(){
	//random nr here, change it
	//if(resNum==200){
	if(resNum==123){
		if(expectContinue) resNum=100;//continue
		if(methodToResCode.find(method) != methodToResCode.end()){
			resNum=methodToResCode[method];
		}
//206 range bytes
//300 bez sensu bo serwer sam wybierze domyslny sposob
		if(host=="oldPernament") resNum=301;//pernament,can change method
		//if(host=="oldTemporary") resNum=302;//temporary,can change method
		//if(host=="oldTemporary") resNum=307;//temporary,cant change method
//if(notModified) resNum=304;
		//if(fileName=="oldPernament") resNum=308;//pernament,cant change method
		//if(host=="oldPernament") resNum=301;//pernament,cant change method		
		if(host=="secure" && !authorized){//TODO
			n = sprintf(bufor, "WWW-Authenticate: Basic realm=\"Enter the password\", charset=\"UTF-8\"\n");
			write(client, bufor, n);		
			resNum=401;
		}
		if(find(badMethods.begin(), badMethods.end(), method) != badMethods.end()) resNum = 405;//bad method
		if(host=="deleted") resNum=410;	
		//if(contentLength==0) resNum=411;//length required
		//if(notModified) resNum=412;//if other than get or head
		if(contentLength>10000) resNum=413;//too big
		if(fileName=="teapot") resNum=418;//im a teapot
		if(fileName=="unprocessable") resNum=422;
		if(noPreconditions) resNum=428;
		if(nRequests>5) resNum=429;
		if(nHeaders>50) resNum=431;
		if(fileName=="illegal") resNum=451;
		
		if(find(methods.begin(), methods.end(), method) == methods.end()) resNum = 501;//unknow method
		if(underConstruction) resNum=503;
	}else{
		if(expectContinue) resNum=417;//abort
	}
	
	/*
	if(host=="forbidden") resNum=403;
	string tmp = host+"/"+fileName;
	int res=open(tmp.c_str(), O_RDONLY);
	close(res);
	if(res<0){
		printf("File %s error\n",tmp.c_str());
		resNum=404;//not found
	}
	if (find(methods.begin(), methods.end(), badMethod) != methods.end()) resNum = 405;//bad method
	if(host=="deleted") resNum=410;	
	//if(contentLength==0) resNum=411;//length required
	//if(notModified) resNum=412;//if other than get or head
	if(contentLength>10000) resNum=413;//too big
	if(expectContinue && !correct) resNum=417;//abort
	if(fileName=="teapot") resNum=418;//im a teapot
	if(fileName=="unprocessable") resNum=422;
	if(noPreconditions) resNum=428;
	in(nRequests>5) resNum=429;
	in(nHeaders>50) resNum=431;
	if(fileName=="illegal") resNum=451;
	
	if(find(methods.begin(), methods.end(), method) == methods.end()) resNum = 501;//unknow method
	if(underConstruction) resNum=503;
	*/	
}