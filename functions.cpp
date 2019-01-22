#include <vector>
#include <algorithm>

using namespace std;

bool searchVector(string val, vector<string> v){
	unsigned int coma = val.find(',');
	unsigned int colon = val.find(';');
	string token;
	while (coma!=string::npos) {
		if(colon!=string::npos && coma>colon) {//is "and" significant slower than "or"? 
			token = val.substr(0, colon);
			if(find(v.begin(), v.end(), token) != v.end()) return true;
			val.erase(0, coma + 1);
		}
		else{
			token = val.substr(0, coma);
			if(find(v.begin(), v.end(), token) != v.end()) return true;
			val.erase(0, coma + 1);
		}
		coma = val.find(',');
		colon = val.find(';');
	}
	if(colon!=string::npos){
		val = val.substr(0, colon);
	}
	if(find(v.begin(), v.end(), val) != v.end()) return true;
	return false;
}

/*
string extractRegex(string val, string sRegex){
	regex myRegex(sRegex);
	smatch myMatch; 
    if (regex_match(val, myMatch, myRegex)) {
		if (myMatch.size() == 2) {
			return myMatch[1].str();
		}else return "-1";
    }else return "-1";
}
*/

string extract(string val, string field){
	unsigned int deli = val.find(field+'=');
	if(deli!=string::npos) {
		val.erase(0, deli+field.length()+1);
		if((deli = val.find(','))!=string::npos) val.erase(deli, val.length()+1);
		return val;
	}
	else return "error";
}

string extension(string filename){
	unsigned int deli = filename.find_last_of('.');
	if(deli != string::npos){
		string ext = filename.substr(deli+1,filename.length()-deli);
		return ext;			
	} else return "folder";
}

char* getTime(){
	time_t now;
	time(&now);
	struct tm *t = gmtime(&now);
	char* godzina = new char[30];
	strftime( godzina, 30, "%a, %d %b %Y %X GMT", t );
	return godzina;
}