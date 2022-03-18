#include "Develop.h"

#include <engine.h>
#include <glm/glm.hpp>

//Path scena
const std::string d_path = "test/simple3Rect.OVO";

//Oggetto camera
std::shared_ptr<Camera> d_camera = std::make_shared<Camera>();

//Nodo principale -> root
std::shared_ptr<Node> d_node;

//Posizioni camera, per il lookAt
glm::vec3 d_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 d_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 d_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Angolo di visione
float d_fov = 45.0f;

//Camera Flags
bool d_dynamic = true;

//Movimento con mouse
bool d_firstMouse = true;
float d_yaw = -90.0f;
float d_pitch = 0.0f;
float d_lastX = 800.0f / 2.0;
float d_lastY = 600.0 / 2.0;

//Fps
clock_t d_current_ticks, d_delta_ticks;

//Flag per braccio da muovere
int d_choose = 0;

//Flag palla
bool d_is_falling = false;
bool d_is_attached = false;
bool d_has_touched = false;

//Flag chiusura finistra
bool d_is_open = true;

//Flag help
bool d_is_tutorial = false;

//Messaggio di default
std::string d_to_screen = "h -help";

//Coordinate per stampa 2D
float d_x_text = 1;
float d_y_text = 15;

//Fps
int d_fps = 0;

void d_keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    float cameraSpeed = 2.5;

    //Segmento da muovere
    std::shared_ptr<Node> sphere;
    switch (d_choose) {
    case 0:
        //Basso
        sphere = d_node->getChildByName("Sphere001");
        break;
    case 1:
        //Mezzo
        sphere = d_node->getChildByName("Sphere002");
        break;
    case 2:
        //Alto
        sphere = d_node->getChildByName("Sphere003");
        break;
    }

    switch (key) {
    case 'w':
        //Cambio camera / Movimento avanti
        if (d_dynamic) {
            d_cameraPos += cameraSpeed * d_cameraFront;
        }
        else {
            glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
            d_camera->setTransformation(glm::inverse(glm::lookAt(glm::vec3(d_node->getChildByName("Camera1")->getFinalMatrix()[3] + glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)), glm::vec3(d_node->getChildByName("Camera1")->getFinalMatrix()[3] + glm::vec4(0.0f, 0.0f, 10.0f, 0.0f)) + cameraFront, d_cameraUp)));
        }
        break;

    case 's':
        //Movimento indietro
        if (d_dynamic) {
            d_cameraPos -= cameraSpeed * d_cameraFront;
        }
        break;

    case 'a':
        //Cambio camera / Movimento a sinistra
        if (d_dynamic) {
            d_cameraPos -= glm::normalize(glm::cross(d_cameraFront, d_cameraUp)) * cameraSpeed;
        }
        else {
            glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            d_camera->setTransformation(glm::inverse(glm::lookAt(glm::vec3(d_node->getChildByName("Camera2")->getFinalMatrix()[3] - glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)), glm::vec3(d_node->getChildByName("Camera1")->getFinalMatrix()[3] - glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)) + cameraFront, d_cameraUp)));
        }
        break;

    case 'd':
        //Movimento a destra
        if (d_dynamic) {
            d_cameraPos += glm::normalize(glm::cross(d_cameraFront, d_cameraUp)) * cameraSpeed;
        }
        break;

    case 'i':
        //Rotazione braccio avanti
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        break;

    case 'j':
        //Rotazione braccio sinistra
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        break;

    case 'k':
        //Rotazione braccio indietro
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        break;

    case 'l':
        //Rotazione braccio destra
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        break;

    case 'n':
        //Cambio nodo
        d_choose = (d_choose + 1) % 3;
        break;

    case 'c': {
        //Comprime palla
        if (d_is_attached) {
            auto cube = d_node->getChildByName("Box006");
            auto cube1 = d_node->getChildByName("Box007");
            auto sphere1 = d_node->getChildByName("Sphere004");

            cube->setTransformation(glm::translate(cube->getFinalMatrix(), glm::vec3(0.0f, 0.0f, 0.1f)));
            cube1->setTransformation(glm::translate(cube1->getFinalMatrix(), glm::vec3(0.0f, 0.0f, -0.1f)));
            sphere1->setTransformation(glm::scale(sphere1->getFinalMatrix(), glm::vec3(1.0f, 1.0f, 0.95f)));
        }
        break;
    }

    case 'v': {
        //Decomprime palla
        if (d_is_attached) {
            auto cube = d_node->getChildByName("Box006");
            auto cube1 = d_node->getChildByName("Box007");
            auto sphere1 = d_node->getChildByName("Sphere004");

            cube->setTransformation(glm::translate(cube->getFinalMatrix(), glm::vec3(0.0f, 0.0f, -0.1f)));
            cube1->setTransformation(glm::translate(cube1->getFinalMatrix(), glm::vec3(0.0f, 0.0f, 0.1f)));
            sphere1->setTransformation(glm::scale(sphere1->getFinalMatrix(), glm::vec3(1.0f, 1.0f, 1.05f)));
        }
        break;
    }

    case 't':
        //Rilascia palla
        if (d_is_attached) {
            d_is_falling = true;
        }
        break;

    case 'p':
        //Switch dynamic/static camera
        d_dynamic = !d_dynamic;
        break;

    case 'y':
        //Exit
        d_is_open = false;
        break;

    case 'h':
        //Tutorial
        d_is_tutorial = !d_is_tutorial;

        if (d_is_tutorial) {
            d_to_screen = "n -choose node of the arm to move\n";
            d_to_screen = d_to_screen + "p -switch dynamic/static camera\n";
            d_to_screen = d_to_screen + "dynamic\n  w -front\n  a -left\n  s -back\n  d -right\n";
            d_to_screen = d_to_screen + "static\n  w -camera 1\n  a -camera 2\n";
            d_to_screen = d_to_screen + "arm direction\n  i -front\n  j -left\n  k -back\n  l -right\n";
            d_to_screen = d_to_screen + "c -compress the ball\n";
            d_to_screen = d_to_screen + "v -decompress the ball\n";
            d_to_screen = d_to_screen + "t -release the ball\n";
            d_to_screen = d_to_screen + "r -reset\n";
            d_to_screen = d_to_screen + "y -exit\n";

            d_x_text = 1;
            d_y_text = 300;

        }
        else {
            d_x_text = 1;
            d_y_text = 15;
            d_to_screen = "h -help";
        }

        break;

    case 'r':
        //Reset
        d_node = Engine::load(d_path);

        //Disattivo ombra piano base
        dynamic_cast<Mesh*>(d_node->getChildByName("Plane001").get())->shadow(false);

        //Reset flag
        d_is_falling = false;
        d_is_attached = false;
        d_is_open = true;
        d_has_touched = false;
        break;
    }

    //effettuo movimento con camera dinamica
    if (d_dynamic) {
        d_camera->setTransformation(glm::inverse(glm::lookAt(d_cameraPos, d_cameraPos + d_cameraFront, d_cameraUp)));
    }
}

