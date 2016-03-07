#pragma once
#include <GL\glew.h>
#include <iostream>
#include "UsefulDefines.h"
#include "Utilities.h"

struct Texture
{
	unsigned int TextureHandle;	
};

////
Texture *SetTexture(Texture *estinationtexture, char *imagePath);
void BindTexture(Texture *texture);
void UnbindTexture();