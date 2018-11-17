#include "variables.cpp"
#include "connection.cpp"

using namespace std;

int main(int argc, char **argv) {
	if(argc<2){
		cout << "Usage: " << argv[0] << " [port]" << endl;
		return -5;
	}
	loadAll();
	int client = myConnect(argv[1]);//port
	if(client<0) return client;	
    time_t start = time(NULL);//start timer(keep alive value) 
//try
	while(true){
		while(read(client,buffer,1)>0){
			line += buffer;
			try{
				if((int)buffer[0]==10){
					if(line.length()>100){
						resNum = 414;//uri too long
						break;
					}
					if(++newLines==4){
						keepGoing=true;
						break; 
					}					
					int deli = line.find(": ");
					if(deli != string::npos){
						string key = line.substr(0, deli);
						string val = line.substr(deli+2,line.length()-deli-4);
						cout << key << ": ";
						
						if(headerFunctions.find(key) != headerFunctions.end()){
							headerFunctions[key](val);
							if(resNum>0) cout << resNum <<endl;
						}
						cout << val << endl;
						nHeaders++;				
					}
					else{
						method = line.substr(0, line.find(" "));
						if(line.find("HTTP/1.")!=string::npos){
							fileName = line.substr(line.find("/")+1,line.find("HTTP/1.")-line.find("/")-2); 
							if(fileName.length()==0) fileName = "default.html";
							cout << method << endl << fileName << endl;
							nRequests++;
						}else{
							resNum=505;//wrong http version
							break;
						}
					}
					line = "";
				}else if((int)buffer[0]==13){
					if(++newLines==4){
						keepGoing=true;
						break; 
					}
				}
				else newLines = 0;
			}catch(...){ resNum = 400; }//generic client error
		}
		if(keepGoing){
			keepGoing=false;
			break;
		}
//hardcoded timeOut, send msg before closing
		if(difftime(time(NULL),start)>timeOut){//check the timer
			//tell client times up and im done with waiting
			close(client);
		}
	}
	if(method=="PUT"){
		int tmpLength;
		while(true){		
			while(read(client,buffer,1)>0){
				content += buffer;
				tmpLength++;
			}
			if(tmpLength==reqLength) break;
//hardcoded timeOut, send msg before closing
			if(difftime(time(NULL),start)>timeOut){//check the timer
				//tell client times up and im done with waiting
				//close(client);
			}
		}
	}
	if(resNum==200){
		if(expectContinue) resNum=100;//continue
		if(methodToResCode.find(method) != methodToResCode.end()){
			resNum=methodToResCode[method];
		}
//206 range bytes
//300 bez sensu bo serwer sam wybierze domyslny sposob
		if(host=="oldPernament") resNum=301;//pernament,can change method
		//if(host=="oldTemporary") resNum=302;//temporary,can change method
		//if(host=="oldTemporary") resNum=307;//temporary,cant change method
//		if(notModified) resNum=304;
		//if(fileName=="oldPernament") resNum=308;//pernament,cant change method
		//if(host=="oldPernament") resNum=301;//pernament,cant change method		
		if(host=="secure" && !authorized){
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
		in(nRequests>5) resNum=429;
		in(nHeaders>50) resNum=431;
		if(fileName=="illegal") resNum=451;
		
		if(find(methods.begin(), methods.end(), method) == methods.end()) resNum = 501;//unknow method
		if(underConstruction) resNum=503;
	}else{
		if(expectContinue) resNum=417;//abort
	}
		
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
	
	resCode=resCodes[resNum];
	time_t now;
	time(&now);
	struct tm *t = gmtime(&now);
	char godzina[ 80 ];
	strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
	char bufor[100];
	int n;
	
	if(method=="DELETE"){
		string tmp = (host+"/"+fileName);
		int res=open(tmp.c_str(), O_RDONLY);
		close(res);
		if(res<0){
			printf("Delete error\n");
			printf("%s\n",tmp.c_str());
		}
		else remove(tmp.c_str());
		n = sprintf(bufor, "HTTP/1.1 200 OK\n");
		write(client, bufor, n);
		n = sprintf(bufor, "Date: %s\n", godzina);
		write(client, bufor, n);
		res=open("delete.html", O_RDONLY);
		if(res<0){
			printf("File error\n");
			printf("%s\n",fileName.c_str());
			return 0;
		}
		while(read(res,buffer,1)>0){
		   write(client, buffer, 1);
		}
		close(res);
		
	}
	if(method=="GET" || method=="PUT"){		
		n = sprintf(bufor, "%s\n", "-----------");
		write(client, bufor, n);
		
		n = sprintf(bufor, "%s %d %s\n", "HTTP/1.1",resNum,resCode.c_str());
		write(client, bufor, n);
		
		//TODO
		//wildcard not implemented everywhere
		//n = sprintf(bufor, "Access-Control-Allow-Headers: *\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Access-Control-Allow-Credentials: true\n");
		//write(client, bufor, n);
		//or
		//n = sprintf(bufor, "Access-Control-Allow-Origin: *\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE, PUT\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Access-Control-Max-Age: 6000\n");
		//write(client, bufor, n);
		
		if(resNum==405){
			n = sprintf(bufor, "Allow: POST, GET, OPTIONS, DELETE, PUT\n");
			write(client, bufor, n);
		}
		
		//TODO
		//n = sprintf(bufor, "Cache-Control: no-store, no-cache, must-revalidate\n");
		//write(client, bufor, n);
		
		//not needed
		//n = sprintf(bufor, "Clear-Site-Data: "*"\n");
		//write(client, bufor, n);
		
		//cant be used with http2
		//n = sprintf(bufor, "Connection: Keep-Alive\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Content-Disposition: attachment; filename="defaultName.html"\n");
		//write(client, bufor, n);
		
		//default
		//n = sprintf(bufor, "Content-Encoding: identity\n");
		//write(client, bufor, n);
		
		n = sprintf(bufor, "Content-Language: en\n");
		write(client, bufor, n);
		
		//TODO
		//n = sprintf(bufor, "Content-Length: 123\n");
		//write(client, bufor, n);
		
		//TODO
		//PUT Content-Location: ./new.html
		
		//TODO
		n = sprintf(bufor, "Content-Type: text/html; charset=utf-8\n");
		write(client, bufor, n);
		
		n = sprintf(bufor, "Date: %s\n", godzina);
		write(client, bufor, n);
		
		//TODO
		//n = sprintf(bufor, ETag: "33a64df551425fcc55e4d42a148795d9f25f89d4\n");
		//write(client, bufor, n);
		
		t->tm_year -= 1; 
		strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
		n = sprintf(bufor, "Expires: %s\n",godzina);
		write(client, bufor, n);
		
		//TODO
		//n = sprintf(bufor, "Keep-Alive: timeout=15, max=100\n");
		//write(client, bufor, n);
		
		//TODO
		//date -r file.txt
		//n = sprintf(bufor, "Last-Modified: Wed, 21 Oct 2015 07:28:00 GMT\n");
		//write(client, bufor, n);
		
		n = sprintf(bufor, "Server: CustomServer2000\n");
		write(client, bufor, n);
		
		n = sprintf(bufor, "Set-Cookie: myCookie=BATMAN; Max-Age=60\n");
		write(client, bufor, n);
		
		//default
		//n = sprintf(bufor, "Transfer-Encoding: identity\n");
		//write(client, bufor, n);
		
		if(resNum==401){
			n = sprintf(bufor, "WWW-Authenticate: Basic\n");
			write(client, bufor, n);
		}
		
		//default
		//n = sprintf(bufor, "X-DNS-Prefetch-Control: on\n");
		//write(client, bufor, n);
		
		//not needed
		//n = sprintf(bufor, "X-Frame-Options: deny\n");
		//write(client, bufor, n);
		
		//default
		//n = sprintf(bufor, "X-XSS-Protection: 1\n");
		//write(client, bufor, n);
		
		n = sprintf(bufor, "\n");
		write(client, bufor, n);
		
	}	
	if(method=="GET"){
		string wholePath=(host+"/"+fileName);
		thread t([client,wholePath]{
			char *threadBuffer = new char;  
			int res=open(wholePath.c_str(), O_RDONLY);
			if(res<0){
				printf("File error\n");
				printf("%s\n",wholePath.c_str());
				return 0;
			}
			while(read(res,threadBuffer,1)>0){
			   write(client, threadBuffer, 1);
			}
			close(res);
		});
		//t.detach();	
		t.join();
	}
//}catch(...) resNum=500; //generic server error
	myClose(client);

    return 0;
}
