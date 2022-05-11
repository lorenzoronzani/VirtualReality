#include "engine.h"
#include "AABB.h"
#include <glm/glm.hpp>

//Path scena
const std::string path = "test/cube.OVO";

//Distanze per la palla
const int min_distance = 7;
const float distance_ground = 3.2f;

//Oggetto camera
auto camera = std::make_shared<Camera>();

//Nodo principale -> root
std::shared_ptr<Node> node;

//Posizioni camera, per il lookAt
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Angolo di visione
float fov = 45.0f;

//Camera Flags
bool dynamic = true;

//Movimento con mouse
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;

//Fps
clock_t current_ticks, delta_ticks;

//Flag per braccio da muovere
int choose = 0;

//Flag palla
bool is_falling = false;
bool is_attached = false;
bool has_touched = false;

//Flag chiusura finistra
bool is_open = true;

//Flag help
bool is_tutorial = false;

//Messaggio di default
std::string to_screen = "h -help";

//Coordinate per stampa 2D
float x_text = 1;
float y_text = 15;

//Fps
int fps = 0;


glm::vec3 keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    float cameraSpeed = 2.5;

    //Segmento da muovere
    std::shared_ptr<Node> sphere;
    switch (choose) {
    case 0:
        //Basso
        sphere = node->getChildByName("Sphere001");
        break;
    case 1:
        //Mezzo
        sphere = node->getChildByName("Sphere002");
        break;
    case 2:
        //Alto
        sphere = node->getChildByName("Sphere003");
        break;
    }

    switch (key) {
    case 'w':
        //Cambio camera / Movimento avanti
        if (dynamic) {
            cameraPos += cameraSpeed * cameraFront;
        }
        else {
            glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
            camera->setTransformation(glm::inverse(glm::lookAt(glm::vec3(node->getChildByName("Camera1")->getFinalMatrix()[3] + glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)), glm::vec3(node->getChildByName("Camera1")->getFinalMatrix()[3] + glm::vec4(0.0f, 0.0f, 10.0f, 0.0f)) + cameraFront, cameraUp)));
        }
        break;

    case 's':
        //Movimento indietro
        if (dynamic) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        break;

    case 'a':
        //Cambio camera / Movimento a sinistra
        if (dynamic) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        else {
            glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            camera->setTransformation(glm::inverse(glm::lookAt(glm::vec3(node->getChildByName("Camera2")->getFinalMatrix()[3] - glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)), glm::vec3(node->getChildByName("Camera1")->getFinalMatrix()[3] - glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)) + cameraFront, cameraUp)));
        }
        break;

    case 'd':
        //Movimento a destra
        if (dynamic) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        break;

    case 'i':
        //Rotazione braccio avanti
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(),glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        break;

    case 'j':
        //Rotazione braccio sinistra
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(),glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
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
        choose = (choose + 1) % 3;
        break;

    case 'c': {
        //Comprime palla
        if (is_attached) {
            auto cube = node->getChildByName("Box006");
            auto cube1 = node->getChildByName("Box007");
            auto sphere1 = node->getChildByName("Sphere004");

            cube->setTransformation(glm::translate(cube->getFinalMatrix(), glm::vec3(0.0f, 0.0f, 0.1f)));
            cube1->setTransformation(glm::translate(cube1->getFinalMatrix(), glm::vec3(0.0f, 0.0f, -0.1f)));
            sphere1->setTransformation(glm::scale(sphere1->getFinalMatrix(), glm::vec3(1.0f, 1.0f, 0.95f)));
        }
        break;
    }

    case 'v': {
        //Decomprime palla
        if (is_attached) {
            auto cube = node->getChildByName("Box006");
            auto cube1 = node->getChildByName("Box007");
            auto sphere1 = node->getChildByName("Sphere004");

            cube->setTransformation(glm::translate(cube->getFinalMatrix(), glm::vec3(0.0f, 0.0f, -0.1f)));
            cube1->setTransformation(glm::translate(cube1->getFinalMatrix(), glm::vec3(0.0f, 0.0f, 0.1f)));
            sphere1->setTransformation(glm::scale(sphere1->getFinalMatrix(), glm::vec3(1.0f, 1.0f, 1.05f)));
        }
        break;
    }

    case 't':
        //Rilascia palla
        if (is_attached) {
            is_falling = true;
        }
        break;

    case 'p':
        //Switch dynamic/static camera
        dynamic = !dynamic;
        break;

    case 'y':
        //Exit
        is_open = false;
        break;

    case 'h':
        //Tutorial
        is_tutorial = !is_tutorial;

        if (is_tutorial) {
            to_screen = "n -choose node of the arm to move\n";
            to_screen = to_screen + "p -switch dynamic/static camera\n";
            to_screen = to_screen + "dynamic\n  w -front\n  a -left\n  s -back\n  d -right\n";
            to_screen = to_screen + "static\n  w -camera 1\n  a -camera 2\n";
            to_screen = to_screen + "arm direction\n  i -front\n  j -left\n  k -back\n  l -right\n";
            to_screen = to_screen + "c -compress the ball\n";
            to_screen = to_screen + "v -decompress the ball\n";
            to_screen = to_screen + "t -release the ball\n";
            to_screen = to_screen + "r -reset\n";
            to_screen = to_screen + "y -exit\n";

            x_text = 1;
            y_text = 300;

        }
        else {
            x_text = 1;
            y_text = 15;
            to_screen = "h -help";
        }

        break;

    case 'r':
        //Reset
        node = Engine::load(path);

        //Disattivo ombra piano base
        dynamic_cast<Mesh*>(node->getChildByName("Plane001").get())->shadow(false);

        //Reset flag
        is_falling = false;
        is_attached = false;
        is_open = true;
        has_touched = false;
        break;
    }

    //effettuo movimento con camera dinamica
    if (dynamic) {
        camera->setTransformation(glm::inverse(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
    }
    return cameraPos;
}

void mouseMove(int mouseX, int mouseY) {
    if (dynamic) {
        //Prendo coordinate del mouse
        if (firstMouse) {
            lastX = (float)mouseX;
            lastY = (float)mouseY;
            firstMouse = false;
        }

        //Sposto offset per movimento camera
        float xoffset = mouseX - lastX;
        float yoffset = lastY - mouseY;
        lastX = (float)mouseX;
        lastY = (float)mouseY;

        //Sensibilità del mouse
        float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        //Rotazione intorno asse y
        yaw += xoffset;
        //Rotazione intorno asse x
        pitch += yoffset;

        //Imposto bound max e min
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        //Ricavo distanza tramite il sin e il cos come da documentazione
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        //Normalizzo  e setto trasformazione
        cameraFront = glm::normalize(front);
        camera->setTransformation(glm::inverse(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
    }
}

void special(int a, int b, int c) {

}

void close() {
    is_open = false;
}

int main()
{
    Engine::Handler handler;
    handler.keyboard = keyboardCallback;
    handler.mouse = mouseMove;
    handler.width = 1920;
    handler.height = 1080;
    handler.special = special;
    handler.close = close;
    handler.skybox_data = { "test/posx.jpg",
      "test/negx.jpg",
      "test/posy.jpg",
      "test/negy.jpg",
      "test/posz.jpg",
      "test/negz.jpg" };
    handler.leap = std::make_shared<LeapHand>();
    AABB aabb;
    if (Engine::init(handler)) {
        //Carico scena
        node = Engine::load(path);
        camera->setTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 50.0f)));
        std::shared_ptr<Node> sphere;
        sphere = node->getChildByName("Sphere001");

        bool passed_1_sec = false;

        //Clock per fps
        clock_t last = clock();
        clock_t current = last;

        //Setto posizione camera iniziale
        cameraPos = glm::vec3(-110, 10, 0);
        camera->setTransformation(glm::inverse(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
        node->getChildByName("Arm")->setTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-110.0f, 0.0f, 0.0f))*glm::rotate(glm::mat4(1.0f),glm::radians(-90.0f),glm::vec3(0.0f,1.0f,0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.5, 0.5, 0.5)));
        auto up = node->getChildByName("Box014");
        auto down = node->getChildByName("Box011");
        auto left = node->getChildByName("Box013");
        auto right = node->getChildByName("Box012");
        auto material = dynamic_cast<Mesh*>(up.get())->material();
        auto settings = dynamic_cast<Mesh*>(up.get())->material()->settings();
        dynamic_cast<Mesh*>(node->getChildByName("Plane001").get())->shadow(false);
        for (int i = 0; i < 23; i++) {
            dynamic_cast<Mesh*>(node->getChildByName("Sphere" + std::to_string(i + 1)).get())->shadow(false);
        }
        auto hand = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1, 0.1, 0.1));
        while (is_open) {
            current_ticks = clock();

            List list;

            Engine::clear();

            list.pass(node);

            Engine::drawText(to_screen, x_text, y_text);
            Engine::drawText("fps: " + std::to_string(fps), 1, 2);

            Engine::render(list, camera);

            Engine::swap();
            Engine::update();
            node->getChildByName("Arm")->setTransformation(glm::translate(glm::mat4(1.0f),cameraPos+cameraFront*glm::vec3(10)+glm::vec3(10.0f,-10.0f,0.0f))*hand);

            bool found = false;
            for (int i = 0; i < 23; i++) {
                auto current_node = list.getByName("Sphere" + std::to_string(i+1));
                auto up = list.getByName("Box014");
                auto down = list.getByName("Box011");
                auto left = list.getByName("Box013");
                auto right = list.getByName("Box012");
                if (aabb.collide(dynamic_cast<Mesh*>(current_node.first.get()), dynamic_cast<Mesh*>(up.first.get()), current_node.second, up.second)) {
                    sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(),glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
                    found = true;
                }
                if (aabb.collide(dynamic_cast<Mesh*>(current_node.first.get()), dynamic_cast<Mesh*>(down.first.get()), current_node.second, down.second)) {
                    sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
                    found = true;
                }
                if (aabb.collide(dynamic_cast<Mesh*>(current_node.first.get()), dynamic_cast<Mesh*>(left.first.get()), current_node.second, left.second)) {
                    sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
                    found = true;
                }
                if (aabb.collide(dynamic_cast<Mesh*>(current_node.first.get()), dynamic_cast<Mesh*>(right.first.get()), current_node.second, right.second)) {
                    sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
                    found = true;
                }
            }
            if (found) {
                material->settings({ glm::vec3(1.0f,0.0f,0.0f),glm::vec3(1.0f,0.0f,0.0f) ,glm::vec3(1.0f,0.0f,0.0f) });
            }
            else {
                material->settings(settings);
            }
            Engine::updateLeap(node);
            
            //Calcolo fps
            delta_ticks = clock() - current_ticks;
            if (delta_ticks > 0 && passed_1_sec) {
                fps = CLOCKS_PER_SEC / delta_ticks;
                passed_1_sec = false;
                std::cout << fps<<std::endl;
            }

            if (current - last > 1000) {
                last = current;
                passed_1_sec = true;
            }
            current = clock();
        }
    }
}