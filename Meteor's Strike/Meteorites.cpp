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

	visualizza = true;

	switch (selMeteor) {
	case 0:
		sizeSphereMeteorites = 0.25;
		posxSphereMeteorites = -17.52;
		posySphereMeteorites = 0.16;
		poszSphereMeteorites = 10.88;
		scene_listNum = 2;
		break;
	case 1:
		sizeSphereMeteorites = 0.36;
		posxSphereMeteorites = -20.64;
		posySphereMeteorites = 0.24;
		poszSphereMeteorites = 14.4;
		scene_listNum = 3;
		break;

	case 2:
		sizeSphereMeteorites = 0.55;
		posxSphereMeteorites = -20.56;
		posySphereMeteorites = 0.08;
		poszSphereMeteorites = 11.04;
		scene_listNum = 4;
		break;
	case 3:
		sizeSphereMeteorites = 0.45;
		posxSphereMeteorites = -17.2;
		posySphereMeteorites = 0.16;
		poszSphereMeteorites = 14.8;
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

float Meteorite::getPosz() {
	return this->zmeteorite;
}

float Meteorite::getPosxSphere() {
	return this->posxSphereMeteorites + xmeteorite;
}

float Meteorite::getPosySphere() {
	return this->posySphereMeteorites + ymeteorite;
}
float Meteorite::getPoszSphere() {
	return this->poszSphereMeteorites + zmeteorite;
}

double Meteorite::getSizeSphere() {
	return this->sizeSphereMeteorites;
}

int Meteorite::getSceneList() {
	return this->scene_listNum;
}

void Meteorite::setSizeSphere(double size) {
	this->sizeSphereMeteorites = size;
}

int Meteorite::getIndex() {
	return this->scene_listNum - 2;
}

void Meteorite::destroyed() {
	this->visualizza = false;
}

bool Meteorite::isDestroyed() {
	return this->visualizza;
}