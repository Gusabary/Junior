using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class GameManager : MonoBehaviour
{
    static GameManager instance;
    public Text timeScore;
    public GameObject gameOverPanel;
    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        timeScore.text = Time.timeSinceLevelLoad.ToString("00");
    }

    private void Awake() {
        if (instance != null) {
            Destroy(gameObject);
        }
        instance = this;
    }

    public static void GameOver(bool dead) {
        if (dead) {
            instance.gameOverPanel.SetActive(true);
            Time.timeScale = 0;
        }
    }

    public void RestartGame() {
        SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        Time.timeScale = 1;
    }

    public void ExitGame() {
        Debug.Log("Exit");
        Application.Quit();
    }
}
