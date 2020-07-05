using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class EndTileController : MonoBehaviour
{
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

    private void OnTriggerEnter2D(Collider2D other)
    {
        if (other.CompareTag("Player")) {
            // Debug.Log("Win");
            if (SceneManager.GetActiveScene().name == "World1-1") {
                EndOfWorld1_1();
            }
            else if (SceneManager.GetActiveScene().name == "World1-2") {
                EndOfWorld1_2();
            }
        }
    }

    private void EndOfWorld1_1() {
        _player.winStage = 3;
        SceneManager.LoadScene("World1-2");
    }

    private void EndOfWorld1_2() {
        _player.winStage = 3;
        GameManager.GameOver(false);
    }
}
