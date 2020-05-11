using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour
{
    public static GameManager instance;
    private int score;
    public Text scoreNum;
    public GameObject gameStartPanel;
    public GameObject gameOverPanel;

    private void Awake() {
        if (instance != null) {
            Destroy(gameObject);
        }
        instance = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        instance.score = 0;
    }

    // Update is called once per frame
    void Update()
    {
    }

    public static void AddScore() {
        instance.score += 10;
        instance.scoreNum.text = instance.score.ToString("000");
    }

    public static void GameOver() {
        instance.gameOverPanel.SetActive(true);
    }

    public void RestartGame() {
        instance.score = 0;
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
    }

    public void StartGame() {
        instance.gameStartPanel.SetActive(false);
    }
}
