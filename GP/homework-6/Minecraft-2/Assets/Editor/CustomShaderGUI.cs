using UnityEngine;
using UnityEditor;
using System;

public class CustomShaderGUI : ShaderGUI
{
    MaterialEditor editor;
    MaterialProperty[] properties;
    Material target;
    enum SpecularChoice {
        True, False
    }
    enum ShaderChoice {
        Normal, BlinnPhong
    }
    public override void OnGUI(MaterialEditor editor, MaterialProperty[] properties)
    {
        this.editor = editor;
        this.properties = properties;
        this.target = editor.target as Material;
        
        MaterialProperty mainTex = FindProperty("_MainTex", properties);
        GUIContent mainTexLabel = new GUIContent(mainTex.displayName);
        editor.TextureProperty(mainTex, mainTexLabel.text);

        SpecularChoice specularChoice = SpecularChoice.False;
        if (target.IsKeywordEnabled("USE_SPECULAR"))
            specularChoice = SpecularChoice.True;

        ShaderChoice shaderChoice = ShaderChoice.BlinnPhong;
        if (target.IsKeywordEnabled("USE_NORMAL_SHADER"))
            shaderChoice = ShaderChoice.Normal;

        EditorGUI.BeginChangeCheck();
        specularChoice = (SpecularChoice)EditorGUILayout.EnumPopup(
            new GUIContent("Use Specular?"), specularChoice
        );
        shaderChoice = (ShaderChoice)EditorGUILayout.EnumPopup(
            new GUIContent("Which shader to use?"), shaderChoice
        );

        if (EditorGUI.EndChangeCheck()) {
            if (specularChoice == SpecularChoice.True)
                target.EnableKeyword("USE_SPECULAR");
            else
                target.DisableKeyword("USE_SPECULAR");

            if (shaderChoice == ShaderChoice.Normal)
                target.EnableKeyword("USE_NORMAL_SHADER");
            else
                target.DisableKeyword("USE_NORMAL_SHADER");
        }

        if (specularChoice == SpecularChoice.True) {
            MaterialProperty shininess = FindProperty("_Shininess", properties);
            GUIContent shininessLabel = new GUIContent(shininess.displayName);
            editor.FloatProperty(shininess, "Specular Factor");
        }

        MaterialProperty outlineColor = FindProperty("_OutlineColor", properties);
        editor.ColorProperty(outlineColor, "Outline Color");

        MaterialProperty outlineThickness = FindProperty("_OutlineThickness", properties);
        editor.RangeProperty(outlineThickness, "Outline Thickness");
    }
}