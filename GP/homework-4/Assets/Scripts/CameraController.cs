using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class CameraController : MonoBehaviour
{
    public GameObject player;
    private float offset;
    float fixedY;
    float fixedZ;
    // Start is called before the first frame update
    void Start()
    {
        offset = GetComponent<Transform>().position.x - player.transform.position.x;
        fixedY = GetComponent<Transform>().position.y;
        fixedZ = GetComponent<Transform>().position.z;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void LateUpdate() {
        float rightBorder = 0;
        float leftBorder = 0;
        if (SceneManager.GetActiveScene().name == "World1-1") {
            rightBorder = 15.44f;
            leftBorder = -15.44f;
        }
        else if (SceneManager.GetActiveScene().name == "World1-2") { 
            rightBorder = 13.12f;
            leftBorder = -13.12f;
        }
        float x = Mathf.Min(rightBorder, player.transform.position.x + offset);
        x = Mathf.Max(leftBorder, x);
        transform.position = new Vector3(x, fixedY, fixedZ);
    }
}
