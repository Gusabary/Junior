using UnityEngine;
using System.Collections;
using System.IO;

public class PendulumController : MonoBehaviour {
    public Transform m_anchor;        //圆点
    public float g = 9.8f;            //重力加速度
    private Vector3 m_rotateAxis;    //旋转轴
    private float w = 0;                //角速度
    public float h = 1f;
    public int mode = 0;  // 0: Explicit Euler, 1: Midpoint, 2: Trapezoid
    private float totalTime = 0f;
    FileStream fs;
    StreamWriter sw;
    public float totalThelta;

    // Use this for initialization
    void Start () {
        //求出旋转轴
        m_rotateAxis = Vector3.Cross (transform.position - m_anchor.position, Vector3.down);
        fs = new FileStream("standardn3.csv", FileMode.OpenOrCreate, FileAccess.ReadWrite);
        sw = new StreamWriter(fs);
    }
    void DoPhysics()
    {
        float deltaTime = Time.deltaTime * h;
        totalTime += deltaTime;
        float r = Vector3.Distance (m_anchor.position, transform.position);
        float l = Vector3.Distance (new Vector3 (m_anchor.position.x, transform.position.y, m_anchor.position.z), transform.position);
        //当钟摆摆动到另外一侧时，l为负，则角加速度alpha为负。
        Vector3 axis = Vector3.Cross (transform.position - m_anchor.position, Vector3.down);
        if (Vector3.Dot (axis, m_rotateAxis) < 0) {
            l = -l;
        }
        
        float cosalpha = l / r;
        float theta = Mathf.Asin(cosalpha);
        //求角加速度
        float alpha = 0f;
        //累计角速度
        if (mode == 0) {
            alpha = cosalpha * (g / r);
        }
        else if (mode == 1) {
            alpha = Mathf.Sin(theta - cosalpha * (deltaTime * g / 2 / r)) * (g / r);
        }
        else if (mode == 2) {
            float a = cosalpha;
            float b = Mathf.Sin(theta - cosalpha * (deltaTime * g / r));
            alpha = (a + b) * (g / 2 / r);
        }
        w += alpha * deltaTime;
        //求角位移(乘以180/PI 是为了将弧度转换为角度)
        float thelta = w * deltaTime * 180.0f / Mathf.PI;
        totalThelta += thelta;
        sw.Write(totalTime);
        sw.Write(",");
        sw.Write(totalThelta);
        sw.WriteLine();
        //绕圆点m_ahchor的旋转轴m_rotateAxis旋转thelta角度
        transform.RotateAround (m_anchor.position, m_rotateAxis, thelta);
    }
    // Update is called once per frame
    void Update () {
        DoPhysics ();
    }
}