using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NormalEnemy : Enemy
{
    public AudioSource KillEnemyAudio;
    float deadTime = 0f;
    void Start () {
        _player = GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>();
        enemyRig = GetComponent<Rigidbody2D>();
        anim = GetComponent<Animator>();
	}
	
	void Update () {
        if (!_player.isDead && !isDead) {
            Move();
        }

        if (_player.isDead) {
            speed = 0f;
        }

        if (isDead) {
            deadTime += Time.deltaTime;
            if (deadTime > 0.5f) {
                this.gameObject.SetActive(false);
            }
        }
	}

    private void OnCollisionEnter2D(Collision2D other)
    {
        if (other.gameObject.CompareTag("Player")) {
            _CheckPlayerPos();
        }
        else {
            ChangeMoveDir();
        }
    }

    private void _CheckPlayerPos()
    {
        var playerPos = _player.checkPoint.position;
        var curPos = transform.position;

        if (playerPos.y - curPos.y > 0)
        {
            isDead = true;
            anim.SetBool("IsDead", true);
            KillEnemyAudio.Play();
            _player.Bounce();
            GameManager.AddScore(30);
        }
        else
        {
            _player.Die();
        }
    }
}
