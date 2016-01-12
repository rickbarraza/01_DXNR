#pragma once

#include <d3d11.h>
#include <robuffer.h>
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) if (a) { a->Release(); a = NULL; }
#endif

static void DebugLog(const char* str)
{
	OutputDebugStringA(str);
}

static float g_Time;

static IUnknown* g_TexturePointer = NULL;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static void DoEventGraphicsDeviceD3D11(UnityGfxDeviceEventType eventType);

static void UNITY_INTERFACE_API OnRenderEvent(int eventID);

static void DoRendering();

static ID3D11Device* g_D3D11Device = NULL;


