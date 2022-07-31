#pragma once

// GLM := OpenGL Mathe Bib
#include "libs/glm/glm/glm.hpp"
#include "libs/glm/glm/ext/matrix_transform.hpp"
#include "libs/glm/glm/gtc/matrix_transform.hpp"

class Camera
{

public:
    Camera(float fov, float width, float heigth) {
        projection = glm::perspective(fov / 2.0f, width / heigth, 0.1f, 1000.0f);
        view = glm::mat4(1.0f);
        position = glm::vec3(0.0f);
        update();
    }

    glm::mat4 getViewProj() {
        return viewProj;
    }

    virtual void update() {
        viewProj = projection * view;
    }

    virtual void translate(glm::vec3 v) {
        position += v;
        // benötigt inverse Matrix
        view = glm::translate(view, v * -1.0f);
        
    } 

protected:
    // Kameraposition
    glm::vec3 position;
    glm::mat4 projection, view, viewProj;
};

