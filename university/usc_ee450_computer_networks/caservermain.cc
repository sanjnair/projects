#include "cexception.h"
#include "cutil.h"
#include "caserver.h"

int main(int argc, char *argv[]) {

    CAServer s;
    
    try {
        s.start();
    } catch (CException &e) {
        string msg(e.getMessage());
        msg += "\n\nExiting the application.";
        CUtil::writeError(msg);
        return -1;
    }
    
    return 0;
}


