Name:     Sanjay Nair
email:    sanjaysn@usc.edu
--------------------------------

What is been done?
-----------------
Everything the spec says has been implemented and reasonable (not exhaustive) testing has been done to make sure the program behaves as per the spec. During the testing, I haven't observed any severe error conditions (such as bus error, segmentation fault). 

The timeout value for the recv call is 10 seconds. That means, if there are no response from client or server, the other side will close the connection and prints error message. 

Both server and client checks for 6 byte header before proceeding (2 byte code and 4 byte data length). If any of these fields are not valid, server will send error reply (code 0xFCFE) and client will print out error message and close the connection. The data is sent one byte at a time. The server also checks for the length of client's request. If the length of data request is more than 1024, server replies with an error. 

Each request the server receives, it hands it over to a separate thread. This thread is now responsible for handling the communication with the client. Server maintains tha list of threads that is servicing the requests. When server shuts down (by invoking Ctrl+c or after shutdown timer expired), the server closes the server socket and asks the thread connections to stop (please note that it does not call destroy on thread, but asks the thread connection class to stop activities as soon as possible). The server then waits for the client threads to join and shuts down gratiously. Please note that at this point, the client threads will stop the work they are doing and close the socket (as oppose to sending error reply), since it may be possible that some part of the reply would have been already reached the client (as part of normal flow). Synchronization is done by using mutexes and conditional waits are done using condition variables.

Some improvements need to be done in the server, such as periodically scanning the queue for threads that are alrady done and removing them from the queue, sending error information to client etc. 

Language and Design
-------------------
I used C++ object oriented model wherever possible. The pthread functions are abstracted as objects for easy use. Exception are used for signaling error conditions. Communication data structure uses a stl vector of characters that is send to the other side. Integer variables are ordered in network order prior to sending over the network. 

Code Reuse
-----------
I have reused the code I developed in my earlier projects (EE450, CS530) on this project. Other than that, I used very small section of code from other sources (such as internet). All code reuse have been noted in the comment section of the code.

Some of the code reused are:

Some of the code reused are:
1. CUtil::trim method partially taken from Thinking in C++ book
2. CWaitCondition::wait method partially taken from QT code (http://www.trolltech.com)
3. Concepts and some code for CThread, CMutex and CWaitCondition classes are taked from QT code (http://www.trolltech.com).

Other than this nothing has been taken (except may be some concept / or may be one line code from internet)  from any other sources and I've developed everything for this project.

Compile instructions
-------------------
A make file has been provided to make this project.
On the commandline, just type make to build the project. Please note that you need to setup the LD_LIBRARY_PATH variable to include openssl library crypto.

Running
--------
1. Server can be run by typing ./server 
2. Client can be run by typing ./client

In case of any qustions, please do not hesitate to contact me at sanjaysn@usc.edu

