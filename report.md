# CS-GY 6533 A – Interactive Computer Graphics - Fall 2021

# Assignment 4: Environment Mapping and Shadow Mapping

Sheng Tong st4048

my code is in src/main.cpp, src/extra.cpp, src/Controller_extra.h, src/MyObject.h, src/skybox.h, and the executable for task 1, task 2 and extra task 1 is Assignment4_bin, and the executabletask 2 is Assignment4_extra. Data for .off files and skybox .ppm are saved in data/ directory.

main.cpp is the code for OpenGL window including drawing objects, handling events and so on for task 1,2, extra 1 (refraction).
extra.cpp is the code for OpenGL window including drawing objects, handling events and so on for extra 2 (Dynamically Generated Cube Map Textures).
Controller_extra.h helps main.cpp to handle events.
MyObject.h defines a class that store an object, including vertexes, normals, color and transformation.
skybox.h helps reading skybox texture into a cube map, or generate dynamic cubemape for dynamically generated cube Map textures.

Press key 'i' to enter view mode to just look at the scene.
Press key 'o' to enter translation mode, you can adjust objects' properties in this mode.
Press key 'l' to enter light mode, you can adjust light position in this mode.

## Mandatory Tasks

## Shadow Mapping

In this task, I use a MyObject class to store the information of an object, and only update all the objects to the VBO once when an object is inserted or deleted.
MyObject class save the vertexes coordinates in a vector V, and save element composition of faces of the object (something works like a EBO in OpenGL) in vector E, when update the VBO, I push the vertex in order of E so shader can draw all the triangles correctly.

When key '1' is pressed, an unit cude will be created in phong shading, I just hardcode the V and E for the unit cude. 
When key '2' is pressed, a pre-define bumpy cube will be imported in phong shading from a .off file.
When key '3' is pressed, a pre-define bunny will be imported in phong shading from a .off file.
When key '4' is pressed, an unit cude will be created in flat shading.

To implement shadow mapping, we need to use another framebuffer and another program with new shaders, we need to render the scene using this program and save depth information into a texture. We first generate a new framebuffer, and generate a texture to store depth map, then bind the texture to the framebuffer so after we render the scene in light position we will get a depth map in the texture:

```cpp
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

//generate depth framebuffer
GLuint depthMapFBO;
glGenFramebuffers(1, &depthMapFBO);

//generate depth texture
GLuint depthMap;
glGenTextures(1, &depthMap);
glBindTexture(GL_TEXTURE_2D, depthMap);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//bind depth texture to framebuffer
glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
glDrawBuffer(GL_NONE);
glReadBuffer(GL_NONE);
glBindFramebuffer(GL_FRAMEBUFFER, 0);

/////////////////////////////////////////////////////////
// more setting ......
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// render the scene in the view position of light source to get depth map
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
// normally render the scene using depth map
/////////////////////////////////////////////////////////
```

Pass the light position and projection & view matrix as uniform to the shader, we can calculate vertex position in light view space and we can render the scene. Now the depth information is saved in depthMap texture, then we switch to the original framebuffer and are ready to render the true scene.

The shader is nearly same as hw3, but this time we will add a shadow test to the shader to control the diffuse and specular color. We have get the depthMap texture, now pass it to the shader using:

```cpp
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, depthMap);
glUniform1i(program.uniform("depthMap"), 0);
```

Then use it as a 2D sampler, we can now get the closest object's depth in the current fragment in fragment shader

```cpp
//.....
uniform sampler2D depthMap;
//.....
float depthValue = texture(depthMap, projCoords.xy).r;
```

Compare the closest depth and current object's fragment depth, we can decide if there should be in the shadow, however directly compare them will cause unnatural shadow shape because shadow map is limited by resolution and will get unprecise depth value. To handle this problem we will add a bias to current depth when comparing. After that we can render the shadow out by controlling the diffuse and specular color:

```cpp
//.......
float shadow = projCoords.z - 0.005 > depthValue  ? 1.0 : 0.0;
vec4 f_color = triangleColor * vec4((ambient + shadow *  (vec3(1.0,1.0,1.0) - vec3(shadowColor)) + (1.0 - shadow ) * ( spec + diffuse ) ), 1.0);
//.......
```

