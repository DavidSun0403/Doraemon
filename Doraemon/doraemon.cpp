#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>
#include "gsrc.h"
#include <windows.h>

#define PI 3.141592654
#define GRIDSIZE 31
#define WIN_POSX   100
#define WIN_POSY   100
#define WIN_WIDTH  1280
#define WIN_HEIGHT 720
#define	MAP_SIZE 1024
#define STEP_SIZE 16
#define SCALERATE 700
#define MAX_CHAR 128 
float a1 =-0.75, b1 =1.0, c1 =1.0, d1=1.0,f1=1.0,e1=0,g1=1.0,h1=1.0,s1=60.0,lastx=0,lasty=0,lastd=0;
GLfloat arm_angleZ=0.0,arm_angleY=0.0,leg_angleY=0.0,fly_angle=0.0,rotateAngle=0.1;
GLfloat direction_Z=0.0,direction_XY=0.0,distance = 0;
GLfloat lightX = 0, lightY = 15, lightZ = 15;
GLfloat nowx,nowy;
int totalTime;
GLint colorFlag = 1,planFlag = 1; 
int region = 20;
typedef unsigned char BYTE;
BYTE pheightInfo[MAP_SIZE * MAP_SIZE];
double t_prev,t_prev2,t,t_run;
int pause = -1;
int check1=1,check2=1,check3=1;
int bulletNum = 0;

typedef struct point{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vertex;

typedef struct bullet{
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat dis;
	GLfloat dire;
	GLfloat a;
	GLfloat nowx;
	GLfloat nowy;
	int side;
	int s;
} bullets;
vertex mesh [GRIDSIZE][GRIDSIZE];
bullets bulletBottle [100];
GLuint Texture;

GLuint loadBMP_custom(const char * filepath){
	unsigned char header[54];
	unsigned char * data;
	unsigned int bmp_width;
	unsigned int bmp_height;
	FILE * file = fopen(filepath,"rb");
	if(file!=NULL){
		fread(header,1,54,file);
		if ( header[0]!='B' || header[1]!='M' ){
			printf("Not a correct BMP file.\n");
			return false;
		}
		else{	
			 bmp_width  = *(int*)&(header[0x12]);
			 bmp_height  = *(int*)&(header[0x16]);		
		}	
	}
	data=(unsigned char *)malloc(bmp_width*bmp_height*3);
	fread(data,1,bmp_height*bmp_width*3,file);
	fclose(file);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D,3,bmp_width,bmp_height,GL_BGR_EXT,GL_UNSIGNED_BYTE,data);glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	free(data);	
	return textureID;
}

void Keyboard (GLubyte key, GLint xMouse, GLint yMouse) {         
	/* simple animation can be achieved by repeating key tabs */  
	switch (key) {  
		case 32:  
		pause = -pause;
		break;   
		case 97:
			s1 += 1;
		break;
		case 115:
			s1 -= 1;
		break;

	}  
	glutPostRedisplay ( );
}

void drawString(const char* str){  
    static int isFirstCall = 1;  
    static GLuint lists;  
  
    if (isFirstCall) {  
        isFirstCall = 0;  
        lists = glGenLists(MAX_CHAR);  
        wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);  
    }  
    for (; *str != '\0'; ++str) {  
        glCallList(lists + *str);  
    }  
}  

