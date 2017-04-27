#include "cexception.h"
#include "cutil.h"
#include "cuserver.h"

void printUsage() {
    string msg("Usage: userver id\nId must be either 1 or 2");
    CUtil::writeOutputLn(msg);
}


/*!
    Program's main function. 
*/
int main(int argc, char *argv[]) {
    
    int status = 0;

    if (argc != 2) {
        printUsage();
        status = -1;
    }

    int id = 0;
    if (0 == status) {
        try {
            id = CUtil::getUInt(string(argv[1]));
        } catch (CException &e) {
            CUtil::writeError(e.toString());
            printUsage();
            status = -1;
        }   
    }

    if (0 == status) {
        if ((id != 1) && (id != 2)) {
            CUtil::writeError("Invalid Server Id entered");
            printUsage();
            status = -1;
        }
    }


    if (0 == status) {
        CUServer s(id);
    
        try {
            s.start();
        } catch (CException &e) {
            string msg(e.getMessage());
            msg += "\n\nExiting the application.\n";
            CUtil::writeError(msg);
            status = -1;
        }
    }
    
    return status;
}
