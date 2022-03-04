#include <time.h>
#include <engine.h>

const int min_distance = 7;
const int distance_ground = 3.2;
auto camera = std::make_shared<Camera>();
std::shared_ptr<Node> node;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

clock_t current_ticks, delta_ticks;


bool dynamic = true;
int choose = 0;
bool is_falling=false;
bool is_attached = false;
bool is_open = true;
bool is_tutorial = false;
bool has_touched = false;
std::string to_screen = "h -help";
float x_text = 1;
float y_text = 15;
int fps = 0;


void keyboardCallback(unsigned char key, int mouseX, int mouseY) {
    float cameraSpeed = 2.5;
    std::shared_ptr<Node> sphere;
    switch (choose) {
    case 0:
        sphere = node->getChildByName("Sphere001");
        break;
    case 1:
        sphere = node->getChildByName("Sphere002");
        break;
    case 2:
        sphere = node->getChildByName("Sphere003");
        break;
    }
    switch (key) {
    case 'w':
        if (dynamic) {
            cameraPos += cameraSpeed * cameraFront;
        }
        else {
            glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
            camera->setTransformation(glm::inverse(glm::lookAt(glm::vec3(node->getChildByName("Camera1")->getFinalMatrix()[3]+glm::vec4(0.0f,0.0f,5.0f,0.0f)), glm::vec3(node->getChildByName("Camera1")->getFinalMatrix()[3]+ glm::vec4(0.0f, 0.0f, 10.0f, 0.0f)) + cameraFront, cameraUp)));
        }
        break;
    case 's':
        if (dynamic) {
            cameraPos -= cameraSpeed * cameraFront;
        }
        break;
    case 'a':
        if (dynamic) {
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        else {
            glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            camera->setTransformation(glm::inverse(glm::lookAt(glm::vec3(node->getChildByName("Camera2")->getFinalMatrix()[3] - glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)), glm::vec3(node->getChildByName("Camera1")->getFinalMatrix()[3] - glm::vec4(0.0f, 0.0f, 5.0f, 0.0f)) + cameraFront, cameraUp)));
        }
        break;
    case 'd':
        if (dynamic) {
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        }
        break;
    case 'i':
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        break;
    case 'j':
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        break;
    case 'k':
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        break;
    case 'l':
        sphere->setTransformation(glm::rotate(sphere->getFinalMatrix(), glm::radians(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        break;
    case 'n':
        choose = (choose + 1) % 3;
        break;
    case 'c': {
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
        if (is_attached) {
            is_falling = true;
        }
        break;
    case 'p':
        dynamic = !dynamic;
        break;
    case 'y':
        is_open = false;
        break;
    case 'h':
        is_tutorial = !is_tutorial;
        if (is_tutorial) {
            to_screen = "n -choose node of the arm to move\n";
            to_screen = to_screen+"p -switch dynamic/static camera\n";
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
        node = Engine::load("test/cube.OVO");
        dynamic_cast<Mesh*>(node->getChildByName("Plane001").get())->shadow(false);
        is_falling = false;
        is_attached = false;
        is_open = true;
        has_touched = false;
        break;
    }
    if (dynamic) {
        camera->setTransformation(glm::inverse(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
    }
}

void mouseMove(int mouseX, int mouseY) {
    if (dynamic) {
        if (firstMouse) {
            lastX = mouseX;
            lastY = mouseY;
            firstMouse = false;
        }

        float xoffset = mouseX - lastX;
        float yoffset = lastY - mouseY;
        lastX = mouseX;
        lastY = mouseY;

        float sensitivity = 0.5f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
        camera->setTransformation(glm::inverse(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
    }
}

void special(int a, int b, int c) {

}

int main()
{
    Engine::Handler handler;
    handler.keyboard = keyboardCallback;
    handler.mouse = mouseMove;
    handler.width = 2048;
    handler.height = 1024;
    handler.special = special;
    Engine::init(handler);

    node = Engine::load("test/cube.OVO");
    dynamic_cast<Mesh*>(node->getChildByName("Plane001").get())->shadow(false);
    camera->setTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 10.0f, 50.0f)));
    bool passed_1_sec = false;
    clock_t last = clock();
    clock_t current = last;
    cameraPos = glm::vec3(-10, 10, 0);
    camera->setTransformation(glm::inverse(glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp)));
    while (is_open) {
        current_ticks = clock();

        Engine::update();
        List list;
        Engine::clear();
        list.pass(node);
        auto sphere_center = list.getByName("CenterSphere");
        auto arm_center = list.getByName("Center");
        auto sphere_column = sphere_center->getFinalMatrix()[3];
        auto arm_column = arm_center->getFinalMatrix()[3];
        Engine::drawText(to_screen,x_text,y_text);
        Engine::drawText("fps: "+std::to_string(fps), 1, 2);

        Engine::render(list, camera);
        Engine::swap();
        node->getChildByName("Spot001")->setTransformation(glm::rotate(node->getChildByName("Spot001")->getFinalMatrix(), glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        if (!is_attached && !is_falling && glm::length(arm_column - sphere_column) < min_distance) {
            auto distance = glm::vec4(0, 0, 0, 0)-sphere_column;
            node->getChildByName("Sphere004")->setTransformation(glm::translate(glm::translate(glm::mat4(1.0f),glm::vec3(distance)) * node->getChildByName("Sphere004")->getFinalMatrix(),glm::vec3(0,2.5,-0.5)));
            node->getChildByName("Center")->addChild(node->getChildByName("Sphere004"));
            node->removeChild(node->getChildByName("Sphere004")->id());
            is_attached = true;
            has_touched = false;
        }
        if (is_falling && !has_touched) {
            if (is_attached) {
                is_attached = false;
                auto last = sphere_column;
                node->addChild(node->getChildByName("Sphere004"));
                node->getChildByName("Center")->removeChild(node->getChildByName("Sphere004")->id());
                node->getChildByName("Sphere004")->setTransformation(glm::translate(glm::mat4(1.0f), -glm::vec3(0, 2.5, -0.5))*glm::translate(glm::mat4(1.0f), glm::vec3(last)) * node->getChildByName("Sphere004")->getFinalMatrix());
            }
            if (sphere_center->getFinalMatrix()[3][1] >= distance_ground) {
                node->getChildByName("Sphere004")->setTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.1, 0)) * node->getChildByName("Sphere004")->getFinalMatrix());
            }
            else {
                has_touched = true;
                is_attached = false;
                is_falling = false;
            }
        }
        delta_ticks = clock() - current_ticks;
        if (delta_ticks > 0 && passed_1_sec) {
            fps = CLOCKS_PER_SEC / delta_ticks;
            passed_1_sec = false;
        }
        if (current - last > 1000) {
            last = current;
            passed_1_sec = true;
        }
        current = clock();
    }
}
