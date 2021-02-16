#version 330

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform mat4 MV; 
uniform mat4 normalMatrix;
uniform vec4 cameraPosition;
uniform float heightFactor;
uniform sampler2D depthTexture;
uniform int textureWidth;
uniform int textureHeight;
uniform vec3 lightPosition;


//for fragment shader
out vec2 textureCoordinate; 
out vec3 vertexNormal; 
out vec3 toLightVector;
out vec3 toCameraVector; 


void calculateHeight(vec3 neighbor, inout float heightVal)
{
    vec2 neighborTexCoord;
    neighborTexCoord.x = abs(neighbor.x-textureWidth)/textureWidth;
    neighborTexCoord.y = abs(neighbor.z-textureHeight)/textureHeight;
    vec4 textureDepth = texture(depthTexture, neighborTexCoord);
    vec3 rgb = textureDepth.xyz;
    heightVal = heightFactor*rgb.x;

}





void main()
{

    vec3 pos = position;
    textureCoordinate = vec2(1 - (float(position.x) / (textureWidth+1)), 1 - (float(position.z) / (textureHeight+1)));

    vec4 textureDepth = texture(depthTexture, textureCoordinate);
    vec3 rgb = textureDepth.xyz;

    pos.y = heightFactor*rgb.x;
    vec3 normal;

    vec3 neighbor1 = vec3(position.x , 0 , position.z - 1);
    vec3 neighbor2 = vec3(position.x , 0 , position.z+1);  
    vec3 neighbor3 = vec3(position.x +1 , 0 , position.z + 1); 
    vec3 neighbor4 = vec3(position.x+1 , 0 , position.z );
    vec3 neighbor5 = vec3(position.x , 0 , position.z-1); 
    vec3 neighbor6 = vec3(position.x -1, 0 , position.z); 


    calculateHeight(neighbor1,neighbor1.y);
    calculateHeight(neighbor2,neighbor2.y); 
    calculateHeight(neighbor3,neighbor3.y); 
    calculateHeight(neighbor4,neighbor4.y); 
    calculateHeight(neighbor5,neighbor5.y); 
    calculateHeight(neighbor6,neighbor6.y); 
    

   

    vec3 final1= (neighbor1.x < 0 || neighbor1.x>=textureWidth || neighbor1.z<0 || neighbor1.z >= textureHeight)?vec3(0,0,0): neighbor1-pos;
    vec3 final2= (neighbor2.x < 0 || neighbor2.x>=textureWidth || neighbor2.z<0 || neighbor2.z >= textureHeight)?vec3(0,0,0): neighbor2-pos;
    vec3 final3= (neighbor3.x < 0 || neighbor3.x>=textureWidth || neighbor3.z<0 || neighbor3.z >= textureHeight)?vec3(0,0,0): neighbor3-pos;
    vec3 final4= (neighbor4.x < 0 || neighbor4.x>=textureWidth || neighbor4.z<0 || neighbor4.z >= textureHeight)?vec3(0,0,0): neighbor4-pos;
    vec3 final5= (neighbor5.x < 0 || neighbor5.x>=textureWidth || neighbor5.z<0 || neighbor5.z >= textureHeight)?vec3(0,0,0): neighbor5-pos;
    vec3 final6= (neighbor6.x < 0 || neighbor6.x>=textureWidth || neighbor6.z<0 || neighbor6.z >= textureHeight)?vec3(0,0,0): neighbor6-pos;
    normal= normalize(cross(final1,final2)+ cross(final2,final3)+cross(final3,final4)+cross(final4,final5)+cross(final5,final6)+cross(final6,final1));    

    toLightVector = normalize(vec3(MV*vec4(lightPosition - pos, 0)));
    toCameraVector = normalize(vec3(MV*(vec4(vec3(cameraPosition) - pos, 0))));
    vertexNormal = normalize(vec3(normalMatrix*vec4(normal,0)));

    
    gl_Position = MVP*vec4(pos,1); 
    
}