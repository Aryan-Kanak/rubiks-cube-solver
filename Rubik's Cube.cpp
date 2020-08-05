// Rubik's Cube Solver
// This program will simulate a Rubik's cube and solve a scrambled cube
// The Square class (excluding the rotate function), the loadShaders function, and the code for the view and projection matrices were made with help from YouTube tutorials by Suraj Sharma
// All other code belongs to Aryan Kanak

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/matrix_transform.hpp"

using namespace std;

/*
0 - U centre
1 - L centre
2 - F centre
3 - R centre
4 - B centre
5 - D centre
6 - UF edge
7 - UB edge
8 - DB edge
9 - DF edge
10 - LF edge
11 - RF edge
12 - RB edge
13 - LB edge
14 - LU edge
15 - RU edge
16 - RD edge
17 - LD edge
18 - LUB corner
19 - RUB corner
20 - RUF corner
21 - LUF corner
22 - LDB corner
23 - RDB corner
24 - RDF corner
25 - LDF corner
*/

// declare position of centre of cube
glm::vec3 position(0.0f, 0.0f, 0.0f);

// square class
class Square
{
private:
    // declare variables
    vector <glm::vec3> vertices;
    GLuint indices[6] =
    {
        0, 1, 2,
        0, 2, 3
    };
    GLuint vertexArrayObject;
    GLuint vertexBuffer;
    GLuint indexBuffer;
    glm::mat4 modelMatrix;

    // intialize vertex data
    void initVertexData(vector <glm::vec3>& vertexArray)
    {
        vertices = vertexArray;
    }

    // intialize vertex array object
    void initVertexArrayObject()
    {
        // create and bind vertex array object
        glGenVertexArrays(1, &this->vertexArrayObject);
        glBindVertexArray(this->vertexArrayObject);

        // create and bind vertex buffer
        glGenBuffers(1, &this->vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), this->vertices.data(), GL_STATIC_DRAW);

        // create and bind index buffer
        glGenBuffers(1, &this->indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), this->indices, GL_STATIC_DRAW);

        // vertex attribute pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // unbind vertex array object
        glBindVertexArray(0);
    }

    // initialize model matrix
    void initModelMatrix()
    {
        this->modelMatrix = glm::mat4(1.0f);
    }

    // translate square
    void translate()
    {
        this->modelMatrix = glm::translate(this->modelMatrix, position);
    }

    // update uniforms
    void updateUniforms(GLuint core_program, glm::mat4 projMatrix, glm::mat4 viewMatrix, glm::vec4 colour)
    {
        glm::mat4 mvp = projMatrix * viewMatrix * this->modelMatrix;
        glUniformMatrix4fv(glGetUniformLocation(core_program, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform4f(glGetUniformLocation(core_program, "colour"), colour.x, colour.y, colour.z, colour.w);
    }

    // rotate square
    void rotate(glm::mat4 rotation)
    {
        this->modelMatrix = this->modelMatrix * rotation;
    }
public:
    // initialize variables
    void init(vector <glm::vec3> vertexArray)
    {
        this->initVertexData(vertexArray);
        this->initVertexArrayObject();
        this->initModelMatrix();
    }

    // destroy square
    void destroy()
    {
        glDeleteVertexArrays(1, &this->vertexArrayObject);
        glDeleteBuffers(1, &this->vertexBuffer);
        glDeleteBuffers(1, &this->indexBuffer);
    }

    // render square
    void render(GLuint core_program, glm::mat4 projMatrix, glm::mat4 viewMatrix, glm::vec4 colour, glm::mat4 rotation)
    {
        this->initModelMatrix();
        this->rotate(rotation);
        this->translate();
        this->updateUniforms(core_program, projMatrix, viewMatrix, colour);

        glBindVertexArray(vertexArrayObject);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
};

// cube class
class Cube
{
private:
    // declare variables
    Square UFace;
    Square DFace;
    Square LFace;
    Square RFace;
    Square FFace;
    Square BFace;
public:
    // constructor to initialize all variables
    Cube(vector < vector <glm::vec3> > vertexArray)
    {
        UFace.init(vertexArray[0]);
        DFace.init(vertexArray[1]);
        LFace.init(vertexArray[2]);
        RFace.init(vertexArray[3]);
        FFace.init(vertexArray[4]);
        BFace.init(vertexArray[5]);
    }

    // destructor to delete vertex array objects and buffers in variables
    ~Cube()
    {
        UFace.destroy();
        DFace.destroy();
        LFace.destroy();
        RFace.destroy();
        FFace.destroy();
        BFace.destroy();
    }

    // render cube
    void render(GLuint core_program, glm::mat4 projMatrix, glm::mat4 viewMatrix, glm::mat4 rotation)
    {
        UFace.render(core_program, projMatrix, viewMatrix, glm::vec4(1.0, 1.0, 1.0, 1.0), rotation);
        DFace.render(core_program, projMatrix, viewMatrix, glm::vec4(1.0, 1.0, 0.0, 1.0), rotation);
        LFace.render(core_program, projMatrix, viewMatrix, glm::vec4(1.0, 0.5, 0.0, 1.0), rotation);
        RFace.render(core_program, projMatrix, viewMatrix, glm::vec4(1.0, 0.0, 0.0, 1.0), rotation);
        FFace.render(core_program, projMatrix, viewMatrix, glm::vec4(0.0, 1.0, 0.0, 1.0), rotation);
        BFace.render(core_program, projMatrix, viewMatrix, glm::vec4(0.0, 0.0, 1.0, 1.0), rotation);
    }
};

// pause program
void delay(double delayLength)
{
    glfwSetTime(0.0);
    while (glfwGetTime() < delayLength)
    {

    }
}

// swap four pieces
void swapPieces(int& piece1, int& piece2, int& piece3, int& piece4)
{
    // swap pieces
    int swapper = piece4;
    piece4 = piece3;
    piece3 = piece2;
    piece2 = piece1;
    piece1 = swapper;
}

// rotate piece about x-axis
glm::mat4 XAxisRotation(glm::mat4 rotation, float angle)
{
    // convert angle to radians
    float radAngle = glm::radians(angle);
    // create matrix to represent rotation
    float rotationArray[16] =
    {
        1.0, 0.0, 0.0, 0.0,
        0.0, glm::cos(radAngle), -1 * glm::sin(radAngle), 0.0,
        0.0, glm::sin(radAngle), glm::cos(radAngle), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glm::mat4 newRotation = glm::make_mat4(rotationArray);
    // multiply old rotation matrix with new rotation matrix
    rotation = newRotation * rotation;

    return rotation;
}

// rotate piece about y-axis
glm::mat4 YAxisRotation(glm::mat4 rotation, float angle)
{
    // convert angle to radians
    float radAngle = glm::radians(angle);
    // create matrix to represent rotation
    float rotationArray[16] =
    {
        glm::cos(radAngle), 0.0, glm::sin(radAngle), 0.0,
        0.0, 1.0, 0.0, 0.0,
        -1 * glm::sin(radAngle), 0.0, glm::cos(radAngle), 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glm::mat4 newRotation = glm::make_mat4(rotationArray);
    // multiply old rotation matrix with new rotation matrix
    rotation = newRotation * rotation;

    return rotation;
}

// rotate piece about z-axis
glm::mat4 ZAxisRotation(glm::mat4 rotation, float angle)
{
    // convert angle to radians
    float radAngle = glm::radians(angle);
    // create matrix to represent rotation
    float rotationArray[16] =
    {
        glm::cos(radAngle), -1 * glm::sin(radAngle), 0.0, 0.0,
        glm::sin(radAngle), cos(radAngle), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glm::mat4 newRotation = glm::make_mat4(rotationArray);
    // multiply old rotation matrix with new rotation matrix
    rotation = newRotation * rotation;

    return rotation;
}

// render cubes
void renderCubes(GLFWwindow* window, Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view, glm::mat4(&rotation)[26])
{
    // clear
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // render outline of cubes
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int i = 0; i < 26; i++)
    {
        cubes[i].render(core_program, proj, view, rotation[i]);
    }
    // render cubes
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for (int i = 0; i < 26; i++)
    {
        cubes[i].render(core_program, proj, view, rotation[i]);
    }
    // swap buffers
    glfwSwapBuffers(window);
}

// turn U face
void turnUFace(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // U move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 90.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 90.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 90.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 90.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 90.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 90.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 90.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 90.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 90.0);
        rotation[pieces[0]] = ZAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[7]] = ZAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[15]] = ZAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[6]] = ZAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[14]] = ZAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[18]] = ZAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[19]] = ZAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[20]] = ZAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[21]] = ZAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[7], pieces[15], pieces[6], pieces[14]);
        swapPieces(pieces[18], pieces[19], pieces[20], pieces[21]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[0], edgeStickers[1], edgeStickers[2], edgeStickers[3]);
        swapPieces(edgeStickers[16], edgeStickers[12], edgeStickers[8], edgeStickers[4]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[0], cornerStickers[1], cornerStickers[2], cornerStickers[3]);
        swapPieces(cornerStickers[16], cornerStickers[12], cornerStickers[8], cornerStickers[4]);
        swapPieces(cornerStickers[17], cornerStickers[13], cornerStickers[9], cornerStickers[5]);
    }
    // U2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 180.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 180.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 180.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 180.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 180.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 180.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 180.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 180.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 180.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -44.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -44.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -44.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -44.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -44.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -44.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -44.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -44.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -44.0);
        // save new positions of pieces
        swap(pieces[7], pieces[6]);
        swap(pieces[15], pieces[14]);
        swap(pieces[18], pieces[20]);
        swap(pieces[19], pieces[21]);
        // save new positions of edge stickers
        swap(edgeStickers[0], edgeStickers[2]);
        swap(edgeStickers[1], edgeStickers[3]);
        swap(edgeStickers[16], edgeStickers[8]);
        swap(edgeStickers[12], edgeStickers[4]);
        // save new positions of corner stickers
        swap(cornerStickers[0], cornerStickers[2]);
        swap(cornerStickers[1], cornerStickers[3]);
        swap(cornerStickers[16], cornerStickers[8]);
        swap(cornerStickers[12], cornerStickers[4]);
        swap(cornerStickers[17], cornerStickers[9]);
        swap(cornerStickers[13], cornerStickers[5]);
    }
    // U' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -90.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -90.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -90.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -90.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -90.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -90.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -90.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -90.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -90.0);
        rotation[pieces[0]] = ZAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[7]] = ZAxisRotation(rotation[pieces[7]], -22.0);
        rotation[pieces[15]] = ZAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[6]] = ZAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[14]] = ZAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[18]] = ZAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[19]] = ZAxisRotation(rotation[pieces[19]], -22.0);
        rotation[pieces[20]] = ZAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[21]] = ZAxisRotation(rotation[pieces[21]], -22.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[7], pieces[14], pieces[6], pieces[15]);
        swapPieces(pieces[18], pieces[21], pieces[20], pieces[19]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[0], edgeStickers[3], edgeStickers[2], edgeStickers[1]);
        swapPieces(edgeStickers[16], edgeStickers[4], edgeStickers[8], edgeStickers[12]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[0], cornerStickers[3], cornerStickers[2], cornerStickers[1]);
        swapPieces(cornerStickers[16], cornerStickers[4], cornerStickers[8], cornerStickers[12]);
        swapPieces(cornerStickers[17], cornerStickers[5], cornerStickers[9], cornerStickers[13]);
    }
}

