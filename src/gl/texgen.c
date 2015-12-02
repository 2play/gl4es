#include "texgen.h"

//extern void* eglGetProcAddress(const char*);

void glTexGeni(GLenum coord, GLenum pname, GLint param) {
    GLfloat params[4] = {0,0,0,0};
    params[0]=param;
    glTexGenfv(coord, pname, params);
}

void glTexGenfv(GLenum coord, GLenum pname, const GLfloat *param) {
    
    /*
    If pname is GL_TEXTURE_GEN_MODE, then the array must contain
    a single symbolic constant, one of
    GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP,
    or GL_REFLECTION_MAP.
    Otherwise, params holds the coefficients for the texture-coordinate
    generation function specified by pname.
    */

    //printf("glTexGenfv(0x%04X, 0x%04X, [%.02f, ...]), texture=%i\n", coord, pname, param[0], state.texture.active);
    if ((state.list.compiling || state.gl_batch) && state.list.active) {
		NewStage(state.list.active, STAGE_TEXGEN);
		rlTexGenfv(state.list.active, coord, pname, param);
        noerrorShim();
		return;
	}

    // pname is in: GL_TEXTURE_GEN_MODE, GL_OBJECT_PLANE, GL_EYE_PLANE
    noerrorShim();
    switch(pname) {
        case GL_TEXTURE_GEN_MODE:
            switch (coord) {
                case GL_S: state.texgen[state.texture.active].S = param[0]; break;
                case GL_T: state.texgen[state.texture.active].T = param[0]; break;
                case GL_R: state.texgen[state.texture.active].R = param[0]; break;
                default:
                    errorShim(GL_INVALID_ENUM);
                return;
            }
        case GL_OBJECT_PLANE:
            switch (coord) {
                case GL_S:
                    memcpy(state.texgen[state.texture.active].S_O, param, 4 * sizeof(GLfloat));
                    break;
                case GL_T:
                    memcpy(state.texgen[state.texture.active].T_O, param, 4 * sizeof(GLfloat));
                    break;
                case GL_R:
                    memcpy(state.texgen[state.texture.active].R_O, param, 4 * sizeof(GLfloat));
                    break;
                default:
                    errorShim(GL_INVALID_ENUM);
                return;
            }
        case GL_EYE_PLANE:
            switch (coord) {
                case GL_S:
                    memcpy(state.texgen[state.texture.active].S_E, param, 4 * sizeof(GLfloat));
                    break;
                case GL_T:
                    memcpy(state.texgen[state.texture.active].T_E, param, 4 * sizeof(GLfloat));
                    break;
                case GL_R:
                    memcpy(state.texgen[state.texture.active].R_E, param, 4 * sizeof(GLfloat));
                    break;
                default:
                    errorShim(GL_INVALID_ENUM);
                return;
            }
        default:
            errorShim(GL_INVALID_ENUM);
    }
}
void glGetTexGenfv(GLenum coord,GLenum pname,GLfloat *params) {
    if (gl_batch) flush();
    noerrorShim();
	switch(pname) {
		case GL_TEXTURE_GEN_MODE:
			switch (coord) {
				case GL_S: *params = state.texgen[state.texture.active].S; break;
				case GL_T: *params = state.texgen[state.texture.active].T; break;
				case GL_R: *params = state.texgen[state.texture.active].R; break;
				default: *params = GL_EYE_LINEAR;
			}
			break;
		case GL_OBJECT_PLANE:
			switch (coord) {
				case GL_S:
					memcpy(params, state.texgen[state.texture.active].S_O, 4 * sizeof(GLfloat));
					break;
				case GL_T:
					memcpy(params, state.texgen[state.texture.active].T_O, 4 * sizeof(GLfloat));
					break;
				case GL_R:
					memcpy(params, state.texgen[state.texture.active].R_O, 4 * sizeof(GLfloat));
					break;
                default:
                    errorShim(GL_INVALID_ENUM);
			}
		case GL_EYE_PLANE:
			switch (coord) {
				case GL_S:
					memcpy(params, state.texgen[state.texture.active].S_E, 4 * sizeof(GLfloat));
					break;
				case GL_T:
					memcpy(params, state.texgen[state.texture.active].T_E, 4 * sizeof(GLfloat));
					break;
				case GL_R:
					memcpy(params, state.texgen[state.texture.active].R_E, 4 * sizeof(GLfloat));
					break;
                default:
                    errorShim(GL_INVALID_ENUM);
			}
		break;
        default:
            errorShim(GL_INVALID_ENUM);
	}
}


