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

#ifndef _ND_BRAIN_AGENT_TD3_TRAINER_H__
#define _ND_BRAIN_AGENT_TD3_TRAINER_H__

#ifdef 	D_USE_FLAT_AGENTS
#include "ndBrainStdafx.h"
#include "ndBrain.h"
#include "ndBrainAgent.h"
#include "ndBrainTrainer.h"
#include "ndBrainReplayBuffer.h"

// this is an implementation of more stable policy gradient for
// continues action controller using deep re enforcement learning. 
// td3 algorithm as described here: 
// https://arxiv.org/pdf/1802.09477.pdf

#define D_TD3_CRITIC_LEARN_RATE		ndReal(0.001f)
#define D_TD3_ACTOR_LEARN_RATE		ndReal(0.0001f)
#define D_TD3_DISCOUNT_FACTOR		ndReal (0.99f)
#define D_TD3_REPLAY_BUFFERSIZE		(1024 * 512)
//#define D_TD3_REPLAY_BUFFERSIZE	(1024)
//#define D_TD3_REPLAY_BASH_SIZE	32
#define D_TD3_REPLAY_BASH_SIZE		64
#define D_TD3_REGULARIZER			ndReal (2.0e-6f)
#define D_TD3_SOFT_TARGET_FACTOR	ndReal (1.0e-3f)
#define D_TD3_ACTION_NOISE_VARIANCE	ndReal (0.05f)

template<ndInt32 statesDim, ndInt32 actionDim>
class ndBrainAgentTD3_Trainer : public ndBrainAgent, public ndBrainThreadPool
{
public:
	ndBrainAgentTD3_Trainer(const ndSharedPtr<ndBrain>& actor, const ndSharedPtr<ndBrain>& critic);
	~ndBrainAgentTD3_Trainer();

	ndReal GetCurrentValue() const;
	ndInt32 GetFramesCount() const;
	ndInt32 GetEposideCount() const;
	ndInt32 GetEpisodeFrames() const;

	ndReal GetLearnRate() const;
	void SetLearnRate(ndReal learnRate);

	ndReal GetActionNoise() const;
	void SetActionNoise(ndReal learnRate);

protected:
	void Step();
	void Optimize();
	void OptimizeStep();
	bool IsTrainer() const;
	bool IsSampling() const;
	bool IsTerminal() const;
	ndReal GetReward() const;
	void PopulateReplayBuffer();
	void SetBufferSize(ndInt32 size);
	ndReal PerturbeAction(ndReal action) const;
	void Save(ndBrainSave* const loadSave) const;

	void BackPropagate();
	void BackPropagateActor(const ndUnsigned32* const bashIndex);
	void BackPropagateCritic(const ndUnsigned32* const bashIndex);
	void CalculateQvalue(const ndBrainVector& state, const ndBrainVector& actions);

	void InitWeights();
	void InitWeights(ndReal weighVariance, ndReal biasVariance);

	ndSharedPtr<ndBrain> m_actor;
	ndSharedPtr<ndBrain> m_critic0;
	ndSharedPtr<ndBrain> m_critic1;

	ndBrain m_targetActor;
	ndBrain m_targetCritic0;
	ndBrain m_targetCritic1;

	ndFixSizeArray<ndSharedPtr<ndBrainTrainer>, D_MAX_THREADS_COUNT> m_actorOptimizer;
	ndFixSizeArray<ndSharedPtr<ndBrainTrainer>, D_MAX_THREADS_COUNT> m_criticOptimizer0;
	ndFixSizeArray<ndSharedPtr<ndBrainTrainer>, D_MAX_THREADS_COUNT> m_criticOptimizer1;

	ndArray<ndInt32> m_bashSamples;
	ndBrainReplayBuffer<ndReal, statesDim, actionDim> m_replayBuffer;
	ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim> m_currentTransition;

