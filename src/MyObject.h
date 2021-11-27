#ifndef ASSIGNMENT3_MYOBJECT_H
#define ASSIGNMENT3_MYOBJECT_H

#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include<cmath>
#include <iostream>
#include <fstream>
#include <vector>

#include "Helpers.h"

#define WIREFRAME 1
#define FLAT_SHADING 2
#define PHONG_SHADING 3

glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c);


struct Triangle
{
    int vec1;
    int vec2;
    int vec3;
    glm::vec3 normal;
};


class MyObject
{
public:
    MyObject(int vertex = 8, int face = 12)
    {
        color = glm::vec3 (0.3f,0.3f,0.3f);
        type = PHONG_SHADING;
        init(vertex, face);
    };

    MyObject(std::string file)
    {
        color = glm::vec3 (0.3f,0.3f,0.3f);
        type = PHONG_SHADING;
        read(file);
    };

    std::vector<glm::vec3> V;                           //vertex list
    std::vector<glm::vec3> N;                           // vertex normals
    std::vector<glm::vec3> C;                           // vertex colors
    std::vector<Triangle> E;                               //element for each triangle
    glm::mat4 T;                                                   //transformation for the entire object
    glm::vec3 color;
    int total_size;
    int type;

    void init(int vertex, int face);
    void read(std::string file);

    glm::vec3 getBarycenter();
    void transform( glm::mat4 trans);
    void rotate(double rotateX, double rotateY, double rotateZ);
    void scale(double scale);
    void setColor(glm::vec3 c);
    void flatShading();
    void phongShading();
    void shading(glm::vec3 light, glm::mat4 view_transform, bool flat = true);

    void drawUnitCube(bool isFlat);
    void updateVBO(VertexBufferObject &VBO);
};

void MyObject::init(int vertex, int face)
{
    V.resize(vertex);
    E.resize(face);
    total_size = face * 3;
    T = glm::mat4(1.f);
    C.resize(total_size);
}

void MyObject::read(std::string file)
{
    int vertex = 8;
    int face = 12;

    std::string header[4];

    double xSum = 0;
    double ySum = 0;
    double zSum = 0;
    double xmax = -1000;
    double ymax = -1000;
    double zmax = -1000;
    double xmin = 1000;
    double ymin = 1000;
    double zmin = 1000;
    double max = 0;


    std::ifstream in;
    in.open(file, std::ios::in | std::ios::binary);
    if (in.is_open())
    {
        for (int i =0; i < 4; i ++)
        {
            in>>header[i];
        }


        vertex = std::stoi(header[1]);
        face = std::stoi(header[2]);


        V.resize(vertex);
        E.resize(face);
        T = glm::mat4(1.f);

        for (int v =0; v < vertex; v++)
        {
            for(int k=0;k<3;k++)
            {
                in>>header[k];
            }
            xSum = std::stod(header[0]);
            ySum = std::stod(header[1]);
            zSum = std::stod(header[2]);

            xmax = xmax<xSum? xSum: xmax;
            ymax = ymax<ySum? ySum: ymax;
            zmax = zmax<zSum? zSum: xmax;

            xmin = xmin>xSum? xSum: xmin;
            ymin = ymin>ySum? ySum: ymin;
            zmin = zmin>zSum? zSum: zmin;

            V[v] = glm::vec3(std::stod(header[0])  ,std::stod(header[1]) ,std::stod(header[2]) );
        }

        for (int e=0; e<face; e++)
        {
            for(int k=0;k<4;k++)
            {
                in>>header[k];
            }
            E[e] = Triangle{std::stoi(header[1]), std::stoi(header[2]), std::stoi(header[3])};
            E[e].normal = normal(V[E[e].vec1],V[E[e].vec2],V[E[e].vec3]);
        }
        total_size = face * 3;

        double x = (xmax + xmin)/ 2;
        double y = (ymax + ymin)/ 2;
        double z = (zmax + zmin)/ 2;
        glm::vec3 center(x,y,z);

        for (int v =0; v < vertex; v++)
        {
            for(int k=0;k<3;k++)
            {
                if (max < std::abs(V[v][k] - center[k] ))
                    max = std::abs(V[v][k] - center[k] );
            }
        }

        glm::mat4 toOrigin =    glm::mat4(
                1, 0, 0,0,
                0, 1, 0,0,
                0,0,1,0,
                -x, -y, -z,1
        );

        glm::mat4 scale =  glm::mat4(
                0.5/max, 0, 0,0,
                0, 0.5/max, 0,0,
                0,0,0.5/max,0,
                0, 0, 0,1
        );
        transform(scale * toOrigin);
        C.resize(total_size);
        in.close();
    }
    else
    {
        V.resize(vertex);
        E.resize(face);
        T = glm::mat4(1.f);
        total_size = face * 3;
        C.resize(total_size);
    }
    phongShading();
}

