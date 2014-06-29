#ifndef common_h
#define common_h

#include <CL/cl.h>
#include <CL/cl_platform.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include "vtypes.h"
#include "debug.h"

#ifdef USE_ROCKET
#include <Rocket/Core.h>
#include "rocketmenu.h"
#endif

typedef void (*LrtClickFunc)(void* data);

#endif
