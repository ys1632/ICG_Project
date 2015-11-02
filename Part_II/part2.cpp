#include "Angel-yjc.h"
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

typedef Angel::vec3  color3;
typedef Angel::vec3  point3;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint coordinate_buffer;/* vertex buffer object id for coordinate */
GLuint sphere_buffer; /* vertex buffer object id for sphere */

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 1.0, zFar = 50.0;

GLfloat angle = 0.0; // rotation angle in degrees
vec4 init_eye(7.0, 3.0, -10.0, 1.0); //VRP
vec4 eye = init_eye;               // current viewer position

#define XOFF          400
#define YOFF          50

int num;
//0 stands for polygons used to draw floor
//1 stands for lines used to draw coordinate
//2 stands for triangles used to draw sphere
int shape; //different shapes to draw the objects
int floorFlag = 1;  // 1: solid floor; 0: wireframe floor. Toggled by key 'f' or 'F'
int coordinateFlag = 0 ;
int sphereFlag = 0;
const int m = 128*3;
bool roll = false; //press "B" or "b", the sphere starts to roll
bool stop = false; //right click button to change its value to make sphere stop and continue rolling
int path=0;

const int floor_NumVertices = 4; //x-z plane vertices
point3 floor_points[floor_NumVertices]; // positions for all vertices
color3 floor_colors[floor_NumVertices]; // colors for all vertices

//positions and colors for coordinate vertices
const int coordinate_NumVertices = 6;
point3 coordinate_points[coordinate_NumVertices];
color3 coordinate_colors[coordinate_NumVertices];

GLfloat * vertices; 
//positions and colors for sphere vertices
const int sphere_NumVertices = m;
point3 sphere_points[sphere_NumVertices];
color3 sphere_colors[sphere_NumVertices];

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

//positions for axes
point3 cr[4] = {
	point3(  0,    0,    0),
    point3(  10.0, 0,    0),
    point3(  0,    10.0, 0),
    point3(  0,    0,    10.0)
};

//positions for x-z plane
point3 fl[4] = {
	point3(  5.0, 0,  8.0),
    point3(  5.0, 0, -4.0),
    point3( -5.0, 0, -4.0),
    point3( -5.0, 0,  8.0)
};

// RGBA colors
color3 vertex_colors[8] = {
    color3( 0.0, 0.0, 0.0),  // black
    color3( 1.0, 0.0, 0.0),  // red
    color3( 1.0, 1.0, 0.0),  // yellow
    color3( 0.0, 1.0, 0.0),  // green
    color3( 0.0, 0.0, 1.0),  // blue
    color3( 1.0, 0.0, 1.0),  // magenta
    color3( 1.0, 1.0, 1.0),  // white
    color3( 0.0, 1.0, 1.0)   // cyan
};

//properties of x-z plane
void floor()
{
    floor_colors[0] = vertex_colors[3]; floor_points[0] = fl[0];
    floor_colors[1] = vertex_colors[3]; floor_points[1] = fl[1];
    floor_colors[2] = vertex_colors[3]; floor_points[2] = fl[2];
    floor_colors[3] = vertex_colors[3]; floor_points[3] = fl[3];
}

//set properties of axes
void coordinate()
{
	coordinate_colors[0] = vertex_colors[1]; coordinate_points[0] = cr[0];
	coordinate_colors[1] = vertex_colors[1]; coordinate_points[1] = cr[1];
	coordinate_colors[2] = vertex_colors[5]; coordinate_points[2] = cr[0];
	coordinate_colors[3] = vertex_colors[5]; coordinate_points[3] = cr[2];
	coordinate_colors[4] = vertex_colors[4]; coordinate_points[4] = cr[0];
	coordinate_colors[5] = vertex_colors[4]; coordinate_points[5] = cr[3];

};

//set properties of sphere
void sphere()
{
	for (int i=0; i<num*3; i++)
		sphere_colors[i]=vertex_colors[2];

	for (int i=0; i<num*3; i++){
		vec3 temp(vertices[3*i],vertices[3*i+1],vertices[3*i+2]);
		sphere_points[i]= temp;
	}

	//for (int i=0; i<12; i++)  //check whether the data has been stored in array 
		//cout<<sphere_points[0]<<endl;

};

// OpenGL initialization
void init()
{
    floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
    glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);
	//coordinate
	coordinate();
	glGenBuffers(1, &coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, coordinate_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinate_points) + sizeof(coordinate_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(coordinate_points), coordinate_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(coordinate_points), sizeof(coordinate_colors),
                    coordinate_colors);
	//sphere
	sphere();
	glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_points) + sizeof(sphere_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sphere_points), sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(sphere_points), sizeof(sphere_colors),
                    sphere_colors);

 // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader42.glsl", "fshader42.glsl");
    
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.529, 0.807, 0.92, 0.0 ); 
    //glLineWidth(1.5);
}

// drawObj(buffer, num_vertices):
// draw the object that is associated with the vertex buffer object "buffer" and has "num_vertices" vertices.
void drawObj(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );

    GLuint vColor = glGetAttribLocation(program, "vColor"); 
    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(point3) * num_vertices) ); 
    // the offset is the (total) size of the previous vertex attribute array(s)

    /* Decide what kind of shape to draw
       (using the attributes specified in each enabled vertex attribute array) */
   if (shape == 0)
		glDrawArrays(GL_POLYGON, 0, num_vertices);
   else if (shape == 1)
		glDrawArrays(GL_LINES, 0, num_vertices);
   else if (shape == 2)
		glDrawArrays(GL_TRIANGLES, 0, num_vertices);
   else
		cout<<"Warning! No shape found!"<<endl;
    /*--- Disable each vertex attribute array being enabled ---*/
    glDisableVertexAttribArray(vPosition);
    glDisableVertexAttribArray(vColor);
}