// turn D face
void turnDFace(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // D move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -90.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -90.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -90.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -90.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -90.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -90.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -90.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -90.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -90.0);
        rotation[pieces[5]] = ZAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[8]] = ZAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[16]] = ZAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[9]] = ZAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[17]] = ZAxisRotation(rotation[pieces[17]], -22.0);
        rotation[pieces[22]] = ZAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[23]] = ZAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[24]] = ZAxisRotation(rotation[pieces[24]], -22.0);
        rotation[pieces[25]] = ZAxisRotation(rotation[pieces[25]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[9], pieces[16], pieces[8], pieces[17]);
        swapPieces(pieces[24], pieces[23], pieces[22], pieces[25]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[20], edgeStickers[21], edgeStickers[22], edgeStickers[23]);
        swapPieces(edgeStickers[6], edgeStickers[10], edgeStickers[14], edgeStickers[18]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[20], cornerStickers[21], cornerStickers[22], cornerStickers[23]);
        swapPieces(cornerStickers[6], cornerStickers[10], cornerStickers[14], cornerStickers[18]);
        swapPieces(cornerStickers[7], cornerStickers[11], cornerStickers[15], cornerStickers[19]);
    }
    // D2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 180.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 180.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 180.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 180.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 180.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 180.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 180.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 180.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 180.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -44.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -44.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -44.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -44.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -44.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -44.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -44.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -44.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -44.0);
        // save new positions of pieces
        swap(pieces[8], pieces[9]);
        swap(pieces[16], pieces[17]);
        swap(pieces[22], pieces[24]);
        swap(pieces[23], pieces[25]);
        // save new positions of edge stickers
        swap(edgeStickers[20], edgeStickers[22]);
        swap(edgeStickers[21], edgeStickers[23]);
        swap(edgeStickers[6], edgeStickers[14]);
        swap(edgeStickers[10], edgeStickers[18]);
        // save new positions of corner stickers
        swap(cornerStickers[20], cornerStickers[22]);
        swap(cornerStickers[21], cornerStickers[23]);
        swap(cornerStickers[6], cornerStickers[14]);
        swap(cornerStickers[10], cornerStickers[18]);
        swap(cornerStickers[7], cornerStickers[15]);
        swap(cornerStickers[11], cornerStickers[19]);
    }
    // D' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 90.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 90.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 90.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 90.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 90.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 90.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 90.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 90.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 90.0);
        rotation[pieces[5]] = ZAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[8]] = ZAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[16]] = ZAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[9]] = ZAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[17]] = ZAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[22]] = ZAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[23]] = ZAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[24]] = ZAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[25]] = ZAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[8], pieces[16], pieces[9], pieces[17]);
        swapPieces(pieces[22], pieces[23], pieces[24], pieces[25]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[20], edgeStickers[23], edgeStickers[22], edgeStickers[21]);
        swapPieces(edgeStickers[6], edgeStickers[18], edgeStickers[14], edgeStickers[10]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[20], cornerStickers[23], cornerStickers[22], cornerStickers[21]);
        swapPieces(cornerStickers[6], cornerStickers[18], cornerStickers[14], cornerStickers[10]);
        swapPieces(cornerStickers[7], cornerStickers[19], cornerStickers[15], cornerStickers[11]);
    }
}

// turn L face
void turnLFace(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // L move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -32.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -90.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -90.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -90.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -90.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -90.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -90.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -90.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -90.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -90.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 32.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[14], pieces[10], pieces[17], pieces[13]);
        swapPieces(pieces[21], pieces[25], pieces[22], pieces[18]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[3], edgeStickers[11], edgeStickers[23], edgeStickers[17]);
        swapPieces(edgeStickers[4], edgeStickers[5], edgeStickers[6], edgeStickers[7]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[3], cornerStickers[11], cornerStickers[23], cornerStickers[17]);
        swapPieces(cornerStickers[4], cornerStickers[5], cornerStickers[6], cornerStickers[7]);
        swapPieces(cornerStickers[0], cornerStickers[8], cornerStickers[20], cornerStickers[18]);
    }
    // L2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -32.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 180.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 180.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 180.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 180.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 180.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 180.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 180.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 180.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 180.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 32.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        // save new positions of pieces
        swap(pieces[14], pieces[17]);
        swap(pieces[10], pieces[13]);
        swap(pieces[21], pieces[22]);
        swap(pieces[25], pieces[18]);
        // save new positions of edge stickers
        swap(edgeStickers[3], edgeStickers[23]);
        swap(edgeStickers[11], edgeStickers[17]);
        swap(edgeStickers[4], edgeStickers[6]);
        swap(edgeStickers[5], edgeStickers[7]);
        // save new positions of corner stickers
        swap(cornerStickers[3], cornerStickers[23]);
        swap(cornerStickers[11], cornerStickers[17]);
        swap(cornerStickers[4], cornerStickers[6]);
        swap(cornerStickers[5], cornerStickers[7]);
        swap(cornerStickers[0], cornerStickers[20]);
        swap(cornerStickers[8], cornerStickers[18]);
    }
    // L' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -32.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 90.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 90.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 90.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 90.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 90.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 90.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 90.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 90.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 90.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 32.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[14], pieces[13], pieces[17], pieces[10]);
        swapPieces(pieces[21], pieces[18], pieces[22], pieces[25]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[3], edgeStickers[17], edgeStickers[23], edgeStickers[11]);
        swapPieces(edgeStickers[4], edgeStickers[7], edgeStickers[6], edgeStickers[5]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[3], cornerStickers[17], cornerStickers[23], cornerStickers[11]);
        swapPieces(cornerStickers[4], cornerStickers[7], cornerStickers[6], cornerStickers[5]);
        swapPieces(cornerStickers[0], cornerStickers[18], cornerStickers[20], cornerStickers[8]);
    }
}

// turn R face
void turnRFace(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // R move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -32.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 90.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 90.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 90.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 90.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 90.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 90.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 90.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 90.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 90.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 32.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[15], pieces[12], pieces[16], pieces[11]);
        swapPieces(pieces[20], pieces[19], pieces[23], pieces[24]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[1], edgeStickers[19], edgeStickers[21], edgeStickers[9]);
        swapPieces(edgeStickers[12], edgeStickers[13], edgeStickers[14], edgeStickers[15]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[1], cornerStickers[19], cornerStickers[21], cornerStickers[9]);
        swapPieces(cornerStickers[12], cornerStickers[13], cornerStickers[14], cornerStickers[15]);
        swapPieces(cornerStickers[2], cornerStickers[16], cornerStickers[22], cornerStickers[10]);
    }
    // R2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -32.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 180.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 180.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 180.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 180.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 180.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 180.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 180.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 180.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 180.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 32.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        // save new positions of pieces
        swap(pieces[15], pieces[16]);
        swap(pieces[12], pieces[11]);
        swap(pieces[20], pieces[23]);
        swap(pieces[19], pieces[24]);
        // save new positions of edge stickers
        swap(edgeStickers[1], edgeStickers[21]);
        swap(edgeStickers[19], edgeStickers[9]);
        swap(edgeStickers[12], edgeStickers[14]);
        swap(edgeStickers[13], edgeStickers[15]);
        // save new positions of corner stickers
        swap(cornerStickers[1], cornerStickers[21]);
        swap(cornerStickers[19], cornerStickers[9]);
        swap(cornerStickers[12], cornerStickers[14]);
        swap(cornerStickers[13], cornerStickers[15]);
        swap(cornerStickers[2], cornerStickers[22]);
        swap(cornerStickers[16], cornerStickers[10]);
    }
    // R' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -32.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -90.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -90.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -90.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -90.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -90.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -90.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -90.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -90.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -90.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 32.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[15], pieces[11], pieces[16], pieces[12]);
        swapPieces(pieces[20], pieces[24], pieces[23], pieces[19]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[1], edgeStickers[9], edgeStickers[21], edgeStickers[19]);
        swapPieces(edgeStickers[12], edgeStickers[15], edgeStickers[14], edgeStickers[13]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[1], cornerStickers[9], cornerStickers[21], cornerStickers[19]);
        swapPieces(cornerStickers[12], cornerStickers[15], cornerStickers[14], cornerStickers[13]);
        swapPieces(cornerStickers[2], cornerStickers[10], cornerStickers[22], cornerStickers[16]);
    }
}

