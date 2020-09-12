#version 450
#extension GL_EXT_gpu_shader4 : enable

//values are replaced at compilation time:
#define FXAA_QUALITY 0 //0=Low, 1=Medium, 2=High, 3=Very high

in vec2 textCoordinates;

uniform sampler2D tex;
uniform vec2 invSceneSize;

out vec4 fragColor;

#if(FXAA_QUALITY == 0)
    #define FXAA_QUALITY_PS 6
    #define FXAA_QUALITY_P0 1.0
    #define FXAA_QUALITY_P1 1.5
    #define FXAA_QUALITY_P2 2.0
    #define FXAA_QUALITY_P3 2.0
    #define FXAA_QUALITY_P4 4.0
    #define FXAA_QUALITY_P5 12.0
#endif

#if (FXAA_QUALITY == 1)
    #define FXAA_QUALITY_PS 8
    #define FXAA_QUALITY_P0 1.0
    #define FXAA_QUALITY_P1 1.5
    #define FXAA_QUALITY_P2 2.0
    #define FXAA_QUALITY_P3 2.0
    #define FXAA_QUALITY_P4 2.0
    #define FXAA_QUALITY_P5 2.0
    #define FXAA_QUALITY_P6 4.0
    #define FXAA_QUALITY_P7 8.0
#endif

#if (FXAA_QUALITY == 2)
    #define FXAA_QUALITY_PS 12
    #define FXAA_QUALITY_P0 1.0
    #define FXAA_QUALITY_P1 1.5
    #define FXAA_QUALITY_P2 2.0
    #define FXAA_QUALITY_P3 2.0
    #define FXAA_QUALITY_P4 2.0
    #define FXAA_QUALITY_P5 2.0
    #define FXAA_QUALITY_P6 2.0
    #define FXAA_QUALITY_P7 2.0
    #define FXAA_QUALITY_P8 2.0
    #define FXAA_QUALITY_P9 2.0
    #define FXAA_QUALITY_P10 4.0
    #define FXAA_QUALITY_P11 8.0
#endif

#if (FXAA_QUALITY == 3)
    #define FXAA_QUALITY_PS 12
    #define FXAA_QUALITY_P0 1.0
    #define FXAA_QUALITY_P1 1.0
    #define FXAA_QUALITY_P2 1.0
    #define FXAA_QUALITY_P3 1.0
    #define FXAA_QUALITY_P4 1.0
    #define FXAA_QUALITY_P5 1.5
    #define FXAA_QUALITY_P6 2.0
    #define FXAA_QUALITY_P7 2.0
    #define FXAA_QUALITY_P8 2.0
    #define FXAA_QUALITY_P9 2.0
    #define FXAA_QUALITY_P10 4.0
    #define FXAA_QUALITY_P11 8.0
#endif

float fxaaLuma(vec4 rgba){
    return rgba.y;
}  

