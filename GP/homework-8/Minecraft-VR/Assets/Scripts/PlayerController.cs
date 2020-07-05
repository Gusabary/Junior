using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
    }

    // Update is called once per frame
    void Update()
    {
        if (transform.position.y < -2.0f) {
            Vector3 origin = transform.position;
            origin.y = -2.0f;
            transform.position = origin;
            GameManager.GameOver();
        }
    }

}
