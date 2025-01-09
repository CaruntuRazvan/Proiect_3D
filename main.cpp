
// Biblioteci
#include <windows.h>  // biblioteci care urmeaza sa fie incluse
#include <stdio.h>
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <cstdlib> 
#include <vector>
#include <math.h>
#include <iostream>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h

#include "loadShaders.h"
#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "objloader.hpp"  


//  Identificatorii obiectelor de tip OpenGL;
GLuint
VaoIdSol,
VboIdSol,
VaoIdCasa,
VboIdCasa,
VaoIdSnowman,
VboIdSnowman,
VaoIdSnowmanNas,
VboIdSnowmanNas,
VaoIdSnowmanAux,
VboIdSnowmanAux,
VaoIdSnowmanAccesorii,
VboIdSnowmanAccesorii,
VaoIdBrad,
VboIdBrad,
VaoIdUsa,
VboIdUsa,
VaoIdWoods,
VboIdWoods,
ProgramId,
nrVertLocation,
myMatrixLocation,
viewLocation,
projLocation,
colorLocation;


// Valoarea lui pi
float PI = 3.141592;
// Variabila pentru numarul de varfuri
int nrVertices;

std::vector<glm::vec2> uvs;

std::vector<glm::vec3> verticesSol, normalsSol;
std::vector<glm::vec3> verticesCasa, normalsCasa;
std::vector<glm::vec3> verticesSnowman, normalsSnowman;
std::vector<glm::vec3> verticesSnowmanNas, normalsSnowmanNas;
std::vector<glm::vec3> verticesSnowmanAux, normalsSnowmanAux;
std::vector<glm::vec3> verticesSnowmanAccesorii, normalsSnowmanAccesorii;
std::vector<glm::vec3> verticesBrad, normalsBrad;
std::vector<glm::vec3> verticesUsa, normalsUsa;
std::vector<glm::vec3> verticesWoods, normalsWoods;

// Matrice utilizate
glm::mat4 myMatrix;
glm::mat4 view;
glm::mat4 projection;

//	Elemente pentru matricea de vizualizare;
float refX = 0.0f, refY = 0.0f, refZ = 0.0f,
obsX, obsY, obsZ,
vX = 0.0f, vY = 5.0f, vZ = 0.0f;
//	Elemente pentru deplasarea pe sfera;
float alpha = 0.0f, beta = 0.0f, dist = 10.0f,
incrAlpha1 = 0.02, incrAlpha2 = 0.02;
//	Elemente pentru matricea de proiectie;
float width = 800, height = 800, dNear = 0.1f, fov = 60.f * PI / 180;
int codCol;
bool isSnowfallActive = true; // Flag pentru activarea mi?c?rii

float  min_y = 0.204959;
// Structura pentru fulgii de nea
struct Snowflake {
	glm::vec3 position; // Pozi?ia fulgului
	float speed;        // Viteza de c?dere
};

std::vector<Snowflake> snowflakes;


void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key) {
	case '+':
		dist -= 0.25;	//	apasarea tastelor `+` si `-` schimba pozitia observatorului (se departeaza / aproprie);
		break;
	case '-':
		dist += 0.25;
		break;
	case 's':
		isSnowfallActive = !isSnowfallActive; 
		break;
	}

	if (key == 27)
		exit(0);
}

void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)				//	Procesarea tastelor 'LEFT', 'RIGHT', 'UP', 'DOWN';
	{							//	duce la deplasarea observatorului pe suprafata sferica in jurul cubului;
	case GLUT_KEY_UP:
		beta -= 0.01;
		break;
	case GLUT_KEY_DOWN:
		beta += 0.01;
		break;
	case GLUT_KEY_LEFT:
		alpha += incrAlpha1;
		if (abs(alpha - PI / 2) < 0.05)
		{
			incrAlpha1 = 0.f;
		}
		else
		{
			incrAlpha1 = 0.01f;
		}
		break;
	case GLUT_KEY_RIGHT:
		alpha -= incrAlpha2;
		if (abs(alpha + PI / 2) < 0.05)
		{
			incrAlpha2 = 0.f;
		}
		else
		{
			incrAlpha2 = 0.01f;
		}
		break;
	}
}


// desenare fulgi de nea-sfere
void drawSphere(float radius, int slices, int stacks) {
	for (int i = 0; i <= stacks; ++i) {
		float lat0 = PI * (-0.5 + (float)(i) / stacks);
		float z0 = sin(lat0);
		float zr0 = cos(lat0);

		float lat1 = PI * (-0.5 + (float)(i + 1) / stacks);
		float z1 = sin(lat1);
		float zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= slices; ++j) {
			float lng = 2 * PI * (float)(j) / slices;
			float x = cos(lng);
			float y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);

			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
		}
		glEnd();
	}
}


