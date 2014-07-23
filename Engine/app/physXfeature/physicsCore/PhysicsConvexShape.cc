/****************************************************************************
Copyright (c) 2011-2013,WebJet Business Division,CYOU

http://www.genesis-3d.com.cn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#if defined (__USE_PHYSX__) || defined (__GENESIS_EDITOR__)
#include "stdneb.h"
#include "appframework/actor.h"
#include "resource/meshres.h"
#include "PhysicsMeshShape.h"
#include "PhysicsConvexShape.h"
#include "graphicfeature/components/meshrendercomponent.h"
#include "graphicfeature/graphicsfeatureprotocol.h"
#include "resource/resourcemanager.h"
#include "util/array.h"
#include "PhysicsServer.h"
#include "PxPhysics.h"
#include "PxCooking.h"
#include "PxConvexMesh.h"
#include "PxConvexMeshGeometry.h"
#include "PxShape.h"
#include "PhysicsUtil.h"
#include "UserStream.h"
#include "physXfeature/PhysicsBodyComponent.h"

namespace App
{
	__ImplementClass(App::PhysicsConvexShape, 'PYCS', App::PhysicsShape);

	using namespace Resources;

	PhysicsConvexShape::PhysicsConvexShape()
		: m_Scale(1.0f, 1.0f, 1.0f)
		, m_RightScale(1.0f, 1.0f, 1.0f)
		, m_ActorScale(1.f,1.f,1.f)
		, m_UseActorMesh(true)
		, m_MeshFileName("sys:box.mesh")
	{
		m_ShapeType = CONVEXSHAPE;
	}

	PhysicsConvexShape::~PhysicsConvexShape()
	{
		OnDestory();
	}

	bool PhysicsConvexShape::OnCreate( GPtr<PhysicsBodyComponent> component )
	{
		if (component.isvalid() && component->GetActor())
		{
			const Math::vector& _scale = component->GetActor()->GetWorldScale();
			m_ActorScale = Math::float3(_scale.x(),_scale.y(),_scale.z());
			return CreateShape(component);
		}
		return false;
	}

	bool PhysicsConvexShape::CopyFrom( const GPtr<PhysicsShape>& pShape )
	{
		if ( !pShape.isvalid() )
		{
			return false;
		}
		Super::CopyFrom(pShape);

		if ( pShape->GetType() == m_ShapeType )
		{
			SetScale(pShape.downcast<PhysicsConvexShape>()->m_Scale);
			SetMeshFileName(pShape.downcast<PhysicsConvexShape>()->GetMeshFileName());
		} 
		else if (pShape->GetType() == MESHSHAPE)
		{
			SetScale(pShape.downcast<PhysicsMeshShape>()->GetScale());
			SetMeshFileName(pShape.downcast<PhysicsMeshShape>()->GetMeshFileName());
		}
		return true;
	}

	void PhysicsConvexShape::Save( AppWriter* pSerialize )
	{
		Super::Save(pSerialize);
		pSerialize->SerializeBool("UseActorMesh", m_UseActorMesh);
		pSerialize->SerializeFloat3("Scale", m_Scale);
		pSerialize->SerializeString("MeshFileName", m_MeshFileName);
	}

	void PhysicsConvexShape::Load( Version ver, AppReader* pReader )
	{
		Super::Load(ver,pReader);
		pReader->SerializeBool("UseActorMesh", m_UseActorMesh);
		pReader->SerializeFloat3("Scale", m_Scale);
		pReader->SerializeString("MeshFileName", m_MeshFileName);
	}

	bool PhysicsConvexShape::IsGeometryValid()
	{
		return IsValid() && ((PxConvexMeshGeometry*)m_pGeometry)->isValid();
	}

	void PhysicsConvexShape::SetMeshFileName( const Util::String& strName )
	{
		if (m_MeshFileName==strName)
		{
			return;
		}

		m_UseActorMesh = false;
		m_MeshFileName = strName;
		m_bMassUpdate = true;

		if (m_pGeometry)
		{
			delete m_pGeometry;
			m_pGeometry = NULL;
		}
		CreateShape(NULL);
		if (m_pShape)
		{
			if (m_pGeometry && m_pShape->getGeometryType() == m_pGeometry->getType())
			{
				m_pShape->setGeometry(*m_pGeometry);
			}
			else
			{
				ReleasePxShape();
				if(m_pEntity != NULL)
					m_pEntity->UpdateEntityMass();
			}
		}
		else
		{
			if(m_pEntity != NULL)
				m_pEntity->OnShapeReBuild(this);
		}
	}

	bool PhysicsConvexShape::CreateShape( GPtr<PhysicsBodyComponent> component )
	{
		Math::float3* pPosData = NULL;  
		SizeT nVertexCount = 0;

		uint* pIndex32 = NULL;
		ushort* pIndex16 = NULL;

		SizeT indexCount = 0; 
		SizeT triCount = 0; 
		GPtr<MeshRes> physXMeshRes(NULL);

		if (m_UseActorMesh && component.isvalid())
		{
			GPtr<Component> comObject = component->GetActor()->FindComponent( MeshRenderComponent::RTTI );
			if (!comObject.isvalid() )
			{
				n_warning("MeshRenderComponent is not exist when create phyx mesh collider, changed to default!");
				m_UseActorMesh = false;
				return CreateShape(component);// Create a default shape
			}
			GPtr<MeshRenderComponent> pMeshComponent = comObject.downcast<MeshRenderComponent>();
			const GPtr<MeshRes>& meshRes = pMeshComponent->GetMesh();
			m_MeshFileName = pMeshComponent->GetMeshID().AsString();
			if(!meshRes.isvalid())
			{
				n_warning("meshRes is empty when create phyx mesh collider, changed to default!");
				m_UseActorMesh = false;
				return CreateShape(component);// Create a default shape
			}
			else if (meshRes->GetState() != Resource::Loaded)
			{
				CreatePhysicsMeshFromFile(m_MeshFileName);
			}
			pPosData     = meshRes->GetVertexData<PositionData>();
			nVertexCount = meshRes->GetVertexCount();
			indexCount   = meshRes->GetIndexCount();
			triCount     = indexCount / 3;
			physXMeshRes = meshRes;
		}
		else
		{
			if (m_MeshFileName=="")
			{
				m_MeshFileName = "sys:box.mesh";
			}
			GPtr<Resources::PrimitiveResInfo> primitiveResInfo=CreatePhysicsMeshFromFile(m_MeshFileName);

			if ( !(primitiveResInfo->GetRes().downcast<Resources::MeshRes>()->GetState() == Resource::Loaded) )
			{
				n_warning("Create Shape failed. read meshRes from file failed when create phyx mesh collider");
				return false;
			}
			pPosData     = primitiveResInfo->GetRes().downcast<Resources::MeshRes>()->GetVertexData<PositionData>();
			nVertexCount = primitiveResInfo->GetRes().downcast<Resources::MeshRes>()->GetVertexCount();
			indexCount   = primitiveResInfo->GetRes().downcast<Resources::MeshRes>()->GetIndexCount();
			triCount     = indexCount / 3;
			physXMeshRes = primitiveResInfo->GetRes().downcast<Resources::MeshRes>();
		}

		if (nVertexCount > 255)
		{
			n_warning("Create Shape failed. The model used in the shape is too complex.You should make sure that the vertex in the model is less than 255 !!");
			return false;
		}

		if (indexCount > 65535)
		{
			pIndex32 = physXMeshRes->GetIndex32();
			n_assert(pIndex32);
		} 
		else
		{
			pIndex16 = physXMeshRes->GetIndex16();
			n_assert(pIndex16);
		}

		PxConvexMesh* _pPxMesh = CreatePxMesh(pPosData, nVertexCount, indexCount, pIndex32, pIndex16);
		if ( _pPxMesh )
		{
			Math::float3 _realScale = _GetRealScale();
			m_pGeometry = new PxConvexMeshGeometry(_pPxMesh, PxMeshScale((PxVec3&)_realScale, PxQuat(0.f,0.f,0.f,1.f)));
			if ( ((PxConvexMeshGeometry*)m_pGeometry)->isValid() )
			{
				m_RightScale = _realScale;
			}
			else
			{
				n_warning("Could not generate triangle mesh, reset to last operation \n");
				((PxConvexMeshGeometry*)m_pGeometry)->scale.scale = Float3ToPxVec(m_RightScale);
			}
		}
		else
		{
			return false;
		}
		return true;
	}

	PxConvexMesh* PhysicsConvexShape::CreatePxMesh( Math::float3* pData, SizeT nVertexCount, SizeT nIndexCount, uint* pIndex32, ushort* pIndex16 )
	{
		Util::Array<Math::float3> cookedPosData;
		cookedPosData.Reserve(nVertexCount);

		n_assert(pData);

		for (IndexT i = 0; i<nVertexCount; ++i)
		{
			Math::float3 pos( pData[i].x(), pData[i].y(), pData[i].z());
			cookedPosData.Append(pos);
		}
		PxConvexMeshDesc convexDesc;
		convexDesc.points.count     = nVertexCount;
		convexDesc.points.stride    = sizeof(Math::float3);
		convexDesc.points.data      = &cookedPosData[0];
		convexDesc.triangles.count  = nIndexCount/3;

		if (nIndexCount > 65535)
		{
			convexDesc.triangles.data       = pIndex32;
			convexDesc.triangles.stride     = 3*sizeof(uint);

		}
		else
		{
			convexDesc.triangles.data       = pIndex16;
			convexDesc.triangles.stride     = 3*sizeof(ushort);
			convexDesc.flags               |= PxConvexFlag::e16_BIT_INDICES;
		}

		PxCooking* pCooking = PhysicsServer::Instance()->GetCookingInterface();

		MemoryOutputStream buf;
		bool ret = convexDesc.isValid();
		try
		{
			ret = pCooking->cookConvexMesh(convexDesc, buf);
		}
		catch (...)
		{
			ret = false;
		}

		if (ret)
		{
			MemoryInputData input(buf.getData(),buf.getSize());
			return PhysicsServer::Instance()->GetPhysics()->createConvexMesh(input);
		}
		n_warning("Create Shape failed. The model is not a convex mesh!");
		return false;
	}

	GPtr<Resources::PrimitiveResInfo> PhysicsConvexShape::CreatePhysicsMeshFromFile( const Util::String& fileName )
	{
		if (! fileName.IsEmpty() )
		{
			return ResourceManager::Instance()->CreatePrimitiveInfo(fileName, 0);
		}
		return NULL;
	}

	void PhysicsConvexShape::SetScaleDo( Math::float3 scale )
	{
		if ( m_pGeometry )
		{
			((PxConvexMeshGeometry*)m_pGeometry)->scale.scale = Float3ToPxVec(scale);
			if ( m_pShape && m_pShape->getGeometryType() == m_pGeometry->getType() )
			{
				m_pShape->setGeometry(*m_pGeometry);
				m_bMassUpdate = true;
			}
		}
	}

	Math::float3 PhysicsConvexShape::_GetRealScale()
	{
		if (Math::n_nearequal(m_ActorScale.x(), 0.0f, MinPrecision))
		{
			m_ActorScale.x()= 0.0f;
		}
		if (Math::n_nearequal(m_ActorScale.y(), 0.0f, MinPrecision))
		{
			m_ActorScale.y() = 0.0f;
		}
		if (Math::n_nearequal(m_ActorScale.z(), 0.0f, MinPrecision))
		{
			m_ActorScale.z()= 0.0f;
		}

		Math::float3 scale = m_ActorScale * m_Scale;
		scale.set(Math::n_abs(scale.x()), Math::n_abs(scale.y()), Math::n_abs(scale.z()));
		scale.x() = Math::n_nearequal(scale.x(), 0.0f, MinPrecision)? MinPrecision:scale.x();
		scale.y() = Math::n_nearequal(scale.y(), 0.0f, MinPrecision)? MinPrecision:scale.y();
		scale.z() = Math::n_nearequal(scale.z(), 0.0f, MinPrecision)? MinPrecision:scale.z();

		if ( scale.x() > 10000.f)
		{
			scale.x() = 10000.f;
		}
		if ( scale.y() > 10000.f)
		{
			scale.y() = 10000.f;
		}
		if ( scale.z() > 10000.f)
		{
			scale.z() = 10000.f;
		}
		return scale;
	}

}
#endif