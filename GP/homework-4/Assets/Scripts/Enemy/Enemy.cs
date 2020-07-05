using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : MonoBehaviour
{
    protected PlayerController _player;
    protected Rigidbody2D enemyRig;
    protected Animator anim;
    protected bool isDead = false;

    public float speed = 0.8f;
    protected Vector3 dir = new Vector3(-1, 0, 0);


    protected void Move() {
        transform.position += dir * Time.deltaTime * speed;
    }

    protected void Reverse() {
        var scale = transform.localScale;
        scale.x *= -1;
        transform.localScale = scale;
    }

    protected void ChangeMoveDir() {
        dir.x *= -1;
        Reverse();
    }
}
