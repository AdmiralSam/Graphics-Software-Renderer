#include <windows.h>
#include <stdio.h>

#include "gl.h"
#include "glu.h"
#include "glut.h"

#include "FrameBuffer.h"
#include "mouse.h"
#include "Timer.h"
#include "globals.h"
#include "Texture.h"
#include "Scene.h"

extern FrameBuffer fb;
extern Scene scene;

bool opengl_test = true;
bool bilerp_flag = false;
int shader_mode = 0;

Timer timer1;

/* this function checks for GL errors that might have cropped up and 
   breaks the program if they have */
void checkGLErrors(char *prefix) {
	GLenum error;
	if ((error = glGetError()) != GL_NO_ERROR) {
		fprintf(stderr,"%s: found gl error: %s\n",prefix, gluErrorString(error));
		exit(-1);
	}
}

void display(void) {
	double time;
	char name[128];

	timer1.startTimer();

	/* first set up the matrices */
	computeModelViewMatrix();
	computeProjectionMatrix();

	if (opengl_test) {
		/* draw the scene with opengl */
		scene.renderSceneOpenGL();
	}
	else {
		/* draw the scene with your software implementation.  you will need to fill the framebuffer with
		   the appropriate color data */
		scene.renderSceneSoftware();
		/* now take the framebuffer you rendered and dump it to the screen */
		fb.dumpToScreen();
	}

	glFinish();

	timer1.stopTimer();
	time = timer1.getTime();
	sprintf(name, "%s: %.4lf s, %.2lf fps", opengl_test ? "GL" : "soft", time, 1.0/time);
	glutSetWindowTitle(name);

	checkGLErrors("Errors in display()!\n");

	glutSwapBuffers();
}

void redisplay(void) {
    glutPostRedisplay();
	return;
}

extern int num_tex;
extern Texture **tex;
void switchTextureFiltering(void) {
	int i;
	for (i = 0; i < num_tex; i++) {
		tex[i]->switchTextureFiltering(bilerp_flag);
	}

	return;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 27:
		case 'q':
		case 'Q':
			/* quit the program */
			exit(0);
			break;
		case ' ':
			/* toggle between opengl and sofware rendering */
			opengl_test = !opengl_test;
			redisplay();
			break;
		case 'w':
			moveForward();
			redisplay();
			break;
		case 'a':
			moveLeft();
			redisplay();
			break;
		case 'd':
			moveRight();
			redisplay();
			break;
		case 's':
			moveBack();
			redisplay();
			break;
		case 'b':
			/* toggle bilinear interpolation of the textures */
			bilerp_flag = !bilerp_flag;
			switchTextureFiltering();
			redisplay();
		case '1': // Texture Shader
		case '2': // Color Shader
		case '3': // Checkered Shader
			shader_mode = key - '1';
			break;
		default:
			break;
	}

	return;
}


void initGLUT(int argc, char **argv) {
    /* Initialize GLUT */
    glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowSize(window_width, window_height);
	glutCreateWindow("window_name");
    glutInitWindowPosition(100,50);

	glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutReshapeFunc(reshape);
	
	/* set an idle function */
	glutIdleFunc(redisplay);

	/* init the mouse */
	initMouse();

	loadScene(argv[1]);

    /* Enter main loop */
	glutMainLoop();

	return;
}