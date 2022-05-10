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
#include "Shader.h"
#include "FreeImage.h"
#include "ObjectLoader.h"
#include "ShaderSetup.h"
#include "Skybox.h"
#include "leap.h"


//Id finestra
int windowId;

//Matrici di proiezione
glm::mat4 projection;
glm::mat4 ortho;

//Fps
int fps;
int frames;

static Engine::Handler handler;
static ShaderSettings shader;

std::shared_ptr<ShaderSetup> shaderSetup;


//total lights
ObjectLoader::LightsType total_lights;

bool isVirtual = false;
bool isLeap = false;

std::shared_ptr<Skybox> skybox;
std::shared_ptr<Leap> leap;


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

void createVoidTexture() {
    glBindTexture(GL_TEXTURE_2D, 0);


    // Set circular coordinates:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set min/mag filters:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

    //Filtro anisotropico

    int intFormat = GL_COMPRESSED_RGB;
    GLenum extFormat = GL_BGR;

    char image[1];
    image[0] = 1;
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap), GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(bitmap));
    glTexImage2D(GL_TEXTURE_2D, 0, intFormat, 1, 1, 0, extFormat, GL_UNSIGNED_BYTE, (void*)image);
    //Unloda texture

    //Disattivo bind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void reshapeCallback(int width, int height)
{
    //Setto matrice di proiezione
    glViewport(0, 0, width, height);
    //glMatrixMode(GL_PROJECTION);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 1.0f, 1000.0f);
    ortho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    //glLoadMatrixf(glm::value_ptr(projection));
    shader.m_shader->setMatrix(shader.projection, projection);

    //Reimposto in ModelView
    //glMatrixMode(GL_MODELVIEW);
}

void specialCallback(int key, int mouseX, int mouseY)
{
    handler.special(key, mouseX, mouseY);
}
glm::vec4 cameraPos;
glm::mat4 headPos;

void keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    handler.keyboard(key, mouseX, mouseY);
    if (isVirtual) {
        switch (key) {
        case 'w':
            cameraPos = cameraPos - headPos[2];
            break;

        case 's':
            cameraPos = cameraPos + headPos[2];

            break;

        case 'a':
            cameraPos = cameraPos + glm::vec4(glm::normalize(glm::cross(glm::vec3(headPos[2]), glm::vec3(headPos[1]))),0.0f);

            break;

        case 'd':
            //Movimento a destra
            cameraPos = cameraPos - glm::vec4(glm::normalize(glm::cross(glm::vec3(headPos[2]), glm::vec3(headPos[1]))), 0.0f);

            break;
        case 'u':
            cameraPos = cameraPos + glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

            break;

        case 'h':
            //Movimento a destra
            cameraPos = cameraPos + glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);

            break;
        }
    }
}

void closeCallback()
{
    handler.close();
}

void mouseMove(int mouseX, int mouseY) {
    handler.mouse(mouseX, mouseY);
}

void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
}