void creatBullet(){
	for(int i = 0; i<100; i++){
		int tmp = rand() % (2);
		int xy = rand() % (41);
		int zs = rand() % (11);
		int direction = rand() % (181);
		bulletBottle[i].side = tmp;
		bulletBottle[i].z = zs;
		bulletBottle[i].dire = direction;
		bulletBottle[i].s = 0;
		switch(tmp){
			case 0:
				bulletBottle[i].x = 0;
				bulletBottle[i].y = xy-20;
				break;
			case 1:
				bulletBottle[i].x = 40;
				bulletBottle[i].y = xy-20;
				break;
			case 2:
				bulletBottle[i].y = 20;
				bulletBottle[i].x = xy;
				break;
			case 3:
				bulletBottle[i].y = -20;
				bulletBottle[i].x = xy;
				break;
		}
	}
}
void specialkeyboard(int key, int xMouse, int yMouse) {  
	if(pause == -1){
		switch (key) {  
			case GLUT_KEY_LEFT:
			lastx = lastx+(distance-lastd)*cos(direction_XY*PI/180);
			lasty = lasty+(distance-lastd)*sin(direction_XY*PI/180);
			lastd = distance;
			direction_XY-=10;
			break;
			case GLUT_KEY_RIGHT:
			lastx = lastx+(distance-lastd)*cos(direction_XY*PI/180);
			lasty = lasty+(distance-lastd)*sin(direction_XY*PI/180);
			lastd = distance;
			direction_XY+=10;
			break;
			case GLUT_KEY_UP:
				if(direction_Z<20)
					direction_Z+=0.1;
			break;
			case GLUT_KEY_DOWN:
				if(direction_Z>0)
					direction_Z-=0.1;
			break;
		
			default:
			printf ("invalid choices!");   
		}
	}
	glutPostRedisplay ( );
}
void xyz(){
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(5,0,0);
	glEnd();
	glColor3f(0,1,0);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,5,0);
	glEnd();
	glColor3f(0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(0,0,5);
	glEnd();
		
}
void drawmesh(){
	for (int i=0; i<GRIDSIZE-1; i++)
		for (int j=0; j<GRIDSIZE-1; j++){		
			if(i>28)
				glColor3f(0,0,0);
			glBegin(GL_TRIANGLES);
				glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
				glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

				glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
				glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);

				glNormal3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
				glVertex3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
			glEnd();

			glBegin(GL_TRIANGLES);
				glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
				glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

				glNormal3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);
				glVertex3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);

				glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
				glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);	
			glEnd();
		}
}

void drawline(){
	int K=GRIDSIZE-1;
	for (int i=0; i<K; i++)
		for (int j=0; j<K; j++){		
			glBegin(GL_LINES);
				glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
				glVertex3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);

				glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
				glVertex3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);
			glEnd();
		}
	
	for (int i=0; i< K; i++){
		glBegin(GL_LINES);
			glVertex3f(mesh[i][K].x,mesh[i][K].y,mesh[i][K].z);
			glVertex3f(mesh[i+1][K].x, mesh[i+1][K].y,mesh[i+1][K].z);
		glEnd();
	}

	for (int j=0; j< K; j++){
		glBegin(GL_LINES);
			glVertex3f(mesh[K][j].x,mesh[K][j].y,mesh[K][j].z);
			glVertex3f(mesh[K][j+1].x, mesh[K][j+1].y,mesh[K][j+1].z);
		glEnd();
	}
}

void ground(){
	glPushMatrix();
	glColor3f(0.753,0.725,0.62);
	glTranslatef(0,0,-2.01);
	glScalef(100,100,0.5);
	glutSolidCube(1);
	glPopMatrix();
}

void light(){
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glColor3f(0.95,0.95,0.1);
	glTranslatef(lightX,lightY,lightZ);
	glutSolidSphere(1,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
	glEnable(GL_LIGHTING);
}

void fly(int z){
	glPushMatrix();
	if(z == 0){
		glColor3f(1,1,0);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidCone(0.05,0.2,GRIDSIZE,GRIDSIZE);
	glTranslatef(0,0,0.2);
	glRotatef(90,1,0,0);
	glPushMatrix();
	glTranslatef(0,0,-0.3);
	glutSolidCone(0.05,0.3,GRIDSIZE,GRIDSIZE);
	glPopMatrix();

	glPushMatrix();
	glRotatef(90,0,1,0);
	glTranslatef(0,0,-0.3);
	glutSolidCone(0.05,0.3,GRIDSIZE,GRIDSIZE);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-180,0,1,0);
	glTranslatef(0,0,-0.3);
	glutSolidCone(0.05,0.3,GRIDSIZE,GRIDSIZE);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-90,0,1,0);
	glTranslatef(0,0,-0.3);
	glutSolidCone(0.05,0.3,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
	glPopMatrix();

}

void eye(int z){
		for (int i=0; i<GRIDSIZE; i++)
			for (int j=0; j<GRIDSIZE; j++){
				float i0=(float)i/(GRIDSIZE-1);
				float j0=(float)j/(GRIDSIZE-1);
				mesh[i][j].y = 0.2*i0*cos(2*PI*j0);
				mesh[i][j].z = 0.3*i0*sin(2*PI*j0);	  
				mesh[i][j].x=  1.005*sqrt(1-powf(mesh[i][j].y/1.32,2)-powf(mesh[i][j].z,2));
			}
			
		for (int i=0; i<GRIDSIZE-1; i++)
			for (int j=0; j<GRIDSIZE-1; j++){	
				if(i>27||i<7){
					glColor3f(0,0,0);
				}
				else{
					if(z == 0){
						glColor3f(1,1,1);
					}else{
						glColor3f(0,0,0);
					}
				}
				glBegin(GL_TRIANGLES);
					glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
					glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

					glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
					glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);

					glNormal3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
					glVertex3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
				glEnd();

				glBegin(GL_TRIANGLES);
					glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
					glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

					glNormal3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);
					glVertex3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);

					glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
					glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);	
				glEnd();
			}
}

