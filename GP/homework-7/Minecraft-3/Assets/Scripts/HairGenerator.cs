using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HairGenerator : MonoBehaviour
{
    void Awake() {
        GameObject go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        go.transform.localScale = new Vector3(0.2f, 0.2f, 0.2f);
        go.transform.position = Vector3.zero;
        GameObject parent = go;
        for (int i = 0; i < 10; i++) {
            GameObject child = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            child.transform.parent = parent.transform;
            child.transform.localPosition = new Vector3(0, -2.5f, 0);
            // child.transform.localScale = new Vector3(0.4f, 0.4f, 0.4f);
            parent = child;
        }
        go.AddComponent<SwingBone>();
    }
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
