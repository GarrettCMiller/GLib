/////////////////////////////////////////////////////////////////////////////
//		File:			Physics.h
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	7/29/2011 08:11:33 PM
//
//		Description:	The physics engine (wrapper to PhysX)
//
/////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Singleton.h"
#include "Vec.h"
#include "MemMgr.h"

#include "PhysicsControllerMgr.h"

//////////////////////////////////////////////////////////////////////////

class PxPhysics;
class PxScene;

//class PxFoundation;
class DefaultCpuDispatcher;
//class pxtask::CudaContextManager;

class PXErrorCallback;
class PxDefaultAllocator;

class PxCapsuleController;
class PxCapsuleControllerDesc;

class PxMaterial;

class PhysicsRigidStaticPlane;
class PhysicsRigidDynamic;

class PxActor;

//////////////////////////////////////////////////////////////////////////

class Physics :	public Singleton<Physics>
{
	Physics(Physics& p);
	Physics& operator = (Physics& p);

public:
	Physics(void);
	~Physics(void);

	Int32 Init();
	Int32 Update(Float dt);
	Int32 Shutdown();

	Int32 CreateRigidStaticPlane(PhysicsRigidStaticPlane* pObj);
	Int32 CreateRigidDynamic(PhysicsRigidDynamic* pObj, Vec3 vPos, Vec3 vVel);

	PxCapsuleController* CreateCapsuleController(PxCapsuleControllerDesc& desc);

	PxMaterial* GetDefaultMaterial()
	{
		return m_pDefaultMaterial;
	}

	PxPhysics* GetSDK()
	{
		return m_pPX;
	}

	PxScene* GetScene()
	{
		return m_pScene;
	}

	PhysicsControllerMgr& GetControllerMgr()
	{
		return m_ControllerMgr;
	}

private:
	Bool			m_bRecordMemAllocs;

	PxPhysics*		m_pPX;
	PxScene*		m_pScene;
	//PxFoundation*	m_pFoundation;
	//PxSceneDesc m_Desc;

	PhysicsControllerMgr	m_ControllerMgr;

	//PxSimulationFilterShader* gDefaultFilterShader=; 

	DefaultCpuDispatcher*	m_pCpuDispatcher;
	//pxtask::CudaContextManager*		mCudaContextManager;

	PxMaterial*			m_pDefaultMaterial;

	PXErrorCallback*	m_pErrorCallback;
	PxDefaultAllocator*		m_pAllocator;
};

//////////////////////////////////////////////////////////////////////////

#define g_Physics	Physics::GetInstance()

//////////////////////////////////////////////////////////////////////////