using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{
    public GameObject player;
    private Vector3 offset;
    float rotSpeed = 400f;
    private float mouseX, mouseY;
    // Start is called before the first frame update
    void Start()
    {
        this.transform.position = new Vector3(player.transform.position.x, 1.7f, player.transform.position.z);
        offset = this.transform.position - player.transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        mouseX += Input.GetAxis("Mouse X") * rotSpeed * Time.deltaTime;
        mouseY -= Input.GetAxis("Mouse Y") * rotSpeed * Time.deltaTime;
        mouseY = Mathf.Clamp(mouseY, -30, 45);

        transform.rotation = Quaternion.Euler(mouseY, mouseX, 0);
        Quaternion playerRot = player.transform.rotation;
        Vector3 tmp = player.transform.rotation.eulerAngles;
        tmp.y = transform.rotation.eulerAngles.y;
        playerRot.eulerAngles = tmp;
        player.transform.rotation = playerRot;
    }

    void LateUpdate() {
        this.transform.position = player.transform.position + offset;
    }
}
