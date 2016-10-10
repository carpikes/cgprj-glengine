#ifndef GLENGINE_INPUTHANDLER_H
#define GLENGINE_INPUTHANDLER_H

namespace GLEngine
{

class InputHandler {
public:
    virtual void handleKeyPress(int key, int scancode, int action, int mods) = 0;
    virtual void handleMouseMove(double xpos, double ypos) = 0;
};
    
} /* GLEngine */ 

#endif /* ifndef GLENGINE_INPUTHANDLER_H */
