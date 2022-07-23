#pragma once
#include <GL/glew.h>

#include "defines.h"

struct Index_Buffer
{
    Index_Buffer(void* data, uint32_t numIndices, uint8_t elementSize) {

        // Buffer auf GPU-Speicher schieben
        glGenBuffers(1, &bufferId);
        //std::cout << vertexBuffer << std::endl;
        // gebundener Buffer wird gezeichnet
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * elementSize, data, GL_STATIC_DRAW);

    }

    virtual ~Index_Buffer() {
        glDeleteBuffers(1, &bufferId);
    }

    void bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);

    }

    void unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

private:
    GLuint bufferId;

};
