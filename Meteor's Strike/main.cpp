#include "assimp.h"
#include "aiPostProcess.h"
#include "aiScene.h"
#include "GL/glut.h"
#include <IL/il.h>
#include <string.h>
#include <map>
#include <list>
#include <iostream>
#include <string>
#include <map>
#include <random>
#include <math.h>
#include "Meteorites.h"
#include "Planet.h"
#include <ctime>

using namespace std;


#define TRUE	1
#define FALSE	0
#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

#define PI 3.14159265

#define SPACEBAR 32


// the global Assimp scene object
const struct aiScene* scene = NULL;
GLuint scene_list = 0;
struct aiVector3D scene_min, scene_max, scene_center;

//Liste da renderizzare
//GLubyte lists[2];

GLfloat LightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[] = { 0.0f, 0.0f, 15.0f, 1.0f };

// current rotation angle
static float angle = 0.f;
static float visualangle = 0.f;

//Angolo per difficoltà
static float diffAngle = 0.002;

//Offset in modo che i meteoriti non compaiano di fronte allo schermo
static float offsetAngleMeteorites = -94.0814;

// images / texture
std::map<std::string, GLuint*> textureIdMap;	// map image filenames to textureIds
GLuint*		textureIds;							// pointer to texture Array

// currently this is hardcoded
//static const std::string basepath = "./models/textures/"; //obj..
static const std::string basepath = "./models/"; // per i file blend

//Variabili movimento 
static float forwardMov = 0.0;
static float leftMov = 0.0;
static float backward = 0.0;
static float alfa = 0.0;
static float up = 0.0;

//Variabili per controllo osservatore virtuale
static float eyex = -18.0f;
static float eyey = 4.0f;
static float eyez = -8.875f;
static float centerx = -18.0f;
static float centery = -0.25f;
static float centerz = 0.75f;

//Numero meteoriti di ogni tipo (numMeteoritiTot = numMeteorites*4)
static int numMeteorites = 2;
//static int numMeteorites = 40;
//Booleano per segnalare un giro effettuato dai meteoriti
static bool lapDone = false;

//Booleano per segnalare l'incremento avvenuto dei meteoriti
//settato a true per non incrementare al primo giro
static bool multiplied = true;

//Lista meteoriti
list<Meteorite> meteorites;

// Liste per rendering
static GLubyte lists[3];

//Valori cubo per collisioni astronave
//static double sizeCubeSpaceship = 1.68;
static double sizeCubeSpaceship = 0.5;
static float posxCubeSpaceship = -18.16;
static float posyCubeSpaceship = 0;
static float poszCubeSpaceship = 0;

// Valori cubo per collisioni asteroidi
static double sizeCubeMeteorites = 1.2;
static float posxCubeMeteorites = -17.6;
static float posyCubeMeteorites = 0;
static float poszCubeMeteorites = 10.96;

static double sizePlanet = 2;
static float posxPlanet = 0.0;
static float posyPlanet = 0.0;
static float poszPlanet = 0.0;

//Posizioni e dimensioni sfera pianeta terra
static double sizeEarthSphere = 8.08;
static float posxEarthSphere = -28;
static float posyEarthSphere = -0.08;
static float poszEarthSphere = -1.68;

//Posizione e dimensioni Luna 
static double sizeMoonSphere = 2.72;
static float posxMoonSphere = -11.28;
static float posyMoonSphere = -1.2;
static float poszMoonSphere = 21.04;

//Posizione e dimensioni Marte
static double sizeMarsSphere = 6.8;
static float posxMarsSphere = 13.52;
static float posyMarsSphere = -4.88;
static float poszMarsSphere = 18.08;

//Posizione e diensioni Saturno
static double sizeSaturnSphere = 8.08;
static float posxSaturnSphere = 23.36;
static float posySaturnSphere = -0.8;
static float poszSaturnSphere = -18;

Planet earth(8.08, -28, 0.08, -1.68);
Planet moon(2.72, -11.28, -1.2, 21.04);
Planet mars(6.8, 13.52, -4.88, 18.08);
Planet saturn(8.08, 23.36, -0.8, -18);

list<Planet> listOfPlanet;


// Flag per inizio gioco
bool startingGame = false;

// Vite astronave
int lives = 3;

//booleano per danneggiamento
bool damaged = false;

//Frame danneggiamento
static int damagedFrames = 200;

//Fine gioco (vittoria)
bool win = false;

//Fine gioco (sconfitta)
bool lost = false;

//Durata animazione finale per sconfitta
int framesAfterLost = 600;

//Stringhe da visualizzare a schermo
static char scoreStr[30] = "";
static char livesStr[30] = "";
static char levelStr[30] = "";

//Punteggio
int score = 0;
time_t startTime;

//Livello
int level = 1;

//Arrivati al livello 15, viene richiesto se si vuole continuare a giocare
bool askToContinue = false;

//Vuoi continuare la partita? 
static char askToContStr[100] = "";
static char askToContStr2[100] = "";

// Game over 
static char gameOver[30] = "";

// Vittoria
static char youWin[30] = "";

// Stringhe iniziali
static char strStartGame[100] = "";
static char strStartGame2[100] = "";

// Tutorial
static char tutorialStr[300] = "";
static char tutorialStr2[300] = "";
static char tutorialStr3[300] = "";
static char tutorialStr4[300] = "";
static char tutorialStr5[300] = "";
static char tutorialStr6[300] = "";
static char tutorialStr7[300] = "";
static char tutorialStr8[300] = "";
static char tutorialStr9[300] = "";

// Avvio tutorial
bool tutorial = false;

