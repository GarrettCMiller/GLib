#pragma once

#include "variable.h"

class IInterface
{
public:
	//One-time init
	virtual Int32 Init(TArgumentArray* parrArgs = NULL) = 0;

	//Every frame update (calls RenderForeground() among other things)
	virtual Int32 Update(TArgumentArray* parrArgs = NULL) = 0;
	
	//One-time final shutdown
	virtual Int32 Shutdown() = 0;

	//Handle keyboard/mouse input
	virtual Int32 HandleInput() = 0;

	//Render textures, stage determines which textures to render from/to
	virtual Int32 Render() = 0;
};
