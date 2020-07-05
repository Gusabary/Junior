using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CloudController : MonoBehaviour
{
    float speed = 0.2f;
    Vector3 speedVec3;
    // Start is called before the first frame update
    void Start()
    {
        speedVec3.x = speed;
    }

    // Update is called once per frame
    void Update()
    {
        Move();
    }

    void Move() {
        float rightBorder = 36.0f;
        transform.position += speedVec3 * Time.deltaTime;
        if (transform.position.x > rightBorder) {
            Destroy(gameObject);
        }
    }
}
