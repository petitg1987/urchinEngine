float linearizeDepth(float depthValue, float nearPlane, float farPlane) {
    float unmapDepthValue = depthValue * 2.0 - 1.0;
    return (2.0 * nearPlane) / (farPlane + nearPlane - unmapDepthValue * (farPlane - nearPlane)); //[0.0 = nearPlane, 1.0 = far plane]
}
