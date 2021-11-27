/*
#include"MyObject.h"

void MyObject::init(int vertex, int face)
{
    V.resize(vertex);
    E.resize(face);
    total_size = face * 3;
    T = glm::mat4(
            1, 0, 0,0,
            0, 1, 0,0,
            0,0,1,0,
            0, 0, 0,1
    );
}

void MyObject::transform( glm::mat4 trans)
{
    T = trans * T;
}

void MyObject::drawUnitCube()
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
    E[1] = Triangle{0,2,3};
    E[2] = Triangle{4,0,2};
    E[3] = Triangle{4,6,2};
    E[4] = Triangle{5,4,6};
    E[5] = Triangle{5,7,6};
    E[6] = Triangle{1,5,7};
    E[7] = Triangle{1,3,7};
    E[8] = Triangle{4,5,1};
    E[9] = Triangle{4,0,1};
    E[10] = Triangle{7,3,2};
    E[11] = Triangle{7,6,2};
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
 */