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
#include "ndCharacter.h"
#include "ndBodyDynamic.h"
#include "ndJointSpherical.h"
#include "ndCharacterInverseDynamicNode.h"

ndCharacterInverseDynamicNode::ndCharacterInverseDynamicNode(const ndMatrix& matrixInGlobalSpace, ndBodyDynamic* const body, ndCharacterNode* const parent)
	:ndCharacterNode(parent)
	,m_body(body)
	,m_joint(new ndJointSpherical(matrixInGlobalSpace, body, parent->GetBody()))
{
	m_localPose = m_body->GetMatrix() * parent->GetBody()->GetMatrix().Inverse();
}

ndCharacterInverseDynamicNode::~ndCharacterInverseDynamicNode()
{
	delete m_joint;
	delete m_body;
}
