using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CloudSpawner : MonoBehaviour
{
    public List<GameObject> clouds;
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
            SpawnCloud();
            countTime = 0;
        }
    }

    public void SpawnCloud() {
        Vector3 spawnPosition = transform.position;
        spawnPosition.x = transform.position.x;
        spawnPosition.y = Random.Range(-0.25f, 0.25f) + 0.6f;

        int index = Random.Range(0, clouds.Count);
        GameObject go = Instantiate(clouds[index], spawnPosition, Quaternion.identity);
        go.transform.SetParent(this.gameObject.transform);
    }
}