//Indicatore menù
static int startMenu;

//Aggiorna il punteggio
void updateScore() {
	if (startingGame && !win && !lost && !askToContinue) {
		time_t currentTime = time(0);
		score = 100 * (currentTime - startTime);

		sprintf_s(scoreStr, "SCORE: %d", score);
	}
}

//Drawing text on screen
void drawString(int x, int y, char *string) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	glOrtho(0, w, 0, h, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 1.0);

	glRasterPos2i(x, y);
	void *font = GLUT_BITMAP_TIMES_ROMAN_24;
	for (char* c = string; *c != '\0'; c++)
	{
		glutBitmapCharacter(font, *c);
	}

	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void drawStringV2(int x, int y, GLfloat scale, GLfloat lineWidth, char *string) {
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor3f(0.0, 1.0, 0.0);
	glLineWidth(lineWidth);

	//Translates the character object with its axis of rotation
	glTranslatef(x, y, 0);

	glScalef(scale, scale, scale);

	//glRotatef(angle, 0.0, 1.0, 0.0);

	//Rotates the characters about z-axis by angle degree
	//glRotatef(angle, 0.0, 0.0, 1.0);

	//translates the character object by 700 unit to the -ve x-axis
	//glTranslatef(-700,0,0); 

	for (int i = 0; i < strlen(string); i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, string[i]);
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void reshape(int width, int height) {

	const double aspectRatio = (float)width / height, fieldOfView = 45.0;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fieldOfView, aspectRatio,
		1.0, 1000.0);  // Znear and Zfar 
	glViewport(0, 0, width, height);
}

void get_bounding_box_for_node(const struct aiNode* nd,
	struct aiVector3D* min,
	struct aiVector3D* max,
	struct aiMatrix4x4* trafo) {

	struct aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo, &nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			struct aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp, trafo);

			min->x = aisgl_min(min->x, tmp.x);
			min->y = aisgl_min(min->y, tmp.y);
			min->z = aisgl_min(min->z, tmp.z);

			max->x = aisgl_max(max->x, tmp.x);
			max->y = aisgl_max(max->y, tmp.y);
			max->z = aisgl_max(max->z, tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n)
		get_bounding_box_for_node(nd->mChildren[n], min, max, trafo);

	*trafo = prev;
}

void get_bounding_box(struct aiVector3D* min, struct aiVector3D* max) {
	struct aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z = 1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene->mRootNode, min, max, &trafo);
}

void color4_to_float4(const struct aiColor4D *c, float f[4]) {
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

void set_float4(float f[4], float a, float b, float c, float d) {
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

void apply_material(const struct aiMaterial *mtl) {

	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	struct aiColor4D diffuse;
	struct aiColor4D specular;
	struct aiColor4D ambient;
	struct aiColor4D emission;
	float shininess, strength;
	int two_sided;
	int wireframe;
	int max;

	int texIndex = 0;

	//contains filename of texture
	aiString texPath;
	if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath)) {

		//bind texture
		unsigned int texId = *textureIdMap[texPath.data];
		glBindTexture(GL_TEXTURE_2D, texId);
	}

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);

	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, (unsigned int *)&max);
	max = 1;
	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, (unsigned int *)&max);
	if ((ret1 == AI_SUCCESS) && (ret2 == AI_SUCCESS))
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if (AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, (unsigned int *)&max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, (unsigned int *)&max)) && two_sided)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

// Can't send color down as a pointer to aiColor4D because AI colors are ABGR.
void Color4f(const struct aiColor4D *color)
{
	glColor4f(color->r, color->g, color->b, color->a);
}

