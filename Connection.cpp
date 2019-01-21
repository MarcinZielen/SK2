#include <sys/stat.h>

using namespace std;

class Connection {
	public:
	bool keepAlive = false;
	bool expectContinue = false;
	bool authorized = false;
	bool isFolder = false;
	int clientFd;
	int keepAliveTime = 10;
	int resNum = 200;
	int reqLength = 0;
	int nRequests = 0;
	string method;
	string fileName;
	string host;
	string user_agent;
	string requestMethod;

	void myPut();
	int readHeaders();
	void writeHeaders();
	void checkConditions();
	
	Connection(int tmp){ clientFd = tmp; };
};

void contentLengthProccess(string val, Connection& con){
	int tmp=stoi(val);
	if(tmp>10000) con.resNum=413;//too big
	else if(tmp>-1) con.reqLength=tmp;
}

void allowProccess(string method, Connection& con){
	if(find(methods.begin(), methods.end(), method) == methods.end()) con.resNum = 501;//unknow method
	else if(find(badMethods.begin(), badMethods.end(), method) != badMethods.end()) con.resNum = 405;//bad method
}

map<string, function<void(string, Connection&)>> headerFunctions={
	{"Accept", [](string val, Connection& con){ if(searchVector(val,MIMEtypes)==false) con.resNum = 406; } },
	{"Accept-Charset", [](string val, Connection& con){ if(searchVector(val,charsets)==false) con.resNum  = 406; } },
	//{"Accept-Encoding", [](string val, Connection& con){ if(searchVector(val,encodings)==false) con.resNum  = 406; } },
	{"Accept-Language", [](string val, Connection& con){ if(searchVector(val,languages)==false) con.resNum  = 406; } },
	{"Content-Type", [](string val, Connection& con){ if(searchVector(val,MIMEtypes)==false) con.resNum  = 415; } },
	//{"Content-Encoding", [](string val, Connection& con){ if(searchVector(val,encodings)==false) con.resNum  = 406; } },
	{"Expect", [](string val, Connection& con){ if(val=="100-continue") con.expectContinue=true; } },
	{"Host", [](string val, Connection& con){ con.host=val; } },
	{"Content-Length", [](string val, Connection& con){ contentLengthProccess(val, con); } },
	{"Connection", [](string val, Connection& con){ if(val=="keep-alive") con.keepAlive=true; } },
	{"Keep-Alive", [](string val, Connection& con){ int tmp=stoi(extract(val,"timeout")); if(tmp>-1) con.keepAliveTime=tmp; } },
	{"User-Agent", [](string val, Connection& con){ con.user_agent=val; } },
	{"Access-Control-Request-Method", [](string val, Connection& con){ allowProccess(val, con); } }
};

void Connection::checkConditions(){
	if(resNum==200){
		if(find(methods.begin(), methods.end(), method) == methods.end()) resNum = 501;//unknow method
		else{
			if(find(badMethods.begin(), badMethods.end(), method) != badMethods.end()) resNum = 405;//bad method
			else{	
				/*
				muFile.lock();
				if(writers.find(fileName.c_str())!=writers.end()){
					muFile.unlock();
					resNum = 503;//temporary unavaible
					return;
				}
				if(method=="DELETE"){
					if(readers.find(fileName.c_str())!=readers.end()){
						muFile.unlock();
						resNum = 503;//temporary unavaible
						return;
					}
					else writers.insert(fileName.c_str());
				}					
				if(method=="GET" || method=="HEAD" || method=="OPTIONS"){					
					if(readers.find(fileName.c_str())!=readers.end()) readers[fileName.c_str()]++;						
					else readers[fileName.c_str()]=1;
				}
				muFile.unlock();
				*/
				
//206 range bytes
//300 bez sensu bo serwer sam wybierze domyslny sposob
				if(fileName=="oldPernament.html") resNum=301;//pernament,can change method
				//if(fileName=="oldTemporary.html") resNum=302;//temporary,can change method
				//if(fileName=="oldTemporary.html") resNum=307;//temporary,cant change method
				//if(notModified) resNum=304;
				//if(fileName=="oldPernament.html") resNum=308;//pernament,cant change method
				//if(fileName=="oldPernament.html") resNum=301;//pernament,cant change method		
				//if(fileName=="forbidden.html") resNum=403;
				if(method=="PUT" || method=="DELETE"){
					if(find(forbidden.begin(), forbidden.end(), fileName) != forbidden.end()) resNum = 403;
				}
				
				struct stat stat_buf;
				if( stat(fileName.c_str(), &stat_buf) != 0) resNum=404;//not found
	
				if(fileName=="deleted.html") resNum=410;	
				if((method=="PUT" || method=="POST") && reqLength==0) resNum=411;//length required
				//if(notModified) resNum=412;//if other than get or head				
				if(fileName=="teapot.html") resNum=418;//im a teapot
				//if(fileName=="unprocessable.html") resNum=422;
				//if(noPreconditions) resNum=428;
				//if(nRequests>5) resNum=429;
				//if(nHeaders>50) resNum=431;
				if(fileName=="illegal.html") resNum=451;		
				//if(fileName=="underConstruction.html") resNum=503;				
			}
		}
	}
	if(resNum==200){
		if(expectContinue) resNum=100;//continue
		//else resNum=methodToResCode[method];
	}else{
		if(expectContinue) resNum=417;//abort	
	}	
}

