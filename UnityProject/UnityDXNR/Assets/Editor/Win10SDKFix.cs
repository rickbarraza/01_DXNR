/* Garth: The Windows 10 SDK was updated recently, which broke Unity builds.
* It came down to a few extra ".0" in an XML file someplace.
* Instead of manually changing it, let's just code Unity to do it for us.
* FOUND ON: http://forum.unity3d.com/threads/warning-do-not-update-to-windows-sdk-10-0-10586-0-just-yet.371074/
*/

#if UNITY_WSA

using UnityEngine;
using UnityEditor;
using UnityEditor.Callbacks;
using System.IO;

public class Win10SdkFix
{

[PostProcessBuild(1)]
  public static void OnPostprocessBuild(BuildTarget target, string pathToBuiltProject)
  {
	string name = PlayerSettings.productName;
	string pathToXml = pathToBuiltProject + "/" + name + "/" + name + ".csproj";
	Debug.Log("Attempting to fix bad SDK version at " + pathToXml);

	if (!File.Exists(pathToXml))
	{
		Debug.Log("Could not find " + name + ".csproj file to fix.");
		return;
	}

	string xml = File.ReadAllText(pathToXml);

	if (!xml.Contains(BadString))
	{
		Debug.LogWarning("Could not find bad string in " + name + ".csproj file. Have you updated Unity? You can remove this script.");
	}
	else
	{
		xml = xml.Replace(BadString, GoodString);
		File.WriteAllText(pathToXml, xml);
		Debug.Log("Fixed sdk version string in " + name + ".csproj file.");
	}
  }

  const string BadString = "10.0.10586.0.0";
  const string GoodString = "10.0.10586.0";

}
#endif
