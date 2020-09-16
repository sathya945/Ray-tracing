#include "main.h"

#ifndef TRI_H
#define TRI_H
class triangle{
    public:
       triangle(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,color_t col,float surface_reflec);
       triangle(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,color_t col,float surface_reflec,int a);
        glm::vec3 v1,v2,v3,n;
        color_t color;
        float surface_type=0;
        color_t get_color(glm::vec3 pos);
        bool PointInTriangle(glm::vec3 pt);
        float sign(glm::vec3 p1,glm::vec3 p2,glm::vec3 p3);
        glm::vec3 get_normal(glm::vec3 p);
        bool RayIntersectsTriangle(Ray ray,float *t);
};

class sphere{
    public:
        sphere(glm::vec3 cen,float r,color_t color,float surface_reflec);
        float r;
        float surface_type=0;
        glm::vec3 cen;
        color_t color;
        glm::vec3 get_normal(glm::vec3 p);
};

#endif