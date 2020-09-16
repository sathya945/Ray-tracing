#include "triangle.h"
#include "main.h"

using namespace std;
triangle::triangle(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,color_t col,float surface_reflec){
            this->v1 = v1;
            this->v2 = v2;
            this->v3 = v3;
            this->n = glm::normalize(glm::cross(v3-v2,v1-v2));
            this->color = col;
            this->surface_type = surface_reflec;
        }
triangle::triangle(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3,color_t col,float surface_reflec,int a){
            this->v1 = v1;
            this->v2 = v2;
            this->v3 = v3;
            this->n = -glm::normalize(glm::cross(v3-v2,v1-v2));
            this->color = col;
            this->surface_type = surface_reflec;
        }
bool triangle::PointInTriangle(glm::vec3 pt)
        {
            bool b1, b2, b3;
            b1 = sign(pt, v1, v2) < 0.0f;
            b2 = sign(pt, v2, v3) < 0.0f;
            b3 = sign(pt, v3, v1) < 0.0f;
            return ((b1 == b2) && (b2 == b3));
        }
float triangle::sign(glm::vec3 p1,glm::vec3 p2,glm::vec3 p3){
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}
bool triangle::RayIntersectsTriangle(Ray ray,float *t)
{
    const float EPSILON = 0.0000001;
    glm::vec3 e1, e2,s,h,q;
    float a,f,u,v;
    e1 = v2 - v1;
    e2 = v3 - v1;
    h = glm::cross(ray.dir,e2);
    a  = glm::dot(h,e1);
    // cout << a<<endl;
    if(abs(a)<EPSILON)return false;
    f = 1/a;
    s = ray.origin - v1;
    u = f * glm::dot(s,h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = glm::cross(s,e1);
    v = f * glm::dot(ray.dir,q);
    if (v < 0.0 || u + v > 1.0)return false;
    *t = f * glm::dot(e2,q);
    if(*t>0)return true;
    return false;
}


color_t triangle::get_color(glm::vec3 pos){
        if(this->color.r!=-1)return this->color;
        int a = floor(pos.x);
        int b = floor(pos.z);
        if( (a+b)%2==0)return {0.5,0,0};
        else return {0,0,0.5};

}


sphere::sphere(glm::vec3 cen,float r,color_t color,float surface_reflec){
    this->cen = cen;
    this->r = r;
    this->color =color;
    this->surface_type = surface_reflec;
}
glm::vec3 sphere::get_normal(glm::vec3 p){
    return normalize(p-this->cen);
}