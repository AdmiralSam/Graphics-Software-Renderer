#include "Triangle.h"




/* for openGL rendering of the triangle */
void Triangle :: renderOpenGL(void) {
	/* first bind the texture */
	if (tex) {
		tex->bind();
	}

	/* now draw the triangle */
	glBegin(GL_TRIANGLES);
		glTexCoord2fv(coords[0]);
		glColor3fv(c[0]);
		glVertex3f(v[0].x, v[0].y, v[0].z);

		glTexCoord2fv(coords[1]);
		glColor3fv(c[1]);
		glVertex3f(v[1].x, v[1].y, v[1].z);

		glTexCoord2fv(coords[2]);
		glColor3fv(c[2]);
		glVertex3f(v[2].x, v[2].y, v[2].z);
	glEnd();

	/* release the texture */
	if (tex) {
		tex->release();
	}

	return;
}