void line(void){
	int const SIZE=9;
	float array_y[SIZE]={0,-0.25,-0.25,-0.25,0,0.25,0.25,0.25,0};
	float array_z[SIZE]={1,1,0,-1,-1,-1,0,1,1};
	for (int i=0; i<SIZE; i++)
			for (int j=0; j<SIZE; j++){
				float i0=(float)i/(SIZE-1);
				  mesh[i][j].y = 0.05*i0*array_y[j];
				  mesh[i][j].z = 0.2*i0*array_z[j];  
				  mesh[i][j].x=	 1.006*sqrt(1-powf(mesh[i][j].y,2)-powf(mesh[i][j].z,2));
			}
	glColor3f(0,0,0);
	for (int i=0; i<SIZE-1; i++)
		for (int j=0; j<SIZE-1; j++){			
			glBegin(GL_TRIANGLES);
				glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
				glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

				glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
				glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);

				glNormal3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
				glVertex3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
			glEnd();

			glBegin(GL_TRIANGLES);
				glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
				glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

				glNormal3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);
				glVertex3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);

				glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
				glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);	
			glEnd();
		}
}

void linex3(void){
	glPushMatrix();
	glRotatef(60,1,0,0);
	line();	
	glRotatef(30,1,0,0);
	glRotatef(-10,0,0,1);
	line();
	glRotatef(-10,0,0,1);
	glRotatef(30,1,0,0);
	line();
	glPopMatrix();
}

void white(int z){
	for (int i=0; i<GRIDSIZE; i++)
		for (int j=0; j<GRIDSIZE; j++){
			float i0=(float)i/(GRIDSIZE-1);
			float j0=(float)j/(GRIDSIZE-1);
	     
				mesh[i][j].y = 0.75*i0*cos(2*PI*j0);
				mesh[i][j].z = 0.75*i0*sin(2*PI*j0);	  
				mesh[i][j].x=  1.004*sqrt(1-powf(mesh[i][j].y,2)-powf(mesh[i][j].z,2));
		}
		if(z == 0){
			glColor3f(1,1,1);
		}else{
			glColor3f(0,0,0);
		}
	for (int i=0; i<GRIDSIZE-1; i++)
		for (int j=0; j<GRIDSIZE-1; j++){	
			if(i>28){
				glColor3f(0,0,0);
			}
			else{
				if(z == 0){
					glColor3f(1,1,1);
				}else{
					glColor3f(0,0,0);
				}
			}
		glBegin(GL_TRIANGLES);
			glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
			glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

			glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
			glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);

			glNormal3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
			glVertex3f(mesh[i][j+1].x,mesh[i][j+1].y,mesh[i][j+1].z);	
		glEnd();

		glBegin(GL_TRIANGLES);
			glNormal3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);
			glVertex3f(mesh[i][j].x,mesh[i][j].y,mesh[i][j].z);

			glNormal3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);
			glVertex3f(mesh[i+1][j].x,mesh[i+1][j].y,mesh[i+1][j].z);

			glNormal3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);
			glVertex3f(mesh[i+1][j+1].x,mesh[i+1][j+1].y,mesh[i+1][j+1].z);	
		glEnd();
		}

}

