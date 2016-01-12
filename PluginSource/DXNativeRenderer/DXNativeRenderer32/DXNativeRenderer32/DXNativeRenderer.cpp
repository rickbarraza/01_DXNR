/*
THIS IS THE STUB FILE IMPLEMENTAITON USE ONLY IN THE UNITY EDITOR AS A PLACE HODLER FOR 
WORKING WITH THE PRIMARY DLL "DXNATIVERENDERER" IN THE COMPANION PROJECT.

THIS DLL PROVIDES A VERY SIMPLE EXAMPLE OF NATIVE RENDERING FOR WIN32, HOWEVER, BY 
SHARING TEXTURE POINTERS BETWEEN UNITY AND A DIRECTX GRAPHICS RESOURCE BELOW AND 
ANIMATING THE TEXTURE BASED ON THE SHARED TIME VARIABLE.

*/

#include "DXNativeRenderer.h"
#include <cmath>


extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTimeFromUnity(float t) { g_Time = t; }

extern "C" float UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API  GetTime() { return g_Time;	}


	// --------------------------------------------------------------------------------------
	// THIS IS THE PRIMARY FUNCTION FOR ITERATING THROUGH THE BYTE ARRAY
	// THAT IS SHARED BETWEEN UNITY AND DIRECTX.
	// CURRENTLY, THIS CODE IS SPECIFICALLY MADE FOR A 512x424 IMAGE FORMAT OF BGRA
	// --------------------------------------------------------------------------------------

	static void FillTextureFromCode(int width, int height, int stride, unsigned char* dst)
	{
		const float t = g_Time * 4.0f;
		
		for (int y = 0; y < height; ++y)
		{
			unsigned char* ptr = dst;
			// GREEN WILL BE GRADED ACROSS THE HEIGHT OF THE IMAGE STATICALLY...
			byte greenValue = (byte) (255 * ((float) y / (float) height));
			// RED WILL SINE WAVE FROM 0 TO 255 BASED ON TIME...
			byte redValue = (byte)(255.0f * sinf(t));
			byte blueValue = (byte) (255.0f * cosf(t));

			for (int x = 0; x < width; ++x)
			{
				// Write the texture pixel
				ptr[0] = blueValue;		// B
				ptr[1] = greenValue;	// G
				ptr[2] = redValue;		// R
				ptr[3] = 255;			// A

				// To next pixel (our pixels are 4 bpp)
				ptr += 4;
				//ptr += 3;
			}

			// To next image row
			dst += stride;
		}
	}


	// ----------------------------------------------------------------------------------------------------------------
	// UNITY REQUIRED METHODS FOR NATIVE LOW LEVEL RENDERING. - REQUIRED -
	// ----------------------------------------------------------------------------------------------------------------

	extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetTextureFromUnity(IUnknown* texturePtr)
	{
		g_TexturePointer = texturePtr;
	}

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


	// --------------------------------------------------------------------------
	// GetRenderEventFunc, an example function we export which is used to get a
	// rendering event callback function.
	// --------------------------------------------------------------------------

	extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
	{
		return OnRenderEvent;
	}

	// -------------------------------------------------------------------------
	// ON RENDER EVENT 
	// This will be called for GL.IssuePluginEvent script calls; eventID will
	// be the integer passed to IssuePluginEvent. In this example, we just ignore
	// that value.
	// -------------------------------------------------------------------------

	static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
	{
		if (s_DeviceType == kUnityGfxRendererNull)
			return;

		DoRendering();
	}

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