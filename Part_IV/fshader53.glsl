/* 
File Name: "fshader53.glsl":
           Fragment Shader
*/

// #version 150 

in	int fog;
in	float z;
in  vec4 color;
in  vec4 fog_Color;
in  vec2 texCoord;
in  float texCoord2;
out vec4 fColor;

uniform sampler2D texture;
uniform int Texture_app_flag;
uniform int Gmap;
uniform vec4 Gcolor;
uniform vec4 Ycolor;
uniform sampler1D texture2;
uniform int Smap;
uniform int ifsphere;

uniform int iff;
varying float Y;

#define M_E        2.71828182845904523536

void main() 
{ 
	float end = 18.0;
	float start = 0.0;
	float f; 
	float iden = 0.09;

	//no fog
	if (fog==0){
	fColor = color;
		if(Gmap==1&&Texture_app_flag==1)
			//fColor =color;
			fColor =texture2D(texture, texCoord) * Gcolor;  
		if(Smap==1&&ifsphere==1||Smap==2&&ifsphere==1||Smap==3&&ifsphere==1||Smap==4&&ifsphere==1)
			//fColor=Gcolor;
			fColor =texture1D(texture2, texCoord2); 
		
	}


	//linear
	if (fog==1){
	f=(end - z)/(end - start);
	fColor = f*color + (1-f)*fog_Color;

	if(Gmap==1&&Texture_app_flag==1)
		fColor = f * Gcolor * texture2D(texture, texCoord ) + (1-f)*fog_Color;
	if(Smap==1&&ifsphere==1||Smap==2&&ifsphere==1||Smap==3&&ifsphere==1||Smap==4&&ifsphere==1)
			//fColor=Gcolor;
			fColor = f *texture1D(texture2, texCoord2) + (1-f)*fog_Color;

	}

	//exponential
	if (fog==2){
	f =1/pow(M_E, iden * z);
	fColor = f * color + (1-f) * fog_Color;

	if(Gmap==1&&Texture_app_flag==1)
	fColor = f * Gcolor * texture2D(texture, texCoord ) + (1-f)*fog_Color;
	if(Smap==1&&ifsphere==1||Smap==2&&ifsphere==1||Smap==3&&ifsphere==1||Smap==4&&ifsphere==1)
			//fColor=Gcolor;
			fColor = f *texture1D(texture2, texCoord2) + (1-f)*fog_Color;
   }

   //exponential square
	if (fog==3){
	f =1/pow(M_E,iden * z * iden * z);
    fColor = f*color + (1-f)*fog_Color;

	if(Gmap==1&&Texture_app_flag==1)
	fColor = f * Gcolor * texture2D(texture, texCoord ) + (1-f)*fog_Color;
	if(Smap==1&&ifsphere==1||Smap==2&&ifsphere==1||Smap==3&&ifsphere==1||Smap==4&&ifsphere==1)
			//fColor=Gcolor;
			fColor = f *texture1D(texture2, texCoord2) + (1-f)*fog_Color;
	}

	if (iff==1)
	 if ( Y < 0.1 )
		discard;


} 

