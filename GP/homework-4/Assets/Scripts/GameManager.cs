using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour
{
    static GameManager instance;
    private static int score;
    private float time;
    public Text scoreNum;
    public Text timeNum;
    public GameObject gameOverPanel;
    public Text gameOverText;
    int winStage = 0;
    // Start is called before the first frame update
    void Start()
    {
        Debug.Log(score);
        instance.scoreNum.text = score.ToString("000000");
        // score = ScorePasser.getScore();
    }

    // Update is called once per frame
    void Update()
    {
        if (winStage == 1) {
            instance.time -= Time.deltaTime * 300;
            if (instance.time < 0) {
                winStage = 2;
                instance.time = 0f;  // avoid negative number
            }
            timeNum.text = instance.time.ToString("000");

            score += Mathf.FloorToInt(Time.deltaTime * 300);
            instance.scoreNum.text = score.ToString("000000");
            return;
        }
        if (winStage == 0) {
            instance.time = 999 - Time.timeSinceLevelLoad;
            if (instance.time < 0) {
                // GameOver(true);
                GameObject.FindGameObjectWithTag("Player").GetComponent<PlayerController>().Die();
                instance.time = 0f;  // avoid negative number
            }
            timeNum.text = instance.time.ToString("000");
        }
    }

    private void Awake() {
        // Debug.Log("AAAAwake");
        if (instance != null) {
            Destroy(gameObject);
        }
        instance = this;
        // DontDestroyOnLoad(this.gameObject);
        // DontDestroyOnLoad(GameObject.Find("Canvas"));
        // DontDestroyOnLoad(GameObject.Find("EventSystem"));
    }

    public static void AddScore(int num) {
        score += num;
        instance.scoreNum.text = score.ToString("000000");
    }

    public static void GameOver(bool dead) {
        if (dead) {
            instance.gameOverText.text = "Game Over";
            instance.gameOverPanel.SetActive(true);
            // Time.timeScale = 1;  // I dont't know why timeScale here cannot be 0
        }
        else {
            instance.gameOverText.text = "To Be Continued";
            instance.gameOverPanel.SetActive(true);
        }
    }

    public void RestartGame() {
        // Time.timeScale = 1;
        score = 0;
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        // instance.gameOverPanel.SetActive(false);
    }

    public void ExitGame() {
        // Debug.Log("Exit");
        Application.Quit();
    }

    public static void Win() {
        instance.winStage = 1;
    }
}
