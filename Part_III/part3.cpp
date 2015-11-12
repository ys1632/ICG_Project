//CS 6533
//Yixiao Shen

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
typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLuint Angel::InitShader(const char* vShaderFile, const char* fShaderFile);

GLuint program;       /* shader program object id */
GLuint floor_buffer;  /* vertex buffer object id for floor */
GLuint coordinate_buffer;/* vertex buffer object id for coordinate */
GLuint sphere_buffer; /* vertex buffer object id for sphere */
GLuint shadow_buffer; /* vertex buffer object id for shadow */

// Projection transformation parameters
GLfloat  fovy = 45.0;  // Field-of-view in Y direction angle (in degrees)
GLfloat  aspect;       // Viewport aspect ratio
GLfloat  zNear = 0.5, zFar = 30.0;

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
int sphereFlag = 1;
const int m = 1024*3;
bool roll = false; //press "B" or "b", the sphere starts to roll
bool stop = false; //right click button to change its value to make sphere stop and continue rolling
int path=0;
bool smooth_shading=false;
bool enable_lighting=false;
bool shadow_on=true;

//shadow translation matrix
mat4 N(	12,0,0,0,
		14,0,3,-1,
		0,0,12,0,
		0,0,0,12);

mat4 MV3, MV4;

const int floor_NumVertices = 4; //x-z plane vertices
point4 floor_points[floor_NumVertices]; // positions for all vertices
color4 floor_colors[floor_NumVertices]; // colors for all vertices
vec3 floor_normals[floor_NumVertices];

//positions and colors for coordinate vertices
const int coordinate_NumVertices = 6;
point4 coordinate_points[coordinate_NumVertices];
color4 coordinate_colors[coordinate_NumVertices];
vec3 coordinate_normals[coordinate_NumVertices];

GLfloat * vertices; 
//positions and colors for sphere vertices
const int sphere_NumVertices = m;
point4 sphere_points[sphere_NumVertices];
color4 sphere_colors[sphere_NumVertices];
vec3 sphere_normals[sphere_NumVertices];

const int shadow_NumVertices = m;
point4 shadow_points[shadow_NumVertices];
color4 shadow_colors[shadow_NumVertices];

GLuint  model_view;  // model-view matrix uniform shader variable location
GLuint  projection;  // projection matrix uniform shader variable location

color4 global_ambient(1.0, 1.0, 1.0, 1.0);
color4 light_ambient( 0.0, 0.0, 0.0, 1.0 );
color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
color4 light_specular( 1.0, 1.0, 1.0, 1.0 );
vec4 d_light(0.0,0.0,1.0,0.0);

    float const_att = 2.0;
    float linear_att = 0.01;
    float quad_att = 0.001;
	

	color4 ground_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 ground_diffuse( 0.0, 1.0, 0.0, 1.0 );
    color4 ground_specular( 0.0, 0.0, 0.0, 1.0 );

	color4 sphere_ambient( 0.2, 0.2, 0.2, 1.0 );
    color4 sphere_diffuse( 1.0, 0.84, 0.0, 1.0 );
    color4 sphere_specular( 1.0, 0.84, 0.0, 1.0 );

    float  material_shininess = 125.0;

    color4 ambient_product = ground_ambient * light_ambient;
    color4 diffuse_product = ground_diffuse * light_diffuse;
    color4 specular_product = ground_specular * light_specular;

	color4 ambient_product2 = sphere_ambient * light_ambient;
    color4 diffuse_product2 = sphere_diffuse * light_diffuse;
    color4 specular_product2 = sphere_specular * light_specular;


void SetUp_Lighting_Uniform_Vars();



