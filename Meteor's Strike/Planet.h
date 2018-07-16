#pragma once


class Planet {
	double sizeSphere;
	float posxSphere;
	float posySphere;
	float poszSphere;

public:
	Planet(double size, float x, float y, float z) {
		sizeSphere = size;
		posxSphere = x;
		posySphere = y;
		poszSphere = z;
	}

	double getSize() {
		return sizeSphere;
	}

	float getx() {
		return posxSphere;
	}

	float gety() {
		return posySphere;
	}

	float getz() {
		return poszSphere;
	}
};
