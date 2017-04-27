Sanjay Nair (sanjaysn@usc.edu)
-----------------------------

Compler requirements
---------------------
Implementation of this project is done using C and C++. The implemenation uses std library classes such as std::string and std::map. My nunki account (default setup by ISD) points to the g++ directory /usr/usc/gnu/gcc/3.3.2. 

NOTE:
-----
Normally your shell should point to g++ and standard C++ libaries. You have to do the following step ONLY if your setup does not point to g++ and standard c+ libraries. 

Make sure that your path environment variable points to openssl lib
setenv LD_LIBRARY_PATH /home/scf-22/csci551b/openssl/lib:/lib

-----------------------------------------------
If you don't have the g++ and std library path in your PATH variable, you have to do the following:

Include the following in your .cshrc file

if($?LD_LIBRARY_PATH) then
setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:/usr/usc/gnu/gcc/3.3.2/lib
else
setenv LD_LIBARY_PATH /usr/usc/gnu/gcc/3.3.2/lib
endif

setenv PATH /usr/usc/gnu/gcc/3.3.2//bin:${PATH}


Design details
--------------
The project is implemented using object oriented methods. Main method is implemented in a file called hw4.cc which is responsible for parsing the input. A Basedump class is created to abstract the dumps. Derived classes such as SignDump, VerifyDump etc. overrides the dump method to provide specific behavior. This way hw4 can create appropriate object based on the input option and call dump on the correct object. The dump method on the called object will perform appropriate dumps.

