Shader "Unlit/MyBRDFShader"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _NormalMap("Normal Map", 2D) = "bump"{}
        _Metallicness("Metallicness",Range(0,1)) = 0
        _Glossiness("Smoothness",Range(0,1)) = 1
    }
    SubShader
    {
        Tags { "RenderType"="Opaque" }
        LOD 100

        Pass
        {
            CGPROGRAM
            #pragma vertex vert
            #pragma fragment frag
            #pragma target 3.0

            #include "UnityCG.cginc"
            #include "UnityStandardBRDF.cginc"////dotclamped
            #include "AutoLight.cginc"
            #include "Lighting.cginc"

            struct VertexData {
                float4 position : POSITION;
                float3 normal : NORMAL;
                float4 tangent : TANGENT;
                float4 uv : TEXCOORD0;
            };

            struct FragmentData {
                float3 worldPos : TEXCOORD0;
                // these three vectors will hold a 3x3 rotation matrix
                // that transforms from tangent to world space
                half3 tspace0 : TEXCOORD1; // tangent.x, bitangent.x, normal.x
                half3 tspace1 : TEXCOORD2; // tangent.y, bitangent.y, normal.y
                half3 tspace2 : TEXCOORD3; // tangent.z, bitangent.z, normal.z
                // texture coordinate for the normal map
                float2 uv : TEXCOORD4;
                float4 position : SV_POSITION;
            };

            
            sampler2D _MainTex;
            float4 _MainTex_ST;
            sampler2D _NormalMap;
            float4 _NormalMap_ST;
            float _Glossiness;
            float _Metallicness;


            float3 Schlick_F(half3 R, half cosA)
            {
                //// TODO: your implementation
                return R + (1 - R) * pow(1 - cosA, 5);
                // return float3(1,1,1);
            }

            float GGX_D(float roughness, float NdotH)
            {
                //// TODO: your implementation
                float r2 = pow(roughness, 2);
                float nh2 = pow(NdotH, 2);
                return r2 / (UNITY_PI * pow(nh2 * (r2 - 1) + 1, 2));
                // return 1;
            }

            float CookTorrence_G (float NdotL, float NdotV, float VdotH, float NdotH){
                //// TODO: your implementation
                float a = (2 * NdotH * NdotV) / VdotH;
                float b = (2 * NdotH * NdotL) / VdotH;
                return min(1, min(a, b));
                // return 1;
            }

            FragmentData vert (VertexData v)
            {
                FragmentData o;
                o.position = UnityObjectToClipPos(v.position);
                o.worldPos = mul(unity_ObjectToWorld, v.position).xyz;
                half3 wNormal = UnityObjectToWorldNormal(v.normal);
                half3 wTangent = UnityObjectToWorldDir(v.tangent.xyz);
                // compute bitangent from cross product of normal and tangent
                half tangentSign = v.tangent.w * unity_WorldTransformParams.w;
                half3 wBitangent = cross(wNormal, wTangent) * tangentSign;
                // output the tangent space matrix
                o.tspace0 = half3(wTangent.x, wBitangent.x, wNormal.x);
                o.tspace1 = half3(wTangent.y, wBitangent.y, wNormal.y);
                o.tspace2 = half3(wTangent.z, wBitangent.z, wNormal.z);
                o.uv = TRANSFORM_TEX(v.uv, _MainTex);
                return o;
            }

            UnityIndirect GetUnityIndirect(float3 lightColor, float3 lightDirection, float3 normalDirection,float3 viewDirection, float3 viewReflectDirection, float attenuation, float roughness, float3 worldPos){
                //// Set UnityLight
                UnityLight light;
                light.color = lightColor;
                light.dir = lightDirection;
                light.ndotl = saturate(dot( normalDirection, lightDirection));

                //// Set UnityGIInput
                UnityGIInput d;
                d.light = light;
                d.worldPos = worldPos;
                d.worldViewDir = viewDirection;
                d.atten = attenuation;
                d.ambient = 0.0h;
                d.boxMax[0] = unity_SpecCube0_BoxMax;
                d.boxMin[0] = unity_SpecCube0_BoxMin;
                d.probePosition[0] = unity_SpecCube0_ProbePosition;
                d.probeHDR[0] = unity_SpecCube0_HDR;
                d.boxMax[1] = unity_SpecCube1_BoxMax;
                d.boxMin[1] = unity_SpecCube1_BoxMin;
                d.probePosition[1] = unity_SpecCube1_ProbePosition;
                d.probeHDR[1] = unity_SpecCube1_HDR;

                //// Set EnvironmentData
                Unity_GlossyEnvironmentData ugls_en_data;
                ugls_en_data.roughness = roughness;
                ugls_en_data.reflUVW = viewReflectDirection;
                
                //// GetGI
                UnityGI gi = UnityGlobalIllumination(d, 1.0h, normalDirection, ugls_en_data );
                return gi.indirect;
            }

            fixed4 frag (FragmentData i) : SV_Target
            {
                float4 mainTex = tex2D( _MainTex, i.uv );
				float4 normalTex = tex2D( _NormalMap, i.uv);

                //// Vectors
                float3 L = normalize(lerp(_WorldSpaceLightPos0.xyz, _WorldSpaceLightPos0.xyz - i.worldPos.xyz,_WorldSpaceLightPos0.w));
                float3 V = normalize(_WorldSpaceCameraPos.xyz - i.worldPos.xyz);
                float3 H = Unity_SafeNormalize(L + V);

                float3 tnormal = UnpackNormal(normalTex);
                // transform normal from tangent to world space
                float3 N;
                N.x = dot(i.tspace0, tnormal);
                N.y = dot(i.tspace1, tnormal);
                N.z = dot(i.tspace2, tnormal);
                
                float3 VR = Unity_SafeNormalize(reflect( -V, N ));

                //// Vector dot
                float NdotL = saturate( dot( N,L ));
                float NdotH = saturate( dot( N,H ));
                float NdotV = saturate( dot( N,V ));
                float VdotH = saturate( dot( V,H ));
                float LdotH = saturate( dot( L,H ));

                //// Light attenuation
                float attenuation = LIGHT_ATTENUATION(i);

                //// Indirect Global Illumination
                UnityIndirect gi =  GetUnityIndirect(_LightColor0.rgb, L, N, V, VR, attenuation, 1- _Glossiness, i.worldPos.xyz);

                //// Compute Roughness
                float perceptualRoughness = SmoothnessToPerceptualRoughness(_Glossiness);
                float roughness = PerceptualRoughnessToRoughness(perceptualRoughness);

                half oneMinusReflectivity;
                half3 specColor;
                float3 albedo = DiffuseAndSpecularFromMetallic (mainTex.rgb, _Metallicness, /*out*/ specColor, /*out*/ oneMinusReflectivity);

                //// BRDF
                //// 1. Diffuse term
                //// 1.1. Direct diffuse term
                float3 directDiffuse = albedo * DisneyDiffuse(NdotV, NdotL, LdotH, perceptualRoughness);
                //// 1.2. Indirect diffuse term
                float3 indirectDiffuse = gi.diffuse.rgb * albedo;
                
                //// 2. Specular term

                //// 2.1. Direct specular term
                float D = GGX_D(roughness, NdotH);
                float3 F = Schlick_F(specColor, LdotH);
                float G = CookTorrence_G(NdotL, NdotV, VdotH, NdotH);
                float3 directSpecular = (D * F * G) * UNITY_PI / (4 * (NdotL * NdotV));

                directSpecular = saturate(directSpecular);
                directDiffuse = saturate(directDiffuse);

                //// 2.2. Indirect specular term
                float grazingTerm = saturate(_Glossiness + (1-oneMinusReflectivity));
                float surfaceReduction = 1.0 / (roughness*roughness + 1.0);
                float3 indirectSpecular =  surfaceReduction * gi.specular * FresnelLerp (specColor, grazingTerm, NdotV);

                //// Sum up directLight and indirectLight
                float3 directLight = directDiffuse * NdotL + directSpecular * NdotL;
                float3 indirectLight =  indirectDiffuse + indirectSpecular;
                
                

                float4 color = float4(directLight * _LightColor0.rgb + indirectLight,1);

                color += float4( UNITY_LIGHTMODEL_AMBIENT.xyz * albedo,1);
                
                //// Direct Diffuse only
                // return float4(directLight,1);

                //// D only
                // return float4(float3(1,1,1)* D,1);

                //// F only (looks like pure color)
                // return float4(float3(1,1,1) * F,1);

                //// G only
                // return float4(float3(1,1,1)* G,1);

                return color;
            }
            ENDCG
        }
    }
}
