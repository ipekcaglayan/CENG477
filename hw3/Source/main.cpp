#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow *win = NULL;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void setEnv()
{
    //Light
    const GLfloat ambient[4] = {scene.ambient_light.x, scene.ambient_light.y,
                                scene.ambient_light.z, 1.};
    const std::vector<parser::PointLight> point_lights = scene.point_lights;
    for (int i = 0; i < point_lights.size(); i++)
    {
        const GLfloat intensity[4] = {point_lights[i].intensity.x, point_lights[i].intensity.y,
                                      point_lights[i].intensity.z, 1.};
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, intensity);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, intensity);
        glEnable(GL_LIGHT0 + i);
    }
    //Camera
    const parser::Camera camera = scene.camera;
    const parser::Vec3f e = camera.position;
    parser::Vec3f c = e + camera.gaze * camera.near_distance;
    parser::Vec3f temp = (camera.gaze.CrossProduct(camera.up)).Normalized();
    const parser::Vec3f v = temp.CrossProduct(camera.gaze);
    const parser::Vec4f near_plane = camera.near_plane;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(near_plane.x, near_plane.y, near_plane.z, near_plane.w,
              camera.near_distance, camera.far_distance);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(e.x, e.y, e.z, c.x, c.y, c.z, v.x, v.y, v.z);
}

void applyTransformations(const std::vector<parser::Transformation> &transformations)
{
    for (int i = transformations.size() - 1; i >= 0; i--)
    {
        if (transformations[i].transformation_type == "Translation")
        {
            const parser::Vec3f &translation = scene.translations[transformations[i].id - 1];
            glTranslatef(translation.x, translation.y, translation.z);
        }
        else if (transformations[i].transformation_type == "Scaling")
        {
            const parser::Vec3f &scaling = scene.scalings[transformations[i].id - 1];
            glScalef(scaling.x, scaling.y, scaling.z);
        }
        else
        {
            const parser::Vec4f &rotation = scene.rotations[transformations[i].id - 1];
            glRotatef(rotation.x, rotation.y, rotation.z, rotation.w);
        }
    }
}

void color(const parser::Material &material)
{
    const GLfloat ambient[4] = {material.ambient.x, material.ambient.y,
                                material.ambient.z, 1.};
    const GLfloat diffuse[4] = {material.diffuse.x, material.diffuse.y,
                                material.diffuse.z, 1.};
    const GLfloat specular[4] = {material.specular.x, material.specular.y,
                                 material.specular.z, 1.};
    const GLfloat phong_exponent[1] = {material.phong_exponent};

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, phong_exponent);
}

void drawElements()
{
    int numberOfVertices = scene.vertex_data.size();
    int vertexPosDataSizeInBytes = numberOfVertices * 3 * sizeof(GLfloat);

    glVertexPointer(3, GL_FLOAT, 0, 0);
    glNormalPointer(GL_FLOAT, 0, (const void *)vertexPosDataSizeInBytes);
    int offset = 0;
    for (const auto &mesh : scene.meshes)
    {
        /*for (auto t : mesh.transformations)
        {
            std::cout << t.transformation_type << std::endl;
        }*/
        color(scene.materials[mesh.material_id - 1]);
        mesh.mesh_type == "Solid" ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glPushMatrix();
        applyTransformations(mesh.transformations);
        glDrawElements(GL_TRIANGLES, mesh.faces.size() * 3, GL_UNSIGNED_INT,
                       (const void *)(offset * 3 * sizeof(GLuint)));
        offset += mesh.faces.size();
        glPopMatrix();
    }
}

void init()
{
    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    if (scene.culling_enabled)
    {
        glEnable(GL_CULL_FACE);
        scene.culling_face ? glCullFace(GL_FRONT) : glCullFace(GL_BACK);
    }
    GLuint vertexAttribBuffer;
    GLfloat *vertexPos = scene.vertex_pos;
    GLfloat *vertexNormals = scene.vertex_normals;
    //std::cout<<scene.face_number<<std::endl;
    GLuint *indices = new GLuint[scene.face_number * 3];
    int indexDataSizeInBytes = scene.face_number * 3 * sizeof(GLuint);
    const std::vector<parser::Vec3f> vertex_data = scene.vertex_data;
    const int numberOfVertices = vertex_data.size();
    int vertexPosDataSizeInBytes = numberOfVertices * 3 * sizeof(GLfloat);
    int i = 0;
    for (const parser::Mesh &mesh : scene.meshes)
    {
        const std::vector<parser::Face> faces = mesh.faces;
        for (const parser::Face &face : faces)
        {
            indices[i++] = face.v0_id;
            indices[i++] = face.v1_id;
            indices[i++] = face.v2_id;
        }
    }
    glGenBuffers(1, &vertexAttribBuffer);
    glGenBuffers(1, &gpuIndexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpuIndexBuffer);

    glBufferData(GL_ARRAY_BUFFER, 2 * vertexPosDataSizeInBytes, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexPosDataSizeInBytes, vertexPos);
    glBufferSubData(GL_ARRAY_BUFFER, vertexPosDataSizeInBytes, vertexPosDataSizeInBytes, vertexNormals);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indices, GL_STATIC_DRAW);
}

void display()
{
    glClearColor(scene.background_color.x, scene.background_color.y,
                 scene.background_color.z, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawElements();
}

double lastTime;
int frameCount;

void showFPS(GLFWwindow *pWindow)
{
    double currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    char ss[500] = {};
    frameCount++;
    if (delta >= 1.0)
    {
        double fps = ((double)(frameCount)) / delta;

        sprintf(ss, "CENG477 - HW3 [%lf FPS]", fps);

        glfwSetWindowTitle(pWindow, ss);

        frameCount = 0;
        lastTime = currentTime;
    }
}

int main(int argc, char *argv[])
{
    scene.loadFromXml(argv[1]);

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    init();
    setEnv();

    /*for(int i=0; i< scene.vertex_data.size() * 3; i++){
        std::cout<<scene.vertex_pos[i]<< std::endl;
    }*/
    /*  std::cout<<scene.vertex_pos[scene.vertex_data.size() * 3-2]<< std::endl; */

    lastTime = glfwGetTime();
    frameCount = 0;
    while (!glfwWindowShouldClose(win))
    {

        display();
        showFPS(win);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