void Connection::myPut(){	
	/*
	muFile.lock();
	if(readers.find(fileName.c_str())!=readers.end() || writers.find(fileName.c_str())!=writers.end()){
		muFile.unlock();
		resNum = 503;//temporary unavaible
		return;
	}
	else{
		writers.insert(fileName.c_str());
		muFile.unlock();
	}
	*/
	if(resNum==403) return;
	struct stat stat_buf;
	if( stat(fileName.c_str(), &stat_buf) != 0) resNum=201;//created
	else{
		resNum=200;//modified
		remove(fileName.c_str());
	}
	
	int length=0;
	char *myChar = new char;
	
	int newFile = open(fileName.c_str(), O_WRONLY | O_CREAT);	
	time_t start = time(NULL);//start timer/keep alive value/how long am i willing to listen for the whole body
	while(true){	
		while(read(clientFd,myChar,1)>0){
			//printf("%c", myChar[0]);
			write(newFile,myChar,1);
			if(++length>=reqLength) {
				close(newFile);	
				
				/*
				char* bufor = new char[30];				
				sprintf(bufor, "chmod 777 %s", fileName.c_str());
				FILE* pipe = popen(bufor, "r");
				pclose(pipe);
				*/
				
				char* bufor = new char[30];	
				int n = sprintf(bufor, "chmod 777 %s", fileName.c_str());
				write(1, bufor, n);
				system(bufor);
				
				return;
			}
		}
//send msg before closing		
		if(keepAlive && difftime(time(NULL),start)>keepAliveTime){//check the timer
			//cout << "Im done with waiting\n";
//close(clientFd);
//tell client times up and im done with waiting
//delete from clientFds set
			//cout << body;
			close(newFile);
			remove(fileName.c_str());
			
			/*
			muFile.lock();
			writers.erase(fileName.c_str());
			muFile.unlock();
			*/
			
			resNum=408;//timeout
				
			return;
		}
	}
}

int Connection::readHeaders(){
	int nHeaders=0;
	int count = 0;
	int newLines = 0;
	string line = "";
	char *myChar = new char;
	time_t start = time(NULL);
	while(true){
		while(read(clientFd, myChar,1)>0){
			count++;
			line += myChar[0];
			try{
				//printf("%c", myChar[0]);
				if((int)myChar[0]==10){
					if(count>1000){
						resNum = 414;//uri too long
						return 1;
					}				
					if(++newLines==4){
						return 1;
					}
					unsigned int deli = line.find(": ");
					if(deli != string::npos){
						string key = line.substr(0, deli);
						string val = line.substr(deli+2,line.length()-deli-4);
						//printf("%s: %s\n", key.c_str(), val.c_str());						
						if(headerFunctions.find(key) != headerFunctions.end()){
							headerFunctions[key](val, *this);
							if(resNum!=200)	printf("%d %s %s\n", resNum, key.c_str(), val.c_str());
						}
						if(++nHeaders>50){
							resNum=431;	
							return 1;
						}
					}
					else{
						method = line.substr(0,line.find(" "));	
						if(line.find("HTTP/")!=string::npos){	
							if(line.find("HTTP/1.")!=string::npos){
								fileName = line.substr(line.find("/")+1,line.find("HTTP/1.")-line.find("/")-2); 
								//if(fileName.length()==0) fileName = "index.html";
								if(extension(fileName)=="folder") isFolder = true;
								if(fileName.length()==0) fileName = ".";
								printf("%d %s %s\n", clientFd, method.c_str(), fileName.c_str());		
								if(++nRequests>5) {
									resNum=429;
									return 1;
								}
							}else{
								resNum=505;//wrong http version
								return 1;
							}
						}
					}
					line = "";
					count = 0;
				}else if((int)myChar[0]==13){ if(++newLines==4) return 1; }
				else newLines = 0;				
			}catch(...){ resNum = 400;}//generic client error
		}
		if(count<0) return -1;
//send msg before closing
		if(difftime(time(NULL),start)>timeOut){//check the timer
//tell client times up and im done with waiting
//delete from clientFds set
			//cout << "Im done with waiting\n";
			resNum=408;//timeout
			return -1;
		}
	}
}

