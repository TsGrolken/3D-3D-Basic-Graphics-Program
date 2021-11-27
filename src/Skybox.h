#ifndef ASSIGNMENT3_SKYBOX_H
#define ASSIGNMENT3_SKYBOX_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include<cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include<math.h>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

void CreateSkyBox(VertexBufferObject &vbo)
{
    std::vector<glm::vec3> skyboxVertices = {
            // positions
            glm::vec3( -1.0f,  1.0f, -1.0f) ,
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),

            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f),
            glm::vec3( -1.0f, -1.0f,  1.0f),

            glm::vec3(-1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f, -1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f,  1.0f),
            glm::vec3(-1.0f,  1.0f, -1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(1.0f, -1.0f, -1.0f),
            glm::vec3(-1.0f, -1.0f,  1.0f),
            glm::vec3(1.0f, -1.0f,  1.0f)
    };

    for (int i = 0; i < skyboxVertices.size(); i++)
        skyboxVertices[i] *= 10.0f;

    vbo.update(skyboxVertices);
}

unsigned  char* readPPM(std::string path, int *width, int *height, int component)
{
    using namespace std;
    try
    {
        string header[4];

        ifstream imgFile;
        imgFile.open(path, ios::in | ios::binary);

        if (imgFile.is_open())
        {
            // read the header and ingore comments start with '#'
            for (int i = 0; i < 4; i++)
            {

                if (!(imgFile >> header[i]))
                {
                    return NULL;
                }

                if (header[i] == "#" || header[i][0] == '#')
                {
                    getline(imgFile, header[i]);
                    i--;
                }
            }

            // get the PPM type
            int type = 0;
            if(header[0][0]=='P')
            {
                type = header[0][1] - '0';
            }

            *width = stoi(header[1]);
            *height = stoi(header[2]);
            int max = stoi(header[3]);
            bool isBinary = false;

            switch (type)
            {
                case 2:
                    component = 1;
                    isBinary = false;
                    break;
                case 5:
                    component = 1;
                    isBinary = true;
                    break;
                case 3:
                    component = 3;
                    isBinary = false;
                    break;
                case 6:
                    component = 3;
                    isBinary = true;
                    break;
                default:
                    component = 0;
                    isBinary = false;
                    break;
            }
            //vector<int> data = vector<int>(width * height * component);
            auto data = new unsigned  char[*width * *height * component];

            // read data from the file
            // ASCII file, read intergers and ingore comments starting with '#'
            if (!isBinary)
            {
                string val;
                imgFile >> val;

                for (int i = 0; i < *width * *height * component ; i++)
                {
                    if (!(imgFile >> val))
                    {
                        break;
                    }

                    if (val == "#" || val[0] == '#')
                    {
                        getline(imgFile, val);
                        i--;
                        continue;
                    }

                    data[i] = (unsigned  char)stoi(val);
                }
            }

                // binary file, read chars, there is no space betwwen differrent chars and no comment line
            else
            {
                char val = 0;
                unsigned char unsval;
                imgFile.read(&val, 1);

                for (int i = 0; i < *width * *height * component ; i++)
                {
                    if (!(imgFile.read(&val, 1)))
                    {
                        break;
                    }
                    unsval = val;
                    data[i] = unsval;
                }
            }

            imgFile.close();
            return data;
        }

            // path error, can not find the file
        else
        {
            cout << "No image found." << endl;
            return NULL;
        }

    }
    catch (exception& e)
    {
        cout << "Error: " << e.what() << endl;
    }

    return NULL;
};

GLuint readSkybox(std::vector<std::string> faces)
{
    GLuint skyboxTexture;
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = readPPM(faces[i].c_str(), &width, &height, nrChannels);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return skyboxTexture;
}

void dynamicEnvMapping(GLuint cubeMapFBO, GLuint cubeMap, VertexArrayObject vao, Program program, VertexArrayObject skyvao, Program skyprogram, glm::mat4 view, float aspect, int object_index, std::vector<MyObject> objs, glm::mat4 camera)
{
    glUniform1i(program.uniform("cubeMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO);
    glEnable(GL_DEPTH_TEST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glViewport(0, 0, 256, 256);


    glm::mat4 orig = glm::mat4(1.f);
    glm::mat4 look = glm::mat4(1.f);

    std::vector<glm::vec4> faces =
            {
                    glm::vec4(1.f,0.f,0.f,1.f),
                    glm::vec4(-1.f,0.f,0.f,1.f),
                    glm::vec4(0.f,10.f,0.f,1.f),
                    glm::vec4(0.f,-10.f,0.f,1.f),
                    glm::vec4(0.f,0.f,10.f,1.f),
                    glm::vec4(0.f,0.f,-10.f,1.f)
            };

    std::vector<glm::vec3> heads =
            {
                    glm::vec3(0.f,1.f,0.f),
                    glm::vec3(0.f,1.f,0.f),
                    glm::vec3(0.f,0.f,-1.f),
                    glm::vec3(0.f,0.f,1.f),
                    glm::vec3(0.f,1.f,0.f),
                    glm::vec3(0.f,1.f,0.f)
            };

    for (int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    for (unsigned int j= 0; j < 6; j++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, cubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT|GL_COLOR_BUFFER_BIT );

        int first = 0;
        glm::vec4 eye4 = objs[object_index].T * glm::vec4(0.f,0.f,0.f, 1.f) ;
        glm::vec3 eye =glm::vec3(eye4.x / eye4.w,eye4.y/ eye4.w,eye4.z/ eye4.w);
        glm::vec4 center4 =objs[object_index].T* faces[j]  ;
        glm::vec3 center =glm::vec3(center4.x / center4.w,center4.y/ center4.w,center4.z/ center4.w);

        look = glm::perspective(glm::radians(90.f), 1.f , 0.01f, 100.0f) * glm::lookAt( eye, center, -heads[j]) *objs[object_index].T* camera;

        skyvao.bind();
        skyprogram.bind();
        glUniformMatrix4fv(skyprogram.uniform("view"), 1, GL_FALSE, glm::value_ptr(look));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        for (int obj = 0; obj < objs.size(); obj ++)
        {

            vao.bind();
            program.bind();

            look = glm::perspective(glm::radians(90.f), 1.f , 0.01f, 100.0f) * glm::lookAt( eye, center, -heads[j])*objs[object_index].T* camera;
            glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(look));

            glUniformMatrix4fv(program.uniform("transform"), 1, GL_FALSE, glm::value_ptr(objs[obj].T));
            if(obj != object_index)
            {
                //std::cout<<obj<<std::endl;
                for (int i =0; i < objs[obj].E.size(); i ++)
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
                            break;

                    }
                }
            }
            first += objs[obj].total_size;
        }


        glUniformMatrix4fv(program.uniform("transform"), 1, GL_FALSE,glm::value_ptr(orig));
        glUniform4f(program.uniform("triangleColor"), 0.4f, 0.4f, 0.4f, 0.4f);
        glUniform4f(program.uniform("isLine"), 1.0f,1.0f,1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, first, 6);
        first = 0;

    }

    glUniformMatrix4fv(program.uniform("view"), 1, GL_FALSE, glm::value_ptr(view));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
/**/
#endif