	ndReal m_gamma;
	ndReal m_currentQValue;
	ndReal m_actorLearnRate;
	ndReal m_criticLearnRate;
	ndReal m_softTargetFactor;
	ndReal m_actionNoiseVariance;
	ndInt32 m_frameCount;
	ndInt32 m_framesAlive;
	ndInt32 m_eposideCount;
	ndInt32 m_bashBufferSize;
	bool m_collectingSamples;
};

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentTD3_Trainer<statesDim, actionDim>::ndBrainAgentTD3_Trainer(const ndSharedPtr<ndBrain>& actor, const ndSharedPtr<ndBrain>& critic)
	:ndBrainAgent()
	, ndBrainThreadPool()
	, m_actor(actor)
	, m_critic0(critic)
	, m_critic1(critic)
	, m_targetActor(*(*actor))
	, m_targetCritic0(*(*critic))
	, m_targetCritic1(*(*critic))
	, m_bashSamples()
	, m_replayBuffer()
	, m_currentTransition()
	, m_gamma(D_TD3_DISCOUNT_FACTOR)
	, m_currentQValue(ndReal(0.0f))
	, m_actorLearnRate(D_TD3_ACTOR_LEARN_RATE)
	, m_criticLearnRate(D_TD3_CRITIC_LEARN_RATE)
	, m_softTargetFactor(D_TD3_SOFT_TARGET_FACTOR)
	, m_actionNoiseVariance(D_TD3_ACTION_NOISE_VARIANCE)
	, m_frameCount(0)
	, m_framesAlive(0)
	, m_eposideCount(0)
	, m_bashBufferSize(D_TD3_REPLAY_BASH_SIZE)
	, m_collectingSamples(true)
{
	ndInt32 threadCount = ndMin(ndBrainThreadPool::GetMaxThreads(), m_bashBufferSize / 4);
	//threadCount = 1;
	SetThreadCount(threadCount);
	for (ndInt32 i = 0; i < ndBrainThreadPool::GetThreadCount(); ++i)
	{
		m_actorOptimizer.PushBack(new ndBrainTrainer(*m_actor));
		m_criticOptimizer0.PushBack(new ndBrainTrainer(*m_critic0));
		m_criticOptimizer1.PushBack(new ndBrainTrainer(*m_critic1));
		m_actorOptimizer[m_actorOptimizer.GetCount() - 1]->SetRegularizer(D_TD3_REGULARIZER);
		m_criticOptimizer0[m_criticOptimizer0.GetCount() - 1]->SetRegularizer(D_TD3_REGULARIZER);
		m_criticOptimizer1[m_criticOptimizer1.GetCount() - 1]->SetRegularizer(D_TD3_REGULARIZER);
	}

	SetBufferSize(D_TD3_REPLAY_BUFFERSIZE);
	InitWeights();
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentTD3_Trainer<statesDim, actionDim>::~ndBrainAgentTD3_Trainer()
{
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::Save(ndBrainSave* const loadSave) const
{
	loadSave->Save(*m_actor);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::SetBufferSize(ndInt32 size)
{
	m_replayBuffer.SetSize(size);
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentTD3_Trainer<statesDim, actionDim>::IsTrainer() const
{
	return true;
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentTD3_Trainer<statesDim, actionDim>::IsSampling() const
{
	return m_collectingSamples;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndReal ndBrainAgentTD3_Trainer<statesDim, actionDim>::GetCurrentValue() const
{
	return m_currentQValue;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentTD3_Trainer<statesDim, actionDim>::GetFramesCount() const
{
	return m_frameCount;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentTD3_Trainer<statesDim, actionDim>::GetEposideCount() const
{
	return m_eposideCount;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndInt32 ndBrainAgentTD3_Trainer<statesDim, actionDim>::GetEpisodeFrames() const
{
	return m_framesAlive;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndReal ndBrainAgentTD3_Trainer<statesDim, actionDim>::GetActionNoise() const
{
	return m_actionNoiseVariance;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::SetActionNoise(ndReal noiseVariance)
{
	m_actionNoiseVariance = noiseVariance;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::InitWeights()
{
	m_actor->InitWeightsXavierMethod();
	m_critic0->InitWeightsXavierMethod();
	m_critic1->InitWeightsXavierMethod();

	m_targetActor.CopyFrom(*(*m_actor));
	m_targetCritic0.CopyFrom(*(*m_critic0));
	m_targetCritic1.CopyFrom(*(*m_critic1));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::InitWeights(ndReal weighVariance, ndReal biasVariance)
{
	m_actor->InitWeights(weighVariance, biasVariance);
	m_critic0->InitWeights(weighVariance, biasVariance);
	m_critic1->InitWeights(weighVariance, biasVariance);

	m_targetActor.CopyFrom(*(*m_actor));
	m_targetCritic0.CopyFrom(*(*m_critic0));
	m_targetCritic1.CopyFrom(*(*m_critic1));
}

template<ndInt32 statesDim, ndInt32 actionDim>
bool ndBrainAgentTD3_Trainer<statesDim, actionDim>::IsTerminal() const
{
	ndAssert(0);
	return false;
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndReal ndBrainAgentTD3_Trainer<statesDim, actionDim>::GetReward() const
{
	ndAssert(0);
	return ndReal(0.0f);
}

template<ndInt32 statesDim, ndInt32 actionDim>
ndReal ndBrainAgentTD3_Trainer<statesDim, actionDim>::PerturbeAction(ndReal action) const
{
	ndReal actionNoise = ndReal(ndGaussianRandom(action, m_actionNoiseVariance));
	return ndClamp(actionNoise, ndReal(-1.0f), ndReal(1.0f));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::PopulateReplayBuffer()
{
	GetObservation(&m_currentTransition.m_nextState[0]);
	m_currentTransition.m_terminalState = IsTerminal();
	m_replayBuffer.AddTransition(m_currentTransition);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagateCritic(const ndUnsigned32* const shuffleBuffer)
{
	ndFixSizeArray<ndReal[actionDim], 256> bashRandomActionNoise;
	bashRandomActionNoise.SetCount(m_bashBufferSize);
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		for (ndInt32 j = 0; j < actionDim; ++j)
		{
			bashRandomActionNoise[i][j] = ndGaussianRandom(ndFloat32(0.0f), ndFloat32(m_actionNoiseVariance));
		}
	}

	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer, &bashRandomActionNoise](ndInt32 threadIndex, ndInt32 threadCount)
		{
			class Loss : public ndBrainLeastSquareErrorLoss
			{
			public:
				Loss(ndBrainTrainer& trainer, ndBrain* const targetCritic
					, const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& replayBuffer, ndReal gamma)
					:ndBrainLeastSquareErrorLoss(trainer.GetBrain()->GetOutputSize())
					, m_criticTrainer(trainer)
					, m_targetCritic(targetCritic)
					, m_replayBuffer(replayBuffer)
					, m_gamma(gamma)
					, m_index(0)
				{
				}

				void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
				{
					ndAssert(loss.GetCount() == 1);
					ndAssert(output.GetCount() == 1);
					ndAssert(m_truth.GetCount() == m_criticTrainer.GetBrain()->GetOutputSize());

					ndReal criticOutputBuffer[2];
					ndDeepBrainMemVector criticOutput(criticOutputBuffer, 1);

					const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[m_index];
					ndReal targetValue = transition.m_reward;
					if (!transition.m_terminalState)
					{
						ndDeepBrainMemVector criticInput(m_targetInputBuffer, statesDim + actionDim);
						m_targetCritic->MakePrediction(criticInput, criticOutput);
						targetValue = transition.m_reward + m_gamma * criticOutput[0];
					}
					criticOutput[0] = targetValue;

					SetTruth(criticOutput);
					ndBrainLeastSquareErrorLoss::GetLoss(output, loss);
				}

				ndBrainTrainer& m_criticTrainer;
				ndBrain* m_targetCritic;
				const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& m_replayBuffer;
				ndFloat32 m_gamma;
				ndInt32 m_index;
				ndReal m_targetInputBuffer[(statesDim + actionDim) * 2];
			};

			ndBrainTrainer& trainer0 = *(*m_criticOptimizer0[threadIndex]);
			ndBrainTrainer& trainer1 = *(*m_criticOptimizer1[threadIndex]);
			trainer0.ClearGradientsAcc();
			trainer1.ClearGradientsAcc();

			Loss loss0(trainer0, &m_targetCritic0, m_replayBuffer, m_gamma);
			Loss loss1(trainer1, &m_targetCritic1, m_replayBuffer, m_gamma);

			const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);

			ndReal targetInputBuffer[statesDim * 2];
			ndReal targetOutputBuffer[actionDim * 2];
			ndReal criticInputBuffer[(statesDim + actionDim) * 2];
			ndDeepBrainMemVector targetInput(targetInputBuffer, statesDim);
			ndDeepBrainMemVector targetOutput(targetOutputBuffer, actionDim);
			ndDeepBrainMemVector criticInput(criticInputBuffer, statesDim + actionDim);

			for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
			{
				ndInt32 index = ndInt32(shuffleBuffer[i]);
				const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[index];

				for (ndInt32 j = 0; j < statesDim; ++j)
				{
					targetInput[j] = transition.m_nextState[j];
					loss0.m_targetInputBuffer[j] = transition.m_nextState[j];
					loss1.m_targetInputBuffer[j] = transition.m_nextState[j];
				}
				m_targetActor.MakePrediction(targetInput, targetOutput);
				for (ndInt32 j = 0; j < actionDim; ++j)
				{
					ndReal noisyAction = targetOutput[j] + bashRandomActionNoise[i][j];
					ndReal action = ndClamp(noisyAction, ndReal(-1.0f), ndReal(1.0f));
					loss0.m_targetInputBuffer[j + statesDim] = action;
					loss1.m_targetInputBuffer[j + statesDim] = action;
				}

				for (ndInt32 j = 0; j < statesDim; ++j)
				{
					criticInput[j] = transition.m_state[j];
				}
				for (ndInt32 j = 0; j < actionDim; ++j)
				{
					criticInput[j + statesDim] = transition.m_action[j];
				}
				loss0.m_index = index;
				loss1.m_index = index;
				trainer0.BackPropagate(criticInput, loss0);
				trainer1.BackPropagate(criticInput, loss1);
			}
		});

	auto AccumulateWeight = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
		{
			ndBrainTrainer* const trainer0 = *m_criticOptimizer0[0];;
			ndBrainTrainer* const trainer1 = *m_criticOptimizer1[0];
			for (ndInt32 i = 1; i < threadCount; ++i)
			{
				ndBrainTrainer* const srcTrainer0 = *m_criticOptimizer0[i];
				ndBrainTrainer* const srcTrainer1 = *m_criticOptimizer1[i];
				trainer0->AcculumateGradients(*srcTrainer0, threadIndex, threadCount);
				trainer1->AcculumateGradients(*srcTrainer1, threadIndex, threadCount);
			}
		});

	ndBrainThreadPool::ParallelExecute(PropagateBash);
	ndBrainThreadPool::ParallelExecute(AccumulateWeight);
	m_criticOptimizer0[0]->UpdateWeights(m_criticLearnRate, m_bashBufferSize);
	m_criticOptimizer1[0]->UpdateWeights(m_criticLearnRate, m_bashBufferSize);

	m_criticOptimizer0[0]->ClampWeights(ndReal(100.0f));
	m_criticOptimizer1[0]->ClampWeights(ndReal(100.0f));

	m_criticOptimizer0[0]->DropOutWeights(ndReal(1.0e-6f), ndReal(1.0e-6f));
	m_criticOptimizer1[0]->DropOutWeights(ndReal(1.0e-6f), ndReal(1.0e-6f));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagateActor(const ndUnsigned32* const shuffleBuffer)
{
	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer](ndInt32 threadIndex, ndInt32 threadCount)
		{
			class ActorLoss : public ndBrainLoss
			{
			public:
				ActorLoss(ndBrain* const critic0, ndBrain* const critic1, ndBrainTrainer& actorTrainer
					, const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& replayBuffer)
					:ndBrainLoss()
					, m_critic0(critic0)
					, m_critic1(critic1)
					, m_actorTrainer(actorTrainer)
					, m_replayBuffer(replayBuffer)
					, m_index(0)
				{
				}

				void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
				{
					ndAssert(loss.GetCount() == actionDim);
					ndAssert(output.GetCount() == actionDim);
					const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[m_index];

					ndReal criticOutputBuffer[2];
					ndReal criticInputBuffer[(statesDim + actionDim) * 2];
					ndDeepBrainMemVector criticInput(criticInputBuffer, statesDim + actionDim);
					ndDeepBrainMemVector criticOutput(criticOutputBuffer, 1);
					for (ndInt32 i = 0; i < statesDim; ++i)
					{
						criticInput[i] = transition.m_state[i];
					}
					for (ndInt32 i = 0; i < actionDim; ++i)
					{
						criticInput[i + statesDim] = output[i];
					}
					m_critic0->MakePrediction(criticInput, criticOutput);
					ndReal gain0 = criticOutput[0];
					m_critic1->MakePrediction(criticInput, criticOutput);
					ndReal gain1 = criticOutput[0];

					ndBrain* const critic = (gain0 <= gain1) ? m_critic0 : m_critic1;

					ndReal inputGradientBuffer[(statesDim + actionDim) * 2];
					ndDeepBrainMemVector inputGradient(inputGradientBuffer, statesDim + actionDim);
					critic->CalculateInputGradients(criticInput, inputGradient);

					for (ndInt32 i = 0; i < actionDim; ++i)
					{
						loss[i] = inputGradient[statesDim + i];
					}
				}

				ndBrain* m_critic0;
				ndBrain* m_critic1;
				ndBrainTrainer& m_actorTrainer;
				const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& m_replayBuffer;
				ndInt32 m_index;
			};

			ndBrainTrainer& actorTrainer = *(*m_actorOptimizer[threadIndex]);

			ActorLoss loss(*m_critic0, *m_critic1, actorTrainer, m_replayBuffer);

			ndReal inputBuffer[statesDim * 2];
			ndDeepBrainMemVector input(inputBuffer, statesDim);

			actorTrainer.ClearGradientsAcc();
			const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);
			for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
			{
				ndInt32 index = ndInt32(shuffleBuffer[i]);
				const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[index];

				for (ndInt32 j = 0; j < statesDim; ++j)
				{
					input[j] = transition.m_state[j];
				}
				loss.m_index = index;
				actorTrainer.BackPropagate(input, loss);
			}
		});

	auto AccumulateWeight = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
		{
			ndBrainTrainer& trainer = *(*m_actorOptimizer[0]);
			for (ndInt32 i = 1; i < threadCount; ++i)
			{
				ndBrainTrainer& srcTrainer = *(*m_actorOptimizer[i]);
				trainer.AcculumateGradients(srcTrainer, threadIndex, threadCount);
			}
		});

	ParallelExecute(PropagateBash);
	ParallelExecute(AccumulateWeight);
	m_actorOptimizer[0]->UpdateWeights(-m_actorLearnRate, m_bashBufferSize);
	m_actorOptimizer[0]->ClampWeights(ndReal(100.0f));

	m_actorOptimizer[0]->DropOutWeights(ndReal(1.0e-6f), ndReal(1.0e-6f));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagate()
{
	ndUnsigned32 shuffleBuffer[1024];
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		shuffleBuffer[i] = ndRandInt() % m_replayBuffer.GetCount();
	}

	BackPropagateCritic(shuffleBuffer);
	BackPropagateActor(shuffleBuffer);

	m_targetActor.SoftCopy(*(*m_actor), m_softTargetFactor);
	m_targetCritic0.SoftCopy(*(*m_critic0), m_softTargetFactor);
	m_targetCritic1.SoftCopy(*(*m_critic1), m_softTargetFactor);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::Optimize()
{
	BackPropagate();
	if (IsSampling())
	{
		ndExpandTraceMessage("%d star training: episode %d\n", m_frameCount, m_eposideCount);
	}
	m_collectingSamples = false;
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::CalculateQvalue(const ndBrainVector& state, const ndBrainVector& actions)
{
	ndReal buffer[(statesDim + actionDim) * 2];
	ndDeepBrainMemVector criticInput(buffer, statesDim + actionDim);
	for (ndInt32 i = 0; i < statesDim; ++i)
	{
		criticInput[i] = state[i];
	}
	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		criticInput[i + statesDim] = actions[i];
	}

	ndReal currentQValueBuffer[2];
	ndDeepBrainMemVector currentQValue(currentQValueBuffer, 1);

	m_critic0->MakePrediction(criticInput, currentQValue);
	ndReal reward0 = currentQValue[0];

	m_critic1->MakePrediction(criticInput, currentQValue);
	ndReal reward1 = currentQValue[0];

	m_currentQValue = ndMin(reward0, reward1);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::Step()
{
	ndReal stateBuffer[statesDim * 2];
	ndReal actionBuffer[actionDim * 2];
	ndDeepBrainMemVector state(stateBuffer, statesDim);
	ndDeepBrainMemVector actions(actionBuffer, actionDim);

	GetObservation(&m_currentTransition.m_state[0]);
	for (ndInt32 i = 0; i < statesDim; ++i)
	{
		state[i] = m_currentTransition.m_state[i];
	}
	m_actor->MakePrediction(state, actions);

	// explore environment
	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		actions[i] = PerturbeAction(actions[i]);
	}
	ApplyActions(&actions[0]);
	m_currentTransition.m_reward = GetReward();

	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		m_currentTransition.m_action[i] = actions[i];
	}

	if (!IsSampling())
	{
		// Get Q vale from Critic
		CalculateQvalue(state, actions);
	}
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::OptimizeStep()
{
	if (!m_frameCount)
	{
		ResetModel();
	}

	PopulateReplayBuffer();
	if (m_replayBuffer.GetCount() == m_replayBuffer.GetCapacity())
	{
		Optimize();
	}

	if (m_currentTransition.m_terminalState)
	{
		ResetModel();
		m_currentTransition.Clear();
		if (IsSampling() && (m_eposideCount % 100 == 0))
		{
			ndExpandTraceMessage("collecting samples: frame %d out of %d, episode %d \n", m_frameCount, m_replayBuffer.GetCapacity(), m_eposideCount);
		}
		m_eposideCount++;
		m_framesAlive = 0;
	}

	m_frameCount++;
	m_framesAlive++;
}

#else

#include "ndBrainStdafx.h"
#include "ndBrainAgentDDPG_Trainer.h"

// this is an implementation of more stable policy gradient for
// continues action controller using deep re enforcement learning. 
// td3 algorithm as described here: 
// https://arxiv.org/pdf/1802.09477.pdf

#define USED_NEWTON_TD3_TWIST_VERSION

template<ndInt32 statesDim, ndInt32 actionDim>
class ndBrainAgentTD3_Trainer: public ndBrainAgentDDPG_Trainer<statesDim, actionDim>
{
	public:
	ndBrainAgentTD3_Trainer(const ndSharedPtr<ndBrain>& actor, const ndSharedPtr<ndBrain>& critic);

	protected:
	void BackPropagate();
	void InitWeights();
	void InitWeights(ndReal weighVariance, ndReal biasVariance);

	virtual void CalculateQvalue(const ndBrainVector& state, const ndBrainVector& actions);

	void BackPropagateActor(const ndUnsigned32* const bashIndex);
	void BackPropagateCritic(const ndUnsigned32* const bashIndex);

	ndBrain m_critic2;
	ndBrain m_target2Critic;
	ndFixSizeArray<ndSharedPtr<ndBrainTrainer>, D_MAX_THREADS_COUNT> m_critic2Optimizer;
};

template<ndInt32 statesDim, ndInt32 actionDim>
ndBrainAgentTD3_Trainer<statesDim, actionDim>::ndBrainAgentTD3_Trainer(const ndSharedPtr<ndBrain>& actor, const ndSharedPtr<ndBrain>& critic)
	:ndBrainAgentDDPG_Trainer<statesDim, actionDim>(actor, critic)
	,m_critic2(*(*critic))
	,m_target2Critic(*(*critic))
{
	for (ndInt32 i = 0; i < ndBrainThreadPool::GetThreadCount(); ++i)
	{
		m_critic2Optimizer.PushBack(new ndBrainTrainer(&m_critic2));
		m_critic2Optimizer[m_critic2Optimizer.GetCount() - 1]->SetRegularizer(D_DDPG_REGULARIZER);
	}

	InitWeights();
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::InitWeights()
{
	m_critic2.InitWeightsXavierMethod();
	m_target2Critic.CopyFrom(m_critic2);
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::InitWeights();
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::InitWeights(ndReal weighVariance, ndReal biasVariance)
{
	m_critic2.InitWeights(weighVariance, biasVariance);
	m_target2Critic.CopyFrom(m_critic2);
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::InitWeights(weighVariance, biasVariance);
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagateCritic(const ndUnsigned32* const shuffleBuffer)
{
	#ifdef USED_NEWTON_TD3_TWIST_VERSION

	ndFixSizeArray<ndReal[actionDim], 256> bashRandomActionNoise;
	bashRandomActionNoise.SetCount(m_bashBufferSize);
	for (ndInt32 i = 0; i < m_bashBufferSize; ++i)
	{
		for (ndInt32 j = 0; j < actionDim; ++j)
		{
			bashRandomActionNoise[i][j] = ndGaussianRandom(ndFloat32(0.0f), ndFloat32(m_actionNoiseVariance));
		}
	}

	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer, &bashRandomActionNoise](ndInt32 threadIndex, ndInt32 threadCount)
	{
		class Loss : public ndBrainLeastSquareErrorLoss
		{
			public:
			Loss(ndBrainTrainer& trainer, ndBrain* const targetCritic
				,const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& replayBuffer, ndReal gamma)
				:ndBrainLeastSquareErrorLoss(trainer.GetBrain()->GetOutputSize())
				,m_criticTrainer(trainer)
				,m_targetCritic(targetCritic)
				,m_replayBuffer(replayBuffer)
				,m_gamma(gamma)
				,m_index(0)
			{
			}

			void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
			{
				ndAssert(loss.GetCount() == 1);
				ndAssert(output.GetCount() == 1);
				ndAssert(m_truth.GetCount() == m_criticTrainer.GetBrain()->GetOutputSize());

				ndReal criticOutputBuffer[2];
				ndDeepBrainMemVector criticOutput(criticOutputBuffer, 1);

				const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[m_index];
				ndReal targetValue = transition.m_reward;
				if (!transition.m_terminalState)
				{
					ndDeepBrainMemVector criticInput(m_targetInputBuffer, statesDim + actionDim);
					m_targetCritic->MakePrediction(criticInput, criticOutput);
					targetValue = transition.m_reward + m_gamma * criticOutput[0];
				}
				criticOutput[0] = targetValue;
				
				SetTruth(criticOutput);
				ndBrainLeastSquareErrorLoss::GetLoss(output, loss);
			}

			ndBrainTrainer& m_criticTrainer;
			ndBrain* m_targetCritic;
			const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& m_replayBuffer;
			ndFloat32 m_gamma;
			ndInt32 m_index;
			ndReal m_targetInputBuffer[(statesDim + actionDim) * 2];
		};

		ndBrainTrainer& trainer1 = *(*m_critic2Optimizer[threadIndex]);
		ndBrainTrainer& trainer0 = *(*ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_criticOptimizer[threadIndex]);
		trainer0.ClearGradientsAcc();
		trainer1.ClearGradientsAcc();

		Loss loss0(trainer0,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetCritic(),
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_gamma);

		Loss loss1(trainer1, &m_target2Critic,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_gamma);

		ndBrain* const targetActor = ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetActor();
		const ndStartEnd startEnd(ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_bashBufferSize, threadIndex, threadCount);

		ndReal targetInputBuffer[statesDim * 2];
		ndReal targetOutputBuffer[actionDim * 2];
		ndReal criticInputBuffer[(statesDim + actionDim) * 2];
		ndDeepBrainMemVector targetInput(targetInputBuffer, statesDim);
		ndDeepBrainMemVector targetOutput(targetOutputBuffer, actionDim);
		ndDeepBrainMemVector criticInput(criticInputBuffer, statesDim + actionDim);

		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			ndInt32 index = ndInt32(shuffleBuffer[i]);
			const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer[index];

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				targetInput[j] = transition.m_nextState[j];
				loss0.m_targetInputBuffer[j] = transition.m_nextState[j];
				loss1.m_targetInputBuffer[j] = transition.m_nextState[j];
			}
			targetActor->MakePrediction(targetInput, targetOutput);
			for (ndInt32 j = 0; j < actionDim; ++j)
			{
				ndReal noisyAction = targetOutput[j] + bashRandomActionNoise[i][j];
				ndReal action = ndClamp(noisyAction, ndReal(-1.0f), ndReal(1.0f));
				loss0.m_targetInputBuffer[j + statesDim] = action;
				loss1.m_targetInputBuffer[j + statesDim] = action;
			}

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				criticInput[j] = transition.m_state[j];
			}
			for (ndInt32 j = 0; j < actionDim; ++j)
			{
				criticInput[j + statesDim] = transition.m_action[j];
			}
			loss0.m_index = index;
			loss1.m_index = index;
			trainer0.BackPropagate(criticInput, loss0);
			trainer1.BackPropagate(criticInput, loss1);
		}
	});

	#else

	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer](ndInt32 threadIndex, ndInt32 threadCount)
	{
		class Loss: public ndBrainLeastSquareErrorLoss
		{
			public:
			Loss(ndBrainTrainer& trainer0, ndBrainTrainer& trainer1
				,ndBrain* const targetActor, ndBrain* const targetCritic0, ndBrain* const targetCritic1
				,const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& replayBuffer
				,ndReal gamma, ndFloat32 actionNoiseVariance, ndRandom& randomGenerator)
				:ndBrainLeastSquareErrorLoss(trainer0.GetBrain()->GetOutputSize())
				,m_randomGenerator(randomGenerator)
				,m_criticTrainer0(trainer0)
				,m_criticTrainer1(trainer1)
				,m_targetActor(targetActor)
				,m_targetCritic0(targetCritic0)
				,m_targetCritic1(targetCritic1)
				,m_replayBuffer(replayBuffer)
				,m_gamma(gamma)
				,m_targetValue(0)
				,m_actionNoiseVariance(actionNoiseVariance)
				,m_index(0)
				,m_pass(0)
			{
			}

			void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
			{
				ndAssert(loss.GetCount() == 1);
				ndAssert(output.GetCount() == 1);
				ndAssert(m_truth.GetCount() == m_criticTrainer0.GetBrain()->GetOutputSize());
				ndAssert(m_truth.GetCount() == m_criticTrainer1.GetBrain()->GetOutputSize());

				ndReal criticOutputBuffer[2];
				ndDeepBrainMemVector criticOutput(criticOutputBuffer, 1);

				if (m_pass == 0)
				{
					const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[m_index];
					//criticOutput[0] = transition.m_reward + m_gamma * m_targetValue;
					m_targetValue = transition.m_reward;
					if (!transition.m_terminalState)
					{
						ndReal actorInputBuffer[statesDim * 2];
						ndReal actorOutputBuffer[actionDim * 2];
						ndDeepBrainMemVector actorInput(actorInputBuffer, statesDim);
						ndDeepBrainMemVector actorOutput(actorOutputBuffer, actionDim);

						for (ndInt32 i = 0; i < statesDim; ++i)
						{
							actorInput[i] = transition.m_nextState[i];
						}
						m_targetActor->MakePrediction(actorInput, actorOutput);
						for (ndInt32 i = 0; i < actionDim; ++i)
						{
							ndReal noiseAction = actorOutput[i] + ndReal(m_randomGenerator.GetGaussianRandom(ndFloat32(0.0f), ndFloat32(m_actionNoiseVariance)));
							actorOutput[i] = ndClamp(noiseAction, ndReal(-1.0f), ndReal(1.0f));
						}

						ndReal criticInputBuffer[(statesDim + actionDim) * 2];
						ndDeepBrainMemVector criticInput(criticInputBuffer, statesDim + actionDim);
						for (ndInt32 i = 0; i < statesDim; ++i)
						{
							criticInput[i] = actorInput[i];
						}
						for (ndInt32 i = 0; i < actionDim; ++i)
						{
							criticInput[i + statesDim] = actorOutput[i];
						}
						m_targetCritic0->MakePrediction(criticInput, criticOutput);
						ndReal reward0 = criticOutput[0];

						m_targetCritic1->MakePrediction(criticInput, criticOutput);
						ndReal reward1 = criticOutput[0];

						// get minimum reward from the two target critics, and use that at the targte loss
						m_targetValue = transition.m_reward + m_gamma * ndMin(reward0, reward1);
					}
				}
				m_pass++;
				criticOutput[0] = m_targetValue;

				SetTruth(criticOutput);
				ndBrainLeastSquareErrorLoss::GetLoss(output, loss);
			}

			ndRandom& m_randomGenerator;
			ndBrainTrainer& m_criticTrainer0;
			ndBrainTrainer& m_criticTrainer1;
			ndBrain* m_targetActor;
			ndBrain* m_targetCritic0;
			ndBrain* m_targetCritic1;
			const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& m_replayBuffer;
			ndFloat32 m_gamma;
			ndFloat32 m_targetValue;
			ndFloat32 m_actionNoiseVariance;
			ndInt32 m_index;
			ndInt32 m_pass;
		};

		ndBrainTrainer& trainer0 = *(*ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_criticOptimizer[threadIndex]);
		ndBrainTrainer& trainer1 = *(*m_critic2Optimizer[threadIndex]);
		trainer0.ClearGradientsAcc();
		trainer1.ClearGradientsAcc();

		ndReal inputBuffer[(statesDim + actionDim) * 2];
		ndDeepBrainMemVector input(inputBuffer, statesDim + actionDim);
		//Loss loss(trainer0, trainer1, &m_targetActor, &m_targetCritic, &m_target2Critic, m_replayBuffer, m_gamma, m_actionNoiseVariance, GetRandomGenerator(threadIndex));

		Loss loss(trainer0, trainer1,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetActor(),
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetCritic(),
			&m_target2Critic,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_gamma,
			ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_actionNoiseVariance,
			ndBrainThreadPool::GetRandomGenerator(threadIndex));
		
		const ndStartEnd startEnd(ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_bashBufferSize, threadIndex, threadCount);
		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			ndInt32 index = ndInt32(shuffleBuffer[i]);
			const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer[index];

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				input[j] = transition.m_state[j];
			}
			for (ndInt32 j = 0; j < actionDim; ++j)
			{
				input[j + statesDim] = transition.m_action[j];
			}

			loss.m_pass = 0;
			loss.m_index = index;
			trainer0.BackPropagate(input, loss);
			trainer1.BackPropagate(input, loss);
		}
	});
	#endif

	auto AccumulateWeight = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
	{
		ndBrainTrainer* const trainer0 = ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCriticTrainer(0);
		ndBrainTrainer* const trainer1 = *m_critic2Optimizer[0];
		for (ndInt32 i = 1; i < threadCount; ++i)
		{
			ndBrainTrainer* const srcTrainer0 = ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCriticTrainer(i);
			ndBrainTrainer* const srcTrainer1 = *m_critic2Optimizer[i];
			trainer0->AcculumateGradients(*srcTrainer0, threadIndex, threadCount);
			trainer1->AcculumateGradients(*srcTrainer1, threadIndex, threadCount);
		}
	});

	ndBrainThreadPool::ParallelExecute(PropagateBash);
	ndBrainThreadPool::ParallelExecute(AccumulateWeight);
	m_critic2Optimizer[0]->UpdateWeights(ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_criticLearnRate, ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_bashBufferSize);
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCriticTrainer(0)->UpdateWeights(ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_criticLearnRate, ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_bashBufferSize);

	m_critic2Optimizer[0]->ClampWeights(ndReal(100.0f));
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCriticTrainer(0)->ClampWeights(ndReal(100.0f));

	m_critic2Optimizer[0]->DropOutWeights(ndReal(1.0e-6f), ndReal(1.0e-6f));
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCriticTrainer(0)->DropOutWeights(ndReal(1.0e-6f), ndReal(1.0e-6f));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagateActor(const ndUnsigned32* const shuffleBuffer)
{
	auto PropagateBash = ndMakeObject::ndFunction([this, &shuffleBuffer](ndInt32 threadIndex, ndInt32 threadCount)
	{
		class ActorLoss : public ndBrainLoss
		{
			public:
			ActorLoss(ndBrain* const critic0, ndBrain* const critic1, ndBrainTrainer& actorTrainer
				,const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& replayBuffer)
				:ndBrainLoss()
				,m_critic0(critic0)
				,m_critic1(critic1)
				,m_actorTrainer(actorTrainer)
				,m_replayBuffer(replayBuffer)
				,m_index(0)
			{
			}

			void GetLoss(const ndBrainVector& output, ndBrainVector& loss)
			{
				ndAssert(loss.GetCount() == actionDim);
				ndAssert(output.GetCount() == actionDim);
				const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[m_index];

				ndReal criticOutputBuffer[2];
				ndReal criticInputBuffer[(statesDim + actionDim) * 2];
				ndDeepBrainMemVector criticInput(criticInputBuffer, statesDim + actionDim);
				ndDeepBrainMemVector criticOutput(criticOutputBuffer, 1);
				for (ndInt32 i = 0; i < statesDim; ++i)
				{
					criticInput[i] = transition.m_state[i];
				}
				for (ndInt32 i = 0; i < actionDim; ++i)
				{
					criticInput[i + statesDim] = output[i];
				}
				m_critic0->MakePrediction(criticInput, criticOutput);
				ndReal gain0 = criticOutput[0];
				m_critic1->MakePrediction(criticInput, criticOutput);
				ndReal gain1 = criticOutput[0];
				
				ndBrain* const critic = (gain0 <= gain1) ? m_critic0 : m_critic1;

				ndReal inputGradientBuffer[(statesDim + actionDim) * 2];
				ndDeepBrainMemVector inputGradient(inputGradientBuffer, statesDim + actionDim);
				critic->CalculateInputGradients(criticInput, inputGradient);

				for (ndInt32 i = 0; i < actionDim; ++i)
				{
					loss[i] = inputGradient[statesDim + i];
				}
			}

			ndBrain* m_critic0;
			ndBrain* m_critic1;
			ndBrainTrainer& m_actorTrainer;
			const ndBrainReplayBuffer<ndReal, statesDim, actionDim>& m_replayBuffer;
			ndInt32 m_index;
		};

		ndBrainTrainer& actorTrainer = *(*m_actorOptimizer[threadIndex]);

		ActorLoss loss(ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCritic(), 
			&m_critic2, actorTrainer, ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer);

		ndReal inputBuffer[statesDim * 2];
		ndDeepBrainMemVector input(inputBuffer, statesDim);

		actorTrainer.ClearGradientsAcc();
		const ndStartEnd startEnd(m_bashBufferSize, threadIndex, threadCount);
		for (ndInt32 i = startEnd.m_start; i < startEnd.m_end; ++i)
		{
			ndInt32 index = ndInt32(shuffleBuffer[i]);
			const ndBrainReplayTransitionMemory<ndReal, statesDim, actionDim>& transition = m_replayBuffer[index];

			for (ndInt32 j = 0; j < statesDim; ++j)
			{
				input[j] = transition.m_state[j];
			}
			loss.m_index = index;
			actorTrainer.BackPropagate(input, loss);
		}
	});

	auto AccumulateWeight = ndMakeObject::ndFunction([this](ndInt32 threadIndex, ndInt32 threadCount)
	{
		ndBrainTrainer& trainer = *(*m_actorOptimizer[0]);
		for (ndInt32 i = 1; i < threadCount; ++i)
		{
			ndBrainTrainer& srcTrainer = *(*m_actorOptimizer[i]);
			trainer.AcculumateGradients(srcTrainer, threadIndex, threadCount);
		}
	});

	ParallelExecute(PropagateBash);
	ParallelExecute(AccumulateWeight);
	m_actorOptimizer[0]->UpdateWeights(-m_actorLearnRate, m_bashBufferSize);
	m_actorOptimizer[0]->ClampWeights(ndReal(100.0f));

	m_actorOptimizer[0]->DropOutWeights(ndReal(1.0e-6f), ndReal(1.0e-6f));
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagate()
{
	ndUnsigned32 shuffleBuffer[1024];
	ndFixSizeArray<ndReal[actionDim], 256> bashRandomActionNoise0;
	ndFixSizeArray<ndReal[actionDim], 256> bashRandomActionNoise1;
	for (ndInt32 i = 0; i < ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_bashBufferSize; ++i)
	{
		shuffleBuffer[i] = ndRandInt() % ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_replayBuffer.GetCount();
	}
	
	ndAssert(0);
	ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagateCritic(shuffleBuffer);

#ifdef USED_NEWTON_TD3_TWIST_VERSION
	ndBrainAgentTD3_Trainer<statesDim, actionDim>::BackPropagateActor(shuffleBuffer);

	m_target2Critic.SoftCopy(m_critic2, ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_softTargetFactor);
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetActor()->SoftCopy(*ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetActor(), ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_softTargetFactor);
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetCritic()->SoftCopy(*ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCritic(), ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_softTargetFactor);
#else
	if (ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_frameCount & 1)
	{
		ndBrainAgentDDPG_Trainer<statesDim, actionDim>::BackPropagateActor(shuffleBuffer);
		m_target2Critic.SoftCopy(m_critic2, ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_softTargetFactor);
		ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetActor()->SoftCopy(*ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetActor(), ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_softTargetFactor);
		ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetTargetCritic()->SoftCopy(*ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCritic(), ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_softTargetFactor);
	}
#endif
}

template<ndInt32 statesDim, ndInt32 actionDim>
void ndBrainAgentTD3_Trainer<statesDim, actionDim>::CalculateQvalue(const ndBrainVector& state, const ndBrainVector& actions)
{
	ndReal buffer[(statesDim + actionDim) * 2];
	ndDeepBrainMemVector criticInput(buffer, statesDim + actionDim);
	for (ndInt32 i = 0; i < statesDim; ++i)
	{
		criticInput[i] = state[i];
	}
	for (ndInt32 i = 0; i < actionDim; ++i)
	{
		criticInput[i + statesDim] = actions[i];
	}
	
	ndReal currentQValueBuffer[2];
	ndDeepBrainMemVector currentQValue(currentQValueBuffer, 1);
	
	m_critic2.MakePrediction(criticInput, currentQValue);
	ndReal reward0 = currentQValue[0];
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::GetCritic()->MakePrediction(criticInput, currentQValue);
	ndReal reward1 = currentQValue[0];
	ndBrainAgentDDPG_Trainer<statesDim, actionDim>::m_currentQValue = ndMin(reward0, reward1);
}
#endif

#endif 
