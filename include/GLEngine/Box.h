#ifndef GLENGINE_BOX_H
#define GLENGINE_BOX_H

#include <GLEngine/Common.h>

namespace GLEngine
{
    
struct Box {
    // xmin,xmax,ymin,ymax,zmin,zmax
    float vmin[3], vmax[3];
    bool empty;
    Box() { 
        empty = true;
        for(int i=0;i<3;i++) { 
            vmin[i] = 0.0f; 
            vmax[i] = 0.0f;
        }
    }

    void clear() {
        empty = true;
    }

    void merge(const Box& b) {
        for(int i=0;i<3;i++) {
            vmin[i] = (vmin[i] > b.vmin[i]) ? b.vmin[i] : vmin[i];
            vmax[i] = (vmax[i] < b.vmax[i]) ? b.vmax[i] : vmax[i];
        }
    }

    void add(const glm::vec3& v) {
        vmin[0] = (empty || v.x < vmin[0]) ? v.x : vmin[0];
        vmin[1] = (empty || v.y < vmin[1]) ? v.y : vmin[1];
        vmin[2] = (empty || v.z < vmin[2]) ? v.z : vmin[2];

        vmax[0] = (empty || v.x > vmax[0]) ? v.x : vmax[0];
        vmax[1] = (empty || v.y > vmax[1]) ? v.y : vmax[1];
        vmax[2] = (empty || v.z > vmax[2]) ? v.z : vmax[2];
    }
};

    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_BOX_H */
