#version 330


uniform mat4 MVP; 
uniform mat4 MV; 
uniform vec4 cameraPosition;
uniform sampler2D colorTexture;
uniform int textureWidth;
uniform int textureHeight;

// from vertex shader
in vec2 textureCoordinate;
in vec3 vertexNormal; 
in vec3 toLightVector; 
in vec3 toCameraVector;

out vec4 color;

void main() {

  
  vec4 textureColor = texture(colorTexture, textureCoordinate);

  // Phong shading
  vec4 ka = vec4(0.25,0.25,0.25,1.0); 
  vec4 Ia = vec4(0.3,0.3,0.3,1.0); 
  vec4 ks = vec4(1.0, 1.0, 1.0, 1.0); 
  vec4 Is = vec4(1.0, 1.0, 1.0, 1.0); 
  int p = 100;
  vec4 kd = vec4(1.0, 1.0, 1.0, 1.0); 
  vec4 Id = vec4(1.0, 1.0, 1.0, 1.0); 

  float cosT = clamp(dot(vertexNormal, toLightVector),0,1);
  vec3 h = normalize(toLightVector + toCameraVector);
  float cosA = clamp(dot(vertexNormal, h),0,1);


  vec4 ambient = ka * Ia;
  vec4 diffuse = kd * cosT * Id;
  vec4 specular = ks * pow(cosA,p) * Is;

  color = vec4(clamp( textureColor.xyz * vec3(ambient + diffuse + specular), 0.0, 1.0), 1.0);
  

}