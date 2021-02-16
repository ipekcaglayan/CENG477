#include "helper.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_inverse.hpp"

static GLFWwindow *win = NULL;
int widthWindow = 1000, heightWindow = 1000;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idHeightTexture;
GLuint idMVPMatrix;

// Buffers
GLuint idVertexBuffer;
GLuint idIndexBuffer;

int textureWidth, textureHeight;
float heightFactor = 10.0f;

//camera adjustment
glm::vec3 cameraUp = glm::vec3(0, 1, 0);
glm::vec3 cameraGaze = glm::vec3(0, 0, 1);
glm::vec3 cameraLeft = glm::cross(cameraUp, cameraGaze);
glm::vec3 cameraPosition, lightPosition;
GLfloat cameraSpeed;

//light adjustment
GLfloat lightIntensity = 1.0f;

//perspective projection adjustment
GLdouble angle = 45;
GLdouble aspectRatio = 1;
GLdouble near = 0.1;
GLdouble far = 1000;
//GLfloat heightFactor = 10.0f;

//Locations
GLint mvpLoc, mvLoc, cameraPosLoc, nmLoc, samplerLoc, widthTextLoc, heightTextLoc, heightFacLoc, lightPosLoc;
glm::vec3 *vertex_data;
GLFWmonitor *monitor;
const GLFWvidmode *vidmode;

int displayedWindowWidth = 1000;
int displayedWindowHeight = 1000;
bool fullScreen = false, resize = false;
int fullScreenWidth, fullScreenHeight;

GLuint depthMapFBO;
GLuint depthCubemap;
bool lightPosFlag = false;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    //6.madde
    if (key == GLFW_KEY_R && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        heightFactor += 0.5;
        heightFacLoc = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(heightFacLoc, heightFactor);
    }

    if (key == GLFW_KEY_F && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        heightFactor -= 0.5;
        heightFacLoc = glGetUniformLocation(idProgramShader, "heightFactor");
        glUniform1f(heightFacLoc, heightFactor);
    }

    //7.madde
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        cameraPosition.x--;
        cameraPosLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
        glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);
    }

    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        cameraPosition.x++;
        cameraPosLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
        glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);
    }

    //9.madde
    if (key == GLFW_KEY_T && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        lightPosition.y++;
        lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    }

    if (key == GLFW_KEY_G && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        lightPosition.y--;
        lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        lightPosition.x -= 5;
        lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        lightPosition.x += 5;
        lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        lightPosition.z -= 5;
        lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    }

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {

        lightPosition.z += 5;
        lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
        glUniform3fv(lightPosLoc, 1, &lightPosition[0]);
    }

    //13.madde pitch-yaw

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraGaze = glm::rotate(cameraGaze, -0.05f, cameraLeft);
        cameraUp = glm::rotate(cameraUp, -0.05f, cameraLeft);
    }

    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraGaze = glm::rotate(cameraGaze, 0.05f, cameraLeft);
        cameraUp = glm::rotate(cameraUp, 0.05f, cameraLeft);
    }

    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraGaze = glm::rotate(cameraGaze, 0.05f, cameraUp);
        cameraLeft = glm::rotate(cameraLeft, 0.05f, cameraUp);
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraGaze = glm::rotate(cameraGaze, -0.05f, cameraUp);
        cameraLeft = glm::rotate(cameraLeft, -0.05f, cameraUp);
    }
    //13.madde speed

    if (key == GLFW_KEY_Y && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraSpeed += 0.01f;
    }

    if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraSpeed -= 0.01f;
    }

    if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraSpeed = 0.0f;
    }

    //14.madde

    if (key == GLFW_KEY_I && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        cameraUp = glm::vec3(0, 1, 0);
        cameraGaze = glm::vec3(0, 0, 1);
        cameraLeft = glm::cross(cameraUp, cameraGaze);
        cameraPosition = glm::vec3(textureWidth / 2, textureWidth / 10, (-1) * (textureWidth / 4));
        cameraPosLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
        glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);
        cameraSpeed = 0.0f;
    }

    //15.madde

    if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        if (fullScreen)
        {
            displayedWindowWidth=1000;
            displayedWindowHeight=1000;
            glfwSetWindowMonitor(window, nullptr, 0, 0, displayedWindowWidth, displayedWindowHeight, 0);
            aspectRatio = 1;
        }
        else
        {
            displayedWindowWidth=vidmode->width;
            displayedWindowHeight=vidmode->height;
            glfwSetWindowMonitor(window, monitor, 0, 0, vidmode->width, vidmode->height, vidmode->refreshRate);
            aspectRatio = (double)vidmode->width / vidmode->height;
        }
        fullScreen = !fullScreen;
        resize = true;
    }
}

