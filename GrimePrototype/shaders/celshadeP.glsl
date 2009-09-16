// toon tree pixel shader

varying vec3 Normal;
varying vec3 LightDirection;
varying vec3 ViewDirection;

varying vec4 Color;

void main()
{
   vec3 N = normalize(Normal);
   vec3 L = normalize(LightDirection);
   vec3 E = normalize(ViewDirection);
   vec3 R = reflect(-L, N);
   
   float light_angle = max(dot(R,E),0.0);
   if (light_angle > 0.01) Color *= 1.5;//diffuse
   if (light_angle > 0.9) Color=vec4(1,1,1,1);//specular
   
   float ink_angle = max(dot(N,E),0.0);
   if (ink_angle <0.25) Color=vec4(0,0,0,0);//inking
   
   gl_FragColor = Color;
} 