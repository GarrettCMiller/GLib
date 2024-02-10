/////////////////////////////////////////////////////////////////////////////
//		File:			PhysicsObject.h
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	8/04/2011 02:46 PM
//
//		Description:	The physics engine objects (wrapper to PhysX PxActor*)
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "PxController.h"
#include "Vec.h"

//////////////////////////////////////////////////////////////////////////

class PxActor;
class PxCapsuleController;

//////////////////////////////////////////////////////////////////////////

class PhysicsBaseObject
{
public:
	PhysicsBaseObject() : m_pObj(NULL)
	{	}
	~PhysicsBaseObject()	
	{	}

	virtual Int32 Init() = 0;

	virtual Int32 Shutdown();

	PxActor* GetObj()
	{
		ASSERT(m_pObj);
		return m_pObj;
	}

	void SetObj(PxActor* pObj)
	{
		m_pObj = pObj;
	}

protected:

	PxActor*	m_pObj;
};

//////////////////////////////////////////////////////////////////////////

class PhysicsRigidStaticPlane : public PhysicsBaseObject
{
public:
	PhysicsRigidStaticPlane() : PhysicsBaseObject()
	{	}
	~PhysicsRigidStaticPlane()
	{	}

	Int32 Init();
};

//////////////////////////////////////////////////////////////////////////

class PhysicsRigidDynamic : public PhysicsBaseObject
{
public:
	PhysicsRigidDynamic() : PhysicsBaseObject()
	{	}
	~PhysicsRigidDynamic()
	{	}

	Int32 Init();
	Int32 Init(Vec3 pos, Vec3 vel);

	Vec3 GetPosition();
};

//////////////////////////////////////////////////////////////////////////

class PhysicsCapsuleController :	public PhysicsBaseObject, 
									public PxUserControllerHitReport
{
public:
	PhysicsCapsuleController() : m_pController(NULL)
	{	}
	~PhysicsCapsuleController()
	{	}

	Int32 Init();
	Int32 Init(Vec3 pos);

	Vec3 GetPosition();

	virtual void onShapeHit(const PxControllerShapeHit& hit);
	virtual void onControllerHit(const PxControllersHit& hit);

protected:
	PxCapsuleController*	m_pController;
};