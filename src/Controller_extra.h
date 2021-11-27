//
// Created by Grolken on 2021/10/6.
//

#ifndef ASSIGNMENT2_CONTROLLER_H
#define ASSIGNMENT2_CONTROLLER_H

#define NORMAL_MODE 0
#define INSERT_MODE 1
#define TRANSLATION_MODE 2
#define DELETE_MODE 3
#define COLOR_MODE 4
#define KEYFRAME_MODE 5
#define VIEW_MODE 6
#define LIGHT_MODE 7

#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include<cmath>
#include"MyObject.h"

struct Controller
{
    unsigned int mode = NORMAL_MODE;
    bool is_in_process = false;

    unsigned int inserted_vertex = 0;
    int selected_item = -1;
    glm::vec2 press_point;
    glm::vec2 start_point[3] ;
    glm::vec3 start_point_color[3] ;
    glm::vec2 keyframe_point_color[3] ;
    glm::vec3 rotate;

    bool keyframe_set = false;
    int keyframe_press_time = 0;
    int speed = 3;
};

bool isInside(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::mat4 t, glm::vec2 p)
{
    glm::vec4 a4 = t *glm::vec4(a,0.f,1.f);
    glm::vec4 b4 = t * glm::vec4(b,0.f,1.f);
    glm::vec4 c4 = t * glm::vec4(c,0.f,1.f);

    a = glm::vec2(a4.x / a4.w, a4.y / a4.w);
    b = glm::vec2(b4.x / b4.w, b4.y / b4.w);
    c = glm::vec2(c4.x / c4.w, c4.y / c4.w);

    glm::vec2 pa = p - a;
    glm::vec2 pb = p - b;
    glm::vec2 pc = p - c;

    float cross1 = pa.x * pb.y - pb.x * pa.y;
    float cross2 = pb.x * pc.y - pc.x * pb.y;
    float cross3 = pc.x * pa.y - pa.x * pc.y;

    if( cross1 >= 0 && cross2 >=0 && cross3 >=0)
        return true;
    if( cross1 < 0 && cross2 < 0 && cross3 <0)
        return true;

    return false;
}

bool isInside(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::mat4 t, glm::vec3 p)
{
    glm::vec4 a4 = t *glm::vec4(a,1.f);
    glm::vec4 b4 = t * glm::vec4(b,1.f);
    glm::vec4 c4 = t * glm::vec4(c,1.f);

    a = glm::vec3(a4.x / a4.w, a4.y / a4.w, a4.y / a4.w);
    b = glm::vec3(b4.x / b4.w, b4.y / b4.w, b4.z / a4.w);
    c = glm::vec3(c4.x / c4.w, c4.y / c4.w, c4.z / a4.w);

    glm::vec3 pa = p - a;
    glm::vec3 pb = p - b;
    glm::vec3 pc = p - c;

    float cross1 = pa.x * pb.y - pb.x * pa.y;
    float cross2 = pb.x * pc.y - pc.x * pb.y;
    float cross3 = pc.x * pa.y - pa.x * pc.y;

    if( cross1 >= 0 && cross2 >=0 && cross3 >=0)
        return true;
    if( cross1 < 0 && cross2 < 0 && cross3 <0)
        return true;

    return false;
}

int FindSelectedItem(std::vector<glm::vec2> V,std::vector<glm::mat4> T, double xworld, double yworld)
{
    glm::vec2 p = glm::vec2(xworld, yworld);
    for(int i =0; i<V.size() / 3 ;i++)
    {
        if( isInside( V[3 * i],V[3 * i+1],V[3 * i+2], T[i], p))
            return i;
    }
    return -1;
}

glm::vec3 findBarycenter(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    double x = a.x + b.x + c.x;
    x /= 3.0;

    double y = a.y + b.y + c.y;
    y /= 3.0;

    double z = a.z + b.z + c.z;
    z /= 3.0;

    return glm::vec3(x,y,z);
}