void MyObject::setColor(glm::vec3 c)
{
    color = c;
}

void MyObject::transform( glm::mat4 trans)
{
    T = trans * T;
}

void MyObject::drawUnitCube(bool isFlat = false)
{
    V[0] = glm::vec3(-0.5f, 0.5f, -0.5f);
    V[1] = glm::vec3(0.5f, 0.5f, -0.5f);
    V[2] = glm::vec3(-0.5f, -0.5f, -0.5f);
    V[3] = glm::vec3(0.5f, -0.5f, -0.5f);
    V[4] = glm::vec3(-0.5f, 0.5f, 0.5f);
    V[5] = glm::vec3(0.5f, 0.5f, 0.5f);
    V[6] = glm::vec3(-0.5f, -0.5f, 0.5f);
    V[7] = glm::vec3(0.5f, -0.5f, 0.5f);

    E[0] = Triangle{0,1,3};
    E[1] = Triangle{3,2,0};
    E[2] = Triangle{4,0,2};
    E[3] = Triangle{2,6,4};
    E[4] = Triangle{5,4,6};
    E[5] = Triangle{6,7,5};
    E[6] = Triangle{1,5,7};
    E[7] = Triangle{7,3,1};
    E[8] = Triangle{4,5,1};
    E[9] = Triangle{1,0,4};
    E[10] = Triangle{3,7,2};
    E[11] = Triangle{6,2,7}; //267

    for (int e = 0; e< E.size(); e++)
    {
        E[e].normal = normal(V[E[e].vec1],V[E[e].vec2],V[E[e].vec3]);
    }
    if (isFlat)
        flatShading();
    else
        phongShading();
}

void MyObject::updateVBO(VertexBufferObject &VBO)
{
    std::vector<glm::vec3> V_new(total_size) ;
    for(int i =0; i < E.size(); i++)
    {
        V_new[3 * i] = V[E[i].vec1];
        V_new[3 * i + 1] = V[E[i].vec2];
        V_new[3 * i + 2] = V[E[i].vec3];
    }
    VBO.update(V_new);
}

glm::vec3 MyObject::getBarycenter()
{
    double x = 0, y =0, z = 0;
    glm::vec4 point;
    for (int i = 0; i < V.size(); i++)
    {
        point = T * glm::vec4(V[i],1);
        x += point.x / point.w;
        y += point.y / point.w;
        z += point.z / point.w;
    }
    x /= V.size();
    y /= V.size();
    z /= V.size();

    glm::vec3 center = glm::vec3(x,y,z);
    //glm::vec4 t_center = T * glm::vec4(center, 1);
    //center = glm::vec3(t_center.x / t_center.w, t_center.y / t_center.w,t_center.z / t_center.w);
    return center;
}

void MyObject::rotate(double rotateX, double rotateY, double rotateZ)
{
    glm::vec3 barycenter = getBarycenter();
    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            barycenter.x, barycenter.y, barycenter.z,1
    );

    double arcX = glm::radians(rotateX);
    double arcY = glm::radians(rotateY);
    double arcZ = glm::radians(rotateZ);

    glm::mat4 sX = glm::mat4(
            1,  0, 0,0,
            0,  std::cos(arcX), std::sin(arcX),0,
            0, -std::sin(arcX), std::cos(arcX),0,
            0,0,0,1
    );

    glm::mat4 sY = glm::mat4(
            std::cos(arcY),  0, std::sin(arcY),0,
            0,  1, 0,0,
            -std::sin(arcY), 0, std::cos(arcY),0,
            0,0,0,1
    );

    glm::mat4 sZ = glm::mat4(
            std::cos(arcZ),  std::sin(arcZ), 0,0,
            -std::sin(arcZ),  std::cos(arcZ), 0,0,
            0,0,1,0,
            0, 0, 0,1
    );

    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -barycenter.x, -barycenter.y, -barycenter.z,1
    );

    glm::mat4 M = t1 * sX* sY * sZ * t2;
    transform(M);
}

