#include "main.h"

#ifndef TRI_H
#define TRI_H
class triangle{
    public:
       triangle(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,color_t col);
       triangle(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,color_t col,int a);
        glm::vec3 v1,v2,v3,n;
        color_t color;
        bool PointInTriangle(glm::vec3 pt);
        float sign(glm::vec3 p1,glm::vec3 p2,glm::vec3 p3);
        glm::vec3 get_normal(glm::vec3 p);
        bool RayIntersectsTriangle(Ray ray,float *t);
};

class sphere{
    public:
        sphere(glm::vec3 cen,float r,color_t color);
        float r;
        glm::vec3 cen;
        color_t color;
        glm::vec3 get_normal(glm::vec3 p);
};

#endif