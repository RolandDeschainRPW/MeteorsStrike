#include "Meteorites.h"

int random()
{
	int min = -10, max = +10;
	int random_integer;
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased

	return  random_integer = uni(rng);
}


Meteorite::Meteorite(int selMeteor) {
	this->xmeteorite = random();
	this->ymeteorite = random();
	this->zmeteorite = random();

	switch (selMeteor) {
	case 0:
		sizeCubeMeteorites = 1.2;
		posxCubeMeteorites = -17.6;
		posyCubeMeteorites = 0;
		poszCubeMeteorites = 10.96;
		scene_listNum = 2;
		break;
	case 1:
		sizeCubeMeteorites = 0.36;
		posxCubeMeteorites = -20.64;
		posyCubeMeteorites = 0.24;
		poszCubeMeteorites =14.4;
		scene_listNum = 3;
		break;

	case 2:
		sizeCubeMeteorites = 0.72;
		posxCubeMeteorites = -20.56;
		posyCubeMeteorites = 0.08;
		poszCubeMeteorites = 11.04;
		scene_listNum = 4;
		break;
	case 3:
		sizeCubeMeteorites = 0.64;
		posxCubeMeteorites = -17.2;
		posyCubeMeteorites = 0.08;
		poszCubeMeteorites = 14.8;
		scene_listNum = 5;
		break;
	}
}

float Meteorite::getPosx() {
	return this->xmeteorite;
}

float Meteorite::getPosy() {
	return this->ymeteorite;
}

float Meteorite::getPosz(){
	return this->zmeteorite;
}

float Meteorite::getPosxCube() {
	return this->posxCubeMeteorites + xmeteorite;
}

float Meteorite::getPosyCube() {
	return this->posyCubeMeteorites + ymeteorite;
}
float Meteorite::getPoszCube() {
	return this->poszCubeMeteorites + zmeteorite;
}

double Meteorite::getSizeCube() {
	return this->sizeCubeMeteorites;
}

int Meteorite::getSceneList() {
	return this->scene_listNum;
}