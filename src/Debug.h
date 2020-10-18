#pragma once
#include <cstdio>
#include <iostream>
#include <GL/glew.h>

#ifdef DEBUG
#define GL_CHECK(expr) do { (expr); checkGlError(__FILE__, __LINE__, #expr); } while (false)
#else
#define GL_CHECK(expr) (expr)
#endif

static void checkGlError(const char* file, unsigned int line, const char* expr){
  auto code = glGetError();
  if (code == GL_NO_ERROR)
    return;

  const char *name;
  const char *desc;

  switch (code) {
  case GL_INVALID_ENUM:
    name = "GL_INVALID_ENUM";
    desc = "An unacceptable value is specified for an enumerated argument.";
    break;
  case GL_INVALID_VALUE:
    name = "GL_INVALID_VALUE";
    desc = "A numeric argument is out of range.";
    break;
  case GL_INVALID_OPERATION:
    name = "GL_INVALID_OPERATION";
    desc = "The specified operation is not allowed in the current state.";
    break;
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    name = "GL_INVALID_FRAMEBUFFER_OPERATION";
    desc = "The command is trying to render to or read from the framebuffer while the currently bound framebuffer is not framebuffer complete.";
    break;
  case GL_OUT_OF_MEMORY:
    name = "GL_OUT_OF_MEMORY";
    desc = "There is not enough memory left to execute the command.";
    break;
  default:
    std::cerr << "Error '" << code <<"' at " << file << ':' << line << " for expression '" << expr << "'" << "\n";
    return;
  }

  std::cerr << "Error '" << name <<"' at " << file << ':' << line << " for expression '" << expr << "': " << desc << "\n";
}
