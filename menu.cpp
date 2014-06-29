
#include "menu.h"

bool MWButton::attemptClick( int2 coord )
{
	int2 rel = coord - min;
	if( rel.x < span.x && rel.y < span.y )
	{
		click();
		return true;
	}
	return false;
}

bool MWButton::peek( int2 coord )
{
	int2 rel = coord - min;
	if( rel.x < span.x && rel.y < span.y )
	{
		return true;
	}
	return false;
}

void MWMenu::draw( GLFWwindow* window )
{
	
	glfwMakeContextCurrent( window ); glErrorCheck;
	glEnable(GL_TEXTURE_2D); glErrorCheck;
	glBindTexture( GL_TEXTURE_2D, image->getTexture() ); glErrorCheck;
	glBegin(GL_QUADS);
	glTexCoord2f( 1.0, 1.0 ); glVertex2f( 0.0, 0.0 );
	glTexCoord2f( 0.0, 1.0 ); glVertex2f( 1.0, 0.0 );
	glTexCoord2f( 0.0, 0.0 ); glVertex2f( 1.0, 1.0 );
	glTexCoord2f( 1.0, 0.0 ); glVertex2f( 0.0, 1.0 );
	glEnd(); glErrorCheck;
	glFlush(); glErrorCheck;
	glDisable(GL_TEXTURE_2D); glErrorCheck;
}
