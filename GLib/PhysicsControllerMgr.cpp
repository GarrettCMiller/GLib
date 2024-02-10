#include "StdAfx.h"
#include "PhysicsControllerMgr.h"

#include "Physics.h"

#include "PxPhysicsAPI.h"
#include "PxFoundation.h"

#define  PX_PHYSX_CHARACTER_STATIC_LIB

#include "PxController.h"
#include "PxControllerManager.h"

//////////////////////////////////////////////////////////////////////////

PhysicsControllerMgr::PhysicsControllerMgr(void)
{
}

PhysicsControllerMgr::~PhysicsControllerMgr(void)
{
}

//////////////////////////////////////////////////////////////////////////

Int32 PhysicsControllerMgr::Init()
{
	m_pMgr = PxCreateControllerManager(g_Physics->GetSDK()->getFoundation());

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 PhysicsControllerMgr::Shutdown()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////

PxController* PhysicsControllerMgr::CreateController(PxCapsuleControllerDesc& desc)
{
	return NULL;
	//return m_pMgr->createController(*g_Physics->GetSDK(), g_Physics->GetScene(), *static_cast<PxControllerDesc*>(&desc));
}