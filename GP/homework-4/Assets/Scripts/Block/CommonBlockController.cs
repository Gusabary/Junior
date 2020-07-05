using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CommonBlockController : MonoBehaviour
{
    public AudioSource BreakBlockAudio;
    PlayerController _player;
    // Start is called before the first frame update
    void Start()
    {
        _player = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnCollisionEnter2D(Collision2D other)
    {
        if (other.gameObject.CompareTag("Player")) {
            var playerPos = _player.blockCheckPoint.position;
            var curPos = transform.position;
            if (playerPos.y - curPos.y < 0) {
                BreakBlockAudio.Play();
                // gameObject.SetActive(false);
                GetComponent<BoxCollider2D>().enabled = false;
                gameObject.GetComponent<Renderer>().enabled = false;
                GameManager.AddScore(5);
            }
        }
    }
}
