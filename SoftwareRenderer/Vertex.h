#ifndef VERTEX_H
#define VERTEX_H

#include <stdio.h>

/* represents a vertex in homogenous coordinates */
class Vertex {
	private:
	public:
		float x, y, z, w;

		Vertex() {
			set(0, 0, 0);
		};

		Vertex(float _x, float _y, float _z) {
			set(_x, _y, _z);
		};

		Vertex(float _x, float _y, float _z, float _w) {
			x = _x;		y = _y;		z = _z;		w = _w;
		};

		void set(float _x, float _y, float _z) {
			x = _x;		y = _y;		z = _z;		w = 1;
		};

		/* for debugging */
		void print(void) {
			fprintf(stderr,"[%f %f %f %f]\n", x, y, z, w);
		}
};

#endif		/* VERTEX_H */s