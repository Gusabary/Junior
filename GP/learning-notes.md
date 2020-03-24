# GP learning notes

+ 预制件拖进场景形成新的游戏对象。更改预制件会使其所有游戏对象产生相应的更改，但是如果先手动更改游戏对象的值（被更改的值变成粗体），再更改预制件的话，该值不会受影响。

+ 在场景中右键按住，WSADQE 用来控制相机位置，但是稍微有点反直觉，一般 W 就是朝着和地面平行的正前方移动，但是实际上 W 是朝着相机的前方移动，也就是说控制相机的移动是相对相机本身坐标而言的，并非世界坐标。

+ LoadScene 以后卡住，player 走不动，时间也不走了，一顿摸索之后发现是 timeScale 的问题，之前 hw2 的时候脚本中有这样的写法：

  ```csharp
  public static void GameOver(bool dead) {
      if (dead) {
          instance.gameOverPanel.SetActive(true);
          Time.timeScale = 0;  // here
      }
  }
  
  public void RestartGame() {
      Time.timeScale = 1;
      SceneManager.LoadScene(SceneManager.GetActiveScene().name);
  }
  ```

  在 hw2 里面没啥问题，但是 hw3 就不行- -（可能是 unity 版本的问题？hw3 用比较新的 unity 做的）

  反正把注释的那个地方改成 1 就解决问题了。

+ AudioSource 做成预制件的话有可能会在播放的时候出现 `Can not play a disabled audio source` 这个错误。倒也不是说 AudioSource 就不能做成预制件，这个错误出现的原因我觉得有可能是因为场景中没有这个 AudioSource 的 GameObject，因为声音总归是要游戏对象发出来的。

  而且需要注意的是，AudioSource 的 Play 方法一定要在被 disable 前被调用（也就是 SetActive = false 前）

+ 应用 2D 光照时，有的对象不受其影响，可能是因为 Sprite Renderer 的 Material 字段没有设为 `Sprite-Lit-Default`

##### Last-modified date: 2020.3.24, 5 p.m.