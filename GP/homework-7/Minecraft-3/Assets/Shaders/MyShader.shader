Shader "Unlit/MyShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _MainColor ("Main Color", Color) = (1,1,1,1)
        _Shininess ("Shininess", Float) = 10
        _OutlineColor ("Outline Color", Color) = (0, 0, 0, 1)
        _OutlineThickness ("Outline Thickness", Range(0, 0.08)) = 0.04
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex MyVertexProgram
            #pragma fragment MyFragmentProgram
            #pragma shader_feature USE_SPECULAR
            #pragma shader_feature USE_NORMAL_SHADER
            // make fog work
            #pragma multi_compile_fog

            #include "UnityCG.cginc"
            #include "UnityStandardBRDF.cginc"

            struct VertexData {
                float4 position : POSITION;
                float3 normal : NORMAL;
                float2 uv : TEXCOORD0;
            };

            struct FragmentData {
                float4 position : SV_POSITION;
                float2 uv : TEXCOORD0;
                float3 normal : TEXCOORD1;
                float3 worldPos : TEXCOORD2;
            };

            sampler2D _MainTex;
            float4 _MainTex_ST;
            float4 _MainColor;
            float _Shininess;

            FragmentData MyVertexProgram (VertexData v) {
                FragmentData i;
                i.position = UnityObjectToClipPos(v.position);
                i.normal = UnityObjectToWorldNormal(v.normal);
                i.uv = TRANSFORM_TEX(v.uv, _MainTex);
                i.worldPos = mul(unity_ObjectToWorld, v.position);
                return i;
            }

            float4 MyFragmentProgram (FragmentData i) : SV_TARGET {
                #if USE_NORMAL_SHADER
                    return float4(i.normal, 1);
                #endif

                float3 lightDir = _WorldSpaceLightPos0.xyz;
                float3 lightColor = _LightColor0.rgb;
                float3 diffuse = tex2D(_MainTex, i.uv).rgb * lightColor * DotClamped(lightDir, i.normal);
                fixed3 ambient = UNITY_LIGHTMODEL_AMBIENT.xyz * tex2D(_MainTex, i.uv).rgb;

                float3 viewDir = normalize(_WorldSpaceCameraPos - i.worldPos);
                float3 halfVector = normalize(lightDir + viewDir);
                float3 specular = float3(0,0,0);
                #if USE_SPECULAR
                    specular = pow(max(dot(i.normal, halfVector), 0.0), _Shininess);
                #endif

                return float4(diffuse + ambient + specular, 1);
            }   

            ENDCG
        }

        Pass {
            Cull Front

            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            #include "UnityCG.cginc"

            //tint of the texture
            fixed4 _OutlineColor;
            float _OutlineThickness;

            struct appdata {
                float4 vertex : POSITION;
                float4 normal : NORMAL;
            };

            struct v2f {
                float4 position : SV_POSITION;
            };

            v2f vert(appdata v) {
                v2f o;
                // convert the vertex positions from object space to clip space so they can be rendered
                // o.position = UnityObjectToClipPos(v.vertex + normalize(v.normal) * _OutlineThickness);
                o.position = UnityObjectToClipPos(v.vertex * (1 + _OutlineThickness));
                return o;
            }

            fixed4 frag(v2f i) : SV_TARGET {
                return _OutlineColor;
            }

            ENDCG
        }
    }
    CustomEditor "CustomShaderGUI"
}
