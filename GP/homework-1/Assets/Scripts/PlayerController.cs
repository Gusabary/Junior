using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerController : MonoBehaviour
{
    public float speed;
    public Text countText;
    private int count;
    private float endTime;
    // Start is called before the first frame update
    void Start()
    {
        count = 0;
        endTime = -1;
        setCountText();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void FixedUpdate() {
        float moveHorizontal = Input.GetAxis("Horizontal");
        float moveVertical = Input.GetAxis("Vertical");

        Vector3 movement = new Vector3(moveHorizontal, 0.0f, moveVertical);

        GetComponent<Rigidbody>().AddForce(movement * speed * Time.deltaTime);

        if (endTime >= 0) {
            endTime += Time.deltaTime;
        }
        if (endTime > 1) {
            Application.Quit();
        }
    }

    private void OnTriggerEnter(Collider other) {
        if (other.gameObject.tag == "PickUp") {
            other.gameObject.SetActive(false);
            count++;
            setCountText();
        }
    }

    void setCountText() {
        if (count == 8) {
            countText.text = "Complete!";
            endTime = 0;
        }
        else {
            countText.text = "Count: " + count;
        }
    }
}
