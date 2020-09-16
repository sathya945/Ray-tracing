#include "triangle.h"
using namespace std;


// const int h = 400,w = 300;
// const int h = 1200,w = 900;
const int h = 1920,w = 1280;

int max_reflections = 6;

bool refraction  = false;
// bool refraction  = true;


float wall_reflec = 0.1;
float floor_reflec = 0;
float div_dis = 100;


unsigned char img[h][w][3];

glm::vec3 lookat=glm::vec3(5.0,5.0,0.0);
glm::vec3 cam_pos=glm::vec3(0,0,-8);
vector<sphere> sphere_list;
vector<triangle> triangles;

vector<glm::vec3> L_list{glm::vec3(-4,-5,6),glm::vec3(4,-5,3)};

float ambient = 0.1;
float diffuse_c = 1,specular_c = 0.5,specular_k = 50;

void push(){
    glm::mat4 M = glm::mat4(1.0f);
    draw_checker();
    sphere_list.push_back( sphere(glm::vec3(-3,4,8),2,{0.75,0.75,0.75},0.5));    
    // sphere_list.push_back( sphere(glm::vec3(4,4,8),2,{0.75,0.75,0.75},0.2));    
    
    sphere_list.push_back( sphere(glm::vec3(-4,5,4),1,{0,1,0},0)) ;    
    
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
    if(ray.inside){
    *t2 = tc - t1c;
	*t1 = tc + t1c;
    return true;
    }
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
            // if(t2<t1)cout << t1<<" "<< t2;
            
            if(t1<near_dis){
                near_dis =t1;
                obj_type = 1;
                obj_index = i;
            }
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

    return make_pair(make_pair(obj_type,obj_index),near_dis);
}

color_t find_color(Ray* ray){
    color_t ans= {ambient,ambient,ambient};
    pair<pair<int,int>,float> p = trace_ray(*ray);
    int obj_type = p.first.first;
    int obj_index = p.first.second;
    if(obj_type==-1)return {0,0,0};
    float t1,t2;
    glm::vec3 surface_point;
    surface_point = ray->origin + ray->dir*p.second;
    
    ray->intersect = surface_point;
    if(obj_type==1){
        bool temp = true;
        if(refraction && obj_index==0){
            glm::vec3 N = sphere_list[obj_index].get_normal(surface_point);
            glm::vec3 i = ray->dir;
            float nu =1.05;
            if(ray->inside){nu = 1/nu;N=-N;}
            // cout << "ha";
            float a=-glm::dot(i,N),b=nu*nu*sqrt(1-a*a);
            if(b<1){
            temp= false;
            // cout << ray->inside;
            ray->inside = !ray->inside;
             ray->reflec_dir = normalize( nu *i + (nu*a-sqrt(1-b) ) *N);
            //  cout << glm::dot(i,ray->reflec_dir);
        
             ray->surface_type = sphere_list[obj_index].surface_type;
            if(ray->inside) ray->intersect = surface_point - 0.0001f*ray->reflec_dir;
            else ray->intersect = surface_point + 0.0001f*ray->reflec_dir;
            }
        }

if(temp==true){
        glm::vec3 N = sphere_list[obj_index].get_normal(surface_point);
        glm::vec3 L = ray->dir;
        ray->reflec_dir = -normalize(2*glm::dot(N,L)*N-L);
        ray->surface_type = sphere_list[obj_index].surface_type;
        ray->intersect = surface_point + 0.0001f*ray->reflec_dir;
}



    }
    if(obj_type==2){
        glm::vec3 N = triangles[obj_index].n;
        glm::vec3 L = ray->dir;
        ray->intersect = surface_point + 0.0001f*L;
        ray->reflec_dir = -normalize(2*glm::dot(N,L)*N-L);
        ray->surface_type = triangles[obj_index].surface_type;
         ray->intersect = surface_point + 0.0001f*ray->reflec_dir;
    }

    for(int ii=0;ii<L_list.size();ii++){
    
    glm::vec3 L_pos = L_list[ii];
    Ray new_ray;

    new_ray.origin = L_pos,new_ray.dir = normalize(surface_point - L_pos);
    
    pair<pair<int,int>,float>p2 = trace_ray(new_ray);

    glm::vec3 new_surface_point =  new_ray.origin+ new_ray.dir*p2.second;
    if(p.first.first!= p2.first.first || glm::length(surface_point -new_surface_point) >0.1){
        continue;
        // return ans;
        }


    if(obj_type==1){
            int i = obj_index;
            glm::vec3 Lm = normalize(L_pos - surface_point);
            glm::vec3 V = -ray->dir;
            glm::vec3 N = sphere_list[i].get_normal(surface_point);
            glm::vec3 Rm = normalize(2*glm::dot(Lm,N)*N - Lm);
            float dis = 1+glm::length(ray->origin-surface_point)/div_dis;
            float diff = (1-sphere_list[i].surface_type) *diffuse_c * max(glm::dot(Lm,N), (float) 0.0)/(dis*dis); 
            ans.r += diff * sphere_list[i].color.r;
            ans.g += diff * sphere_list[i].color.g;
            ans.b += diff * sphere_list[i].color.b;
            float spec = (1-sphere_list[i].surface_type)* specular_c *(pow( max(glm::dot(Rm,V), (float) 0.0),specular_k)) / (dis*dis);
            ans.r += spec ;
            ans.g += spec;
            ans.b += spec;
    }
    if(obj_type==2){
            int i = obj_index;
            glm::vec3 Lm = normalize(L_pos - surface_point);
            glm::vec3 V = -ray->dir;
            glm::vec3 N = triangles[i].n;
            // if(glm::dot(Lm,N)<0) N=-N;
            glm::vec3 Rm = 2*glm::dot(Lm,N)*N - Lm;
            float dis = 1+glm::length(ray->origin-surface_point)/div_dis;
            float diff = (1-triangles[i].surface_type) * diffuse_c * max(glm::dot(Lm,N), (float) 0.0)/(dis*dis);
            color_t color = triangles[i].get_color(surface_point); 
            ans.r += diff * color.r;
            ans.g += diff * color.g;
            ans.b += diff * color.b;
            float spec = (1-triangles[i].surface_type)*specular_c *(pow( max(glm::dot(Rm,V), (float) 0.0),specular_k))/(dis*dis);
            ans.r += spec ;
            ans.g += spec;
            ans.b += spec;
    }
    }
    return ans;
}



