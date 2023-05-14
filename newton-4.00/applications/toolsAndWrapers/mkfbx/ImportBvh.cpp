#include "stdafx.h"
#include "exportMeshNode.h"

void exportMeshNode::SetFrame(int index)
{
	int stack = 1;
	exportMeshNode* stackPool[128];

	stack = 1;
	stackPool[0] = this;
	while (stack)
	{
		stack--;

		exportMeshNode* const node = stackPool[stack];

		_ASSERT(0);
		//exportVector euler(node->m_rotationsKeys[index]);
		//exportMatrix pitch(ndPitchMatrix(euler.m_x));
		//exportMatrix yaw(ndYawMatrix(euler.m_y));
		//exportMatrix roll(ndRollMatrix(euler.m_z));
		//exportMatrix matrix(pitch * yaw * roll);
		//matrix.m_posit = node->m_matrix.m_posit;
		//node->m_matrix = matrix;

		for (std::list<exportMeshNode*>::const_iterator iter = node->m_children.begin();
			iter != node->m_children.end(); iter++)
		{
			stackPool[stack] = *iter;
			stack++;
		}
	}
}

void exportMeshNode::DeleteEffector()
{
	int stack = 1;
	exportMeshNode* stackPool[128];

	stack = 1;
	stackPool[0] = this;
	while (stack)
	{
		stack--;
		exportMeshNode* const node = stackPool[stack];

		if (node->m_name == "effector")
		{
			node->m_parent->m_children.clear();
			delete node;
		}
		else
		{
			for (std::list<exportMeshNode*>::const_iterator iter = node->m_children.begin();
				iter != node->m_children.end(); iter++)
			{
				stackPool[stack] = *iter;
				stack++;
			}
		}
	}
}

