int startListening(char* port){
	struct sockaddr_in x, xClient;
    socklen_t mySize = sizeof(x);
    x.sin_family = AF_INET;
    x.sin_addr.s_addr = INADDR_ANY;
    x.sin_port = htons(atoi(port));
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock<0){
		cout << "Error occured trying to create socket" << endl;
		return -1;
	}
    const int one = 1;	
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	/*
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one));
	linger l;
	l.l_onoff=1;
	l.l_linger=3;
	setsockopt(sock, SOL_SOCKET, SO_LINGER, &l, sizeof(l));
	*/
    int binding = bind(sock,(struct sockaddr*)&x, mySize);
	if(binding<0){
		cout << "Error occured trying to bind socket" << endl;
		return -2;
	}
    int listening = listen(sock, 1);
	if(listening<0){
		cout << "Error occured trying to listen to socket" << endl;
		return -3;
	}
	int client = accept(sock,(struct sockaddr*)&xClient,&mySize);   
	if(client<0){
		cout << "Error occured trying to accept new connection" << endl;
		return -4;
	}
	fcntl(client, F_SETFL, O_NONBLOCK, 1);
    printf("connection from %s : %d\n", inet_ntoa((struct in_addr)xClient.sin_addr), ntohs(xClient.sin_port)); 

	sleep(1);
	readHeaders(client);
	client = accept(sock,(struct sockaddr*)&xClient,&mySize);
	sleep(1);
	readHeaders(client);

	return client;
}

int myClose(int _client){
	if(shutdown(_client, SHUT_RDWR)<0){
		cout << "Error occured trying to shut down socket" << endl;
		return -7;
	}
	if(close(_client)<0){
		cout << "Error occured trying to close socket" << endl;
		return -8;
	}
	return 0;
}