void clear()
{
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void setCamera()
{

    glm::vec3 cameraTarget = cameraPosition + glm::vec3(cameraGaze.x * near, cameraGaze.y * near, cameraGaze.z * near);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);
    glm::mat4 projection = glm::perspective(angle, aspectRatio, near, far);
    glm::mat4 mvp = projection * view * model;

    mvpLoc = glGetUniformLocation(idProgramShader, "MVP");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);

    mvLoc = glGetUniformLocation(idProgramShader, "MV");
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, &view[0][0]);

    nmLoc = glGetUniformLocation(idProgramShader, "normalMatrix");
    glUniformMatrix4fv(nmLoc, 1, GL_FALSE, &glm::inverseTranspose(view)[0][0]);

    cameraPosLoc = glGetUniformLocation(idProgramShader, "cameraPosition");
    glUniform3fv(cameraPosLoc, 1, &cameraPosition[0]);
}

void render()
{
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertex_data);

    glDrawArrays(GL_TRIANGLES, 0, 6 * textureWidth * textureHeight);

    glDisableClientState(GL_VERTEX_ARRAY);
}

void init()
{
    lightPosition = glm::vec3(textureWidth / 2, 100, textureHeight / 2);
    vertex_data = new glm::vec3[6 * textureWidth * textureHeight];
    cameraPosition = glm::vec3(textureWidth / 2, textureWidth / 10, (-1) * (textureWidth / 4));
    cameraSpeed = 0.0f;
    setCamera();
    glViewport(0, 0, displayedWindowWidth, displayedWindowHeight);
    glEnable(GL_DEPTH_TEST);
    GLint samplerLoc = glGetUniformLocation(idProgramShader, "depthTexture");
    glUniform1i(samplerLoc, 1);
    GLint colorSamplerLoc = glGetUniformLocation(idProgramShader, "colorTexture");
    glUniform1i(colorSamplerLoc, 0);

    widthTextLoc = glGetUniformLocation(idProgramShader, "textureWidth");
    glUniform1i(widthTextLoc, textureWidth);

    heightTextLoc = glGetUniformLocation(idProgramShader, "textureHeight");
    glUniform1i(heightTextLoc, textureHeight);

    heightFacLoc = glGetUniformLocation(idProgramShader, "heightFactor");
    glUniform1f(heightFacLoc, heightFactor);

    lightPosLoc = glGetUniformLocation(idProgramShader, "lightPosition");
    glUniform3fv(lightPosLoc, 1, &lightPosition[0]);

    int k = 0;
    //meshes
    for (int i = 0; i < textureWidth; i++)
    {
        for (int j = 0; j < textureHeight; j++)
        {

            vertex_data[k++] = glm::vec3(i, 0, j);
            vertex_data[k++] = glm::vec3(i + 1, 0, j + 1);
            vertex_data[k++] = glm::vec3(i + 1, 0, j);
            vertex_data[k++] = glm::vec3(i, 0, j);
            vertex_data[k++] = glm::vec3(i, 0, j + 1);
            vertex_data[k++] = glm::vec3(i + 1, 0, j + 1);
        }
    }
    monitor = glfwGetPrimaryMonitor();
    vidmode = (glfwGetVideoMode(monitor));
    fullScreenWidth = vidmode->height;
    fullScreenHeight = vidmode->width;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Please provide height and texture image files!\n");
        exit(-1);
    }

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        exit(-1);
    }

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This is required for remote
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // This might be used for local

    win = glfwCreateWindow(widthWindow, heightWindow, "CENG477 - HW4", NULL, NULL);

    if (!win)
    {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

        glfwTerminate();
        exit(-1);
    }
    std::string vertexShader = "shader.vert";
    std::string fragmentShader = "shader.frag";
    initShaders(idProgramShader, vertexShader, fragmentShader);
    glUseProgram(idProgramShader);

    glfwSetKeyCallback(win, keyCallback);

    initTexture(argv[1], argv[2], &textureWidth, &textureHeight);

    init();

    while (!glfwWindowShouldClose(win))
    {
        if (resize)
        {
            glViewport(0, 0, displayedWindowWidth, displayedWindowHeight);
            resize = false;
        }

        clear();
        cameraPosition += cameraSpeed * cameraGaze;
        setCamera();
        render();
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