void Connection::writeHeaders(){
	int n;
	char* bufor = new char[100];
	
	string resCode = resCodes[resNum];
	
	n = sprintf(bufor, "HTTP/1.1 %d %s\n", resNum, resCode.c_str());
	write(clientFd, bufor, n);
	if(resNum!=200) write(1, bufor, n);

	n = sprintf(bufor, "Date: %s\n", getTime());
	write(clientFd, bufor, n);
	//write(1, bufor, n);
	
	n = sprintf(bufor, "Server: CustomServer2000\n");
	write(clientFd, bufor, n);
	//write(1, bufor, n);
	
	n = sprintf(bufor, "Access-Control-Allow-Origin: *\n");
	write(clientFd, bufor, n);
	//write(1, bufor, n);
	
//POST
	//n = sprintf(bufor, "Access-Control-Allow-Origin: *\n");
	//write(clientFd, bufor, n);
	//write(1, bufor, n);
	
	/*
	if((fileName=="secure.html") && !authorized){//TODO
		n = sprintf(bufor, "WWW-Authenticate: Basic realm=\"Enter the password\", charset=\"UTF-8\"\n");
		write(clientFd, bufor, n);
		write(1, bufor, n);		
		resNum=401;
	}
	*/
	
	if(method=="PUT"){
		n = sprintf(bufor, "Content-Location: /%s\n", fileName.c_str());
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Content-Length: 0\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		/*
		muFile.lock();
		writers.erase(fileName.c_str());
		muFile.unlock();
		*/
	}
	
	if(method=="OPTIONS"){
		n = sprintf(bufor, "Access-Control-Allow-Headers: *\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		//cant be set with "Access-Control-Allow-Origin: *\n"
		//n = sprintf(bufor, "Access-Control-Allow-Credentials: true\n");
		//write(clientFd, bufor, n);		
		
		n = sprintf(bufor, "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE, PUT\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Access-Control-Max-Age: 6000\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "Content-Length: 0\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		n = sprintf(bufor, "\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		/*
		muFile.lock();		
		if(--readers[fileName.c_str()]==0){
			readers.erase(fileName.c_str());
		}
		muFile.unlock();
		*/
	}
	
	if(method=="GET" || method=="HEAD"){
		n = sprintf(bufor, "Content-Language: en\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		string ext = extension(fileName);
		if(extToType.find(ext)!=extToType.end() ){
			string type = extToType[ext];
			n = sprintf(bufor, "Content-Type: %s\n", type.c_str());
			write(clientFd, bufor, n);	
			//write(1, bufor, n);
		}else{
			if(isFolder) n = sprintf(bufor, "Content-Type: text/plain\n");
			else n = sprintf(bufor, "Content-Type: */*\n");
			write(clientFd, bufor, n);	
			//write(1, bufor, n);
		}
		
		string table = "";
		if(isFolder){
			//printf("1its folder named %s\n", fileName.c_str());
			sprintf(bufor, "cd ./%s;ls -l", fileName.c_str());
			FILE* pipe = popen(bufor, "r");			
			char* bufor2 = new char[2];
			while (!feof(pipe)) {
				if (fgets(bufor2, 2, pipe) != NULL){
					table += bufor2;
				}
			}
			pclose(pipe);
		}
		
		struct stat stat_buf;
		if( stat(fileName.c_str(), &stat_buf) == 0){
			if(isFolder) n = sprintf(bufor, "Content-Length: %d\n", table.length());
			else n = sprintf(bufor, "Content-Length: %ld\n", stat_buf.st_size);
			if(resNum==403) n = sprintf(bufor, "Content-Length: 0\n");
			write(clientFd, bufor, n);	
			//write(1, bufor, n);
		}
		else {
			n = sprintf(bufor, "Content-Length: 0\n");
			write(clientFd, bufor, n);	
			//write(1, bufor, n);
		}
				
		n = sprintf(bufor, "\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		if(resNum!=403 && method=="GET"){
			char *myChar = new char;
			if(isFolder){
				for(unsigned int i=0;i<table.length();i++){
					write(clientFd, &(table.c_str()[i]), 1);
					//write(1, &(table.c_str()[i]), 1);
				}
			}
			else{
				//if(fileName=="index.txt") ls.lock();
				int res=open(fileName.c_str(), O_RDONLY);
				if(res<0){
					printf("File error with %s\n",("./"+fileName).c_str());
				}else{
					while(read(res,myChar,1)>0){
						write(clientFd, myChar, 1);
						//write(1, myChar, 1);
					}
				}
				close(res);
				//if(fileName=="index.txt") ls.unlock();
			}
		}
		
		/*
		muFile.lock();		
		if(--readers[fileName.c_str()]==0){
			readers.erase(fileName.c_str());
		}
		muFile.unlock();
		*/
	}
	
	if(method=="DELETE"){
		n = sprintf(bufor, "Content-Length: 0\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);

		n = sprintf(bufor, "\n");
		write(clientFd, bufor, n);
		//write(1, bufor, n);
		
		if(resNum==200) {
			n = sprintf(bufor, "rm -r %s", fileName.c_str());
			write(1, bufor, n);
			//system(bufor);
		}
					
	
		/*
		muFile.lock();		
		writers.erease(fileName.c_str());
		muFile.unlock();
		*/
	}
}