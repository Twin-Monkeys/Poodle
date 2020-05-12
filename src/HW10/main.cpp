#include "ShaderProgram.h"
#include "stb_image.h"
#include "TextureUtil.h"
#include "VertexAttributeListFactory.h"
#include "VertexArray.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;
using namespace glm; 

static bool initGL();
static void registerCallback();
static void startMainLoop();
static void releaseGL();
static void framebufferSizeCallback(GLFWwindow* pWindow, int width, int height);
static void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
static void eventDispatch();
static void updateShader();
static void render();
static void log(const string& msg);

static GLFWwindow* pWindow = nullptr;
static shared_ptr<VertexArray> pVao;
static shared_ptr<ShaderProgram> pShaderProgram;

int main()
{
    if (!initGL())
        return -1;

    registerCallback();

    constexpr GLfloat vertices[] = 
    {
        // top-left
        -0.5f, 0.5f, 0.f, // pos
        1.f, 1.f, 0.f, 1.f, // vert-color

        // top-right
        0.5f, 0.5f, 0.f,
        0.f, 0.f, 1.f, 1.f,

        // bottom-right
        0.5f, -0.5f, 0.f,
        0.f, 0.1f, 0.f, 1.f,

        // bottom-left
        -0.5f, -0.5f, 0.f,
        1.f, 0.f, 0.f, 1.f
    };

    constexpr GLuint indices[] = 
    { 
        0, 3, 2, 1, 0, 2
    };

    pVao = make_shared<VertexArray>(
        VertexAttributeListFactory::get(VertexAttributeFlag::POS3 | VertexAttributeFlag::COLOR4),
        make_shared<VertexBuffer>(vertices, sizeof(vertices), GL_STATIC_DRAW),
        make_shared<IndexBuffer>(indices, sizeof(indices), GL_STATIC_DRAW),
        (sizeof(indices) / sizeof(indices[0])));

    pShaderProgram = make_shared<ShaderProgram>("rectangle_vert.glsl", "rectangle_frag.glsl");

    startMainLoop();
    releaseGL();

    return 0;
}

bool initGL() 
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    pWindow = glfwCreateWindow(800, 800, "HW10", nullptr, nullptr);

    if (!pWindow)
    {
        log("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        log("Failed to initialize GLAD");
        glfwTerminate();
        return false;
    }

    glfwSwapInterval(1); // VSYNC 0: off, 1: on

    return true;
}

void registerCallback() 
{
    glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
    glfwSetKeyCallback(pWindow, keyCallback);
}

void startMainLoop() 
{
    while (!glfwWindowShouldClose(pWindow))
    {
        eventDispatch();
        updateShader();
        render();
        glfwSwapBuffers(pWindow);
    }
}

void releaseGL() 
{
    pVao.reset();
    pShaderProgram.reset();

    glfwTerminate();
}

void framebufferSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
    {
        log("ESC Ű�� �����̽��ϴ�. ���α׷��� �����մϴ�.");
        glfwSetWindowShouldClose(pWindow, true);
    }
}

void eventDispatch()
{
    glfwPollEvents();
}

void updateShader() 
{
    mat4 transMat = translate(mat4{ 1.f }, vec3{ 0.7f, -0.3f, 0.f });
    transMat = rotate(transMat, quarter_pi<float>(), vec3{ 0.f, 0.f, 1.f });
    transMat = scale(transMat, vec3{ 0.5f, 0.5f, 1.f });

    pShaderProgram->setUniformMatrix4f("transMat", transMat);
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    pShaderProgram->bind();
    pVao->draw();
}

void log(const string& msg)
{
    // c ǥ�� (NDEBUG: �������忡�� ���ǵȴ�.)
#ifndef NDEBUG
    cout << msg << endl;
#endif
}