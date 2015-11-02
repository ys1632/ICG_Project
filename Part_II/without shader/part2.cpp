#define WINDOWS     1 /* Set to 1 for Windows, 0 else */
#define UNIX_LINUX  0 /* Set to 1 for Unix/Linux, 0 else */

#if WINDOWS
#include <windows.h>
#include <GL/glut.h>
#endif

#if UNIX_LINUX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;

//decide the window position
#define XOFF          400
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

void display(void);
void reshape(int,int);  
void idle(void);
void open_file(void);

void rb(int,int,int,int);
void keyboard(unsigned char, int ,int);
void create_menu();
void menu(int);

void roll_sphere();
void enhance_roll(int);
void draw_background();
void draw_sphere();

float xt=-4.0,zt=4.0;
float xr=-8.0,zr=-7.0;
bool roll=true;
int num;   //store the number of the triangles
int roll_path=0; //initial the path, you can see 3 paths later
GLfloat * vertices; 
GLfloat M[16]={1,0,0,0,
	           0,1,0,0,
			   0,0,1,0,
			   0,0,0,1};
GLfloat viewer[]={7,3,-10}; //VRP
GLfloat angle[]={0.0,0.0,0.0};

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(XOFF, YOFF);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("CS6533 Assignment 2 (Yixiao Shen)");
    glutDisplayFunc(display);
	glutReshapeFunc(reshape);                       
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	create_menu();
	open_file();
	glEnable(GL_DEPTH_TEST);
    glutMainLoop();
	delete [] vertices;	
	
	return 0;
}

//File read function
void open_file(void)
{
int n,count=0;
fstream sphere_file;
char fileName[20];
bool fail_to_open=true;
		do
		{
			cout<<"Please choose the input file -- sphere.8 or sphere.128?"<<endl<<""<<endl;
			cin>>fileName;
			sphere_file.open(fileName,ios::in);
			if(!sphere_file.is_open()){
				cout<<"Warning! Fail to open file or can't find the file!"<<endl<<""<<endl;
			}
			else
			fail_to_open=false;
		}while(fail_to_open);
		sphere_file>>num;
		int count_num=num;
		vertices = new GLfloat[count_num*9];
		while (count_num>0)
		{
			sphere_file>>n;
			if(n==3)
				{
					for(int i=0;i<9;i++)
					sphere_file>>vertices[count+i];
				}
			count+=9;
			count_num--;
		}
		sphere_file.close();
}

//display function
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  draw_background();
  glTranslatef(xt,1.0,zt);
  glRotatef(angle[roll_path], xr, 0.0, zr);	
  glMultMatrixf(M);	
  draw_sphere();
  glFlush();                         
  glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutSwapBuffers();                   
}


void reshape(int w, int h)
{
  glClearColor(0.529, 0.807, 0.92, 0.0); //set the background color sky blue	
  glViewport(0, 0, w, h);           
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45,(float)w/(float)h,1,50);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void idle(void)
{
	glutPostRedisplay(); 
}

void roll_sphere()
{
	float speed = 0.1;	//change the sphere's moving speed here														
	float ds= (speed*2*3.14159265358)/(360*sqrt(xr*xr+zr*zr));	
	angle[roll_path] +=speed;														
	if(angle[roll_path]>=360.0) angle[roll_path]-=360.0;
	xt=xt-zr*ds;													
	zt=zt+xr*ds;
	//3 paths for rolling: A-->B, B-->C, C-->A 
	if(roll_path==0 && (xt>3 || zt<-4))
		{			
			enhance_roll(speed);
			xr=1.0; zr=6.0;
			roll_path++;
		}
	else if(roll_path==1 && (xt<-3.0 || zt>-3.0))
		{			
			enhance_roll(speed);
			xr=7.0; zr=1.0;
			roll_path++;
		}
	else if(roll_path==2 && (xt<-4 || zt>4))
		{		
			enhance_roll(speed);
			xr=-8; zr=-7;
			roll_path=0;
		}
	glutPostRedisplay();
}

void enhance_roll(int s)
{
	glLoadIdentity();				
	glRotatef(angle[roll_path]-s, xr, 0.0, zr);				
	glMultMatrixf(M);												
	glGetFloatv(GL_MODELVIEW_MATRIX, M);							
    angle[roll_path]=s;
}

//use keyboard to change the viewer position
//uppercase letter to increase and lowercase letter to decrease
void keyboard(unsigned char k,int x,int y)
{
	if(k=='x')viewer[0]-=1.0;												
	if(k=='X')viewer[0]+=1.0;												
	if(k=='y')viewer[1]-=1.0;												
	if(k=='Y')viewer[1]+=1.0;												
	if(k=='z')viewer[2]-=1.0;												
	if(k=='Z')viewer[2]+=1.0;	
	if(k=='b' || k=='B')
	{
		glutIdleFunc(roll_sphere);
		glutMouseFunc(rb);
		roll=true;
	}
	
}

//click on the right button to make sphere stop or roll
void rb(int btn,int state, int x, int y)
{
	if(roll == false && btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{	
		glutIdleFunc(roll_sphere);
		roll =true;
	}
    else if(roll == true && btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		glutIdleFunc(idle);
		roll =false;
	}
}

//set axes and background
void draw_background()
{
	glBegin(GL_POLYGON); 										
		glColor3f(0,1,0);
		//indicate the x-z plane
		glVertex3f(5,0,8);
		glVertex3f(5,0,-4);
		glVertex3f(-5,0,-4);
		glVertex3f(-5,0,8);
	glEnd();

	glLineWidth(3); 
	glBegin(GL_LINES);												
		glColor3f(1,0,0);
		glVertex3f(0,0,0);
		glVertex3f(10,0,0);

		glColor3f(1,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,10,0);

		glColor3f(0,0,1);
		glVertex3f(0,0,0);
		glVertex3f(0,0,10);
	glEnd();
}

void draw_sphere()
{
	glLineWidth(1);
	glColor3f(1,0.84,0);
	for(int i=0;i<num;i++)
		{
			glBegin(GL_LINE_LOOP);										
			for(int j=0;j<3;j++)
				{	
					glVertex3f(vertices[i*9+j*3+0],vertices[i*9+j*3+1],vertices[i*9+j*3+2]);
				}
			glEnd();
		}
}

//create the menu
void create_menu()
{
	glutCreateMenu(menu);
	//add menu entry
	glutAddMenuEntry("Default View Point", 1);
	glutAddMenuEntry("Quit", 2);
	glutAttachMenu(GLUT_LEFT_BUTTON);

}

//the function of every menu entry
void menu(int m)
{
   switch (m)
   {
   case 1:
			viewer[0]=7.0;
			viewer[1]=3.0;
			viewer[2]=-10.0;	   
	   break;
   case 2:
	   exit(0);
	   delete [] vertices;
	   break;
	
   }

}