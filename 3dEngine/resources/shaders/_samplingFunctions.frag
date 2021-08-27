vec3 downSampleBox13Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize) {
    //See http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare

    const float CENTER_SQUARE_WEIGHT = (1.0 / 4.0) * 0.5;
    const float CORNER_SQUARES_WEIGHT = (1.0 / 4.0) * 0.125;

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

    vec3 averageValue = CENTER_SQUARE_WEIGHT * (centerA + centerB + centerC + centerD); //center square (red)
    averageValue += CORNER_SQUARES_WEIGHT * (topA + topB + middleA + middleB); //top left square (yellow)
    averageValue += CORNER_SQUARES_WEIGHT * (topB + topC + middleB + middleC); //top right square (green)
    averageValue += CORNER_SQUARES_WEIGHT * (middleA + middleB + bottomA + bottomB); //bottom left square (purple)
    averageValue += CORNER_SQUARES_WEIGHT * (middleB + middleC + bottomB + bottomC); //bottom right square (blue)

    return averageValue;
}

vec3 upSampleTent9Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize) {
    vec3 topLeft = texture(inputTex, uv + texelSize * vec2(-1.0, -1.0)).rgb; //TODO add tent radius/scale ?
    vec3 topMiddle = texture(inputTex, uv + texelSize * vec2(0.0, -1.0)).rgb * 2.0;
    vec3 topRight = texture(inputTex, uv + texelSize * vec2(1.0, -1.0)).rgb;

    vec3 middleLeft = texture(inputTex, uv + texelSize * vec2(-1.0, 0.0)).rgb * 2.0;
    vec3 center = texture(inputTex, uv + texelSize).rgb * 4.0;
    vec3 middleRight = texture(inputTex, uv + texelSize * vec2(1.0, 0.0)).rgb * 2.0;

    vec3 bottomLeft = texture(inputTex, uv + texelSize * vec2(-1.0, 1.0)).rgb;
    vec3 bottomMiddle = texture(inputTex, uv + texelSize * vec2(0.0, 1.0)).rgb * 2.0;
    vec3 bottomRight = texture(inputTex, uv + texelSize * vec2(1.0, 1.0)).rgb;

    return (1.0 / 16.0) * (topLeft + topMiddle + topRight + middleLeft + center + middleRight + bottomLeft + bottomMiddle + bottomRight);
}
