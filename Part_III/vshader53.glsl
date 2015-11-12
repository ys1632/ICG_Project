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
out vec4 color;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct, GlobalAmbient;
uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 Normal_Matrix;
uniform vec4 LightPosition;   // Must be in Eye Frame
uniform float Shininess;
uniform int Source;
uniform int Test;

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
	if(Test==2)
	color = vColor;
	
	
}
