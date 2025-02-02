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
#include "ndBrain.h"
#include "ndBrainLoss.h"
#include "ndBrainLayer.h"
#include "ndBrainVector.h"
#include "ndBrainMatrix.h"
#include "ndBrainTrainer.h"
#include "ndBrainLayerSoftmaxActivation.h"

class ndBrainTrainer::ndLayerData : public ndClassAlloc
{
	public:
	ndLayerData(ndBrainLayer* const layer)
		:ndClassAlloc()
		,m_layer(layer)
		,m_gradient(nullptr)
	{
		if (layer->HasParameters())
		{
			m_gradient = layer->Clone();
		}
	}

	~ndLayerData()
	{
		if (m_gradient)
		{
			delete m_gradient;
		}
	}

	void Clear()
	{
		if (m_layer->HasParameters())
		{
			m_gradient->Clear();
		}
	}

	void Add(const ndLayerData& src)
	{
		if (m_layer->HasParameters())
		{
			m_gradient->Add(*src.m_gradient);
		}
	}

	void Scale(ndBrainFloat s)
	{
		if (m_layer->HasParameters())
		{
			m_gradient->Scale(s);
		}
	}

	ndBrainLayer* m_layer;
	ndBrainLayer* m_gradient;
};

ndBrainTrainer::ndBrainTrainer(ndBrain* const brain)
	:ndClassAlloc()
	,m_data()
	,m_brain(brain)
{
	for (ndInt32 i = 0; i < m_brain->GetCount(); ++i)
	{
		m_data.PushBack(new ndLayerData((*m_brain)[i]));
	}
}

ndBrainTrainer::ndBrainTrainer(const ndBrainTrainer& src)
	:ndClassAlloc()
	,m_data()
	,m_brain(src.m_brain)
{
	ndAssert(0);
	for (ndInt32 i = 0; i < m_brain->GetCount(); ++i)
	{
		m_data.PushBack(new ndLayerData((*m_brain)[i]));
	}
}

ndBrainTrainer::~ndBrainTrainer()
{
	for (ndInt32 i = 0; i < m_data.GetCount(); ++i)
	{
		delete (m_data[i]);
	}
}

ndBrain* ndBrainTrainer::GetBrain() const
{
	return m_brain;
}

ndBrainLayer* ndBrainTrainer::GetWeightsLayer(ndInt32 index) const
{
	return m_data[index]->m_layer;
}

ndBrainLayer* ndBrainTrainer::GetGradientLayer(ndInt32 index) const
{
	return m_data[index]->m_gradient;
}

void ndBrainTrainer::AcculumateGradients(const ndBrainTrainer& src, ndInt32 index)
{
	ndLayerData* const dstData = m_data[index];
	ndAssert(dstData->m_layer->HasParameters());
	const ndLayerData* const srcData = src.m_data[index];
	dstData->Add(*srcData);
}

void ndBrainTrainer::ClearGradients()
{
	for (ndInt32 i = m_data.GetCount() - 1; i >= 0; --i)
	{
		m_data[i]->Clear();
	}
}

void ndBrainTrainer::AddGradients(const ndBrainTrainer* const src)
{
	for (ndInt32 i = m_data.GetCount() - 1; i >= 0; --i)
	{
		m_data[i]->Add(*src->m_data[i]);
	}
}

void ndBrainTrainer::ScaleWeights(const ndBrainFloat s)
{
	for (ndInt32 i = m_data.GetCount() - 1; i >= 0; --i)
	{
		m_data[i]->Scale(s);
	}
}

void ndBrainTrainer::BackPropagate(const ndBrainVector& input, ndBrainLoss& loss)
{
	ndFixSizeArray<ndInt32, 256> prefixScan;
	const ndArray<ndBrainLayer*>& layers = *m_brain;

	ndInt32 maxSize = 0;
	const ndInt32 layersCount = m_brain->GetCount();
	ndInt32 sizeAcc = (layers[0]->GetInputSize() + 7) & -8;

	ndAssert(!(loss.IsCategorical() ^ (!strcmp(layers[layersCount - 1]->GetLabelId(), "ndBrainLayerCategoricalSoftmaxActivation"))));

	prefixScan.PushBack(0);
	for (ndInt32 i = 0; i < layersCount; ++i)
	{
		prefixScan.PushBack(sizeAcc);
		sizeAcc += (layers[i]->GetOutputSize() + 7) & -8;
		maxSize = ndMax(maxSize, layers[i]->GetOutputSize());
	}
	prefixScan.PushBack(sizeAcc);

	const ndBrainFloat* const memBuffer = ndAlloca(ndBrainFloat, sizeAcc + 8);

	ndBrainMemVector in0(memBuffer, input.GetCount());
	in0.Set(input);

	for (ndInt32 i = 0; i < layersCount; ++i)
	{
		const ndBrainMemVector in(memBuffer + prefixScan[i + 0], layers[i]->GetInputSize());
		ndBrainMemVector out(memBuffer + prefixScan[i + 1], layers[i]->GetOutputSize());
		layers[i]->MakePrediction(in, out);
	}
	const ndBrainMemVector output(memBuffer + prefixScan[layers.GetCount()], m_brain->GetOutputSize());

	const ndBrainFloat* const gradientBuffer = ndAlloca(ndBrainFloat, maxSize * 2 + 256);
	ndBrainMemVector gradientIn(gradientBuffer, m_brain->GetOutputSize());
	ndBrainMemVector gradientOut(gradientBuffer + maxSize + 128, m_brain->GetOutputSize());
	loss.GetLoss(output, gradientOut);

	for (ndInt32 i = m_data.GetCount() - 1; i >= 0; --i)
	{
		const ndBrainLayer* const layer = m_data[i]->m_layer;
		gradientIn.SetSize(layer->GetInputSize());
		const ndBrainMemVector in(memBuffer + prefixScan[i + 0], layer->GetInputSize());
		const ndBrainMemVector out(memBuffer + prefixScan[i + 1], layer->GetOutputSize());
		layer->CalculateParamGradients(in, out, gradientOut, gradientIn, m_data[i]->m_gradient);
		gradientIn.Swap(gradientOut);
	}
}


