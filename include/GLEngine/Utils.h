#ifndef GLENGINE_UTILS_H
#define GLENGINE_UTILS_H

#include <GLEngine/Common.h>

namespace GLEngine {

using std::string;

class Utils {
    public:
    static string getExtension(const string& name) {
        string::size_type pos = name.rfind(".");
        if(pos == string::npos)
            return "";
        string ext = name.substr(pos+1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }

    static bool readFile(const string& path, const string& file, string& out) {
        std::stringstream ss;
        ss << path;
        ss << "/";
        ss << file;

        std::ifstream fp(ss.str().c_str());
        if(!fp.good())
            return false;

        std::stringstream buf;
        buf << fp.rdbuf();
        out = buf.str();
        return true;
    }
private:
    Utils();

};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_UTILS_H */