void recursive_render(const struct aiScene *sc, const struct aiNode* nd, float scale) {

	unsigned int i;
	unsigned int n = 0, t;
	struct aiMatrix4x4 m = nd->mTransformation;

	printf("Node name: %s\n", nd->mName.data);

	//m.Scaling(aiVector3D(scale, scale, scale), m);

	// update transform
	m.Transpose();
	glPushMatrix();
	glMultMatrixf((float*)&m);

	// draw all meshes assigned to this node
	for (; n < nd->mNumMeshes; ++n) {

		const struct aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		printf("Drawing MESH with this name: %s\n", mesh->mName.data);

		apply_material(sc->mMaterials[mesh->mMaterialIndex]);

		if (mesh->HasTextureCoords(0)) glEnable(GL_TEXTURE_2D);
		else glDisable(GL_TEXTURE_2D);
		if (mesh->mNormals == NULL) glDisable(GL_LIGHTING);
		else glEnable(GL_LIGHTING);
		if (mesh->mColors[0] != NULL) glEnable(GL_COLOR_MATERIAL);
		else glDisable(GL_COLOR_MATERIAL);

		for (t = 0; t < mesh->mNumFaces; ++t) {

			const struct aiFace* face = &mesh->mFaces[t];
			GLenum face_mode;

			switch (face->mNumIndices) {

			case 1: face_mode = GL_POINTS; break;
			case 2: face_mode = GL_LINES; break;
			case 3: face_mode = GL_TRIANGLES; break;
			default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			// go through all vertices in face
			for (i = 0; i < face->mNumIndices; i++) {

				// get group index for current index
				int vertexIndex = face->mIndices[i];
				if (mesh->mColors[0] != NULL) Color4f(&mesh->mColors[0][vertexIndex]);
				if (mesh->mNormals != NULL)
					// HasTextureCoords(texture_coordinates_set)
					if (mesh->HasTextureCoords(0))
						// mTextureCoords[channel][vertex]
						glTexCoord2f(mesh->mTextureCoords[0][vertexIndex].x, 1 - mesh->mTextureCoords[0][vertexIndex].y);

				glNormal3fv(&mesh->mNormals[vertexIndex].x);
				glVertex3fv(&mesh->mVertices[vertexIndex].x);
			}

			glEnd();
		}
	}

	// draw all children
	for (n = 0; n < nd->mNumChildren; ++n)
		recursive_render(sc, nd->mChildren[n], scale);

	glPopMatrix();
}

void do_motion(void) {
	
		static GLint prev_time = 0;
		int time = glutGet(GLUT_ELAPSED_TIME);
		if (startingGame && !askToContinue) {
			angle -= (time - prev_time)*diffAngle;
			//angle -= 0.08;
		}
		else if(!askToContinue)
			//visualangle -= (time - prev_time)*0.002;
			visualangle -= 0.08;
		prev_time = time;
		glutPostRedisplay();
	
}

bool checkCollisionWithMeteor() {

	list<Meteorite>::iterator iter = meteorites.begin();
	while (iter != meteorites.end()) {
		Meteorite m = *iter;
		/*
		x' = xcos(a) + zsen(a)
		y'=y
		z'= -xsen(a)+zcos(a)
		*/
		double radiantAngle = (((angle * 10)+offsetAngleMeteorites)*PI) / 180;
		double x1 = m.getPosxCube();
		double x2 = posxCubeSpaceship + leftMov;
		double y1 = m.getPosyCube();
		double y2 = posyCubeSpaceship + up;
		double z1 = m.getPoszCube();
		double z2 = poszCubeSpaceship + forwardMov;


		double xrot = (x1 * cos(radiantAngle)) + (z1*sin(radiantAngle));
		double zrot = (-x1 * sin(radiantAngle)) + (z1 * cos(radiantAngle));

		double dx = xrot - x2;
		double dy = y1 - y2;
		double dz = zrot - z2;

		double distance = sqrt(dx * dx + dy * dy + dz * dz);
		if (distance <= m.getSizeCube() + sizeCubeSpaceship) {
			return true;
		}

		iter++;

	}
	return false;
}

//bool checkCollisionSpaceshipWithPlanet() {
//
//	list<Planet>::iterator iter = listOfPlanet.begin();
//	while (iter != listOfPlanet.end()) {
//		Planet p = *iter;
//		/*
//		x' = xcos(a) + zsen(a)
//		y'=y
//		z'= -xsen(a)+zcos(a)
//		*/
//		double radiantAngle = (angle*PI) / 180;
//		double x1 = p.getx();
//		double x2 = posxCubeSpaceship + leftMov;
//		double y1 = p.gety();
//		double y2 = posyCubeSpaceship + up;
//		double z1 = p.getz();
//		double z2 = poszCubeSpaceship + forwardMov;
//
//
//		double xrot = (x1 * cos(radiantAngle)) + (z1*sin(radiantAngle));
//		double zrot = (-x1 * sin(radiantAngle)) + (z1 * cos(radiantAngle));
//
//		double dx = xrot - x2;
//		double dy = y1 - y2;
//		double dz = zrot - z2;
//
//		double distance = sqrt(dx * dx + dy * dy + dz * dz);
//		if (distance <= p.getSize() + sizeCubeSpaceship) {
//			return true;
//		}
//
//		iter++;
//
//	}
//	return false;
//
//}

//bool checkCollisionMeteorWithPlanet(list<Meteorite>::iterator iterM) {
//	list<Planet>::iterator iterP = listOfPlanet.begin();
//	Meteorite m = *iterM;
//	/*
//	x' = xcos(a) + zsen(a)
//	y'=y
//	z'= -xsen(a)+zcos(a)
//	*/
//
//	while (iterP != listOfPlanet.end()) {
//		double radiantAngleMeteorites = (((angle * 10) + offsetAngleMeteorites)*PI) / 180;
//		double radiantAnglePlanet = (angle*PI) / 180;
//
//		Planet p = *iterP;
//		double x2 = p.getx();
//		double y2 = p.gety();
//		double z2 = p.getz();
//
//		double x1 = m.getPosx();
//		double y1 = m.getPosy();
//		double z1 = m.getPosz();
//
//		double xrotPlanet = (x2 * cos(radiantAnglePlanet)) + (z2*sin(radiantAnglePlanet));
//		double zrotPlanet = (-x2 * sin(radiantAnglePlanet)) + (z2 * cos(radiantAnglePlanet));
//
//		double xrotMeteorites = (x1 * cos(radiantAngleMeteorites)) + (z1*sin(radiantAngleMeteorites));
//		double zrotMeteorites = (-x1 * sin(radiantAngleMeteorites)) + (z1 * cos(radiantAngleMeteorites));
//
//		double dx = xrotMeteorites - xrotPlanet;
//		double dy = y1 - y2;
//		double dz = zrotMeteorites - zrotPlanet;
//
//		double distance = sqrt(dx * dx + dy * dy + dz * dz);
//		if (distance <= p.getSize() + m.getSizeCube()) {
//			return true;
//		}
//		iterP++;
//
//	}
//
//	return false;
//}

bool showSpaceship() {
	int num = 190;
	while (num != 10) {
		if (damagedFrames >= num && damagedFrames<num + 10) {
			return false;
		}
		num -= 20;
	}
	return true;
}

void resetGame() {
	//Resetto variabili al loro valore iniziale
	angle = 0.f;
	visualangle = 0.f;

	forwardMov = 0.0;
	leftMov = 0.0;
	backward = 0.0;
	alfa = 0.0;
	up = 0.0;

	eyex = -18.0f;
	eyey = 4.0f;
	eyez = -8.875f;
	centerx = -18.0f;
	centery = -0.25f;
	centerz = 0.75f;

	
	numMeteorites = 2;

	lapDone = false;

	multiplied = true;

	//Svuoto la lista dei meteoriti
	meteorites.clear();

	//Cancello display list
	glDeleteLists(scene_list, 6);

	scene_list = 0;

	startingGame = false;

	lives = 3;

	damaged = false;

	damagedFrames = 200;

	win = false;

	lost = false;

	framesAfterLost = 200;

	score = 0;

	level = 1;
	
	sprintf_s(scoreStr, "", "");
	sprintf_s(livesStr, "", "");
	sprintf_s(levelStr, "", "");
	sprintf_s(askToContStr, "", "");
	sprintf_s(askToContStr2, "", "");
	sprintf_s(youWin, "", "");
	sprintf_s(gameOver, "", "");
	

	sprintf_s(strStartGame, "", "");
	sprintf_s(strStartGame2, "", "");
	sprintf_s(tutorialStr, "", "");
	sprintf_s(tutorialStr2, "", "");
	sprintf_s(tutorialStr3, "", "");
	sprintf_s(tutorialStr4, "", "");
	sprintf_s(tutorialStr5, "", "");
	sprintf_s(tutorialStr6, "", "");
	sprintf_s(tutorialStr7, "", "");
	sprintf_s(tutorialStr8, "", "");
	sprintf_s(tutorialStr9, "", "");

	askToContinue = false;

	diffAngle = 0.002;

	glutPostRedisplay();
}

void startGame(int choice) {
	switch (choice) {
		//Avvia gioco
	case 1:
		if (!startingGame || win || lost) {
			startingGame = true;
			visualangle = 0;
			//Inizializzazione scritte a schermo
			sprintf_s(scoreStr, "SCORE: %d", score);
			sprintf_s(livesStr, "LIVES: %d", lives);
			sprintf_s(levelStr, "LEVEL  %d", level);
			startTime = time(0);

			sprintf_s(strStartGame, "", "");
			sprintf_s(strStartGame2, "", "");


			glutDestroyMenu(startMenu);

		}
		break;
		//Tutorial
	case 2:
		if (!startingGame || win || lost) {
			tutorial = true;
		}
		break;

	}
}


void display(void) {
	if (!startingGame) {
		//Apparizione stringa iniziale
		sprintf_s(strStartGame, "%s", "CLICCA COL TASTO SINISTRO");
		sprintf_s(strStartGame2, "%s", "PER APRIRE IL MENU'");

		//Creazione menù
		startMenu = glutCreateMenu(startGame);
		glutAddMenuEntry("Avvia gioco", 1);
		glutAddMenuEntry("Tutorial", 2);
		glutAttachMenu(GLUT_LEFT_BUTTON);
	}

	if (tutorial) {
		//Visualizza tutorial
		sprintf_s(tutorialStr, "%s", "EVITA GLI ASTEROIDI E SOPRAVVIVI AL MASSACRO!");
		sprintf_s(tutorialStr2, "%s", "CONTROLLI ASTRONAVE:");
		sprintf_s(tutorialStr3, "%s", "W - AVANTI");
		sprintf_s(tutorialStr4, "%s", "S - INDIETRO");
		sprintf_s(tutorialStr5, "%s", "A - SINISTRA");
		sprintf_s(tutorialStr6, "%s", "D - DESTRA");
		sprintf_s(tutorialStr7, "%s", "Q - UPBOOST");
		sprintf_s(tutorialStr8, "%s", "E - DOWNBOOST");
		sprintf_s(tutorialStr9, "%s", "PER AVVIARE IL GIOCO PREMI SPACE");

		glutDestroyMenu(startMenu);

		sprintf_s(strStartGame, "", "");
		sprintf_s(strStartGame2, "", "");

	}
	if (!win && !lost) {
		// Mantiene la rotazione nel periodo 0-360 e resetta il booleano multiplied
		if (angle < -360) angle += 360;

		
		// Verifica se i meteoriti hanno effettuato un giro completo
		if ((int)((angle * 10)) % 360 > -20) {
			//if (angle != 0)
				//cout << "Giro completo dei meteoriti!" << endl;
			lapDone = true;
		}
		else lapDone = false;

		if (askToContinue) {

			sprintf_s(askToContStr, "%s", "PREMI [Y] PER CONTINUARE LA PARTITA");
			sprintf_s(askToContStr2, "%s", "PREMI [N] PER FERMARTI");

		}

		float tmp;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//gluLookAt(0.f, 0.f, 3.f, 0.f, 0.f, -5.f, 0.f, 1.f, 0.f);
		//gluLookAt(11.0f, 0.0f, 11.0f, 10.5f, 0.f, 11.5f, 0.f, 1.f, 0.f);
		//printf("a:%f\t b:%f\t c:%f\t d:%f\t e:%f\t f:%f\n", eyex, eyey, eyez,centerx,centery,centerz);
		//Focus to spaceship
		gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.f, 1.f, 0.f);
		// rotate it around the y axis
		glRotatef(visualangle, 0.f, 1.f, 0.f);


		// scale the whole asset to fit into our view frustum
		/*tmp = scene_max.x - scene_min.x;
		tmp = aisgl_max(scene_max.y - scene_min.y, tmp);
		tmp = aisgl_max(scene_max.z - scene_min.z, tmp);
		tmp = 1.f / tmp;
		glScalef(tmp, tmp, tmp);*/

		// center the model
		glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);


		// if the display list has not been made yet, create a new one and
		// fill it with scene contents
		if (scene_list == 0) {
			scene_list = glGenLists(6);
			glNewList(scene_list, GL_COMPILE);

			// now begin at the root node of the imported data and traverse
			// the scenegraph by multiplying subsequent local transforms
			// together on GL's matrix stack.
			//recursive_render(scene, scene->mRootNode, 1.0);		

			for (int i = 0; i < scene->mRootNode->mNumChildren - 5; i++) {
				recursive_render(scene, scene->mRootNode->mChildren[i], 1.0);
			}

			glEndList();

			//Lista astronave
			glNewList(scene_list + 1, GL_COMPILE);
			recursive_render(scene, scene->mRootNode->mChildren[8], 1.0);
			glEndList();

			//Lista asteroide 1
			glNewList(scene_list + 2, GL_COMPILE);
			recursive_render(scene, scene->mRootNode->mChildren[9], 1.0);
			glEndList();

			//Lista asteroide 2
			glNewList(scene_list + 3, GL_COMPILE);
			recursive_render(scene, scene->mRootNode->mChildren[10], 1.0);
			glEndList();

			//Lista asteroide 3
			glNewList(scene_list + 4, GL_COMPILE);
			recursive_render(scene, scene->mRootNode->mChildren[11], 1.0);
			glEndList();

			//Lista asteroide 4
			glNewList(scene_list + 5, GL_COMPILE);
			recursive_render(scene, scene->mRootNode->mChildren[12], 1.0);
			glEndList();


			// Calcolo posizioni meteoriti
			for (int i = 0; i < numMeteorites; i++) {
				Meteorite m1(0);
				meteorites.push_back(m1);

				Meteorite m2(1);
				meteorites.push_back(m2);

				Meteorite m3(2);
				meteorites.push_back(m3);

				Meteorite m4(3);
				meteorites.push_back(m4);

			}



		}

		//cout << (angle * 10) << " " << (int)(angle * 10) % 360 << endl;

		// Incrementa il numero di meteoriti
		if (lapDone && !multiplied) {
			//Pulisci la lista dei meteoriti
			meteorites.clear();

			//Incrementa i meteoriti
			numMeteorites += 4;

			// Calcolo posizioni meteoriti
			for (int i = 0; i < numMeteorites; i++) {
				Meteorite m1(0);
				meteorites.push_back(m1);

				Meteorite m2(1);
				meteorites.push_back(m2);

				Meteorite m3(2);
				meteorites.push_back(m3);

				Meteorite m4(3);
				meteorites.push_back(m4);

			}
			multiplied = true;
			level++;
			

			if ((level % 1) == 0) {
				//Aumento velocità angolo
				diffAngle += 0.0005;
			}

				
			if (((level-1)%15) == 0)
				askToContinue = true;

			sprintf_s(levelStr, "LEVEL %d", level);
			cout << "Incremento dei meteoriti!" << endl;
		}
		else if (!lapDone) multiplied = false;

		/* Non serve per ora
		lists[0] = 0;
		lists[1] = 1;
		glListBase(scene_list);*/

		//Invoco la lista con le istruzioni per visualizzare lo scenario, lo scenario è quello che ruota
		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glCallList(scene_list);
		glPopMatrix();

		// Iteratore per iterare sulla lista di meteoriti
		list<Meteorite>::iterator meteoritesiter = meteorites.begin();

		// Ciclo per renderizzare tutti i meteoriti
		for (int i = 0; i < numMeteorites * 4; i++) {
			// Trasformazioni sul meteorite
			glPushMatrix();
			glRotatef((angle * 10) + offsetAngleMeteorites, 0.f, 1.f, 0.f);
			glTranslatef(meteoritesiter->getPosx(), meteoritesiter->getPosy(), meteoritesiter->getPosz());
			glCallList(scene_list + meteoritesiter->getSceneList());
			glPopMatrix();



			// Trasformazioni sul cubo del meteorite

			/*glPushMatrix();
			glRotatef((angle * 10)+offsetAngleMeteorites, 0.f, 1.f, 0.f);
			//glTranslatef(meteoritesiter->getPosxCube(), meteoritesiter->getPosyCube(), meteoritesiter->getPoszCube());
			//glutSolidCube(sizeCubeMeteorites);
			//Rendo invisibile il cubo
			//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glTranslatef(meteoritesiter->getPosxCube(), meteoritesiter->getPosyCube(), meteoritesiter->getPoszCube());
			//glutSolidSphere(meteoritesiter->getSizeCube(), 50, 50);
			glutSolidSphere(meteoritesiter->getSizeCube(), 50, 50);

			glPopMatrix();*/


			meteoritesiter++;

		}

		meteoritesiter = meteorites.begin();

		if (checkCollisionWithMeteor() && !damaged) {
			damaged = true;
			lives--;
			//Aggiornamento vite a schermo
			if(lives!=-1)
				sprintf_s(livesStr, "LIVES: %d", lives);
			// Fine gioco
			if (lives == -1)
				lost = true;
		}

		//Invoco la lista con le istruzioni per visualizzare l'astronave, con tutte le trasformazioni
		if (!damaged) {
			glPushMatrix();
			glTranslated(leftMov, up, forwardMov);
			glCallList(scene_list + 1);
			glPopMatrix();
		}
		else {
			bool show = showSpaceship();

			if (!show) {
				damagedFrames--;
			}
			else {
				glPushMatrix();
				glTranslated(leftMov, up, forwardMov);
				glCallList(scene_list + 1);
				glPopMatrix();
				damagedFrames--;
				if (damagedFrames == 0) {
					damaged = false;
					damagedFrames = 200;
				}
			}
		}

		// Trasformazioni sul cubo dell'astronave
		/*glPushMatrix();
		glTranslatef(posxCubeSpaceship + leftMov, posyCubeSpaceship + up, poszCubeSpaceship + forwardMov);
		//glutSolidCube(sizeCubeSpaceship);
		glutSolidSphere(sizeCubeSpaceship, 50, 50);

		glPopMatrix();*/

		//Collisioni pianeti
		/*glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glTranslatef(posxPlanet, posyPlanet, poszPlanet);
		glutSolidSphere(sizePlanet,50,50);
		glPopMatrix();

		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glTranslatef(posxEarthSphere, posyEarthSphere, poszEarthSphere);
		glutSolidSphere(sizeEarthSphere, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glTranslatef(posxMoonSphere, posyMoonSphere, poszMoonSphere);
		glutSolidSphere(sizeMoonSphere, 50, 50);
		glPopMatrix();
			
		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glTranslatef(posxMarsSphere, posyMarsSphere, poszMarsSphere);
		glutSolidSphere(sizeMarsSphere, 50, 50);
		glPopMatrix();

		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glTranslatef(posxSaturnSphere, posySaturnSphere, poszSaturnSphere);
		glutSolidSphere(sizeSaturnSphere, 50, 50);
		glPopMatrix();*/

	} else if(win) {
		//Elimino richiesta 
		sprintf_s(askToContStr, "", "");
		sprintf_s(askToContStr2, "", "");
		//Fine gioco, movimento videocamera finale
		if (eyex < 10) {
			eyex += 0.08;
		}
		eyez += 0.08;


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	
		//Focus to spaceship
		gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.f, 1.f, 0.f);

		// center the model
		glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);

		//Invoco la lista con le istruzioni per visualizzare lo scenario, lo scenario è quello che ruota
		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glCallList(scene_list);
		glPopMatrix();

		//Invoco la lista con le istruzioni per visualizzare l'astronave, con tutte le trasformazioni

		glPushMatrix();
		glTranslated(leftMov, up, forwardMov);
		glCallList(scene_list + 1);
		glPopMatrix();

		sprintf_s(youWin, "%s", "HAI VINTO!!!");

		if (eyez > 5) {
			resetGame();
		}
	}
	else if (lost) {

		float tmp;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//Focus to spaceship
		gluLookAt(eyex, eyey, eyez, centerx, centery, centerz, 0.f, 1.f, 0.f);
		// rotate it around the y axis
		glRotatef(visualangle, 0.f, 1.f, 0.f);


		// center the model
		glTranslatef(-scene_center.x, -scene_center.y, -scene_center.z);


		//Invoco la lista con le istruzioni per visualizzare lo scenario, lo scenario è quello che ruota
		glPushMatrix();
		glRotatef(angle, 0.f, 1.f, 0.f);
		glCallList(scene_list);
		glPopMatrix();

		// Iteratore per iterare sulla lista di meteoriti
		list<Meteorite>::iterator meteoritesiter = meteorites.begin();

		// Ciclo per renderizzare tutti i meteoriti
		for (int i = 0; i < numMeteorites * 4; i++) {
			// Trasformazioni sul meteorite
			glPushMatrix();
			glRotatef((angle * 10) + offsetAngleMeteorites, 0.f, 1.f, 0.f);
			glTranslatef(meteoritesiter->getPosx(), meteoritesiter->getPosy(), meteoritesiter->getPosz());
			glCallList(scene_list + meteoritesiter->getSceneList());
			glPopMatrix();


			meteoritesiter++;

		}

		meteoritesiter = meteorites.begin();

		sprintf_s(gameOver, "%s", "GAME OVER");


		framesAfterLost--;
		if (framesAfterLost == 0) {
			resetGame();
		}

	}

	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);

	//In alto a sinistra
	updateScore();
	//drawString(50, (h - 50), scoreStr);
	drawStringV2(20, (h - 50), 0.25, 4.0, scoreStr);

	//In alto a destra
	//drawString((w - 200), (h - 50), livesStr);
	drawStringV2((w - 150), (h - 50), 0.25, 4.0, livesStr);

	//In alto al centro
	//drawString((w / 2), (h - 50), levelStr);
	drawStringV2((w / 2) - 75, (h - 50), 0.25, 4.0, levelStr);
	
	//Stringa per chiedere se continuare il gioco 
	//drawString((w / 2) - 300, (h - 50) / 2, askToContStr);
	drawStringV2((w / 2) - 300, (h - 50) / 2, 0.25, 4.0, askToContStr);
	drawStringV2((w / 2) - 200, (h - 150) / 2, 0.25, 4.0, askToContStr2);

	//Game over
	//drawString((w / 2) - 100, (h - 50) / 2, gameOver);
	drawStringV2((w / 2) - 400, (h / 2) - 50, 1, 8.0, gameOver);

	//Vittoria
	//drawString((w / 2) - 100, (h - 50) / 2, youWin);
	drawStringV2((w / 2) - 380, (h / 2) - 50, 1, 8.0, youWin);

	//Stringa iniziale
	//drawString((w / 2) - 300, (h - 50) / 2, strStartGame);
	drawStringV2((w / 2) - 300, (h - 50) / 2, 0.25, 4.0, strStartGame);
	drawStringV2((w / 2) - 250, (h - 150) / 2, 0.25, 4.0, strStartGame2);

	//Tutorial
	//drawString(50, (h - 50), tutorialStr);
	//drawString(50, (h - 100), tutorialStr2);
	//drawString(50, (h - 150), tutorialStr3);
	//drawString(50, (h - 200), tutorialStr4);
	//drawString(50, (h - 250), tutorialStr5);
	//drawString(50, (h - 300), tutorialStr6);
	//drawString(50, (h - 350), tutorialStr7);
	//drawString(50, (h - 400), tutorialStr8);
	//drawString(50, (h - 450), tutorialStr9);

	drawStringV2(20, (h - 50), 0.25, 4.0, tutorialStr);
	drawStringV2(20, (h - 100), 0.25, 4.0, tutorialStr2);
	drawStringV2(20, (h - 150), 0.25, 4.0, tutorialStr3);
	drawStringV2(20, (h - 200), 0.25, 4.0, tutorialStr4);
	drawStringV2(20, (h - 250), 0.25, 4.0, tutorialStr5);
	drawStringV2(20, (h - 300), 0.25, 4.0, tutorialStr6);
	drawStringV2(20, (h - 350), 0.25, 4.0, tutorialStr7);
	drawStringV2(20, (h - 400), 0.25, 4.0, tutorialStr8);
	drawStringV2(20, (h - 450), 0.25, 4.0, tutorialStr9);

	glutSwapBuffers();
	do_motion();
}

