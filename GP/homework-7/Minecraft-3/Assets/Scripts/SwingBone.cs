using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public class BoneNode
{
    //父节点
    public Transform transform;
    //父节点方向
    public Quaternion localRotation;
    //前帧/本帧的位置  
    public Vector3 prePos, curPos;
    //和上一节点的止动长度
    public float length;
}

//如果Editor中可编辑,必需声明特性Serializable
[Serializable]
public class ColliderBone
{
    public Transform transform;
    public float radius = 0.5f;
}

public class SwingBone : MonoBehaviour
{
    //本身的碰撞体大小
    public float radius = 0.05f;
    //拖拉力
    public float dragForce = 0.4f;
    //自己的刚性，如0.001布 10石头，由boneAxis方向
    public float stiffnessForce = 0.1f;
    //父子线切线 
    public Vector3 boneAxis = new Vector3(-1.0f, 0.0f, 0.0f);

    //用户添加影响力 以关链节点的坐标系为准
    private Vector3 swingForce = Vector3.zero;
    //添加影响力各分量的scale
    public Vector3 swingScale = Vector3.one;

    public AnimationCurve swingXCurve = AnimationCurve.Linear(0, 0, 1, 0);
    public AnimationCurve swingYCurve = AnimationCurve.Linear(0, 0, 1, 0);
    public AnimationCurve swingZCurve = AnimationCurve.Linear(0, 0, 1, 0);
    //检查碰撞
    public ColliderBone[] colliders;

    public bool isDebug = false;

    private bool startParent = false;
    //当前节点下的子节点链.自动构造
    private List<BoneNode> nodes = new List<BoneNode>();
    void Start()
    {
        var tran = this.transform;
        if (startParent && tran.parent != null)
        {
            nodes.Add(GetNode(tran.parent, tran));
        }
        while (tran.childCount > 0)
        {
            var child = tran.GetChild(0);
            var node = GetNode(tran, child);
            nodes.Add(node);
            tran = child;
        }
    }
    //tran当前节点，child联动的子节点
    public BoneNode GetNode(Transform tran, Transform child)
    {
        BoneNode node = new BoneNode();
        node.transform = tran;
        node.localRotation = tran.transform.localRotation;
        node.prePos = child.position;
        node.curPos = child.position;
        node.length = Vector3.Distance(child.position, tran.position);
        return node;
    }

    private float GetCurveValue(AnimationCurve curve)
    {
        if (curve.keys.Length == 0)
        {
            return 0.0f;
        }
        else if (curve.keys.Length == 1)
        {
            return curve.keys[0].value;
        }
        var start = curve.keys[0].time;
        var end = curve.keys[curve.length - 1].time;
        float loc = Time.time % (end - start) + start;
        var result = curve.Evaluate(loc);
        return result;
    }

    // 更新用户自己设置的力
    void Update()
    {
        var x = GetCurveValue(swingXCurve);
        var y = GetCurveValue(swingYCurve);
        var z = GetCurveValue(swingZCurve);

        swingForce = new Vector3(swingScale.x * x, swingScale.y * y, swingScale.z * z);
    }

    public void LateUpdate()
    {
        for (int n = 0; n < nodes.Count; n++)
        {
            var node = nodes[n];
            //回复方向
            node.transform.localRotation = Quaternion.identity * node.localRotation;
            //平方的求导，对于力来说，时间的平分差比
            float sqrDt = Time.deltaTime * Time.deltaTime;
            //刚性影响的父子节点
            Vector3 force = node.transform.rotation * (boneAxis * stiffnessForce) / sqrDt;
            //drag 从现在位置指向上个位置 拖力方向
            force += (node.prePos - node.curPos) * dragForce / sqrDt;
            //添加用户影响力
            force += (node.transform.rotation * swingForce) / sqrDt;

            //简单阻尼效果的Verlet数值积分 x(t+T)=x(t)+d*(x(t)-x(t-T))+a(t)*T^2 
            //T-时间增量 x(t)-t时间位置 a(t)-t时间加速度(force/mass)
            Vector3 temp = node.curPos;
            //verlet计算结果 新的现在位置 x(t+T)=x(t)+d*(x(t)-x(t-T))+a(t)*T^2 
            node.curPos = (node.curPos - node.prePos) + node.curPos + (force * sqrDt);
            //新位置保持与父节点的长度一样 
            node.curPos = ((node.curPos - node.transform.position).normalized * node.length) + node.transform.position;
            node.prePos = temp;

            //currTipPos表示下一节点位置，currTipPos如果和colliders碰撞，重新设定currTipPos位置
            //大致算法，在球半径与节点半径内，移到球半径与节点半径外，根据当前位置trs重新方位上位置
            if (colliders.Length > 0)
            {
                for (int i = 0; i < colliders.Length; i++)
                {
                    if (Vector3.Distance(node.curPos, colliders[i].transform.position) <= (radius + colliders[i].radius))
                    {
                        Vector3 normal = (node.curPos - colliders[i].transform.position).normalized;
                        node.curPos = colliders[i].transform.position + (normal * (radius + colliders[i].radius));
                        node.curPos = ((node.curPos - node.transform.position).normalized * node.length) + node.transform.position;
                    }
                }
            }

            //boneAxis 转到世界坐标系下 统一计算
            Vector3 aimVector = node.transform.TransformDirection(boneAxis);
            //从aimVector到新父子节点(世界坐标系) 的旋转
            Quaternion aimRotation = Quaternion.FromToRotation(aimVector, node.curPos - node.transform.position);
            //重新设置当前节点的旋转方向 
            node.transform.rotation = aimRotation * node.transform.rotation;
        }
    }
}

