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

#include "ndBrainStdafx.h"
#include "ndBrainSaveLoad.h"
#include "ndBrainLayerTanhActivation.h"

ndBrainVector4 ndBrainLayerApproximateTanhActivation::m_max(ndBrainFloat(10.f));
ndBrainVector4 ndBrainLayerApproximateTanhActivation::m_min(ndBrainFloat(-10.f));
ndBrainVector4 ndBrainLayerApproximateTanhActivation::m_c1(ndBrainFloat(0.03138777f));
ndBrainVector4 ndBrainLayerApproximateTanhActivation::m_c2(ndBrainFloat(0.276281267f));
ndBrainVector4 ndBrainLayerApproximateTanhActivation::m_log2f(ndBrainFloat(1.442695022f));

ndBrainLayerTanhActivation::ndBrainLayerTanhActivation(ndInt32 neurons)
	:ndBrainLayerActivation(neurons)
{
}

ndBrainLayerTanhActivation::ndBrainLayerTanhActivation(const ndBrainLayerTanhActivation& src)
	:ndBrainLayerActivation(src)
{
}

ndBrainLayer* ndBrainLayerTanhActivation::Clone() const
{
	return new ndBrainLayerTanhActivation(*this);
}

const char* ndBrainLayerTanhActivation::GetLabelId() const
{
	return "ndBrainLayerTanhActivation";
}

ndBrainLayer* ndBrainLayerTanhActivation::Load(const ndBrainLoad* const loadSave)
{
	char buffer[1024];
	loadSave->ReadString(buffer);

	loadSave->ReadString(buffer);
	ndInt32 inputs = loadSave->ReadInt();
	ndBrainLayerTanhActivation* const layer = new ndBrainLayerTanhActivation(inputs);
	loadSave->ReadString(buffer);
	return layer;
}

void ndBrainLayerTanhActivation::MakePrediction(const ndBrainVector& input, ndBrainVector& output) const
{
	ndAssert(input.GetCount() == output.GetCount());
	for (ndInt32 i = input.GetCount() - 1; i >= 0; --i)
	{
		output[i] = ndBrainFloat (ndTanh(input[i]));
	}
	output.FlushToZero();
}

void ndBrainLayerTanhActivation::InputDerivative(const ndBrainVector& output, const ndBrainVector& outputDerivative, ndBrainVector& inputDerivative) const
{
	ndAssert(output.GetCount() == outputDerivative.GetCount());
	ndAssert(output.GetCount() == inputDerivative.GetCount());

	inputDerivative.Set(ndBrainFloat(1.0f));
	inputDerivative.MulSub(output, output);
	inputDerivative.Mul(outputDerivative);
	inputDerivative.FlushToZero();
}

ndBrainLayerApproximateTanhActivation::ndBrainLayerApproximateTanhActivation(ndInt32 neurons)
	:ndBrainLayerTanhActivation(neurons)
{
}

ndBrainLayerApproximateTanhActivation::ndBrainLayerApproximateTanhActivation(const ndBrainLayerTanhActivation& src)
	:ndBrainLayerTanhActivation(src)
{
}

ndBrainLayer* ndBrainLayerApproximateTanhActivation::Clone() const
{
	return new ndBrainLayerApproximateTanhActivation(*this);
}

ndBrainLayer* ndBrainLayerApproximateTanhActivation::Load(const ndBrainLoad* const loadSave)
{
	char buffer[1024];
	loadSave->ReadString(buffer);

	loadSave->ReadString(buffer);
	ndInt32 inputs = loadSave->ReadInt();
	ndBrainLayerApproximateTanhActivation* const layer = new ndBrainLayerApproximateTanhActivation(inputs);
	loadSave->ReadString(buffer);
	return layer;
}

const char* ndBrainLayerApproximateTanhActivation::GetLabelId() const
{
	return "ndBrainLayerApproximateTanhActivation";
}

//#if defined (D_SCALAR_VECTOR_CLASS) || (defined (D_NEWTON_USE_DOUBLE) && defined (D_BRAIN_USES_REAL))
#if defined (D_NEWTON_USE_DOUBLE)
void ndBrainLayerApproximateTanhActivation::MakePrediction(const ndBrainVector& input, ndBrainVector& output) const
{
	ndBrainLayerTanhActivation::MakePrediction(input, output);
}
#else