// Initializarea fulgilor
void InitSnowflakes(int count) {
	
	for (int i = 0; i < count; ++i) {
		Snowflake snowflake;
		snowflake.position = glm::vec3(
			static_cast<float>(rand()) / RAND_MAX * (2.32669 - (-0.182578)) + (-0.182578), 
			static_cast<float>(rand()) / RAND_MAX * (15.0f - 5.0f) + 5.0f, 
			static_cast<float>(rand()) / RAND_MAX * (2.786716 - 0.005901) - 2.486716  
		);
		snowflake.speed = static_cast<float>(rand() % 100) / 10000.0f + 0.005f; 

		snowflakes.push_back(snowflake);
	}
}


void UpdateSnowflakes() {
	if (!isSnowfallActive) return; 

	for (auto& snowflake : snowflakes) {
		snowflake.position.y -= snowflake.speed;
		if (snowflake.position.y < min_y) {
			snowflake.position.y = 10.0f;
			snowflake.position.x = static_cast<float>(rand()) / RAND_MAX * (2.32669 - (-0.182578)) + (-0.182578);
			snowflake.position.z = static_cast<float>(rand()) / RAND_MAX * (2.786716 - 0.005901) - 2.486716;
		}
	}
}

void RenderSnowflakes() {
	for (const auto& flake : snowflakes) {
		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), flake.position) *
			glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)); 
		glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniform1i(colorLocation, 0); // cod 0 - alb
		drawSphere(0.15f, 40, 40);
	}
	glBindVertexArray(0);
}



// Se initializeaza un vertex Buffer Object(VBO) pentru transferul datelor spre memoria placii grafice(spre shadere);

void CreateVBOSol(void)
{

	// Generare VAO;
	glGenVertexArrays(1, &VaoIdSol);
	glBindVertexArray(VaoIdSol);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdSol);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdSol);
	glBufferData(GL_ARRAY_BUFFER, verticesSol.size() * sizeof(glm::vec3) + normalsSol.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSol.size() * sizeof(glm::vec3), &verticesSol[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesSol.size() * sizeof(glm::vec3), normalsSol.size() * sizeof(glm::vec3), &normalsSol[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesSol.size() * sizeof(glm::vec3)));

}

void CreateVBOCasa(void)
{

	// Generare VAO;
	glGenVertexArrays(1, &VaoIdCasa);
	glBindVertexArray(VaoIdCasa);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdCasa);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdCasa);
	glBufferData(GL_ARRAY_BUFFER, verticesCasa.size() * sizeof(glm::vec3) + normalsCasa.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesCasa.size() * sizeof(glm::vec3), &verticesCasa[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesCasa.size() * sizeof(glm::vec3), normalsCasa.size() * sizeof(glm::vec3), &normalsCasa[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesCasa.size() * sizeof(glm::vec3)));

}

void CreateVBOSnowman(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdSnowman);
	glBindVertexArray(VaoIdSnowman);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdSnowman);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdSnowman);
	glBufferData(GL_ARRAY_BUFFER, verticesSnowman.size() * sizeof(glm::vec3) + normalsSnowman.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSnowman.size() * sizeof(glm::vec3), &verticesSnowman[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesSnowman.size() * sizeof(glm::vec3), normalsSnowman.size() * sizeof(glm::vec3), &normalsSnowman[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesSnowman.size() * sizeof(glm::vec3)));

}

void CreateVBOSnowmanNas(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdSnowmanNas);
	glBindVertexArray(VaoIdSnowmanNas);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdSnowmanNas);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdSnowmanNas);
	glBufferData(GL_ARRAY_BUFFER, verticesSnowmanNas.size() * sizeof(glm::vec3) + normalsSnowmanNas.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSnowmanNas.size() * sizeof(glm::vec3), &verticesSnowmanNas[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesSnowmanNas.size() * sizeof(glm::vec3), normalsSnowmanNas.size() * sizeof(glm::vec3), &normalsSnowmanNas[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesSnowmanNas.size() * sizeof(glm::vec3)));

}

