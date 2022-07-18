#pragma once
#include <GL/glew.h>

#include "defines.h"

struct Vertex_Buffer
{
    Vertex_Buffer(void* data, uint32_t numVertices) {

        // Vertex array object erstellen und binden
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Buffer auf GPU-Speicher schieben
        glGenBuffers(1, &bufferId);
        //std::cout << vertexBuffer << std::endl;
        // gebundener Buffer wird gezeichnet
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

        // OpenGL Datentyp übergeben
        glEnableVertexAttribArray(0);
        // 3D-Objekt, was im Array übergeben wird (Anzahl float in Vertex, )
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(struct Vertex, x));

        glBindVertexArray(0);
    }

    virtual ~Vertex_Buffer() {
        glDeleteBuffers(1, &bufferId);
    }

    void bind() {
        /* ~Ohne VAO
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(struct Vertex, x));
        */

       // mit VAO
       glBindVertexArray(vao);
    }

    void unbind() {
        /* ~Ohne VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        */

       // mit VAO
       glBindVertexArray(0);
    }

private:
    GLuint bufferId;
    // Vertex Array Object
    GLuint vao;
};
