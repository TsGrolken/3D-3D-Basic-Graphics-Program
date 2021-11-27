// This example is heavily based on the tutorial at https://open.gl

// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include"MyObject.h"
#include"Controller_extra.h"
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

// Timer
#include <chrono>

// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_N;
// Contains the view transformation
glm::mat4 view;
glm::mat4  homography = glm::mat4(1.f);
glm::mat4 view_transform = glm::mat4(1.f);

Controller controller;
glm::vec3 color(0.3f, 0.3f, 0.3f);

double camera_x_rotate = 0;
double camera_y_rotate = 0;
double camera_z_rotate = 0;

glm::vec3 light(0.f,5.f,0.f);
// Contains the vertex positions
std::vector<glm::vec3> V(3);

std::vector<MyObject> objs;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // Get the position of the mouse in the window
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    glm::vec4 p_screen(xpos,height-1-ypos,-1,1);
    glm::vec4 p_canonical((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,-1,1);
    glm::vec4 p_world = glm::inverse(view) * p_canonical;

    double xworld = p_world.x;//((xpos/double(width))*2)-1;
    double yworld = p_world.y;//(((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw
    double zworld = p_world.z;//(((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw
    // Update the position of the first vertex if the left button is pressed
    if(controller.mode == TRANSLATION_MODE  && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        //controller.selected_item = FindSelectedObject3D(objs, xworld, yworld, zworld, view);
        int prev = controller.selected_item;
        controller.selected_item = FindSelectedObject3DUsingProjection(objs,p_canonical.x,p_canonical.y,view);
        if (controller.selected_item != -1)
        {

            controller.press_point = glm::vec2(xworld, yworld);
            controller.is_in_process = true;

        }
        else
            controller.is_in_process = false;
    }

    if(controller.mode == VIEW_MODE && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        controller.is_in_process = true;
        controller.press_point =glm::vec2 (xpos, ypos);
        controller.rotate = glm::vec3(camera_x_rotate, camera_y_rotate, camera_z_rotate);
    }

    if(controller.mode == VIEW_MODE && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        controller.is_in_process = false;
    }

}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // Get the size of the window
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Convert screen position to world coordinates
    // Convert screen position to world coordinates
    glm::vec4 p_screen(xpos,height-1-ypos,0,1);
    glm::vec4 p_canonical((p_screen.x/width)*2-1,(p_screen.y/height)*2-1,0,1);
    glm::vec4 p_world = glm::inverse(view) * p_canonical;

    double xworld = p_world.x;//((xpos/double(width))*2)-1;
    double yworld = p_world.y;//(((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    if(controller.mode==VIEW_MODE && controller.is_in_process == true)
    {
        glm::vec2 move = glm::vec2(xpos, ypos) - controller.press_point;
        camera_y_rotate -= move.x / 4;
        camera_x_rotate += move.y / 4;
        controller.press_point =glm::vec2 (xpos, ypos);
    }

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Update the position of the first vertex if the keys 1,2, or 3 are pressed
    switch (key)
    {
        case GLFW_KEY_O:
            if (controller.is_in_process )
                break;
            controller.mode = TRANSLATION_MODE;

            glfwSetWindowTitle(window, "HW3: translation mode");
            break;

        case GLFW_KEY_I:
            if (controller.is_in_process )
                break;
            controller.selected_item = -1;
            controller.mode = VIEW_MODE;
            glfwSetWindowTitle(window, "HW3: view mode");
            break;

        case GLFW_KEY_L:
            if (controller.is_in_process )
                break;
            controller.selected_item = -1;
            controller.mode = LIGHT_MODE;
            glfwSetWindowTitle(window, "HW3: light source mode");
            break;

        case  GLFW_KEY_1:
            if(action == GLFW_PRESS)
            {
                MyObject cube = MyObject(8, 12);
                cube.drawUnitCube();
                objs.push_back(cube);
                updateAllObjectsVBO(objs, VBO);
                updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case GLFW_KEY_2:
            if(action == GLFW_PRESS)
            {
                MyObject cube = MyObject("../data/bumpy_cube.off");
                objs.push_back(cube);
                updateAllObjectsVBO(objs, VBO);
                updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case  GLFW_KEY_3:
            if(action == GLFW_PRESS)
            {
                MyObject cube = MyObject("../data/bunny.off");
                objs.push_back(cube);
                updateAllObjectsVBO(objs, VBO);
                updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case GLFW_KEY_X:
            camera_x_rotate = 0;
             camera_y_rotate = 0;
            camera_z_rotate =0;
            if(controller.mode == LIGHT_MODE)
                light = glm::vec3(0.f,5.f,0.f);
            break;
        case GLFW_KEY_W:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].rotate(5,0,0);
                else
                    camera_x_rotate += 5;
            }

            break;

        case GLFW_KEY_S:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].rotate(-5,0,0);
                else
                    camera_x_rotate -= 5;
            }
            break;
        case GLFW_KEY_A:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].rotate(0,5,0);
                else
                    camera_y_rotate += 5;
            }
            break;

        case GLFW_KEY_D:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].rotate(0,-5,0);
                else
                    camera_y_rotate -= 5;
            }
            break;
        case GLFW_KEY_Q:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].rotate(0,0,5);
                else
                    camera_z_rotate += 5;
            }
            break;
        case GLFW_KEY_E:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].rotate(0,0,-5);
                else
                    camera_z_rotate -= 5;
            }
            break;
        case GLFW_KEY_KP_ADD:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].scale(1.05);
                else
                {
                    view_transform[0][0] += 0.05f;
                    view_transform[1][1] += 0.05f;
                    view_transform[2][2] += 0.05f;
                }
            }
            break;

        case GLFW_KEY_KP_SUBTRACT:
            if(action != GLFW_RELEASE)
            {
                if(controller.selected_item != -1)
                    objs[controller.selected_item].scale(0.95);
                else
                {
                    view_transform[0][0] -= 0.05f;
                    view_transform[1][1] -= 0.05f;
                    view_transform[2][2] -= 0.05f;
                }
            }
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            {
                glm::mat4 move = glm::mat4(
                        1.f, 0.f, 0.f, 0.f,
                        0.f, 1.f, 0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        0.f, 0.f, -2.5f, 1.f
                );

                glm::mat4 pers = glm::perspective(glm::radians(45.f), 1.f, 1.0f, 100.0f);
                homography = pers * move;
            }
            break;
        case GLFW_KEY_LEFT_BRACKET:
            homography = glm::ortho(-1.f,1.f,-1.f,1.f, -1.f, 1.f);
            break;
        case GLFW_KEY_KP_4:
            if(action != GLFW_RELEASE && controller.selected_item != -1&&controller.mode == TRANSLATION_MODE)
            {
                glm::mat4 t = glm::mat4 {
                    1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    -0.05,0,0,1
                };
                objs[controller.selected_item].transform(t);
            }
            if(action != GLFW_RELEASE &&controller.mode == LIGHT_MODE)
                light[0] -= 0.1f;
            break;
        case GLFW_KEY_KP_6:
            if(action != GLFW_RELEASE && controller.selected_item != -1&&controller.mode == TRANSLATION_MODE)
            {
                glm::mat4 t = glm::mat4 {
                        1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0.05,0,0,1
                };
                objs[controller.selected_item].transform(t);
            }
            if(action != GLFW_RELEASE &&controller.mode == LIGHT_MODE)
                light[0] += 0.1f;
            break;
        case GLFW_KEY_KP_8:
            if(action != GLFW_RELEASE && controller.selected_item != -1&&controller.mode == TRANSLATION_MODE)
            {
                glm::mat4 t = glm::mat4 {
                        1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0.05,0,1
                };
                objs[controller.selected_item].transform(t);
            }
            if(action != GLFW_RELEASE &&controller.mode == LIGHT_MODE)
                light[1] += 0.1f;
            break;
        case GLFW_KEY_KP_2:
            if(action != GLFW_RELEASE && controller.selected_item != -1&&controller.mode == TRANSLATION_MODE)
            {
                glm::mat4 t = glm::mat4 {
                        1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,-0.05,0,1
                };
                objs[controller.selected_item].transform(t);
            }
            if(action != GLFW_RELEASE &&controller.mode == LIGHT_MODE)
                light[1] -= 0.1f;
            break;
        case GLFW_KEY_KP_5:
            if(action != GLFW_RELEASE && controller.selected_item != -1&&controller.mode == TRANSLATION_MODE)
            {
                glm::mat4 t = glm::mat4 {
                        1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0.05,1
                };
                objs[controller.selected_item].transform(t);
            }
            if(action != GLFW_RELEASE &&controller.mode == LIGHT_MODE)
                light[2] += 0.1f;
            break;
        case GLFW_KEY_KP_0:
            if(action != GLFW_RELEASE && controller.selected_item != -1 &&controller.mode == TRANSLATION_MODE)
            {
                glm::mat4 t = glm::mat4 {
                        1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,-0.05,1
                };
                objs[controller.selected_item].transform(t);
            }
            if(action != GLFW_RELEASE &&controller.mode == LIGHT_MODE)
                light[2] -= 0.1f;
            break;

        case GLFW_KEY_7:
            if(action != GLFW_RELEASE && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(0.8f,0.1f,0.1f));
            }
            break;

        case GLFW_KEY_8:
            if(action != GLFW_RELEASE && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(0.1f,0.8f,0.1f));
            }
            break;

        case GLFW_KEY_9:
            if(action != GLFW_RELEASE && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(0.5f,0.5f,0.1f));
            }
            break;

        case GLFW_KEY_0:
            if(action != GLFW_RELEASE && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(0.3f,0.3f,0.3f));
            }
            break;

        case GLFW_KEY_P:
            if(action == GLFW_PRESS && controller.selected_item != -1)
            {
                std::vector<MyObject> new_objs(objs.size() - 1);
                int bias = 0;
                for(int i = 0; i < objs.size(); i++)
                {
                    if (i == controller.selected_item)
                    {
                        bias = 1;
                        continue;
                    }
                    new_objs[i - bias] = objs[i];
                }
                objs = new_objs;
                updateAllObjectsVBO(objs, VBO);
                updateAllObjectsCBO(objs, VBO_N);
                controller.selected_item = -1;
                controller.is_in_process = false;
            }
            break;

        case GLFW_KEY_B:
            if(action == GLFW_PRESS && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(1.f,1.f,1.f));
                objs[controller.selected_item].type = WIREFRAME;
                updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case GLFW_KEY_N:
            if(action == GLFW_PRESS && controller.selected_item != -1)
            {
                objs[controller.selected_item].flatShading();
                objs[controller.selected_item].type = FLAT_SHADING;
                updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case GLFW_KEY_M:
            if(action == GLFW_PRESS && controller.selected_item != -1)
            {
                objs[controller.selected_item].phongShading();
                objs[controller.selected_item].type = PHONG_SHADING;
                updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        default:
            break;
    }
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "HW3: view mode", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
      glewExperimental = true;
      GLenum err = glewInit();
      if(GLEW_OK != err)
      {
        /* Problem: glewInit failed, something is seriously wrong. */
       fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
      }
      glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
      fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Initialize the VAO
    // A Vertex Array Object (or VAO) is an object that describes how the vertex
    // attributes are stored in a Vertex Buffer Object (or VBO). This means that
    // the VAO is not the actual object storing the vertex data,
    // but the descriptor of the vertex data.
    VertexArrayObject VAO;
    VAO.init();
    VAO.bind();

    // Initialize the VBO with the vertices data
    // A VBO is a data container that lives in the GPU memory
    VBO.init();

    V.resize(6);
#if 0
    V[0] = glm::vec3(0,  0.5, -0.5);
    V[1] = glm::vec3(0.1,  0.6, -0.4);
    V[2] = glm::vec3(0.5, -0.5, -0.5);
    V[3] = glm::vec3(0.6, -0.4, -0.4);
    V[4] = glm::vec3(0, 0, 0);
    V[5] = glm::vec3(0.5, 0.5, 0.5);
#endif
    V[0] = glm::vec3(0,  0.5, -0.5);
    V[1] = glm::vec3(-0.5, -0.5, -0.5);
    V[2] = glm::vec3(0.5, -0.5, -0.5);
    V[3] = glm::vec3(0.1,  0.6, -0.4);
    V[4] = glm::vec3(-0.4, -0.4, -0.4);
    V[5] = glm::vec3(0.6, -0.4, -0.4);
    VBO.update(V);

    std::vector<glm::vec3> C(3);
    VBO_N.init();
    VBO_N.update(C);

    controller.mode = VIEW_MODE;
    //MyObject cube(8,12);
    //cube.drawUnitCube();
    //objs.push_back(cube);
    // Initialize the OpenGL Program
    // A program controls the OpenGL pipeline and it must contains
    // at least a vertex shader and a fragment shader to be valid
    Program program;
    const GLchar* vertex_shader =
            "#version 150 core\n"
                    "in vec3 position;"
                    "in vec3 normal;"
                    "out vec4 f_color;"
                    "uniform vec4 triangleColor;"
                    "uniform mat4 view;"
                    "uniform mat4 transform;"
                    "uniform vec4 v;"
                    "uniform vec3 light;"
                    "uniform vec4 isLine;;"
                    "void main()"
                    "{"
                    "    gl_Position = view * transform * vec4(position, 1.0);"
                    "    vec4 lDirection = vec4(  normalize(light - position), 1.0);"
                    "    vec4 n = normalize(transpose(inverse(transform)) * vec4(normal, 1.0));"
                    "    vec4 diffuse = vec4(max(dot(normalize(vec3(lDirection)), normalize(vec3(n))), 0) * vec3(0.4,0.4,0.4), 1.0);"
                    "    vec4 h = normalize(v + lDirection);"
                    "    vec4 spec = vec4(max(dot(normalize(vec3(h)), normalize(vec3(-n))), 0) * vec3(0.4,0.4,0.4), 1.0);"
                    "    vec4 ambient = vec4(0.8,0.8,0.8,1.0) * triangleColor;"
                    "    f_color = isLine * (ambient + diffuse + spec);"
                    "}";
    const GLchar* fragment_shader =
            "#version 150 core\n"
            "out vec4 outColor;"
            "in vec4 f_color;"
            "void main()"
            "{"
            "    outColor = f_color ;"
            "}";
    /*
    const GLchar* fragment_shader =
            "#version 150 core\n"
                    "out vec4 outColor;"
                    "in vec4 f_color;"
                    "void main()"
                    "{"
                    "    outColor = f_color ;"
                    "}";
    */
    // Compile the two shaders and upload the binary to the GPU
    // Note that we have to explicitly specify that the output "slot" called outColor
    // is the one that we want in the fragment buffer (and thus on screen)
    program.init(vertex_shader,fragment_shader,"outColor");
    program.bind();

    // The vertex shader wants the position of the vertices as an input.
    // The following line connects the VBO we defined above with the position "slot"
    // in the vertex shader
    program.bindVertexAttribArray("position",VBO);
    program.bindVertexAttribArray("normal", VBO_N);

    // Register the keyboard callback
    glfwSetKeyCallback(window, key_callback);

    // Register the mouse callback
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Mouse movement callback
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        // Bind your program
        program.bind();

        // Clear the framebuffer
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Enable depth test
        glEnable(GL_DEPTH_TEST);

        // Draw a triangle (red)
        //cube.updateVBO(VBO);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect_ratio = float(height)/float(width); // corresponds to the necessary width scaling

        // identity view = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));mat4(1.f), glm::vec3
        //       // glm::mat4 aspect_scale = glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f)) * glm::scale(glm::mat4(1.f), glm::vec3(1.f, aspect_ratio, 1.f)) * glm::scale(glm::(1.f, 1.f, aspect_ratio));
// * glm::lookAt(glm::vec3(0.f,0.f, -1.f), glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,0.f,0.f));
        view =   homography  * glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f)) * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(-camera_x_rotate) * view_transform  ;
        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::vec4 v = glm::vec4 (0.f,0.f,3.f, 1.f) * homography  * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(-camera_x_rotate) * view_transform;
        glUniform4f(program.uniform("v"), v.x, v.y, v.z, v.w);
        glUniform3f(program.uniform("light"), light.x, light.y,light.z);
        glUniform4f(program.uniform("triangleColor"), 0.1f, 0.1f, 0.1f, 1.0f);

        int first = 0;
        for (int obj = 0; obj < objs.size(); obj ++)
        {
            glUniformMatrix4fv(program.uniform("transform"), 1, GL_FALSE, glm::value_ptr(objs[obj].T));
            for (int i =0; i < objs[obj].E.size(); i ++)
            {
                if(obj == controller.selected_item)
                {
                    glUniform4f(program.uniform("triangleColor"), 0.1f, 0.1f, 0.8f, 0.5f);
                    glDrawArrays(GL_TRIANGLES, first + 3*i, 3);
                    glUniform4f(program.uniform("isLine"), 0.f,0.f,0.f, 0.f);
                    glUniform4f(program.uniform("triangleColor"), 0.1f, 0.1f, 0.1f, 1.0f);
                    glDrawArrays(GL_LINE_LOOP, first + 3*i, 3);
                    glUniform4f(program.uniform("isLine"), 1.0f,1.0f,1.0f, 1.0f);
                }
                else
                {
                    switch (objs[obj].type)
                    {
                        case WIREFRAME:
                            glUniform4f(program.uniform("isLine"), 0.f,0.f,0.f, 0.f);
                            glUniform4f(program.uniform("triangleColor"), 0.1f, 0.1f, 0.1f, 1.0f);
                            glDrawArrays(GL_LINE_LOOP, first + 3*i, 3);
                            glUniform4f(program.uniform("isLine"), 1.0f,1.0f,1.0f, 1.0f);
                            break;

                        case FLAT_SHADING:
                            glUniform4f(program.uniform("triangleColor"), objs[obj].color.x, objs[obj].color.y, objs[obj].color.z, 0.5f);
                            glDrawArrays(GL_TRIANGLES, first + 3*i, 3);
                            glUniform4f(program.uniform("isLine"), 0.f,0.f,0.f, 0.f);
                            glUniform4f(program.uniform("triangleColor"), 0.1f, 0.1f, 0.1f, 1.0f);
                            glDrawArrays(GL_LINE_LOOP, first + 3*i, 3);
                            glUniform4f(program.uniform("isLine"), 1.0f,1.0f,1.0f, 1.0f);
                            break;

                        case PHONG_SHADING:
                            glUniform4f(program.uniform("triangleColor"), objs[obj].color.x, objs[obj].color.y, objs[obj].color.z, 0.5f);
                            glDrawArrays(GL_TRIANGLES, first + 3*i, 3);
                    }
                }


            }
            first += objs[obj].total_size;
        }

        /*
        glUniform3f(program.uniform("triangleColor"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw a triangle (green)
        glUniform3f(program.uniform("triangleColor"), 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);
        */



        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    VAO.free();
    VBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
