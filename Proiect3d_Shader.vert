
 #version 330 core


layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;

//  Variabile de iesire;
out vec3 FragPos;
out vec3 Normal;
out vec3 inLightPos;
out vec3 inViewPos;
out vec4 ex_Color;

//  Variabile uniforme;
uniform int nrVertices;
uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 objectColor;
uniform int codCol; // cod pentru culoare

vec3 viewPos = vec3(3, 2, 1);
//vec3 viewPos = vec3(3.0f, 3.0f, 3.0f);  // Pozi?ia camerei


void main(void)
  {
    gl_Position = projection*view*myMatrix*vec4(in_Position, 1.0);
    FragPos = vec3(gl_Position);
    Normal=vec3(projection*view*vec4(in_Normal,0.0));
    inLightPos= vec3(projection*view*myMatrix* vec4(2.0f, 4.0f, 0.0f, 1.0f));
    //inLightPos = vec3(2.0f, 4.0f, 3.0f); // Seteaz? pozi?ia luminii direct (în coordonate globale)

    inViewPos=vec3(projection*view*vec4(viewPos, 1.0f));
	 //ex_Color=vec4(0.59, 0.29, 0.0, 1.0);
      switch (codCol)
    {
        case 0: ex_Color = vec4(1.0, 1.0, 1.0, 1.0); break; // alb
        case 1: ex_Color = vec4(0.5, 0.35, 0.05, 1.0); break; // maro 
        case 2: ex_Color = vec4(0.2, 0.4, 0.0, 1.0); break; 
        case 3: ex_Color = vec4(1.0, 0.5, 0.0, 1.0);  break; // portocaliu
        case 4: ex_Color = vec4(1.0, 0.0, 0.0, 1.0); break; // rosu
        case 5: ex_Color = vec4(0.0, 0.0, 0.0, 1.0); break; // negru
        case 6: ex_Color = vec4(0.3, 0.2, 0.1, 1.0); break; // maro inchis
        default: ex_Color = vec4(1.0, 1.0, 1.0, 1.0); break; // fallback alb
    }
   } 
 