bool LIB_API Engine::init(Handler t_handler) {
    //Inizializzazioni di glut
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitContextVersion(4, 4);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_DEBUG); // <-- Debug flag required by the OpenGL debug callback      

    glutInitWindowPosition(150, 150);
    glutInitWindowSize(1920, 1080);
    total_lights = { 0,0 };
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

    if (GLEW_VERSION_4_4)
        std::cout << "Driver supports OpenGL 4.4\n" << std::endl;
    else
    {
        std::cout << "[ERROR] OpenGL 4.4 not supported\n" << std::endl;
        return -1;
    }


    // Register OpenGL debug callback:
    glDebugMessageCallback((GLDEBUGPROC)DebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    //Lighting
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    //OpenGL enables
    glEnable(GL_DEPTH_TEST);


    //Callbacks
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);
    glutSpecialFunc(specialCallback);
    glutKeyboardFunc(keyboardCallback);
    glutCloseFunc(closeCallback);
    glutPassiveMotionFunc(mouseMove);

    createVoidTexture();

    //Inizializza lettore texture
    FreeImage_Initialise();
    cameraPos = glm::vec4(0.0f, 0.0f, 0.0f,0.0f);
    std::ifstream virtualRead("../engine/resources/is3d.txt");
    virtualRead >> isVirtual;
    virtualRead >> isLeap;
    virtualRead.close();
    if (isVirtual) {
        shader.ovr = std::make_shared<OvVR>();
        if (shader.ovr->init() == false)
        {
            std::cout << "[ERROR] Unable to init OpenVR" << std::endl;
            return -2;
        }

        // Report some info:
        std::cout << "   Manufacturer . . :  " << shader.ovr->getManufacturerName() << std::endl;
        std::cout << "   Tracking system  :  " << shader.ovr->getTrackingSysName() << std::endl;
        std::cout << "   Model number . . :  " << shader.ovr->getModelNumber() << std::endl;
    }
    if (isLeap) {
        // Leap init:
        leap = std::make_shared<Leap>();
        if (!leap->init())
        {
            std::cout << "[ERROR] Unable to init Leap Motion" << std::endl;
            return -1;
        }
    }
    std::shared_ptr<Shader> m_vertex_shader;
    std::shared_ptr<Shader> m_fragment_shader;

    // Compile vertex shader:
    m_vertex_shader = std::make_shared<Shader>();
    m_vertex_shader->loadFromMemory(Shader::TYPE_VERTEX, "../engine/resources/vertex.glsl");

    // Compile fragment shader:
    m_fragment_shader = std::make_shared<Shader>();
    m_fragment_shader->loadFromMemory(Shader::TYPE_FRAGMENT, "../engine/resources/fragment.glsl");


    shader.m_shader = std::make_shared<Shader>();

    shader.m_shader->build(m_vertex_shader.get(), m_fragment_shader.get());
    shaderSetup = std::make_shared<ShaderSetup>(shader,isVirtual);
    //FBO SETTINGS
    std::shared_ptr<Shader> passthroughVs = std::make_shared<Shader>();
    passthroughVs->loadFromMemory(Shader::TYPE_VERTEX, "../engine/resources/vertex_fbo.glsl");

    std::shared_ptr<Shader> passthroughFs = std::make_shared<Shader>();
    passthroughFs->loadFromMemory(Shader::TYPE_FRAGMENT, "../engine/resources/fragment_fbo.glsl");

    std::shared_ptr<Shader> cubemapVs = std::make_shared<Shader>();
    cubemapVs->loadFromMemory(Shader::TYPE_VERTEX, "../engine/resources/vertex_cubemap.glsl");

    std::shared_ptr<Shader> cubemapFs = std::make_shared<Shader>();
    cubemapFs->loadFromMemory(Shader::TYPE_FRAGMENT, "../engine/resources/fragment_cubemap.glsl");

    shader.passthroughShader = std::make_shared<Shader>();
    shader.cubemapShader = std::make_shared<Shader>();
    shader.passthroughShader->build(passthroughVs.get(), passthroughFs.get());
    shader.cubemapShader->build(cubemapVs.get(), cubemapFs.get());

    skybox = std::make_shared<Skybox>();
    skybox->load(handler.skybox_data);
    shaderSetup->setupShader();

    shaderSetup->setupFboShader();

    clear();

    handler.leap->setPosition({ 0.0f,0.0f,0.0f });

    return true;
}

