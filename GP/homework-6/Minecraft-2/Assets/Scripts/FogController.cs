using UnityEngine;

public class FogController : MonoBehaviour
{
    private Material material;
    public float fogEnd = 2000;
    public float fogStart = 0;

    private void OnEnable()
    {
        Camera.main.depthTextureMode |= DepthTextureMode.Depth;
        material = new Material(Shader.Find("Unlit/FogShader"));
    }

    private void OnDisable()
    {
        Camera.main.depthTextureMode &= ~DepthTextureMode.Depth;
    }

    private void OnRenderImage(RenderTexture source, RenderTexture destination)
    {
        var inverseProjectionMatrix = Camera.main.projectionMatrix.inverse;
        material.SetMatrix("_ClipToCameraMatrix", inverseProjectionMatrix);
        material.SetFloat("_FogStart", fogStart);
        material.SetFloat("_FogEnd", fogEnd);
        Graphics.Blit(source, destination, material);
    }
}