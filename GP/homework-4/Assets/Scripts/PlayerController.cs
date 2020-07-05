using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    Rigidbody2D rig2d;
    Animator anim;
    public Transform checkPoint;
    public Transform blockCheckPoint;
    float curSpeed = 1f;
    float jumpHeight = 200f;
    bool isFacingRight = true;
    public bool isJumping;
    public float speed;

    public bool isDead;
    public int winStage = 0;
    float deadTime;
    bool hasDeadBounce;
    public AudioSource JumpAudio;
    public AudioSource DieAudio;
    public AudioSource BGMAudio;

    LayerMask groundLayer; //地面层
    // LayerMask enemyLayer; //敌人层

    void Start ()
    {
        // Debug.Log("START");
        isDead = false;
        deadTime = 0f;
        hasDeadBounce = false;
        rig2d = GetComponent<Rigidbody2D>();
        anim = GetComponent<Animator>();
        checkPoint = transform.Find("GroundCheckPoint");
        blockCheckPoint = transform.Find("BlockCheckPoint");

        groundLayer = 1 << LayerMask.NameToLayer("Ground");
        // enemyLayer = 1 << LayerMask.NameToLayer("Enemy");     
    }

    void Update()
    {
        if (isDead) {
            if (!hasDeadBounce) {
                rig2d.velocity = new Vector2(0, 0);
            }
            deadTime += Time.deltaTime;
            if (deadTime > 0.5f && !hasDeadBounce) {
                rig2d.velocity = new Vector2(0, 3f);
                GetComponent<CapsuleCollider2D>().enabled = false;
                hasDeadBounce = true;
            }
            if (deadTime > 1.5f) {
                GameManager.GameOver(true);
            }
            return;
        }
        if (winStage == 1) {
            rig2d.velocity = new Vector2(0, -0.35f);
            if (transform.position.y < -0.63f) {
                winStage = 2;
            }
            return;
        }
        if (winStage == 3) {
            rig2d.velocity = new Vector2(0, 0);
            return;
        }

        CheckIsJumping();
        if (!isJumping && Input.GetKeyDown(KeyCode.Space)) {
            Jump();
        }

        var h = Input.GetAxis("Horizontal");
        // Debug.Log(h);
        Move(h);
        if (h > 0 && !isFacingRight || h < 0 && isFacingRight) {
            Reverse();
        }
    }

    private void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("FallDown")) {
            Die();
        }
    }
    
    void Reverse()
    {
        isFacingRight = !isFacingRight;
        var scale = transform.localScale;
        scale.x *= -1;
        transform.localScale = scale;
    }

    void Move(float dic)
    {
        // Debug.Log(dic * curSpeed);
        rig2d.velocity = new Vector2(dic * curSpeed, rig2d.velocity.y);
        anim.SetFloat("Speed", Mathf.Abs(dic * curSpeed));
        speed = Mathf.Abs(dic * curSpeed);
    }
    
    void Jump()
    {
        rig2d.AddForce(new Vector2(0, jumpHeight));
        // rig2d.velocity = new Vector2(rig2d.velocity.x, 3.5f);
        anim.SetBool("IsJumping", true);
        isJumping = true;
        JumpAudio.Play();
    }

    void CheckIsJumping()
    {
        float checkDistance = 0.05f;
        Vector2 check = checkPoint.position;
        RaycastHit2D hit = Physics2D.Raycast(check, Vector2.down, checkDistance, groundLayer.value);

        if (hit.collider != null) {
            anim.SetBool("IsJumping", false);
            isJumping = false;
        }
        else {
            anim.SetBool("IsJumping", true);
            isJumping = true;
        }
    }

    public void Bounce() {
        rig2d.velocity = new Vector2(rig2d.velocity.x, 3f);
    }

    public void Die()
    {
        if (isDead) {
            return;
        }
        // Debug.Log("Player Die!");
        rig2d.velocity = new Vector2(0, 0);
        isDead = true;
        anim.SetBool("IsDead", true);
        BGMAudio.Stop();
        DieAudio.Play();
    }
}