int loadasset(const char* path) {

	// we are taking one of the postprocessing presets to avoid
	// writing 20 single postprocessing flags here.
	scene = aiImportFile(path, aiProcessPreset_TargetRealtime_Quality);

	if (scene) {
		get_bounding_box(&scene_min, &scene_max);
		scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
		scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
		scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
		return 0;
	}
	return 1;
}

int LoadGLTextures(const aiScene* scene) {

	ILboolean success;

	/* Before calling ilInit() version should be checked. */
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION) {
		ILint test = ilGetInteger(IL_VERSION_NUM);

		/// wrong DevIL version ///
		std::string err_msg = "Wrong DevIL version. Old devil.dll in system32/SysWow64?";
		char* cErr_msg = (char *)err_msg.c_str();

		return -1;
	}

	/* Initialization of DevIL */
	ilInit();

	//if (scene->HasTextures()) abortGLInit("Support for meshes with embedded textures is not implemented");

	/* getTexture Filenames and Numb of Textures */
	for (unsigned int m = 0; m < scene->mNumMaterials; m++) {

		int texIndex = 0;
		aiReturn texFound = AI_SUCCESS;

		aiString path;	// filename

		while (texFound == AI_SUCCESS) {
			texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);

			// fill map with textures, pointers still NULL yet
			textureIdMap[path.data] = NULL;
			texIndex++;
		}
	}

	int numTextures = textureIdMap.size();

	/* array with DevIL image IDs */
	ILuint* imageIds = NULL;
	imageIds = new ILuint[numTextures];

	/* generate DevIL Image IDs */
	/* Generation of numTextures image names */
	ilGenImages(numTextures, imageIds);

	/* create and fill array with GL texture ids */
	textureIds = new GLuint[numTextures];

	/* Texture name generation */
	glGenTextures(numTextures, textureIds);

	/* define texture path */
	//std::string texturepath = "../../../test/models/Obj/";

	/* get iterator */
	std::map<std::string, GLuint*>::iterator itr = textureIdMap.begin();

	for (int i = 0; i<numTextures; i++) {

		//save IL image ID
		// get filename
		std::string filename = (*itr).first;

		// save texture id for filename in map
		(*itr).second = &textureIds[i];

		// next texture
		itr++;

		/* Binding of DevIL image name */
		ilBindImage(imageIds[i]);

		/* Loading of image */
		std::string fileloc = basepath + filename;
		success = ilLoadImage((/* const wchar_t * */const char *)fileloc.c_str());

		fprintf(stdout, "Loading Image: %s\n", fileloc.data());

		if (success) /* If no error occured: */ {

			/*
			* Convert every colour component into
			* unsigned byte. If your image contains alpha
			* channel you can replace IL_RGB with IL_RGBA
			*/
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
			if (!success) {

				/* Error occured */
				fprintf(stderr, "Couldn't convert image");
				return -1;
			}

			/* Texture name generation */
			//glGenTextures(numTextures, &textureIds[i]); 

			/* Binding of texture name */
			glBindTexture(GL_TEXTURE_2D, textureIds[i]);

			//redefine standard texture values
			/*
			* We will use linear
			* interpolation for magnification filter
			*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			/*
			* We will use linear
			* interpolation for minifying filter
			*/
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH),
				ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE,
				ilGetData()); /* Texture specification */
		}
		else {

			/* Error occured */
			fprintf(stderr, "Couldn't load Image: %s\n", fileloc.data());
		}
	}

	/*
	* Because we have already copied image data into texture data
	* we can release memory used by image.
	*/
	ilDeleteImages(numTextures, imageIds);

	// cleanup
	delete[] imageIds;
	imageIds = NULL;

	// return success;
	return TRUE;
}

