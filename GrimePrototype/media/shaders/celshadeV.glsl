
// toon tree vertex shader

varying vec4 Color;
varying vec3 Normal;
varying vec3 ViewDirection;
varying vec3 LightDirection;

void main()
{
   gl_Position = ftransform();
   gl_TexCoord[0] = gl_MultiTexCoord0;
   Normal  = gl_NormalMatrix * gl_Normal;
   vec3 Position = gl_ModelViewMatrix * gl_Vertex;
   LightDirection =  gl_NormalMatrix * vec3(0.75,1,-0.75);//directional sunlight
   ViewDirection = -Position;
   Color = gl_Color;         
} 