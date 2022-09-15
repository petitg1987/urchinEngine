#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(constant_id = 0) const uint FXAA_QUALITY_PS = 0;
layout(constant_id = 1) const float FXAA_QUALITY_P0 = -1.0;
layout(constant_id = 2) const float FXAA_QUALITY_P1 = -1.0;
layout(constant_id = 3) const float FXAA_QUALITY_P2 = -1.0;
layout(constant_id = 4) const float FXAA_QUALITY_P3 = -1.0;
layout(constant_id = 5) const float FXAA_QUALITY_P4 = -1.0;
layout(constant_id = 6) const float FXAA_QUALITY_P5 = -1.0;
layout(constant_id = 7) const float FXAA_QUALITY_P6 = -1.0;
layout(constant_id = 8) const float FXAA_QUALITY_P7 = -1.0;
layout(constant_id = 9) const float FXAA_QUALITY_P8 = -1.0;
layout(constant_id = 10) const float FXAA_QUALITY_P9 = -1.0;
layout(constant_id = 11) const float FXAA_QUALITY_P10 = -1.0;
layout(constant_id = 12) const float FXAA_QUALITY_P11 = -1.0;

layout(std140, set = 0, binding = 0) uniform Scene {
    vec2 invSceneSize;
} scene;
layout(binding = 1) uniform sampler2D tex;

layout(location = 0) in vec2 texCoordinates;

layout(location = 0) out vec4 fragColor;