glm::mat4 rotateMatrix(std::vector<glm::vec2> V,  glm::mat4 t, int selected, double rotate)
{
//    glm::vec3 a (V[3 * selected],1);
//    glm::vec3 b (V[3 * selected + 1],1);
//    glm::vec3 c (V[3 * selected + 2],1);

    glm::vec4 a4 = t *  glm::vec4(V[3 * selected],0,1);
    glm::vec4 b4 = t *  glm::vec4 (V[3 * selected + 1],0,1);
    glm::vec4 c4 = t * glm::vec4 (V[3 * selected + 2],0,1);

    glm::vec3 a (a4);
    glm::vec3 b (b4);
    glm::vec3 c (c4);

    glm::vec4 barycenter = glm::vec4 (findBarycenter(a, b, c), 1);
    barycenter.x /= barycenter.w;
    barycenter.y /= barycenter.w;

    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            barycenter.x, barycenter.y, 0,1
    );

    double arc = glm::radians(rotate);

    glm::mat4 r = glm::mat4(
            std::cos(arc),  std::sin(arc), 0,0,
            -std::sin(arc),  std::cos(arc), 0,0,
            0,0,1,0,
            0, 0, 0,1
    );

    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -barycenter.x, -barycenter.y, 0,1
    );

    glm::mat4 M = t1 * r * t2;

    return M;
}

glm::mat4 scaleMatrix(std::vector<glm::vec2> V,  glm::mat4 t, int selected, double scale)
{
//    glm::vec3 a (V[3 * selected],1);
//    glm::vec3 b (V[3 * selected + 1],1);
//    glm::vec3 c (V[3 * selected + 2],1);

    glm::vec4 a4 = t *  glm::vec4(V[3 * selected],0,1);
    glm::vec4 b4 = t *  glm::vec4 (V[3 * selected + 1],0,1);
    glm::vec4 c4 = t * glm::vec4 (V[3 * selected + 2],0,1);

    glm::vec3 a (a4);
    glm::vec3 b (b4);
    glm::vec3 c (c4);

    glm::vec4 barycenter = glm::vec4 (findBarycenter(a, b, c), 1);
    barycenter.x /= barycenter.w;
    barycenter.y /= barycenter.w;

    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            barycenter.x, barycenter.y, 0,1
    );


    glm::mat4 s = glm::mat4(
            scale,  0, 0,0,
            0,  scale, 0,0,
            0, 0, 1,0,
            0,0,0,1
    );

    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -barycenter.x, -barycenter.y, 0,1
    );

    glm::mat4 M = t1 * s * t2;

    return M;
}

glm::mat4 cameraXRotate(double rotate)
{
    double arc = glm::radians(rotate);
    glm::mat4 s = glm::mat4(
           1,  0, 0,0,
            0,  std::cos(arc), std::sin(arc),0,
            0, -std::sin(arc), std::cos(arc),0,
            0,0,0,1
    );

    return s;
}

glm::mat4 cameraYRotate(double rotate)
{
    double arc = glm::radians(rotate);
    glm::mat4 s = glm::mat4(
            std::cos(arc),  0, std::sin(arc),0,
            0,  1, 0,0,
            -std::sin(arc), 0, std::cos(arc),0,
            0,0,0,1
    );

    return s;
}

glm::mat4 cameraZRotate(double rotate)
{
    double arc = glm::radians(rotate);
    glm::mat4 r = glm::mat4(
            std::cos(arc),  std::sin(arc), 0,0,
            -std::sin(arc),  std::cos(arc), 0,0,
            0,0,1,0,
            0, 0, 0,1
    );

    return r;
}

glm::mat4 cameraXRotate(double rotate, glm::vec3 e)
{
    double arc = glm::radians(rotate);
    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            e.x, e.y, e.z,1
    );

    glm::mat4 s = glm::mat4(
            1,  0, 0,0,
            0,  std::cos(arc), std::sin(arc),0,
            0, -std::sin(arc), std::cos(arc),0,
            0,0,0,1
    );

    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -e.x, -e.y, -e.z,1
    );

    return t1 * s * t2;
}