//sphere rolling matrix
mat4 rolling(mat4 mv2)
{
float d;
	
	d = angle * M_PI/180;
	vec3 OA(-4,1,4); 
	vec3 OB(3,1,-4); 
	vec3 OC(-3,1,-3); 

	vec3 AB(7, 0 ,-8);
	vec3 BC(-6, 0 ,1);
	vec3 CA(-1, 0 ,7);

	vec3 nAB(normalize(AB));
	vec3 nBC(normalize(BC));
	vec3 nCA(normalize(CA));

	vec3 trans1(OA + d * nAB);
	vec3 trans2(OB + d * nBC);
	vec3 trans3(OC + d * nCA);

	mat4 mv3;
	//rolling between A and B
	if (path==0)
	{
		mv3= mv2 * Translate(trans1) * Rotate(angle, -8, 0, -7);
		if (d>length(AB)){
		angle=0;
		path++;
	
		}
	}//rolling between B and C
	else if (path==1){
		mv3= mv2 * Translate(trans2) * Rotate(angle, 1, 0, 6);
	if (d>length(BC)){
		angle=0;
		path++;
	
		}}//rolling between C and A
	else if (path==2){
		mv3= mv2 * Translate(trans3) * Rotate(angle, 7, 0, 1);
	
	if (d>length(CA)){
		angle=0;
		path=0;
		}
	
	}
	else 
		cout<<"No such path exists! Please check!"<<endl;

	return mv3;

}

//----------------------------------------------------------------------------
void display( void )
{
  GLuint  model_view;  // model-view matrix uniform shader variable location
  GLuint  projection;  // projection matrix uniform shader variable location

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glUseProgram(program); // Use the shader program

    model_view = glGetUniformLocation(program, "model_view" );
    projection = glGetUniformLocation(program, "projection" );

    /*---  Set up and pass on Projection matrix to the shader ---*/
    mat4  p = Perspective(fovy, aspect, zNear, zFar);
    glUniformMatrix4fv(projection, 1, GL_TRUE, p); // GL_TRUE: matrix is row-major

    /*---  Set up and pass on Model-View matrix to the shader ---*/
    // eye is a global variable of vec4 set to init_eye and updated by keyboard()
    vec4    at(0.0, 0.0, 0.0, 1.0);
    vec4    up(0.0, 1.0, 0.0, 0.0);

    mat4  mv = LookAt(eye, at, up);

	mat4 mv2 = Translate(-1,-1,0) * mv; //set the origin to the center of the window

	mat4 mv3;

	mv3 = rolling(mv2);

    glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2); // GL_TRUE: matrix is row-major
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 0;
	glLineWidth(2);
	drawObj(floor_buffer, floor_NumVertices);  // draw the floor

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2); // GL_TRUE: matrix is row-major
    if (coordinateFlag == 1) 
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else            
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 1;
	drawObj(coordinate_buffer, coordinate_NumVertices);  // draw the coordinate

	//the initial state of the sphere
	//it changes right after pressing "B" or "b"
	if (roll==false)
	{
	mv3= mv2 * Translate(-4,1,4);
	angle=0;
	//path=0;
	}

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv3); // GL_TRUE: matrix is row-major
    if (sphereFlag == 1) 
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else            
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 2;
	glLineWidth(1);
	drawObj(sphere_buffer, sphere_NumVertices);  // draw the sphere

    glutSwapBuffers();
}

//---------------------------------------------------------------------------
void idle (void)
{
  if (stop==false) 
  angle += 0.05; //change this value to adjust the cube rotation speed.
  
  else if (stop == true) //right click button to make the sphere stop (angle will remain same instead of incresing)
  angle = angle;  
  glutPostRedisplay();
}

//----------------------------------------------------------------------------
void reshape(int width, int height)
{
    glViewport(0.0, 0.0, width, height);
    aspect = (GLfloat) width  / (GLfloat) height;
    glutPostRedisplay();
}

void rb(int btn, int state, int x, int y)
{
	if (stop == false && btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		
	stop = true;
	
	}
   else if (stop == true && btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
	stop = false;
	
	}
	
}

//use keyboard to change the viewer position
//uppercase letter to increase and lowercase letter to decrease
void keyboard(unsigned char k,int x,int y)
{
	if(k=='x')eye[0]-=1.0;												
	if(k=='X')eye[0]+=1.0;												
	if(k=='y')eye[1]-=1.0;												
	if(k=='Y')eye[1]+=1.0;												
	if(k=='z')eye[2]-=1.0;												
	if(k=='Z')eye[2]+=1.0;	
	if(k=='b' || k=='B')
	{
		//glutIdleFunc(roll_sphere);
		glutMouseFunc(rb);
		roll=true;
	}
	
}

//the function of every menu entry
void menu(int m)
{
   switch (m)
   {
   case 1: 
	   eye = init_eye;
	   break;
   case 2:
	   exit(0);
	   delete [] vertices;
	   break;
	
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

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{ int err;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(XOFF, YOFF);
    glutInitWindowSize(512, 512);
	glutCreateWindow("CS6533 Assignment 2 (Yixiao Shen)");
open_file();
  /* Call glewInit() and error checking */
  err = glewInit();
  if (GLEW_OK != err)
  { printf("Error: glewInit failed: %s\n", (char*) glewGetErrorString(err)); 
    exit(1);
  }
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	create_menu();
	
    init();
    glutMainLoop();
	
    return 0;
}