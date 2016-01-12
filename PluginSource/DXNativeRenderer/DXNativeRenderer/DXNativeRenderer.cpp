#include "pch.h"
#include "DXNativeRenderer.h"
#include <cmath>

using namespace Windows::Storage::Streams;

// ********************************************************************************
// THE PRIMARY FUNCTION USED TO MANIPULATE THE BYTE ARRAY THAT WILL BE STORED
// TO THE DX <> UNITY SHARED TEXTURE. CALLED BY DoRendering() BELOW.
// ********************************************************************************

static void FillTextureFromCode(int width, int height, int stride, unsigned char* dst)
{
	const float t = g_Time * 4.0f;

	int totalPixels = width * height;
	int totalBytes = totalPixels * 4;

	int copyIndex = 0;

	for (int i = 0; i < totalPixels; i++)
	{
		byte redValue = (byte) (255.0f * sinf(t));
		dst[copyIndex++] = (byte) (255 - redValue); 
		dst[copyIndex++] = 0;
		dst[copyIndex++] = redValue;
		dst[copyIndex++] = 255;
	}
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTimeFromUnity(float t) { g_Time = t; }
extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API  GetTime() { return g_Time; }

// ********************************************************************************
// REQUIRED LOADING, UNLOADING AND BINDING FUNCTIONS FOR NATIVE RENDERING IN UNITY
// ********************************************************************************

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
{
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureFromUnity(IUnknown* texturePtr)
{
	g_TexturePointer = texturePtr;
}

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	UnityGfxRenderer currentDeviceType = s_DeviceType;

	switch (eventType)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		DebugLog("OnGraphicsDeviceEvent(Initialize).\n");
		s_DeviceType = s_Graphics->GetRenderer();
		currentDeviceType = s_DeviceType;
		break;
	}

	case kUnityGfxDeviceEventShutdown:
	{
		DebugLog("OnGraphicsDeviceEvent(Shutdown).\n");
		s_DeviceType = kUnityGfxRendererNull;
		g_TexturePointer = NULL;
		break;
	}

	case kUnityGfxDeviceEventBeforeReset:
	{
		DebugLog("OnGraphicsDeviceEvent(BeforeReset).\n");
		break;
	}

	case kUnityGfxDeviceEventAfterReset:
	{
		DebugLog("OnGraphicsDeviceEvent(AfterReset).\n");
		break;
	}
	};

	if (currentDeviceType == kUnityGfxRendererD3D11)
	{
		DebugLog("Current Device Type from Unity is kUnityGFX Renderer D3D11!");
		DoEventGraphicsDeviceD3D11(eventType);
	}
}

static void DoEventGraphicsDeviceD3D11(UnityGfxDeviceEventType eventType)
{
	if (eventType == kUnityGfxDeviceEventInitialize)
	{
		IUnityGraphicsD3D11* d3d11 = s_UnityInterfaces->Get<IUnityGraphicsD3D11>();
		g_D3D11Device = d3d11->GetDevice();
	}
}


// ********************************************************************************
// RENDERING FUNCTIONS. 

// GetRenderEventFunc() IS THE CALL BACK WHICH UNITY CODE CALLS WITH THE FOLLOWING CODE:
//		GL.IssuePluginEvent(GetRenderEventFunc(), 1); 

// THIS WILL CALL THE OnRenderEvent() WHICH CALLS DoRendering();
// ********************************************************************************


extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEvent;
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
	if (s_DeviceType == kUnityGfxRendererNull)
		return;

	DoRendering();
}

// ********************************************************************************
// DoRendering() SETUPS THE DX TEXTURE BASED ON THE ORIGINAL POINTER PASSED IN
// BY UNITY. ONCE EVERYTHING IS SETUP AND BOUND, IT CALLS THE FUNCITON FillTextureFromCode()
// TO DO THE ACTUAL BYTE ARRAY MANIPULATIONS ABOVE.
// ********************************************************************************

static void DoRendering()
{
	if (s_DeviceType == kUnityGfxRendererD3D11)
	{
		ID3D11DeviceContext* ctx = NULL;
		g_D3D11Device->GetImmediateContext(&ctx);

		if (g_TexturePointer)
		{
			ID3D11Texture2D* d3dtex = (ID3D11Texture2D*) g_TexturePointer;
			D3D11_TEXTURE2D_DESC desc;
			d3dtex->GetDesc(&desc);

			unsigned char* data = new unsigned char[desc.Width*desc.Height * 4];
			FillTextureFromCode(desc.Width, desc.Height, desc.Width * 4, data);
			ctx->UpdateSubresource(d3dtex, 0, NULL, data, desc.Width * 4, 0);
			delete [] data;
		}

		ctx->Release();
	}
}




