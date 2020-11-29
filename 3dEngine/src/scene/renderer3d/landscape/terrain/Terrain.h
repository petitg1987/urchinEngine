#ifndef URCHINENGINE_TERRAIN_H
#define URCHINENGINE_TERRAIN_H

#include <string>
#include <vector>
#include <memory>
#include "UrchinCommon.h"

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/target/RenderTarget.h"
#include "scene/renderer3d/landscape/terrain/TerrainMaterial.h"
#include "scene/renderer3d/landscape/terrain/TerrainMesh.h"
#include "scene/renderer3d/landscape/terrain/grass/TerrainGrass.h"
#include "scene/renderer3d/camera/Camera.h"
#include "resources/image/Image.h"

namespace urchin {

    class Terrain {
        public:
            Terrain(std::shared_ptr<TerrainMesh> &, std::unique_ptr<TerrainMaterial>&, const Point3<float>&);

            void onCameraProjectionUpdate(const Matrix4<float>&);

            void setMesh(const std::shared_ptr<TerrainMesh>&);
            const TerrainMesh *getMesh() const;
            void setMaterial(std::unique_ptr<TerrainMaterial> &);
            const TerrainMaterial *getMaterial() const;
            TerrainGrass *getGrass() const;

            void setPosition(const Point3<float>&);
            const Point3<float> &getPosition() const;

            float getAmbient() const;
            void setAmbient(float);

            Point3<float> findPointAt(const Point2<float>&) const;
            float findHeightAt(const Point2<float>&) const;

            void display(const RenderTarget*, const Camera*, float invFrameRate) const;

        private:
            void refreshMaterial();
            void refreshGrassMesh();
            void refreshGrassAmbient();

            std::unique_ptr<GenericRenderer> terrainRenderer;
            std::unique_ptr<Shader> terrainShader;
            ShaderVar vPositionShaderVar, mProjectionShaderVar, mViewShaderVar;
            ShaderVar ambientShaderVar;
            ShaderVar sRepeatShaderVar, tRepeatShaderVar;

            Matrix4<float> projectionMatrix;

            std::shared_ptr<TerrainMesh> mesh;
            std::unique_ptr<TerrainMaterial> material;
            std::unique_ptr<TerrainGrass> grass;
            Point3<float> position;
            float ambient;
    };

}

#endif
