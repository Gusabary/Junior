using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    Rigidbody2D rb;
    public float speed;
    Animator animator;
    public float checkRadius;
    public LayerMask platformMask;
    public GameObject checkPoint;
    public bool isOnGround;
    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        animator = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        isOnGround = Physics2D.OverlapCircle(checkPoint.transform.position, checkRadius, platformMask);
        animator.SetBool("isOnGround", isOnGround);
        Move();
    }

    void Move() {
        float xInput = Input.GetAxisRaw("Horizontal");
        rb.velocity = new Vector2(xInput * speed, rb.velocity.y);
        if (xInput != 0) {
            transform.localScale = new Vector3(xInput, 1, 1);
        }
        animator.SetFloat("speed", Mathf.Abs(rb.velocity.x));
    }

    public void Die() {
        Debug.Log("Die");
        GameManager.GameOver(true);
    }

    void OnTriggerEnter2D(Collider2D other) {
        if (other.CompareTag("Spike")) {
            Die();
        }
    }

    void OnCollisionStay2D(Collision2D other) {
        if (other.gameObject.CompareTag("Spike")) {
            Die();
        }
    }

    void OnCollisionEnter2D(Collision2D other) {
        if (other.gameObject.CompareTag("Spike")) {
            Die();
        }
    }

    private void OnDrawGizmosSelected() {
        Gizmos.color = Color.blue;
        Gizmos.DrawWireSphere(checkPoint.transform.position, checkRadius);
    }
}
