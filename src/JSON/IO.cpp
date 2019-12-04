#include "IO.h"

#ifdef __linux__
    #include <stdio.h>
    #include <unistd.h>
#endif

namespace ffGraph {
namespace JSON {
namespace IO {

std::string GetTmpFile(int ID)
{
#ifdef __linux__
    int pid = getpid();
    std::string n(P_tmpdir);
    n.append("/ffGraph-");
    n.append(std::to_string(pid));
    n.append("-");
    n.append(std::to_string(ID));

    return n;
#endif

}

}
}
}