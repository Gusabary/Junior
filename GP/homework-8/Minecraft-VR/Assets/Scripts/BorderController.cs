using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BorderController : MonoBehaviour
{
    public Vector3 dir;
    public GameObject player;
    float mainSpeed = 0.5f; //regular speed

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void Move() {
        player.transform.position += dir * mainSpeed;
    }
}
