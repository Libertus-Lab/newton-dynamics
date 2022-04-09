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

#ifndef __CU_SOLVER_TYPES_H__
#define __CU_SOLVER_TYPES_H__

#include <cuda.h>
#include <vector_types.h>
#include <cuda_runtime.h>
#include <ndNewtonStdafx.h>

#include "cuVector.h"

class cuSpatialVector
{
	public:
	cuVector m_linear;
	cuVector m_angular;
};

class cuBoundingBox
{
	public:
	cuVector m_min;
	cuVector m_max;
};

class cuAabbGridHash
{
	public:
	union
	{
		int4 m_key;
		char m_bytes[4 * 3];
		struct
		{
			int m_x;
			int m_y;
			int m_z;
			int m_id;
		};
	};
};

template <typename Predicate>
__global__ void CudaSortHistogram(Predicate EvaluateKey, const cuAabbGridHash* array, int* histogram, int size, int digit)
{
	__shared__  int cacheBuffer[D_THREADS_PER_BLOCK];

	cacheBuffer[threadIdx.x] = 0;
	__syncthreads();
	int threadIndex = threadIdx.x;
	int index = threadIndex + blockDim.x * blockIdx.x;
	if (index < size)
	{
		int key = EvaluateKey(array[index], digit);
		atomicAdd(&cacheBuffer[key], 1);
	}
	__syncthreads();

	histogram[index] = cacheBuffer[threadIndex];
}

template <typename Predicate>
__global__ void CudaSortPrefixScans(Predicate PrefixScan, int* scan, int size)
{
	PrefixScan(scan, size);
}

class CudaCountingSort
{
	public:
	CudaCountingSort(int* histogram, int size, cudaStream_t stream)
		:m_histogram(histogram)
		,m_stream(stream)
		,m_size(size)
		,m_blocks((m_size + D_THREADS_PER_BLOCK - 1) / D_THREADS_PER_BLOCK)
	{
	}

	void Sort(cuAabbGridHash* const src, cuAabbGridHash* const dst)
	{
		for (int i = 0; i < 3; i++)
		{
			Sort(dst, src, i * 4 + 0);
			Sort(src, dst, i * 4 + 1);
		}
	}

	// Cuda does not let provate of protected lamdda to be passes as argumnet of kerners.
	public:
	void Sort(const cuAabbGridHash* const src, cuAabbGridHash* const dst, int digit)
	{
		auto EvaluateKey = [] __device__(const cuAabbGridHash & dataElement, int digit)
		{
			return dataElement.m_bytes[digit];
		};

		auto PrefixScanSum = [] __device__(int* scan, int size)
		{
			__shared__  int cacheBuffer[2 * D_THREADS_PER_BLOCK + 1];
			//__shared__  int xxxx[D_THREADS_PER_BLOCK];
			//__shared__  int xxxx1[D_THREADS_PER_BLOCK];

			int threadId = threadIdx.x;
			int threadId1 = threadId + D_THREADS_PER_BLOCK;

			//xxxx[threadId] = scan[threadId];
			int sum = 0;
			cacheBuffer[threadId] = 0;
			if (threadId == 0)
			{
				cacheBuffer[threadId1] = 0;
			}
			const int blocks = size / D_THREADS_PER_BLOCK;
			for (int i = 0; i < blocks; i++)
			{
				sum = scan[i * D_THREADS_PER_BLOCK + threadId];
			}
			//xxxx[threadId] = sum;
			cacheBuffer[threadId1 + 1] = sum;
			__syncthreads();
			
			for (int i = 1; i < D_THREADS_PER_BLOCK; i = i << 1)
			{
				int sum = cacheBuffer[threadId1] + cacheBuffer[threadId1 - i];
				__syncthreads();
				cacheBuffer[threadId1] = sum;
				__syncthreads();
			}
			sum = cacheBuffer[threadId1];
			//xxxx1[threadId] = sum;

			for (int i = 0; i < blocks; i++)
			{
				int partialSum = scan[i * D_THREADS_PER_BLOCK + threadId];
				//__syncthreads();
				scan[i * D_THREADS_PER_BLOCK + threadId] = sum;
				//xxxx1[threadId] = sum;
				sum += partialSum;
			}
		};

		CudaSortHistogram << <m_blocks, D_THREADS_PER_BLOCK, 0, m_stream >> > (EvaluateKey, src, m_histogram, m_size, digit);
		CudaSortPrefixScans << <1, D_THREADS_PER_BLOCK, 0, m_stream >> > (PrefixScanSum, m_histogram, m_size);
	}

	int* m_histogram;
	cudaStream_t m_stream;
	int m_size;
	int m_blocks;
};


#endif