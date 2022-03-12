#include <iostream>

// GLM:
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Glew
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>
#include "engine.h"
// FreeImage
#include "FreeImage.h"

//Id finestra
int windowId;

//Matrici di proiezione
glm::mat4 projection;
glm::mat4 ortho;

//Fps
int fps;
int frames;

static Engine::Handler handler;

#ifdef _WINDOWS
#include <Windows.h>

int APIENTRY DllMain(HANDLE instDLL, DWORD reason, LPVOID _reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        break;


    case DLL_PROCESS_DETACH:
        break;
    }

    return true;
}
#endif

void displayCallback()
{

}

void reshapeCallback(int width, int height)
{
    //Setto matrice di proiezione
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 1000.0f);
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    glLoadMatrixf(glm::value_ptr(projection));

    //Reimposto in ModelView
    glMatrixMode(GL_MODELVIEW);
}

void specialCallback(int key, int mouseX, int mouseY)
{
    handler.special(key, mouseX, mouseY);
}

void keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    handler.keyboard(key, mouseX, mouseY);
}

void closeCallback()
{
    handler.close();
}

void mouseMove(int mouseX, int mouseY) {
    handler.mouse(mouseX, mouseY);
}

bool LIB_API Engine::init(Handler t_handler) {
    //Inizializzazioni di glut
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(150, 150);
    glutInitWindowSize(handler.width, handler.height);

    //default per inizializzazione glut
    char* argv[1];
    int argc = 1;

    #ifdef _WINDOWS
    argv[0] = _strdup("Application");
    #else
    argv[0] = strdup("Application");
    #endif

    //Inizializzazione glut
    glutInit(&argc, argv);

    //Flag opzionali
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    //Settaggio handler per callbacks
    handler = t_handler;

    //Creazione window e contesto OpenGL
    windowId = glutCreateWindow("VR Robotic Arm");

    //Utilizzo versione più aggiornata OpenGL
    glewExperimental = GL_TRUE;
    glewInit();

    // OpenGL 2.1 is required:
    if (!glewIsSupported("GL_VERSION_2_1"))
    {
        std::cout << "OpenGL 2.1 not supported" << std::endl;
        return false;
    }

    //Lighting
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);
    glm::vec4 gAmbient(0.2f, 0.2f, 0.2f, 1.0f);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, glm::value_ptr(gAmbient));

    //OpenGL enables
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    //Callbacks
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);
    glutCloseFunc(closeCallback);
    glutPassiveMotionFunc(mouseMove);

    //Inizializza lettore texture
    FreeImage_Initialise();

    //Dice a OpenGL di cercare nella VRAM
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    return true;
}

void LIB_API Engine::clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LIB_API Engine::render(const List& list, std::shared_ptr<Camera> camera)
{
    glMatrixMode(GL_MODELVIEW);
    for (int i = 0; i < list.size(); i++) {
        list[i].first->render(camera->inverseCamera()*list[i].second);
        Mesh* mesh = dynamic_cast<Mesh*>(list[i].first.get());
        if(mesh)
            if(mesh->shadow())
                mesh->render_shadow(camera->inverseCamera()*mesh->get_shadow_mat()*list[i].second);
    }
}

void LIB_API Engine::swap()
{
    glutSwapBuffers();
}

std::shared_ptr<Node LIB_API> Engine::load(std::string file) {
    ObjectLoader object_loader;
    return object_loader.LoadScene(file);
}

void LIB_API Engine::free()
{
    FreeImage_DeInitialise();
}

void LIB_API Engine::update()
{
    glutMainLoopEvent();
}

void LIB_API Engine::drawText(const std::string& text,float x,float y)
{
    //Setto matrice di proiezione
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(ortho));

    //Setto matrice di model view
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    // Disable lighting before rendering 2D text:
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    // Write some text:
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)text.c_str());

    // Reactivate lighting:
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Reimposto proiezione
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(projection));
}