int main(){
    push();

    // screen 
    float x_min=-5,x_max=5;
    float y_min=-5,y_max=5;
    float screen_z = 0;
    Ray ray;

    lookat.z = screen_z;
    ray.origin = cam_pos;
    for(int i=0;i<w;i++){
        for(int j=0;j<h;j++){
            float x = x_min + i * ((x_max-x_min)/(w-1));
            float y = y_min + j * ((y_max-y_min)/(h-1));
            lookat.x = x,lookat.y = y;

            ray.origin = cam_pos;
            ray.inside = false;
            ray.dir = normalize(lookat-cam_pos);

            color_t color = {0,0,0};
            int reflections = 0;
            float factor = 1;
            float sum =0;
            while(reflections<max_reflections){ 
                color_t new_color = find_color(&ray);
                if(new_color.r+new_color.g+new_color.b==0)break;
                color.r += factor*new_color.r;
                color.g += factor*new_color.g;
                color.b += factor*new_color.b;
                ray.origin = ray.intersect;
                ray.dir = ray.reflec_dir;
                sum+=factor;
                factor *= ray.surface_type;
                reflections += 1;
            }
            // cout <<endl;
            img[j][i][0] = min(int(color.r/sum*256),255);
            img[j][i][1] = min(int(color.g/sum*256),255);
            img[j][i][2] = min(int(color.b/sum*256),255);


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




// for (int i = -2; i < 2 ; ++i) {
//     for (int j = -2; j < 2; ++j) {
//         if((i + j)%2 == 0)color = {0, 0, 0};
//         else color = { 1, 1, 1};
//         v1 = glm::vec3(i*size,6,6+j*size);
//         v2 = glm::vec3((i+1)*size,6,6+j*size);
//         v3 = glm::vec3( (i+1)*size,6,6+(j+1)*size);
//         v4 = glm::vec3(i*size,6,6+(j+1)*size);
//         triangles.push_back(triangle(v1,v2,v3,color));
//         triangles.push_back(triangle(v3,v4,v1,color));
//     }
// }
//floor

color = {-1,-1,-1}; //floor 
v1 = glm::vec3(-2*size,6,6+(-2)*size);
v2 = glm::vec3(2*size,6,6+(-2)*size);
v3 = glm::vec3( 2*size,6,6+2*size);
v4 = glm::vec3(-2*size,6,6+2*size);
triangles.push_back(triangle(v1,v2,v3,color,floor_reflec));
triangles.push_back(triangle(v3,v4,v1,color,floor_reflec));

//celling
color = {0,0,0.5};
v1 = glm::vec3(-2*size,-6,6+(-2)*size);
v2 = glm::vec3(2*size,-6,6+(-2)*size);
v3 = glm::vec3( 2*size,-6,6+2*size);
v4 = glm::vec3(-2*size,-6,6+2*size);
triangles.push_back(triangle(v1,v2,v3,color,wall_reflec,-1));
triangles.push_back(triangle(v3,v4,v1,color,wall_reflec,-1));

color = {0.5,0.5,0};
v1 = glm::vec3(-6,-6,12);
v2 = glm::vec3(-6,6,12);
v3 = glm::vec3(6,6,12);
v4 = glm::vec3(6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color,wall_reflec));
triangles.push_back(triangle(v3,v4,v1,color,wall_reflec));

color = {0.5,0,0.5};
v1 = glm::vec3(-6,-6,12);
v2 = glm::vec3(-6,6,12);
v3 = glm::vec3(6,6,12);
v4 = glm::vec3(6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color,wall_reflec));
triangles.push_back(triangle(v3,v4,v1,color,wall_reflec));



color = {0,0.5,0};
v1 = glm::vec3(6,-6,0);
v2 = glm::vec3(6,6,0);
v3 = glm::vec3(6,6,12);
v4 = glm::vec3(6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color,wall_reflec,-1));
triangles.push_back(triangle(v3,v4,v1,color,wall_reflec,-1));

