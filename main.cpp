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
    }

    uint64_t perfCounterFrequency = SDL_GetPerformanceFrequency();
    uint64_t lastCounter = SDL_GetPerformanceCounter();
    float delta = 0.0f;

    // Zugriff auf den Shader (CPU -> GPU) | Für globale Sachen zum Speichern oder Ändern in uniform
    int colorUniformLocation = glGetUniformLocation(shader.getShaderId(), "u_color");
    if (colorUniformLocation != -1)
    {
        GLCALL(glUniform4f(colorUniformLocation, 1.0f, 0.1f, 1.0f, 1.0f));
    }

    int textureUniformLocation = GLCALL( glGetUniformLocation(shader.getShaderId(), "u_texture") );
    if ( textureUniformLocation != -1) {
        GLCALL( glUniform1i(textureUniformLocation, 0) );
    }

    float time = 0.0f;

    bool close = false;
    while (!close)
    {
        // Hintergrundfarbe
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Zeitmessung
        time += delta;

        if (colorUniformLocation != -1)
        {
            GLCALL(glUniform4f(colorUniformLocation, sinf(time)*sinf(time), 0.1f, 1.0f, 1.0f));
        }

        vertexBuffer.bind();
        indexBuffer.bind();
        // 3D-Objekt zeichnen

        // Texturslot 0 nutzen für Textur
        GLCALL( glActiveTexture(GL_TEXTURE0) );
        // Textur binden
        GLCALL( glBindTexture(GL_TEXTURE_2D, texturId) );

        // Aufruf alter Errormassagehandlingfunktion
        // GLCALL( glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0) );
        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
        indexBuffer.unbind();
        vertexBuffer.unbind();

        SDL_GL_SwapWindow(window);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                close = true;
            }
        }

        uint64_t endCounter = SDL_GetPerformanceCounter();
        uint64_t counterElapsed = endCounter - lastCounter;
        delta = ((float)counterElapsed) / (float)perfCounterFrequency;
        uint32_t FPS = (uint32_t)((float)perfCounterFrequency / (float)counterElapsed);
        lastCounter = endCounter;
    }

    // Speicher der Textur freigeben
    GLCALL( glDeleteTextures(1, &texturId) );

    return 0;
}