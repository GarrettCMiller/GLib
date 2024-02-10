/////////////////////////////////////////////////////////////////////////////
//		File:			PhysicsControllerMgr.h
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	08/04/2011 08:16 PM
//
//		Description:	The game
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class PxController;
class PxControllerManager;
class PxCapsuleControllerDesc;

class Physics;

class PhysicsControllerMgr
{
	//friend class Physics;

public:
	PhysicsControllerMgr(void);
	~PhysicsControllerMgr(void);

	Int32 Init();
	Int32 Shutdown();

	PxController* CreateController(PxCapsuleControllerDesc& desc);

protected:
	PxControllerManager*	m_pMgr;
};
