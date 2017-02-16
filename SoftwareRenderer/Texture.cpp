#include <atlimage.h>

#include "globals.h"

#include "Texture.h"

extern bool bilerp_flag;

/* this function copies the color buffer to the given CImage */
void Texture :: copyTextureData(CImage *image) {
	int i, j;
	int pitch = image->GetPitch();
	u08 *ptr_src = (u08 *)image->GetBits();
	u08 *ptr_dst = data + (height - 1) * width * 3 ;


	/* note that CImage is upside down, so we start reading the framebuffer
	   from the top of the screen */

	for (j = 0; j < height; j++) {
		for (i = 0; i < width; i++) {
			*(ptr_dst++) = *(ptr_src + 2);
			*(ptr_dst++) = *(ptr_src + 1);
			*(ptr_dst++) = *(ptr_src);

			ptr_src += 3;
		}

		ptr_dst += -(width * 3) * 2;
		ptr_src += pitch - (width * 3); 
	}

	return;
}


Texture :: Texture(char *name) {
	CImage image;

	image.Load(name);

	if (image.IsNull()) {
		fprintf(stderr,"ERROR! Could not read texture %s\n", name);
		fflush(stderr);
		exit(-1);
	}

	/* now copy the image into the data array */
	width = image.GetWidth();
	height = image.GetHeight();

	/* make room for the data array */
	data = (u08 *)malloc(sizeof(u08) * width * height * 3);

	/* now copy the contents from one to the other */
	copyTextureData(&image);

	/* finally cleanup after myself */
	image.Destroy();

	/* if opengl i should bind it and all that */
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	if (bilerp_flag) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);


	return;
};

/* switches between nearest neighbor and bilinear interpolation */
void Texture :: switchTextureFiltering(bool flag) {

	bilerp_flag = flag;

	glBindTexture(GL_TEXTURE_2D, tex);
	if (bilerp_flag) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	return;
}