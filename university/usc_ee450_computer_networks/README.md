EE 450 Project - summer 2006
-----------------------------

A. Name:          Sanjay Nair
B. Introduction
---------------
This assignment is written using C++ (using standard C++ STL libraries). In this assignment, I used the socket API to program to the given project spec. All features required by the project spec have been implemented. The implementation package consists of 4 packages, 3 executables and 1 shared library. The shared library contains common code that is shared by all executable projects. 

The three executable are:
1. aserver - Authentication server
2. userver - Upload server
3. client  - Client

They all share a library named util.

Authentication sever
-------------------
Authentication server is implemented in an executable called aserver. Upon startup, this server will bind to a pre-defined port 10041. This port is hard-coded in the application. It will look for a file name login to read all login/password combinations. If this file does not exist, it will print out error message and exit. Please note that only one authentication server could be run on a single machine interface. Rest of the server behavior follows what is in the spec.

The login information is entered as name-value pairs. Each login/password entry is separated by "=" character and multiple login/password pairs are inserted after a new line. Please take a look at the login file that comes with the package for more information.

Upload server
-------------
Upload server is implemented in an executable called userver. The upload server needs a command line parameter known as id that needs to be supplied by the user. This id can be either 1 or 2 that identifies an upload server. Note that this number can be easily changed in the code to allow more than 2 upload servers. For this project, this number is hard-coded to 2.

Upon startup, this server opens a TCP port and then it will try to contact the authentication server using UDP protocol. If the authentication server could not be contacted, this server prints out the message and exit. If the server could be contacted, it sends its id, ip, port and lock value to the server. It then waits for the client to connect. Once the client connects, it follows the requirements outlined in the spec. 

Note: UDP timeout is not implemented in this project. If the authentication server is not up and running, program may wait indefinitely. You can kill it by pressing Ctrl C. Also, some shortcut file paths may not be understandable by this program. You can always pass relative path or absolute path to save this fie. Please not that this program will not create non-existent directories.

Client
------
Client is implemented in a executable called client. Client gets inputs from users and performs operation on behalf of the user. The client follows the requirements outlined in the spec.

Note: The client program will reject the user inputs if the input contains reserved characters (such as , and =). These characters are used by protocol. This is identified as one area for improvement.

C. Source files
---------------
1.  cabstractfstream.h  - Encapsulates the I/O stream. 
                          Manly used for abstracting input and output file streams.
2.  cabstractfstream.cc - Implementation of cabstractfstream.h
3.  cabstractsocket.cc  - Encapsulates socket object. 
                          Contains commonly used methods for TCP and UDP sockets.
4.  cabstractsocket.h   - Implementation of cabstractsocket.cc
5.  cconstant.h         - Constants shared by different apps. 
                          (To avoid hard coding of constants) 
6.  cconstant.cc        - Implementation of cconstant.cc
7.  cexception.h        - Class that encapsulates C++ exception. 
                          The object of this class is thrown for all exceptions.
8.  cexception.cc       - Implementation of cexception.h
9.  cfile.h             - Encapsulation of file object.
10. cfile.cc            - Implementation of cfile.h
11. cglobal.h           - Global settings and typedefs
12. cifstream.h         - Encapsulates Input file stream for reading data from files.
13. cifstream.cc        - Implementation of cifstream.h
14. cofstream.h         - Encapsulates Output file stream for writing data to files.
15. cofstream.cc        - Implementation of cofstream.h
16. ctcpsocket.h        - Encapsulates a TCP socket. Both server and client
                          methods are included here.
17. ctcpsocket.cc       - Implementation of ctcpsocket.h
18. cudpsocket.h        - Encapsulates the UDP socket. 
19. cudpsocket.cc       - Implementation of cudpsocket.h
20. cutil.h             - Common utility methods are included here.
21. cutil.cc            - Implementation of cutil.h
22. caserver.h          - Implementation of authenticaion server. 
23. caserver.cc         - Implementation of caserver.h
24. caservermain.cc     - Main function that starts the authentication server.
25. cuserver.h          - Implementation of upload server.
26. cuserver.cc         - Implementation of cuserver.h
27. cuservermain.cc     - Main function that starts the upload server.
28. cclient.h           - Implementation of upload client.
29. cclient.cc          - Implementation of cclient.h
30. cclientmain.cc      - Main function that starts the client.

Additional Files
----------------
31. MakeFile            - Unix make file that builds this project
33. login               - sample login file that contains 
                          some login/password combinations.
33. README              - This file

D. Building and running the program
-----------------------------------
Once you unzipped and un-tarred the project, you can build the project by typing make in the command line shell. 

commands
---------
gunzip nairsanjay.tar.gz
tar xvf nairsanjay.tar


Please make sure that your shell have the g++ and socket library paths.
g++ version I'm using is: gcc version 3.3.2
Your path and LD_LIBRARY path should contain library path for socket and nsl libraries.
If you find any issues with compiling and linking, please do contact me (nairsanjay@usc.edu). I have compiled, linked and ran the code in both windows and Solaris (nunki.usc.edu) successfully.

1. To run authentication server, type ./aserver in  the shell
2. To run upload server, type ./userver (id) in the shell
3, To run client program, type ./client in the shell.

E. Message format exchanged
------------------------------
The message format exchanged between different communicating entities uses  simple string format. Individual messages consist of name-value pairs (NVP) separated by "," character. Each NVP is separated by the "=" character. Users are not allowed to user these characters as they might interfere with the message format. A sample message format between upload server and authentication server is: 

c_from=from_userver,c_ip=192.168.1.103,c_lock=20,c_port=13,c_userverid=1


F. Idiosyncrasies
-----------------
The project uses gethostname to get the current system host name. Upon getting a valid name it uses gethostbyname API to obtatin the IP addresses corresponds to the name. If there are multiple IP addresses, it picks the first one. It will be a problem if the server starts in one interface and client tries to establish a connection in other interface. Although this scenario is very possible in real life scenario, it is unlikely going to happen in this project as all applications are going to be running from the same system. This can be easily fixed by allowing the user to input the IP address of the host it wants to bind and connect to.

F. Reused code
-----------------
Only one function is reused in this project. The function is named trim and located in cutil.cc file. This snippet of code is taken from the Thinking in C++ book (the file clearly states this). Everything else is coded by me.

