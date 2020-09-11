#include "triangle.h"
using namespace std;


const int h = 1200,w = 900;
unsigned char img[h][w][3];

glm::vec3 L_pos=glm::vec3(-4,-6,-2),lookat=glm::vec3(5.0,5.0,0.0);
glm::vec3 cam_pos=glm::vec3(0,0,-8);
vector<sphere> sphere_list;
vector<triangle> triangles;

float ambient = .1;
float diffuse_c = 1,specular_c = 0.5,specular_k = 50;

void push(){
    glm::mat4 M = glm::mat4(1.0f);
    draw_checker();

    // sphere s1 = sphere()
    // sphere_list.push_back( sphere(glm::vec3(5,5,2),3,{0,0,1}) );
    // sphere_list.push_back( sphere(glm::vec3(5,10,5),3,{0,1,0}) );
   
   
    sphere_list.push_back( sphere(glm::vec3(-4,5,4),1,{1,0,0}) );
    // sphere_list.push_back( sphere(glm::vec3(-2,-3,2),1,{0,1,0}) );
    

}


	
bool intersect(Ray ray, sphere s, float* t1, float *t2)
{
	//solve for tc
	glm::vec3 L = s.cen - ray.origin;
	float tc = glm::dot(L, ray.dir);
	
	if ( tc < 0.0 ) return false;
	
    float d2 = glm::length(L)*glm::length(L) - (tc*tc) ;
	float radius2 = s.r * s.r;
	if ( d2 > radius2) return false;

	//solve for t1c
	float t1c = sqrt( radius2 - d2 );

	//solve for intersection points
	*t1 = tc - t1c;
	*t2 = tc + t1c;
	
	return true;
}

pair<pair<int,int>,float> trace_ray(Ray ray){
    float near_dis = 1000;
    glm::vec3 near_intersection;
    int obj_type=-1;
    int obj_index;
    color_t ans= {ambient,ambient,ambient};
    for(int i=0;i<sphere_list.size();i++){
        float t1,t2;
        if(intersect(ray,sphere_list[i],&t1,&t2)){
            // glm::vec3 temp = ray.origin + t1 * ray.dir;
            if(t1<near_dis){
                near_dis =t1;
                obj_type = 1;
                obj_index = i;
                // near_intersection = temp;
            }
            // cout << glm::length(temp-sphere_list[i].cen)<< " "<< ans << endl;
        }
    }

    for(int i=0;i<triangles.size();i++){
        float t;
        if(triangles[i].RayIntersectsTriangle(ray,&t)){
            if(t<near_dis){
                near_dis = t;
                obj_type =2;
                obj_index = i;
            }

        }
    }

    // cout << obj_type <<endl;
    return make_pair(make_pair(obj_type,obj_index),near_dis);
}

color_t find_color(Ray ray){

    color_t ans= {ambient,ambient,ambient};
    pair<pair<int,int>,float> p = trace_ray(ray);
    int obj_type = p.first.first;
    int obj_index = p.first.second;
    if(obj_type==-1)return {0,0,0};
    float t1,t2;
    glm::vec3 surface_point;
    surface_point = ray.origin + ray.dir*p.second;

    Ray new_ray;
    new_ray.origin = L_pos,new_ray.dir = normalize(surface_point - L_pos);
    
    pair<pair<int,int>,float>p2 = trace_ray(new_ray);

    glm::vec3 new_surface_point =  new_ray.origin+ new_ray.dir*p2.second;
    if(p.first.first!= p2.first.first || glm::length(surface_point -new_surface_point) >0.1){
        // cout << surface_point.x << " " << surface_point.y  << " " << surface_point.z <<endl;
        // cout << new_surface_point.x << " " << new_surface_point.y  << " " << new_surface_point.z <<endl;
        return ans;
        }
    if(obj_type==1){
            int i = obj_index;
            // return sphere_list[i].color;
            float len = max(1.0f,glm::length(L_pos - surface_point));
            glm::vec3 Lm = normalize(L_pos - surface_point);
            glm::vec3 V = -ray.dir;
            glm::vec3 N = sphere_list[i].get_normal(surface_point);
            glm::vec3 Rm = 2*glm::dot(Lm,N)*N - Lm;
            float diff = diffuse_c * max(glm::dot(Lm,N), (float) 0.0); 
            ans.r += diff * sphere_list[i].color.r;
            ans.g += diff * sphere_list[i].color.g;
            ans.b += diff * sphere_list[i].color.b;
            float spec = specular_c *(pow( max(glm::dot(Rm,V), (float) 0.0),specular_k));
            ans.r += spec ;
            ans.g += spec;
            ans.b += spec;
    }
    if(obj_type==2){
            int i = obj_index;
            // return triangles[i].color;

            glm::vec3 Lm = normalize(L_pos - surface_point);
            glm::vec3 V = -ray.dir;
            glm::vec3 N = triangles[i].n;
            // if(glm::dot(Lm,N)<0) N=-N;
            glm::vec3 Rm = 2*glm::dot(Lm,N)*N - Lm;
            float diff = diffuse_c * max(glm::dot(Lm,N), (float) 0.0); 
            ans.r += diff * triangles[i].color.r;
            ans.g += diff * triangles[i].color.g;
            ans.b += diff * triangles[i].color.b;
            float spec = specular_c *(pow( max(glm::dot(Rm,V), (float) 0.0),specular_k));
            ans.r += spec ;
            ans.g += spec;
            ans.b += spec;
    }
    return ans;
}



