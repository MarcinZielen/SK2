#include <vector>
#include <map>

using namespace std;

bool notModified=false;
bool noPreconditions=false;
int timeOut=10;

vector<string> methods = {"GET","PUT","HEAD","DELETE","OPTIONS","CONNECT","TRACE","PATCH","POST"};
vector<string> badMethods = {"CONNECT","TRACE","PATCH","POST"};
vector<string> charsets = {"utf-8", "*"};
//vector<string> encodings = {"identity", "*"};
vector<string> languages = {"pl", "pl-PL", "en", "en-US", "*"};
vector<string> MIMEtypes = {
	"text/html", "text/css", "text/plain", "text/xml", "text/*",
	"image/gif", "image/png", "image/jpeg", "image/bmp", "image/webp", "image/svg+xml", "image/*",
	"audio/midi", "audio/mpeg", "audio/webm", "audio/wav", "audio/*",
	"video/webm", "video/*",
	"application/javascript", "application/pdf", "application/*",
	"*/*"
};
vector<string> forbidden = {"x.cpp", "index.html", "server.cpp"};

map<string, string> extToType={
	{"txt", "text/plain"},
	{"xml", "text/xml"},
	{"html", "text/html"},
	{"css", "text/css"},
	{"gif", "image/gif"},
	{"png", "image/png"},
	{"jpeg", "image/jpeg"},
	{"jpg", "image/jpeg"},
	{"bmp", "image/bmp"},
	{"webp", "image/webp"},
	{"svg", "image/svg+xml"},
	{"midi", "audio/midi"},
	{"mp3", "audio/mpeg"},
	{"wav", "audio/wav"},
	{"webm", "video/webm"},
	{"js", "application/javascript"},
	{"pdf", "application/pdf"}
};
map<int, string> resCodes={
	{100, "Continue"},
	{101, "Switching Protocols"},
	{110, "Connection Timed Out"},
	{111, "Connection refused"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{306, "Switch Proxy"},
	{307, "Too many redirects"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length required"},
	{412, "Precondition Failed"},
	{413, "Request Entity Too Large"},
	{414, "Request-URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Requested Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{422, "Unprocessable Entity"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{509, "Bandwidth Limit Exceeded"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"}
};
map<string, int> methodToResCode={
	{"GET", 200},
	{"HEAD", 204},//no content,no refresh	
	{"OPTIONS", 204},//no content,no refresh	
	{"DELETE", 205}//no content, refresh
	//{"PUT", 200}//if put modifies,sends back resource
	//{"PUT", 201}//created,link to resource
};