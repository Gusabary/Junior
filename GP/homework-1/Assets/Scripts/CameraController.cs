using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public GameObject player;
    private Vector3 offset;
    // Start is called before the first frame update
    void Start()
    {
        offset = GetComponent<Transform>().position;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void LateUpdate() {
        transform.position = player.transform.position + offset;
    }
}