// turn F face
void turnFFace(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // F move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -32.0);
        rotation[pieces[2]] = ZAxisRotation(rotation[pieces[2]], 90.0);
        rotation[pieces[6]] = ZAxisRotation(rotation[pieces[6]], 90.0);
        rotation[pieces[11]] = ZAxisRotation(rotation[pieces[11]], 90.0);
        rotation[pieces[9]] = ZAxisRotation(rotation[pieces[9]], 90.0);
        rotation[pieces[10]] = ZAxisRotation(rotation[pieces[10]], 90.0);
        rotation[pieces[20]] = ZAxisRotation(rotation[pieces[20]], 90.0);
        rotation[pieces[24]] = ZAxisRotation(rotation[pieces[24]], 90.0);
        rotation[pieces[25]] = ZAxisRotation(rotation[pieces[25]], 90.0);
        rotation[pieces[21]] = ZAxisRotation(rotation[pieces[21]], 90.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 32.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[6], pieces[11], pieces[9], pieces[10]);
        swapPieces(pieces[20], pieces[24], pieces[25], pieces[21]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[8], edgeStickers[9], edgeStickers[10], edgeStickers[11]);
        swapPieces(edgeStickers[2], edgeStickers[15], edgeStickers[20], edgeStickers[5]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[8], cornerStickers[9], cornerStickers[10], cornerStickers[11]);
        swapPieces(cornerStickers[2], cornerStickers[15], cornerStickers[20], cornerStickers[5]);
        swapPieces(cornerStickers[3], cornerStickers[12], cornerStickers[21], cornerStickers[6]);
    }
    // F2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -32.0);
        rotation[pieces[2]] = ZAxisRotation(rotation[pieces[2]], 180.0);
        rotation[pieces[6]] = ZAxisRotation(rotation[pieces[6]], 180.0);
        rotation[pieces[11]] = ZAxisRotation(rotation[pieces[11]], 180.0);
        rotation[pieces[9]] = ZAxisRotation(rotation[pieces[9]], 180.0);
        rotation[pieces[10]] = ZAxisRotation(rotation[pieces[10]], 180.0);
        rotation[pieces[20]] = ZAxisRotation(rotation[pieces[20]], 180.0);
        rotation[pieces[24]] = ZAxisRotation(rotation[pieces[24]], 180.0);
        rotation[pieces[25]] = ZAxisRotation(rotation[pieces[25]], 180.0);
        rotation[pieces[21]] = ZAxisRotation(rotation[pieces[21]], 180.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 32.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        // save new positions of pieces
        swap(pieces[6], pieces[9]);
        swap(pieces[11], pieces[10]);
        swap(pieces[20], pieces[25]);
        swap(pieces[24], pieces[21]);
        // save new positions of edge stickers
        swap(edgeStickers[8], edgeStickers[10]);
        swap(edgeStickers[9], edgeStickers[11]);
        swap(edgeStickers[2], edgeStickers[20]);
        swap(edgeStickers[15], edgeStickers[5]);
        // save new positions of corner stickers
        swap(cornerStickers[8], cornerStickers[10]);
        swap(cornerStickers[9], cornerStickers[11]);
        swap(cornerStickers[2], cornerStickers[20]);
        swap(cornerStickers[15], cornerStickers[5]);
        swap(cornerStickers[3], cornerStickers[21]);
        swap(cornerStickers[12], cornerStickers[6]);
    }
    // F' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], 22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], 22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], 22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], 22.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], -32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], -32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], -32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], -32.0);
        rotation[pieces[2]] = ZAxisRotation(rotation[pieces[2]], -90.0);
        rotation[pieces[6]] = ZAxisRotation(rotation[pieces[6]], -90.0);
        rotation[pieces[11]] = ZAxisRotation(rotation[pieces[11]], -90.0);
        rotation[pieces[9]] = ZAxisRotation(rotation[pieces[9]], -90.0);
        rotation[pieces[10]] = ZAxisRotation(rotation[pieces[10]], -90.0);
        rotation[pieces[20]] = ZAxisRotation(rotation[pieces[20]], -90.0);
        rotation[pieces[24]] = ZAxisRotation(rotation[pieces[24]], -90.0);
        rotation[pieces[25]] = ZAxisRotation(rotation[pieces[25]], -90.0);
        rotation[pieces[21]] = ZAxisRotation(rotation[pieces[21]], -90.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 32.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 32.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 32.0);
        rotation[pieces[20]] = YAxisRotation(rotation[pieces[20]], 32.0);
        rotation[pieces[24]] = YAxisRotation(rotation[pieces[24]], 32.0);
        rotation[pieces[25]] = YAxisRotation(rotation[pieces[25]], 32.0);
        rotation[pieces[21]] = YAxisRotation(rotation[pieces[21]], 32.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[20]] = XAxisRotation(rotation[pieces[20]], -22.0);
        rotation[pieces[24]] = XAxisRotation(rotation[pieces[24]], -22.0);
        rotation[pieces[25]] = XAxisRotation(rotation[pieces[25]], -22.0);
        rotation[pieces[21]] = XAxisRotation(rotation[pieces[21]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[6], pieces[10], pieces[9], pieces[11]);
        swapPieces(pieces[20], pieces[21], pieces[25], pieces[24]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[8], edgeStickers[11], edgeStickers[10], edgeStickers[9]);
        swapPieces(edgeStickers[2], edgeStickers[5], edgeStickers[20], edgeStickers[15]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[8], cornerStickers[11], cornerStickers[10], cornerStickers[9]);
        swapPieces(cornerStickers[2], cornerStickers[5], cornerStickers[20], cornerStickers[15]);
        swapPieces(cornerStickers[3], cornerStickers[6], cornerStickers[21], cornerStickers[12]);
    }
}

// turn B face
void turnBFace(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // B move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -32.0);
        rotation[pieces[4]] = ZAxisRotation(rotation[pieces[4]], -90.0);
        rotation[pieces[7]] = ZAxisRotation(rotation[pieces[7]], -90.0);
        rotation[pieces[13]] = ZAxisRotation(rotation[pieces[13]], -90.0);
        rotation[pieces[8]] = ZAxisRotation(rotation[pieces[8]], -90.0);
        rotation[pieces[12]] = ZAxisRotation(rotation[pieces[12]], -90.0);
        rotation[pieces[18]] = ZAxisRotation(rotation[pieces[18]], -90.0);
        rotation[pieces[22]] = ZAxisRotation(rotation[pieces[22]], -90.0);
        rotation[pieces[23]] = ZAxisRotation(rotation[pieces[23]], -90.0);
        rotation[pieces[19]] = ZAxisRotation(rotation[pieces[19]], -90.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 32.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[7], pieces[13], pieces[8], pieces[12]);
        swapPieces(pieces[18], pieces[22], pieces[23], pieces[19]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[16], edgeStickers[17], edgeStickers[18], edgeStickers[19]);
        swapPieces(edgeStickers[0], edgeStickers[7], edgeStickers[22], edgeStickers[13]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[16], cornerStickers[17], cornerStickers[18], cornerStickers[19]);
        swapPieces(cornerStickers[0], cornerStickers[7], cornerStickers[22], cornerStickers[13]);
        swapPieces(cornerStickers[1], cornerStickers[4], cornerStickers[23], cornerStickers[14]);
    }
    // B2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -32.0);
        rotation[pieces[4]] = ZAxisRotation(rotation[pieces[4]], 180.0);
        rotation[pieces[7]] = ZAxisRotation(rotation[pieces[7]], 180.0);
        rotation[pieces[13]] = ZAxisRotation(rotation[pieces[13]], 180.0);
        rotation[pieces[8]] = ZAxisRotation(rotation[pieces[8]], 180.0);
        rotation[pieces[12]] = ZAxisRotation(rotation[pieces[12]], 180.0);
        rotation[pieces[18]] = ZAxisRotation(rotation[pieces[18]], 180.0);
        rotation[pieces[22]] = ZAxisRotation(rotation[pieces[22]], 180.0);
        rotation[pieces[23]] = ZAxisRotation(rotation[pieces[23]], 180.0);
        rotation[pieces[19]] = ZAxisRotation(rotation[pieces[19]], 180.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 32.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        // save new positions of pieces
        swap(pieces[7], pieces[8]);
        swap(pieces[13], pieces[12]);
        swap(pieces[18], pieces[23]);
        swap(pieces[22], pieces[19]);
        // save new positions of edge stickers
        swap(edgeStickers[16], edgeStickers[18]);
        swap(edgeStickers[17], edgeStickers[19]);
        swap(edgeStickers[0], edgeStickers[22]);
        swap(edgeStickers[7], edgeStickers[13]);
        // save new positions of corner stickers
        swap(cornerStickers[16], cornerStickers[18]);
        swap(cornerStickers[17], cornerStickers[19]);
        swap(cornerStickers[0], cornerStickers[22]);
        swap(cornerStickers[7], cornerStickers[13]);
        swap(cornerStickers[1], cornerStickers[23]);
        swap(cornerStickers[4], cornerStickers[14]);
    }
    // B' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], 22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], 22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], 22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], 22.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], -32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], -32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], -32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], -32.0);
        rotation[pieces[4]] = ZAxisRotation(rotation[pieces[4]], 90.0);
        rotation[pieces[7]] = ZAxisRotation(rotation[pieces[7]], 90.0);
        rotation[pieces[13]] = ZAxisRotation(rotation[pieces[13]], 90.0);
        rotation[pieces[8]] = ZAxisRotation(rotation[pieces[8]], 90.0);
        rotation[pieces[12]] = ZAxisRotation(rotation[pieces[12]], 90.0);
        rotation[pieces[18]] = ZAxisRotation(rotation[pieces[18]], 90.0);
        rotation[pieces[22]] = ZAxisRotation(rotation[pieces[22]], 90.0);
        rotation[pieces[23]] = ZAxisRotation(rotation[pieces[23]], 90.0);
        rotation[pieces[19]] = ZAxisRotation(rotation[pieces[19]], 90.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 32.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 32.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 32.0);
        rotation[pieces[18]] = YAxisRotation(rotation[pieces[18]], 32.0);
        rotation[pieces[22]] = YAxisRotation(rotation[pieces[22]], 32.0);
        rotation[pieces[23]] = YAxisRotation(rotation[pieces[23]], 32.0);
        rotation[pieces[19]] = YAxisRotation(rotation[pieces[19]], 32.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[18]] = XAxisRotation(rotation[pieces[18]], -22.0);
        rotation[pieces[22]] = XAxisRotation(rotation[pieces[22]], -22.0);
        rotation[pieces[23]] = XAxisRotation(rotation[pieces[23]], -22.0);
        rotation[pieces[19]] = XAxisRotation(rotation[pieces[19]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[7], pieces[12], pieces[8], pieces[13]);
        swapPieces(pieces[18], pieces[19], pieces[23], pieces[22]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[16], edgeStickers[19], edgeStickers[18], edgeStickers[17]);
        swapPieces(edgeStickers[0], edgeStickers[13], edgeStickers[22], edgeStickers[7]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[16], cornerStickers[19], cornerStickers[18], cornerStickers[17]);
        swapPieces(cornerStickers[0], cornerStickers[13], cornerStickers[22], cornerStickers[7]);
        swapPieces(cornerStickers[1], cornerStickers[14], cornerStickers[23], cornerStickers[4]);
    }
}

