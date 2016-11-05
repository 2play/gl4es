#include "texgen.h"

//extern void* eglGetProcAddress(const char*);

void gl4es_glTexGeni(GLenum coord, GLenum pname, GLint param) {
    GLfloat params[4] = {0,0,0,0};
    params[0]=param;
    gl4es_glTexGenfv(coord, pname, params);
}

void gl4es_glTexGenfv(GLenum coord, GLenum pname, const GLfloat *param) {
    
    /*
    If pname is GL_TEXTURE_GEN_MODE, then the array must contain
    a single symbolic constant, one of
    GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP,
    or GL_REFLECTION_MAP.
    Otherwise, params holds the coefficients for the texture-coordinate
    generation function specified by pname.
    */

    //printf("glTexGenfv(%s, %s, [%s, ...]), texture=%i\n", PrintEnum(coord), PrintEnum(pname), PrintEnum(param[0]), glstate->texture.active);
    if ((glstate->list.compiling || glstate->gl_batch) && glstate->list.active) {
		NewStage(glstate->list.active, STAGE_TEXGEN);
		rlTexGenfv(glstate->list.active, coord, pname, param);
        noerrorShim();
		return;
	}

    // pname is in: GL_TEXTURE_GEN_MODE, GL_OBJECT_PLANE, GL_EYE_PLANE
    noerrorShim();
    switch(pname) {
        case GL_TEXTURE_GEN_MODE:
            switch (coord) {
                case GL_S: glstate->texgen[glstate->texture.active].S = param[0]; break;
                case GL_T: glstate->texgen[glstate->texture.active].T = param[0]; break;
                case GL_R: glstate->texgen[glstate->texture.active].R = param[0]; break;
                case GL_Q: glstate->texgen[glstate->texture.active].Q = param[0]; break;
                default:
                    errorShim(GL_INVALID_ENUM);
                return;
            }
        case GL_OBJECT_PLANE:
            switch (coord) {
                case GL_S:
                    memcpy(glstate->texgen[glstate->texture.active].S_O, param, 4 * sizeof(GLfloat));
                    break;
                case GL_T:
                    memcpy(glstate->texgen[glstate->texture.active].T_O, param, 4 * sizeof(GLfloat));
                    break;
                case GL_R:
                    memcpy(glstate->texgen[glstate->texture.active].R_O, param, 4 * sizeof(GLfloat));
                    break;
                case GL_Q:
                    memcpy(glstate->texgen[glstate->texture.active].Q_O, param, 4 * sizeof(GLfloat));
                    break;
                default:
                    errorShim(GL_INVALID_ENUM);
                return;
            }
        case GL_EYE_PLANE:
            switch (coord) {
                case GL_S:
                    memcpy(glstate->texgen[glstate->texture.active].S_E, param, 4 * sizeof(GLfloat));
                    break;
                case GL_T:
                    memcpy(glstate->texgen[glstate->texture.active].T_E, param, 4 * sizeof(GLfloat));
                    break;
                case GL_R:
                    memcpy(glstate->texgen[glstate->texture.active].R_E, param, 4 * sizeof(GLfloat));
                    break;
                case GL_Q:
                    memcpy(glstate->texgen[glstate->texture.active].Q_E, param, 4 * sizeof(GLfloat));
                    break;
                default:
                    errorShim(GL_INVALID_ENUM);
                return;
            }
        default:
            errorShim(GL_INVALID_ENUM);
    }
}
void gl4es_glGetTexGenfv(GLenum coord,GLenum pname,GLfloat *params) {
    if (gl_batch) flush();
    noerrorShim();
	switch(pname) {
		case GL_TEXTURE_GEN_MODE:
			switch (coord) {
				case GL_S: *params = glstate->texgen[glstate->texture.active].S; break;
				case GL_T: *params = glstate->texgen[glstate->texture.active].T; break;
				case GL_R: *params = glstate->texgen[glstate->texture.active].R; break;
				case GL_Q: *params = glstate->texgen[glstate->texture.active].Q; break;
				default: *params = GL_EYE_LINEAR;
			}
			break;
		case GL_OBJECT_PLANE:
			switch (coord) {
				case GL_S:
					memcpy(params, glstate->texgen[glstate->texture.active].S_O, 4 * sizeof(GLfloat));
					break;
				case GL_T:
					memcpy(params, glstate->texgen[glstate->texture.active].T_O, 4 * sizeof(GLfloat));
					break;
				case GL_R:
					memcpy(params, glstate->texgen[glstate->texture.active].R_O, 4 * sizeof(GLfloat));
					break;
				case GL_Q:
					memcpy(params, glstate->texgen[glstate->texture.active].Q_O, 4 * sizeof(GLfloat));
					break;
                default:
                    errorShim(GL_INVALID_ENUM);
			}
		case GL_EYE_PLANE:
			switch (coord) {
				case GL_S:
					memcpy(params, glstate->texgen[glstate->texture.active].S_E, 4 * sizeof(GLfloat));
					break;
				case GL_T:
					memcpy(params, glstate->texgen[glstate->texture.active].T_E, 4 * sizeof(GLfloat));
					break;
				case GL_R:
					memcpy(params, glstate->texgen[glstate->texture.active].R_E, 4 * sizeof(GLfloat));
					break;
				case GL_Q:
					memcpy(params, glstate->texgen[glstate->texture.active].Q_E, 4 * sizeof(GLfloat));
					break;
                default:
                    errorShim(GL_INVALID_ENUM);
			}
		break;
        default:
            errorShim(GL_INVALID_ENUM);
	}
}