//File read function
void open_file(void)
{
int n,count=0;
fstream sphere_file;
char fileName[20];
bool fail_to_open=true;
		do
		{
			cout<<"Please choose the input file -- sphere.256 or sphere.1024?"<<endl<<""<<endl;
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
point4 cr[4] = {
	point4(  0,    0,    0, 1.0),
    point4(  10.0, 0,    0, 1.0),
    point4(  0,    10.0, 0, 1.0),
    point4(  0,    0,    10.0, 1.0)
};

//positions for x-z plane
point4 fl[4] = {
	point4(  5.0, 0,  8.0, 1.0),
    point4(  5.0, 0, -4.0, 1.0),
    point4( -5.0, 0, -4.0, 1.0),
    point4( -5.0, 0,  8.0, 1.0)
};

// RGBA colors
color4 vertex_colors[8] = {
    color4( 0.0, 0.0, 0.0, 1.0),  // black
    color4( 1.0, 0.0, 0.0, 1.0),  // red
    color4( 1.0, 1.0, 0.0, 1.0),  // yellow
    color4( 0.0, 1.0, 0.0, 1.0),  // green
    color4( 0.0, 0.0, 1.0, 1.0),  // blue
    color4( 1.0, 0.0, 1.0, 1.0),  // magenta
    color4( 1.0, 1.0, 1.0, 1.0),  // white
    color4( 0.0, 1.0, 1.0, 1.0)   // cyan
};

//properties of x-z plane
void floor()
{
    floor_colors[0] = vertex_colors[3]; floor_points[0] = fl[0];
    floor_colors[1] = vertex_colors[3]; floor_points[1] = fl[1];
    floor_colors[2] = vertex_colors[3]; floor_points[2] = fl[2];
    floor_colors[3] = vertex_colors[3]; floor_points[3] = fl[3];

	vec4 u = fl[2] - fl[0];
	vec4 v = fl[1] - fl[0];

	vec3 normal = normalize(cross(u,v));

	floor_normals[0] = normal; floor_points[0] = fl[0];
    floor_normals[1] = normal; floor_points[1] = fl[1];
    floor_normals[2] = normal; floor_points[2] = fl[2];
    floor_normals[3] = normal; floor_points[3] = fl[3];
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
		vec4 temp(vertices[3*i],vertices[3*i+1],vertices[3*i+2], 1.0);
		sphere_points[i]= temp;
	}

	if (smooth_shading==false){
		for (int i=0; i<num; i++){
		
		vec4 u = sphere_points[3*i+2]-sphere_points[3*i];
		vec4 v = sphere_points[3*i+1]-sphere_points[3*i];
		
		vec3 normal = normalize(cross(u,v));

		sphere_normals[3*i] = normal;
		sphere_normals[3*i+1] = normal;
		sphere_normals[3*i+2] = normal;
		}
	}
	if (smooth_shading==true){
		for (int i=0; i<num*3; i++){
			vec4 n(0,0,0,1);
			vec4 normal = normalize(n-sphere_points[i]);
			vec3 normal2 = vec3(normal.x,normal.y,normal.z);
			sphere_normals[i] = normal2;
			
		}
	}
};

//set properties of shadow
void shadow()
{
	for (int i=0; i<num*3; i++)
		shadow_colors[i]=(0.25,0.25,0.25,0.65);

for (int i=0; i<num*3; i++){
		vec4 temp(vertices[3*i],vertices[3*i+1],vertices[3*i+2],1.0);
		shadow_points[i]= temp;
}
	
	
	//for (int i=0; i<12; i++)  //check whether the data has been stored in array 
		
};


void sphereinit(){
	sphere();
	if(enable_lighting){
	glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_points) + sizeof(sphere_normals),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sphere_points), sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(sphere_points), sizeof(sphere_normals),
		sphere_normals);}
	if(enable_lighting==false){
	glGenBuffers(1, &sphere_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_points) + sizeof(sphere_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(sphere_points), sphere_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(sphere_points), sizeof(sphere_colors),
		sphere_colors);}



}

// OpenGL initialization
void init()
{
	//shadow
	shadow();
	glGenBuffers(1, &shadow_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shadow_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(shadow_points) + sizeof(shadow_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shadow_points), shadow_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(shadow_points), sizeof(shadow_colors),
                    shadow_colors);

	//coordinate
	coordinate();
	glGenBuffers(1, &coordinate_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, coordinate_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coordinate_points) + sizeof(coordinate_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(coordinate_points), coordinate_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(coordinate_points), sizeof(coordinate_colors),
                    coordinate_colors);


	floor();     
 // Create and initialize a vertex buffer object for floor, to be used in display()
	if(enable_lighting==false){
	glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_colors),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_colors),
                    floor_colors);
	}

	if(enable_lighting){
	glGenBuffers(1, &floor_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, floor_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor_points) + sizeof(floor_normals),
		 NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(floor_points), floor_points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(floor_points), sizeof(floor_normals),
                    floor_normals);
	}


	//sphere
	sphereinit();
	

 // Load shaders and create a shader program (to be used in display())
    program = InitShader("vshader53.glsl", "fshader53.glsl");
    glEnable( GL_DEPTH_TEST );
    glClearColor( 0.529, 0.807, 0.92, 0.0 ); 
    //glLineWidth(1.5);
}

vec4 light_position(-14.0,12.0,-3.0,1.0);