// turn M slice
void turnMSlice(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // M move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -32.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -32.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -90.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -90.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -90.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -90.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -90.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -90.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -90.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -90.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 32.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 32.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[0], pieces[2], pieces[5], pieces[4]);
        swapPieces(pieces[6], pieces[9], pieces[8], pieces[7]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[0], edgeStickers[8], edgeStickers[20], edgeStickers[18]);
        swapPieces(edgeStickers[2], edgeStickers[10], edgeStickers[22], edgeStickers[16]);
    }
    // M2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -32.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -32.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 180.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 180.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 180.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 180.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 180.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 180.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 180.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 180.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 32.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 32.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        // save new positions of pieces
        swap(pieces[0], pieces[5]);
        swap(pieces[2], pieces[4]);
        swap(pieces[6], pieces[8]);
        swap(pieces[9], pieces[7]);
        // save new positions of edge stickers
        swap(edgeStickers[0], edgeStickers[20]);
        swap(edgeStickers[8], edgeStickers[18]);
        swap(edgeStickers[2], edgeStickers[22]);
        swap(edgeStickers[10], edgeStickers[16]);
    }
    // M' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 22.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -32.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -32.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], -32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], -32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], -32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], -32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 90.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], 90.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 90.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], 90.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], 90.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], 90.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], 90.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], 90.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 32.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 32.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 32.0);
        rotation[pieces[6]] = YAxisRotation(rotation[pieces[6]], 32.0);
        rotation[pieces[9]] = YAxisRotation(rotation[pieces[9]], 32.0);
        rotation[pieces[8]] = YAxisRotation(rotation[pieces[8]], 32.0);
        rotation[pieces[7]] = YAxisRotation(rotation[pieces[7]], 32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[6]] = XAxisRotation(rotation[pieces[6]], -22.0);
        rotation[pieces[9]] = XAxisRotation(rotation[pieces[9]], -22.0);
        rotation[pieces[8]] = XAxisRotation(rotation[pieces[8]], -22.0);
        rotation[pieces[7]] = XAxisRotation(rotation[pieces[7]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[0], pieces[4], pieces[5], pieces[2]);
        swapPieces(pieces[6], pieces[7], pieces[8], pieces[9]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[0], edgeStickers[18], edgeStickers[20], edgeStickers[8]);
        swapPieces(edgeStickers[2], edgeStickers[16], edgeStickers[22], edgeStickers[10]);
    }
}

// turn E slice
void turnESlice(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // E move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -90.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], -90.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -90.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], -90.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], -90.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], -90.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], -90.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], -90.0);
        rotation[pieces[1]] = ZAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[2]] = ZAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[3]] = ZAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[4]] = ZAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[10]] = ZAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[11]] = ZAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[12]] = ZAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[13]] = ZAxisRotation(rotation[pieces[13]], -22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[1], pieces[2], pieces[3], pieces[4]);
        swapPieces(pieces[10], pieces[11], pieces[12], pieces[13]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[5], edgeStickers[9], edgeStickers[13], edgeStickers[17]);
        swapPieces(edgeStickers[7], edgeStickers[11], edgeStickers[15], edgeStickers[19]);
    }
    // E2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 180.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 180.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 180.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 180.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 180.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 180.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 180.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 180.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -44.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -44.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -44.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -44.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -44.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -44.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -44.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -44.0);
        // save new positions of pieces
        swap(pieces[1], pieces[3]);
        swap(pieces[2], pieces[4]);
        swap(pieces[10], pieces[12]);
        swap(pieces[11], pieces[13]);
        // save new positions of edge stickers
        swap(edgeStickers[5], edgeStickers[13]);
        swap(edgeStickers[9], edgeStickers[17]);
        swap(edgeStickers[7], edgeStickers[15]);
        swap(edgeStickers[11], edgeStickers[19]);
    }
    // E' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 90.0);
        rotation[pieces[2]] = YAxisRotation(rotation[pieces[2]], 90.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 90.0);
        rotation[pieces[4]] = YAxisRotation(rotation[pieces[4]], 90.0);
        rotation[pieces[10]] = YAxisRotation(rotation[pieces[10]], 90.0);
        rotation[pieces[11]] = YAxisRotation(rotation[pieces[11]], 90.0);
        rotation[pieces[12]] = YAxisRotation(rotation[pieces[12]], 90.0);
        rotation[pieces[13]] = YAxisRotation(rotation[pieces[13]], 90.0);
        rotation[pieces[1]] = ZAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[2]] = ZAxisRotation(rotation[pieces[2]], 22.0);
        rotation[pieces[3]] = ZAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[4]] = ZAxisRotation(rotation[pieces[4]], 22.0);
        rotation[pieces[10]] = ZAxisRotation(rotation[pieces[10]], 22.0);
        rotation[pieces[11]] = ZAxisRotation(rotation[pieces[11]], 22.0);
        rotation[pieces[12]] = ZAxisRotation(rotation[pieces[12]], 22.0);
        rotation[pieces[13]] = ZAxisRotation(rotation[pieces[13]], 22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[2]] = XAxisRotation(rotation[pieces[2]], -22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[4]] = XAxisRotation(rotation[pieces[4]], -22.0);
        rotation[pieces[10]] = XAxisRotation(rotation[pieces[10]], -22.0);
        rotation[pieces[11]] = XAxisRotation(rotation[pieces[11]], -22.0);
        rotation[pieces[12]] = XAxisRotation(rotation[pieces[12]], -22.0);
        rotation[pieces[13]] = XAxisRotation(rotation[pieces[13]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[1], pieces[4], pieces[3], pieces[2]);
        swapPieces(pieces[10], pieces[13], pieces[12], pieces[11]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[5], edgeStickers[17], edgeStickers[13], edgeStickers[9]);
        swapPieces(edgeStickers[7], edgeStickers[19], edgeStickers[15], edgeStickers[11]);
    }
}

