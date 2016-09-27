#ifndef GLENGINE_UTILS_H
#define GLENGINE_UTILS_H

#include <GLEngine/Common.h>

namespace GLEngine {
namespace Utils {

using std::string;

string getExtension(const string& name) {
    string::size_type pos = name.rfind(".");
    if(pos == string::npos)
        return "";
    string ext = name.substr(pos+1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

bool readFile(const string& file, string& out) {
    std::ifstream fp(file.c_str());
    if(!fp.good())
        return false;

    std::stringstream buf;
    buf << fp.rdbuf();
    out = buf.str();
    return true;
}

} /* Utils */
} /* GLEngine */ 

#endif /* ifndef GLENGINE_UTILS_H */