void LIB_API Engine::clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LIB_API Engine::render(const List& list, std::shared_ptr<Camera> camera)
{
    const LEAP_TRACKING_EVENT* l=nullptr;
    if (isVirtual) {
        shader.ovr->update();
        // Store the current viewport size:
        headPos = shader.ovr->getModelviewMatrix();
        headPos[3] = headPos[3] + cameraPos;
    }
    for (int c = 0; c < OvVR::EYE_LAST; c++)
    {

        glm::mat4 ovrProjMat;
        glm::mat4 ovrModelViewMat;
        OvVR::OvEye curEye;
        if (isVirtual) {
            // Get OpenVR matrices:
            curEye = (OvVR::OvEye)c;
            glm::mat4 projMat = shader.ovr->getProjMatrix(curEye, 1.0f, 1024.0f);
            glm::mat4 eye2Head = shader.ovr->getEye2HeadMatrix(curEye);

            // Update camera projection matrix:
            ovrProjMat = projMat * glm::inverse(eye2Head);
            #ifdef APP_VERBOSE   
                        std::cout << "Eye " << c << " proj matrix: " << glm::to_string(ovrProjMat) << std::endl;
            #endif

                        // Update camera modelview matrix:
                        ovrModelViewMat = glm::inverse(headPos); // Inverted because this is the camera matrix
            #ifdef APP_VERBOSE   
                        std::cout << "Eye " << c << " modelview matrix: " << glm::to_string(ovrModelViewMat) << std::endl;
            #endif
        }
        else {
            ovrProjMat = projection;
            ovrModelViewMat = camera->inverseCamera();
        }
        shader.fbo[c]->render();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.m_shader->render();
        shader.m_shader->setMatrix(shader.projection, ovrProjMat);
        shader.m_shader->setInt(shader.num_lights, total_lights.numOmni);
        shader.m_shader->setInt(shader.num_lights_spot, total_lights.numSpot);
        shader.m_shader->setMatrix(shader.view, ovrModelViewMat);

        for (int i = 0; i < list.size(); i++) {
            list[i].first->render(ovrModelViewMat * list[i].second, shader);
            
            Mesh* mesh = dynamic_cast<Mesh*>(list[i].first.get());
            if (mesh) {
                if (mesh->shadow()) {
                    mesh->render_shadow(camera->inverseCamera() * mesh->get_shadow_mat() * list[i].second, shader);
                }
            }
        }
        shader.cubemapShader->render();
        shader.cubemapShader->setMatrix(shader.projCubemap, ovrProjMat);
        skybox->render(ovrModelViewMat,shader);
        if (isVirtual) {
            shader.ovr->pass(curEye, shader.fboTexId[c]);
        }
    }
    if (isVirtual) {
        shader.ovr->render();
    }
    // Done with the FBO, go back to rendering into the window context buffers:
    Fbo::disable();

    glViewport(0, 0, shaderSetup->viewport()[2], shaderSetup->viewport()[3]);
    // Set a matrix for the left "eye":    
    glm::mat4 f = glm::mat4(1.0f);
    glBindVertexArray(shaderSetup->vao());

    // Setup the passthrough shader:
    shader.passthroughShader->render();
    shader.passthroughShader->setMatrix(shader.ptProjLoc, ortho);
    shader.passthroughShader->setMatrix(shader.ptMvLoc, f);


    // Bind the FBO buffer as texture and render:
    glBindTexture(GL_TEXTURE_2D, shader.fboTexId[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
}

void LIB_API Engine::updateLeap(std::shared_ptr<Node> node)
{
    const LEAP_TRACKING_EVENT* l = nullptr;
    if (isLeap) {
        leap->update();
        l = leap->getCurFrame();
    }
    glm::vec3 scale = glm::vec3(5);
    if (isLeap) {
        // Render hands using spheres:
        handler.leap->setNumHands(l->nHands);
        for (unsigned int h = 0; h < l->nHands; h++)
        {
            LEAP_HAND hand = l->pHands[h];
            handler.leap->setPosition({ hand.palm.position.x, hand.palm.position.y, hand.palm.position.z });
            // Elbow:
            glm::mat4 c = glm::translate(glm::mat4(1.0f), glm::vec3(hand.arm.prev_joint.x, hand.arm.prev_joint.y, hand.arm.prev_joint.z)/scale);
            node->getChildByName("Arm")->getChildByName("Sphere1")->setTransformation(c);

            // Wrist:
            c = glm::translate(glm::mat4(1.0f), glm::vec3(hand.arm.next_joint.x, hand.arm.next_joint.y, hand.arm.next_joint.z)/scale);
            node->getChildByName("Arm")->getChildByName("Sphere2")->setTransformation(c);

            // Palm:
            c = glm::translate(glm::mat4(1.0f), glm::vec3(hand.palm.position.x, hand.palm.position.y, hand.palm.position.z)/scale);
            node->getChildByName("Arm")->getChildByName("Sphere3")->setTransformation(c);

            // Distal ends of bones for each digit:
            for (unsigned int d = 0; d < 5; d++)
            {
                LEAP_DIGIT finger = hand.digits[d];
                for (unsigned int b = 0; b < 4; b++)
                {
                    LEAP_BONE bone = finger.bones[b];
                    c = glm::translate(glm::mat4(1.0f), glm::vec3(bone.next_joint.x, bone.next_joint.y, bone.next_joint.z)/scale);
                    node->getChildByName("Arm")->getChildByName("Sphere"+std::to_string(d*4+b+4))->setTransformation(c);
                }
            }
        }
    }
}

void LIB_API Engine::swap()
{
    glutSwapBuffers();
}

std::shared_ptr<Node LIB_API> Engine::load(std::string file) {
    ObjectLoader object_loader(shader);
    auto scene = object_loader.LoadScene(file);
    total_lights = object_loader.getLights();
    return scene;
}

void LIB_API Engine::free()
{
    FreeImage_DeInitialise();
}

void LIB_API Engine::update()
{
    glutMainLoopEvent();
}

void LIB_API Engine::drawText(const std::string& text, float x, float y)
{
    //Setto matrice di proiezione
   /* glMatrixMode(GL_PROJECTION);
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
    glLoadMatrixf(glm::value_ptr(projection));*/
}