// turn S slice
void turnSSlice(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // S move
    if (degree == 1)
    {
        // transform cube
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -32.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -32.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -32.0);
        rotation[pieces[0]] = ZAxisRotation(rotation[pieces[0]], 90.0);
        rotation[pieces[3]] = ZAxisRotation(rotation[pieces[3]], 90.0);
        rotation[pieces[5]] = ZAxisRotation(rotation[pieces[5]], 90.0);
        rotation[pieces[1]] = ZAxisRotation(rotation[pieces[1]], 90.0);
        rotation[pieces[14]] = ZAxisRotation(rotation[pieces[14]], 90.0);
        rotation[pieces[15]] = ZAxisRotation(rotation[pieces[15]], 90.0);
        rotation[pieces[16]] = ZAxisRotation(rotation[pieces[16]], 90.0);
        rotation[pieces[17]] = ZAxisRotation(rotation[pieces[17]], 90.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 32.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 32.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[0], pieces[3], pieces[5], pieces[1]);
        swapPieces(pieces[14], pieces[15], pieces[16], pieces[17]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[1], edgeStickers[14], edgeStickers[23], edgeStickers[4]);
        swapPieces(edgeStickers[3], edgeStickers[12], edgeStickers[21], edgeStickers[6]);
    }
    // S2 move
    else if (degree == 2)
    {
        // transform cube
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -32.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -32.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -32.0);
        rotation[pieces[0]] = ZAxisRotation(rotation[pieces[0]], 180.0);
        rotation[pieces[3]] = ZAxisRotation(rotation[pieces[3]], 180.0);
        rotation[pieces[5]] = ZAxisRotation(rotation[pieces[5]], 180.0);
        rotation[pieces[1]] = ZAxisRotation(rotation[pieces[1]], 180.0);
        rotation[pieces[14]] = ZAxisRotation(rotation[pieces[14]], 180.0);
        rotation[pieces[15]] = ZAxisRotation(rotation[pieces[15]], 180.0);
        rotation[pieces[16]] = ZAxisRotation(rotation[pieces[16]], 180.0);
        rotation[pieces[17]] = ZAxisRotation(rotation[pieces[17]], 180.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 32.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 32.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        // save new positions of pieces
        swap(pieces[0], pieces[5]);
        swap(pieces[3], pieces[1]);
        swap(pieces[14], pieces[16]);
        swap(pieces[15], pieces[17]);
        // save new positions of edge stickers
        swap(edgeStickers[1], edgeStickers[23]);
        swap(edgeStickers[14], edgeStickers[4]);
        swap(edgeStickers[3], edgeStickers[21]);
        swap(edgeStickers[12], edgeStickers[6]);
    }
    // S' move
    else if (degree == 3)
    {
        // transform cube
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], 22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], 22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], 22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], 22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], 22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], 22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], 22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], 22.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], -32.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], -32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], -32.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], -32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], -32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], -32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], -32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], -32.0);
        rotation[pieces[0]] = ZAxisRotation(rotation[pieces[0]], -90.0);
        rotation[pieces[3]] = ZAxisRotation(rotation[pieces[3]], -90.0);
        rotation[pieces[5]] = ZAxisRotation(rotation[pieces[5]], -90.0);
        rotation[pieces[1]] = ZAxisRotation(rotation[pieces[1]], -90.0);
        rotation[pieces[14]] = ZAxisRotation(rotation[pieces[14]], -90.0);
        rotation[pieces[15]] = ZAxisRotation(rotation[pieces[15]], -90.0);
        rotation[pieces[16]] = ZAxisRotation(rotation[pieces[16]], -90.0);
        rotation[pieces[17]] = ZAxisRotation(rotation[pieces[17]], -90.0);
        rotation[pieces[0]] = YAxisRotation(rotation[pieces[0]], 32.0);
        rotation[pieces[3]] = YAxisRotation(rotation[pieces[3]], 32.0);
        rotation[pieces[5]] = YAxisRotation(rotation[pieces[5]], 32.0);
        rotation[pieces[1]] = YAxisRotation(rotation[pieces[1]], 32.0);
        rotation[pieces[14]] = YAxisRotation(rotation[pieces[14]], 32.0);
        rotation[pieces[15]] = YAxisRotation(rotation[pieces[15]], 32.0);
        rotation[pieces[16]] = YAxisRotation(rotation[pieces[16]], 32.0);
        rotation[pieces[17]] = YAxisRotation(rotation[pieces[17]], 32.0);
        rotation[pieces[0]] = XAxisRotation(rotation[pieces[0]], -22.0);
        rotation[pieces[3]] = XAxisRotation(rotation[pieces[3]], -22.0);
        rotation[pieces[5]] = XAxisRotation(rotation[pieces[5]], -22.0);
        rotation[pieces[1]] = XAxisRotation(rotation[pieces[1]], -22.0);
        rotation[pieces[14]] = XAxisRotation(rotation[pieces[14]], -22.0);
        rotation[pieces[15]] = XAxisRotation(rotation[pieces[15]], -22.0);
        rotation[pieces[16]] = XAxisRotation(rotation[pieces[16]], -22.0);
        rotation[pieces[17]] = XAxisRotation(rotation[pieces[17]], -22.0);
        // save new positions of pieces
        swapPieces(pieces[0], pieces[1], pieces[5], pieces[3]);
        swapPieces(pieces[14], pieces[17], pieces[16], pieces[15]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[1], edgeStickers[4], edgeStickers[23], edgeStickers[14]);
        swapPieces(edgeStickers[3], edgeStickers[6], edgeStickers[21], edgeStickers[12]);
    }
}

// rotate cube about x-axis
void rotateCubeX(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // X rotation
    if (degree == 1)
    {
        // rotate cube
        for (int i = 0; i < 26; i++)
        {
            rotation[i] = XAxisRotation(rotation[i], 22.0);
            rotation[i] = YAxisRotation(rotation[i], -32.0);
            rotation[i] = XAxisRotation(rotation[i], 90.0);
            rotation[i] = YAxisRotation(rotation[i], 32.0);
            rotation[i] = XAxisRotation(rotation[i], -22.0);
        }
        // save new positions of pieces
        swapPieces(pieces[14], pieces[13], pieces[17], pieces[10]);
        swapPieces(pieces[21], pieces[18], pieces[22], pieces[25]);
        swapPieces(pieces[0], pieces[4], pieces[5], pieces[2]);
        swapPieces(pieces[6], pieces[7], pieces[8], pieces[9]);
        swapPieces(pieces[15], pieces[12], pieces[16], pieces[11]);
        swapPieces(pieces[20], pieces[19], pieces[23], pieces[24]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[3], edgeStickers[17], edgeStickers[23], edgeStickers[11]);
        swapPieces(edgeStickers[4], edgeStickers[7], edgeStickers[6], edgeStickers[5]);
        swapPieces(edgeStickers[0], edgeStickers[18], edgeStickers[20], edgeStickers[8]);
        swapPieces(edgeStickers[2], edgeStickers[16], edgeStickers[22], edgeStickers[10]);
        swapPieces(edgeStickers[1], edgeStickers[19], edgeStickers[21], edgeStickers[9]);
        swapPieces(edgeStickers[12], edgeStickers[13], edgeStickers[14], edgeStickers[15]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[3], cornerStickers[17], cornerStickers[23], cornerStickers[11]);
        swapPieces(cornerStickers[4], cornerStickers[7], cornerStickers[6], cornerStickers[5]);
        swapPieces(cornerStickers[0], cornerStickers[18], cornerStickers[20], cornerStickers[8]);
        swapPieces(cornerStickers[1], cornerStickers[19], cornerStickers[21], cornerStickers[9]);
        swapPieces(cornerStickers[12], cornerStickers[13], cornerStickers[14], cornerStickers[15]);
        swapPieces(cornerStickers[2], cornerStickers[16], cornerStickers[22], cornerStickers[10]);
    }
    // X' rotation
    else if (degree == 2)
    {
        // rotate cube
        for (int i = 0; i < 26; i++)
        {
            rotation[i] = XAxisRotation(rotation[i], 22.0);
            rotation[i] = YAxisRotation(rotation[i], -32.0);
            rotation[i] = XAxisRotation(rotation[i], -90.0);
            rotation[i] = YAxisRotation(rotation[i], 32.0);
            rotation[i] = XAxisRotation(rotation[i], -22.0);
        }
        // save new positions of pieces
        swapPieces(pieces[14], pieces[10], pieces[17], pieces[13]);
        swapPieces(pieces[21], pieces[25], pieces[22], pieces[18]);
        swapPieces(pieces[0], pieces[2], pieces[5], pieces[4]);
        swapPieces(pieces[6], pieces[9], pieces[8], pieces[7]);
        swapPieces(pieces[15], pieces[11], pieces[16], pieces[12]);
        swapPieces(pieces[20], pieces[24], pieces[23], pieces[19]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[3], edgeStickers[11], edgeStickers[23], edgeStickers[17]);
        swapPieces(edgeStickers[4], edgeStickers[5], edgeStickers[6], edgeStickers[7]);
        swapPieces(edgeStickers[0], edgeStickers[8], edgeStickers[20], edgeStickers[18]);
        swapPieces(edgeStickers[2], edgeStickers[10], edgeStickers[22], edgeStickers[16]);
        swapPieces(edgeStickers[1], edgeStickers[9], edgeStickers[21], edgeStickers[19]);
        swapPieces(edgeStickers[12], edgeStickers[15], edgeStickers[14], edgeStickers[13]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[3], cornerStickers[11], cornerStickers[23], cornerStickers[17]);
        swapPieces(cornerStickers[4], cornerStickers[5], cornerStickers[6], cornerStickers[7]);
        swapPieces(cornerStickers[0], cornerStickers[8], cornerStickers[20], cornerStickers[18]);
        swapPieces(cornerStickers[2], cornerStickers[10], cornerStickers[22], cornerStickers[16]);
        swapPieces(cornerStickers[1], cornerStickers[9], cornerStickers[21], cornerStickers[19]);
        swapPieces(cornerStickers[12], cornerStickers[15], cornerStickers[14], cornerStickers[13]);
    }
}

