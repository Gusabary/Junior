using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VoxelController : MonoBehaviour
{
    public Vector3 delta;
    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnMouseOver()
    {
        // If the left mouse button is pressed
        if (Input.GetMouseButtonDown(0))
        {
            // Display a message in the Console tab
            Debug.Log("Left click!");
            Destroy(this.transform.parent.gameObject);
        }
    
        // If the right mouse button is pressed
        if (Input.GetMouseButtonDown(1))
        {
            // Display a message in the Console tab
            Debug.Log("Right click!");
            Vector3 newPosition = this.transform.parent.transform.position + delta;
            WorldGenerator.CloneAndPlace(newPosition, this.transform.parent.gameObject);
        }
    }
}
