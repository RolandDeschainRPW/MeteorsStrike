#pragma once
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

//Classe relativa ad un meteorite

class Meteorite {
	// Posizione del meteorite
	float xmeteorite;
	float ymeteorite;
	float zmeteorite;
	
	// Valori cubo per collisioni meteoriti
	double sizeCubeMeteorites;
	float posxCubeMeteorites;
	float posyCubeMeteorites;
	float poszCubeMeteorites;

	//Indice scene_list per visualizzazione
	int scene_listNum;

public:
	Meteorite(int selMeteor);
	float getPosx();
	float getPosy();
	float getPosz();
	float getPosxCube();
	float getPosyCube();
	float getPoszCube();
	double getSizeCube();
	int getSceneList();
};