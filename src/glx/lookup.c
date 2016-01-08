#ifdef ANDROID
#include "../gl/gl.h"
#endif
#include "glx.h"

#include "../gl/directstate.h"
//#define DEBUG_ADDRESS

#ifdef DEBUG_ADDRESS
#define MAP(func_name, func) \
    if(cnt==1) {if ((uint32_t)((void*)func) <0x4000000) printf("glxGetProcAddress %s = %p\n", func_name, (void*)func);} if (strcmp(name, func_name) == 0) return (void *)func;
#else
#define MAP(func_name, func) \
    if (strcmp(name, func_name) == 0) return (void *)func;
#endif

#define MAP_EGL(func_name, egl_func) \
    MAP(#func_name, egl_eglGetProcAddress(#egl_func))

#define EX(func_name) MAP(#func_name, func_name)

#define ARB(func_name) MAP(#func_name "ARB", func_name)

#define EXT(func_name) MAP(#func_name "EXT", func_name)

#define _EX(func_name) MAP(#func_name, glshim_ ## func_name)

#define _ARB(func_name) MAP(#func_name "ARB", glshim_ ## func_name)

#define _EXT(func_name) MAP(#func_name "EXT", glshim_ ## func_name)

#define STUB(func_name)                       \
    if (strcmp(name, #func_name) == 0) {      \
        printf("glX stub: %s\n", #func_name); \
        return (void *)glXStub;               \
    }

void glXStub(void *x, ...) {
    return;
}

void *glXGetProcAddressARB(const char *name) {
    LOAD_EGL(eglGetProcAddress);
#ifdef DEBUG_ADDRESS
    static int cnt = 0;
    cnt++;
#endif
    // generated gles wrappers
#ifdef USE_ES2
    #include "gles2funcs.inc"
#else
    #include "glesfuncs.inc"
#endif

#ifndef ANDROID
    // glX calls
    EX(glXChooseVisual);
    EX(glXCopyContext);
    EX(glXCreateContext);
    EX(glXCreateNewContext);
	EX(glXCreateContextAttribsARB);
    EX(glXCreateGLXPixmap);
    EX(glXDestroyContext);
    EX(glXDestroyGLXPixmap);
    EX(glXGetConfig);
    EX(glXGetCurrentDisplay);
    EX(glXGetCurrentDrawable);
    EX(glXIsDirect);
    EX(glXMakeCurrent);
    EX(glXMakeContextCurrent);
    EX(glXQueryExtensionsString);
    EX(glXQueryServerString);
    EX(glXSwapBuffers);
    EX(glXSwapIntervalEXT);
#endif //ANDROID
    EX(glXSwapIntervalMESA);
    EX(glXSwapIntervalSGI);
#ifndef ANDROID
    EX(glXUseXFont);
    EX(glXWaitGL);
    EX(glXWaitX);
    EX(glXGetCurrentContext);
    EX(glXQueryExtension);
    EX(glXQueryDrawable);
    EX(glXQueryVersion);
    EX(glXGetClientString);
    EX(glXGetFBConfigs);
    EX(glXChooseFBConfig);
    EX(glXChooseFBConfigSGIX);
    EX(glXGetFBConfigAttrib);
    EX(glXQueryContext);
    EX(glXGetVisualFromFBConfig);
    EX(glXCreateWindow);
    EX(glXDestroyWindow);
    
    STUB(glXCreatePbuffer); // to do, using Renderbuffers....
    STUB(glXDestroyPbuffer);
    STUB(glXCreatePixmap);
    STUB(glXDestroyPixmap);
    STUB(glXGetCurrentReadDrawable);
    STUB(glXGetSelectedEvent);
    STUB(glXSelectEvent);
#endif //ANDROID
    
    // GL_ARB_vertex_buffer_object
    _ARB(glBindBuffer);
    _ARB(glBufferData);
    _ARB(glBufferSubData);
    _ARB(glDeleteBuffers);
    _ARB(glGenBuffers);
    _ARB(glIsBuffer);
    _EX(glGetBufferPointerv);
    _ARB(glGetBufferPointerv);
    _EX(glMapBuffer);
    _EX(glUnmapBuffer);
    _ARB(glMapBuffer);
    _ARB(glUnmapBuffer);
    _ARB(glGetBufferParameteriv);
    _EX(glGetBufferSubData);
    _ARB(glGetBufferSubData);

    // GL_ARB_vertex_array_object
    _EX(glGenVertexArrays);
    _EX(glBindVertexArray);
    _EX(glDeleteVertexArrays);
    _EX(glIsVertexArray);
    _ARB(glGenVertexArrays);
    _ARB(glBindVertexArray);
    _ARB(glDeleteVertexArrays);
    _ARB(glIsVertexArray);
    
    // GL_ARB_frameBuffer_ext
    _EX(glFramebufferTexture1D);
    _EX(glFramebufferTexture3D);
    _EX(glFramebufferTextureLayer);
    _EX(glRenderbufferStorageMultisample);
    _EX(glBlitFramebuffer);
    _EXT(glGenFramebuffers);
    _EXT(glDeleteFramebuffers);
    _EXT(glIsFramebuffer);
    _EXT(glCheckFramebufferStatus);
    _EXT(glBindFramebuffer);
    _EXT(glFramebufferTexture2D);
    _EXT(glFramebufferTexture1D);
    _EXT(glFramebufferTexture3D);
    _EXT(glGenRenderbuffers);
    _EXT(glFramebufferRenderbuffer);
    _EXT(glDeleteRenderbuffers);
    _EXT(glRenderbufferStorage);
    _EXT(glRenderbufferStorageMultisample);
    _EXT(glBindRenderbuffer);
    _EXT(glIsRenderbuffer);
    _EXT(glGenerateMipmap);
    _EXT(glGetFramebufferAttachmentParameteriv);
    _EXT(glGetRenderbufferParameteriv);
    _EXT(glFramebufferTextureLayer);
    _EXT(glBlitFramebuffer);
    _ARB(glGenFramebuffers);
    _ARB(glDeleteFramebuffers);
    _ARB(glIsFramebuffer);
    _ARB(glCheckFramebufferStatus);
    _ARB(glBindFramebuffer);
    _ARB(glFramebufferTexture2D);
    _ARB(glFramebufferTexture1D);
    _ARB(glFramebufferTexture3D);
    _ARB(glGenRenderbuffers);
    _ARB(glFramebufferRenderbuffer);
    _ARB(glDeleteRenderbuffers);
    _ARB(glRenderbufferStorage);
    _ARB(glRenderbufferStorageMultisample);
    _ARB(glBindRenderbuffer);
    _ARB(glIsRenderbuffer);
    _ARB(glGenerateMipmap);
    _ARB(glGetFramebufferAttachmentParameteriv);
    _ARB(glGetRenderbufferParameteriv);
    _ARB(glFramebufferTextureLayer);
    _ARB(glBlitFramebuffer);
    STUB(glDrawBuffersARB);
    
        /*
    MAP_EGL(glGenFramebuffersARB, glGenFramebuffersOES);
    MAP_EGL(glDeleteFramebuffersARB, glDeleteFramebuffersOES);
    MAP_EGL(glBindFramebufferARB, glBindFramebufferOES);
    MAP_EGL(glFramebufferRenderbufferARB, glFramebufferRenderbufferOES);
    MAP_EGL(glFramebufferTexture2DARB, glFramebufferTexture2DOES);
    MAP_EGL(glIsFramebufferARB, glIsFramebufferOES);
    MAP_EGL(glGenRenderbuffersARB, glGenRenderbuffersOES);
    MAP_EGL(glDeleteRenderbuffersARB, glDeleteRenderbuffersOES);
    MAP_EGL(glCheckFramebufferStatusARB, glCheckFramebufferStatusOES);
    MAP_EGL(glRenderbufferStorageARB, glRenderbufferStorageOES);
    MAP_EGL(glBindRenderbufferARB, glBindRenderbufferOES);
    MAP_EGL(glIsRenderbufferARB, glIsRenderbufferOES);
    */

    // GL_EXT_vertex_array
    EXT(glArrayElement);
    EXT(glDrawArrays);
    EXT(glVertexPointer);
    EXT(glNormalPointer);
    EXT(glColorPointer);
    EX(glIndexPointer);	//TODO, stub for now
    EXT(glIndexPointer);
    EXT(glTexCoordPointer);
    EX(glEdgeFlagPointer);	//TODO, stub for now
    EXT(glEdgeFlagPointer);
    EX(glGetPointerv);
    EXT(glGetPointerv);


    // OES wrapper
    EX(glClearDepthfOES);
    EX(glClipPlanefOES);
    EX(glDepthRangefOES);
    EX(glFrustumfOES);
    EX(glGetClipPlanefOES);
    EX(glOrthofOES);

    // passthrough
    // batch thunking!
    #define THUNK(suffix, type)       \
    EX(glColor3##suffix##v);          \
    EX(glColor3##suffix);             \
    EX(glColor4##suffix##v);          \
    EX(glColor4##suffix);             \
    EX(glSecondaryColor3##suffix##v); \
    EX(glSecondaryColor3##suffix);    \
    EXT(glSecondaryColor3##suffix##v); \
    EXT(glSecondaryColor3##suffix);    \
    EX(glIndex##suffix##v);           \
    EX(glIndex##suffix);              \
    EX(glNormal3##suffix##v);         \
    EX(glNormal3##suffix);            \
    EX(glRasterPos2##suffix##v);      \
    EX(glRasterPos2##suffix);         \
    EX(glRasterPos3##suffix##v);      \
    EX(glRasterPos3##suffix);         \
    EX(glRasterPos4##suffix##v);      \
    EX(glRasterPos4##suffix);         \
    EX(glWindowPos2##suffix##v);      \
    EX(glWindowPos2##suffix);         \
    EX(glWindowPos3##suffix##v);      \
    EX(glWindowPos3##suffix);         \
    EX(glVertex2##suffix##v);         \
    EX(glVertex2##suffix);            \
    EX(glVertex3##suffix##v);         \
    EX(glVertex3##suffix);            \
    EX(glVertex4##suffix##v);         \
    EX(glVertex4##suffix);            \
    EX(glTexCoord1##suffix##v);       \
    EX(glTexCoord1##suffix);          \
    EX(glTexCoord2##suffix##v);       \
    EX(glTexCoord2##suffix);          \
    EX(glTexCoord3##suffix##v);       \
    EX(glTexCoord3##suffix);          \
    EX(glTexCoord4##suffix##v);       \
    EX(glTexCoord4##suffix);          \
    EX(glMultiTexCoord1##suffix##v);  \
    EX(glMultiTexCoord1##suffix);     \
    EX(glMultiTexCoord2##suffix##v);  \
    EX(glMultiTexCoord2##suffix);     \
    EX(glMultiTexCoord3##suffix##v);  \
    EX(glMultiTexCoord3##suffix);     \
    EX(glMultiTexCoord4##suffix##v);  \
    EX(glMultiTexCoord4##suffix);     \
    EXT(glMultiTexCoord1##suffix##v); \
    EXT(glMultiTexCoord1##suffix);    \
    EXT(glMultiTexCoord2##suffix##v); \
    EXT(glMultiTexCoord2##suffix);    \
    EXT(glMultiTexCoord3##suffix##v); \
    EXT(glMultiTexCoord3##suffix);    \
    EXT(glMultiTexCoord4##suffix##v); \
    EXT(glMultiTexCoord4##suffix);    \
    ARB(glMultiTexCoord1##suffix##v); \
    ARB(glMultiTexCoord1##suffix);    \
    ARB(glMultiTexCoord2##suffix##v); \
    ARB(glMultiTexCoord2##suffix);    \
    ARB(glMultiTexCoord3##suffix##v); \
    ARB(glMultiTexCoord3##suffix);    \
    ARB(glMultiTexCoord4##suffix##v); \
    ARB(glMultiTexCoord4##suffix);

    THUNK(b, GLbyte);
    THUNK(d, GLdouble);
    THUNK(i, GLint);
    THUNK(s, GLshort);
    THUNK(ub, GLubyte);
    THUNK(ui, GLuint);
    THUNK(us, GLushort);
    THUNK(f, GLfloat);
    #undef THUNK
    
    EX(glPointParameterf);
    EX(glPointParameterfv);
    ARB(glPointParameterf);
    ARB(glPointParameterfv);
    EXT(glPointParameterf);
    EXT(glPointParameterfv);

#ifdef USE_ES2
    EX(glCompileShaderARB);
    EX(glCreateShaderObjectARB);
    EX(glGetObjectParameterivARB);
    EX(glShaderSourceARB);
#endif

    // functions we actually define
    EXT(glActiveTexture);
    ARB(glActiveTexture);
    EX(glArrayElement);
    EX(glBegin);
    EX(glBitmap);
    /*EXT(glBlendColor);
    ARB(glBlendColor);*/
    EXT(glBlendEquation);
    ARB(glBlendEquation);
    EXT(glBlendFunc);
    ARB(glBlendFunc);
#ifndef ODROID
    EXT(glBlendEquationSeparate);
    ARB(glBlendEquationSeparate);
    EX(glBlendEquationSeparatei);
    EXT(glBlendEquationSeparatei);
    ARB(glBlendEquationSeparatei);
    EXT(glBlendFuncSeparate);
    ARB(glBlendFuncSeparate);
    EX(glBlendFuncSeparatei);
    EXT(glBlendFuncSeparatei);
    ARB(glBlendFuncSeparatei);
#endif
    EX(glCallList);
    EX(glCallLists);
    EX(glClearDepth);
    EXT(glClientActiveTexture);
    ARB(glClientActiveTexture);
    EX(glClipPlane);
    EX(glCopyPixels);
    EX(glDeleteLists);
    EX(glDepthRange);
    EX(glDrawBuffer);
    EX(glDrawPixels);
    EX(glDrawRangeElements);
    EX(glDrawRangeElementsEXT);
    EX(glEdgeFlag);
    EX(glEnd);
    EX(glEndList);
    EX(glEvalCoord1d);
    EX(glEvalCoord1f);
    EX(glEvalCoord2d);
    EX(glEvalCoord2f);
    EX(glEvalMesh1);
    EX(glEvalMesh2);
    EX(glEvalPoint1);
    EX(glEvalPoint2);
    EX(glFogCoordd);
    EX(glFogCoorddv);
    EX(glFogCoordf);
    EX(glFogCoordfv);
    EX(glFogi);
    EX(glFogiv);
    EX(glFrustum);
    EX(glGenLists);
    EX(glGetDoublev);
    EX(glGetIntegerv);
    EX(glGetMapdv);
    EX(glGetMapfv);
    EX(glGetMapiv);
    EX(glGetTexImage);
    EX(glGetTexLevelParameterfv);
    EX(glGetTexLevelParameteriv);
    EX(glInitNames);
    EX(glInterleavedArrays);
    EX(glIsList);
#ifndef USE_ES2
    EX(glLighti);
    EX(glLightiv);
    EX(glLightModeli);
    EX(glLightModeliv);
#endif
    EX(glLineStipple);
    EX(glListBase);
    EX(glLoadMatrixd);
    EX(glLoadName);
    EX(glLockArraysEXT);
    EX(glMap1d);
    EX(glMap1f);
    EX(glMap2d);
    EX(glMap2f);
    EX(glMapGrid1d);
    EX(glMapGrid1f);
    EX(glMapGrid2d);
    EX(glMapGrid2f);
    EX(glMateriali);
    EX(glMultMatrixd);
    EX(glNewList);
    EX(glOrtho);
    EX(glPixelTransferf);
    EX(glPixelTransferi);
    EX(glPixelZoom);
    EX(glPolygonMode);
    EX(glPolygonStipple);
    EX(glPopAttrib);
    EX(glPopClientAttrib);
    EX(glPopName);
    EX(glPushAttrib);
    EX(glPushClientAttrib);
    EX(glPushName);
    EX(glRasterPos2i);
    EX(glReadBuffer);
    EX(glRectd);
    EX(glRectf);
    EX(glRecti);
    EX(glRects);
    EX(glRectdv);
    EX(glRectfv);
    EX(glRectiv);
    EX(glRectsv);
    EX(glRenderMode);
    EX(glRotated);
    EX(glScaled);
    EX(glSecondaryColorPointer);
    EXT(glSecondaryColorPointer);
    EX(glTexEnvf);
    EX(glTexEnviv);
    EX(glTexGend);
    EX(glTexGendv);
    EX(glTexGenf);
    EX(glTexGenfv);
    EX(glTexGeni);
    EX(glTexGeniv);
    EX(glTexImage1D);
    EX(glTexImage3D);
    EX(glTexSubImage1D);
    EX(glTexSubImage3D);
    EX(glCompressedTexImage1D);
    EX(glCompressedTexSubImage1D);
    EX(glCompressedTexImage3D);
    EX(glCompressedTexSubImage3D);
    EX(glGetCompressedTexImage);
    EXT(glCompressedTexImage2D);
    EXT(glCompressedTexSubImage2D);
    EXT(glCompressedTexImage1D);
    EXT(glCompressedTexSubImage1D);
    EXT(glCompressedTexImage3D);
    EXT(glCompressedTexSubImage3D);
    EXT(glGetCompressedTexImage);
    ARB(glCompressedTexImage2D);
    ARB(glCompressedTexSubImage2D);
    ARB(glCompressedTexImage1D);
    ARB(glCompressedTexSubImage1D);
    ARB(glCompressedTexImage3D);
    ARB(glCompressedTexSubImage3D);
    ARB(glGetCompressedTexImage);
    EX(glCopyTexImage1D);
    EX(glCopyTexSubImage1D);
    EX(glTranslated);
    EX(glUnlockArraysEXT);
	EX(glGetTexGenfv);
	EX(glLoadTransposeMatrixf);
	EX(glLoadTransposeMatrixd);
	EX(glMultTransposeMatrixd);
	EX(glMultTransposeMatrixf);
    // stubs for unimplemented functions
    STUB(glAccum);
    STUB(glAreTexturesResident);
    STUB(glClearAccum);
    STUB(glColorMaterial);
    STUB(glCopyTexImage3D);
    STUB(glCopyTexSubImage3D);
    STUB(glFeedbackBuffer);
    STUB(glGetClipPlane);
    STUB(glGetLightiv);
    STUB(glGetMaterialiv);
    STUB(glGetPixelMapfv);
    STUB(glGetPixelMapuiv);
    STUB(glGetPixelMapusv);
    STUB(glGetPolygonStipple);
    STUB(glGetStringi);
    STUB(glGetTexGendv);
    //STUB(glGetTexGenfv);
    STUB(glGetTexGeniv);    //TODO
    STUB(glMaterialiv);     //TODO
    STUB(glPassThrough);
    STUB(glPixelMapfv);
    STUB(glPixelMapuiv);
    STUB(glPixelMapusv);
    EX(glPixelStoref);
    STUB(glPrioritizeTextures);
    STUB(glSelectBuffer);   //TODO
    
    STUB(glFogCoordPointer);
    /*STUB(glEdgeFlagPointerEXT);
    STUB(glIndexPointerEXT);*/
    
    //EXT_direct_state_access
    EX(glClientAttribDefaultEXT);
    EX(glPushClientAttribDefaultEXT);
    EX(glMatrixLoadfEXT);
    EX(glMatrixLoaddEXT);
    EX(glMatrixMultfEXT);
    EX(glMatrixMultdEXT);
    EX(glMatrixLoadIdentityEXT);
    EX(glMatrixRotatefEXT);
    EX(glMatrixRotatedEXT);
    EX(glMatrixScalefEXT);
    EX(glMatrixScaledEXT);
    EX(glMatrixTranslatefEXT);
    EX(glMatrixTranslatedEXT);
    EX(glMatrixOrthoEXT);
    EX(glMatrixFrustumEXT);
    EX(glMatrixPushEXT);
    EX(glMatrixPopEXT);
    EX(glTextureParameteriEXT);
    EX(glTextureParameterivEXT);
    EX(glTextureParameterfEXT);
    EX(glTextureParameterfvEXT);
    EX(glTextureImage1DEXT);
    EX(glTextureImage2DEXT);
    EX(glTextureSubImage1DEXT);
    EX(glTextureSubImage2DEXT);
    EX(glCopyTextureImage1DEXT);
    EX(glCopyTextureImage2DEXT);
    EX(glCopyTextureSubImage1DEXT);
    EX(glCopyTextureSubImage2DEXT);
    EX(glGetTextureImageEXT);
    EX(glGetTextureParameterfvEXT);
    EX(glGetTextureParameterivEXT);
    EX(glGetTextureLevelParameterfvEXT);
    EX(glGetTextureLevelParameterivEXT);
    EX(glTextureImage3DEXT);
    EX(glTextureSubImage3DEXT);
    EX(glCopyTextureSubImage3DEXT);
    EX(glBindMultiTextureEXT);
    EX(glMultiTexCoordPointerEXT);
    EX(glMultiTexEnvfEXT);
    EX(glMultiTexEnvfvEXT);
    EX(glMultiTexEnviEXT);
    EX(glMultiTexEnvivEXT);
    EX(glMultiTexGendEXT);
    EX(glMultiTexGendvEXT);
    EX(glMultiTexGenfEXT);
    EX(glMultiTexGenfvEXT);
    EX(glMultiTexGeniEXT);
    EX(glMultiTexGenivEXT);
    EX(glGetMultiTexEnvfvEXT);
    EX(glGetMultiTexEnvivEXT);
    EX(glGetMultiTexGendvEXT);
    EX(glGetMultiTexGenfvEXT);
    EX(glGetMultiTexGenivEXT);
    EX(glMultiTexParameteriEXT);
    EX(glMultiTexParameterivEXT);
    EX(glMultiTexParameterfEXT);
    EX(glMultiTexParameterfvEXT);
    EX(glMultiTexImage1DEXT);
    EX(glMultiTexImage2DEXT);
    EX(glMultiTexSubImage1DEXT);
    EX(glMultiTexSubImage2DEXT);
    EX(glCopyMultiTexImage1DEXT);
    EX(glCopyMultiTexImage2DEXT);
    EX(glCopyMultiTexSubImage1DEXT);
    EX(glCopyMultiTexSubImage2DEXT);
    EX(glGetMultiTexImageEXT);
    EX(glGetMultiTexParameterfvEXT);
    EX(glGetMultiTexParameterivEXT);
    EX(glGetMultiTexLevelParameterfvEXT);
    EX(glGetMultiTexLevelParameterivEXT);
    EX(glMultiTexImage3DEXT);
    EX(glMultiTexSubImage3DEXT);
    EX(glCopyMultiTexSubImage3DEXT);
    STUB(EnableClientStateIndexedEXT);
    STUB(DisableClientStateIndexedEXT);
    STUB(GetFloatIndexedvEXT);
    STUB(GetDoubleIndexedvEXT);
    STUB(GetPointerIndexedvEXT);
    STUB(EnableIndexedEXT);
    STUB(DisableIndexedEXT);
    STUB(IsEnabledIndexedEXT);
    STUB(GetIntegerIndexedvEXT);
    STUB(GetBooleanIndexedvEXT);
    EX(glCompressedTextureImage3DEXT);
    EX(glCompressedTextureImage2DEXT);
    EX(glCompressedTextureImage1DEXT);
    EX(glCompressedTextureSubImage3DEXT);
    EX(glCompressedTextureSubImage2DEXT);
    EX(glCompressedTextureSubImage1DEXT);
    EX(glGetCompressedTextureImageEXT);
    EX(glCompressedMultiTexImage3DEXT);
    EX(glCompressedMultiTexImage2DEXT);
    EX(glCompressedMultiTexImage1DEXT);
    EX(glCompressedMultiTexSubImage3DEXT);
    EX(glCompressedMultiTexSubImage2DEXT);
    EX(glCompressedMultiTexSubImage1DEXT);
    EX(glGetCompressedMultiTexImageEXT);
    EX(glMatrixLoadTransposefEXT);
    EX(glMatrixLoadTransposedEXT);
    EX(glMatrixMultTransposefEXT);
    EX(glMatrixMultTransposedEXT);

    printf("glXGetProcAddress: %s not found.\n", name);
    return NULL;
}

void *glXGetProcAddress(const char *name) {
    return glXGetProcAddressARB(name);
}
