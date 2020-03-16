using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LinkController : MonoBehaviour
{
    public GameObject chain;
    public GameObject ball;
    public float factor;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        float x = chain.transform.position.x * factor + ball.transform.position.x * (1 - factor);
        float y = chain.transform.position.y * factor + ball.transform.position.y * (1 - factor);
        transform.position = new Vector3(x, y, 0);
    }
}