GLfloat FASTMATH dot(const GLfloat *a, const GLfloat *b) {
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

GLfloat FASTMATH dot4(const GLfloat *a, const GLfloat *b) {
#ifdef __ARM_NEON__
    register float ret;
    asm volatile (
    "vld1.f32 {d0-d1}, [%1]        \n" //q0 = a(0..3)
    "vld1.f32 {d2-d3}, [%2]        \n" //q1 = b(0..3)
    "vmul.f32 q0, q0, q1           \n" //q0 = a(0)*b(0),a(1)*b(1),a(2)*b(2),a(3)*b(3)
    "vadd.f32 d0, d0, d1           \n" //d0 = a(0)*b(0)+a(2)*b(2),a(1)*b(1)+a(3)*b(3)
    "vpadd.f32 d0,d0               \n" //d0 = a(0)*b(0)+a(2)*b(2)+a(1)*b(1)+a(3)*b(3),a(0)*b(0)+a(2)*b(2)+a(1)*b(1)+a(3)*b(3)
    "vmov.f32 %0, s0               \n"
    :"=w"(ret): "r"(a), "r"(b)
    : "q0", "q1"
        );
    return ret;
#else
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
#endif
}

void matrix_vector(const GLfloat *a, const GLfloat *b, GLfloat *c) {
#ifdef __ARM_NEON__
    const float* a1 = a+8;
    asm volatile (
    "vld4.f32 {d0,d2,d4,d6}, [%1]        \n" 
    "vld4.f32 {d1,d3,d5,d7}, [%2]        \n" // q0-q3 = a(0,4,8,12/1,5,9,13/2,6,10,14/3,7,11,15)
    "vld1.f32 {q4}, [%3]       \n" // q4 = b
    "vmul.f32 q0, q0, d8[0]    \n" // q0 = a(0,4,8,12)*b[0]
    "vmla.f32 q0, q1, d8[1]    \n" // q0 = q0 + a(1,5,9,13)*b[1]
    "vmla.f32 q0, q2, d9[0]    \n" // q0 = q0 + a(2,6,10,14)*b[2]
    "vmla.f32 q0, q3, d9[1]    \n" // q0 = q0 + a(3,7,11,15)*b[3]
    "vst1.f32 {q0}, [%0]       \n"
    ::"r"(c), "r"(a), "r"(a1), "r"(b)
    : "q0", "q1", "q2", "q3", "q4", "memory"
        );
#else
    c[0] = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
    c[1] = a[4] * b[0] + a[5] * b[1] + a[6] * b[2] + a[7] * b[3];
    c[2] = a[8] * b[0] + a[9] * b[1] + a[10] * b[2] + a[11] * b[3];
    c[3] = a[12] * b[0] + a[13] * b[1] + a[14] * b[2] + a[15] * b[3];
#endif
}

void vector_matrix(const GLfloat *a, const GLfloat *b, GLfloat *c) {
#ifdef __ARM_NEON__
    const float* b2=b+4;
    const float* b3=b+8;
    const float* b4=b+12;
    asm volatile (
    "vld1.f32 {q0}, [%1]        \n" // %q0 = a(0..3)
    "vld1.f32 {q1}, [%2]        \n" // %q1 = b(0..3)
    "vmul.f32 q1, q1, d0[0]     \n" // %q1 = b(0..3)*a[0]
    "vld1.f32 {q2}, [%3]        \n" // %q2 = b(4..7)
    "vmla.f32 q1, q2, d0[1]     \n" // %q1 = %q1 + b(4..7)*a[1]
    "vld1.f32 {q2}, [%4]        \n" // %q2 = b(8..11)
    "vmla.f32 q1, q2, d1[0]     \n" // %q1 = %q1 + b(8..11)*a[2]
    "vld1.f32 {q2}, [%5]        \n" // %q2 = b(12..15)
    "vmla.f32 q1, q2, d1[1]     \n" // %q1 = %q1 + b(12..15)*a[3]
    "vst1.f32 {q1}, [%0]        \n"
    ::"r"(c), "r"(a), "r"(b), "r"(b2), "r"(b3), "r"(b4)
    : "%2", "q0", "q1", "q2", "memory"
        );
#else
    c[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
    c[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
    c[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
    c[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
#endif
}

void vector3_matrix(const GLfloat *a, const GLfloat *b, GLfloat *c) {
#ifdef __ARM_NEON__
    const float* b2=b+4;
    const float* b3=b+8;
    const float* b4=b+12;
    asm volatile (
    //"vld1.f32 {q0}, [%1]        \n" // %q0 = a(0..2)
    "vld1.32  {d4}, [%1]        \n"
    "flds     s10, [%1, #8]     \n"
    "vsub.f32 s11, s11, s11     \n"
    "vld1.f32 {q1}, [%2]        \n" // %q1 = b(0..3)
    "vmul.f32 q1, q1, d0[0]    \n" // %q1 = b(0..3)*a[0]
    "vld1.f32 {q2}, [%3]   \n" // %q2 = b(4..7)
    "vmla.f32 q1, q2, d0[1]    \n" // %q1 = %q1 + b(4..7)*a[1]
    "vld1.f32 {q2}, [%4]   \n" // %q2 = b(8..11)
    "vmla.f32 q1, q2, d1[0]    \n" // %q1 = %q1 + b(8..11)*a[2]
    "vld1.f32 {q2}, [%5]   \n" // %q2 = b(12..15)
    "vadd.f32 q1, q1, q2    \n" // %q1 = %q1 + b(12..15)
    "vst1.f32 {q1}, [%0]        \n"
    ::"r"(c), "r"(a), "r"(b), "r"(b2), "r"(b3), "r"(b4)
    : "q0", "q1", "q2", "memory"
        );
#else
    c[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + b[12];
    c[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + b[13];
    c[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + b[14];
    c[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + b[15];
#endif
}

void vector_normalize(GLfloat *a) {
#ifdef __ARM_NEON__
        asm volatile (
        "vld1.32                {d4}, [%0]                      \n\t"   //d4={x0,y0}
        "flds                   s10, [%0, #8]                   \n\t"   //d5[0]={z0}
        "vsub.f32               s11, s11, s11                   \n\t"

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
        "vst1.32                {d4}, [%0]                     	\n\t"   //
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

void vector4_normalize(GLfloat *a) {
#ifdef __ARM_NEON__
        asm volatile (
        "vld1.32                {q2}, [%0]                      \n\t"   //q2={x0,y0,z0,00}

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
        "vst1.32                {q2}, [%0]                    	\n\t"   //
        
        :"+&r"(a): 
    : "d0", "d1", "d2", "d3", "d4", "d5", "memory"
        );
#else
    float det=1.0f/sqrtf(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    a[0]*=det;
    a[1]*=det;
    a[2]*=det;
    // a[3] is ignored and left as 0.0f
#endif
}

void FASTMATH matrix_transpose(const GLfloat *a, GLfloat *b) {
    // column major -> row major
    // a(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15) -> b(0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15)
#ifdef __ARM_NEON__
   const float* a1 = a+8;
	float* b1=b+8;
    asm volatile (
    "vld4.f32 {d0,d2,d4,d6}, [%1]        \n" 
    "vld4.f32 {d1,d3,d5,d7}, [%2]        \n" // %q0-%q3 = a(0,4,8,12/1,5,9,13/2,6,10,14/3,7,11,15)
    "vst1.f32 {d0-d3}, [%0]        \n"
    "vst1.f32 {d4-d7}, [%3]        \n"
    ::"r"(b), "r"(a), "r"(a1), "r"(b1)
    : "q0", "q1", "q2", "q3", "memory"
        );
#else
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            b[i*4+j]=a[i+j*4];
#endif
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
        out[k*4] = dot4(verts+k*4, params);// + params[3];
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
    gl4es_glGetFloatv(GL_MODELVIEW_MATRIX, InvModelview);
    // column major -> row major
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            ModelviewMatrix[i*4+j]=InvModelview[i+j*4];
    // And get the inverse
    matrix_inverse(ModelviewMatrix, InvModelview);
    GLfloat eye[4], eye_norm[4], reflect[4];
    GLfloat a;
    for (int i=0; i<count; i++) {
	GLushort k = indices?indices[i]:i;
        matrix_vector(ModelviewMatrix, verts+k*4, eye);
        vector4_normalize(eye);
        vector3_matrix((norm)?(norm+k*3):glstate->normal, InvModelview, eye_norm);
        vector4_normalize(eye_norm);
        a=dot4(eye, eye_norm)*2.0f;
        for (int j=0; j<4; j++)
            reflect[j]=eye[j]-eye_norm[j]*a;
        reflect[2]+=1.0f;
        a = 0.5f / sqrtf(dot4(reflect, reflect));
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
    matrix_transpose(InvModelview, ModelviewMatrix);
    // And get the inverse
    matrix_inverse(ModelviewMatrix, InvModelview);
    GLfloat plane[4], tmp[4];
    vector_matrix(param, InvModelview, plane);
    for (int i=0; i<count; i++) {
	GLushort k = indices?indices[i]:i;
        matrix_vector(ModelviewMatrix, verts+k*4, tmp);
        out[k*4]=dot4(plane, tmp);
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
//printf("gen_tex_coords(%p, %p, %p, %d, %p, %d, %p, %d) texgen = S:%s T:%s R:%s Q:%s\n", verts, norm, *coords, count, needclean, texture, indices, ilen, (glstate->enable.texgen_s[texture])?PrintEnum(glstate->texgen[texture].S):"-", (glstate->enable.texgen_t[texture])?PrintEnum(glstate->texgen[texture].T):"-", (glstate->enable.texgen_r[texture])?PrintEnum(glstate->texgen[texture].R):"-", (glstate->enable.texgen_q[texture])?PrintEnum(glstate->texgen[texture].Q):"-");
    // TODO: do less work when called from glDrawElements?
    (*needclean) = 0;
    // special case : no texgen but texture activated, create a simple 1 repeated element
    if (!glstate->enable.texgen_s[texture] && !glstate->enable.texgen_t[texture] && !glstate->enable.texgen_r[texture] && !glstate->enable.texgen_q[texture]) {
        if ((*coords)==NULL) 
            *coords = (GLfloat *)malloc(count * 4 * sizeof(GLfloat));
        if (indices)
            for (int i=0; i<ilen; i++) {
                memcpy((*coords)+indices[i]*4, glstate->texcoord[texture], sizeof(GLfloat)*4);
            }
        else
            for (int i=0; i<count*4; i+=4) {
                memcpy((*coords)+i, glstate->texcoord[texture], sizeof(GLfloat)*4);
            }
        return;
    }
    // special case: SPHERE_MAP needs both texgen to make sense
    if ((glstate->enable.texgen_s[texture] && (glstate->texgen[texture].S==GL_SPHERE_MAP)) && (glstate->enable.texgen_t[texture] && (glstate->texgen[texture].T==GL_SPHERE_MAP)))
    {
	if (!glstate->enable.texture_2d[texture])
	    return;
	if ((*coords)==NULL) 
	    *coords = (GLfloat *)malloc(count * 4 * sizeof(GLfloat));
        sphere_loop(verts, norm, *coords, (indices)?ilen:count, indices);
        return;
    }
    // special case: REFLECTION_MAP  needs the 3 texgen to make sense
    if ((glstate->enable.texgen_s[texture] && (glstate->texgen[texture].S==GL_REFLECTION_MAP)) 
     && (glstate->enable.texgen_t[texture] && (glstate->texgen[texture].T==GL_REFLECTION_MAP))
     && (glstate->enable.texgen_r[texture] && (glstate->texgen[texture].R==GL_REFLECTION_MAP)))
    {
        *needclean=1;
        // setup reflection map!
        GLuint old_tex=glstate->texture.active;
        if (old_tex!=texture) gl4es_glActiveTexture(GL_TEXTURE0 + texture);
        LOAD_GLES_OES(glTexGeni);
        LOAD_GLES_OES(glTexGenfv);
        LOAD_GLES(glEnable);
        // setup cube map mode
        gles_glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        gles_glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        gles_glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
        // enable texgen
        gles_glEnable(GL_TEXTURE_GEN_STR);      //GLES only support the 3 gen at the same time!

        if (old_tex!=texture) gl4es_glActiveTexture(GL_TEXTURE0 + old_tex);
            
        return;
    }
    // special case: NORMAL_MAP  needs the 3 texgen to make sense
    if ((glstate->enable.texgen_s[texture] && (glstate->texgen[texture].S==GL_NORMAL_MAP)) 
     && (glstate->enable.texgen_t[texture] && (glstate->texgen[texture].T==GL_NORMAL_MAP))
     && (glstate->enable.texgen_r[texture] && (glstate->texgen[texture].R==GL_NORMAL_MAP)))
    {
        *needclean=1;
        // setup reflection map!
        GLuint old_tex=glstate->texture.active;
        if (old_tex!=texture) gl4es_glActiveTexture(GL_TEXTURE0 + texture);
        LOAD_GLES_OES(glTexGeni);
        LOAD_GLES_OES(glTexGenfv);
        LOAD_GLES(glEnable);
        // setup cube map mode
        gles_glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        gles_glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        gles_glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
        // enable texgen
        gles_glEnable(GL_TEXTURE_GEN_STR);

        if (old_tex!=texture) gl4es_glActiveTexture(GL_TEXTURE0 + old_tex);
            
        return;
    }
    if (!glstate->enable.texture_2d[texture])
	return;
    if ((*coords)==NULL) 
        *coords = (GLfloat *)malloc(count * 4 * sizeof(GLfloat));
    if (glstate->enable.texgen_s[texture])
        tex_coord_loop(verts, norm, (*coords), (indices)?ilen:count, glstate->texgen[texture].S, glstate->texgen[texture].S_O, glstate->texgen[texture].S_E, indices);
    if (glstate->enable.texgen_t[texture])
        tex_coord_loop(verts, norm, (*coords)+1, (indices)?ilen:count, glstate->texgen[texture].T, glstate->texgen[texture].T_O, glstate->texgen[texture].T_E, indices);
    if (glstate->enable.texgen_r[texture])
        tex_coord_loop(verts, norm, (*coords)+2, (indices)?ilen:count, glstate->texgen[texture].R, glstate->texgen[texture].R_O, glstate->texgen[texture].R_E, indices);
    else
        for (int i=0; i<((indices)?ilen:count); i++) {
            GLushort k = indices?indices[i]:i;
            (*coords)[k*4+2] = 0.0f;
        }
    if (glstate->enable.texgen_q[texture])
        tex_coord_loop(verts, norm, (*coords)+3, (indices)?ilen:count, glstate->texgen[texture].Q, glstate->texgen[texture].Q_O, glstate->texgen[texture].Q_E, indices);
    else
        for (int i=0; i<((indices)?ilen:count); i++) {
            GLushort k = indices?indices[i]:i;
            (*coords)[k*4+3] = 1.0f;
        }
}

void gen_tex_clean(GLint cleancode, int texture) {
	if (cleancode == 0)
		return;
	if (cleancode == 1) {
		GLuint old_tex=glstate->texture.active;
		LOAD_GLES(glDisable);
		gles_glDisable(GL_TEXTURE_GEN_STR);
		return;
	}
}

void gl4es_glLoadTransposeMatrixf(const GLfloat *m) {
	GLfloat mf[16];
	matrix_transpose(m, mf);
	gl4es_glLoadMatrixf(mf);
    errorGL();
}

void gl4es_glLoadTransposeMatrixd(const GLdouble *m) {
	GLfloat mf[16];
	for (int i=0; i<16; i++)
		mf[i] = m[i];
	gl4es_glLoadTransposeMatrixf(mf);
}

void gl4es_glMultTransposeMatrixd(const GLdouble *m) {
	GLfloat mf[16];
	for (int i=0; i<16; i++)
		mf[i] = m[i];
	gl4es_glMultTransposeMatrixf(mf);
}
void gl4es_glMultTransposeMatrixf(const GLfloat *m) {
	GLfloat mf[16];
	matrix_transpose(m, mf);
	gl4es_glMultMatrixf(mf);
    errorGL();
}

void glTexGenfv(GLenum coord, GLenum pname, const GLfloat *params) AliasExport("gl4es_glTexGenfv");
void glTexGeni(GLenum coord, GLenum pname, GLint param) AliasExport("gl4es_glTexGeni");
void glGetTexGenfv(GLenum coord,GLenum pname,GLfloat *params) AliasExport("gl4es_glGetTexGenfv");

void glLoadTransposeMatrixf(const GLfloat *m) AliasExport("gl4es_glLoadTransposeMatrixf");
void glLoadTransposeMatrixd(const GLdouble *m) AliasExport("gl4es_glLoadTransposeMatrixd");
void glMultTransposeMatrixd(const GLdouble *m) AliasExport("gl4es_glMultTransposeMatrixd");
void glMultTransposeMatrixf(const GLfloat *m) AliasExport("gl4es_glMultTransposeMatrixf");