void CreateVBOSnowmanAux(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdSnowmanAux);
	glBindVertexArray(VaoIdSnowmanAux);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdSnowmanAux);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdSnowmanAux);
	glBufferData(GL_ARRAY_BUFFER, verticesSnowmanAux.size() * sizeof(glm::vec3) + normalsSnowmanAux.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSnowmanAux.size() * sizeof(glm::vec3), &verticesSnowmanAux[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesSnowmanAux.size() * sizeof(glm::vec3), normalsSnowmanAux.size() * sizeof(glm::vec3), &normalsSnowmanAux[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesSnowmanAux.size() * sizeof(glm::vec3)));

}
void CreateVBOSnowmanAccesorii(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdSnowmanAccesorii);
	glBindVertexArray(VaoIdSnowmanAccesorii);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdSnowmanAccesorii);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdSnowmanAccesorii);
	glBufferData(GL_ARRAY_BUFFER, verticesSnowmanAccesorii.size() * sizeof(glm::vec3) + normalsSnowmanAccesorii.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSnowmanAccesorii.size() * sizeof(glm::vec3), &verticesSnowmanAccesorii[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesSnowmanAccesorii.size() * sizeof(glm::vec3), normalsSnowmanAccesorii.size() * sizeof(glm::vec3), &normalsSnowmanAccesorii[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesSnowmanAccesorii.size() * sizeof(glm::vec3)));

}
void CreateVBOBrad(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdBrad);
	glBindVertexArray(VaoIdBrad);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdBrad);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdBrad);
	glBufferData(GL_ARRAY_BUFFER, verticesBrad.size() * sizeof(glm::vec3) + normalsBrad.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesBrad.size() * sizeof(glm::vec3), &verticesBrad[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesBrad.size() * sizeof(glm::vec3), normalsBrad.size() * sizeof(glm::vec3), &normalsBrad[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesBrad.size() * sizeof(glm::vec3)));

}
void CreateVBOUsa(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdUsa);
	glBindVertexArray(VaoIdUsa);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdUsa);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdUsa);
	glBufferData(GL_ARRAY_BUFFER, verticesUsa.size() * sizeof(glm::vec3) + normalsUsa.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesUsa.size() * sizeof(glm::vec3), &verticesUsa[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesUsa.size() * sizeof(glm::vec3), normalsUsa.size() * sizeof(glm::vec3), &normalsUsa[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesUsa.size() * sizeof(glm::vec3)));

}
void CreateVBOWoods(void)
{
	// Generare VAO;
	glGenVertexArrays(1, &VaoIdWoods);
	glBindVertexArray(VaoIdWoods);

	// Generare VBO - varfurile si normalele sunt memorate in sub-buffere;
	glGenBuffers(1, &VboIdWoods);
	glBindBuffer(GL_ARRAY_BUFFER, VboIdWoods);
	glBufferData(GL_ARRAY_BUFFER, verticesWoods.size() * sizeof(glm::vec3) + normalsWoods.size() * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesWoods.size() * sizeof(glm::vec3), &verticesWoods[0]);
	glBufferSubData(GL_ARRAY_BUFFER, verticesWoods.size() * sizeof(glm::vec3), normalsWoods.size() * sizeof(glm::vec3), &normalsWoods[0]);

	// Atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(verticesWoods.size() * sizeof(glm::vec3)));

}

//  Eliminarea obiectelor de tip VBO dupa rulare;
void DestroyVBO(void)
{
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteVertexArrays(1,&VaoIdSol);
	glDeleteVertexArrays(1, &VaoIdWoods);
	glDeleteVertexArrays(1, &VaoIdBrad);
	glDeleteVertexArrays(1, &VaoIdUsa);
	glDeleteVertexArrays(1, &VaoIdCasa);
	glDeleteVertexArrays(1, &VaoIdSnowmanAux);
	glDeleteVertexArrays(1, &VaoIdSnowmanAccesorii);
	glDeleteVertexArrays(1, &VaoIdSnowmanNas);
}

//  Crearea si compilarea obiectelor de tip shader;
void CreateShaders(void)
{
	ProgramId = LoadShaders("Proiect3d_Shader.vert", "Proiect3d_Shader.frag");
	glUseProgram(ProgramId);
}

// Elimina obiectele de tip shader dupa rulare;
void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

//  Functia de eliberare a resurselor alocate de program;
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
	
};

