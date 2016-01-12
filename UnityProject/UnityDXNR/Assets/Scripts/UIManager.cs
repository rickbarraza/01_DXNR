using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.Runtime.InteropServices;
using System;

public class UIManager : MonoBehaviour {

    Text txtOutput;
    Texture2D processedTexture;
    byte[] textureData;
    public RawImage riOutput;
        
    [DllImport("DXNativeRenderer")]
    private static extern void SetTimeFromUnity(float t);

    [DllImport("DXNativeRenderer")]
    private static extern IntPtr GetRenderEventFunc();

    [DllImport("DXNativeRenderer")]
    private static extern void SetTextureFromUnity(System.IntPtr texture);
    
    void Start () {
        txtOutput = GameObject.Find("txtOutput").GetComponent<Text>();
        txtOutput.text = "The above texture is created locally as a BGRA32 texture, completely blue.\n";
        txtOutput.text += "When you click the button, the C++ DLL will take over with Native Rendering and create a gradient texture.";

        CreateTextureLocally();
    }
	
    void CreateTextureLocally()
    {
        FillTextureDataLocally();
        processedTexture = new Texture2D(512, 424, TextureFormat.BGRA32, false);
        processedTexture.filterMode = FilterMode.Point;
        processedTexture.LoadRawTextureData(textureData);

        processedTexture.Apply();
        riOutput.texture = processedTexture;
    }

    void FillTextureDataLocally()
    {
        textureData = new byte[512 * 424 * 4];

        int byteIndex = 0;

        // SINCE THE TEXTURE IS BGRA, THE FOLLOWING 
        // WOULD CREATE A SOLID BLUE TEXTURE WITH FULL ALPHA
        for (int y = 0; y < 424; y++)
        {
            for (int x = 0; x < 512; x++)
            {
                textureData[byteIndex++] = 255;
                textureData[byteIndex++] = 0;
                textureData[byteIndex++] = 0;
                textureData[byteIndex++] = 255;
            }
        }
    }

    public void TestButtonClicked()
    {
        txtOutput.text = "Pressed.";
        SetTextureFromUnity(processedTexture.GetNativeTexturePtr());
        StartCoroutine(StartDLLLoop());
    }

    IEnumerator StartDLLLoop()
    {
        yield return StartCoroutine("CallPluginAtEndOfFrames");
    }

    private IEnumerator CallPluginAtEndOfFrames()
    {
        while (true)
        {
            yield return new WaitForEndOfFrame();
            SetTimeFromUnity(Time.timeSinceLevelLoad);
            GL.IssuePluginEvent(GetRenderEventFunc(), 1);
        }
    }
}
