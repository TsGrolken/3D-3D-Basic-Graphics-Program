// OpenGL Helpers to reduce the clutter
#include "Helpers.h"
#include"MyObject.h"
#include"Controller_extra.h"
#include"Skybox.h"
// GLFW is necessary to handle the OpenGL context
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include<math.h>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

// Timer
#include <chrono>

// VertexBufferObject wrapper
VertexBufferObject VBO;
VertexBufferObject VBO_N;
// Contains the view transformation
glm::mat4 view;
glm::mat4 camera_move = glm::mat4(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, -2.5f, 1.f
);
glm::mat4  homography = glm::perspective(glm::radians(45.f), 1.0f, 1.0f, 100.0f) * camera_move;//glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f)  * glm::scale(glm::mat4(1.f) , glm::vec3(10.f, 10.f, 10.f));
glm::mat4 view_transform = glm::mat4(1.f);

Controller controller;
glm::vec3 color(0.3f, 0.3f, 0.3f);
glm::vec4 shadowColor(1.f, 1.f ,1.f , 1.f);

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

    if(controller.mode != TRANSLATION_MODE && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        controller.is_in_process = true;
        controller.press_point =glm::vec2 (xpos, ypos);
        controller.rotate = glm::vec3(camera_x_rotate, camera_y_rotate, camera_z_rotate);
    }

    if(controller.mode != TRANSLATION_MODE && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
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

    if(controller.mode!=TRANSLATION_MODE && controller.is_in_process == true)
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

        case  GLFW_KEY_4:
            if(action == GLFW_PRESS)
            {
                MyObject cube = MyObject(8, 12);
                cube.drawUnitCube(true);
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
            if(controller.mode == LIGHT_MODE)
            {
                light = glm::vec3(0.f, 5.f, 0.f);
                break;
            }
            camera_x_rotate = 0;
             camera_y_rotate = 0;
            camera_z_rotate =0;

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
                if(controller.mode == LIGHT_MODE)
                {
                    if(shadowColor.x == 1)
                    {
                        shadowColor.x = -10;
                        shadowColor.y = shadowColor.z  = 1.5;
                    }
                    else
                        shadowColor.x = shadowColor.y = shadowColor.z  = 1;
                    break;
                }
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
                    view_transform[3][3] -= 0.05f;
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
                    view_transform[3][3] += 0.05f;
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
            homography =glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f)  * glm::scale(glm::mat4(1.f) , glm::vec3(10.f, 10.f, 10.f));
            break;

        case GLFW_KEY_EQUAL:
            if(action == GLFW_RELEASE)
            {
                //glm::mat4 move = glm::lookAt(light, glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,0.f,1.f));
                //homography =  glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 10.f)    * move ; //glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f)  *
                std::cout<<light.x<<","<<light.y<<","<<-light.z<<std::endl;
                if(shadowColor.x == 1)
                {
                    shadowColor.x = -10;
                    shadowColor.y = shadowColor.z  = 1.5;
                }
                else
                    shadowColor.x = shadowColor.y = shadowColor.z  = 1;
            }
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
                objs[controller.selected_item].setColor(glm::vec3(0.5f,0.1f,0.1f));
            }
            break;

        case GLFW_KEY_8:
            if(action != GLFW_RELEASE && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(0.1f,0.5f,0.1f));
            }
            break;

        case GLFW_KEY_9:
            if(action != GLFW_RELEASE && controller.selected_item != -1)
            {
                objs[controller.selected_item].setColor(glm::vec3(0.3f,0.3f,0.1f));
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
                //objs[controller.selected_item].phongShading();
                objs[controller.selected_item].type = WIREFRAME;
                //updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case GLFW_KEY_N:
            if(action == GLFW_PRESS && controller.selected_item != -1)
            {
                //objs[controller.selected_item].phongShading();
                objs[controller.selected_item].type = FLAT_SHADING;
                //updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        case GLFW_KEY_M:
            if(action == GLFW_PRESS && controller.selected_item != -1)
            {
                //objs[controller.selected_item].phongShading();
                objs[controller.selected_item].type = PHONG_SHADING;
                //updateAllObjectsCBO(objs, VBO_N);
            }
            break;

        default:
            break;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    view_transform[3][3] -= 0.1f* yoffset;
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
    window = glfwCreateWindow(640 * 1.5, 480 * 1.5, "HW3: view mode", NULL, NULL);
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
    VertexArrayObject skyboxVAO;
    skyboxVAO.init();
    skyboxVAO.bind();

    VertexBufferObject skyboxVBO;
    skyboxVBO.init();
    skyboxVBO.bind();
    CreateSkyBox(skyboxVBO);

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
    V[0] = glm::vec3(1.f,  -0.9f, -1.f);
    V[1] = glm::vec3(-1.f,  -0.9f, -1.f);
    V[2]  = glm::vec3(1.f,  -0.9f, 1.f);
    V[3]  = glm::vec3(-1.f,  -0.9f, -1.f);
    V[4]  = glm::vec3(-1.f,  -0.9f, 1.f);
    V[5]  = glm::vec3(1.f,  -0.9f, 1.f);
    VBO.update(V);

    std::vector<glm::vec3> C(6);
    C[0] = glm::vec3(0.f,  1.0f, 0.f);
    C[1] = glm::vec3(0.f,  1.0f, 0.f);
    C[2] = glm::vec3(0.f,  1.0f, 0.f);
    C[3] = glm::vec3(0.f,  1.0f, 0.f);
    C[4] = glm::vec3(0.f,  1.0f, 0.f);
    C[5] = glm::vec3(0.f,  1.0f, 0.f);

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
            "out vec3 FragPos;"
            "out vec3 Normal;"
            "out vec3 test_Position;"
            "out vec4 fragPosLightSpace;"

            "uniform mat4 transform;"
            "uniform mat4 view;"
            "uniform mat4 lightSpaceMatrix;"
            "void main()"
            "{"
            "    vec3 fragPosition = vec3(transform * vec4(position, 1.0));"
            "    test_Position = vec3(transform * vec4(position, 1.0));"
            "    fragPosLightSpace = lightSpaceMatrix * transform * vec4(position, 1.0);"
            "    FragPos = vec3(transform * vec4(position, 1.0));"
            "    Normal = mat3(transpose(inverse(transform))) * normal;"
            "    gl_Position = view * vec4(FragPos, 1.0);"
            " }";

    const GLchar* fragment_shader =
            "#version 150 core\n"
            "out vec4 outColor;"

            "in vec3 Normal;"
            "in vec3 FragPos;"
            "in vec3 test_Position;"
            "in vec4 fragPosLightSpace;"

            "uniform vec3 light;"
            "uniform vec4 v;"
            "uniform sampler2D depthMap;"
            "uniform vec4 triangleColor;"
            "uniform vec4 shadowColor;"

            "uniform samplerCube cubeMap;"
            "uniform float isMirror;"

            "uniform float refractRate;"
            "uniform float isRefract;"

            "void main()"
            "{"
            " vec3 lightColor = vec3(1.0, 1.0, 1.0);"
            " vec3 lightPos = light;"
            " vec3 viewPos = v.xyz / v.w;"
            " vec3 objectColor = vec3(triangleColor);"
            "float ambientStrength = 0.8;"
            "vec3 ambient = ambientStrength * lightColor;"

            " float diffStrength = 0.7;"
            "vec3 norm = normalize(Normal);"
            "vec3 lightDir = normalize(lightPos - FragPos);"
            "float diff = max(dot(norm, lightDir), 0.0);"
            "vec3 diffuse = diffStrength * diff * lightColor;"

            " float specularStrength = 0.5;"
            "vec3 viewDir = -normalize(viewPos + FragPos);"
            "vec3 reflectDir = reflect(-lightDir, norm);"
            "float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);"
            "vec3 specular = specularStrength * spec * lightColor;"

            "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;"
            "    projCoords = projCoords * 0.5 + 0.5;"
            "    float depthValue = texture(depthMap, projCoords.xy).r;"
            "    vec4 FColor = vec4(vec3(depthValue), 1.0);"
            "    float shadow = projCoords.z - 0.005 > depthValue  ? 1.0 : 0.0;"
            "    vec4 f_color = triangleColor * vec4((ambient + shadow *  (vec3(1.0,1.0,1.0) - vec3(shadowColor)) + (1.0 - shadow ) * ( spec + diffuse ) ), 1.0);"

            "    vec3 R = reflect(normalize( viewPos + test_Position), normalize(Normal));"
            "    vec4 reflect_color =  vec4(texture(cubeMap, R).rgb, 1.0);"

            "    vec3 R_refract = refract(normalize( viewPos + test_Position ), normalize(Normal), refractRate);"
            "    vec4 refract_color =  vec4(texture(cubeMap, R_refract).rgb, 1.0);"
            "    outColor = (1 - isMirror) * (1 - isRefract) * f_color + isMirror * (1 - isRefract) * reflect_color + isRefract * refract_color ;"
            "}";
    Program depth_program;
    const GLchar* depth_vertex_shader =
            "#version 150 core\n"
            "in vec3 position;"
            "uniform mat4 transform;"
            "uniform mat4 lightSpaceMatrix;"
            "void main()"
            "{"
            "    gl_Position = lightSpaceMatrix * transform * vec4(position, 1.0);"
            "}";

    const GLchar* empty_fragment_shader =
            "#version 150 core\n"
                    "void main()"
                    "{"
                    "}";

    Program skybox_program;
    const GLchar* skybox_vertex_shader =
            "#version 150 core\n"
            "in vec3 position;"
            "uniform mat4 view;"
            "out vec3 TexCoords;"
            "void main()"
            "{"
            "    TexCoords = position / 10.f;"
            "    gl_Position = view * vec4(position, 1.0);"
            "}";

    const GLchar* skybox_fragment_shader =
            "#version 150 core\n"
            "out vec4 outColor_skybox;"
            "in vec3 TexCoords;"
            "uniform samplerCube cubeMap;"
            "void main()"
            "{"
            "    outColor_skybox = texture(cubeMap, TexCoords);"
            "}";

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

    // Mouse scroll callback
    glfwSetScrollCallback(window,scroll_callback);


    // Update viewport
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // shadow mapping part
    // depth mapping program
    depth_program.init(depth_vertex_shader,empty_fragment_shader,"outColor_d");
    depth_program.bind();
    depth_program.bindVertexAttribArray("position",VBO);
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    skyboxVAO.bind();
    skybox_program.init(skybox_vertex_shader,skybox_fragment_shader,"outColor_skybox");
    skybox_program.bind();
    skybox_program.bindVertexAttribArray("position",skyboxVBO);

    std::vector<std::string> faces =
    {
            "../data/night_negx.ppm",
            "../data/night_posx.ppm",
            "../data/night_posy.ppm",
            "../data/night_negy.ppm",
            "../data/night_posz.ppm",
            "../data/night_negz.ppm"
    };

    GLuint cubemapTexture = readSkybox(faces);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(skybox_program.uniform("cubeMap"), 1);

    auto t_start = std::chrono::high_resolution_clock::now();
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        auto t_now = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();
        light.x = (float)(sin(time / 2.f ))* 2.f ;
        light.z = (float)(cos(time / 2.f)) * 2.f;
        // Bind your VAO (not necessary if you have only one)
        VAO.bind();

        depth_program.bind();

        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE );

        // Draw a triangle (red)
        //cube.updateVBO(VBO);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float aspect_ratio = float(height)/float(width); // corresponds to the necessary width scaling

        glm::mat4 orig = glm::mat4(1.f);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // shadow mapping part
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        //glCullFace(GL_FRONT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightView  = glm::lookAt(light, glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,0.f,1.f));
        //glm::mat4 lightSpaceMatrix  =  glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 10.f)  * lightView  ;
        glm::mat4 lightSpaceMatrix  =  glm::perspective(glm::radians(45.f), 1.f, 1.0f, 100.0f)  * lightView  ;
        glUniformMatrix4fv(depth_program.uniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        int first_d = 0;
        for (int obj = 0; obj < objs.size(); obj ++)
        {
            glUniformMatrix4fv(depth_program.uniform("transform"), 1, GL_FALSE, glm::value_ptr(objs[obj].T));
            for (int i =0; i < objs[obj].E.size(); i ++)
            {
                if(objs[obj].type != WIREFRAME)
                {
                    glDrawArrays(GL_TRIANGLES, first_d + 3*i, 3);
                }
            }
            first_d += objs[obj].total_size;
        }
        glUniformMatrix4fv(depth_program.uniform("transform"), 1, GL_FALSE,glm::value_ptr(orig));
        glDrawArrays(GL_TRIANGLES, first_d, 6);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glCullFace(GL_BACK);
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // normal rendering part
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Bind your program
        program.bind();
        glEnable(GL_DEPTH_TEST);

        // Clear the framebuffer
        glViewport(0, 0, width, height);
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(program.uniform("depthMap"), 0);
        glUniform1i(program.uniform("cubeMap"), 1);
        glUniformMatrix4fv(program.uniform("lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // identity view = glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));mat4(1.f), glm::vec3
        //       // glm::mat4 aspect_scale = glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f)) * glm::scale(glm::mat4(1.f), glm::vec3(1.f, aspect_ratio, 1.f)) * glm::scale(glm::(1.f, 1.f, aspect_ratio));
// * glm::lookAt(glm::vec3(0.f,0.f, -1.f), glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,0.f,0.f));
        view =   homography  * glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f)) * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(camera_x_rotate) * view_transform  ;
        glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

        glm::vec4 v = glm::vec4 (0.f,0.f,3.f * view_transform[3][3], 1.f) * homography  * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(camera_x_rotate)  ;
        //glm::vec4 v = glm::vec4(0.f,0.f,-1.0f,1.f) * glm::lookAt(glm::vec3(0.f,0.f, -1.f), glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,1.f,0.f))* homography * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(camera_x_rotate) * view_transform ;
        //glm::vec4 v = glm::inverse(glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f)) * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(camera_x_rotate) * view_transform) * glm::vec4(0.f,0.f,-1.5f,1.f) ;

        //std::cout<<v.x / v.w<<' '<<v.y / v.w<<' '<<v.z / v.w<<' '<<std::endl;

        glUniform4f(program.uniform("v"), v.x, v.y, v.z, v.w);
        glUniform3f(program.uniform("light"), light.x, light.y,light.z);
        glUniform4f(program.uniform("triangleColor"), 0.1f, 0.1f, 0.1f, 1.0f);
        glUniform4f(program.uniform("shadowColor"), shadowColor.x,shadowColor.y,shadowColor.z,shadowColor.w);

        glUniform1f(program.uniform("isMirror"), 0.f);
        glUniform1f(program.uniform("isRefract"), 0.f);

        glm::vec4 cameraPos = view * glm::vec4(0.f,0.f,-2.5f,1.f);


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
                            glUniform4f(program.uniform("triangleColor"), objs[obj].color.x, objs[obj].color.y, objs[obj].color.z, 0.5f);
                            glUniform1f(program.uniform("refractRate"), 1.f / (1.f + objs[obj].color.x));
                            glUniform1f(program.uniform("isRefract"), 1.f);
                            glDrawArrays(GL_TRIANGLES, first + 3*i, 3);
                            glUniform1f(program.uniform("isRefract"), 0.f);
                            break;

                        case FLAT_SHADING:
                            glUniform4f(program.uniform("triangleColor"), objs[obj].color.x, objs[obj].color.y, objs[obj].color.z, 0.5f);
                            glUniform1f(program.uniform("isMirror"), 1.f);
                            glDrawArrays(GL_TRIANGLES, first + 3*i, 3);
                            glUniform1f(program.uniform("isMirror"), 0.f);
                            break;

                        case PHONG_SHADING:
                            glUniform4f(program.uniform("triangleColor"), objs[obj].color.x, objs[obj].color.y, objs[obj].color.z, 0.5f);
                            glDrawArrays(GL_TRIANGLES, first + 3*i, 3);
                    }
                }


            }
            first += objs[obj].total_size;
        }
        glUniformMatrix4fv(program.uniform("transform"), 1, GL_FALSE,glm::value_ptr(orig));
        glUniform4f(program.uniform("triangleColor"), 0.4f, 0.4f, 0.4f, 0.4f);
        glUniform4f(program.uniform("isLine"), 1.0f,1.0f,1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, first, 6);
        /*
        glUniform3f(program.uniform("triangleColor"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw a triangle (green)
        glUniform3f(program.uniform("triangleColor"), 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3);
        */
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // skybox rendering part
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        skyboxVAO.bind();
        skybox_program.bind();

        //glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        glm::mat4 camera_trans = glm::mat4(glm::mat3(view_transform));
        glm::mat4 skyboxView = glm::perspective( 1.0f + 3.f /exp(1.f + exp(1.f -  view_transform[3][3])) , 1.f, 0.01f, 100.0f)  * camera_move * glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f))  * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(camera_x_rotate) * camera_trans;
        //view = glm::mat4(glm::mat3(view));
        //view =   homography  * glm::scale(glm::mat4(1.f), glm::vec3(aspect_ratio, 1.f, 1.f)) * cameraZRotate(camera_z_rotate) * cameraYRotate(camera_y_rotate) * cameraXRotate(camera_x_rotate) * view_transform  ;
        glUniformMatrix4fv(skybox_program.uniform("view"), 1, GL_FALSE, glm::value_ptr(skyboxView));



        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDepthMask(GL_TRUE);
        //glDepthFunc(GL_LESS);


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    // Deallocate opengl memory
    program.free();
    depth_program.free();
    skybox_program.free();
    VAO.free();
    VBO.free();
    VBO_N.free();
    skyboxVAO.free();
    skyboxVBO.free();

    // Deallocate glfw internals
    glfwTerminate();
    return 0;
}
