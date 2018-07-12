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


Meteorite::Meteorite() {
	this->xmeteorite = random();
	this->ymeteorite = random();
	this->zmeteorite = random();
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

double::Meteorite::getSizeCube() {
	return this->sizeCubeMeteorites;
}