#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "iostream"
#include "vector"
#include <math.h>
#include <algorithm>    // std::max
#ifndef main_H
#define main_H
struct color_t {
    float r;
    float g;
    float b;
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir;
    glm::vec3 intersect;
    glm::vec3 reflec_dir;
    float surface_type;
};

glm::vec3 normalize(glm::vec3 a);
void draw_checker();

#endif