void MyObject::scale(double scale)
{
    glm::vec3 barycenter = getBarycenter();
    glm::mat4 t1 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            barycenter.x, barycenter.y, barycenter.z,1
    );

    glm::mat4 s = glm::mat4(
            scale,  0, 0,0,
            0,  scale, 0,0,
            0, 0, scale,0,
            0,0,0,1
    );

    glm::mat4 t2 = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            -barycenter.x, -barycenter.y, -barycenter.z,1
    );

    glm::mat4 M = t1 * s * t2;
    transform(M);
}

glm::vec3 normal(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    glm::vec3 term = glm::cross(b - a, c - a);
    glm::vec3 normal = term / glm::length(term);
    return normal;
}

void MyObject::flatShading()
{
    N.resize(3 * E.size());
    for(int i =0 ; i < E.size(); i++)
    {
        glm::vec4 n4 = glm::vec4(E[i].normal, 1.0f);
        glm::vec3 normal = glm::vec3 (n4.x / n4.w, n4.y / n4.w, n4.z / n4.w);
        n4 = glm::vec4(normal, 1);
        N[3 * i] = N[3 * i + 1] = N[3 * i + 2] = glm::vec3 (n4.x / n4.w, n4.y / n4.w, n4.z / n4.w);
    }
}

void MyObject::phongShading()
{
    N.resize(3 * E.size());
    std::vector<glm::vec3> vertex_normal(V.size());
    std::vector<int> count(V.size());
    for(int i =0 ; i < V.size(); i++)
        vertex_normal[i] = glm::vec3(0.f,0.f,0.f);

    for(int i =0 ; i < E.size(); i++)
    {
        glm::vec4 n4 = glm::vec4(E[i].normal, 1.0f);
        glm::vec3 normal = glm::vec3 (n4.x / n4.w, n4.y / n4.w, n4.z / n4.w);

        vertex_normal[E[i].vec1] += normal;
        vertex_normal[E[i].vec2] += normal;
        vertex_normal[E[i].vec3] += normal;

        count[E[i].vec1] += 1;
        count[E[i].vec2] += 1;
        count[E[i].vec3] += 1;
    }

    for(int i =0 ; i <  E.size(); i++)
    {
        N[3* i] = glm::vec3(vertex_normal[E[i].vec1].x / (GLint)count[E[i].vec1] , vertex_normal[E[i].vec1].y / (GLint)count[E[i].vec1], vertex_normal[E[i].vec1].z / (GLint)count[E[i].vec1]) ;
        N[3* i + 1] = glm::vec3(vertex_normal[E[i].vec2].x / (GLint)count[E[i].vec2] , vertex_normal[E[i].vec2].y / (GLint)count[E[i].vec2], vertex_normal[E[i].vec2].z / (GLint)count[E[i].vec2]) ;
        N[3* i + 2] = glm::vec3(vertex_normal[E[i].vec3].x /(GLint) count[E[i].vec3] , vertex_normal[E[i].vec3].y / (GLint)count[E[i].vec3], vertex_normal[E[i].vec3].z / (GLint)count[E[i].vec3]) ;
    }
}

