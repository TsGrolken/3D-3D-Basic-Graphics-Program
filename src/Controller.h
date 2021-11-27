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

#include <glm/glm.hpp> // glm::vec3
#include <glm/vec3.hpp> // glm::vec3
#include<cmath>

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

    bool keyframe_set = false;
    int keyframe_press_time = 0;
    int speed = 3;
};

bool isInside(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p)
{
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


int FindSelectedItem(std::vector<glm::vec2> V, double xworld, double yworld)
{
    glm::vec2 p = glm::vec2(xworld, yworld);
    for(int i =0; i<V.size() / 3 ;i++)
    {
        if( isInside( V[3 * i],V[3 * i+1],V[3 * i+2], p))
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

    return glm::vec3(x,y,1);
}

std::vector<glm::vec2> rotate(std::vector<glm::vec2> V, int selected, double rotate)
{
    glm::vec3 a (V[3 * selected],1);
    glm::vec3 b (V[3 * selected + 1],1);
    glm::vec3 c (V[3 * selected + 2],1);
    glm::vec3 barycenter = findBarycenter(a, b, c);

    glm::mat3 t1 = glm::mat3(
                1, 0, 0,
                0, 1, 0,
                barycenter.x, barycenter.y, 1
            );

    double arc = glm::radians(rotate);

    glm::mat3 r = glm::mat3(
            std::cos(arc),  std::sin(arc), 0,
            -std::sin(arc),  std::cos(arc), 0,
            0, 0, 1
    );

    glm::mat3 t2 = glm::mat3(
            1, 0, 0,
            0, 1, 0,
            -barycenter.x, -barycenter.y, 1
    );

    glm::mat3 M = t1 * r *t2;
    a = M * a;
    b = M * b;
    c = M * c;

    V[3 * selected] = glm::vec2(a.x / a.z, a.y / a.z);
    V[3 * selected + 1] = glm::vec2(b.x / b.z, b.y / b.z);
    V[3 * selected + 2] = glm::vec2(c.x / c.z, c.y / c.z);

    return V;
}

std::vector<glm::vec2> scale(std::vector<glm::vec2> V, int selected, double scale)
{
    glm::vec3 a (V[3 * selected],1);
    glm::vec3 b (V[3 * selected + 1],1);
    glm::vec3 c (V[3 * selected + 2],1);
    glm::vec3 barycenter = findBarycenter(a, b, c);

    glm::mat3 t1 = glm::mat3(
            1, 0, 0,
            0, 1, 0,
            barycenter.x, barycenter.y, 1
    );

    glm::mat3 s = glm::mat3(
            scale,  0, 0,
            0,  scale, 0,
            0, 0, 1
    );

    glm::mat3 t2 = glm::mat3(
            1, 0, 0,
            0, 1, 0,
            -barycenter.x, -barycenter.y, 1
    );

    glm::mat3 M = t1 * s *t2;
    a = M * a;
    b = M * b;
    c = M * c;

    V[3 * selected] = glm::vec2(a.x / a.z, a.y / a.z);
    V[3 * selected + 1] = glm::vec2(b.x / b.z, b.y / b.z);
    V[3 * selected + 2] = glm::vec2(c.x / c.z, c.y / c.z);

    return V;
}

#endif //ASSIGNMENT2_CONTROLLER_H
