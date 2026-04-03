#version 460 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D u_ScreenTexture;
uniform vec2 u_InvScreenSize; // (1.0 / width, 1.0 / height)

#define FXAA_EDGE_THRESHOLD      (1.0/8.0)
#define FXAA_EDGE_THRESHOLD_MIN  (1.0/24.0)
#define FXAA_SEARCH_STEPS        32
#define FXAA_SEARCH_THRESHOLD    (1.0/4.0)
#define FXAA_SUBPIX_CAP          (3.0/4.0)
#define FXAA_SUBPIX_TRIM         (1.0/4.0)
#define FXAA_SUBPIX_TRIM_SCALE   (1.0/(1.0 - FXAA_SUBPIX_TRIM))

float FxaaLuma(vec3 rgb)
{
    return rgb.y * (0.587 / 0.299) + rgb.x;
}

vec3 FxaaLerp3(vec3 a, vec3 b, float amountOfA)
{
    return (-amountOfA * b) + (a * amountOfA + b);
}

vec4 FxaaTexOff(vec2 pos, ivec2 off)
{
    vec2 coord = pos + vec2(off) * u_InvScreenSize;
    return texture(u_ScreenTexture, coord);
}

vec3 FxaaPixelShader(vec2 pos)
{
    vec3 rgbN = FxaaTexOff(pos, ivec2( 0,-1)).xyz;
    vec3 rgbW = FxaaTexOff(pos, ivec2(-1, 0)).xyz;
    vec3 rgbM = FxaaTexOff(pos, ivec2( 0, 0)).xyz;
    vec3 rgbE = FxaaTexOff(pos, ivec2( 1, 0)).xyz;
    vec3 rgbS = FxaaTexOff(pos, ivec2( 0, 1)).xyz;

    float lumaN = FxaaLuma(rgbN);
    float lumaW = FxaaLuma(rgbW);
    float lumaM = FxaaLuma(rgbM);
    float lumaE = FxaaLuma(rgbE);
    float lumaS = FxaaLuma(rgbS);

    float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
    float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
    float range = rangeMax - rangeMin;

    if (range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD))
    return rgbM;

    vec3 rgbL = rgbN + rgbW + rgbM + rgbE + rgbS;

    float lumaL = (lumaN + lumaW + lumaE + lumaS) * 0.25;
    float rangeL = abs(lumaL - lumaM);
    float blendL = max(0.0, (rangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
    blendL = min(FXAA_SUBPIX_CAP, blendL);

    vec3 rgbNW = FxaaTexOff(pos, ivec2(-1,-1)).xyz;
    vec3 rgbNE = FxaaTexOff(pos, ivec2( 1,-1)).xyz;
    vec3 rgbSW = FxaaTexOff(pos, ivec2(-1, 1)).xyz;
    vec3 rgbSE = FxaaTexOff(pos, ivec2( 1, 1)).xyz;

    rgbL += rgbNW + rgbNE + rgbSW + rgbSE;
    rgbL *= (1.0 / 9.0);

    float lumaNW = FxaaLuma(rgbNW);
    float lumaNE = FxaaLuma(rgbNE);
    float lumaSW = FxaaLuma(rgbSW);
    float lumaSE = FxaaLuma(rgbSE);

    float edgeVert =
    abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
    abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +
    abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));

    float edgeHorz =
    abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
    abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +
    abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));

    bool horzSpan = edgeHorz >= edgeVert;
    float lengthSign = horzSpan ? -u_InvScreenSize.y : -u_InvScreenSize.x;

    if (!horzSpan)
    {
        lumaN = lumaW;
        lumaS = lumaE;
    }

    float gradientN = abs(lumaN - lumaM);
    float gradientS = abs(lumaS - lumaM);

    lumaN = (lumaN + lumaM) * 0.5;
    lumaS = (lumaS + lumaM) * 0.5;

    if (gradientN < gradientS)
    {
        lumaN = lumaS;
        gradientN = gradientS;
        lengthSign *= -1.0;
    }

    vec2 posN;
    posN.x = pos.x + (horzSpan ? 0.0 : lengthSign * 0.5);
    posN.y = pos.y + (horzSpan ? lengthSign * 0.5 : 0.0);

    gradientN *= FXAA_SEARCH_THRESHOLD;

    vec2 posP = posN;
    vec2 offNP = horzSpan ? vec2(u_InvScreenSize.x, 0.0) : vec2(0.0, u_InvScreenSize.y);

    float lumaEndN = lumaN;
    float lumaEndP = lumaN;
    bool doneN = false;
    bool doneP = false;

    posN += offNP * vec2(-1.0, -1.0);
    posP += offNP * vec2( 1.0,  1.0);

    for (int i = 0; i < FXAA_SEARCH_STEPS; i++)
    {
        if (!doneN)
        lumaEndN = FxaaLuma(texture(u_ScreenTexture, posN).xyz);
        if (!doneP)
        lumaEndP = FxaaLuma(texture(u_ScreenTexture, posP).xyz);

        doneN = doneN || (abs(lumaEndN - lumaN) >= gradientN);
        doneP = doneP || (abs(lumaEndP - lumaN) >= gradientN);

        if (doneN && doneP)
        break;

        if (!doneN)
        posN -= offNP;
        if (!doneP)
        posP += offNP;
    }

    float dstN = horzSpan ? pos.x - posN.x : pos.y - posN.y;
    float dstP = horzSpan ? posP.x - pos.x : posP.y - pos.y;

    bool directionN = dstN < dstP;
    lumaEndN = directionN ? lumaEndN : lumaEndP;

    if (((lumaM - lumaN) < 0.0) == ((lumaEndN - lumaN) < 0.0))
    lengthSign = 0.0;

    float spanLength = dstP + dstN;
    dstN = directionN ? dstN : dstP;
    float subPixelOffset = (0.5 + (dstN * (-1.0 / spanLength))) * lengthSign;

    vec3 rgbF = texture(
        u_ScreenTexture,
        vec2(
        pos.x + (horzSpan ? 0.0 : subPixelOffset),
        pos.y + (horzSpan ? subPixelOffset : 0.0)
        )
    ).xyz;

    return FxaaLerp3(rgbL, rgbF, blendL);
}

void main()
{
    FragColor = vec4(FxaaPixelShader(vTexCoord), 1.0);
}