glm::mat4 cameraYRotate(double rotate, glm::vec3 e)
{
    double arc = glm::radians(rotate);
    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            e.x, e.y, e.z,1
    );

    glm::mat4 s = glm::mat4(
            std::cos(arc),  0, std::sin(arc),0,
            0,  1, 0,0,
            -std::sin(arc), 0, std::cos(arc),0,
            0,0,0,1
    );
    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -e.x, -e.y, -e.z,1
    );

    return t1 * s * t2;
}

glm::mat4 cameraZRotate(double rotate, glm::vec3 e)
{
    double arc = glm::radians(rotate);
    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            e.x, e.y, e.z,1
    );

    glm::mat4 s = glm::mat4(
            std::cos(arc),  std::sin(arc), 0,0,
            -std::sin(arc),  std::cos(arc), 0,0,
            0,0,1,0,
            0, 0, 0,1
    );

    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -e.x, -e.y, -e.z,1
    );

    return t1 * s * t2;
}

bool isInside3D(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::mat4 t, glm::vec2 p)
{
    glm::vec4 a4 = t *glm::vec4(a,1.f);
    glm::vec4 b4 = t * glm::vec4(b,1.f);
    glm::vec4 c4 = t * glm::vec4(c,1.f);

    glm::vec2 a2 = glm::vec2(a4.x / a4.w, a4.y / a4.w);
    glm::vec2 b2 = glm::vec2(b4.x / b4.w, b4.y / b4.w);
    glm::vec2 c2 = glm::vec2(c4.x / c4.w, c4.y / c4.w);

    glm::vec2 pa = p - a2;
    glm::vec2 pb = p - b2;
    glm::vec2 pc = p - c2;

    float cross1 = pa.x * pb.y - pb.x * pa.y;
    float cross2 = pb.x * pc.y - pc.x * pb.y;
    float cross3 = pc.x * pa.y - pa.x * pc.y;

    if( cross1 >= 0 && cross2 >=0 && cross3 >=0)
        return true;
    if( cross1 < 0 && cross2 < 0 && cross3 <0)
        return true;

    return false;
}

double findDepth3D(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::mat4 t, glm::vec2 p)
{
glm::vec4 a4 = t *glm::vec4(a,1.f);
glm::vec4 b4 = t * glm::vec4(b,1.f);
glm::vec4 c4 = t * glm::vec4(c,1.f);

glm::vec2 a2 = glm::vec2(a4.x / a4.w, a4.y / a4.w);
glm::vec2 b2 = glm::vec2(b4.x / b4.w, b4.y / b4.w);
glm::vec2 c2 = glm::vec2(c4.x / c4.w, c4.y / c4.w);

double d1=b2.x-a2.x, s1=c2.x-a2.x, f1=p.x-a2.x;
double d2=b2.y-a2.y, s2=c2.y-a2.y, f2=p.y-a2.y;

double u=(s1*f2-s2*f1) / (d2*s1-d1*s2);
double v=(f1-d1 * u) / s1;

double ans=(1-u-v)*a.z+u*b.z+v*c.z;

return ans;
}
/*
int FindSelectedObject3D(std::vector<MyObject> objs, double xworld, double yworld)
{
    glm::vec2 p = glm::vec2(xworld, yworld);
    double closest_depth = 1000;
    int closest_obj = -1;
    for (int obj = 0; obj < objs.size(); obj ++)
    {
        std::vector<glm::vec3> V = objs[obj].V;
        std::vector<Triangle> E = objs[obj].E;
        for(int i =0; i<E.size() ;i++)
        {
            if(isInside3D(V[E[i].vec1], V[E[i].vec2], V[E[i].vec3], objs[obj].T, p))
            {
                double depth = findDepth3D(V[E[i].vec1], V[E[i].vec2], V[E[i].vec3], objs[obj].T, p);
                if (depth < closest_depth)
                {
                    closest_obj = obj;
                    closest_depth = depth;
                }
            }
        }
    }
    return closest_obj;
}
*/
glm::vec3 vec3trans(glm::vec3 v, glm::mat4 T)
{
    glm::vec4 v4 = T * glm::vec4(v, 1);
    glm::vec3 v3 = glm::vec3(v4.x / v4.w, v4.y / v4.w, v4.z / v4.w);
    return v3;
}


