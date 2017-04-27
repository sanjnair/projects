Name:     Sanjay Nair
email:    sanjaysn@usc.edu
--------------------------------

What has been done?
-----------------
Everything the spec says has been implemented and reasonable (not exhaustive) testing has been done to make sure the program behaves as per the spec. During the testing, I haven't observed any severe error conditions (such as bus error, segmentation fault etc.). 

Limits to input parameters are enforced so that the program can handle reasonable user inputs. The program will output error if the input parameters does not fit within min and max values. The program also takes care of the book-keeping time (the time spent on keeping internal data structures updated) and sleeps accordingly. A Customer datastructure is created to encapsulate the various stages (and times) the customer goes through. Standard deviation is calculated using the Deviation method (http://www.med.umkc.edu/tlwbiostats/variability.html).

Language and Design
-------------------
I used C++ object oriented model wherever possible. The pthread functions are abstracted as objects for easy use. Exception are used for signaling error conditions. The Queue is implemented as STL list and the Queue operations are protected with a Mutex. 

Code Reuse
-----------
I have reused the code I developed in my earlier projects (EE450, CS530) on this project. Other than that, I used very small section of code from other sources (such as internet). All code reuse have been noted in the comment section of the code.

Some of the code reused are:

Some of the code reused are:
1. CUtil::trim method partially taken from Thinking in C++ book
2. CWaitCondition::wait method partially taken from QT code (http://www.trolltech.com)
3. Concepts and some code for CThread, CMutex and CWaitCondition classes are taked from QT code (http://www.trolltech.com).
4. CUtil::getDiffMillis method partially taken from The GNU C Library
  http://www.delorie.com/gnu/docs/glibc/libc_428.html. However, calls to this method has not been made from anywhere. I may use this method in the future projects.


Other than this nothing has been taken (except may be some concept / or may be one line code from internet)  from any other sources and I've developed everything for this project.

Compile instructions
-------------------
A make file has been provided to make this project.
On the commandline, just type make to build the project. Please note that you need to setup the LD_LIBRARY_PATH path according to the project spec.

Running
--------
1. Start running the program by typing ./mm2 in the command shell. Optional parameters can be supplied (according to the spec) to the prorgram.

In case of any qustions, please do not hesitate to contact me at sanjaysn@usc.edu

