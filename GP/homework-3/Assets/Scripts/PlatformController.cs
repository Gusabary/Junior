using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlatformController : MonoBehaviour
{
    float speed = 2.4f;
    Vector3 speedVec3;
    GameObject topLine;
    // Start is called before the first frame update
    void Start()
    {
        speedVec3.y = speed;
        topLine = GameObject.Find("TopLine");
    }

    // Update is called once per frame
    void Update()
    {
        Move();
    }

    void Move() {
        transform.position += speedVec3 * Time.deltaTime;
        if (transform.position.y > topLine.transform.position.y) {
            Destroy(gameObject);
        }
    }
}
