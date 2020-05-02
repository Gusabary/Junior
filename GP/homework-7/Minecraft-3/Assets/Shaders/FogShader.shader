Shader "Unlit/FogShader"
{
    Properties
    {
        _MainTex("MainTex", 2D) = "white" {}
    }

    SubShader
    {
        pass
        {
            ZWrite Off ZTest Off Cull Off

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #include "UnityCG.cginc"

            sampler2D _CameraDepthTexture;
            sampler2D _MainTex;
            float4 _MainTex_TexelSize;
            float4x4 _ClipToCameraMatrix;
            half _FogEnd;
            half _FogStart;

            struct VertexData {
                float4 vertex : POSITION;
                // float3 normal : NORMAL;
                float2 texcoord : TEXCOORD0;
            };

            struct v2f
            {
                half2 uv : TEXCOORD0;
                half2 depthUV : TEXCOORD0;
                float4 pos : SV_POSITION;
            };

            v2f vert(VertexData v)
            {
                v2f f;
                f.uv = v.texcoord;
                f.depthUV = v.texcoord;
                f.pos = UnityObjectToClipPos(v.vertex);
                
                // #if UNITY_UV_STARTS_AT_TOP
                // if(_MainTex_TexelSize.y < 0)
                // {
                //     f.depthUV.y = 1 - f.depthUV.y;
                // }               
                // #endif
                
                return f;
            }

            fixed4 frag(v2f f) : SV_Target
            {
                float depth = SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, f.uv);
                // #if defined(UNITY_REVERSED_Z)
                    depth = 1 - depth;
                // #endif

                float4 centerUV = float4(f.uv.x * 2 - 1, f.uv.y * 2 - 1, depth * 2 - 1, 1);
                float4 positionInCameraSpace = mul(_ClipToCameraMatrix, centerUV);
                positionInCameraSpace /= positionInCameraSpace.w;
                
                fixed4 screenColor = tex2D(_MainTex, f.uv);
                half fogDensity = smoothstep(_FogStart, _FogEnd, length(positionInCameraSpace));
                fixed4 finalColor = lerp(screenColor, fixed4(1, 1, 1, 1), fogDensity);
                // fixed4 finalColor = float4(fogDensity,fogDensity,fogDensity,1);
                return fixed4(finalColor.rgb, 1);
            }
            ENDCG
        }
    }

    FallBack "Diffuse"
}