void MyObject::shading(glm::vec3 light, glm::mat4 view_transform, bool flat )
{
    if (flat)
        flatShading();
    else
        phongShading();

    glm::vec3 kd = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 ks = glm::vec3(0.8f, 0.8f, 0.8f);

    C.resize(3 * E.size());

    glm::vec4 v4;
    glm::vec3 v;
    glm::vec3 l;
    glm::vec3 h;
    double NdotL;
    glm::vec3 diffuse;
    double NdotH;
    glm::vec3 spec;

    for(int i=0; i < E.size(); i++)
    {
        v4 = glm::vec4(V[E[i].vec1], 1) - glm::vec4(0.f,0.f,-1.f, 1.f) * view_transform;
        v = glm::normalize(glm::vec3(v4.x / v4.w, v4.y / v4.w,v4.z / v4.w));
        l = glm::normalize(V[E[i].vec1] - light);
        h = glm::normalize(v + l);

        NdotL = glm::dot(N[3 * i], l);
        diffuse = kd * (GLfloat)std::max(0.0, NdotL);

        NdotH = glm::dot(N[3 * i], h);
        spec = kd * (GLfloat)std::max(0.0, NdotH);

        C[3 * i] = color * (diffuse + spec);


        v4 = glm::vec4(V[E[i].vec2], 1) - glm::vec4(0.f,0.f,-1.f, 1.f) * view_transform;
        v = glm::normalize(glm::vec3(v4.x / v4.w, v4.y / v4.w,v4.z / v4.w));
        l = glm::normalize(V[E[i].vec2] - light);
        h = glm::normalize(v + l);

        NdotL = glm::dot(N[3 * i + 1], l);
        diffuse = kd * (GLfloat)std::max(0.0, NdotL);

        NdotH = glm::dot(N[3 * i + 1], h);
        spec = kd * (GLfloat)std::max(0.0, NdotH);

        C[3 * i + 1] = color * (diffuse + spec);


        v4 = glm::vec4(V[E[i].vec3], 1) - glm::vec4(0.f,0.f,-1.f, 1.f) * view_transform;
        v = glm::normalize(glm::vec3(v4.x / v4.w, v4.y / v4.w,v4.z / v4.w));
        l = glm::normalize(V[E[i].vec3] - light);
        h = glm::normalize(v + l);

        NdotL = glm::dot(N[3 * i + 2], l);
        diffuse = kd * (GLfloat)std::max(0.0, NdotL);

        NdotH = glm::dot(N[3 * i + 2], h);
        spec = kd * (GLfloat)std::max(0.0, NdotH);

        C[3 * i + 2] = color * (diffuse + spec);
    }

}

void updateAllObjectsVBO(std::vector<MyObject> objs, VertexBufferObject &VBO)
{
    int all_size = 6;
    for (int obj = 0; obj < objs.size(); obj++)
    {
        all_size += objs[obj].total_size;
    }

    std::vector<glm::vec3> V_new(all_size) ;
    MyObject temp;
    int first = 0;

    for (int obj = 0; obj < objs.size(); obj++)
    {
        temp = objs[obj];
        for (int i = 0; i < temp.E.size(); i++)
        {
            V_new[first + 3 * i] = temp.V[temp.E[i].vec1];
            V_new[first +3 * i + 1] = temp.V[temp.E[i].vec2];
            V_new[first + 3 * i + 2] = temp.V[temp.E[i].vec3];
        }
        first += objs[obj].total_size;
    }


    V_new[first ] = glm::vec3(1.f,  -0.9f, -1.f);
    V_new[first + 1] = glm::vec3(-1.f,  -0.9f, -1.f);
    V_new[first + 2]  = glm::vec3(1.f,  -0.9f, 1.f);
    V_new[first + 3]  = glm::vec3(-1.f,  -0.9f, -1.f);
    V_new[first + 4]  = glm::vec3(-1.f,  -0.9f, 1.f);
    V_new[first + 5]  = glm::vec3(1.f,  -0.9f, 1.f);

    VBO.update(V_new);
}

void updateAllObjectsCBO(std::vector<MyObject> objs, VertexBufferObject &VBO)
{
    int all_size = 6;
    for (int obj = 0; obj < objs.size(); obj++)
    {
        all_size += objs[obj].total_size;
    }

    std::vector<glm::vec3> V_new(all_size) ;
    MyObject temp;
    int first = 0;

    for (int obj = 0; obj < objs.size(); obj++)
    {
        temp = objs[obj];
        for (int i = 0; i < temp.E.size(); i++)
        {
            V_new[first + 3 * i] = temp.N[3 * i];
            V_new[first +3 * i + 1] = temp.N[3 * i + 1];
            V_new[first + 3 * i + 2] = temp.N[3 * i + 2];
        }
        first += objs[obj].total_size;
    }

    V_new[first ] = glm::vec3(0.f,  1.0f, 0.f);
    V_new[first + 1] = glm::vec3(0.f,  1.0f, 0.f);
    V_new[first + 2] = glm::vec3(0.f,  1.0f, 0.f);
    V_new[first + 3] = glm::vec3(0.f,  1.0f, 0.f);
    V_new[first + 4] = glm::vec3(0.f,  1.0f, 0.f);
    V_new[first + 5] = glm::vec3(0.f,  1.0f, 0.f);

    VBO.update(V_new);
}
#endif
