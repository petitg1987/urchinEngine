vec3 downSampleBlur13Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize) {
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

vec3 downSampleBlur4Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize) {
    vec3 topLeft = texture(inputTex, uv + texelSize * vec2(-1.0, -1.0)).rgb;
    vec3 topRight = texture(inputTex, uv + texelSize * vec2(1.0, -1.0)).rgb;
    vec3 bottomLeft = texture(inputTex, uv + texelSize * vec2(-1.0, 1.0)).rgb;
    vec3 bottomRight = texture(inputTex, uv + texelSize * vec2(1.0, 1.0)).rgb;

    return (1.0 / 4.0) * (topLeft + topRight + bottomLeft + bottomRight);
}

vec3 upSample9Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize, float sampleScale) {
    vec3 topLeft = texture(inputTex, uv + texelSize * vec2(-sampleScale, -sampleScale)).rgb;
    vec3 topMiddle = texture(inputTex, uv + texelSize * vec2(0.0, -sampleScale)).rgb * 2.0;
    vec3 topRight = texture(inputTex, uv + texelSize * vec2(sampleScale, -sampleScale)).rgb;

    vec3 middleLeft = texture(inputTex, uv + texelSize * vec2(-sampleScale, 0.0)).rgb * 2.0;
    vec3 center = texture(inputTex, uv + texelSize).rgb * 4.0;
    vec3 middleRight = texture(inputTex, uv + texelSize * vec2(sampleScale, 0.0)).rgb * 2.0;

    vec3 bottomLeft = texture(inputTex, uv + texelSize * vec2(-sampleScale, sampleScale)).rgb;
    vec3 bottomMiddle = texture(inputTex, uv + texelSize * vec2(0.0, sampleScale)).rgb * 2.0;
    vec3 bottomRight = texture(inputTex, uv + texelSize * vec2(sampleScale, sampleScale)).rgb;

    return (1.0 / 16.0) * (topLeft + topMiddle + topRight + middleLeft + center + middleRight + bottomLeft + bottomMiddle + bottomRight);
}

vec3 upSample4Fetch(sampler2D inputTex, vec2 uv, vec2 texelSize, float sampleScale) {
    vec3 topLeft = texture(inputTex, uv + texelSize * vec2(-sampleScale, -sampleScale)).rgb;
    vec3 topRight = texture(inputTex, uv + texelSize * vec2(sampleScale, -sampleScale)).rgb;
    vec3 bottomLeft = texture(inputTex, uv + texelSize * vec2(-sampleScale, sampleScale)).rgb;
    vec3 bottomRight = texture(inputTex, uv + texelSize * vec2(sampleScale, sampleScale)).rgb;

    return (1.0 / 4.0) * (topLeft + topRight + bottomLeft + bottomRight);
}

vec3 sampleCatmullRom(sampler2D inputTex, vec2 uv) {
    // Samples a texture with Catmull-Rom filtering, using 9 texture fetches instead of 16.
    // We're going to sample a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    vec2 resolution = textureSize(inputTex, 0);
    vec2 samplePosition = uv * resolution;
    vec2 texPos1 = floor(samplePosition - 0.5f) + 0.5f;

    // Compute the fractional offset from our starting texel to our original sample location, which we'll
    // feed into the Catmull-Rom spline function to get our filter weights.
    vec2 f = samplePosition - texPos1;

    // Compute the Catmull-Rom weights using the fractional offset that we calculated earlier.
    // These equations are pre-expanded based on our knowledge of where the texels will be located,
    // which lets us avoid having to evaluate a piece-wise function.
    vec2 w0 = f * (-0.5f + f * (1.0f - 0.5f * f));
    vec2 w1 = 1.0f + f * f * (-2.5f + 1.5f * f);
    vec2 w2 = f * (0.5f + f * (2.0f - 1.5f * f));
    vec2 w3 = f * f * (-0.5f + 0.5f * f);

    // Work out weighting factors and sampling offsets that will let us use bilinear filtering to
    // simultaneously evaluate the middle 2 samples from the 4x4 grid.
    vec2 w12 = w1 + w2;
    vec2 offset12 = w2 / (w1 + w2);

    // Compute the final UV coordinates we'll use for sampling the texture
    vec2 texPos0 = (texPos1 - 1) / resolution;
    vec2 texPos3 = (texPos1 + 2) / resolution;
    vec2 texPos12 = (texPos1 + offset12) / resolution;

    vec3 result = vec3(0.0, 0.0, 0.0);
    result += textureLod(inputTex, vec2(texPos0.x, texPos0.y), 0).rgb * w0.x * w0.y;
    result += textureLod(inputTex, vec2(texPos12.x, texPos0.y), 0).rgb * w12.x * w0.y;
    result += textureLod(inputTex, vec2(texPos3.x, texPos0.y), 0).rgb * w3.x * w0.y;

    result += textureLod(inputTex, vec2(texPos0.x, texPos12.y), 0).rgb * w0.x * w12.y;
    result += textureLod(inputTex, vec2(texPos12.x, texPos12.y), 0).rgb * w12.x * w12.y;
    result += textureLod(inputTex, vec2(texPos3.x, texPos12.y), 0).rgb * w3.x * w12.y;

    result += textureLod(inputTex, vec2(texPos0.x, texPos3.y), 0).rgb * w0.x * w3.y;
    result += textureLod(inputTex, vec2(texPos12.x, texPos3.y), 0).rgb * w12.x * w3.y;
    result += textureLod(inputTex, vec2(texPos3.x, texPos3.y), 0).rgb * w3.x * w3.y;

    return result;
}
