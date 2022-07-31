#include <iostream>
#include <cmath>
// GLEW holt Hardwareparameter und Funktionen aus den Grafiktreibern und gibt Funktionspointer zurück
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

// Bilder lesen stb...
#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

// GLM := OpenGL Mathe Bib
#include "libs/glm/glm/glm.hpp"
#include "libs/glm/glm/ext/matrix_transform.hpp"
#include "libs/glm/glm/gtc/matrix_transform.hpp"

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
#include "index_buffer.h"
#include "shader.h"
#include "camera.h"
//#include "fps_camera.h"
#include "floating_camera.h"

// Fehlerfunktion für Konsolenausgabe
void openGlDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    std::cout << "[OpenGL Error] " << message << std::endl;
}

#ifdef _DEBUG
// alte Errormassagehandlingfunktion
void _GLGetError(const char *file, int line, const char *call)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] " << glewGetErrorString(error) << " in " << file << " " << line << " Call: " << call << std::endl;
    }
}

// Makro für alte Errormassagehandlingfunktion
#define GLCALL(call) \
    call;            \
    _GLGetError(__FILE__, __LINE__, #call)

#else

#define GLCALL(call) call

#endif

int main(int argc, char *argv[])
{
    SDL_Window *window;
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
    // Vsync anschalten := Grafikkarte wartet, bis Monitor bereit ist
    SDL_GL_SetSwapInterval(1);

// Debug verbose
#ifdef _DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

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

#ifdef _DEBUG
    // Debugoutput aktiviert
    glEnable(GL_DEBUG_OUTPUT);
    // sofortige Benachrichtigung
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(openGlDebugCallback, 0);
#endif

    // Daten für Vertexbuffer auf GPU-Speicher
    Vertex vertices[] = {
        Vertex{-0.5f, -0.5f, 0.0f,
               0.0f, 0.0f,
               1.0f, 0.0f, 0.0f, 1.0f},
        Vertex{-0.5f, 0.5f, 0.0f,
               0.0f, 1.0f,
               0.0f, 1.0f, 0.0f, 1.0f},
        Vertex{0.5f, -0.5f, 0.0f,
               1.0f, 0.0f,
               0.0f, 0.0f, 1.0f, 1.0f},
        Vertex{0.5f, 0.5f, 0.0f,
               1.0f, 1.0f,
               1.0f, 0.0f, 0.0f, 1.0f}};
    uint32_t numVertices = 4;

    uint32_t indices[] = {
        0, 1, 2,
        1, 2, 3};
    uint32_t numIndices = 6;

    Index_Buffer indexBuffer(indices, numIndices, sizeof(indices[0]));

    Vertex_Buffer vertexBuffer(vertices, numVertices);
    vertexBuffer.unbind();

    Shader shader("shaders/basics.vs", "shaders/basics.fs");
    shader.bind();

    /*
    // Textur laden
    int32_t texturWidth = 0;
    int32_t texturHeigth = 0;
    int32_t bitsPerPixel = 0;

    // y-Achse spiegeln (openGl spezifisch)
    stbi_set_flip_vertically_on_load(true);
    // Pixeldaten laden
    auto texturBuffer = stbi_load("graphics/logo.png", &texturWidth, &texturHeigth, &bitsPerPixel, 4);

    // Daten auf GPU laden
    GLuint texturId;
    GLCALL( glGenTextures(1, &texturId) );
    GLCALL( glBindTexture(GL_TEXTURE_2D, texturId) );

    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) );
    // magnifikation bei heranzoomen von Texturen
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) );
    // Tiling := eine Textur in Array x-mal über Fläche gelegt
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE) );
    GLCALL( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE) );

    // laden der Textur in GPU-Speicher
    GLCALL( glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texturWidth, texturHeigth, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturBuffer) );
    GLCALL( glBindTexture(GL_TEXTURE_2D, 0) );

    if(texturBuffer) {
        stbi_image_free(texturBuffer);
    }*/

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;

    /*
    // Zugriff auf den Shader (CPU -> GPU) | Für globale Sachen zum Speichern oder Ändern in uniform
    int colorUniformLocation = glGetUniformLocation(shader.getShaderId(), "u_color");
    if (colorUniformLocation != -1)
    {
        GLCALL(glUniform4f(colorUniformLocation, 1.0f, 0.1f, 1.0f, 1.0f));
    }

    int textureUniformLocation = GLCALL( glGetUniformLocation(shader.getShaderId(), "u_texture") );
    if ( textureUniformLocation != -1) {
        GLCALL( glUniform1i(textureUniformLocation, 0) );
    }*/

    // Rotationsmatrix
    glm::mat4 model = glm::mat4(1.0f);
    // skalieren
    model = glm::scale(model, glm::vec3(1.0f, 2.0f, 1.0f));

    // Perspektivenwechsel
    bool ortho = false;
    // Orthogonale Projektion
    glm::mat4 orthoProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, -10.0f, 100.0f);
    glm::mat4 perspective = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 projection = perspective;

    FloatingCamera camera(90.0f, 800.0f, 600.0f);
    // Kamera nach hinten
    camera.translate(glm::vec3(0.0f, 0.0f, 5.0f));
    camera.update();

    // Kameramatrix
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));

    glm::mat4 modelViewProjection = camera.getViewProj() * model;

    // int modelMatrixLocation = GLCALL( glGetUniformLocation(shader.getShaderId(), "u_model") );
    int modelViewProjMatrixLocation = GLCALL(glGetUniformLocation(shader.getShaderId(), "u_modelViewProj"));

    float time = 0.0f;

    //Kamerageschwindigkeit [m/s]
    float cameraSpeed = 6.0f;

    // buttons
    bool buttonW = false;
    bool buttonS = false;
    bool buttonA = false;
    bool buttonD = false;
    bool buttonSpace = false;
    bool buttonShift = false;

    bool close = false;

    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (!close)
    {
        SDL_Event event;
        // Maus und Tastaturinput abfragen
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                close = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    buttonW = true;
                    break;
                case SDLK_s:
                    buttonS = true;
                    break;
                case SDLK_a:
                    buttonA = true;
                    break;
                case SDLK_d:
                    buttonD = true;
                    break;
                case SDLK_LSHIFT:
                    buttonShift = true;
                    break;
                case SDLK_SPACE:
                    buttonSpace = true;
                    break;
                }
                
            }
            else if ( event.type == SDL_KEYUP )
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    buttonW = false;
                    break;
                case SDLK_s:
                    buttonS = false;
                    break;
                case SDLK_a:
                    buttonA = false;
                    break;
                case SDLK_d:
                    buttonD = false;
                    break;
                case SDLK_LSHIFT:
                    buttonShift = false;
                    break;
                case SDLK_SPACE:
                    buttonSpace = false;
                    break;
                }
            }
            else if ( event.type == SDL_MOUSEMOTION ) 
            {
                camera.onMouseMoved(event.motion.xrel, event.motion.yrel );
            }
            /*else if ( event.type == SDL_KEYDOWN && (event.key.keysym.mod & KMOD_LALT) )
            {
                // && event.key.keysym.mod (Modifizierer "alt", "ctl" ...) & KMOD_LCTL
                if ( event.key.keysym.sym == SDLK_p ) {
                    if ( ortho ) {
                        projection = perspective;
                    } else {
                        projection = orthoProjection;
                    }
                    ortho = !ortho;
                }
            }*/
        }

        // Hintergrundfarbe
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Zeitmessung
        time += delta;

        if ( buttonW ) {
            camera.moveFront(delta * cameraSpeed);
        }
        if ( buttonS ) {
            camera.moveFront(-delta * cameraSpeed);
        }
        if ( buttonA ) {
            camera.moveSideways(-delta * cameraSpeed);
        }
        if ( buttonD ) {
            camera.moveSideways(delta * cameraSpeed);
        }
        if ( buttonSpace ) {
            camera.moveUp(delta * cameraSpeed);
        }
        if ( buttonShift ) {
            camera.moveUp(-delta * cameraSpeed);
        }

        camera.update();

        // dynamische Rotation [rad]
        model = glm::rotate(model, 1.0f * delta, glm::vec3(0, 1, 0));
        // Projektionsansicht
        modelViewProjection = camera.getViewProj() * model;
        // sinusförmig skalieren
        // model = glm::mat4(1.0f);
        // model = glm::scale(model, glm::vec3(sinf(time),1,1));

        /*if (colorUniformLocation != -1)
        {
            GLCALL(glUniform4f(colorUniformLocation, sinf(time)*sinf(time), 0.1f, 1.0f, 1.0f));
        }*/

        vertexBuffer.bind();
        indexBuffer.bind();
        GLCALL(glUniformMatrix4fv(modelViewProjMatrixLocation, 1, GL_FALSE, &modelViewProjection[0][0]));
        // 3D-Objekt zeichnen

        // Texturslot 0 nutzen für Textur
        // GLCALL( glActiveTexture(GL_TEXTURE0) );
        // Textur binden
        // GLCALL( glBindTexture(GL_TEXTURE_2D, texturId) );

        // Aufruf alter Errormassagehandlingfunktion
        GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
        // glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        indexBuffer.unbind();
        vertexBuffer.unbind();

        SDL_GL_SwapWindow(window);

        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / (float)perfCounterFrequency;
        uint32_t FPS = (uint32_t)((float)perfCounterFrequency / (float)counterElapsed);
        lastCounter = endCounter;
    }

    // Speicher der Textur freigeben
    // GLCALL( glDeleteTextures(1, &texturId) );

    return 0;
}