#include <iostream>

// GLM:
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Glew
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>

#include <openvr.h>

#include "engine.h"
// FreeImage
#include "Shader.h"
#include "FreeImage.h"
#include "ObjectLoader.h"
#include "ShaderSetup.h"



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

////////////////////////////
static const char* vertShader = R"(
   #version 440 core



   uniform mat4 projection;
   uniform mat4 modelview;
   uniform mat3 normalMatrix;
   layout(location = 0) in vec3 in_Position;
   layout(location = 1) in vec3 in_Normal;
   layout (location = 2) in vec2 aTexCoord;
   // Varying:
   out vec4 fragPosition;
   out vec3 normal; 
   out vec2 TexCoord;
   out mat4 model_view;


   void main(void)
   {
      model_view=modelview;
      fragPosition = modelview * vec4(in_Position, 1.0f);
      gl_Position = projection * fragPosition; 
      normal = normalMatrix * in_Normal;
      TexCoord = vec2(aTexCoord.x, aTexCoord.y);
   }
)";

////////////////////////////
static const char* fragShader = R"(
   #version 440 core

   #define MAX_LIGHTS 16
   out vec4 frag_Output;
   in vec4 fragPosition;
   in vec3 normal;   
   in vec2 TexCoord;   
   in mat4 model_view;
   // Material properties:
   uniform vec3 matEmission;
   uniform vec3 matAmbient;
   uniform vec3 matDiffuse;
   uniform vec3 matSpecular;
   uniform float matShininess;
   // Light properties:
   struct omniLight{
       vec3 lightPosition; 
       vec3 lightAmbient; 
       vec3 lightDiffuse; 
       vec3 lightSpecular;
   };
   struct SpotLight {
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 lightPosition; 
    vec3 lightAmbient; 
    vec3 lightDiffuse; 
    vec3 lightSpecular;    
};

   uniform omniLight lights[MAX_LIGHTS];
   uniform SpotLight lightsSpot[MAX_LIGHTS];

   uniform int numLights;
   uniform int numLightsSpot;

   layout(binding = 0) uniform sampler2D texture1;

   void main(void)
   {
      vec3 finalColor=vec3(0.0,0.0,0.0);
      for(int i=0;i<numLights;i++){

          // Ambient term:
          vec3 fragColor = matEmission + matAmbient * lights[i].lightAmbient;
          // Diffuse term:
          vec3 _normal = normalize(normal);
          vec3 lightDirection = normalize(lights[i].lightPosition - fragPosition.xyz);      
          float nDotL = dot(lightDirection, _normal);   
          if (nDotL > 0.0f)
          {
             fragColor += matDiffuse * nDotL * lights[i].lightDiffuse;
      
             // Specular term:
             vec3 halfVector = normalize(lightDirection + normalize(-fragPosition.xyz));                     
             float nDotHV = dot(_normal, halfVector);         
             fragColor += matSpecular * pow(nDotHV, matShininess) * lights[i].lightSpecular;
          } 
          finalColor=finalColor+fragColor;
      }
      for(int i=0;i<numLightsSpot;i++){
          // Ambient term:
          vec3 fragColor = matEmission + matAmbient * lightsSpot[i].lightAmbient;
          // Diffuse term:
          vec3 _normal = normalize(normal);
          vec3 lightDirection = normalize(normalize(lightsSpot[i].lightPosition - fragPosition.xyz)*mat3(model_view));   
          float theta = dot(lightDirection, normalize(-lightsSpot[i].direction));    
          float nDotL = dot(lightDirection, _normal);   
          if (nDotL > 0 && theta > lightsSpot[i].cutOff)
          {
             fragColor += matDiffuse * nDotL * lightsSpot[i].lightDiffuse;
             // Specular term:
             vec3 halfVector = normalize(lightDirection + normalize((-fragPosition.xyz)));                     
             float nDotHV = dot(_normal, halfVector);         
             fragColor += matSpecular * pow(nDotHV, matShininess) * lightsSpot[i].lightSpecular;
          } 
          
          finalColor=finalColor+fragColor;
      }
      frag_Output = texture(texture1, TexCoord)*vec4(finalColor, 1.0f);
   }
)";

// Passthrough shader with texture mapping:
static const char* passthroughVertShader = R"(
   #version 440 core

   // Uniforms:
   uniform mat4 projection;
   uniform mat4 modelview;   

   // Attributes:
   layout(location = 0) in vec2 in_Position;   
   layout(location = 2) in vec2 in_TexCoord;
   
   // Varying:   
   out vec2 texCoord;

   void main(void)
   {      
      gl_Position = projection * modelview * vec4(in_Position, 0.0f, 1.0f);    
      texCoord = in_TexCoord;
   }
)";

static const char* passthroughFragShader = R"(
   #version 440 core
   
   in vec2 texCoord;
   
   out vec4 fragOutput;   

   // Texture mapping:
   layout(binding = 0) uniform sampler2D texSampler;

   void main(void)   
   {  
      // Texture element:
      vec4 texel = texture(texSampler, texCoord);      
      
      // Final color:
      fragOutput = texel;       
   }
)";

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

void __stdcall DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
    std::cout << "OpenGL says: \"" << std::string(message) << "\"" << std::endl;
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

    std::shared_ptr<Shader> m_vertex_shader;
    std::shared_ptr<Shader> m_fragment_shader;

    // Compile vertex shader:
    m_vertex_shader = std::make_shared<Shader>();
    m_vertex_shader->loadFromMemory(Shader::TYPE_VERTEX, vertShader);

    // Compile fragment shader:
    m_fragment_shader = std::make_shared<Shader>();
    m_fragment_shader->loadFromMemory(Shader::TYPE_FRAGMENT, fragShader);


    shader.m_shader = std::make_shared<Shader>();

    shader.m_shader->build(m_vertex_shader.get(), m_fragment_shader.get());
    shaderSetup = std::make_shared<ShaderSetup>(shader);
    shaderSetup->setupShader();
    //FBO SETTINGS
    shader.fbo = std::make_shared<Fbo>();
    std::shared_ptr<Shader> passthroughVs = std::make_shared<Shader>();
    passthroughVs->loadFromMemory(Shader::TYPE_VERTEX, passthroughVertShader);

    std::shared_ptr<Shader> passthroughFs = std::make_shared<Shader>();
    passthroughFs->loadFromMemory(Shader::TYPE_FRAGMENT, passthroughFragShader);


    shader.passthroughShader = std::make_shared<Shader>();

    shader.passthroughShader->build(passthroughVs.get(), passthroughFs.get());
    shaderSetup->setupFboShader();
    clear();
    return true;
}

void LIB_API Engine::clear()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LIB_API Engine::render(const List& list, std::shared_ptr<Camera> camera)
{

    // Store the current viewport size:
    shader.fbo->render();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.m_shader->render();

    shader.m_shader->setMatrix(shader.projection, projection);

    for (int i = 0; i < list.size(); i++) {
        shader.m_shader->setMatrix(shader.modelview, camera->inverseCamera() * list[i].second);
        shader.m_shader->setInt(shader.num_lights, total_lights.numOmni);
        shader.m_shader->setInt(shader.num_lights_spot, total_lights.numSpot);

        list[i].first->render(camera->inverseCamera() * list[i].second, shader);

        Mesh* mesh = dynamic_cast<Mesh*>(list[i].first.get());
        if (mesh) {
            if (mesh->shadow()) {
                //mesh->render_shadow(camera->inverseCamera() * mesh->get_shadow_mat() * list[i].second);
            }
        }
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
    glBindTexture(GL_TEXTURE_2D, shader.fboTexId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
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
