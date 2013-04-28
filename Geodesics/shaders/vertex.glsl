varying vec3 position;
varying vec3 normal;

void main()
{	
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   gl_FrontColor = gl_Color;

   position = gl_Vertex.xyz;
   normal = gl_Normal.xyz;
}
