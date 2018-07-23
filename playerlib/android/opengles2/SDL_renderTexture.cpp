
static int texSubImage2D(GLenum target,
						 GLint xoffset, GLint yoffset,
						 GLsizei width, GLsizei height,
						 GLenum format, GLenum type,
						 const GLvoid *pixels, GLint pitch, GLint bpp) {
	Uint8 *blob = NULL;
	Uint8 *src;
	int src_pitch;
	int y;

	/* Reformat the texture data into a tightly packed array */
	src_pitch = width * bpp;
	src = (Uint8 *)pixels;
	if (pitch != src_pitch) {
		blob = (Uint8 *)FFL_malloc(src_pitch * height);
		if (!blob) {
			return 0;
		}
		src = blob;
		for (y = 0; y < height; ++y)
		{
			memcpy(src, pixels, src_pitch);
			src += src_pitch;
			pixels = (Uint8 *)pixels + pitch;
		}
		src = blob;
	}


	//glTexSubImage2D(target, 0, xoffset, yoffset, width, height, format, type, src);
	glTexImage2D(target,
				 0,
				 GL_LUMINANCE,
				 width,
	             height,
				 0,
				 GL_LUMINANCE,
				 GL_UNSIGNED_BYTE,
				 src);

   // glTexSubImage2D(target, 0, 100, 20, width-120, height-20, GL_LUMINANCE, GL_UNSIGNED_BYTE, src);

	if (blob) {
		FFL_free(blob);
	}
	return 0;
}

static bool uploadTexture_rgba(player::VideoTexture* tex,const SDL_Rect *rect,GLuint gltex[]) {
	const static int planesCont=1;
	const GLsizei xOffset[planesCont]   = { rect->x};
	const GLsizei yOffset[planesCont]   = { rect->y};
	const GLsizei widths[planesCont]    = { rect->w};
	const GLsizei heights[planesCont]   = { rect->h};
	const GLubyte *pixels[planesCont]   = { tex->mPixels[0]};

	GLenum format=GL_RGBA;
	GLenum type = GL_UNSIGNED_BYTE;;
	for(int32_t i=0;i<planesCont;i++) {
		glBindTexture(GL_TEXTURE_2D, gltex[i]);
		texSubImage2D(GL_TEXTURE_2D,
					  xOffset[i],
					  yOffset[i],
					  widths[i],
					  heights[i],
					  format,
					  type,
					  tex->mPixels[0],
					  tex->mPitches[0], 1);
	}

}
static bool uploadTexture_yuv(player::VideoTexture* tex,const SDL_Rect *rect,GLuint gltex[]){
	const static int planesCont=3;
	const GLsizei xOffset[planesCont]  = { rect->x, rect->x/2,rect->x/2 };
	const GLsizei yOffset[planesCont]  = { rect->y, rect->y/2,rect->y/2 };
	const GLsizei widths[planesCont]   = { rect->w, rect->w/2,rect->w/2 };
	const GLsizei heights[planesCont]  = { rect->h, rect->h/2,rect->h/2 };
	const GLubyte *pixels[planesCont]  = { tex->mPixels[0],  tex->mPixels[1],  tex->mPixels[2] };

	for(int32_t i=0;i<planesCont;i++) {
        glBindTexture(GL_TEXTURE_2D, gltex[i]);
		texSubImage2D(GL_TEXTURE_2D,
					  xOffset[i],
					  yOffset[i],
					  widths[i],
				      heights[i],
                      GL_LUMINANCE,
                      GL_UNSIGNED_BYTE,
					  tex->mPixels[i],
					  tex->mPitches[i], 1);
	}

	return true;
}
static GLboolean yuv420p_uploadTexture(player::VideoTexture* tex,GLuint gltex[]){
    int     planes[3]    = { 0, 1, 2 };
    const GLsizei widths[3]    = { tex->mPitches[0], tex->mPitches[1], tex->mPitches[2] };
    const GLsizei heights[3]   = { tex->mHeight,          tex->mHeight / 2,      tex->mHeight / 2 };
    const GLubyte *pixels[3]   = { tex->mPixels[0],  tex->mPixels[1],  tex->mPixels[2] };


    for (int i = 0; i < 3; ++i) {
        int plane = planes[i];

        glBindTexture(GL_TEXTURE_2D, gltex[i]);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_LUMINANCE,
                     widths[plane],
                     heights[plane],
                     0,
                     GL_LUMINANCE,
                     GL_UNSIGNED_BYTE,
                     pixels[plane]);
    }

    return GL_TRUE;
}