color = {0,0.5,0};
v1 = glm::vec3(-6,-6,0);
v2 = glm::vec3(-6,6,0);
v3 = glm::vec3(-6,6,12);
v4 = glm::vec3(-6,-6,12);
triangles.push_back(triangle(v1,v2,v3,color,wall_reflec));
triangles.push_back(triangle(v3,v4,v1,color,wall_reflec));

//prism

color = {0.75,0.75,0.75};
float prism_reflec = 0.3;
float l = 6,h=sqrt(3)*l/2,a=2,b=3;
v1 = glm::vec3(a,6,b);
v2 = glm::vec3(a-l/2,6,b+h);
v3 = glm::vec3(a+l/2,6,b+h);
v4 = glm::vec3(a,6-h+2,b+h/2);
triangles.push_back(triangle(v1,v2,v3,color,prism_reflec));
triangles.push_back(triangle(v4,v2,v1,color,prism_reflec));
triangles.push_back(triangle(v3,v2,v4,color,prism_reflec));
triangles.push_back(triangle(v1,v3,v4,color,prism_reflec));



// float l=1,b=1,h=3;

// float a[] = {
//         -1.0f,-1.0f,-1.0f, // triangle 1 : begin
//         -1.0f,-1.0f, 1.0f,
//         -1.0f, 1.0f, 1.0f, // triangle 1 : end
//         1.0f, 1.0f,-1.0f, // triangle 2 : begin
//         -1.0f,-1.0f,-1.0f,
//         -1.0f, 1.0f,-1.0f, // triangle 2 : end
//         1.0f,-1.0f, 1.0f,
//         -1.0f,-1.0f,-1.0f,
//         1.0f,-1.0f,-1.0f,
//         1.0f, 1.0f,-1.0f,
//         1.0f,-1.0f,-1.0f,
//         -1.0f,-1.0f,-1.0f,
//         -1.0f,-1.0f,-1.0f,
//         -1.0f, 1.0f, 1.0f,
//         -1.0f, 1.0f,-1.0f,
//         1.0f,-1.0f, 1.0f,
//         -1.0f,-1.0f, 1.0f,
//         -1.0f,-1.0f,-1.0f,
//         -1.0f, 1.0f, 1.0f,
//         -1.0f,-1.0f, 1.0f,
//         1.0f,-1.0f, 1.0f,
//         1.0f, 1.0f, 1.0f,
//         1.0f,-1.0f,-1.0f,
//         1.0f, 1.0f,-1.0f,
//         1.0f,-1.0f,-1.0f,
//         1.0f, 1.0f, 1.0f,
//         1.0f,-1.0f, 1.0f,
//         1.0f, 1.0f, 1.0f,
//         1.0f, 1.0f,-1.0f,
//         -1.0f, 1.0f,-1.0f,
//         1.0f, 1.0f, 1.0f,
//         -1.0f, 1.0f,-1.0f,
//         -1.0f, 1.0f, 1.0f,
//         1.0f, 1.0f, 1.0f,
//         -1.0f, 1.0f, 1.0f,
//         1.0f,-1.0f, 1.0f
//     };
    // color = {0,0.8,1};
    // glm::mat4 M = glm::mat4(1.0f);
    // M = glm::scale(M,glm::vec3(l,b,h));
    // M= glm::translate(glm::mat4(1.0f),glm::vec3(0,5,4))*M;
    
    // for(int v=0;v<6;v++){

    //         v1 = glm::vec3(a[9*v+0],a[9*v+1],a[9*v+2]);
    //         v2 = glm::vec3(a[9*v+3],a[9*v+4],a[9*v+5]);
    //         v3 = glm::vec3(a[9*v+6],a[9*v+7],a[9*v+8]);
    //         v1 = glm::vec3(M*glm::vec4(v1,1.0f));
    //         v2 = glm::vec3(M*glm::vec4(v2,1.0f));
    //         v3 = glm::vec3(M*glm::vec4(v3,1.0f));
    //         cout << v1.x << v1.y << v1.z <<endl;
    //         triangles.push_back(triangle(v1,v2,v3,color));
    // }


}