#ifndef GLENGINE_UTILS_H
#define GLENGINE_UTILS_H

#include "Common.h"

namespace GLEngine {

using std::string;

class Utils {
    TAG_DEF("Utils")
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

    static string getPath(const string& name) {
        string::size_type pos = name.rfind("/");
        if(pos == string::npos)
            throw "Invalid path";
        string path = name.substr(0, pos);
        return path;
    }

    static void cleanFileName(string& name) {
        string::size_type p;
        while((p = name.find("\\")) != string::npos) {
            name[p] = '/';
        }
    }

    static std::string getUniformName(const string& arrName, size_t pos, 
                                      const string& varName) {
        std::stringstream ss;
        ss << arrName << "[" << pos << "]." << varName;
        return ss.str();
    }
private:
    Utils();
};

} /* GLEngine */ 

#endif /* ifndef GLENGINE_UTILS_H */
