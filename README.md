# Computer Networks 2 project
Final project for the course of Computer Networks 2  
Authors: Bartosz Paulewicz, Marcin Zielen

## Topic: Simple RFC 2616-compliant HTTP server

The server is implemented in C++ and uses separate files with functions and constants.
It supports GET, HEAD, OPTIONS, PUT and DELETE methods.
The client is a web application. The graphical interface allows to create, edit and delete each file and folder
located in the root directory (with some exceptions in the form of e.g. a server file) with a single click.

Procedure of the server compilation and startup is included in the main file. Run "make" command in the console in the subdirectory with the server.
To connect to the server, enter the IP address of the computer with the server and the port on which in the browser
works (port 1234 by default), plus "/index.html", for example: "192.168.10.2:1234/index.html".

| File | Description |
| --- | --- |
| server.cpp | Server |
| Connection.cpp | Connection class and related functions for handling connections |
| constants.cpp | Constants, vectors, dictionaries, global variables common to all connections |
| functions.cpp | Global helper functions |
| index.html | Application main view |
| scripts.js | Application view control file |
| styles.css | Describes the appearance |
| Makefile | Build directives file |
