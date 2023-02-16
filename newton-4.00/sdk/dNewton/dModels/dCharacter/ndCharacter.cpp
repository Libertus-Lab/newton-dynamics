/* Copyright (c) <2003-2022> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "ndCoreStdafx.h"
#include "ndNewtonStdafx.h"
#include "ndWorld.h"
#include "ndCharacter.h"
#include "ndBodyDynamic.h"
#include "ndCharacterNode.h"
#include "ndIk6DofEffector.h"
#include "ndCharacterRootNode.h"
#include "ndCharacterForwardDynamicNode.h"
#include "ndCharacterInverseDynamicNode.h"

ndCharacter::ndCharacter()
	:ndModel()
	,m_rootNode(nullptr)
	,m_effectors()
	,m_extraJointAttachments()
{
}

ndCharacter::~ndCharacter()
{
	for (ndList<ndEffetorInfo>::ndNode* node = m_effectors.GetFirst(); node; node = node->GetNext())
	{
		ndJointBilateralConstraint* const joint = node->GetInfo().m_effector;
		delete joint;
	}

	for (ndList<ndJointBilateralConstraint*>::ndNode* node = m_extraJointAttachments.GetFirst(); node; node = node->GetNext())
	{
		ndJointBilateralConstraint* const joint = node->GetInfo();
		delete joint;
	}

	if (m_rootNode)
	{
		delete m_rootNode;
	}
}

ndCharacterRootNode* ndCharacter::CreateRoot(ndBodyDynamic* const body)
{
	m_rootNode = new ndCharacterRootNode(this, body);
	return m_rootNode;
}

ndCharacterForwardDynamicNode* ndCharacter::CreateForwardDynamicLimb(const ndMatrix& matrixInGlobalSpase, ndBodyDynamic* const body, ndCharacterNode* const parent)
{
	ndCharacterForwardDynamicNode* const limb = new ndCharacterForwardDynamicNode(matrixInGlobalSpase, body, parent);
	return limb;
}

ndCharacterInverseDynamicNode* ndCharacter::CreateInverseDynamicLimb(const ndMatrix& matrixInGlobalSpace, ndBodyDynamic* const body, ndCharacterNode* const parent)
{
	ndCharacterInverseDynamicNode* const limb = new ndCharacterInverseDynamicNode(matrixInGlobalSpace, body, parent);
	return limb;
}

//ndCharacterEffectorNode* ndCharacter::CreateInverseDynamicEffector(const ndMatrix& matrixInGlobalSpace, ndCharacterNode* const parent)
//{
//	ndCharacterEffectorNode* const effector = new ndCharacterEffectorNode(matrixInGlobalSpace, parent);
//	return effector;
//}

//ndCharacterCentreOfMassState ndCharacter::CalculateCentreOfMassState() const
//{
//	ndInt32 stack = 1;
//	ndCharacterNode* nodePool[32];
//	
//	nodePool[0] = m_rootNode;
//
//	ndVector com(ndVector::m_zero);
//	ndVector veloc(ndVector::m_zero);
//	ndFloat32 mass = ndFloat32(0.0f);
//
//	while (stack)
//	{
//		stack--;
//		ndCharacterNode* const node = nodePool[stack];
//		if (!node->GetAsEffectorNode())
//		{
//			ndBodyDynamic* const body = node->GetBody();
//			if (body)
//			{
//				ndFloat32 partMass = body->GetMassMatrix().m_w;
//				mass += partMass;
//				ndMatrix bodyMatrix(body->GetMatrix());
//				com += bodyMatrix.TransformVector(body->GetCentreOfMass()).Scale(partMass);
//				veloc += body->GetVelocity().Scale(partMass);
//			}
//		}
//
//		for (ndCharacterNode* child = node->GetChild(); child; child = child->GetSibling())
//		{
//			nodePool[stack] = child;
//			stack++;
//		}
//	}
//	//inertia.m_posit.m_w = ndFloat32(1.0f);
//	ndVector invMass (ndFloat32(1.0f) / mass);
//	com = com * invMass;
//	veloc = veloc * invMass;
//	com.m_w = ndFloat32(1.0f);
//	veloc.m_w = ndFloat32(0.0f);
//
//	ndCharacterCentreOfMassState state;
//	state.m_mass = mass;
//	state.m_centerOfMass = com;
//	state.m_centerOfMassVeloc = veloc;
//	return state;
//}

void ndCharacter::Debug(ndConstraintDebugCallback& context) const
{
	ndFloat32 scale = context.GetScale();
	context.SetScale(scale * 0.25f);

	for (ndList<ndEffetorInfo>::ndNode* node = m_effectors.GetFirst(); node; node = node->GetNext())
	{
		ndJointBilateralConstraint* const joint = node->GetInfo().m_effector;
		joint->DebugJoint(context);
		node->GetInfo().m_controlNode->GetJoint()->DebugJoint(context);
	}

	if (m_rootNode)
	{
		m_rootNode->Debug(context);
	}

	//if (m_controller)
	//{
	//	m_controller->Debug(context);
	//}
	context.SetScale(scale);
}

void ndCharacter::AddAttachment(ndJointBilateralConstraint* const joint)
{
	m_extraJointAttachments.Append(joint);
}

void ndCharacter::RemoveAttachment(ndJointBilateralConstraint* const joint)
{
	for (ndList<ndJointBilateralConstraint*>::ndNode* node = m_extraJointAttachments.GetFirst(); node; node = node->GetNext())
	{
		if (node->GetInfo() == joint)
		{
			m_extraJointAttachments.Remove(node);
			break;
		}
	}
}

//void ndCharacter::CreateKinematicChain(const ndMatrix& globalOrientation, const ndCharacterNode* const footNode)
void ndCharacter::CreateKinematicChain(const ndMatrix&, const ndCharacterNode* const)
{
	ndAssert(0);
	//ndCharacterNode* const calf = footNode->GetParent();
	//ndCharacterNode* const leg = calf->GetParent();
	//ndCharacterNode* const hip = leg->GetParent();
	//
	//ndMatrix pinAndPivot(globalOrientation);
	//ndVector hipPivot(leg->GetBody()->GetMatrix().m_posit);
	//pinAndPivot.m_posit = footNode->GetBody()->GetMatrix().m_posit;
	//ndJointKinematicChain* const joint = new ndJointKinematicChain(hipPivot, pinAndPivot, footNode->GetBody(), hip->GetBody());
	//
	//
	//ndEffetorInfo& info = m_effectors.Append()->GetInfo();
	////ndMatrix localPivot1(footNode->GetBody()->GetMatrix() * hip->GetBody()->GetMatrix().Inverse());
	////ndMatrix localPivot0(pinAndPivot * hip->GetBody()->GetMatrix().Inverse());
	//
	//const ndMatrix localPivot0(pinAndPivot);
	//const ndMatrix localPivot1(footNode->GetBody()->GetMatrix());
	//
	//info.m_bindMatrix = localPivot0 * localPivot1.Inverse();
	//info.m_effector = joint;
	//info.m_controlNode = (ndCharacterNode*)footNode;
}

//void ndCharacter::UpdateGlobalPose(ndWorld* const world, ndFloat32 timestep)
//{
//	ndInt32 stack = 1;
//	ndCharacterNode* nodePool[32];
//	nodePool[0] = m_rootNode;
//
//	while (stack)
//	{
//		stack--;
//		ndCharacterNode* const node = nodePool[stack];
//		node->UpdateGlobalPose(world, timestep);
//
//		for (ndCharacterNode* child = node->GetChild(); child; child = child->GetSibling())
//		{
//			nodePool[stack] = child;
//			stack++;
//		}
//	}
//}

//void ndCharacter::CalculateLocalPose(ndWorld* const world, ndFloat32 timestep)
//{
//	ndInt32 stack = 1;
//	ndCharacterNode* nodePool[32];
//	nodePool[0] = m_rootNode;
//
//	while (stack)
//	{
//		stack--;
//		ndCharacterNode* const node = nodePool[stack];
//		node->CalculateLocalPose(world, timestep);
//
//		for (ndCharacterNode* child = node->GetChild(); child; child = child->GetSibling())
//		{
//			nodePool[stack] = child;
//			stack++;
//		}
//	}
//}

void ndCharacter::PostUpdate(ndWorld* const, ndFloat32)
{
}

//void ndCharacter::Update(ndWorld* const world, ndFloat32 timestep)
void ndCharacter::Update(ndWorld* const, ndFloat32)
{
	//if (m_controller)
	//{
	//	m_controller->Evaluate(world, timestep);
	//}
}

//ndCharacterSkeleton* ndCharacter::CreateSkeleton() const
//{
//	ndAssert(0);
//	return nullptr;
//	//ndInt32 stack = 0;
//	//ndCharacterNode* nodePool[32];
//	//ndCharacterSkeleton* parentPool[32];
//	//
//	//ndCharacterSkeleton* const skeleton = new ndCharacterSkeleton(m_rootNode, dGetIdentityMatrix(), nullptr);
//	//for (ndCharacterNode* child = m_rootNode->GetChild(); child; child = child->GetSibling())
//	//{
//	//	nodePool[stack] = child;
//	//	parentPool[stack] = skeleton;
//	//	stack++;
//	//}
//	//
//	//while (stack)
//	//{
//	//	stack--;
//	//	ndCharacterNode* const node = nodePool[stack];
//	//	ndCharacterSkeleton* const boneParent = parentPool[stack];
//	//	ndMatrix matrix(node->GetBoneMatrix() * boneParent->m_node->GetBoneMatrix().Inverse());
//	//	ndCharacterSkeleton* const parent = new ndCharacterSkeleton(node, matrix, boneParent);
//	//
//	//	for (ndCharacterNode* child = node->GetChild(); child; child = child->GetSibling())
//	//	{
//	//		nodePool[stack] = child;
//	//		parentPool[stack] = parent;
//	//		stack++;
//	//	}
//	//}
//	//
//	//return skeleton;
//}

void ndCharacter::SetPose()
{
	ndAssert(0);
	//for (ndList<ndEffetorInfo>::ndNode* node = m_effectors.GetFirst(); node; node = node->GetNext())
	//{
	//	ndEffetorInfo& info = node->GetInfo();
	//	ndJointKinematicChain* const joint = info.m_effector;
	//	
	//	ndMatrix matrix(info.m_bindMatrix * info.m_controlNode->GetLocalPose());
	//	for (ndCharacterNode* bone = info.m_controlNode->GetParent(); bone->GetBody() != joint->GetBody1(); bone = bone->GetParent())
	//	{
	//		matrix = matrix * bone->GetLocalPose();
	//	}
	//	joint->SetTargetLocalMatrix(matrix);
	//}
}