void main() {
    //amount of sub-pixel aliasing removal (0.0 = off, 1.0 = softer)
    const float fxaaQualitySubpix = 0.8;

    //amount of local contrast required to apply algorithm (0.333 = too little (faster), 0.063 = overkill)
    const float fxaaQualityEdgeThreshold = 0.1;

    //trims the algorithm from processing darks (0.0312 = max pixel process, 0.3 = min pixel process)
    const float fxaaQualityEdgeThresholdMin = 0.1;

    vec2 posM;
    posM.x = texCoordinates.x;
    posM.y = texCoordinates.y;

    vec4 rgbyM = textureLod(tex, posM, 0.0);
    float lumaM = rgbyM.g;
    float lumaS = textureOffset(tex, posM, ivec2(0, 1)).g;
    float lumaE = textureOffset(tex, posM, ivec2(1, 0)).g;
    float lumaN = textureOffset(tex, posM, ivec2(0, -1)).g;
    float lumaW = textureOffset(tex, posM, ivec2(-1, 0)).g;

    float maxSM = max(lumaS, lumaM);
    float minSM = min(lumaS, lumaM);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaN, lumaW);
    float minWN = min(lumaN, lumaW);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);
    float rangeMaxScaled = rangeMax * fxaaQualityEdgeThreshold;
    float range = rangeMax - rangeMin;
    float rangeMaxClamped = max(fxaaQualityEdgeThresholdMin, rangeMaxScaled);

    if (range < rangeMaxClamped) {
        fragColor = rgbyM;
        return;
    }

    float lumaNW = textureOffset(tex, posM, ivec2(-1, -1)).g;
    float lumaSE = textureOffset(tex, posM, ivec2(1, 1)).g;
    float lumaNE = textureOffset(tex, posM, ivec2(1, -1)).g;
    float lumaSW = textureOffset(tex, posM, ivec2(-1, 1)).g;

    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0 / range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = (-2.0 * lumaM) + lumaNS;
    float edgeVert1 = (-2.0 * lumaM) + lumaWE;

    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = (-2.0 * lumaE) + lumaNESE;
    float edgeVert2 = (-2.0 * lumaN) + lumaNWNE;

    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
    float edgeHorz3 = (-2.0 * lumaW) + lumaNWSW;
    float edgeVert3 = (-2.0 * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;

    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float lengthSign = scene.invSceneSize.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;

    if (!horzSpan) {
        lumaN = lumaW;
        lumaS = lumaE;
    } else {
        lengthSign = scene.invSceneSize.y;
    }
    float subpixB = (subpixA * (1.0 / 12.0)) - lumaM;

    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    float lumaNN = lumaN + lumaM;
    float lumaSS = lumaS + lumaM;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if (pairN) {
        lengthSign = -lengthSign;
    }
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);

    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : scene.invSceneSize.x;
    offNP.y = (horzSpan) ? 0.0 : scene.invSceneSize.y;
    if (!horzSpan) {
        posB.x += lengthSign * 0.5;
    } else {
        posB.y += lengthSign * 0.5;
    }

    vec2 posN;
    posN.x = posB.x - offNP.x * FXAA_QUALITY_P0;
    posN.y = posB.y - offNP.y * FXAA_QUALITY_P0;
    vec2 posP;
    posP.x = posB.x + offNP.x * FXAA_QUALITY_P0;
    posP.y = posB.y + offNP.y * FXAA_QUALITY_P0;

    float subpixD = ((-2.0) * subpixC) + 3.0;
    float lumaEndN = textureLod(tex, posN, 0.0).g;
    float subpixE = subpixC * subpixC;
    float lumaEndP = textureLod(tex, posP, 0.0).g;

    if (!pairN) {
        lumaNN = lumaSS;
    }
    float gradientScaled = gradient * 1.0 / 4.0;
    float halfLumaNN = lumaNN * 0.5;
    float lumaMM = lumaM - halfLumaNN;
    float subpixF = subpixD * subpixE;

    lumaEndN -= halfLumaNN;
    lumaEndP -= halfLumaNN;
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if (!doneN) {
        posN.x -= offNP.x * FXAA_QUALITY_P1;
        posN.y -= offNP.y * FXAA_QUALITY_P1;
    }
    if (!doneP) {
        posP.x += offNP.x * FXAA_QUALITY_P1;
        posP.y += offNP.y * FXAA_QUALITY_P1;
    }

    if ((!doneN) || (!doneP)) {
        if (!doneN)
            lumaEndN = textureLod(tex, posN.xy, 0.0).g;
        if (!doneP)
            lumaEndP = textureLod(tex, posP.xy, 0.0).g;
        if (!doneN)
            lumaEndN = lumaEndN - halfLumaNN;
        if (!doneP)
            lumaEndP = lumaEndP - halfLumaNN;
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if (!doneN) {
            posN.x -= offNP.x * FXAA_QUALITY_P2;
            posN.y -= offNP.y * FXAA_QUALITY_P2;
        }
        if (!doneP) {
            posP.x += offNP.x * FXAA_QUALITY_P2;
            posP.y += offNP.y * FXAA_QUALITY_P2;
        }

        if ((!doneN) || (!doneP)) {
            if (!doneN)
                lumaEndN = textureLod(tex, posN.xy, 0.0).g;
            if (!doneP)
                lumaEndP = textureLod(tex, posP.xy, 0.0).g;
            if (!doneN)
                lumaEndN = lumaEndN - halfLumaNN;
            if (!doneP)
                lumaEndP = lumaEndP - halfLumaNN;
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if (!doneN) {
                posN.x -= offNP.x * FXAA_QUALITY_P3;
                posN.y -= offNP.y * FXAA_QUALITY_P3;
            }
            if (!doneP) {
                posP.x += offNP.x * FXAA_QUALITY_P3;
                posP.y += offNP.y * FXAA_QUALITY_P3;
            }

            if ((!doneN) || (!doneP)) {
                if (!doneN)
                    lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                if (!doneP)
                    lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                if (!doneN)
                    lumaEndN = lumaEndN - halfLumaNN;
                if (!doneP)
                    lumaEndP = lumaEndP - halfLumaNN;
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if (!doneN) {
                    posN.x -= offNP.x * FXAA_QUALITY_P4;
                    posN.y -= offNP.y * FXAA_QUALITY_P4;
                }
                if (!doneP) {
                    posP.x += offNP.x * FXAA_QUALITY_P4;
                    posP.y += offNP.y * FXAA_QUALITY_P4;
                }

                if ((!doneN) || (!doneP)) {
                    if (!doneN)
                        lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                    if (!doneP)
                        lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                    if (!doneN)
                        lumaEndN = lumaEndN - halfLumaNN;
                    if (!doneP)
                        lumaEndP = lumaEndP - halfLumaNN;
                    doneN = abs(lumaEndN) >= gradientScaled;
                    doneP = abs(lumaEndP) >= gradientScaled;
                    if (!doneN) {
                        posN.x -= offNP.x * FXAA_QUALITY_P5;
                        posN.y -= offNP.y * FXAA_QUALITY_P5;
                    }
                    if (!doneP) {
                        posP.x += offNP.x * FXAA_QUALITY_P5;
                        posP.y += offNP.y * FXAA_QUALITY_P5;
                    }

                    if (FXAA_QUALITY_PS > 6) {
                        if ((!doneN) || (!doneP)) {
                            if (!doneN)
                                lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                            if (!doneP)
                                lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                            if (!doneN)
                                lumaEndN = lumaEndN - halfLumaNN;
                            if (!doneP)
                                lumaEndP = lumaEndP - halfLumaNN;
                            doneN = abs(lumaEndN) >= gradientScaled;
                            doneP = abs(lumaEndP) >= gradientScaled;
                            if (!doneN) {
                                posN.x -= offNP.x * FXAA_QUALITY_P6;
                                posN.y -= offNP.y * FXAA_QUALITY_P6;
                            }
                            if (!doneP) {
                                posP.x += offNP.x * FXAA_QUALITY_P6;
                                posP.y += offNP.y * FXAA_QUALITY_P6;
                            }

                            if ((!doneN) || (!doneP)) {
                                if (!doneN)
                                    lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                                if (!doneP)
                                    lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                                if (!doneN)
                                    lumaEndN = lumaEndN - halfLumaNN;
                                if (!doneP)
                                    lumaEndP = lumaEndP - halfLumaNN;
                                doneN = abs(lumaEndN) >= gradientScaled;
                                doneP = abs(lumaEndP) >= gradientScaled;
                                if (!doneN) {
                                    posN.x -= offNP.x * FXAA_QUALITY_P7;
                                    posN.y -= offNP.y * FXAA_QUALITY_P7;
                                }
                                if (!doneP) {
                                    posP.x += offNP.x * FXAA_QUALITY_P7;
                                    posP.y += offNP.y * FXAA_QUALITY_P7;
                                }

                                if (FXAA_QUALITY_PS > 8) {
                                    if ((!doneN) || (!doneP)) {
                                        if (!doneN)
                                        lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                                        if (!doneP)
                                        lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                                        if (!doneN)
                                        lumaEndN = lumaEndN - halfLumaNN;
                                        if (!doneP)
                                        lumaEndP = lumaEndP - halfLumaNN;
                                        doneN = abs(lumaEndN) >= gradientScaled;
                                        doneP = abs(lumaEndP) >= gradientScaled;
                                        if (!doneN) {
                                            posN.x -= offNP.x * FXAA_QUALITY_P8;
                                            posN.y -= offNP.y * FXAA_QUALITY_P8;
                                        }
                                        if (!doneP) {
                                            posP.x += offNP.x * FXAA_QUALITY_P8;
                                            posP.y += offNP.y * FXAA_QUALITY_P8;
                                        }

                                        if ((!doneN) || (!doneP)) {
                                            if (!doneN)
                                            lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                                            if (!doneP)
                                            lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                                            if (!doneN)
                                            lumaEndN = lumaEndN - halfLumaNN;
                                            if (!doneP)
                                            lumaEndP = lumaEndP - halfLumaNN;
                                            doneN = abs(lumaEndN) >= gradientScaled;
                                            doneP = abs(lumaEndP) >= gradientScaled;
                                            if (!doneN) {
                                                posN.x -= offNP.x * FXAA_QUALITY_P9;
                                                posN.y -= offNP.y * FXAA_QUALITY_P9;
                                            }
                                            if (!doneP) {
                                                posP.x += offNP.x * FXAA_QUALITY_P9;
                                                posP.y += offNP.y * FXAA_QUALITY_P9;
                                            }

                                            if ((!doneN) || (!doneP)) {
                                                if (!doneN)
                                                lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                                                if (!doneP)
                                                lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                                                if (!doneN)
                                                lumaEndN = lumaEndN - halfLumaNN;
                                                if (!doneP)
                                                lumaEndP = lumaEndP - halfLumaNN;
                                                doneN = abs(lumaEndN) >= gradientScaled;
                                                doneP = abs(lumaEndP) >= gradientScaled;
                                                if (!doneN) {
                                                    posN.x -= offNP.x * FXAA_QUALITY_P10;
                                                    posN.y -= offNP.y * FXAA_QUALITY_P10;
                                                }
                                                if (!doneP) {
                                                    posP.x += offNP.x * FXAA_QUALITY_P10;
                                                    posP.y += offNP.y * FXAA_QUALITY_P10;
                                                }

                                                if ((!doneN) || (!doneP)) {
                                                    if (!doneN)
                                                    lumaEndN = textureLod(tex, posN.xy, 0.0).g;
                                                    if (!doneP)
                                                    lumaEndP = textureLod(tex, posP.xy, 0.0).g;
                                                    if (!doneN)
                                                    lumaEndN = lumaEndN - halfLumaNN;
                                                    if (!doneP)
                                                    lumaEndP = lumaEndP - halfLumaNN;
                                                    doneN = abs(lumaEndN) >= gradientScaled;
                                                    doneP = abs(lumaEndP) >= gradientScaled;
                                                    if (!doneN) {
                                                        posN.x -= offNP.x * FXAA_QUALITY_P11;
                                                        posN.y -= offNP.y * FXAA_QUALITY_P11;
                                                    }
                                                    if (!doneP) {
                                                        posP.x += offNP.x * FXAA_QUALITY_P11;
                                                        posP.y += offNP.y * FXAA_QUALITY_P11;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if (!horzSpan) {
        dstN = posM.y - posN.y;
        dstP = posP.y - posM.y;
    }

    bool lumaMMLowerZero = lumaMM < 0.0;
    bool goodSpanN = (lumaEndN < 0.0) != lumaMMLowerZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMMLowerZero;
    float spanLengthRcp = 1.0 / spanLength;

    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * fxaaQualitySubpix;

    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if (!horzSpan) {
        posM.x += pixelOffsetSubpix * lengthSign;
    } else {
        posM.y += pixelOffsetSubpix * lengthSign;
    }

    fragColor = vec4(textureLod(tex, posM, 0.0).xyz, lumaM);
}
