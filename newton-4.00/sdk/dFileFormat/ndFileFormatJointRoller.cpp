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

#include "ndFileFormatStdafx.h"
#include "ndFileFormatJointRoller.h"

ndFileFormatJointRoller::ndFileFormatJointRoller()
	:ndFileFormatJoint(ndJointRoller::StaticClassName())
{
}

ndFileFormatJointRoller::ndFileFormatJointRoller(const char* const className)
	:ndFileFormatJoint(className)
{
}

void ndFileFormatJointRoller::SaveJoint(ndFileFormatSave* const scene, nd::TiXmlElement* const parentNode, const ndJointBilateralConstraint* const joint)
{
	nd::TiXmlElement* const classNode = xmlCreateClassNode(parentNode, "ndJointClass", ndJointRoller::StaticClassName());
	ndFileFormatJoint::SaveJoint(scene, classNode, joint);

	ndFloat32 spring;
	ndFloat32 damper;
	ndFloat32 regularizer;
	ndFloat32 minTwistAngle; 
	ndFloat32 maxTwistAngle;
	
	ndFloat32 spring1;
	ndFloat32 damper1;
	ndFloat32 regularizer1;
	ndFloat32 minPositLimit;
	ndFloat32 maxPositLimit;

	ndJointRoller* const exportJoint = (ndJointRoller*)joint;

	exportJoint->GetLimitsAngle(minTwistAngle, maxTwistAngle);
	exportJoint->GetLimitsPosit(minPositLimit, maxPositLimit);
	exportJoint->GetSpringDamperAngle(regularizer, spring, damper);
	exportJoint->GetSpringDamperPosit(regularizer1, spring1, damper1);
	
	xmlSaveParam(classNode, "offsetAngle", exportJoint->GetOffsetAngle() * ndRadToDegree);
	xmlSaveParam(classNode, "springConstantAngle", spring);
	xmlSaveParam(classNode, "damperConstantAngle", damper);
	xmlSaveParam(classNode, "springRegularizerAngle", regularizer);
	xmlSaveParam(classNode, "minTwistAngle", minTwistAngle * ndRadToDegree);
	xmlSaveParam(classNode, "maxTwistAngle", minTwistAngle * ndRadToDegree);
	xmlSaveParam(classNode, "limitStateAngle", exportJoint->GetLimitStateAngle() ? 1 : 0);
	
	xmlSaveParam(classNode, "offsetPosit", exportJoint->GetOffsetPosit());
	xmlSaveParam(classNode, "springConstantPosit", spring1);
	xmlSaveParam(classNode, "damperConstantPosit", damper1);
	xmlSaveParam(classNode, "springRegularizerPosit", regularizer1);
	xmlSaveParam(classNode, "minPositLimit", minPositLimit);
	xmlSaveParam(classNode, "maxPositLimit", maxPositLimit);
	xmlSaveParam(classNode, "limitStatePosit", exportJoint->GetLimitStatePosit() ? 1 : 0);
}