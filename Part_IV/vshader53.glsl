/* 
File Name: "vshader53.glsl":
Vertex shader:
  - Per vertex shading for a single point light source;
    distance attenuation is Yet To Be Completed.
  - Entire shading computation is done in the Eye Frame.
*/

// #version 150 

in  vec4 vPosition;
in  vec3 vNormal;
in  vec4 vColor;
in  vec2 vTexCoord;
in float vTexCoord2;
out vec4 color;
out int fog;
out float z;
out vec4 fog_Color;
out vec2 texCoord;
out float texCoord2;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct, GlobalAmbient;
uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 Normal_Matrix;
uniform vec4 LightPosition;   // Must be in Eye Frame
uniform float Shininess;
uniform int Source;
uniform int Test;
uniform int vFog;
uniform vec4 eye;
uniform vec4 fogColor;

uniform float Time;
uniform int iff;
varying float Y;

uniform float ConstAtt;  // Constant Attenuation
uniform float LinearAtt; // Linear Attenuation
uniform float QuadAtt;   // Quadratic Attenuation

void main()
{

	//vec4 vColor4 = vec4(vColor.r, vColor.g, vColor.b, 1.0); 
	//vec4 vPosition4 = vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);


    // Transform vertex  position into eye coordinates
    vec3 pos = (model_view * vPosition).xyz;
	vec3 L;
	if (Source==2||Source==3)
	L = normalize( LightPosition.xyz - pos );
	if (Source==1)
	L = normalize(-LightPosition.xyz);
    vec3 E = normalize( -pos );
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
      // vec3 N = normalize( model_view * vec4(vNormal, 0.0) ).xyz;
    vec3 N = normalize(Normal_Matrix * vNormal);



 // Compute terms in the illumination equation
    vec4 ambient = AmbientProduct;

    float d = max( dot(L, N), 0.0 );
    vec4  diffuse = d * DiffuseProduct;

    float s = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = s * SpecularProduct;
    
    if( dot(L, N) < 0.0 ) {
	specular = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = projection * model_view * vPosition;


/*--- To Do: Compute attenuation ---*/
float dd = length(LightPosition.xyz - pos);
float attenuation=1;

if (Source==1)
attenuation = 1.0; 

if (Source==2)
attenuation = pow(dot(L, N),5)/(ConstAtt + LinearAtt * dd + QuadAtt *dd*dd);

if (Source==3)
attenuation = 1/(ConstAtt + LinearAtt * dd + QuadAtt *dd*dd);

/*--- attenuation below must be computed properly ---*/
	

	if( Test == 1)
	color = AmbientProduct * GlobalAmbient + attenuation * (ambient + diffuse + specular);
	if (Test == 2)
	color = vColor;
	
	fog =vFog;
	fog_Color=fogColor;
	z = length(vPosition - eye);
	

	texCoord=vTexCoord;
	texCoord2=vTexCoord2;


	if(iff==1){
	float a = 0.001 * Time * vPosition.x;
	float b = 0.1 + 0.001 * Time * vPosition.y + 0.5 * (-0.00000049) * Time * Time;
	float c = 0.001 * Time * vPosition.z;
	Y=b;

	vec4 abc = vec4(a,b,c,1.0); 
	gl_Position = projection * model_view * abc ;

	}
}
