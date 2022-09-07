#ifndef OPENGL_ERROR_H
#define OPENGL_ERROR_H

namespace OpenGL
{
	void CheckError(const char* file, int line);
}

#define GLValidate(glFunc) glFunc; \
	OpenGL::CheckError(__FILE__, __LINE__);


#endif