void ndBrainLayerApproximateTanhActivation::MakePrediction(const ndBrainVector& input, ndBrainVector& output) const
{
	// rational approximation of tanh, approximation 4 time faster that standard tanh.
	// error bound lower than 2.0e-6 for the entire rage[-10, 10] 
	// only problem is that is not exact zero for input zero, 
	// however this can be very good for dense hidden layers, 
	// in fact it seems to produce better or equal result than the standard tanh  

#if 0
	auto ScalarTanhApproximation = [](ndBrainFloat in)
	{
		const ndBrainFloat c1 = ndReal(0.03138777f);
		const ndBrainFloat c2 = ndReal(0.276281267f);
		const ndBrainFloat log2f = ndReal(1.442695022f);

		ndBrainFloat v = log2f * ndClamp(in, ndBrainFloat(-10.0f), ndBrainFloat(10.0f));
		ndBrainFloat floatIntPart = ndBrainFloat(ndFloor(v));
		ndBrainFloat x = v - floatIntPart;
		ndBrainFloat xx = x * x;
		ndBrainFloat v1 = log2f + c2 * xx;
		ndBrainFloat v2 = x + xx * c1 * x;
		ndBrainFloat v3 = v2 + v1;
		ndBrainFloat v4 = v2 - v1;
		#ifdef D_BRAIN_USES_REAL
			* ((ndInt32*)&v3) += ndInt32(floatIntPart) << 24;
		#else
			* ((ndInt64*)&v3) += ndInt64(floatIntPart) << 53;
		#endif
		return (v3 + v4) / (v3 - v4);
	};

	auto VectorTanhApproximation = [](const ndBrainVector4& in)
	{
		ndBrainVector4 v(m_log2f * in.GetMin(m_max).GetMax(m_min));
		ndBrainVector4 intPart(v.GetInt());
		ndBrainVector4 x(v - v.Floor());
		ndBrainVector4 xx(x * x);
		ndBrainVector4 v1(m_log2f + m_c2 * xx);
		ndBrainVector4 v2(x + xx * m_c1 * x);
		ndBrainVector4 v3(v2 + v1);
		for (ndInt32 i = 0; i < 4; ++i)
		{
			#ifdef D_BRAIN_USES_REAL
				v3.m_i[i] += intPart.m_i[i] << 24;
			#else
				v3.m_i[i] += intPart.m_i[i] << 53;
			#endif
		}
		ndBrainVector4 v4(v2 - v1);
		ndBrainVector4 num(v3 + v4);
		ndBrainVector4 den(v3 - v4);
		return num.Divide(den);
	};

#else

	const ndBrainVector4 c1(m_c1);
	const ndBrainVector4 c2(m_c2);
	const ndBrainVector4 min(m_min);
	const ndBrainVector4 max(m_max);
	const ndBrainVector4 log2f(m_log2f);

	auto ScalarTanhApproximation = [c1, c2, min, max, log2f](ndBrainFloat in)
	{
		ndBrainFloat v = log2f[0] * ndClamp(in, min[0], max[0]);
		ndBrainFloat floatIntPart = ndBrainFloat(ndFloor(v));
		ndBrainFloat x = v - floatIntPart;
		ndBrainFloat xx = x * x;
		ndBrainFloat v1 = log2f[0] + c2[0] * xx;
		ndBrainFloat v2 = x + xx * c1[0] * x;
		ndBrainFloat v3 = v2 + v1;
		ndBrainFloat v4 = v2 - v1;
		#ifdef D_BRAIN_USES_REAL
			* ((ndInt32*)&v3) += ndInt32(floatIntPart) << 24;
		#else
			* ((ndInt64*)&v3) += ndInt64(floatIntPart) << 53;
		#endif
		return (v3 + v4) / (v3 - v4);
	};
	
	auto VectorTanhApproximation = [c1, c2, min, max, log2f](const ndBrainVector4& in)
	{
		ndBrainVector4 v(log2f * in.GetMin(max).GetMax(min));
		ndBrainVector4 intPart(v.GetInt());
		ndBrainVector4 x(v - v.Floor());
		ndBrainVector4 xx(x * x);
		ndBrainVector4 v1(log2f + c2 * xx);
		ndBrainVector4 v2(x + xx * c1 * x);
		ndBrainVector4 v3(v2 + v1);
		for (ndInt32 i = 0; i < 4; ++i)
		{
			#ifdef D_BRAIN_USES_REAL
				v3.m_i[i] += intPart.m_i[i] << 24;
			#else
				v3.m_i[i] += intPart.m_i[i] << 53;
			#endif
		}
		ndBrainVector4 v4(v2 - v1);
		ndBrainVector4 num(v3 + v4);
		ndBrainVector4 den(v3 - v4);
		return num.Divide(den);
	};
#endif

	const ndInt32 count = input.GetCount() / 4;
	ndBrainVector4* const vectorOutput = (ndBrainVector4*)&output[0];
	const ndBrainVector4* const vectorInput = (const ndBrainVector4*)&input[0];

	for (ndInt32 i = count - 1; i >= 0 ; --i)
	{
		vectorOutput[i] = VectorTanhApproximation(vectorInput[i]);
		#if _DEBUG
			// check accuracy, expected error less that 2.0e-6 of all cases.
			ndBrainVector4 xxx0(vectorOutput[i]);
			ndBrainVector4 xxx1 (ndTanh(vectorInput[i].m_x), ndTanh(vectorInput[i].m_y), ndTanh(vectorInput[i].m_z), ndTanh(vectorInput[i].m_w));
			ndAssert(ndAbs(xxx0.m_x - xxx1.m_x) < ndFloat32(2.0e-6f));
			ndAssert(ndAbs(xxx0.m_y - xxx1.m_y) < ndFloat32(2.0e-6f));
			ndAssert(ndAbs(xxx0.m_z - xxx1.m_z) < ndFloat32(2.0e-6f));
			ndAssert(ndAbs(xxx0.m_w - xxx1.m_w) < ndFloat32(2.0e-6f));
		#endif
	}
	for (ndInt32 i = input.GetCount() - 1; i >= count * 4; --i)
	{
		output[i] = ndBrainFloat(ScalarTanhApproximation(input[i]));
		#if _DEBUG
			// check accuracy, expected error less that 2.0e-6 of all cases.
			ndFloat32 xxx0 = output[i];
			ndFloat32 xxx1 = ndTanh(input[i]);
			ndAssert(ndAbs(xxx1 - xxx0) < ndFloat32(2.0e-6f));
		#endif
	}

	output.FlushToZero();
}
#endif