With shadowColor vector, we can control the shadow color, now if there is shadow, we only add a shadow color to final color, if there is no shadow, we just normally add diffuse and specular color. The results are shown beneath, light source is moving around the center on the top of the scene, a timer is controlling its horizontal position:

![objects](report/1.gif)

In light mode, you can press

'8', '2' on keypad to adjust light source height
'x' to reset light source height

And in view mode or light mode, you can press 'x' or '=' to change shadow color.

## Environment Mapping

To implement environment mapping, we will first read pictures into texture to get a cubemap. I converted provided .jpg files to .ppm file and use code from hw1 to read them. This time we need to generate a cubemap texture, and write pictures to each of its faces, then we can pass it to the program as a uniform:

```cpp
GLuint skyboxTexture;
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
glGenTextures(1, &skyboxTexture);
glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

//......

int width, height, nrChannels;
for (unsigned int i = 0; i < faces.size(); i++)
{
    unsigned char *data = readPPM(faces[i].c_str(), &width, &height, nrChannels);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        free(data);
    }
}

//......

glUniform1i(skybox_program.uniform("cubeMap"), 1);
```

I use another program and VAO/VBOs to draw the skybox, and we need to draw the skybox at last with glDepthMask(GL_FALSE) so skybox will not overwrite any thing and will not influence shadow mapping. After get the cubemap texture, we render the skybox out using shader:

```cpp
//fragment shader
out vec4 outColor_skybox;
in vec3 TexCoords;
uniform samplerCube cubeMap;
void main()
{
    outColor_skybox = texture(cubeMap, TexCoords);
};
```

Then to implement mirror appearance, we need also pass the cubemap texture to the normal rendering program, and based on the fragment position, normal and view direction we sample from the cubemap texture as fragment color. And with a isMirror float uniform to switch between normal phong shading color and environment mapping color (there is a flat shading unit cube in the gif below, just to show the reflection more reasonable):

![drag](report/2.gif)

When selecting an object, you can press 'n' to make it a mirror appearance, and press 'm' to set it to normal phong shading.

## Camera Control

In homework2 we have done the view transformation which move the camera left, right, up, down, and zoom in, zoom out in 2D. And I have implemented trackball in hw3, when no object is selected, you can press following keys:
'w', 's': rotate the camera around the x axis (without changing its barycenter)
'a', 'd': rotate the camera around the y axis (without changing its barycenter)
'q', 'e': rotate the camera around the z axis (without changing its barycenter)
'+', '-' on keypad: zoom in/out

And 'x': reset the camera rotation if you lose your way, can press even if one object is selected

To make it easier to operate, you can press 'i' to enter view mode, and simply drag your mouse to move the camera (And also you can press 'x' to reset the camera rotation if you lose your way)

![drag](report/3.gif)

## Optional Task

## Refraction

With simple changes to normal calculation, we can easily implement refraction in the shader (there is a flat shading unit cube in the gif below, just to show the refraction more reasonable):

![drag](report/4.gif)

When selecting an object, you can press 'b' to make it transparent, when it is transparent, it does not generate any shadow.

## Dynamically Generated Cube Map Textures

This part is done in src/extra.cpp, and the executabletask 2 is Assignment4_extra.

To implement dynamic environment mapping, instead of just use the skybox cubemap as texture when rendering the scene, we need to render 6 times on each object at each direction, writing what we get into a cubemap texture, and pass this generated texture as the cubemap texture when rendering the scene, so the object will reflect/refract all other objects.

To do this, we combine what we did for shadow mapping and skybox, we first generate the cubemap texture, and then for each object, we render at 6 different directions and save waht we get to the cubemap texture through a framebuffer. The shader for normal rendering do not need to change.

After get the cubemap texture for the object, we switch the framebuffer to 0, render the object out, and switch the framebuffer back and start to render the next object until all objects get rendered, now objects reflect/refract all other objects (there is a flat shading unit cube in the gif below, just to show the reflection/refraction more reasonable):

![drag](report/5.gif)

The program will be very slow, since the rendering times dramatically increase, and more memory is needed to save texture data.