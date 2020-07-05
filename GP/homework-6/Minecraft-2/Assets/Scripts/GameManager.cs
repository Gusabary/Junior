using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static GameManager instance;
    GameObject camera;
    public Material TopNormal;
    public Material SideNormal;
    public Material BottomNormal;
    public Material TopPencil;
    public Material SidePencil;
    public Material BottomPencil;
    public static bool isPencil;

    private void Awake() {
        if (instance != null) {
            Destroy(gameObject);
        }
        instance = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        camera = GameObject.Find("Main Camera");
        isPencil = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.F)) {
            bool original = camera.GetComponent<FogController>().enabled;
            camera.GetComponent<FogController>().enabled = !original;
        }
        if (Input.GetKeyDown(KeyCode.C)) {
            GameObject[] voxels = GameObject.FindGameObjectsWithTag("voxel");
            foreach (GameObject voxel in voxels) {
                if (isPencil) {
                    voxel.transform.Find("Top").GetComponent<Renderer>().material = TopNormal;
                    voxel.transform.Find("Front").GetComponent<Renderer>().material = SideNormal;
                    voxel.transform.Find("Back").GetComponent<Renderer>().material = SideNormal;
                    voxel.transform.Find("Left").GetComponent<Renderer>().material = SideNormal;
                    voxel.transform.Find("Right").GetComponent<Renderer>().material = SideNormal;
                    voxel.transform.Find("Bottom").GetComponent<Renderer>().material = BottomNormal;
                }
                else {
                    voxel.transform.Find("Top").GetComponent<Renderer>().material = TopPencil;
                    voxel.transform.Find("Front").GetComponent<Renderer>().material = SidePencil;
                    voxel.transform.Find("Back").GetComponent<Renderer>().material = SidePencil;
                    voxel.transform.Find("Left").GetComponent<Renderer>().material = SidePencil;
                    voxel.transform.Find("Right").GetComponent<Renderer>().material = SidePencil;
                    voxel.transform.Find("Bottom").GetComponent<Renderer>().material = BottomPencil;
                }
            }
            isPencil = !isPencil;
        }
    }
}
