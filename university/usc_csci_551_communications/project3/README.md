Name: Sanjay Nair
Final Project - Part 2
****************************

Design
------
The central part of the part 2 project is the mini file system. The mini file system conists of the following files:

1. cert.pem		- Certificate file
2. private.pem          - Private key file
3. name_index           - list of file names and corresponding file numbers.
4. kwrd_index		- list of keywords and corresponding file numbers.
5. sha1_index		- list of sha1 values and corresponding file numbers.
6. file_index 		- File that keeps track of the cache space, data space and file number.
7. files                - Directory where all the permenant files and data files are stored.

When the system starts, it generates a public/private pair of keys and stores it in the mini file system. For every store, a certiciate file is also stored along with the data/cache file. This certificate file is used to verify the owner of the file in response to a delete request. An LRU algorithm is implemented for the cache file system. This algorithm replaces old files (least accessed) in order to make room for new files (if necessary).

Please refer to the part 1 README document for list of threads and other design details. Part 1 detals are not included here for brevity. If you need to get hold of this README file, please send me an email.

Index file data structure
-------------------------
Index file data structure is basically a hash table maps to list of file numbers. For example, if there are 4 files with same name, say x. the the hash table will have only one entry for x. The key is x and the value is list of 4 file numbers. This way the look up is very fast for a given file. Similar concept is applied for kwrd_index file and sha1_index file. For finding a particular entry, you don't have to traverse the entire list indexes.

Implementation
-------------
The code is developed using C++ and almost entirely developed by me (utility function such as trim has been borrowed from the Think in C++ book). The code comments clearly indicates the function(s) that has been taken from other sources. For example, the code for sacndir is developed by Joerg-R. Hill and the code clearly indicates the copyright notice and code origin(location). This function is needed in solaris as solaris does not implement the scandir function. 

Logging
-------
All required items are stored in the log files. Additional items such as error conditions, debug information etc. is also stored in the same log file. For example, if the node performs an LRU operation/ delete method, details are logged into the log file, not to the console. 

Build Instructions
------------------
You need to have pthreads library and openssl library in the path (LD_LIBRARY_PATH). A Makefile is provided for making the application. The output of the Make process is an executable named sv_node. The code has been tested on nunki.usc.edu system.
If you encounter any build issues, please donot hesitate to contact me (sanjaysn@usc.edu).

