
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
 
public class MaterialChange : MonoBehaviour {
 
	// Use this for initialization
	void Start () {
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	public void SetParam(float metallicness, float smoothness){
		MaterialPropertyBlock propertyBlock = new MaterialPropertyBlock();
	    GetComponent<Renderer>().GetPropertyBlock(propertyBlock);
	    propertyBlock.SetFloat("_Metallicness", metallicness);
	    propertyBlock.SetFloat("_Glossiness", smoothness);
	    GetComponent<Renderer>().SetPropertyBlock(propertyBlock);
	   
	}
}