// All Setup For OpenGL goes here
int InitGL() {

	if (!LoadGLTextures(scene)) return FALSE;
	glEnable(GL_TEXTURE_2D);

	// Enables Smooth Shading
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Depth Buffer Setup
	glClearDepth(1.0f);

	// Enables Depth Testing
	glEnable(GL_DEPTH_TEST);

	// The Type Of Depth Test To Do
	glDepthFunc(GL_LEQUAL);

	// Really Nice Perspective Calculation
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);

	// Uses default lighting parameters
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_NORMALIZE);
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT1);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	// Initialization Went OK
	return TRUE;
}


//Interazione tastiera
static void keyboard(unsigned char key, int x, int y) {
	
	switch (key) {
	case 27:
		exit(1);
		break;
		//Movimento in avanti
	case 'w':
		if(startingGame && forwardMov<=6)
			forwardMov += 0.08;
		glutPostRedisplay();
		break;
		//Movimento indietro
	case 's':
		if (startingGame && forwardMov>=-5.2)
			forwardMov -= 0.08;
		glutPostRedisplay();
		break;
		//Movimento a sinistra
	case 'a':
		if (startingGame && leftMov<=6)
			leftMov += 0.08;
		glutPostRedisplay();
		break;
		//Movimento a destra
	case 'd':
		if (startingGame && leftMov>=-6)
			leftMov -= 0.08;
		glutPostRedisplay();
		break;
		//Up-boost
	case 'q':
		if (startingGame)
			up += 0.08;
		cout << up << endl;
		glutPostRedisplay();
		break;
		//Down-boost
	case 'e':
		if (startingGame)
			up -= 0.08;
		cout << up << endl;
		glutPostRedisplay();
		break;
		//Tasti per test 
	case 'r':
		//xprova += 0.08;
		//cout << "posx" << xprova<<endl;
		//ridisegna = true;
		posxPlanet += 0.08;
		cout << posxPlanet << endl;
		glutPostRedisplay();
		break;
	case 'f':
		/*spinmov -= 0.8;
		centerx -= 0.125;*/
		//xprova -= 0.08;
		//cout << "posx" << xprova << endl;
		//ridisegna = true;
		posxPlanet -= 0.08;
		cout << posxPlanet << endl;
		glutPostRedisplay();
		break;
	case 't':
		/*spinmov -= 0.8;
		centery += 0.125;*/
		//zprova += 0.8;
		posyPlanet += 0.08;
		cout << posyPlanet << endl;

		glutPostRedisplay();
		break;
	case 'g':
		/*spinmov -= 0.8;
		centery -= 0.125;*/
		//zprova -= 0.8;
		posyPlanet -= 0.08;
		cout << posyPlanet << endl;

		glutPostRedisplay();
		break;
	case 'h':
		//spinmov -= 0.8;
		//centerx -= 0.125;
		//eyex -= 0.125;
		//ridisegna = true;
		poszPlanet -= 0.08;
		cout << poszPlanet << endl;
		glutPostRedisplay();
		break;
	case 'i':
		sizePlanet += 0.08;
		cout << sizePlanet << endl;
		break;
	case 'k':
		sizePlanet -= 0.08;
		cout << sizePlanet << endl;
		break;
	case'u':
		askToContinue = true;
		glutPostRedisplay();
		break;
	case 'y':
		//Continuo il gioco per altri 15 livelli
		if (askToContinue) {
			askToContinue = false;
			sprintf_s(askToContStr, "", "");
			sprintf_s(askToContStr2, "", "");
			glutPostRedisplay();
		}
		glutPostRedisplay();
		break;
	case 'n':
		//Fine gioco 
		if (askToContinue) {
			win = true;
		}
		break;
	case SPACEBAR:
		if (!startingGame || win || lost) {
			tutorial = false;
			startingGame = true;
			visualangle = 0;
			//Inizializzazione scritte a schermo
			sprintf_s(scoreStr, "SCORE: %d", score);
			sprintf_s(livesStr, "LIVES: %d", lives);
			sprintf_s(levelStr, "LEVEL  %d", level);
			startTime = time(0);

			sprintf_s(strStartGame, "", "");
			sprintf_s(strStartGame2, "", "");
			sprintf_s(tutorialStr, "", "");
			sprintf_s(tutorialStr2, "", "");
			sprintf_s(tutorialStr3, "", "");
			sprintf_s(tutorialStr4, "", "");
			sprintf_s(tutorialStr5, "", "");
			sprintf_s(tutorialStr6, "", "");
			sprintf_s(tutorialStr7, "", "");
			sprintf_s(tutorialStr8, "", "");
			sprintf_s(tutorialStr9, "", "");
			glutPostRedisplay();
		}
		break;
	default:
		break;
	}
}




