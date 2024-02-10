/////////////////////////////////////////////////////////////////////////////
//		File:			PhysicsObject.cpp
//
//		Programmer:		Garrett Miller (GM)
//
//		Date Created:	8/04/2011 02:47 PM
//
//		Description:	The physics engine objects (wrapper to PhysX PxActor*)
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PhysicsObject.h"

#include "Physics.h"

#include "PxPhysicsAPI.h"
#include "extensions/PxExtensionsAPI.h"
#include "PxShapeExt.h"

#include "PxCapsuleController.h"

//////////////////////////////////////////////////////////////////////////

extern PxExtendedVec3 PxVec3ToPxEVec3(const PxVec3& pV);
extern PxExtendedVec3 Vec3ToPxEVec3(const Vec3& pV);
extern Vec3 PxEVec3ToVec3(const PxExtendedVec3& pV);

extern Vec3 PxVec3ToVec3(const PxVec3& pV);
extern PxVec3 Vec3ToPxVec3(const Vec3& vP);

extern Vec3 PxTransformToVec3(const PxTransform& pT);
extern PxTransform Vec3ToPxTransform(const Vec3& vP);

//////////////////////////////////////////////////////////////////////////

Int32 PhysicsBaseObject::Shutdown()
{
	if (m_pObj)
	{
		m_pObj->release();
		m_pObj = NULL;
		return 0;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 PhysicsRigidStaticPlane::Init()
{
	return g_Physics->CreateRigidStaticPlane(this);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Int32 PhysicsRigidDynamic::Init()
{
	return g_Physics->CreateRigidDynamic(this, k_v3Zero, k_v3Zero);
}

//////////////////////////////////////////////////////////////////////////

Int32 PhysicsRigidDynamic::Init(Vec3 pos, Vec3 vel)
{
	return g_Physics->CreateRigidDynamic(this, pos, vel);
}

//////////////////////////////////////////////////////////////////////////

Vec3 PhysicsRigidDynamic::GetPosition()
{
	PxRigidDynamic* pRD = static_cast<PxRigidDynamic*>(GetObj());
	PxU32 nShapes = pRD->getNbShapes();
	PxShape** shapes=new PxShape*[nShapes];

	pRD->getShapes(shapes, nShapes);

	PxShape* shape = shapes[0];

	PxTransform pT = PxShapeExt::getGlobalPose(*shape);

	delete [] shapes;

	return PxTransformToVec3(pT);
}

//////////////////////////////////////////////////////////////////////////

Int32 PhysicsCapsuleController::Init()
{
	PxCapsuleControllerDesc desc;
	desc.material		= g_Physics->GetDefaultMaterial();
	//desc.position		= 
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 PhysicsCapsuleController::Init(Vec3 pos)
{
	PxCapsuleControllerDesc desc;

	desc.material		= g_Physics->GetDefaultMaterial();
	desc.position		= Vec3ToPxEVec3(pos);
	desc.height			= 15.0f;
	desc.radius			= 10.0f;
	desc.slopeLimit		= 0.0f;
	desc.contactOffset	= 0.1f;
	desc.stepOffset		= 0.02f;
	desc.callback		= this;

	m_pController = g_Physics->CreateCapsuleController(desc);

	ASSERT(m_pController);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Vec3 PhysicsCapsuleController::GetPosition()
{
	/*PxRigidDynamic* pRD = static_cast<PxRigidDynamic*>(GetObj());
	
	PxShape* shape;
	pRD->getShapes(&shape, 1);	

	PxTransform pT = PxShapeExt::getGlobalPose(*shape);

	return PxTransformToVec3(pT);*/

	return PxEVec3ToVec3(m_pController->getPosition());
}

//////////////////////////////////////////////////////////////////////////

void PhysicsCapsuleController::onShapeHit(const PxControllerShapeHit& hit)
{
	PxRigidDynamic* actor = hit.shape->getActor().isRigidDynamic();
	if(actor)
	{
		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		if(hit.dir.y==0.0f)
		{
			PxReal coeff = actor->getMass() * hit.length;
			PxRigidBodyExt::addForceAtLocalPos(*actor,hit.dir*coeff, PxVec3(0,0,0), PxForceMode::eIMPULSE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PhysicsCapsuleController::onControllerHit(const PxControllersHit& hit)
{
	PxRigidDynamic* actor = hit.controller->getActor()->isRigidDynamic();
	if(actor)
	{
		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		/*if(hit.dir.y==0.0f)
		{
			PxReal coeff = actor->getMass() * hit.length;
			PxRigidBodyExt::addForceAtLocalPos(*actor,hit.dir*coeff, PxVec3(0,0,0), PxForceMode::eIMPULSE);
		}*/
		actor = actor;
	}
}

//////////////////////////////////////////////////////////////////////////