//  Setarea parametrilor necesari pentru fereastra de vizualizare;
void Initialize(void)
{
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f); //fundal albastru închis
	// Incarcarea modelului 3D in format OBJ, trebuie sa fie in acelasi director cu proiectul actual;
	bool model = loadOBJ("sol.obj", verticesSol, uvs, normalsSol);
	bool model2 = loadOBJ("casa.obj", verticesCasa, uvs, normalsCasa);
	bool model3 = loadOBJ("corp_snowman.obj", verticesSnowman, uvs, normalsSnowman);
	bool model3_nas = loadOBJ("nas_snowman.obj", verticesSnowmanNas, uvs, normalsSnowmanNas);
	bool model3_auxiliare = loadOBJ("aux_snowman.obj", verticesSnowmanAux, uvs, normalsSnowmanAux);
	bool model3_accesorii = loadOBJ("accesorii_snowman.obj", verticesSnowmanAccesorii, uvs, normalsSnowmanAccesorii);
	bool model4 = loadOBJ("brad.obj", verticesBrad, uvs, normalsBrad);
	bool model5 = loadOBJ("woods.obj", verticesWoods, uvs, normalsWoods);
	bool model6 = loadOBJ("usa.obj", verticesUsa, uvs, normalsUsa);
	nrVertices = verticesSol.size() + verticesCasa.size() + verticesBrad.size() + verticesSnowman.size() + verticesWoods.size() + verticesSnowmanNas.size() + verticesSnowmanAux.size()+ verticesSnowmanAccesorii.size()+ verticesUsa.size();

	// Crearea VBO / shadere-lor
	CreateVBOSol();
	CreateVBOCasa();
	CreateVBOSnowman();
	CreateVBOSnowmanNas();
	CreateVBOSnowmanAux();
	CreateVBOSnowmanAccesorii();
	CreateVBOBrad();
	CreateVBOWoods();
	CreateVBOUsa();
	CreateShaders();

	// Locatii ptr shader
	nrVertLocation = glGetUniformLocation(ProgramId, "nrVertices");
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	colorLocation = glGetUniformLocation(ProgramId, "codCol");

	// Variabile ce pot fi transmise catre shadere
	glUniform1i(ProgramId, nrVertices);
}

//	Functia de desenare a graficii pe ecran;
void RenderFunction(void)
{
	// Initializare ecran + test de adancime;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Matricea de modelare 
	myMatrix = glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 1.0, 0.0))
		* glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 0.0, 1.0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	//	Vizualizare;
	//	Pozitia observatorului - se deplaseaza pe sfera;
	obsX = refX + dist * cos(alpha) * cos(beta);
	obsY = refY + dist * cos(alpha) * sin(beta);
	obsZ = refZ + dist * sin(alpha);
	//	Vectori pentru matricea de vizualizare;
	glm::vec3 obs = glm::vec3(obsX, obsY, obsZ);		//	Pozitia observatorului;	
	glm::vec3 pctRef = glm::vec3(refX, refY, refZ); 	//	Pozitia punctului de referinta;
	glm::vec3 vert = glm::vec3(vX, vY, vZ);			//	Verticala din planul de vizualizare; 
	// Matricea de vizualizare, transmitere catre shader
	view = glm::lookAt(obs, pctRef, vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	//	Proiectie;
	projection = glm::infinitePerspective(GLfloat(fov), GLfloat(width) / GLfloat(height), dNear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// roteste scena
	glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(0.0, 1.0, 0.0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &rotate[0][0]);

	glUniform4f(colorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
	glBindVertexArray(VaoIdSol);
	glUniform1i(colorLocation, 0); // cod 0 - alb
	glDrawArrays(GL_TRIANGLES, 0, verticesSol.size());

	glBindVertexArray(VaoIdCasa);
	glUniform1i(colorLocation, 1); // cod 1 - maro
	glDrawArrays(GL_TRIANGLES, 0, verticesCasa.size());
	
	glBindVertexArray(VaoIdSnowman);
	glUniform1i(colorLocation, 0); // cod 0 - alb
	glDrawArrays(GL_TRIANGLES, 0, verticesSnowman.size());

	glBindVertexArray(VaoIdSnowmanNas);
	glUniform1i(colorLocation, 3); // cod 3 - portocaliu
	glDrawArrays(GL_TRIANGLES, 0, verticesSnowmanNas.size());
	
	glBindVertexArray(VaoIdSnowmanAux);
	glUniform1i(colorLocation, 1); // cod 1 - maro
	glDrawArrays(GL_TRIANGLES, 0, verticesSnowmanAux.size());

	glBindVertexArray(VaoIdSnowmanAccesorii);
	glUniform1i(colorLocation, 4); // cod 4 - rosu
	glDrawArrays(GL_TRIANGLES, 0, verticesSnowmanAccesorii.size());

	glBindVertexArray(VaoIdBrad);
	glUniform1i(colorLocation, 2); // cod 2 - verde
	glDrawArrays(GL_TRIANGLES, 0, verticesBrad.size());


	glBindVertexArray(VaoIdUsa);
	glUniform1i(colorLocation, 6); // cod 6 - maro inchis
	glDrawArrays(GL_TRIANGLES, 0, verticesUsa.size());

	glBindVertexArray(VaoIdWoods);
	glUniform1i(colorLocation, 1); // cod 1 - maro
	glDrawArrays(GL_TRIANGLES, 0, verticesWoods.size());
	UpdateSnowflakes();
	RenderSnowflakes();
	glutSwapBuffers();
	glFlush();
}

//	Punctul de intrare in program, se ruleaza rutina OpenGL;
int main(int argc, char* argv[])
{	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("Peisaj de iarna");
	glewInit();
	Initialize();
	InitSnowflakes(150); // 150 de fulgi
	glutIdleFunc(RenderFunction);
	glutDisplayFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}