int source=1; // 1 stands for directional light; 2 for spot light and 3 for point source
int type = 0; // 0 for gound, 1 for shpere
int test =1; //1 stands for using vNormal in vshader file, 2 for using vColor in vshader file.
void SetUp_Lighting_Uniform_Vars(mat4 mv2)
{
   glUniform4fv( glGetUniformLocation(program, "GlobalAmbient"),
		  1, global_ambient );

   if (type==0){
	glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product);
	}
   else if  (type==1){
	   glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		  1, ambient_product2 );
    glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		  1, diffuse_product2 );
    glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		  1, specular_product2 );
	}
 
   // The Light Position in Eye Frame
	vec4 light_position_eyeFrame = mv2 * light_position;

	if (source==1)
    glUniform4fv( glGetUniformLocation(program, "LightPosition"),
   		  1, d_light);
	if (source==2||source==3)
	glUniform4fv( glGetUniformLocation(program, "LightPosition"),
   		  1, light_position_eyeFrame);
    glUniform1f(glGetUniformLocation(program, "ConstAtt"),
		        const_att);
    glUniform1f(glGetUniformLocation(program, "LinearAtt"),
		        linear_att);
    glUniform1f(glGetUniformLocation(program, "QuadAtt"),
		        quad_att);

    glUniform1f(glGetUniformLocation(program, "Shininess"),
		        material_shininess );
	glUniform1i(glGetUniformLocation(program, "Source"),
		        source );
	glUniform1i(glGetUniformLocation(program, "Test"),
		        test );
	
	
}


// drawObj(buffer, num_vertices):
// draw the object that is associated with the vertex buffer object "buffer" and has "num_vertices" vertices.

void drawObj(GLuint buffer, int num_vertices)
{
    //--- Activate the vertex buffer object to be drawn ---//
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    /*----- Set up vertex attribute arrays for each vertex attribute -----*/
    // the offset is the (total) size of the previous vertex attribute array(s)
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(0) );

    GLuint vNormal = glGetAttribLocation(program, "vNormal"); 
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(sphere_points)) ); 

	GLuint vColor = glGetAttribLocation(program, "vColor"); 

    glEnableVertexAttribArray(vColor);
    glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
			  BUFFER_OFFSET(sizeof(point4) * num_vertices) ); 


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
    glDisableVertexAttribArray(vNormal);
	glDisableVertexAttribArray(vColor);
}


//make ratation smooth
mat4 enhance_roll(mat4 mv2, vec3 trans,int x, int z)
{
	float s=0.5;
	mat4 ma= mv2 * Translate(trans) * Rotate(angle-s, x, 0, z);
	angle=s;
	return ma;
}

//sphere rolling matrix
void rolling(mat4 mv2)
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
	mat4 mv4;
	//rolling between A and B
	if (path==0)
	{
		mv3= mv2 * Translate(trans1) * Rotate(angle, -8, 0, -7);
		mv4= mv2 * N * Translate(trans1) * Rotate(angle, -8, 0, -7);
		if (d>length(AB)){
		mv3= enhance_roll(mv2,trans1,-8,-7);
		//	angle=0;
		path++;
	
		}
	}//rolling between B and C
	else if (path==1){
		mv3= mv2  * Translate(trans2) * Rotate(angle, 1, 0, 6);
		mv4= mv2  * N * Translate(trans2) * Rotate(angle, 1, 0, 6);
	if (d>length(BC)){
		mv3= enhance_roll(mv2,trans2,1,6);
		//angle=0;
		path++;
	
		}}//rolling between C and A
	else if (path==2){
		mv3= mv2 * Translate(trans3) * Rotate(angle, 7, 0, 1);
		mv4= mv2 * N * Translate(trans3) * Rotate(angle, 7, 0, 1);
	if (d>length(CA)){
		mv3= enhance_roll(mv2,trans3,7,1);
		//angle=0;
		path=0;
		}
	
	}
	else 
		cout<<"No such path exists! Please check!"<<endl;
	MV3=mv3;
	MV4=mv4;
	

}
  