void main(){
    //amount of sub-pixel aliasing removal (0.0=off, 1.0=softer)
    const float fxaaQualitySubpix = 0.8;

    //amount of local contrast required to apply algorithm (0.333=too little (faster), 0.063=overkill)
    const float fxaaQualityEdgeThreshold = 0.1;

    //trims the algorithm from processing darks (0.0312=max pixel process, 0.3=min pixel process)
    const float fxaaQualityEdgeThresholdMin = 0.1;

    vec2 posM;
    posM.x = textCoordinates.x;
    posM.y = textCoordinates.y;

    vec4 rgbyM = textureLod(tex, posM, 0.0);
    float lumaM = fxaaLuma(rgbyM);
    float lumaS = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2( 0, 1)));
    float lumaE = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2( 1, 0)));
    float lumaN = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2( 0,-1)));
    float lumaW = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2(-1, 0)));

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

    if(range < rangeMaxClamped){
        fragColor = rgbyM;
        return;
    }

    float lumaNW = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2(-1,-1)));
    float lumaSE = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2( 1, 1)));
    float lumaNE = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2( 1,-1)));
    float lumaSW = fxaaLuma(texture2DLodOffset(tex, posM, 0.0, ivec2(-1, 1)));

    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0/range;
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
    float lengthSign = invSceneSize.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = subpixNSWE * 2.0 + subpixNWSWNESE;

    if(!horzSpan){
        lumaN = lumaW;
        lumaS = lumaE;
    }else{
        lengthSign = invSceneSize.y;
    }
    float subpixB = (subpixA * (1.0/12.0)) - lumaM;

    float gradientN = lumaN - lumaM;
    float gradientS = lumaS - lumaM;
    float lumaNN = lumaN + lumaM;
    float lumaSS = lumaS + lumaM;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if(pairN){
        lengthSign = -lengthSign;
    }
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);

    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : invSceneSize.x;
    offNP.y = (horzSpan) ? 0.0 : invSceneSize.y;
    if(!horzSpan){
        posB.x += lengthSign * 0.5;
    }else{
        posB.y += lengthSign * 0.5;
    }

    vec2 posN;
    posN.x = posB.x - offNP.x * FXAA_QUALITY_P0;
    posN.y = posB.y - offNP.y * FXAA_QUALITY_P0;
    vec2 posP;
    posP.x = posB.x + offNP.x * FXAA_QUALITY_P0;
    posP.y = posB.y + offNP.y * FXAA_QUALITY_P0;

    float subpixD = ((-2.0)*subpixC) + 3.0;
    float lumaEndN = fxaaLuma(textureLod(tex, posN, 0.0));
    float subpixE = subpixC * subpixC;
    float lumaEndP = fxaaLuma(textureLod(tex, posP, 0.0));

    if(!pairN){
        lumaNN = lumaSS;
    }
    float gradientScaled = gradient * 1.0/4.0;
    float halfLumaNN = lumaNN * 0.5;
    float lumaMM = lumaM - halfLumaNN;
    float subpixF = subpixD * subpixE;

    lumaEndN -= halfLumaNN;
    lumaEndP -= halfLumaNN;
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if(!doneN){
        posN.x -= offNP.x * FXAA_QUALITY_P1;
        posN.y -= offNP.y * FXAA_QUALITY_P1;
    }
    if(!doneP){
        posP.x += offNP.x * FXAA_QUALITY_P1;
        posP.y += offNP.y * FXAA_QUALITY_P1;
    }

    if((!doneN) || (!doneP)){
        if(!doneN)
            lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
        if(!doneP)
            lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
        if(!doneN)
            lumaEndN = lumaEndN - halfLumaNN;
        if(!doneP)
            lumaEndP = lumaEndP - halfLumaNN;
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if(!doneN){
            posN.x -= offNP.x * FXAA_QUALITY_P2;
            posN.y -= offNP.y * FXAA_QUALITY_P2;
        }
        if(!doneP){
            posP.x += offNP.x * FXAA_QUALITY_P2;
            posP.y += offNP.y * FXAA_QUALITY_P2;
        }

        if((!doneN) || (!doneP)){
            if(!doneN)
                lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
            if(!doneP)
                lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
            if(!doneN)
                lumaEndN = lumaEndN - halfLumaNN;
            if(!doneP)
                lumaEndP = lumaEndP - halfLumaNN;
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if(!doneN){
                posN.x -= offNP.x * FXAA_QUALITY_P3;
                posN.y -= offNP.y * FXAA_QUALITY_P3;
            }
            if(!doneP){
                posP.x += offNP.x * FXAA_QUALITY_P3;
                posP.y += offNP.y * FXAA_QUALITY_P3;
            }

            if((!doneN) || (!doneP)){
                if(!doneN)
                    lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                if(!doneP)
                    lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                if(!doneN)
                    lumaEndN = lumaEndN - halfLumaNN;
                if(!doneP)
                    lumaEndP = lumaEndP - halfLumaNN;
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if(!doneN){
                    posN.x -= offNP.x * FXAA_QUALITY_P4;
                    posN.y -= offNP.y * FXAA_QUALITY_P4;
                }
                if(!doneP){
                    posP.x += offNP.x * FXAA_QUALITY_P4;
                    posP.y += offNP.y * FXAA_QUALITY_P4;
                }

                if((!doneN) || (!doneP)){
                    if(!doneN)
                        lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                    if(!doneP)
                        lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                    if(!doneN)
                        lumaEndN = lumaEndN - halfLumaNN;
                    if(!doneP)
                        lumaEndP = lumaEndP - halfLumaNN;
                    doneN = abs(lumaEndN) >= gradientScaled;
                    doneP = abs(lumaEndP) >= gradientScaled;
                    if(!doneN){
                        posN.x -= offNP.x * FXAA_QUALITY_P5;
                        posN.y -= offNP.y * FXAA_QUALITY_P5;
                    }
                    if(!doneP){
                        posP.x += offNP.x * FXAA_QUALITY_P5;
                        posP.y += offNP.y * FXAA_QUALITY_P5;
                    }

                    #if (FXAA_QUALITY_PS > 6)
                        if((!doneN) || (!doneP)){
                            if(!doneN)
                                lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                            if(!doneP)
                                lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                            if(!doneN)
                                lumaEndN = lumaEndN - halfLumaNN;
                            if(!doneP)
                                lumaEndP = lumaEndP - halfLumaNN;
                            doneN = abs(lumaEndN) >= gradientScaled;
                            doneP = abs(lumaEndP) >= gradientScaled;
                            if(!doneN){
                                posN.x -= offNP.x * FXAA_QUALITY_P6;
                                posN.y -= offNP.y * FXAA_QUALITY_P6;
                            }
                            if(!doneP){
                                posP.x += offNP.x * FXAA_QUALITY_P6;
                                posP.y += offNP.y * FXAA_QUALITY_P6;
                            }

                            if((!doneN) || (!doneP)){
                                if(!doneN)
                                    lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                                if(!doneP)
                                    lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                                if(!doneN)
                                    lumaEndN = lumaEndN - halfLumaNN;
                                if(!doneP)
                                    lumaEndP = lumaEndP - halfLumaNN;
                                doneN = abs(lumaEndN) >= gradientScaled;
                                doneP = abs(lumaEndP) >= gradientScaled;
                                if(!doneN){
                                    posN.x -= offNP.x * FXAA_QUALITY_P7;
                                    posN.y -= offNP.y * FXAA_QUALITY_P7;
                                }
                                if(!doneP){
                                    posP.x += offNP.x * FXAA_QUALITY_P7;
                                    posP.y += offNP.y * FXAA_QUALITY_P7;
                                }

                                #if (FXAA_QUALITY_PS > 8)
                                    if((!doneN) || (!doneP)){
                                        if(!doneN)
                                            lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                                        if(!doneP)
                                            lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                                        if(!doneN)
                                            lumaEndN = lumaEndN - halfLumaNN;
                                        if(!doneP)
                                            lumaEndP = lumaEndP - halfLumaNN;
                                        doneN = abs(lumaEndN) >= gradientScaled;
                                        doneP = abs(lumaEndP) >= gradientScaled;
                                        if(!doneN){
                                            posN.x -= offNP.x * FXAA_QUALITY_P8;
                                            posN.y -= offNP.y * FXAA_QUALITY_P8;
                                        }
                                        if(!doneP){
                                            posP.x += offNP.x * FXAA_QUALITY_P8;
                                            posP.y += offNP.y * FXAA_QUALITY_P8;
                                        }

                                        if((!doneN) || (!doneP)){
                                            if(!doneN)
                                                lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                                            if(!doneP)
                                                lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                                            if(!doneN)
                                                lumaEndN = lumaEndN - halfLumaNN;
                                            if(!doneP)
                                                lumaEndP = lumaEndP - halfLumaNN;
                                            doneN = abs(lumaEndN) >= gradientScaled;
                                            doneP = abs(lumaEndP) >= gradientScaled;
                                            if(!doneN){
                                                posN.x -= offNP.x * FXAA_QUALITY_P9;
                                                posN.y -= offNP.y * FXAA_QUALITY_P9;
                                            }
                                            if(!doneP){
                                                posP.x += offNP.x * FXAA_QUALITY_P9;
                                                posP.y += offNP.y * FXAA_QUALITY_P9;
                                            }

                                            if((!doneN) || (!doneP)){
                                                if(!doneN)
                                                    lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                                                if(!doneP)
                                                    lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                                                if(!doneN)
                                                    lumaEndN = lumaEndN - halfLumaNN;
                                                if(!doneP)
                                                    lumaEndP = lumaEndP - halfLumaNN;
                                                doneN = abs(lumaEndN) >= gradientScaled;
                                                doneP = abs(lumaEndP) >= gradientScaled;
                                                if(!doneN){
                                                    posN.x -= offNP.x * FXAA_QUALITY_P10;
                                                    posN.y -= offNP.y * FXAA_QUALITY_P10;
                                                }
                                                if(!doneP){
                                                    posP.x += offNP.x * FXAA_QUALITY_P10;
                                                    posP.y += offNP.y * FXAA_QUALITY_P10;
                                                }

                                                if((!doneN) || (!doneP)){
                                                    if(!doneN)
                                                        lumaEndN = fxaaLuma(textureLod(tex, posN.xy, 0.0));
                                                    if(!doneP)
                                                        lumaEndP = fxaaLuma(textureLod(tex, posP.xy, 0.0));
                                                    if(!doneN)
                                                        lumaEndN = lumaEndN - halfLumaNN;
                                                    if(!doneP)
                                                        lumaEndP = lumaEndP - halfLumaNN;
                                                    doneN = abs(lumaEndN) >= gradientScaled;
                                                    doneP = abs(lumaEndP) >= gradientScaled;
                                                    if(!doneN){
                                                        posN.x -= offNP.x * FXAA_QUALITY_P11;
                                                        posN.y -= offNP.y * FXAA_QUALITY_P11;
                                                    }
                                                    if(!doneP){
                                                        posP.x += offNP.x * FXAA_QUALITY_P11;
                                                        posP.y += offNP.y * FXAA_QUALITY_P11;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                #endif
                            }
                        }
                    #endif
                }
            }
        }
    }

    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if(!horzSpan){
        dstN = posM.y - posN.y;
        dstP = posP.y - posM.y;
    }

    bool lumaMMLowerZero = lumaMM < 0.0;
    bool goodSpanN = (lumaEndN < 0.0) != lumaMMLowerZero;
    float spanLength = (dstP + dstN);
    bool goodSpanP = (lumaEndP < 0.0) != lumaMMLowerZero;
    float spanLengthRcp = 1.0/spanLength;

    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * fxaaQualitySubpix;

    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if(!horzSpan){
        posM.x += pixelOffsetSubpix * lengthSign;
    }else{
        posM.y += pixelOffsetSubpix * lengthSign;
    }

    fragColor = vec4(textureLod(tex, posM, 0.0).xyz, lumaM);
}