exportMeshNode* exportMeshNode::ImportBvhSkeleton(const char* const name)
{
	exportMeshNode* entity = nullptr;

	FILE* const fp = fopen(name, "rt");

	char token[256];
	auto ReadToken = [fp, &token]()
	{
		fscanf(fp, "%s", token);
	};

	auto ReadFloat = [fp]()
	{
		float value;
		fscanf(fp, "%f", &value);
		return value;
	};

	auto ReadInt = [fp]()
	{
		int value;
		fscanf(fp, "%d", &value);
		return value;
	};

	int stack = 0;
	exportMeshNode* stackPool[128];
	std::vector<exportMeshNode*> nodeIndex;
	
	float scale = 1.0f;
	int framesCount = 0;
	if (fp)
	{
		ReadToken();
		if (!strcmp(token, "HIERARCHY"))
		{
			while (!feof(fp))
			{
				ReadToken();
				if (!strcmp(token, "ROOT"))
				{
					entity = new exportMeshNode();
					nodeIndex.push_back(entity);
					ReadToken();
					entity->m_name = token;
					stackPool[stack] = entity;
					stack = 1;
					ReadToken();
				}
				else if (!strcmp(token, "JOINT"))
				{
					exportMeshNode* const node = new exportMeshNode(stackPool[stack - 1]);
					nodeIndex.push_back(node);
					ReadToken();
					if (node->m_parent->m_parent)
					{
						//strcat(token, "_bone");
					}
					else
					{
						//strcat(token, "_ref");
					}
					node->m_name = token;
					stackPool[stack] = node;
					stack++;
					ReadToken();
				}
				else if (!strcmp(token, "End"))
				{
					exportMeshNode* const node = new exportMeshNode(stackPool[stack - 1]);
					ReadToken();
					node->m_name = "effector";
					stackPool[stack] = node;
					stack++;
					ReadToken();
					//while (strcmp(token, "}")) ReadToken();
				}
				else if (!strcmp(token, "OFFSET"))
				{
					exportMeshNode* const node = stackPool[stack - 1];
					node->m_matrix.m_posit.m_x = ReadFloat() * scale;
					node->m_matrix.m_posit.m_y = ReadFloat() * scale;
					node->m_matrix.m_posit.m_z = ReadFloat() * scale;
					node->m_matrix.m_posit.m_w = 1.0f;
				}
				else if (!strcmp(token, "CHANNELS"))
				{
					int count = ReadInt();
					exportMeshNode* const node = stackPool[stack - 1];
					for (int i = 0; i < count; ++i)
					{
						ReadToken();
						if (!strcmp(token, "Xposition"))
						{
							node->m_animDof.m_channel[i] = 3;
						} 
						else if (!strcmp(token, "Yposition"))
						{
							node->m_animDof.m_channel[i] = 4;
						}
						else if (!strcmp(token, "Zposition"))
						{
							node->m_animDof.m_channel[i] = 5;
						}
						else if (!strcmp(token, "Xrotation"))
						{
							node->m_animDof.m_channel[i] = 0;
						}
						else if (!strcmp(token, "Yrotation"))
						{
							node->m_animDof.m_channel[i] = 1;
						}
						else if (!strcmp(token, "Zrotation"))
						{
							node->m_animDof.m_channel[i] = 2;
						}
					}
				}
				else if (!strcmp(token, "}"))
				{
					stack--;
				}
				else if (!strcmp(token, "MOTION"))
				{
					break;
				}
			}
		}

		ReadToken();
		framesCount = ReadInt();
		ReadToken();
		ReadToken();
		float frameTime = ReadFloat();
		
		for (int i = 0; i < framesCount; ++i)
		{
			for (int j = 0; j < nodeIndex.size(); ++j)
			{
				float values[6];
				exportMeshNode* const node = nodeIndex[j];
				
				values[0] = 0.0f;
				values[1] = 0.0f;
				values[2] = 0.0f;
				values[3] = node->m_matrix.m_posit.m_x;
				values[4] = node->m_matrix.m_posit.m_y;
				values[5] = node->m_matrix.m_posit.m_z;
				for (int k = 0; k < 6; ++k)
				{
					int index = node->m_animDof.m_channel[k];
					if (index != -1)
					{
						float value = ReadFloat();
						values[index] = value;
					}
				}

				exportMatrix pitch(ndPitchMatrix(values[0] * M_PI / 180.0f));
				exportMatrix yaw(ndYawMatrix(values[1] * M_PI / 180.0f));
				exportMatrix roll(ndRollMatrix(values[2] * M_PI / 180.0f));
				exportMatrix matrix(yaw * pitch * roll);
				matrix.m_posit = node->m_matrix.m_posit;
				if (node->m_parent == entity)
				{
					matrix.m_posit = exportVector(values[3], values[4], values[5], 1.0f);
				}
				//node->m_matrix = matrix;
				node->m_keyFrame.push_back(matrix);
			}

			//int stack = 1;
			//exportMeshNode* stackPool[128];
			//stack = 1;
			//stackPool[0] = entity;
			//while (stack)
			//{
			//	stack--;
			//
			//	exportMeshNode* const node = stackPool[stack];
			//	exportMatrix matrix(node->m_matrix);
			//	exportVector euler1;
			//	exportVector euler0(matrix.CalcPitchYawRoll(euler1));
			//
			//	if (i != 0)
			//	{
			//		float error = node->CalculateDeltaAngle(euler0.m_x, node->m_rotationsKeys[i-1].m_x);
			//		if (fabsf(error) > 90.0 * M_PI / 180.0f)
			//		{
			//			euler0 = euler1;
			//		}
			//
			//		float deltax = node->CalculateDeltaAngle(euler0.m_x, node->m_rotationsKeys[i - 1].m_x);
			//		float deltay = node->CalculateDeltaAngle(euler0.m_y, node->m_rotationsKeys[i - 1].m_y);
			//		float deltaz = node->CalculateDeltaAngle(euler0.m_z, node->m_rotationsKeys[i - 1].m_z);
			//
			//		euler0.m_x = node->m_rotationsKeys[i - 1].m_x + deltax;
			//		euler0.m_y = node->m_rotationsKeys[i - 1].m_y + deltay;
			//		euler0.m_z = node->m_rotationsKeys[i - 1].m_z + deltaz;
			//	}
			//	_ASSERT(0);
			//	node->m_rotationsKeys.push_back(euler0);
			//	node->m_positionsKeys.push_back(matrix.m_posit);
			//
			//	for (std::list<exportMeshNode*>::const_iterator iter = node->m_children.begin();
			//		iter != node->m_children.end(); iter++)
			//	{
			//		stackPool[stack] = *iter;
			//		stack++;
			//	}
			//}
		}

		fclose(fp);
	}

	//entity->SetFrame(0);
	//entity->CalculateTpose();

	entity->DeleteEffector();
	return entity;
}

