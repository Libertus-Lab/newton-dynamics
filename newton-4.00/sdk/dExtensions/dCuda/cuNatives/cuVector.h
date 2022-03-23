/* Copyright (c) <2003-2021> <Julio Jerez, Newton Game Dynamics>
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

#ifndef __ND_CU_VECTOR_H__
#define __ND_CU_VECTOR_H__

#include <cuda.h>
#include <vector_types.h>
#include <cuda_runtime.h>

#include <ndNewtonStdafx.h>
#include <device_launch_parameters.h>

class cuVector: public float4
{
	public:
	inline __device__ cuVector()
	{
	}

	inline __device__ cuVector(float val)
	{
		x = val;
		y = val;
		z = val;
		w = val;
	}

	inline __device__ cuVector(float val_x, float val_y, float val_z, float val_w)
	{
		x = val_x;
		y = val_y;
		z = val_z;
		w = val_w;
	}

	inline __device__ cuVector(const cuVector& src)
	{
		x = src.x;
		y = src.y;
		z = src.z;
		w = src.w;
	}

	inline cuVector(const ndVector& src)
	{
		x = src.m_x;
		y = src.m_y;
		z = src.m_z;
		w = src.m_w;
	}

	inline float __device__ GetElement(int i) const
	{
		return (&this->x)[i];
	}

	inline void __device__ SetElement(int i, float val)
	{
		(&this->x)[i] = val;
	}

	inline ndVector ToNdVector(const cuVector& src) const
	{
		return ndVector(src.x, src.y, src.z, src.w);
	}
	
	inline cuVector __device__ operator+ (const cuVector& A) const
	{
		return cuVector(x + A.x, y + A.y, z + A.z, w + A.w);
	}

	inline cuVector __device__ operator- (const cuVector& A) const
	{
		return cuVector(x - A.x, y - A.y, z - A.z, w - A.w);
	}

	inline cuVector __device__ operator* (const cuVector& A) const
	{
		return cuVector(x * A.x, y * A.y, z * A.z, w * A.w);
	}

	inline cuVector __device__ Scale(float s) const
	{
		return cuVector(x * s, y * s, z * s, w * s);
	}

	inline float __device__ AddHorizontal() const
	{
		return x + y + z + w;
	}

	inline float __device__ DotProduct(const cuVector& B) const
	{
		return (*this * B).AddHorizontal();
	}

	inline cuVector __device__ CrossProduct(const cuVector& B) const
	{
		return cuVector(y * B.z - z * B.y,
						z * B.x - x * B.z,
						x * B.y - y * B.x,
						w);
	}



	inline cuVector __device__ Normalize() const
	{
		float den = 1.0f / sqrt(DotProduct(*this));
		return cuVector(x * den, y * den, z * den, w * den);
	}
};

#endif