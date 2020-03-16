using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spawner : MonoBehaviour
{
    public List<GameObject> platforms;
    public float spawnTime;
    private float countTime;
    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        countTime += Time.deltaTime;

        if (countTime >= spawnTime) {
            SpawnPlatform();
            countTime = 0;
        }
    }

    public void SpawnPlatform() {
        Vector3 spawnPosition = transform.position;
        float range = 2.5f;
        spawnPosition.x = Random.Range(-range, range);
        spawnPosition.y = transform.position.y + 3;

        int index = Random.Range(0, platforms.Count);
        GameObject go = Instantiate(platforms[index], spawnPosition, Quaternion.identity);
        go.transform.SetParent(this.gameObject.transform);
    }
}