void d_mouseMove(int mouseX, int mouseY) {
    if (d_dynamic) {
        //Prendo coordinate del mouse
        if (d_firstMouse) {
            d_lastX = (float)mouseX;
            d_lastY = (float)mouseY;
            d_firstMouse = false;
        }

        //Sposto offset per movimento camera
        float xoffset = mouseX - d_lastX;
        float yoffset = d_lastY - mouseY;
        d_lastX = (float)mouseX;
        d_lastY = (float)mouseY;

        //Sensibilità del mouse
        float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        //Rotazione intorno asse y
        d_yaw += xoffset;
        //Rotazione intorno asse x
        d_pitch += yoffset;

        //Imposto bound max e min
        if (d_pitch > 89.0f)
            d_pitch = 89.0f;
        if (d_pitch < -89.0f)
            d_pitch = -89.0f;

        //Ricavo distanza tramite il sin e il cos come da documentazione
        glm::vec3 front;
        front.x = cos(glm::radians(d_yaw)) * cos(glm::radians(d_pitch));
        front.y = sin(glm::radians(d_pitch));
        front.z = sin(glm::radians(d_yaw)) * cos(glm::radians(d_pitch));
        //Normalizzo  e setto trasformazione
        d_cameraFront = glm::normalize(front);
        d_camera->setTransformation(glm::inverse(glm::lookAt(d_cameraPos, d_cameraPos + d_cameraFront, d_cameraUp)));
    }
}

void d_close() {
    d_is_open = false;
}

void LIB_API Develop::start() {
    Engine::Handler handler;
    handler.keyboard = d_keyboardCallback;
    handler.mouse = d_mouseMove;
    handler.width = 2048;
    handler.height = 1024;
    handler.close = d_close;

    if (Engine::init(handler)) {
        //Carico scena
        d_node = Engine::load(d_path);

        d_camera->setTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 50.0f)));

        bool passed_1_sec = false;

        //Clock per fps
        clock_t last = clock();
        clock_t current = last;

        //Setto posizione camera iniziale
        d_cameraPos = glm::vec3(-10, 10, 0);
        d_camera->setTransformation(glm::inverse(glm::lookAt(d_cameraPos, d_cameraPos + d_cameraFront, d_cameraUp)));

        while (d_is_open) {
            d_current_ticks = clock();

            List list;

            Engine::clear();

            list.pass(d_node);

            Engine::drawText(d_to_screen, d_x_text, d_y_text);
            Engine::drawText("fps: " + std::to_string(d_fps), 1, 2);

            Engine::render(list, d_camera);

            Engine::swap();
            Engine::update();

            //Calcolo fps
            d_delta_ticks = clock() - d_current_ticks;

            if (d_delta_ticks > 0 && passed_1_sec) {
                d_fps = CLOCKS_PER_SEC / d_delta_ticks;
                passed_1_sec = false;
            }

            if (current - last > 1000) {
                last = current;
                passed_1_sec = true;
            }
            current = clock();
        }
    }
}