GLfloat dot(const GLfloat *a, const GLfloat *b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

//TODO: NEONize all thoses functions, maybe also making the vector an array of 4 float can help.
void matrix_vector(const GLfloat *a, const GLfloat *b, GLfloat *c) {
    c[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3];
    c[1] = a[4] * b[0] + a[5] * b[1] + a[6] * b[2] + a[7];
    c[2] = a[8] * b[0] + a[9] * b[1] + a[10] * b[2] + a[11];
}

void vector_matrix(const GLfloat *a, const GLfloat *b, GLfloat *c) {
    c[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + b[12];
    c[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + b[13];
    c[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + b[14];
}

void vector_normalize(GLfloat *a) {
#ifdef __ARM_NEON__
        asm volatile (
        "vld1.32                {d4}, [%0]                      \n\t"   //d4={x0,y0}
        "flds                   s10, [%0, #8]                   \n\t"   //d5[0]={z0}

        "vmul.f32               d0, d4, d4                      \n\t"   //d0= d4*d4
        "vpadd.f32              d0, d0                          \n\t"   //d0 = d[0] + d[1]
        "vmla.f32               d0, d5, d5                      \n\t"   //d0 = d0 + d5*d5 
        
        "vmov.f32               d1, d0                          \n\t"   //d1 = d0
        "vrsqrte.f32    		d0, d0                          \n\t"   //d0 = ~ 1.0 / sqrt(d0)
        "vmul.f32               d2, d0, d1                      \n\t"   //d2 = d0 * d1
        "vrsqrts.f32    		d3, d2, d0                      \n\t"   //d3 = (3 - d0 * d2) / 2        
        "vmul.f32               d0, d0, d3                      \n\t"   //d0 = d0 * d3
/*        "vmul.f32               d2, d0, d1                      \n\t"   //d2 = d0 * d1  
        "vrsqrts.f32    		d3, d2, d0                      \n\t"   //d4 = (3 - d0 * d3) / 2        
        "vmul.f32               d0, d0, d3                      \n\t"   //d0 = d0 * d4  */  // 1 iteration should be enough

        "vmul.f32               q2, q2, d0[0]                   \n\t"   //d0= d2*d4
        "vst1.32                d4, [%0]                      	\n\t"   //
        "fsts                   s10, [%0, #8]                   \n\t"   //
        
        :"+&r"(a): 
    : "d0", "d1", "d2", "d3", "d4", "d5", "memory"
        );
#else
    float det=1.0f/sqrtf(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]*=det;
    a[1]*=det;
    a[2]*=det;
#endif
}

void matrix_column_row(const GLfloat *a, GLfloat *b) {
    // column major -> row major
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            b[i*4+j]=a[i+j*4];
}

void matrix_row_column(const GLfloat *a, GLfloat *b) {
    // row major -> column major
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            b[i+j*4]=a[i*4+j];
}

void matrix_inverse(const GLfloat *m, GLfloat *r) {

    r[0] = m[5]*m[10]*m[15] - m[5]*m[14]*m[11] - m[6]*m[9]*m[15] + m[6]*m[13]*m[11] + m[7]*m[9]*m[14] - m[7]*m[13]*m[10];
    r[1] = -m[1]*m[10]*m[15] + m[1]*m[14]*m[11] + m[2]*m[9]*m[15] - m[2]*m[13]*m[11] - m[3]*m[9]*m[14] + m[3]*m[13]*m[10];
    r[2] = m[1]*m[6]*m[15] - m[1]*m[14]*m[7] - m[2]*m[5]*m[15] + m[2]*m[13]*m[7] + m[3]*m[5]*m[14] - m[3]*m[13]*m[6];
    r[3] = -m[1]*m[6]*m[11] + m[1]*m[10]*m[7] + m[2]*m[5]*m[11] - m[2]*m[9]*m[7] - m[3]*m[5]*m[10] + m[3]*m[9]*m[6];

    r[4] = -m[4]*m[10]*m[15] + m[4]*m[14]*m[11] + m[6]*m[8]*m[15] - m[6]*m[12]*m[11] - m[7]*m[8]*m[14] + m[7]*m[12]*m[10];
    r[5] = m[0]*m[10]*m[15] - m[0]*m[14]*m[11] - m[2]*m[8]*m[15] + m[2]*m[12]*m[11] + m[3]*m[8]*m[14] - m[3]*m[12]*m[10];
    r[6] = -m[0]*m[6]*m[15] + m[0]*m[14]*m[7] + m[2]*m[4]*m[15] - m[2]*m[12]*m[7] - m[3]*m[4]*m[14] + m[3]*m[12]*m[6];
    r[7] = m[0]*m[6]*m[11] - m[0]*m[10]*m[7] - m[2]*m[4]*m[11] + m[2]*m[8]*m[7] + m[3]*m[4]*m[10] - m[3]*m[8]*m[6];

    r[8] = m[4]*m[9]*m[15] - m[4]*m[13]*m[11] - m[5]*m[8]*m[15] + m[5]*m[12]*m[11] + m[7]*m[8]*m[13] - m[7]*m[12]*m[9];
    r[9] = -m[0]*m[9]*m[15] + m[0]*m[13]*m[11] + m[1]*m[8]*m[15] - m[1]*m[12]*m[11] - m[3]*m[8]*m[13] + m[3]*m[12]*m[9];
    r[10] = m[0]*m[5]*m[15] - m[0]*m[13]*m[7] - m[1]*m[4]*m[15] + m[1]*m[12]*m[7] + m[3]*m[4]*m[13] - m[3]*m[12]*m[5];
    r[11] = -m[0]*m[5]*m[11] + m[0]*m[9]*m[7] + m[1]*m[4]*m[11] - m[1]*m[8]*m[7] - m[3]*m[4]*m[9] + m[3]*m[8]*m[5];

    r[12] = -m[4]*m[9]*m[14] + m[4]*m[13]*m[10] + m[5]*m[8]*m[14] - m[5]*m[12]*m[10] - m[6]*m[8]*m[13] + m[6]*m[12]*m[9];
    r[13] = m[0]*m[9]*m[14] - m[0]*m[13]*m[10] - m[1]*m[8]*m[14] + m[1]*m[12]*m[10] + m[2]*m[8]*m[13] - m[2]*m[12]*m[9];
    r[14] = -m[0]*m[5]*m[14] + m[0]*m[13]*m[6] + m[1]*m[4]*m[14] - m[1]*m[12]*m[6] - m[2]*m[4]*m[13] + m[2]*m[12]*m[5];
    r[15] = m[0]*m[5]*m[10] - m[0]*m[9]*m[6] - m[1]*m[4]*m[10] + m[1]*m[8]*m[6] + m[2]*m[4]*m[9] - m[2]*m[8]*m[5];

    GLfloat det = 1/(m[0]*r[0] + m[1]*r[4] + m[2]*r[8] + m[3]*r[12]);
    for (int i = 0; i < 16; i++) r[i] *= det;
}

void matrix_mul(const GLfloat *a, const GLfloat *b, GLfloat *c) {
    memset(c, 0, sizeof(GLfloat)*16);
// c = a * b
    for (int j=0 ; j<4; ++j) {
        for (int i=0 ; i<4; ++i) {
            for (int k=0; k<4; ++k) {
                c[i*4+j] += a[k*4+j] * b[i*4+k]; 
            }
        }
    }
}

void dot_loop(const GLfloat *verts, const GLfloat *params, GLfloat *out, GLint count, GLushort *indices) {
    for (int i = 0; i < count; i++) {
	GLushort k = indices?indices[i]:i;
        out[k*4] = dot(verts+k*3, params) + params[3];
    }
}

void sphere_loop(const GLfloat *verts, const GLfloat *norm, GLfloat *out, GLint count, GLushort *indices) {
    // based on https://www.opengl.org/wiki/Mathematics_of_glTexGen
/*    if (!norm) {
        printf("LIBGL: GL_SPHERE_MAP without Normals\n");
        return;
    }*/
    // First get the ModelviewMatrix
    GLfloat ModelviewMatrix[16], InvModelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, InvModelview);
    // column major -> row major
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            ModelviewMatrix[i*4+j]=InvModelview[i+j*4];
    // And get the inverse
    matrix_inverse(ModelviewMatrix, InvModelview);
    GLfloat eye[3], eye_norm[3], reflect[3];
    GLfloat a;
    for (int i=0; i<count; i++) {
	GLushort k = indices?indices[i]:i;
        matrix_vector(ModelviewMatrix, verts+k*3, eye);
        vector_normalize(eye);
        vector_matrix((norm)?(norm+k*3):state.normal, InvModelview, eye_norm);
        vector_normalize(eye_norm);
        a=dot(eye, eye_norm)*2.0f;
        for (int j=0; j<3; j++)
            reflect[j]=eye[j]-eye_norm[j]*a;
        reflect[2]+=1.0f;
        a = 1.0f / (2.0f*sqrtf(dot(reflect, reflect)));
        out[k*4+0] = reflect[0]*a + 0.5f;
        out[k*4+1] = reflect[1]*a + 0.5f;
        out[k*4+2] = 0.0f;
        out[k*4+3] = 1.0f;
    }

}

void eye_loop(const GLfloat *verts, const GLfloat *param, GLfloat *out, GLint count, GLushort *indices) {
    // based on https://www.opengl.org/wiki/Mathematics_of_glTexGen
    // First get the ModelviewMatrix
    GLfloat ModelviewMatrix[16], InvModelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, InvModelview);
    // column major -> row major
    matrix_column_row(InvModelview, ModelviewMatrix);
    // And get the inverse
    matrix_inverse(ModelviewMatrix, InvModelview);
    GLfloat plane[3], tmp[3];
    vector_matrix(param, InvModelview, plane);
    for (int i=0; i<count; i++) {
	GLushort k = indices?indices[i]:i;
        matrix_vector(ModelviewMatrix, verts+k*3, tmp);
        out[k*4]=dot(plane, tmp);
    }

}

static inline void tex_coord_loop(GLfloat *verts, GLfloat *norm, GLfloat *out, GLint count, GLenum type, GLfloat *param_o, GLfloat *param_e, GLushort *indices) {
    switch (type) {
        case GL_OBJECT_LINEAR:
            dot_loop(verts, param_o, out, count, indices);
            break;
        case GL_EYE_LINEAR:
            eye_loop(verts, param_e, out, count, indices);
            break;
        case GL_SPHERE_MAP:
            //printf("LIBGL: GL_SPHERE_MAP with only 1 TexGen available");  //Broken here
            break;
    }
}

void gen_tex_coords(GLfloat *verts, GLfloat *norm, GLfloat **coords, GLint count, GLint *needclean, int texture, GLushort *indices, GLuint ilen) {
    // TODO: do less work when called from glDrawElements?
    (*needclean) = 0;
    // special case : no texgen but texture activated, create a simple 1 repeated element
    if (!state.enable.texgen_s[texture] && !state.enable.texgen_t[texture] && !state.enable.texgen_r[texture]) {
	if ((*coords)==NULL) 
	    *coords = (GLfloat *)malloc(count * 2 * sizeof(GLfloat));
	if (indices)
	    for (int i=0; i<ilen; i++) {
		memcpy((*coords)+indices[i]*4, state.texcoord[texture], sizeof(GLfloat)*4);
	    }
	else
	    for (int i=0; i<count*4; i+=4) {
		memcpy((*coords)+i, state.texcoord[texture], sizeof(GLfloat)*4);
	    }
	return;
    }
    // special case: SPHERE_MAP needs both texgen to make sense
    if ((state.enable.texgen_s[texture] && (state.texgen[texture].S==GL_SPHERE_MAP)) && (state.enable.texgen_t[texture] && (state.texgen[texture].T==GL_SPHERE_MAP)))
    {
	if (!state.enable.texture_2d[texture])
	    return;
	if ((*coords)==NULL) 
	    *coords = (GLfloat *)malloc(count * 4 * sizeof(GLfloat));
        sphere_loop(verts, norm, *coords, (indices)?ilen:count, indices);
        return;
    }
    // special case: REFLECTION_MAP  needs the 3 texgen to make sense
    if ((state.enable.texgen_s[texture] && (state.texgen[texture].S==GL_REFLECTION_MAP)) 
     && (state.enable.texgen_t[texture] && (state.texgen[texture].T==GL_REFLECTION_MAP))
     && (state.enable.texgen_r[texture] && (state.texgen[texture].R==GL_REFLECTION_MAP)))
    {
        *needclean=1;
        // setup reflection map!
        GLuint old_tex=state.texture.active;
        if (old_tex!=texture) glActiveTexture(GL_TEXTURE0 + texture);
        LOAD_GLES_OES(glTexGeni);
        LOAD_GLES_OES(glTexGenfv);
        LOAD_GLES(glEnable);
        // setup cube map mode
        gles_glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        gles_glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        gles_glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        // enable texgen
        gles_glEnable(GL_TEXTURE_GEN_STR);      //GLES only support the 3 gen at the same time!

        if (old_tex!=texture) glActiveTexture(GL_TEXTURE0 + old_tex);
            
        return;
    }
    // special case: NORMAL_MAP  needs the 3 texgen to make sense
    if ((state.enable.texgen_s[texture] && (state.texgen[texture].S==GL_NORMAL_MAP)) 
     && (state.enable.texgen_t[texture] && (state.texgen[texture].T==GL_NORMAL_MAP))
     && (state.enable.texgen_r[texture] && (state.texgen[texture].R==GL_NORMAL_MAP)))
    {
        *needclean=1;
        // setup reflection map!
        GLuint old_tex=state.texture.active;
        if (old_tex!=texture) glActiveTexture(GL_TEXTURE0 + texture);
        LOAD_GLES_OES(glTexGeni);
        LOAD_GLES_OES(glTexGenfv);
        LOAD_GLES(glEnable);
        // setup cube map mode
        gles_glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        gles_glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        gles_glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        // enable texgen
        gles_glEnable(GL_TEXTURE_GEN_STR);

        if (old_tex!=texture) glActiveTexture(GL_TEXTURE0 + old_tex);
            
        return;
    }
    if (!state.enable.texture_2d[texture])
	return;
    if ((*coords)==NULL) 
        *coords = (GLfloat *)malloc(count * 2 * sizeof(GLfloat));
    if (state.enable.texgen_s[texture])
        tex_coord_loop(verts, norm, *coords, (indices)?ilen:count, state.texgen[texture].S, state.texgen[texture].S_O, state.texgen[texture].S_E, indices);
    if (state.enable.texgen_t[texture])
        tex_coord_loop(verts, norm, *coords+1, (indices)?ilen:count, state.texgen[texture].T, state.texgen[texture].T_O, state.texgen[texture].T_E, indices);
    for (int i=0; i<count; i++) {
        GLushort k = indices?indices[i]:i;
        (*coords)[k*4+2] = 0.0f;
        (*coords)[k*4+3] = 1.0f;
    }
}

void gen_tex_clean(GLint cleancode, int texture) {
	if (cleancode == 0)
		return;
	if (cleancode == 1) {
		GLuint old_tex=state.texture.active;
		if (old_tex!=texture) glActiveTexture(GL_TEXTURE0 + texture);
		LOAD_GLES(glDisable);
		gles_glDisable(GL_TEXTURE_GEN_STR);
		if (old_tex!=texture) glActiveTexture(GL_TEXTURE0 + old_tex);
		return;
	}
}

void glLoadTransposeMatrixf(const GLfloat *m) {
	GLfloat mf[16];
	matrix_row_column(m, mf);
	glLoadMatrixf(mf);
    errorGL();
}

void glLoadTransposeMatrixd(const GLdouble *m) {
	GLfloat mf[16];
	for (int i=0; i<16; i++)
		mf[i] = m[i];
	glLoadTransposeMatrixf(mf);
}

void glMultTransposeMatrixd(const GLdouble *m) {
	GLfloat mf[16];
	for (int i=0; i<16; i++)
		mf[i] = m[i];
	glMultTransposeMatrixf(mf);
}
void glMultTransposeMatrixf(const GLfloat *m) {
	GLfloat mf[16];
	matrix_row_column(m, mf);
	glMultMatrixf(mf);
    errorGL();
}