// rotate cube about y-axis
void rotateCubeY(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // Y rotation
    if (degree == 1)
    {
        // rotate cube
        for (int i = 0; i < 26; i++)
        {
            rotation[i] = XAxisRotation(rotation[i], 22.0);
            rotation[i] = YAxisRotation(rotation[i], -32.0);
            rotation[i] = YAxisRotation(rotation[i], 90.0);
            rotation[i] = YAxisRotation(rotation[i], 32.0);
            rotation[i] = XAxisRotation(rotation[i], -22.0);
        }
        // save new positions of pieces
        swapPieces(pieces[7], pieces[15], pieces[6], pieces[14]);
        swapPieces(pieces[18], pieces[19], pieces[20], pieces[21]);
        swapPieces(pieces[1], pieces[4], pieces[3], pieces[2]);
        swapPieces(pieces[10], pieces[13], pieces[12], pieces[11]);
        swapPieces(pieces[8], pieces[16], pieces[9], pieces[17]);
        swapPieces(pieces[22], pieces[23], pieces[24], pieces[25]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[0], edgeStickers[1], edgeStickers[2], edgeStickers[3]);
        swapPieces(edgeStickers[16], edgeStickers[12], edgeStickers[8], edgeStickers[4]);
        swapPieces(edgeStickers[5], edgeStickers[17], edgeStickers[13], edgeStickers[9]);
        swapPieces(edgeStickers[7], edgeStickers[19], edgeStickers[15], edgeStickers[11]);
        swapPieces(edgeStickers[20], edgeStickers[23], edgeStickers[22], edgeStickers[21]);
        swapPieces(edgeStickers[6], edgeStickers[18], edgeStickers[14], edgeStickers[10]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[0], cornerStickers[1], cornerStickers[2], cornerStickers[3]);
        swapPieces(cornerStickers[16], cornerStickers[12], cornerStickers[8], cornerStickers[4]);
        swapPieces(cornerStickers[17], cornerStickers[13], cornerStickers[9], cornerStickers[5]);
        swapPieces(cornerStickers[20], cornerStickers[23], cornerStickers[22], cornerStickers[21]);
        swapPieces(cornerStickers[6], cornerStickers[18], cornerStickers[14], cornerStickers[10]);
        swapPieces(cornerStickers[7], cornerStickers[19], cornerStickers[15], cornerStickers[11]);
    }
    // Y' rotation
    else if (degree == 2)
    {
        // rotate cube
        for (int i = 0; i < 26; i++)
        {
            rotation[i] = XAxisRotation(rotation[i], 22.0);
            rotation[i] = YAxisRotation(rotation[i], -32.0);
            rotation[i] = YAxisRotation(rotation[i], -90.0);
            rotation[i] = YAxisRotation(rotation[i], 32.0);
            rotation[i] = XAxisRotation(rotation[i], -22.0);
        }
        // save new positions of pieces
        swapPieces(pieces[7], pieces[14], pieces[6], pieces[15]);
        swapPieces(pieces[18], pieces[21], pieces[20], pieces[19]);
        swapPieces(pieces[1], pieces[2], pieces[3], pieces[4]);
        swapPieces(pieces[10], pieces[11], pieces[12], pieces[13]);
        swapPieces(pieces[8], pieces[17], pieces[9], pieces[16]);
        swapPieces(pieces[22], pieces[25], pieces[24], pieces[23]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[0], edgeStickers[3], edgeStickers[2], edgeStickers[1]);
        swapPieces(edgeStickers[16], edgeStickers[4], edgeStickers[8], edgeStickers[12]);
        swapPieces(edgeStickers[5], edgeStickers[9], edgeStickers[13], edgeStickers[17]);
        swapPieces(edgeStickers[7], edgeStickers[11], edgeStickers[15], edgeStickers[19]);
        swapPieces(edgeStickers[20], edgeStickers[21], edgeStickers[22], edgeStickers[23]);
        swapPieces(edgeStickers[6], edgeStickers[10], edgeStickers[14], edgeStickers[18]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[0], cornerStickers[3], cornerStickers[2], cornerStickers[1]);
        swapPieces(cornerStickers[16], cornerStickers[4], cornerStickers[8], cornerStickers[12]);
        swapPieces(cornerStickers[5], cornerStickers[9], cornerStickers[13], cornerStickers[17]);
        swapPieces(cornerStickers[7], cornerStickers[11], cornerStickers[15], cornerStickers[19]);
        swapPieces(cornerStickers[20], cornerStickers[21], cornerStickers[22], cornerStickers[23]);
        swapPieces(cornerStickers[6], cornerStickers[10], cornerStickers[14], cornerStickers[18]);
    }
}

// rotate cube about z-axis
void rotateCubeZ(int degree, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // Z rotation
    if (degree == 1)
    {
        // rotate cube
        for (int i = 0; i < 26; i++)
        {
            rotation[i] = XAxisRotation(rotation[i], 22.0);
            rotation[i] = YAxisRotation(rotation[i], -32.0);
            rotation[i] = ZAxisRotation(rotation[i], 90.0);
            rotation[i] = YAxisRotation(rotation[i], 32.0);
            rotation[i] = XAxisRotation(rotation[i], -22.0);
        }
        // save new positions of pieces
        swapPieces(pieces[6], pieces[11], pieces[9], pieces[10]);
        swapPieces(pieces[20], pieces[24], pieces[25], pieces[21]);
        swapPieces(pieces[0], pieces[3], pieces[5], pieces[1]);
        swapPieces(pieces[14], pieces[15], pieces[16], pieces[17]);
        swapPieces(pieces[7], pieces[12], pieces[8], pieces[13]);
        swapPieces(pieces[18], pieces[19], pieces[23], pieces[22]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[8], edgeStickers[9], edgeStickers[10], edgeStickers[11]);
        swapPieces(edgeStickers[2], edgeStickers[15], edgeStickers[20], edgeStickers[5]);
        swapPieces(edgeStickers[1], edgeStickers[14], edgeStickers[23], edgeStickers[4]);
        swapPieces(edgeStickers[3], edgeStickers[12], edgeStickers[21], edgeStickers[6]);
        swapPieces(edgeStickers[16], edgeStickers[19], edgeStickers[18], edgeStickers[17]);
        swapPieces(edgeStickers[0], edgeStickers[13], edgeStickers[22], edgeStickers[7]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[8], cornerStickers[9], cornerStickers[10], cornerStickers[11]);
        swapPieces(cornerStickers[2], cornerStickers[15], cornerStickers[20], cornerStickers[5]);
        swapPieces(cornerStickers[3], cornerStickers[12], cornerStickers[21], cornerStickers[6]);
        swapPieces(cornerStickers[16], cornerStickers[19], cornerStickers[18], cornerStickers[17]);
        swapPieces(cornerStickers[0], cornerStickers[13], cornerStickers[22], cornerStickers[7]);
        swapPieces(cornerStickers[1], cornerStickers[14], cornerStickers[23], cornerStickers[4]);
    }
    // Z' rotation
    else if (degree == 2)
    {
        // rotate cube
        for (int i = 0; i < 26; i++)
        {
            rotation[i] = XAxisRotation(rotation[i], 22.0);
            rotation[i] = YAxisRotation(rotation[i], -32.0);
            rotation[i] = ZAxisRotation(rotation[i], -90.0);
            rotation[i] = YAxisRotation(rotation[i], 32.0);
            rotation[i] = XAxisRotation(rotation[i], -22.0);
        }
        // save new positions of pieces
        swapPieces(pieces[6], pieces[10], pieces[9], pieces[11]);
        swapPieces(pieces[20], pieces[21], pieces[25], pieces[24]);
        swapPieces(pieces[0], pieces[1], pieces[5], pieces[3]);
        swapPieces(pieces[14], pieces[17], pieces[16], pieces[15]);
        swapPieces(pieces[7], pieces[13], pieces[8], pieces[12]);
        swapPieces(pieces[18], pieces[22], pieces[23], pieces[19]);
        // save new positions of edge stickers
        swapPieces(edgeStickers[8], edgeStickers[11], edgeStickers[10], edgeStickers[9]);
        swapPieces(edgeStickers[2], edgeStickers[5], edgeStickers[20], edgeStickers[15]);
        swapPieces(edgeStickers[1], edgeStickers[4], edgeStickers[23], edgeStickers[14]);
        swapPieces(edgeStickers[3], edgeStickers[6], edgeStickers[21], edgeStickers[12]);
        swapPieces(edgeStickers[16], edgeStickers[17], edgeStickers[18], edgeStickers[19]);
        swapPieces(edgeStickers[0], edgeStickers[7], edgeStickers[22], edgeStickers[13]);
        // save new positions of corner stickers
        swapPieces(cornerStickers[8], cornerStickers[11], cornerStickers[10], cornerStickers[9]);
        swapPieces(cornerStickers[2], cornerStickers[5], cornerStickers[20], cornerStickers[15]);
        swapPieces(cornerStickers[3], cornerStickers[6], cornerStickers[21], cornerStickers[12]);
        swapPieces(cornerStickers[16], cornerStickers[17], cornerStickers[18], cornerStickers[19]);
        swapPieces(cornerStickers[0], cornerStickers[7], cornerStickers[22], cornerStickers[13]);
        swapPieces(cornerStickers[1], cornerStickers[4], cornerStickers[23], cornerStickers[14]);
    }
}