double rayTracing(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 normal, glm::mat4 T, glm::vec3 e, glm::vec3 d)
{
    glm::vec3 n3 = vec3trans(normal, (glm::inverse(T)));
    a = vec3trans(a, T);
    b = vec3trans(b, T);
    c = vec3trans(c, T);
    d = vec3trans(d, T);
    e = vec3trans(e, T);
    double coeff = glm::dot(n3, a);
    double t = ( coeff -  glm::dot(n3, e) ) / glm::dot(n3, d);
    return t;
}

int FindSelectedObject3D(std::vector<MyObject> objs, double xworld, double yworld, double zworld, glm::mat4 view)
{
    glm::vec3 e = glm::vec3(xworld, yworld, zworld);
    glm::vec4 d4 = glm::inverse(view) * glm::vec4(0,0,-1, 1);
    glm::vec3 d = glm::vec3(d4.x / d4.w, d4.y / d4.w, d4.z / d4.w);
    std::cout<<"e:"<<e.x<<","<<e.y<<","<<e.z<<std::endl;
    std::cout<<"d:"<<d.x<<","<<d.y<<","<<d.z<<std::endl;

    double closest_depth = 1000;
    int closest_obj = -1;

    double t;
    for (int obj = 0; obj < objs.size(); obj ++)
    {
        std::vector<glm::vec3> V = objs[obj].V;
        std::vector<Triangle> E = objs[obj].E;
        for(int i =0; i<E.size() ;i++)
        {
            glm::vec3 n3 =  vec3trans(E[i].normal, glm::transpose(glm::inverse(objs[obj].T)));
            if (glm::dot(n3, d) == 0)
                continue;

            t = rayTracing(V[E[i].vec1], V[E[i].vec2], V[E[i].vec3], E[i].normal,objs[obj].T, e, d);

            glm::vec3 Q = glm::vec3(e.x + t * d.x, e.y + t * d.y, e.z + t * d.z);

            if(isInside(V[E[i].vec1], V[E[i].vec2], V[E[i].vec3], objs[obj].T, Q))
            {
                if (t < closest_depth)
                {
                    std::cout<<Q.x<<","<<Q.y<<","<<Q.z<<std::endl;
                    closest_obj = obj;
                    closest_depth = t;
                }
            }
        }
    }
    return closest_obj;
}

double triangleInterpolate(glm::vec3 a , glm::vec3 b , glm::vec3 c, glm::vec2 p )
{
    double w1 = ( (b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y) ) / ( (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y) );
    double w2 = ( (c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y) ) / ( (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y) );
    double w3 = 1 - w1 - w2;

    return w1 * a.z + w2 * b.z + w3 * c.z;
}


int FindSelectedObject3DUsingProjection(std::vector<MyObject> objs, double xpos, double ypos, glm::mat4 view)
{
    glm::vec2 Q = glm::vec2(xpos, ypos);
    double depth = 0;
    double closest_depth = 1000;
    int closest_obj = -1;

    double t;
    for (int obj = 0; obj < objs.size(); obj ++)
    {
        std::vector<glm::vec3> V = objs[obj].V;
        std::vector<Triangle> E = objs[obj].E;
        glm::mat4 T = objs[obj].T;
        for(int i =0; i<E.size() ;i++)
        {
            glm::vec3 a = vec3trans(vec3trans( V[E[i].vec1], T), view) ;
            glm::vec3 b = vec3trans(vec3trans( V[E[i].vec2], T), view);
            glm::vec3 c = vec3trans(vec3trans( V[E[i].vec3], T), view);

            //depth = (a.z + b.z + c.z) / 3;
            depth = triangleInterpolate(a,b,c, glm::vec2(xpos, ypos));

            if(isInside(a, b, c, glm::mat4(1.0f), Q))
            {
                if (depth < closest_depth)
                {
                    closest_obj = obj;
                    closest_depth = depth;
                }
            }
        }
    }
    return closest_obj;
}
/*
unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
*/
#endif //ASSIGNMENT2_CONTROLLER_H