void redBall(int z){
	glPushMatrix();
	if(z == 0){
		glColor3f(1,0,0);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidSphere(0.1,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
}

void mouth(float rr1,float rr2,float c_b,float c_g,int z){
	for (int i=0; i<GRIDSIZE; i++)
		for (int j=0; j<GRIDSIZE; j++){
			float i0=(float)i/(GRIDSIZE-1);
			float j0=(float)j/(GRIDSIZE-1);
	     
				mesh[i][j].y = rr2*i0*cos(-PI+PI*j0);
				mesh[i][j].z = rr2*i0*sin(-PI+PI*j0);	  
				mesh[i][j].x=  rr1*sqrt(1-powf(mesh[i][j].y,2)-powf(mesh[i][j].z,2));
		}
		if(z == 0){
			glColor3f(1,c_b,c_g);
		}else{
			glColor3f(0,0,0);
		}
		drawmesh();
}

void head(int z){
	glPushMatrix();
	if(z == 0){
		glColor3f(0,0,1);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidSphere(1,GRIDSIZE,GRIDSIZE);
	white(z);

	glPushMatrix();
	glRotatef(-45,0,1,0);
	glRotatef(11,0,0,1);
	eye(z);
	glRotatef(-22,0,0,1);
	eye(z);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-30,0,1,0);
	glTranslatef(1.05,0,0);
	redBall(z);
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(-15,0,1,0);
	glScalef(1,1,0.9);
	line();
	glPopMatrix();
	
	glPushMatrix();
	glRotatef(-10,0,1,0);
	mouth(1.005,0.3,0,0,z);
	glPopMatrix();

	glPopMatrix();
}

void torus(int z){
	glPushMatrix();
	if(z == 0){
		glColor3f(1,0,0);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidTorus(0.05,0.9,GRIDSIZE,GRIDSIZE);
	glTranslatef(1,0,0);
	if(z == 0){
		glColor3f(1,1,0);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidSphere(0.1,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
}

void body(int z){
	glPushMatrix();
	if(z == 0){
		glColor3f(0,0,1);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidSphere(0.9,GRIDSIZE,GRIDSIZE);
	mouth(0.9005,0.4,1,1,z);
	glPopMatrix();
}

void arm(int z){
	glPushMatrix();
	glTranslatef(0,0,0.3);
	glPushMatrix();
	if(z == 0){
		glColor3f(0.85,0.85,0.85);
	}else{
		glColor3f(0,0,0);
	}
	glTranslatef(0,0,0.3);
	glutSolidSphere(0.15,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
	glScalef(0.4,0.4,1);
	if(z == 0){
		glColor3f(0,0,1);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidSphere(0.3,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
}

void bulletAnimation(int z){
	for(int i =0; i<100; i++){
		if(bulletBottle[i].s == 1){
			glPushMatrix();
			if(z == 0){
				glColor3f(1,0,0);
			}else{
				glColor3f(0,0,0);
			}
			switch(bulletBottle[i].side){
				case 0:
					bulletBottle[i].nowx = bulletBottle[i].x+bulletBottle[i].dis*sin(bulletBottle[i].dire*PI/180);
					bulletBottle[i].nowy = bulletBottle[i].y+bulletBottle[i].dis*cos(bulletBottle[i].dire*PI/180);
					break;
				case 1:
					bulletBottle[i].nowx = bulletBottle[i].x-bulletBottle[i].dis*sin(bulletBottle[i].dire*PI/180);
					bulletBottle[i].nowy = bulletBottle[i].y+bulletBottle[i].dis*cos(bulletBottle[i].dire*PI/180);
					break;
			}
			glTranslatef(bulletBottle[i].nowx,bulletBottle[i].nowy,bulletBottle[i].z);
			glutSolidSphere(1,GRIDSIZE,GRIDSIZE);
			glPopMatrix();
		}
	}
}
void hand(float rY,float rZ,int z)
{
	glPushMatrix();
	glTranslatef(0,-0.85,-0.75);
	glRotatef(-90-rZ,0,0,1);
	glRotatef(90+rY,0,1,0);
	arm(z);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0.85,-0.75);
	glRotatef(90+rZ,0,0,1);
	glRotatef(90+rY,0,1,0);
	arm(z);
	glPopMatrix();
}

void ball(int z)
{
	glPushMatrix();
	glTranslatef(0,0,-0.3*0.5);
	glScalef(1,1,0.5);
	if(z == 0){
		glColor3f(0.85,0.85,0.85);
	}else{
		glColor3f(0,0,0);
	}
	glutSolidSphere(0.3,GRIDSIZE,GRIDSIZE);
	glPopMatrix();
}
void foot(float rY,int z)
{
	glPushMatrix();
	glTranslatef(0,-0.45,-1.5);
	glRotatef(rY,0,1,0);
	ball(z);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0.45,-1.5);
	glRotatef(rY,0,1,0);
	ball(z);
	glPopMatrix();
}

void doraemon(int z)
{
	glPushMatrix();
	glTranslatef(0,0,direction_Z);
	float nowx = lastx+(distance-lastd)*cos (direction_XY*PI/180);
	float nowy = lasty+(distance-lastd)*sin (direction_XY*PI/180);
	glTranslatef(nowx,nowy,0);
	glPushMatrix();
	glRotatef(direction_XY,0,0,1);
	head(z);
	glPushMatrix();
	glTranslatef(0,0,-0.75);
	body(z);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.8,0,-1.25);
	redBall(z);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0,0,-0.5);
	torus(z);
	glPopMatrix();

	glPushMatrix();
	glRotatef(-20,0,1,0);
	glRotatef(-45,0,0,1);
	linex3();
	glRotatef(90,0,0,1);
	glScalef(1,-1,1);
	linex3();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0,0,1);
	glRotatef(fly_angle,0,0,1);
	fly(z);
	glPopMatrix();

	hand(arm_angleY,arm_angleZ,z);
	foot(leg_angleY,z);
	glPopMatrix();
	glPopMatrix();
}

void checkCollision(){
	int checkC = 0;
	float distanceCheck = 10000.0;
	for(int i = 0; i < 100; i++){
		if(bulletBottle[i].s == 1){
		float bulletX = bulletBottle[i].x+bulletBottle[i].dis*sin(bulletBottle[i].dire*PI/180);
		float bulletY = bulletBottle[i].x+bulletBottle[i].dis*sin(bulletBottle[i].dire*PI/180);
		float dis_DB = (nowx-bulletBottle[i].nowx)*(nowx-bulletBottle[i].nowx)+(nowy-bulletBottle[i].nowy)*(nowy-bulletBottle[i].nowy)+(direction_Z-bulletBottle[i].z)*(direction_Z-bulletBottle[i].z);
		if(dis_DB < distanceCheck)
			distanceCheck = dis_DB;
		}
	}
	if(distanceCheck < 6){
		pause = 1;
		totalTime = glutGet(GLUT_ELAPSED_TIME);
	}	
}
void animationFcn(void){
	t = glutGet(GLUT_ELAPSED_TIME);
	lightY  = 30 * cos(t*PI/18000);
	lightZ  = 30 * sin(t*PI/18000);
	if(pause == -1){
		if(t-t_prev2>400){
			bulletBottle[bulletNum].s = 1;
			bulletNum++;
			if(bulletNum >99){
				bulletNum = 0;
				for(int i =0; i < 100; i++){
					bulletBottle[i].dis = 0;
					bulletBottle[i].s = 0;
				}
			}
			t_prev2 = t;
		}
		if(t-t_prev>10){
			for(int n = 0; n <= bulletNum; n++){
				bulletBottle[n].dis +=0.15;
			}
			fly_angle+=0.2*(t-t_prev);
			double swing_time  = 1000.0;	

			if(fly_angle>=360)
				fly_angle=0;

			if(leg_angleY<=-20)
				check1=1;
			else if(leg_angleY>=20)
				check1=0;
	
			if(check1==1)
				leg_angleY+=0.5*(t-t_prev);
			else
				leg_angleY-=0.5*(t-t_prev);

			if(check2==1)
				arm_angleY+=0.5*(t-t_prev);
			else
				arm_angleY-=0.5*(t-t_prev);

			if(arm_angleY<=-20)
				check2=1;
			else if(arm_angleY>=20)
				check2=0;
			nowx=lastx+(distance-lastd)*cos (direction_XY*PI/180);
			nowy = lasty+(distance-lastd)*sin (direction_XY*PI/180);
			if((nowx>2*region && cos (direction_XY*PI/180) >0)||(nowx<0 && cos (direction_XY*PI/180) <0)||(nowy>region && sin (direction_XY*PI/180) >0)||(nowy<-region && sin (direction_XY*PI/180) <0)) {
			
			}else{
				distance += 0.1;
			}
			if(t-1000 > t_run)
				checkCollision();
			t_prev = t;
		}
	}
	glutPostRedisplay();
}

void displayobject (void){
	
  GLint viewport[4];
 
  glGetIntegerv( GL_VIEWPORT, viewport ); 
  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( 45, double(viewport[2])/viewport[3], 0.1, 100);
   glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,5,s1, 0,0,0, 0,1,0 );
    glMultMatrixf( gsrc_getmo() );  
    glClear (GL_DEPTH_BUFFER_BIT);
    glEnable (GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable (GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT); 
	glRotated(-90,1,0,0);
	glRotated(-90,0,0,1);
	//xyz();
	//ground();
	light();
	bulletAnimation(0);
	GLfloat lightPosType[] = {lightX,lightY,-lightZ,0};
	glLightfv(GL_LIGHT0,GL_POSITION,lightPosType);
	GLfloat M[16] = {1,0,0,0,0,1,0,0,0,0,1,-1/(lightZ+direction_Z+1.72),0,0,0,0};
	doraemon(0);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(lightX,lightY,lightZ);
	glMultMatrixf(M);
	glTranslatef(-lightX,-lightY,-lightZ);
	if(lightZ>0)
		doraemon(1);
	glPopMatrix();
	//show life time
	glPushMatrix();
     glColor3f(1.0f, 0.0f, 0.0f);
	 glTranslatef(0,0,20);
     glRasterPos2f(1.5f, -1.5f);
	glScalef(15,15,15);
	if(pause == -1){
		float gameTime = (glutGet (GLUT_ELAPSED_TIME) - t_run)/1000;
		char *buffer = new char[30];
		sprintf(buffer,"%.3f",gameTime);
		drawString(buffer);
	}else{
		float gameTime = (totalTime - t_run)/1000;
		char *buffer = new char[30];
		sprintf(buffer,"%.3f",gameTime);
		drawString(buffer);
	}
	glPopMatrix();
	glPushMatrix();
	glEnable (GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,Texture);
	glColor3f(1,1,1);
	glBegin(GL_QUADS);
	glTexCoord2f (0.0, 0.0); glVertex3f (-100.0,-100.0,-5.1);
	glTexCoord2f (1.0, 0.0); glVertex3f (100.0,-100.0,-5.1);
	glTexCoord2f (1.0, 1.0); glVertex3f (100.0,100.0,-5.1);
	glTexCoord2f (0.0, 1.0); glVertex3f (-100.0,100.0,-5.1);
	glEnd ();
	glDisable (GL_TEXTURE_2D);
	glPopMatrix();
	glutSwapBuffers();
}

void main (int argc, char** argv){
  creatBullet();
  glutInit (&argc, argv);                                   
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  
  glutInitWindowPosition( WIN_POSX, WIN_POSY );                
  t_prev = glutGet (GLUT_ELAPSED_TIME); 
  t_prev2 = glutGet (GLUT_ELAPSED_TIME);
  t_run =glutGet (GLUT_ELAPSED_TIME);
  glutInitWindowSize( WIN_WIDTH, WIN_HEIGHT );		        
  glutCreateWindow( "DORAEMON" ); 
  glutKeyboardFunc (Keyboard);
  glutSpecialFunc(specialkeyboard);
  glutIdleFunc (animationFcn);

  glutMouseFunc( gsrc_mousebutton );
  glutMotionFunc( gsrc_mousemove );
  Texture = loadBMP_custom("floor.bmp");  
  glutDisplayFunc( displayobject );	 
  glutMainLoop();

}