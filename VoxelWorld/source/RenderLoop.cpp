#include "RenderLoop.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>

#include <imgui.h>

#include <fmt/format.h>

#include "voxel/WorldRenderer.h"

#include "gui/imgui_impl_glfw.h"
#include "gui/imgui_impl_opengl3.h"

static void openglErrorCallback(GLenum /*unused*/, GLenum type, GLuint /*unused*/, GLenum severity, GLsizei /*unused*/,
                                const GLchar* message, const void* /*unused*/) {
    if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
        fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
    }
}

const int INITIAL_WINDOW_WIDTH = 1280;
const int INITIAL_WINDOW_HEIGHT = 800;

const float CAMERA_FOV = 45.0f;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 100.0f;

void RenderLoop::init() {
    this->initGlfw();
    this->initGlew();
    this->initOpenGL();
    this->initGui();
    this->initCamera();
}

void RenderLoop::initGlfw() {
    glfwSetErrorCallback(
        [](int /*unused*/, const char* message) { std::cerr << "GLFW error:" << message << std::endl; });

    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW3");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    this->window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Voxel World", nullptr, nullptr);

    if (!this->window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(this->window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(this->window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
        auto loop = static_cast<RenderLoop*>(glfwGetWindowUserPointer(window));
        loop->projectionMatrix = glm::perspective(
            glm::radians(CAMERA_FOV), static_cast<float>(width) / static_cast<float>(height), NEAR_PLANE, FAR_PLANE);
    });

    glfwSetWindowUserPointer(this->window, static_cast<void*>(this));
}

void RenderLoop::initGlew() {
    glewExperimental = GL_TRUE;
    glewInit();
}

void RenderLoop::initOpenGL() {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openglErrorCallback, nullptr);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS); // smaller value is closer
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glViewport(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
}

void RenderLoop::initGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(this->window, false);
    glfwSetMouseButtonCallback(this->window, ImGui_ImplGlfw_MouseButtonCallback);
    glfwSetCursorPosCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        auto loop = static_cast<RenderLoop*>(glfwGetWindowUserPointer(window));
        loop->mouseCursorPositionCallback(xPosition, yPosition);
    });
    glfwSetScrollCallback(this->window, [](GLFWwindow* window, double xPosition, double yPosition) {
        auto loop = static_cast<RenderLoop*>(glfwGetWindowUserPointer(window));
        loop->mouseScrollCallback(xPosition, yPosition);
    });
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void RenderLoop::initCamera() {
    this->projectionMatrix = glm::perspective(
        glm::radians(CAMERA_FOV), static_cast<float>(INITIAL_WINDOW_WIDTH) / static_cast<float>(INITIAL_WINDOW_HEIGHT),
        NEAR_PLANE, FAR_PLANE);
}

void RenderLoop::mainLoop() {
    bool wireframe = false;

    WorldRenderer worldRenderer;
    worldRenderer.init();

    const std::uint32_t uiMaxFrameTimes = 10;
    std::vector<float> vecFrameTimes(uiMaxFrameTimes);
    std::uint32_t uiNextIndex = 0;
    double dLastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // std::cout << 1.0f / deltaTime << std::endl;

        this->handleInput();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(this->cameraPos, this->cameraPos + this->cameraFront, this->cameraUp);

        // draw cube
        glm::mat4 vp = this->projectionMatrix * view;
        worldRenderer.render(vp, this->cameraPos, this->cameraFront, wireframe);

        if (drawGui) {
            // draw gui
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat("Camera Speed", &this->cameraSpeed, 0.0f, 10.0f);
            const float fAverageTime =
                std::accumulate(std::begin(vecFrameTimes), std::end(vecFrameTimes), 0.0f) / vecFrameTimes.size();
            ImGui::Text("FPS: %g", 1.0 / fAverageTime);
            ImGui::Text("%s", fmt::format("Position: X: {:.2f}, Y: {:.2f}, Z: {:.2f}", cameraPos.x, cameraPos.y, cameraPos.z).c_str());
            ImGui::Text("%s", fmt::format("Front:    X: {:.2f}, Y: {:.2f}, Z: {:.2f}", cameraFront.x, cameraFront.y, cameraFront.z).c_str());
            ImGui::Text("%s", fmt::format("Pitch: {:.2f}, Yaw: {:.2f}", pitch, yaw).c_str());

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwPollEvents();
        glfwSwapBuffers(window);

        const double dCurrentTime = glfwGetTime();
        const double dFrameTime = dCurrentTime - dLastTime;
        dLastTime = dCurrentTime;
        vecFrameTimes[uiNextIndex] = static_cast<float>(dFrameTime);
        uiNextIndex = (uiNextIndex + 1) % uiMaxFrameTimes;
    }

    glfwTerminate();
}

void RenderLoop::handleInput() {
    static bool ctrlDown = false;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += this->deltaTime * this->cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= this->deltaTime * this->cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (this->cameraSpeed * this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (this->cameraSpeed * this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        cameraPos += glm::vec3(0.0, 1.0f, 0.0) * (this->cameraSpeed * this->deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
        cameraPos -= glm::vec3(0.0, 1.0f, 0.0) * (this->cameraSpeed * this->deltaTime);
    }

    // toggle gui with Ctrl key, you also have to press Alt to have a cursor to interact with the GUI
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        ctrlDown = true;
    } else if (ctrlDown) {
        ctrlDown = false;
        this->drawGui = !drawGui;
    }

    // holding down the left Alt key shows a cursor and allows interaction with the GUI
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        // the Alt key has been pressed the first time in this frame (was not pressed before)
        if (!this->showMouseCursor && this->drawGui) {
            glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            this->showMouseCursor = true;
        }
    // alt key was pressed before, but is no longer pressed
    } else if (this->showMouseCursor) {
        this->showMouseCursor = false;
        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        double mouseX;
        double mouseY;
        glfwGetCursorPos(this->window, &mouseX, &mouseY);
        this->lastX = static_cast<float>(mouseX);
        this->lastY = static_cast<float>(mouseY);
    }
}

void RenderLoop::mouseCursorPositionCallback(double xPositionDouble, double yPositionDouble) {
    if (this->showMouseCursor) {
        return;
    }

    float xPosition = static_cast<float>(xPositionDouble);
    float yPosition = static_cast<float>(yPositionDouble);

    if (firstMouse) {
        lastX = xPosition;
        lastY = yPosition;
        firstMouse = false;
    }

    float xoffset = xPosition - lastX;
    float yoffset = lastY - yPosition; // reversed since y-coordinates go from bottom to top
    lastX = xPosition;
    lastY = yPosition;

    float sensitivity = 0.05f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    glm::vec3 front;
    front.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
    front.y = static_cast<float>(sin(glm::radians(pitch)));
    front.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
    cameraFront = glm::normalize(front);
}

void RenderLoop::mouseScrollCallback(double xOffset, double yOffset) {
    if (this->drawGui) {
        ImGui_ImplGlfw_ScrollCallback(this->window, xOffset, yOffset);
    } else {
        cameraDistance -= static_cast<float>(yOffset);
        if (cameraDistance < 2) {
            cameraDistance = 1;
        }
    }
}