//----------------------------------------------------------------------------
void display( void )
{
	//glEnable(GL_DEPTH_TEST);


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
	
	rolling(mv2);

	//the initial state of the sphere
	//it changes right after pressing "B" or "b"
	if (roll==false)
	{
	MV3= mv2 * Translate(-4,1,4);
	angle=0;
	//path=0;
	}


	//Axises
	test=2;
	glUniform1i(glGetUniformLocation(program, "Test"), test );
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2); // GL_TRUE: matrix is row-major
	
    if (coordinateFlag == 1) 
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else            
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 1;
	glLineWidth(2);
	drawObj(coordinate_buffer, coordinate_NumVertices);  // draw the coordinate

	//sphere
	if(enable_lighting){
	test=1;
	type=1;
	SetUp_Lighting_Uniform_Vars(mv2);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, MV3); // GL_TRUE: matrix is row-major
	mat3 normal_matrix2 = NormalMatrix(MV3, 1);
    if (sphereFlag == 1) 
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else            
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 2;
	glLineWidth(1);
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 
		       1, GL_TRUE, normal_matrix2 );
	}
	if(enable_lighting==false){
	test=2;
	glUniform1i(glGetUniformLocation(program, "Test"), test );
	glUniformMatrix4fv(model_view, 1, GL_TRUE, MV3); // GL_TRUE: matrix is row-major
    if (sphereFlag == 1) 
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else            
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 2;
	glLineWidth(1);
	}
	drawObj(sphere_buffer, sphere_NumVertices);  // draw the sphere

	//make Decal
	glDepthMask(GL_FALSE);//disable Z-buffer
	//Ground
	if(enable_lighting){
	test=1;
	type=0;
	SetUp_Lighting_Uniform_Vars(mv2);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2);
	mat3 normal_matrix = NormalMatrix(mv2, 1);
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 0;
	glLineWidth(2);
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 
		       1, GL_TRUE, normal_matrix );
	}
	if(enable_lighting==false){
	test=2;
	glUniform1i(glGetUniformLocation(program, "Test"), test );
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2);
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 0;
	glLineWidth(2);
	}

	drawObj(floor_buffer, floor_NumVertices);  // draw the floor


	glDepthMask(GL_TRUE);//enable Z-buffer
	//shadow
	if (shadow_on){
	glUniformMatrix4fv(model_view, 1, GL_TRUE, MV4); // GL_TRUE: matrix is row-major
    if (sphereFlag == 1) 
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else            
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 2;
	glLineWidth(1);
	drawObj(shadow_buffer, shadow_NumVertices);  // draw the shadow
	}

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE); //disable frame buffer

	//Ground
	if(enable_lighting){
	test=1;
	type=0;
	SetUp_Lighting_Uniform_Vars(mv2);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2);
	mat3 normal_matrix = NormalMatrix(mv2, 1);
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 0;
	glLineWidth(2);
	glUniformMatrix3fv(glGetUniformLocation(program, "Normal_Matrix"), 
		       1, GL_TRUE, normal_matrix );
	}
	if(enable_lighting==false){
	test=2;
	glUniform1i(glGetUniformLocation(program, "Test"), test );
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv2);
    if (floorFlag == 1) // Filled floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else              // Wireframe floor
       glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shape = 0;
	glLineWidth(2);
	}
	drawObj(floor_buffer, floor_NumVertices);  // draw the floor
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //enable frame buffer
	//glFlush();
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
		   sphereFlag=0;
		   enable_lighting=false;
		   sphereinit();
		   break;

   case 4:
	   exit(0);
	   delete [] vertices;
	   break;
	 
   }

}


void ShadowMenu(int id)
{
switch(id)
	{
	case 1:
		shadow_on=true;
		break;
	case 2:
		shadow_on=false;
		break;
	}

	glutPostRedisplay();

}


void ShadingMenu(int id)
{
switch(id)
	{
	case 1:
		sphereFlag=1;
		smooth_shading=false;
	sphereinit();	
		break;
	case 2:
		sphereFlag=1;
		smooth_shading=true;
		sphereinit();
		break;
	}

	glutPostRedisplay();}

void enLightingMenu(int id)
{
switch(id)
	{
	case 1:
		sphereFlag=1;
		source=1;
		enable_lighting=true;
		sphereinit();
		break;
	case 2:
		enable_lighting=false;
		sphereinit();
		break;
	}

	glutPostRedisplay();

}

void LightingMenu(int id)
{
switch(id)
	{
	case 1:
		source=2;
		enable_lighting=true;
		sphereinit();
		break;
	case 2:
		source=3;
		enable_lighting=true;
		sphereinit();
		break;
	}

	glutPostRedisplay();

}
//create the menu
void create_menu()
{
	//Shadow submenu
	int submenu_shadow = glutCreateMenu(ShadowMenu);
	glutAddMenuEntry("Yes", 1);
	glutAddMenuEntry("No", 2);

	int submenu_Shading = glutCreateMenu(ShadingMenu);
	glutAddMenuEntry("flat shading", 1);
	glutAddMenuEntry("smooth shading", 2);

	int submenu_enLighting = glutCreateMenu(enLightingMenu);
	glutAddMenuEntry("Yes", 1);
	glutAddMenuEntry("No", 2);
	
	int submenu_Lighting = glutCreateMenu(LightingMenu);
	glutAddMenuEntry("spot light", 1);
	glutAddMenuEntry("point source", 2);

	glutCreateMenu(menu);
	//add menu entry
	glutAddMenuEntry("Default View Point", 1);
	glutAddSubMenu("Shadow", submenu_shadow);
	glutAddSubMenu("Enable Lighting", submenu_enLighting);
	glutAddSubMenu("Shading", submenu_Shading);
	glutAddSubMenu("Lighting", submenu_Lighting);

	glutAddMenuEntry("Wire Frame Sphere", 2);
	glutAddMenuEntry("Quit", 4);
	glutAttachMenu(GLUT_LEFT_BUTTON);

}

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{ 
	int err;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(XOFF, YOFF);
    glutInitWindowSize(512, 512);
	glutCreateWindow("CS6533 Assignment 3 (Yixiao Shen)");
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