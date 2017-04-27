#include "cexception.h"
#include "cutil.h"
#include "cclient.h"

/*!
    Program's main function.    
*/
int main(int argc, char *argv[]) {
    
    int status = 0;

    CClient c;

    try {
        c.start();
    } catch (CException &e) {
        string msg(e.getMessage());
        msg += "\n\nExiting the application.\n";
        CUtil::writeError(msg);
        status = -1;
    }
    
    return status;
}


