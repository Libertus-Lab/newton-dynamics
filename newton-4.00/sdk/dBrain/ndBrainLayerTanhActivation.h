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

#ifndef _ND_BRAIN_LAYER_TANH_ACTIVATION_H__
#define _ND_BRAIN_LAYER_TANH_ACTIVATION_H__

#include "ndBrainStdafx.h"
#include "ndBrainLayerActivation.h"

class ndBrainLayerTanhActivation : public ndBrainLayerActivation
{
	public:
	ndBrainLayerTanhActivation(ndInt32 neurons);
	ndBrainLayerTanhActivation(const ndBrainLayerTanhActivation& src);
	ndBrainLayer* Clone() const;
	static ndBrainLayer* Load(const ndBrainLoad* const loadSave);

	const char* GetLabelId() const;
	void MakePrediction(const ndBrainVector& input, ndBrainVector& output) const;
	void InputDerivative(const ndBrainVector& output, const ndBrainVector& outputDerivative, ndBrainVector& inputDerivative) const;
};

class ndBrainLayerApproximateTanhActivation : public ndBrainLayerTanhActivation
{
	public:
	ndBrainLayerApproximateTanhActivation(ndInt32 neurons);
	ndBrainLayerApproximateTanhActivation(const ndBrainLayerTanhActivation& src);
	ndBrainLayer* Clone() const;
	static ndBrainLayer* Load(const ndBrainLoad* const loadSave);

	const char* GetLabelId() const;
	void MakePrediction(const ndBrainVector& input, ndBrainVector& output) const;

	static ndBrainVector4 m_c1;
	static ndBrainVector4 m_c2;
	static ndBrainVector4 m_max;
	static ndBrainVector4 m_min;
	static ndBrainVector4 m_log2f;
};

#endif 

