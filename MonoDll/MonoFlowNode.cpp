#include "StdAfx.h"
#include "MonoFlowNode.h"

#include "MonoCommon.h"
#include "MonoArray.h"

#include <IMonoObject.h>

#include "MonoScriptSystem.h"
#include "FlowManager.h"
#include <IMonoEntityManager.h>

#include <IGameFramework.h>

CFlowNode::CFlowNode(SActivationInfo *pActInfo)
	: m_pScriptClass(NULL)
	, m_pActInfo(pActInfo)
	, m_cloneType(eNCT_Instanced)
	, m_pNodeType(NULL)
{
	// We *have* to get the id right away or inputs won't work, so lets use this fugly solution.
	pActInfo->pGraph->RegisterHook(this);
	// Keep it so we can unregister it after we've got what we came for.
	m_pHookedGraph = pActInfo->pGraph;
}

CFlowNode::~CFlowNode()
{
	SAFE_RELEASE(m_pScriptClass);
}

void CFlowNode::Release()
{
	static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager()->UnregisterNode(this);
}

bool CFlowNode::CreatedNode(TFlowNodeId id, const char *name, TFlowNodeTypeId typeId, IFlowNodePtr pNode) 
{ 
	if(pNode==this)
	{
		m_pNodeType = static_cast<CScriptSystem *>(gEnv->pMonoScriptSystem)->GetFlowManager()->InstantiateNode(this, gEnv->pFlowSystem->GetTypeName(typeId));

		// Set by CFlowManager::InstantiateNode.
		return m_pScriptClass != NULL;
	}

	return true; 
}

IFlowNodePtr CFlowNode::Clone(SActivationInfo *pActInfo)
{
	switch(m_cloneType)
	{
	case eNCT_Singleton:
		return this;
	case eNCT_Instanced:
		return new CFlowNode(pActInfo);
	default:
		break;
	}

	return NULL;
}

void CFlowNode::ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
{	
	if(m_pHookedGraph && m_pScriptClass->GetScriptId()!=-1)
	{
		m_pHookedGraph->UnregisterHook(this);
		m_pHookedGraph = NULL;
	}

	switch(event)
	{
	case eFE_Activate:
		{
			IFlowNodeData *pNodeData = m_pActInfo->pGraph->GetNodeData(pActInfo->myID);
			if(!pNodeData)
				return;

			for(int i = 0; i < pNodeData->GetNumInputPorts(); i++)
			{
				if(IsPortActive(pActInfo, i))
				{
					switch(GetPortType(pActInfo, i))
					{
					case eFDT_Void:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i);
						}
						break;
					case eFDT_Int:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i, GetPortInt(pActInfo, i));
						}
						break;
					case eFDT_Float:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i, GetPortFloat(pActInfo, i));
						}
						break;
					case eFDT_EntityId:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i, GetPortEntityId(pActInfo, i));
						}
						break;
					case eFDT_Vec3:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i, GetPortVec3(pActInfo, i));
						}
						break;
					case eFDT_String:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i, GetPortString(pActInfo, i));
						}
						break;
					case eFDT_Bool:
						{
							CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnPortActivated", i, GetPortBool(pActInfo, i));
						}
						break;
					default:
						break;
					}
				}
			}

		}
		break;
	case eFE_Initialize:
		{
			CallMonoScript<void>(m_pScriptClass->GetScriptId(), "OnInitialized");
		}
		break;
	case eFE_SetEntityId:
		{
			if(m_pNodeType->IsEntityNode())
			{
				int entityScriptId = 0;

				if(IMonoEntityManager *pEntityManager = gEnv->pMonoScriptSystem->GetEntityManager())
				{
					if(pActInfo && pActInfo->pGraph->GetEntityId(pActInfo->myID))
						entityScriptId = pEntityManager->GetScriptId(pActInfo->pGraph->GetEntityId(pActInfo->myID));
				}

				int scriptId = m_pScriptClass->GetScriptId();
				if(entityScriptId!=scriptId && entityScriptId!=0)
				{
					gEnv->pMonoScriptSystem->RemoveScriptInstance(scriptId);
					m_pScriptClass = gEnv->pMonoScriptSystem->GetScriptById(entityScriptId);
				}
			}
		}
		break;
	}
}

void CFlowNode::GetConfiguration(SFlowNodeConfig &config)
{
	if(!m_pScriptClass)
	{
		CryWarning(VALIDATOR_MODULE_FLOWGRAPH, VALIDATOR_ERROR, "CFlowNode::GetConfiguration: m_pScriptClass was NULL!");
		return;
	}

	if(IMonoObject *pResult = m_pScriptClass->CallMethod("GetNodeConfig"))
	{
		SMonoNodeConfig monoConfig = pResult->Unbox<SMonoNodeConfig>();

		config.pInputPorts = m_pNodeType->GetInputPorts(m_pScriptClass);
		config.pOutputPorts = m_pNodeType->GetOutputPorts(m_pScriptClass);

		config.nFlags |= monoConfig.flags;
		config.sDescription = _HELP(ToCryString(monoConfig.description));
		config.SetCategory(monoConfig.category);
	}
}