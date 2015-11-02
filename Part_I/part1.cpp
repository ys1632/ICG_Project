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
#include <utility>
#include <fstream>
using namespace std;

#define XOFF          50
#define YOFF          50
#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600
#define NUM			  6		

int i;
int xd;
int yd;
int rd;
double d;
int arr[NUM][3];//store data from input.txt
int n=1;
int k=150;

void draw_circle(void);
void draw_circle2(void);
void display(void);
void myinit(void);
void transform(void);
void idle(void);
/* Function to handle file input; modification may be needed */
void file_in(void);

/*-----------------
The main function
------------------*/
int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    /* Use both double buffering and Z buffer */
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowPosition(XOFF, YOFF);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("CS6533 Assignment 1");
	cout<<"Select question type: c,d or e?\n";
	
	char option;
	cin>>option;
	switch(option)
	{
	case 'c':
	case 'C':
			cout<<"Please enter the values of X, Y and R.\n";
			 glutDisplayFunc(draw_circle);	//call draw_cirle function
	break;

	case 'd':
	case 'D':
			
			glutDisplayFunc(draw_circle2);
			/* Function call to handle file input here */
			file_in();
			transform();
			break;
	case 'e':
	case 'E':
			glutDisplayFunc(draw_circle2);
			glutIdleFunc(idle);
			break;
    
	}
    myinit();
    glutMainLoop();
}

/*----------
file_in(): file input function. Modify here.
------------*/
void file_in(void)
{
	int m;
	ifstream fin;
	fin.open("input.txt");
	if(fin.is_open())
		fin>>m;
	for(i=0;i<m;i++){
		fin>>xd;
		fin>>yd;
		fin>>rd;
		arr[i][0]=xd;
		arr[i][1]=yd;
		arr[i][2]=rd;
	}
}


/*---------------------------------------------------------------------
display(): This function is called once for _every_ frame. 
---------------------------------------------------------------------*/
void draw_circle(void)
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  int xin, yin, rin;
  cin>>xin>>yin>>rin;
  int y=0;
  int r=rin;
  int x=r;


  glColor3f(1.0, 0.84, 0);              /* draw in golden yellow */
  glPointSize(1.0);                     /* size of each point */

  glBegin(GL_POINTS);
  //x is the most right point of the circle
  //increase the y-value by 1 and choose the best x-value
  while(y<x){
		if(d>0){
			d+=2*y-2*x+3;
			y++;
			x--;
		}
		else{
			d+=2*y+1;
			y++;
		}
		//draw 8 symmetric points in the 8 regions corresponding to (x,y) 
		glVertex2i(x+xin,y+yin);  
		glVertex2i(x+xin,-y+yin);
		glVertex2i(y+xin,x+yin);
		glVertex2i(y+xin,-x+yin);
		glVertex2i(-x+xin,y+yin);
		glVertex2i(-x+xin,-y+yin);
		glVertex2i(-y+xin,x+yin);
		glVertex2i(-y+xin,-x+yin);
	}
              
  glEnd();

  glFlush();                            /* render graphics */

  glutSwapBuffers();                    /* swap buffers */
}

//draw circle defined by input file
void draw_circle2(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(i=0; i < NUM; i++)
	{
		int y=0;
		int x=arr[i][2];                      
		int r=arr[i][2];


  glColor3f(1.0, 0.84, 0);              /* draw in golden yellow */
  glPointSize(1.0);                     /* size of each point */

  glBegin(GL_POINTS);
  //x is the most right point of the circle
  //increase the y-value by 1 and choose the best x-value
  while(y<x){
		if(d>0){
			d+=2*y-2*x+3;
			y++;
			x--;
		}
		else{
			d+=2*y+1;
			y++;
		}
		//draw 8 symmetric points in the 8 regions corresponding to (x,y) 
			glVertex2i(x+arr[i][0],y+arr[i][1]);
			glVertex2i(x+arr[i][0],-y+arr[i][1]);
			glVertex2i(-x+arr[i][0],y+arr[i][1]);
			glVertex2i(-x+arr[i][0],-y+arr[i][1]);
			glVertex2i(y+arr[i][0],x+arr[i][1]);
			glVertex2i(-y+arr[i][0],x+arr[i][1]);
			glVertex2i(y+arr[i][0],-x+arr[i][1]);
			glVertex2i(-y+arr[i][0],-x+arr[i][1]);
	}
              
  glEnd();
  }
  glFlush();                            /* render graphics */

  glutSwapBuffers();                    /* swap buffers */

}

/*---------------------------------------------------------------------
myinit(): Set up attributes and viewing
---------------------------------------------------------------------*/
void myinit()
{
  glClearColor(0.0, 0.0, 0.92, 0.0);    /* blue background*/

  /* set up viewing */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-600.0, WINDOW_WIDTH, -600.0, WINDOW_HEIGHT);
  glMatrixMode(GL_MODELVIEW);
}

//make the scale of the x- and y-axes automatically adjust
void transform(void)
{
	int max_x=0;
	int max_y=0;
	int max_coord=0;
	for(i = 0; i < NUM; i++){
		max_x=max(max_x,arr[i][0]+arr[i][2]);
		max_x=max(max_x,abs(arr[i][0]-arr[i][2]));

		max_y=max(max_y,arr[i][1]+arr[i][2]);
		max_y=max(max_y,abs(arr[i][1]-arr[i][2]));

		max_coord=max(max_coord,max_x);
		max_coord=max(max_coord,max_y);
	}
	for(i = 0; i < NUM; i++){
		arr[i][0]=arr[i][0]*600/max_coord;
		arr[i][1]=arr[i][1]*600/max_coord;
		arr[i][2]=arr[i][2]*600/max_coord;
	}


}

//animation
void idle(void)
{
	n++;                         
	if(n > k){
		n = 1;
	}

	//read data
	file_in();
	transform();
	//update data
	for(int j = 0; j < NUM; j++){
		arr[j][2] = (arr[j][2] * n)/k;
	}

    draw_circle2();
	Sleep(30);

}