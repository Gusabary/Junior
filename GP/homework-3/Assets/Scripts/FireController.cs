using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FireController : MonoBehaviour
{
    Animator animator;
    float time;
    float interval = 1.5f;
    // Start is called before the first frame update
    void Start()
    {
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        time += Time.deltaTime;
        if (time > interval) {
            tag = "Spike";
            animator.SetBool("On", true);
        }
        if (time > interval * 2) {
            tag = "Untagged";
            animator.SetBool("On", false);
            time = 0f;
        }
    }
}
