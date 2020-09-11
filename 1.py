import numpy as np
import matplotlib.pyplot as plt


# def find_intersection(ray,obj):
#     v1,v2,v3,v4 = obj[0],obj[1],obj[2],obj[3]
#     n = np.cross((v3-v2),(v1-v2))
#     n = n/np.linalg.norm(n)
#     t = np.dot((v1 - cam_pos),n)/np.dot(ray,n)
#     return ray*t+cam_pos,t

def sphere_ray(ray,obj):
    L = obj.cen - cam_pos
    tc = np.dot(ray,L)
    if  tc<0:
        return False
    


def func(ray):
    min_dis = 1e9
    pos_ans = None
    obj_ans = None
    for obj in obj_list:
        pos,dis = sphere_ray(ray,obj)
        if dis < min_dis:
            min_dis = dis
            pos_ans = pos
            

h,w = 400,300 

img = np.zeros((h, w, 3))
bg_col = np.array([0,0,0])

L_pos = np.array([5.0,10.0,5.0])
L_col = np.array([1,1,1])

cam_pos = np.array([5.0,5.0,-10])
lookat = np.array([5.0,5.0,0.0])



x_min,x_max = -1,11
y_min,y_max = -1,11


class sphere:
    def __init__(self,r,pos,color):
        self.r  = r
        self.cen = pos
        self.col = color
s1 = sphere(3,np.array([5.0,5.0,5.0]),np.array([1.0,0.0,0.0]))
obj_list = [s1]

for i in range(w):
    for j in range(h):
        x = x_min + i * ((x_max-x_min)/(w-1)) 
        y = y_min + j * ((y_max-y_min)/(h-1))
        lookat[0],lookat[1] = x,y 
        ray = lookat-cam_pos
        ray = ray/np.linalg.norm(ray)
        func(ray)
        # exit()