int main(){
    push();
    // screen 
    float x_min=-5,x_max=5;
    float y_min=-5,y_max=5;
    float screen_z = -3;
    Ray ray;

    lookat.z = screen_z;
    ray.origin = cam_pos;
    for(int i=0;i<w;i++){
        for(int j=0;j<h;j++){
            float x = x_min + i * ((x_max-x_min)/(w-1));
            float y = y_min + j * ((y_max-y_min)/(h-1));
            lookat.x = x,lookat.y = y;
            ray.dir = normalize(lookat-cam_pos);

            // ray.origin = lookat;
            // ray.dir = glm::vec3(0,0,1);


            color_t color = find_color(ray);
            img[j][i][0] = min(int(color.r*256),255);
            img[j][i][1] = min(int(color.g*256),255);
            img[j][i][2] = min(int(color.b*256),255);
        }
    }





FILE *imageFile;
   int x,y,pixel,height=h,width=w;

   imageFile=fopen("image.ppm","wb");
   if(imageFile==NULL){
      perror("ERROR: Cannot open output file");
      exit(EXIT_FAILURE);
   }

   fprintf(imageFile,"P6\n");               // P6 filetype
   fprintf(imageFile,"%d %d\n",width,height);   // dimensions
   fprintf(imageFile,"255\n");              // Max pixel

   fwrite(img,1,h*w*3,imageFile);
   fclose(imageFile);




}




glm::vec3 normalize(glm::vec3 a){
    return a/glm::length(a);
}



void draw_checker(){

color_t color;
float size = 3;
glm::vec3 v1,v2,v3,v4;




for (int i = -2; i < 2 ; ++i) {
    for (int j = -2; j < 2; ++j) {
        if((i + j)%2 == 0)color = {0, 0, 0};
        else color = { 1, 1, 1};
        v1 = glm::vec3(i*size,6,6+j*size);
        v2 = glm::vec3((i+1)*size,6,6+j*size);
        v3 = glm::vec3( (i+1)*size,6,6+(j+1)*size);
        v4 = glm::vec3(i*size,6,6+(j+1)*size);
        triangles.push_back(triangle(v1,v2,v3,color));
        triangles.push_back(triangle(v3,v4,v1,color));
    }
}




color = {1,1,0};
v1 = glm::vec3(-6,-6,12);
v2 = glm::vec3(-6,6,12);
v3 = glm::vec3(6,6,12);
v4 = glm::vec3(6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color));
triangles.push_back(triangle(v3,v4,v1,color));



color = {1,0,0};
v1 = glm::vec3(6,-6,0);
v2 = glm::vec3(6,6,0);
v3 = glm::vec3(6,6,12);
v4 = glm::vec3(6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color,-1));
triangles.push_back(triangle(v3,v4,v1,color,-1));

color = {0,1,0};
v1 = glm::vec3(-6,-6,0);
v2 = glm::vec3(-6,6,0);
v3 = glm::vec3(-6,6,12);
v4 = glm::vec3(-6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color));
triangles.push_back(triangle(v3,v4,v1,color));


float l=1,b=1,h=3;

float a[] = {
        -1.0f,-1.0f,-1.0f, // triangle 1 : begin
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, // triangle 1 : end
        1.0f, 1.0f,-1.0f, // triangle 2 : begin
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f, // triangle 2 : end
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };
    color = {0,0.8,1};
    glm::mat4 M = glm::mat4(1.0f);
    M = glm::scale(M,glm::vec3(l,b,h));
    M= glm::translate(glm::mat4(1.0f),glm::vec3(0,5,4))*M;
    
    for(int v=0;v<6;v++){

        // if(v<2){
            v1 = glm::vec3(a[9*v+0],a[9*v+1],a[9*v+2]);
            v2 = glm::vec3(a[9*v+3],a[9*v+4],a[9*v+5]);
            v3 = glm::vec3(a[9*v+6],a[9*v+7],a[9*v+8]);
            v1 = glm::vec3(M*glm::vec4(v1,1.0f));
            v2 = glm::vec3(M*glm::vec4(v2,1.0f));
            v3 = glm::vec3(M*glm::vec4(v3,1.0f));
            cout << v1.x << v1.y << v1.z <<endl;
            triangles.push_back(triangle(v1,v2,v3,color));
        // }
    }


}