// check if cube is solved
bool isSolved(string check, int* pieces, int* edgeStickers, int* cornerStickers)
{
    // declare variable that contains whether or not the cube is solved
    bool solved = true;
    // check if edges are solved
    if (check == "CUBE" || check == "EDGES")
    {
        // check if edges are solved
        for (int i = 0; i < 6; i++)
        {
            if (edgeStickers[4 * i] + edgeStickers[4 * i + 1] + edgeStickers[4 * i + 2] + edgeStickers[4 * i + 3] != 16 * pieces[i] + 6)
            {
                solved = false;
                break;
            }
        }
    }
    // check if corners are solved
    if (check == "CUBE" || check == "CORNERS")
    {
        for (int i = 0; i < 6; i++)
        {
            if (cornerStickers[4 * i] + cornerStickers[4 * i + 1] + cornerStickers[4 * i + 2] + cornerStickers[4 * i + 3] != 16 * pieces[i] + 6)
            {
                solved = false;
                break;
            }
        }
    }

    return solved;
}

// perform edge swap algorithm
void edgeSwap(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // perform algorithm and render cubes after each turn
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
}

// perform corner swap algorithm
void cornerSwap(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // perform algorithm and render cubes after each turn
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
}

// perform parity algorithm
void parity(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(2, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
    turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
    renderCubes(window, cubes, core_program, proj, view, rotation);
    delay(0.2);
}

// orient cube
void orientCube(int frontFace, int topFace, GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // orient front face
    for (int i = 0; i < 3; i++)
    {
        // check if front face is already oriented
        if (pieces[2] == frontFace)
        {
            break;
        }
        // rotate cube
        else
        {
            rotateCubeY(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
    }
    // orient front face if it is on the top or bottom
    if (pieces[2] != frontFace)
    {
        rotateCubeX(1, pieces, edgeStickers, cornerStickers, rotation);
    }
    if (pieces[2] != frontFace)
    {
        rotateCubeX(2, pieces, edgeStickers, cornerStickers, rotation);
    }

    // orient top face
    for (int i = 0; i < 3; i++)
    {
        // check if top face is already oriented
        if (pieces[0] == topFace)
        {
            break;
        }
        // rotate cube
        else
        {
            rotateCubeZ(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
    }
}

// solve edges
void solveEdges(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // check if edges are already solved
    if (isSolved("EDGES", pieces, edgeStickers, cornerStickers) == false)
    {
        int target;
        // find target piece if edge buffer piece is in buffer position
        if (pieces[15] == 15)
        {
            // choose an unsolved piece as target
            for (int i = 0; i < 24; i++)
            {
                if (edgeStickers[i] != i && i != 1 && i != 12)
                {
                    target = i;
                    break;
                }
            }
        }
        // find target piece if edge buffer piece is not in buffer position
        else
        {
            // find target piece
            target = edgeStickers[1];
        }
        // move target piece into setup position
        if (target == 0)
        {
            turnMSlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 2)
        {
            turnMSlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 4)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 5)
        {
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 6)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 7)
        {
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 8)
        {
            turnMSlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 9)
        {
            turnESlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 10)
        {
            turnMSlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 11)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 13)
        {
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 14)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 15)
        {
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 16)
        {
            turnMSlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 17)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 18)
        {
            turnMSlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 19)
        {
            turnESlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 20)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 21)
        {
            turnDFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 22)
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 23)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        // perform edge swap algorithm
        edgeSwap(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
        // undo moves that were performed to move piece into setup position
        if (target == 0)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 2)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 4)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 5)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 6)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 7)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 8)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 9)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 10)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 11)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 13)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 14)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 15)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 16)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 17)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 18)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnMSlice(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 19)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnESlice(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 20)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 21)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 22)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 23)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        // solve next edge piece
        solveEdges(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
    }
}

// solve corners
void solveCorners(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // check if corners are already solved
    if (isSolved("CORNERS", pieces, edgeStickers, cornerStickers) == false)
    {
        int target;
        // find target piece if corner buffer piece is in buffer position
        if (pieces[18] == 18)
        {
            // choose an unsolved piece as target
            for (int i = 0; i < 24; i++)
            {
                if (cornerStickers[i] != i && i != 0 && i != 4 && i != 17)
                {
                    target = i;
                    break;
                }
            }
        }
        // find target piece if corner buffer piece is not in buffer position
        else
        {
            // find target piece
            target = cornerStickers[4];
        }
        // move target piece into setup position
        if (target == 1)
        {
            turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 2)
        {
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 3)
        {
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 5)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 6)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 7)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 8)
        {
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 9)
        {
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 10)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 11)
        {
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 12)
        {
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 13)
        {
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 14)
        {
            turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 15)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 16)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 18)
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 19)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 20)
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 22)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 23)
        {
            turnDFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        // perform corner swap algorithm
        cornerSwap(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
        // undo moves that were performed to move piece into setup position
        if (target == 1)
        {
            turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 2)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 3)
        {
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 5)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 6)
        {
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 7)
        {
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 8)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 9)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 10)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 11)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 12)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 13)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 14)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 15)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 16)
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 18)
        {
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 19)
        {
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 20)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 22)
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        else if (target == 23)
        {
            turnDFace(2, pieces, edgeStickers, cornerStickers, rotation);
            renderCubes(window, cubes, core_program, proj, view, rotation);
            delay(0.2);
        }
        // solve next corner piece
        solveCorners(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
    }
}

// solve cube
void solveCube(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // check if cube is already solved
    if (isSolved("CUBE", pieces, edgeStickers, cornerStickers) == false)
    {
        // orient cube so that green face is at front and white face is on top
        orientCube(2, 0, window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
        // solve edges
        solveEdges(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
        // check if parity has occured
        int unsolvedCorners = 0;
        for (int i = 18; i < 26; i++)
        {
            if (pieces[i] != i)
            {
                unsolvedCorners++;
            }
        }
        // perform parity algorithm if needed
        if (unsolvedCorners % 2 == 1)
        {
            parity(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
        }
        // solve corners
        solveCorners(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
    }
}

// scramble cube
void scrambleCube(int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26])
{
    // randomly generate moves
    vector <int> moves;
    for (int i = 0; i < 25; i++)
    {
        moves.push_back(rand() % 18 + 1);
    }

    // perform randomly generated moves
    for (int i = 0; i < 25; i++)
    {
        if (moves[i] == 1)
        {
            turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 2)
        {
            turnUFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 3)
        {
            turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 4)
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 5)
        {
            turnDFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 6)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 7)
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 8)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 9)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 10)
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 11)
        {
            turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 12)
        {
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 13)
        {
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 14)
        {
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 15)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 16)
        {
            turnBFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 17)
        {
            turnBFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        else if (moves[i] == 18)
        {
            turnBFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
    }
}

