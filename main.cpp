#include <iostream>
// GLEW holt Hardwareparameter und Funktionen aus den Grafiktreibern und gibt Funktionspointer zurück
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <SDL.h>
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "opengl32.lib")
#else
//#include <SDL2/SDL.h>
#endif

#include "defines.h"
#include "vertex_buffer.h"
#include "shader.h"


int main(int argc, char *argv[]) {
    SDL_Window* window;
    SDL_Init(SDL_INIT_EVERYTHING);

    // 8 Bit in Farbkanal (0 - 255)
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    // Größe eines Buffers
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    // Doublebuffering an
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    window = SDL_CreateWindow("C++ OpenGL Tut", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    // init Glew -> Funktionszeiger initialisieren
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error" << glewGetErrorString(err) << std::endl;
        std::cin.get();
        return -1;
    }
    
    std::cout << "OpenGL-Version:" << glGetString(GL_VERSION) << std::endl;

    // Daten für Vertexbuffer auf GPU-Speicher 
    Vertex vertices[] = {
        Vertex{-0.5f, -0.5f, 0.0f},
        Vertex{0.0f, 0.5f, 0.0f},
        Vertex{0.5f, -0.5f, 0.0f}
    };
    uint32_t numVertices = 3;

    Vertex_Buffer vertexBuffer(vertices, numVertices);
    //vertexBuffer.unbind();

    Shader shader("shaders/basics.vs", "shaders/basics.fs");
    shader.bind();

    bool close = false;
    while (!close)
    {
        // Hintergrundfarbe
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        vertexBuffer.bind();
        // 3D-Objekt zeichnen
        glDrawArrays(GL_TRIANGLES, 0, numVertices);

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT) {
                close = true;
            }
        }
            
    }
    
    
    return 0;
}