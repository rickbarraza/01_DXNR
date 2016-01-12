/* 
THIS IS THE STUB FILE REQUIRED BY THE UNITY EDITOR.
-------------------------------------------------------

See: http://docs.unity3d.com/Manual/windowsstore-plugins.html for more information.

Basically, the Unity Editor can not use Windows Store Apps specific plugins if you use
Windows Runtime APIs, so when you want a project DLL that will take advantage of Windows 10
WSA features, you need to make a fake "stub" dll for the Editor during development time that
simulates the API surface you will use in your "real" dll.

In this case, we will be sharing a Texture pointer between Unity and the DLL. This stub file
will simply paint an animated texture in the DLL while testing in the Editor and can still serve 
as a basic example of Win32 Native Rendering.
 
HowevThe primary project "DXNativeRender" will be setup to take advantage of additional DirectX features
and eventually expanded to include vision and perception device features avaialable in Windows Store.

*/

#pragma once

#ifndef UNITY_INTERFACE_API
#define UNITY_INTERFACE_API __stdcall
#define UNITY_INTERFACE_EXPORT __declspec(dllexport)
#endif 

#include <d3d11.h>
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"

static void DebugLog(const char* str)
{
	OutputDebugStringA(str);
}

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a) if (a) { a->Release(); a = NULL; }
#endif

static float g_Time;
static IUnknown* g_TexturePointer = NULL;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType);

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;
static void DoEventGraphicsDeviceD3D11(UnityGfxDeviceEventType eventType);

static void DoRendering();

static void UNITY_INTERFACE_API OnRenderEvent(int eventID);

static ID3D11Device* g_D3D11Device = NULL;



