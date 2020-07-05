using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SpecialBlockController : MonoBehaviour
{
    public AudioSource SpecialBlockAudio;
    PlayerController _player;
    Animator anim;
    public bool isHard = false;
    // Start is called before the first frame update
    void Start()
    {
        _player = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>();
        anim = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnCollisionEnter2D(Collision2D other)
    {
        if (other.gameObject.CompareTag("Player") && !isHard) {
            var playerPos = _player.blockCheckPoint.position;
            var curPos = transform.position;
            if (playerPos.y - curPos.y < 0) {
                generateRandomly();
            }
        }
    }

    private void generateRandomly() {
        float randomNum = Random.Range(0.0f, 1.0f);
        if (randomNum < 0.6f) {
            // generate a coin
            transform.Find("Coin").gameObject.SetActive(true);
        }
        else {
            // generate an enemy
            transform.Find("NormalEnemy").gameObject.SetActive(true);
        }
        isHard = true;
        anim.SetBool("IsHard", true);
        SpecialBlockAudio.Play();
        GameManager.AddScore(10);
    }
}
