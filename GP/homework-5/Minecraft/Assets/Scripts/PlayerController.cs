using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    Rigidbody rig;
    Transform checkPoint;
    float mainSpeed = 3.0f; //regular speed
    float jumpHeight = 250f;
    public bool isJumping;

    // Start is called before the first frame update
    void Start()
    {
        rig = GetComponent<Rigidbody>();
        checkPoint = transform.Find("GroundCheckPoint");
    }

    // Update is called once per frame
    void Update()
    {
        float xInput = Input.GetAxis("Horizontal");
        float zInput = Input.GetAxis("Vertical");
        Vector3 p = GetBaseInput();
        Vector3 dir = Vector3.Normalize(transform.forward * p.z + transform.right * p.x);
        // Debug.Log(transform.forward);
        transform.position += dir * mainSpeed * Time.deltaTime;

        CheckIsJumping();
        if (!isJumping && Input.GetKeyDown(KeyCode.Space)) {
            Jump();
        }
    }

    void Jump()
    {
        rig.AddForce(new Vector3(0, jumpHeight, 0));
        isJumping = true;
    }

    void CheckIsJumping()
    {
        float checkDistance = 0.05f;
        Vector3 check = checkPoint.position;
        if (Physics.Raycast(check, Vector3.down, checkDistance)) {
            isJumping = false;
        }
        else {
            isJumping = true;
        }
    }

    private Vector3 GetBaseInput() {
        Vector3 p_Velocity = new Vector3();
        if (Input.GetKey(KeyCode.W)) {
            p_Velocity += new Vector3(0, 0 , 1);
        }
        if (Input.GetKey(KeyCode.S)) {
            p_Velocity += new Vector3(0, 0, -1);
        }
        if (Input.GetKey(KeyCode.A)) {
            p_Velocity += new Vector3(-1, 0, 0);
        }
        if (Input.GetKey(KeyCode.D)) {
            p_Velocity += new Vector3(1, 0, 0);
        }
        return p_Velocity;
    }
}
