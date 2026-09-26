#pragma once

namespace REngine {

inline const char* GetLightingVertexShaderCode() {
    return 
        "#version 330\n"
        "in vec3 vertexPosition;\n"
        "in vec2 vertexTexCoord;\n"
        "in vec3 vertexNormal;\n"
        "in vec4 vertexColor;\n"
        "\n"
        "uniform mat4 mvp;\n"
        "uniform mat4 matModel;\n"
        "uniform mat4 matNormal;\n"
        "\n"
        "out vec3 fragPosition;\n"
        "out vec2 fragTexCoord;\n"
        "out vec4 fragColor;\n"
        "out vec3 fragNormal;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    fragPosition = vec3(matModel * vec4(vertexPosition, 1.0));\n"
        "    fragTexCoord = vertexTexCoord;\n"
        "    fragColor = vertexColor;\n"
        "    fragNormal = normalize(vec3(matNormal * vec4(vertexNormal, 1.0)));\n"
        "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
        "}\n";
}

inline const char* GetLightingFragmentShaderCode() {
    return
        "#version 330\n"
        "in vec3 fragPosition;\n"
        "in vec2 fragTexCoord;\n"
        "in vec4 fragColor;\n"
        "in vec3 fragNormal;\n"
        "\n"
        "uniform sampler2D texture0;\n"
        "uniform vec4 colDiffuse;\n"
        "\n"
        "uniform vec3 lightDir;\n"
        "uniform vec4 lightColor;\n"
        "uniform vec4 ambientColor;\n"
        "uniform vec3 viewPos;\n"
        "\n"
        "out vec4 finalColor;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec3 normal = normalize(fragNormal);\n"
        "    float NdotL = max(dot(normal, -lightDir), 0.0);\n"
        "    vec3 diffuse = lightColor.rgb * NdotL;\n"
        "\n"
        "    vec3 viewDir = normalize(viewPos - fragPosition);\n"
        "    vec3 halfwayDir = normalize(-lightDir + viewDir);\n"
        "    float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);\n"
        "    vec3 specular = lightColor.rgb * spec * 0.15;\n"
        "\n"
        "    vec3 lightTotal = ambientColor.rgb + diffuse + specular;\n"
        "    vec4 baseColor = fragColor * colDiffuse;\n"
        "    finalColor = vec4(baseColor.rgb * lightTotal, baseColor.a);\n"
        "}\n";
}

} // namespace REngine
