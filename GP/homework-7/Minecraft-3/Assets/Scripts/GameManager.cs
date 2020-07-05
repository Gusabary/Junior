using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

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
    public Text fps;
    private float totalTime = 0f;
    private float lastTime = 0f;

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
        instance.totalTime += Time.deltaTime;
        if (instance.totalTime - instance.lastTime > 1) {
            // update fps
            float curFps = 1 / Time.deltaTime;
            instance.fps.text = curFps.ToString("000");
            instance.lastTime = instance.totalTime;
        }
        
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
        if (Input.GetKeyDown(KeyCode.T)) {
            // increase number of hair
            GameObject go = GameObject.FindWithTag("hair");
            GameObject newgo = Instantiate(go, go.transform.position, Quaternion.identity);
            newgo.transform.parent = go.transform.parent;
            newgo.name = "NewHair";
        }
        if (Input.GetKeyDown(KeyCode.Y)) {
            // decrease number of hair
            GameObject[] hairs = GameObject.FindGameObjectsWithTag("hair");
            if (hairs.Length > 1) {
                Destroy(hairs[0]);
            }
        }
        if (Input.GetKeyDown(KeyCode.G)) {
            // increase length of hair
            GameObject[] hairs = GameObject.FindGameObjectsWithTag("hair");
            foreach (GameObject hair in hairs) {
                var tran = hair.transform;
                while (tran.childCount > 0) {
                    tran = tran.GetChild(0);
                }
                GameObject child = GameObject.CreatePrimitive(PrimitiveType.Sphere);
                child.transform.parent = tran.transform;
                child.transform.localPosition = new Vector3(0, -2.5f, 0);
            }
        }
        if (Input.GetKeyDown(KeyCode.H)) {
            // decrease length of hair
            GameObject[] hairs = GameObject.FindGameObjectsWithTag("hair");
            foreach (GameObject hair in hairs) {
                var tran = hair.transform;
                bool find = false;
                while (tran.childCount > 0) {
                    find = true;
                    tran = tran.GetChild(0);
                }
                if (find)
                    Destroy(tran.gameObject);
            }
        }
    }
}
