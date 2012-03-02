#include "StdAfx.h"
#include "ScriptBind_Renderer.h"

#include <IViewSystem.h>
#include <IGameFramework.h>

CScriptBind_Renderer::CScriptBind_Renderer()
{
	REGISTER_METHOD(CreateView);
	REGISTER_METHOD(RemoveView);

	REGISTER_METHOD(GetActiveView);
	REGISTER_METHOD(SetActiveView);

	REGISTER_METHOD(GetViewParams);
	REGISTER_METHOD(SetViewParams);

	REGISTER_METHOD(GetWidth);
	REGISTER_METHOD(GetHeight);

	REGISTER_METHOD(ScreenToWorld);

	REGISTER_METHOD(DrawTextToScreen);

	REGISTER_METHOD(LoadTexture);
	REGISTER_METHOD(DrawTextureToScreen);

	REGISTER_METHOD(CreateRenderTarget);
	REGISTER_METHOD(DestroyRenderTarget);
	REGISTER_METHOD(SetRenderTarget);
}

// Externals below
unsigned int CScriptBind_Renderer::CreateView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetViewId(pViewSystem->CreateView());

	return 0;
}

void CScriptBind_Renderer::DrawTextToScreen(float xpos, float ypos, float fontSize, ColorF color, bool center, mono::string text)
{
	float actualColor[] = { color.r, color.g, color.b, color.a };

	gEnv->pRenderer->Draw2dLabel(xpos, ypos, fontSize, actualColor, center, ToCryString(text));
}

void CScriptBind_Renderer::RemoveView(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetViewByEntityId(viewId))
			pViewSystem->RemoveView(viewId);
	}
}

unsigned int CScriptBind_Renderer::GetActiveView()
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
		return pViewSystem->GetActiveViewId();

	return 0;
}

void CScriptBind_Renderer::SetActiveView(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(pViewSystem->GetViewByEntityId(viewId))
			pViewSystem->SetActiveView(viewId);
	}
}

SViewParams CScriptBind_Renderer::GetViewParams(unsigned int viewId)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetActiveView())
			return *pView->GetCurrentParams();
	}

	return SViewParams();
}

void CScriptBind_Renderer::SetViewParams(unsigned int viewId, SViewParams viewParams)
{
	if (IViewSystem *pViewSystem = gEnv->pGameFramework->GetIViewSystem())
	{
		if(IView *pView = pViewSystem->GetActiveView())
		{
			viewParams.SaveLast();

			pView->SetCurrentParams(viewParams);
		}
	}
}

int CScriptBind_Renderer::GetWidth()
{
	return gEnv->pRenderer->GetWidth();
}

int CScriptBind_Renderer::GetHeight()
{
	return gEnv->pRenderer->GetHeight();
}

Vec3 CScriptBind_Renderer::ScreenToWorld(int x, int y)
{	
	if(gEnv->pPhysicalWorld)
	{
		float mouseX, mouseY, mouseZ;
		Vec3  camPos = gEnv->pSystem->GetViewCamera().GetPosition();

		gEnv->pRenderer->UnProjectFromScreen((float)x, GetHeight() - y, 0.0f, &mouseX, &mouseY, &mouseZ);
		Vec3 dir = (Vec3(mouseX, mouseY, mouseZ) - camPos).GetNormalizedSafe();

		static ray_hit hit;
		IPhysicalEntity *pPhysEnt = NULL;

		if (gEnv->pPhysicalWorld->RayWorldIntersection(camPos, dir * gEnv->p3DEngine->GetMaxViewDistance(), ent_all, rwi_stop_at_pierceable | rwi_colltype_any, &hit, 1, pPhysEnt))
			return hit.pt;
	}

	return Vec3(ZERO);
}

int CScriptBind_Renderer::LoadTexture(mono::string texturePath)
{
	if(ITexture *pTexture = gEnv->pRenderer->EF_LoadTexture(ToCryString(texturePath)))
		return pTexture->GetTextureID();

	return -1;
}

void CScriptBind_Renderer::DrawTextureToScreen(float xpos, float ypos, float width, float height, int textureId, float s0, float t0, float s1, float t1, float angle, float r, float g, float b, float a, float z)
{
	// Could expose the optional args later.
	gEnv->pRenderer->Draw2dImage(xpos, ypos, width, height, textureId, s0, t0, s1, t1, angle, r, g, b, a, z);
}

int CScriptBind_Renderer::CreateRenderTarget(int width, int height, ETEX_Format texFormat)
{
	return gEnv->pRenderer->CreateRenderTarget(width, height, texFormat);
}

void CScriptBind_Renderer::DestroyRenderTarget(int textureId)
{
	gEnv->pRenderer->DestroyRenderTarget(textureId);
}

void CScriptBind_Renderer::SetRenderTarget(int textureId)
{
	gEnv->pRenderer->SetRenderTarget(textureId);
}