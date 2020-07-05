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

    public void CreateVoxel()
    {
        Vector3 newPosition = this.transform.parent.transform.position + delta;
        WorldGenerator.CloneAndPlace(newPosition, this.transform.parent.gameObject);
        GameManager.AddScore();
    }
}
