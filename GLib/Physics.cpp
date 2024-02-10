#include "StdAfx.h"
#include "Physics.h"

#include "PxPhysicsAPI.h"
#include "PxExtensionsAPI.h"
#include "PxDefaultCpuDispatcher.h"
//#include "PxCudaContextManager.h"

#include "PhysicsObject.h"

//////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "Foundation")
#pragma comment(lib, "PhysX3Common")
//#pragma comment(lib, "PhysX3CharacterDynamic")
#pragma comment(lib, "PhysX3CharacterKinematic")
#pragma comment(lib, "PhysX3Vehicle")
#pragma comment(lib, "PhysX3Extensions")
#pragma comment(lib, "LowLevel")
#pragma comment(lib, "GeomUtils")
#pragma comment(lib, "SceneQuery")
#pragma comment(lib, "SimulationController")
#pragma comment(lib, "PvdRuntime")
#pragma comment(lib, "RepX3")
#pragma comment(lib, "PxTask")

#pragma comment(lib, "PhysX3Gpu_x86")
#pragma comment(lib, "PhysX3Cooking_x86")
#pragma comment(lib, "PhysX3_x86")

//////////////////////////////////////////////////////////////////////////

using namespace physx;

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_SINGLETON(Physics);

//////////////////////////////////////////////////////////////////////////

PxExtendedVec3 PxVec3ToPxEVec3(const PxVec3& pV)
{
	return PxExtendedVec3(pV.x, pV.y, pV.z);
}

PxExtendedVec3 Vec3ToPxEVec3(const Vec3& pV)
{
	return PxExtendedVec3(pV.x, pV.y, pV.z);
}

Vec3 PxEVec3ToVec3(const PxExtendedVec3& pV)
{
	return Vec3(pV.x, pV.y, pV.z);
}

Vec3 PxVec3ToVec3(const PxVec3& pV)
{
	return Vec3(pV.x, pV.y, pV.z);
}

PxVec3 Vec3ToPxVec3(const Vec3& vP)
{
	return PxVec3(vP.x, vP.y, vP.z);
}

Vec3 PxTransformToVec3(const PxTransform& pT)
{
	return PxVec3ToVec3(pT.p);
}

PxTransform Vec3ToPxTransform(const Vec3& vP)
{
	return PxTransform(Vec3ToPxVec3(vP));
}

//////////////////////////////////////////////////////////////////////////

class PXErrorCallback : public PxDefaultErrorCallback
{
public:
	PXErrorCallback()
	{	}

	void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		g_Log.LOG_ERROR(message);
	}
};

//class PXAllocator : public PxAllocatorCallback
//{
//public:
//	PXAllocator()
//	{	}
//
//	void* allocate(size_t size, const char* typeName, const char* filename, int line)
//	{
//		return g_MemMgr->AllocateMem(size, typeName, line, filename);
//	}
//
//	void deallocate(void* ptr)
//	{
//		if (ptr)
//			g_MemMgr->Release(ptr);
//	}
//};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Physics::Physics(void) :
	m_pPX(NULL), m_pScene(NULL),
	//m_pFoundation(NULL),
	m_pCpuDispatcher(NULL), 
	m_pDefaultMaterial(NULL),
	m_bRecordMemAllocs(true)
{
}

Physics::~Physics(void)
{
	Shutdown();
}

//////////////////////////////////////////////////////////////////////////

Int32 Physics::Init()
{
	m_pAllocator		= new PxDefaultAllocator;
	m_pErrorCallback	= new PXErrorCallback;

	m_pPX = PxCreatePhysics(PX_PHYSICS_VERSION, 
							*m_pAllocator, *m_pErrorCallback,
							PxTolerancesScale(), m_bRecordMemAllocs);

	if (!m_pPX)
	{
		g_Log.LOG_ERROR("Couldn't create PhysX object!\n");
		return -1;
	}

	if (!PxInitExtensions(*m_pPX))
	{
		g_Log.LOG_ERROR("Couldn't init PhysX extensions!\n");
		return -2;
	}

	m_ControllerMgr.Init();

	//cooking too

	PxSceneDesc desc = (m_pPX->getTolerancesScale());
	desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	
 	if (!desc.cpuDispatcher)
 	{
 		desc.cpuDispatcher = PxDefaultCpuDispatcherCreate(8);

		if (!desc.cpuDispatcher)
		{
			g_Log.LOG_ERROR("Couldn't create PhysX cpu dispatcher!\n");
			return -3;
		}
 	}

	if (!desc.filterShader)
	{
		desc.filterShader = PxDefaultSimulationFilterShader;
	}

	m_pScene = m_pPX->createScene(desc);

	if (!m_pScene)
	{
		g_Log.LOG_ERROR("Couldn't create PhysX scene!\n");
		return -4;
	}

	m_pDefaultMaterial = m_pPX->createMaterial(0.5f, 0.5f, 0.5f);

	if (!m_pDefaultMaterial)
	{
		g_Log.LOG_ERROR("Couldn't create default PhysX material!\n");
		return -5;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 Physics::Update(Float dt)
{
	m_pScene->simulate(dt);

	while (!m_pScene->fetchResults())
	{
		//do work
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 Physics::Shutdown()
{
	m_ControllerMgr.Shutdown();

	m_pDefaultMaterial->release();
	m_pDefaultMaterial = NULL;

	m_pPX->release();
	m_pPX = NULL;

	delete m_pErrorCallback;
	delete m_pAllocator;

	return 1;
}

//////////////////////////////////////////////////////////////////////////

Int32 Physics::CreateRigidStaticPlane(PhysicsRigidStaticPlane* pObj)
{
	ASSERT(pObj);

	PxTransform pose = PxTransform(Vec3ToPxVec3(k_v3Zero), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

	PxRigidStatic* pPxObj = m_pPX->createRigidStatic(pose);

	if (!pPxObj)
	{
		g_Log.LOG_ERROR("Couldn't create PhysX rigid static!\n");
		return -1;
	}

	PxShape* pShape = pPxObj->createShape(PxPlaneGeometry(), *m_pDefaultMaterial);

	if (!pShape)
	{
		g_Log.LOG_ERROR("Couldn't create PhysX rigid static shape!\n");
		return -2;
	}

	m_pScene->addActor(*pPxObj);

	pObj->SetObj(pPxObj);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

Int32 Physics::CreateRigidDynamic(PhysicsRigidDynamic* pObj, Vec3 vPos, Vec3 vVel)
{
	ASSERT(pObj);

	Float fDensity = 1.0f;

	PxTransform pose = PxTransform(Vec3ToPxVec3(vPos));
	PxVec3 size(15.0f, 15.0f, 15.0f);
	PxBoxGeometry box(size);

	PxRigidDynamic* pPxObj = PxCreateDynamic(*m_pPX, pose, box, *m_pDefaultMaterial, fDensity);

	if (!pPxObj)
	{
		g_Log.LOG_ERROR("Couldn't create PhysX rigid dynamic!\n");
		return -1;
	}

	pPxObj->setAngularDamping(0.75f);
	pPxObj->setLinearVelocity(Vec3ToPxVec3(vVel));

	m_pScene->addActor(*pPxObj);

	pObj->SetObj(pPxObj);

	return 0;
}

//////////////////////////////////////////////////////////////////////////

PxCapsuleController* Physics::CreateCapsuleController(PxCapsuleControllerDesc& pDesc)
{
	return NULL;
	//return static_cast<PxCapsuleController*>(m_ControllerMgr.CreateController(pDesc));
}