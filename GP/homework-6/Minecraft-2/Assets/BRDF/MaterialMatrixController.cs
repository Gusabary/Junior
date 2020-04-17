using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MaterialMatrixController : UnitySingleton<MaterialMatrixController>
{
    public GameObject prefab;
    public List<List<GameObject>> materialMatrix;
    int rowSize = 5;
    int colSize = 5;

    // Start is called before the first frame update
    void Start()
    {


        materialMatrix = new List<List<GameObject>>();
        for (int rowIdx = 0; rowIdx < rowSize; rowIdx++)
        {
            materialMatrix.Add(new List<GameObject>());
            materialMatrix[rowIdx] = new List<GameObject>();
            for (int colIdx = 0; colIdx < colSize; colIdx++)
            {
                GameObject go = Instantiate(prefab) as GameObject;
                go.transform.SetParent(transform);
                go.transform.position += new Vector3(10 * colIdx, 10 * rowIdx, 0);
                // go.transform.GetComponent<MaterialChange>().SetParam(0.5f,0.5f);
                go.GetComponent<MaterialChange>().SetParam(1.0f/(rowSize-1) * rowIdx, 1.0f/(colSize-1) * colIdx);
            }
        }

    }

    // Update is called once per frame
    void Update()
    {
    }
}
