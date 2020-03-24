using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FlagpoleController : MonoBehaviour
{
    public AudioSource BGMAudio;
    public AudioSource WinAudio;
    PlayerController _player;
    Rigidbody2D flagRig;
    public bool shouldFallFlag = false;
    // Start is called before the first frame update
    void Start()
    {
        _player = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>();
        flagRig = GameObject.Find("Flag").GetComponent<Rigidbody2D>();
    }

    // Update is called once per frame
    void Update()
    {
        if (!shouldFallFlag || flagRig.gameObject.transform.position.y < -0.63f) {
            flagRig.bodyType = RigidbodyType2D.Static;
            // flagRig.velocity = new Vector2(0, 0);
        }
        else {
            flagRig.bodyType = RigidbodyType2D.Dynamic;
            flagRig.velocity = new Vector2(0, -0.35f);
        }
    }

    private void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("Player")) {
            shouldFallFlag = true;
            _player.winStage = 1;
            WinAudio.Play();
            BGMAudio.Stop();
            GameManager.Win();
        }
    }
}
