vec3 downSampleBox13Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize) {
    //See http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare

    //center square texels
    vec3 centerA = texture(inputTex, uv + texelSize * vec2(-1.0, -1.0)).rgb;
    vec3 centerB = texture(inputTex, uv + texelSize * vec2(1.0, -1.0)).rgb;
    vec3 centerC = texture(inputTex, uv + texelSize * vec2(-1.0, 1.0)).rgb;
    vec3 centerD = texture(inputTex, uv + texelSize * vec2(1.0, 1.0)).rgb;

    //top texels
    vec3 topA = texture(inputTex, uv + texelSize * vec2(-2.0, -2.0)).rgb;
    vec3 topB = texture(inputTex, uv + texelSize * vec2(0.0, -2.0)).rgb;
    vec3 topC = texture(inputTex, uv + texelSize * vec2(2.0, -2.0)).rgb;

    //middle texels
    vec3 middleA = texture(inputTex, uv + texelSize * vec2(-2.0, 0.0)).rgb;
    vec3 middleB = texture(inputTex, uv).rgb;
    vec3 middleC = texture(inputTex, uv + texelSize * vec2(-2.0, 0.0)).rgb;

    //bottom texels
    vec3 bottomA = texture(inputTex, uv + texelSize * vec2(-2.0, 2.0)).rgb;
    vec3 bottomB = texture(inputTex, uv + texelSize * vec2(0.0, 2.0)).rgb;
    vec3 bottomC = texture(inputTex, uv + texelSize * vec2(2.0, 2.0)).rgb;

    const float centerSquareWeight = (1.0 / 4.0) * 0.5;
    const float cornerSquaresWeight = (1.0 / 4.0) * 0.125;

    vec3 averageValue = centerSquareWeight * (centerA + centerB + centerC + centerD); //center square (red)
    averageValue += cornerSquaresWeight * (topA + topB + middleA + middleB); //top left square (yellow)
    averageValue += cornerSquaresWeight * (topB + topC + middleB + middleC); //top right square (green)
    averageValue += cornerSquaresWeight * (middleA + middleB + bottomA + bottomB); //bottom left square (purple)
    averageValue += cornerSquaresWeight * (middleB + middleC + bottomB + bottomC); //bottom right square (blue)

    return averageValue;
}

vec3 upSampleTent9Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize) {
    vec3 topLeft = texture(inputTex, uv + texelSize * vec2(-1.0, -1.0)).rgb; //TODO add tent radius/scale ?
    vec3 topMiddle = texture(inputTex, uv + texelSize * vec2(0.0, -1.0)).rgb * 2.0 /* weight */;
    vec3 topRight = texture(inputTex, uv + texelSize * vec2(1.0, -1.0)).rgb;

    vec3 middleLeft = texture(inputTex, uv + texelSize * vec2(-1.0, 0.0)).rgb * 2.0 /* weight */;
    vec3 center = texture(inputTex, uv + texelSize).rgb * 4.0 /* weight */;
    vec3 middleRight = texture(inputTex, uv + texelSize * vec2(1.0, 0.0)).rgb * 2.0 /* weight */;

    vec3 bottomLeft = texture(inputTex, uv + texelSize * vec2(-1.0, 1.0)).rgb;
    vec3 bottomMiddle = texture(inputTex, uv + texelSize * vec2(0.0, 1.0)).rgb * 2.0 /* weight */;
    vec3 bottomRight = texture(inputTex, uv + texelSize * vec2(1.0, 1.0)).rgb;

    return (1.0 / 16.0) * (topLeft + topMiddle + topRight + middleLeft + center + middleRight + bottomLeft + bottomMiddle + bottomRight);
}