void initPlanet() {
	listOfPlanet.push_back(earth);
	listOfPlanet.push_back(moon);
	listOfPlanet.push_back(mars);
	listOfPlanet.push_back(saturn);
}


int main(int argc, char **argv) {

	initPlanet();

	struct aiLogStream stream;
	glutInitWindowSize(900, 600);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInit(&argc, argv);
	glutCreateWindow("Meteor's Strike");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);

	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It will be active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);

	// ... exactly the same, but this stream will now write the
	// log file to assimp_log.txt
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
	aiAttachLogStream(&stream);

	// qui invocheremo la scena del gioco!
	/*
	if (argc >= 2)
	loadasset(argv[1]);
	else
	{
	fprintf(stderr, "usage: >>SimpleOpenGLTexturedLoader <modelpath\\modelname>");
	exit(1);
	}


	*/

	loadasset("models\\scenario.obj");


	if (!InitGL()) {
		fprintf(stderr, "Initialization failed");
		return FALSE;
	}

	glutGet(GLUT_ELAPSED_TIME);

	//Associo un nome al nodo relativo all'astronave
	scene->mRootNode->mChildren[8]->mName.Set("Spaceship");

	

	glutMainLoop();

	// cleanup - calling 'aiReleaseImport' is important, as the library
	// keeps internal resources until the scene is freed again. Not
	// doing so can cause severe resource leaking.
	aiReleaseImport(scene);

	// We added a log stream to the library, it's our job to disable it
	// again. This will definitely release the last resources allocated
	// by Assimp.
	aiDetachAllLogStreams();
	return 0;
}
