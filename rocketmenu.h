#ifndef rocketmenu_h
#define rocketmenu_h

#include "common.h"
#include <Rocket/Core/RenderInterface.h>
#include <Rocket/Core/FontDatabase.h>
#include <Rocket/Debugger.h>
#include <Rocket/Input.h>
#include <Rocket/Shell.h>






extern Rocket::Core::Context* rContext;
void rocketLoop( void );
void initRocketMenu( void );
void registerListener( std::string ID, LrtClickFunc listener, void* data );
void registerListener( char* ID, LrtClickFunc listener, void* data );

#endif
