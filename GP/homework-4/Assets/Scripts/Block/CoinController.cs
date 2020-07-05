using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CoinController : MonoBehaviour
{
    public AudioSource CoinAudio;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("Player")) {
            CoinAudio.Play();
            // gameObject.SetActive(false);
            GetComponent<CapsuleCollider2D>().enabled = false;
            gameObject.GetComponent<Renderer>().enabled = false;
            GameManager.AddScore(50);
        }
    }
}