// update input
void updateInput(GLFWwindow* window, int(&pieces)[26], int(&edgeStickers)[24], int(&cornerStickers)[24], glm::mat4(&rotation)[26], Cube* cubes, GLuint core_program, glm::mat4 proj, glm::mat4 view)
{
    // exit window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // turn U face
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        // U2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnUFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // U' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnUFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // U move
        else
        {
            turnUFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn D face
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        // D2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnDFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // D' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnDFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // D move
        else
        {
            turnDFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn L face
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // L2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnLFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // L' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnLFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // L move
        else
        {
            turnLFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn R face
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        // R2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnRFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // R' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnRFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // R move
        else
        {
            turnRFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn F face
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        // F2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnFFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // F' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnFFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // F move
        else
        {
            turnFFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn B face
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        // B2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnBFace(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // B' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnBFace(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // B move
        else
        {
            turnBFace(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn M slice
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        // M2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnMSlice(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // M' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnMSlice(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // M move
        else
        {
            turnMSlice(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn E slice
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        // E2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnESlice(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // E' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnESlice(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // E move
        else
        {
            turnESlice(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // turn S slice
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        // S2 move
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
        {
            turnSSlice(2, pieces, edgeStickers, cornerStickers, rotation);
        }
        // S' move
        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
        {
            turnSSlice(3, pieces, edgeStickers, cornerStickers, rotation);
        }
        // S move
        else
        {
            turnSSlice(1, pieces, edgeStickers, cornerStickers, rotation);
        }
        delay(0.2);
    }
    // rotate cube clockwise about x-axis
    if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
    {
        rotateCubeX(1, pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
    // rotate cube counter-clockwise about x-axis
    if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        rotateCubeX(2, pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
    // rotate cube clockwise about y-axis
    if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        rotateCubeY(1, pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
    // rotate cube counter-clockwise about y-axis
    if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        rotateCubeY(2, pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
    // rotate cube clockwise about z-axis
    if (glfwGetKey(window, GLFW_KEY_KP_9) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
    {
        rotateCubeZ(1, pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
    // rotate cube counter-clockwise about z-axis
    if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
    {
        rotateCubeZ(2, pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
    // solve cube
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        solveCube(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);
        delay(0.2);
    }
    // randomly scramble cube
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        scrambleCube(pieces, edgeStickers, cornerStickers, rotation);
        delay(0.2);
    }
}

// load vertices from file
vector < vector <glm::vec3> > loadVertices(string keyword)
{
    // declare variables
    ifstream file;
    vector < vector <glm::vec3> > vertices;
    vector <glm::vec3> tempVect;
    string line;
    stringstream ss;
    float x;
    float y;
    float z;
    int component;

    // open file
    file.open("vertices.txt");

    // find keyword
    while (line.find(keyword) == string::npos)
    {
        getline(file, line);
    }

    // push vertices from file to vector
    for (int i = 0; i < 6; i++)
    {
        // get vertices for each square on a cube
        for (int j = 0; j < 4; j++)
        {
            component = 0;
            getline(file, line);
            // determine x, y, and z components of vertex
            for (int k = 0; k < line.length(); k++)
            {
                if (line[k] == ' ')
                {
                    component++;
                    if (component == 1)
                    {
                        ss >> x;
                        ss.str("");
                    }
                    else if (component == 2)
                    {
                        ss >> y;
                        ss.str("");
                    }
                    else if (component == 3)
                    {
                        ss >> z;
                        ss.str("");
                    }
                }
                else
                {
                    ss << line[k];
                }
            }
            tempVect.push_back(glm::vec3(x, y, z));
        }
        vertices.push_back(tempVect);
        tempVect.clear();
    }

    return vertices;
}

// load shaders
bool loadShaders(GLuint& program)
{
    // declare variables
    bool loadSuccess = true;
    char infoLog[512];
    GLint success;
    string temp = "";
    string src = "";
    ifstream in_file;

    // load vertex shader
    in_file.open("vertex_core.glsl");
    if (in_file.is_open())
    {
        while (getline(in_file, temp))
        {
            src += temp + "\n";
        }
    }
    else
    {
        loadSuccess = false;
        cout << "ERROR::LOADSHADERS::COULD_NOT_OPEN_VERTEX_FILE" << endl;
    }
    in_file.close();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertSrc = src.c_str();
    glShaderSource(vertexShader, 1, &vertSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == false)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        loadSuccess = false;
        cout << "ERROR::LOADSHADERS::COULD_NOT_COMPILE_VERTEX_FILE" << endl;
        cout << infoLog << endl;
    }
    temp = "";
    src = "";

    // load fragment shader
    in_file.open("fragment_core.glsl");

    if (in_file.is_open())
    {
        while (getline(in_file, temp))
        {
            src += temp + "\n";
        }
    }
    else
    {
        loadSuccess = false;
        cout << "ERROR::LOADSHADERS::COULD_NOT_OPEN_FRAGMENT_FILE" << endl;
    }
    in_file.close();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragSrc = src.c_str();
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == false)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        loadSuccess = false;
        cout << "ERROR::LOADSHADERS::COULD_NOT_COMPILE_FRAGMENT_FILE" << endl;
        cout << infoLog << endl;
    }

    // create program and attach shaders
    program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == false)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        loadSuccess = false;
        cout << "ERROR::LOADSHADERS::COULD_NOT_LINK_PROGRAM" << endl;
        cout << infoLog << endl;
    }

    // end
    glUseProgram(0);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return loadSuccess;
}

int main()
{
    // initialize GLFW
    glfwInit();

    // seed random number generator
    srand(time(NULL));

    // create window
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 480;

    // declare openGL version
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // determine whether window is resizable or not
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Rubik's Cube Solver", NULL, NULL);

    // make context current
    glfwMakeContextCurrent(window);

    // enable openGL to test for depth
    glEnable(GL_DEPTH_TEST);

    // initialize GLEW
    glewInit();

    // declare vertices
    // vertices of centre pieces
    vector < vector <glm::vec3> > UCentreVertices = loadVertices("UCentre");
    vector < vector <glm::vec3> > DCentreVertices = loadVertices("DCentre");
    vector < vector <glm::vec3> > LCentreVertices = loadVertices("LCentre");
    vector < vector <glm::vec3> > RCentreVertices = loadVertices("RCentre");
    vector < vector <glm::vec3> > FCentreVertices = loadVertices("FCentre");
    vector < vector <glm::vec3> > BCentreVertices = loadVertices("BCentre");
    // vertices of edge pieces on M slice
    vector < vector <glm::vec3> > UFEdgeVertices = loadVertices("UFEdge");
    vector < vector <glm::vec3> > DFEdgeVertices = loadVertices("DFEdge");
    vector < vector <glm::vec3> > UBEdgeVertices = loadVertices("UBEdge");
    vector < vector <glm::vec3> > DBEdgeVertices = loadVertices("DBEdge");
    // vertices of edge pieces on E slice
    vector < vector <glm::vec3> > LFEdgeVertices = loadVertices("LFEdge");
    vector < vector <glm::vec3> > RFEdgeVertices = loadVertices("RFEdge");
    vector < vector <glm::vec3> > LBEdgeVertices = loadVertices("LBEdge");
    vector < vector <glm::vec3> > RBEdgeVertices = loadVertices("RBEdge");
    // vertices of edge pieces on S slice
    vector < vector <glm::vec3> > LUEdgeVertices = loadVertices("LUEdge");
    vector < vector <glm::vec3> > RUEdgeVertices = loadVertices("RUEdge");
    vector < vector <glm::vec3> > LDEdgeVertices = loadVertices("LDEdge");
    vector < vector <glm::vec3> > RDEdgeVertices = loadVertices("RDEdge");
    // vertices of corner pieces on U face
    vector < vector <glm::vec3> > LUFCornerVertices = loadVertices("LUFCorner");
    vector < vector <glm::vec3> > RUFCornerVertices = loadVertices("RUFCorner");
    vector < vector <glm::vec3> > LUBCornerVertices = loadVertices("LUBCorner");
    vector < vector <glm::vec3> > RUBCornerVertices = loadVertices("RUBCorner");
    // vertices of corner pieces on D face
    vector < vector <glm::vec3> > LDFCornerVertices = loadVertices("LDFCorner");
    vector < vector <glm::vec3> > RDFCornerVertices = loadVertices("RDFCorner");
    vector < vector <glm::vec3> > LDBCornerVertices = loadVertices("LDBCorner");
    vector < vector <glm::vec3> > RDBCornerVertices = loadVertices("RDBCorner");

    // declare array for positions of pieces of cube
    int pieces[26] =
    {
        0, 1, 2, 3, 4, 5, // centre pieces
        6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, // edge pieces
        18, 19, 20, 21, 22, 23, 24, 25 // corner pieces
    };

    // declare arrays for positions of stickers of cube
    int edgeStickers[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 }; // edge stickers
    int cornerStickers[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 }; // corner stickers

    // declare array for rotation of pieces of cube
    glm::mat4 rotation[26];
    for (int i = 0; i < 26; i++)
    {
        rotation[i] = glm::mat4(1.0);
        rotation[i] = YAxisRotation(rotation[i], 32.0);
        rotation[i] = XAxisRotation(rotation[i], -22.0);
    }

    // create and load shaders
    GLuint core_program;
    if (loadShaders(core_program) == false)
    {
        glfwTerminate();
    }

    // create projection matrix
    float fov = 90.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    glm::mat4 proj = glm::mat4(1.0f);
    proj = glm::perspective(glm::radians(fov), 640.0f / 480.0f, nearPlane, farPlane);

    // create view matrix
    glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 1.5f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(camPosition, camPosition + camFront, worldUp);

    // create cubes
    Cube cubes[26] =
    {
        Cube(UCentreVertices),
        Cube(LCentreVertices),
        Cube(FCentreVertices),
        Cube(RCentreVertices),
        Cube(BCentreVertices),
        Cube(DCentreVertices),
        Cube(UFEdgeVertices),
        Cube(UBEdgeVertices),
        Cube(DBEdgeVertices),
        Cube(DFEdgeVertices),
        Cube(LFEdgeVertices),
        Cube(RFEdgeVertices),
        Cube(RBEdgeVertices),
        Cube(LBEdgeVertices),
        Cube(LUEdgeVertices),
        Cube(RUEdgeVertices),
        Cube(RDEdgeVertices),
        Cube(LDEdgeVertices),
        Cube(LUBCornerVertices),
        Cube(RUBCornerVertices),
        Cube(RUFCornerVertices),
        Cube(LUFCornerVertices),
        Cube(LDBCornerVertices),
        Cube(RDBCornerVertices),
        Cube(RDFCornerVertices),
        Cube(LDFCornerVertices)
    };

    // display instructions
    cout << "INSTRUCTIONS" << endl;
    cout << "Press space to scramble the cube" << endl;
    cout << "Press enter to solve the cube" << endl;
    cout << "Press 8 to rotate the cube 90 degrees clockwise along the x-axis" << endl;
    cout << "Press 2 to rotate the cube 90 degrees counter-clockwise along the x-axis" << endl;
    cout << "Press 4 to rotate the cube 90 degrees clockwise along the y-axis" << endl;
    cout << "Press 6 to rotate the cube 90 degrees counter-clockwise along the y-axis" << endl;
    cout << "Press 9 to rotate the cube 90 degrees clockwise along the z-axis" << endl;
    cout << "Press 7 to rotate the cube 90 degrees counter-clockwise along the z-axis" << endl;
    cout << "Press the button associated with a face/slice to turn it 90 degrees clockwise" << endl;
    cout << "Hold shift and press the button associated with a face/slice to turn it 90 degrees counter-clockwise" << endl;
    cout << "Hold control and press the button associated with a face/slice to turn it 180 degrees" << endl;
    cout << "Below is a list of faces/slices and the button associated with them" << endl;
    cout << "--------------" << endl;
    cout << "L Face - A" << endl;
    cout << "R Face - D" << endl;
    cout << "U Face - W" << endl;
    cout << "D Face - S" << endl;
    cout << "F Face - E" << endl;
    cout << "B Face - Q" << endl;
    cout << "M Slice - Z" << endl;
    cout << "E Slice - X" << endl;
    cout << "S Slice - C" << endl;
    cout << "--------------" << endl;

    // main program loop
    while (!glfwWindowShouldClose(window))
    {
        // poll for events
        glfwPollEvents();

        // update input
        updateInput(window, pieces, edgeStickers, cornerStickers, rotation, cubes, core_program, proj, view);

        // use program
        glUseProgram(core_program);

        // render cubes
        renderCubes(window, cubes, core_program, proj, view, rotation);
    }

    // end program
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
