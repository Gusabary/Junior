using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class UnitySingleton<T> : MonoBehaviour where T : MonoBehaviour
{
    private static object _singletonLock = new object();
 
    private static T _instance;
 
    public static T Instance
    {
        get
        {
            if (_instance == null)
            {
                lock (_singletonLock)
                {
                    T[] singletonInstances = FindObjectsOfType(typeof (T)) as T[];
                    if (singletonInstances.Length == 0) return null;
 
                    if (singletonInstances.Length > 1)
                    {
                        if (Application.isEditor)
                            Debug.LogWarning(
                                "MonoSingleton<T>.Instance: Only 1 singleton instance can exist in the scene. Null will be returned.");
                        return null;
                    }
                    _instance = singletonInstances[0];
                }
            }
            return _instance;
        }
    }
}
