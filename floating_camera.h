#include "fps_camera.h"

class FloatingCamera : public FPSCamera
{
public:
    FloatingCamera(float fov, float width, float heigth) : FPSCamera(fov, width, heigth) {

    }
    void moveUp(float amount) {
        translate(up * amount);
    }
};

