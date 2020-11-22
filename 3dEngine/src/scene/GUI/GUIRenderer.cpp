#include <memory>
#include <algorithm>

#include "scene/GUI/GUIRenderer.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/MediaManager.h"
#include "resources/font/Font.h"
#include "texture/renderer/TextureRenderer.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    //Debug parameters
    bool DEBUG_DISPLAY_FONT_TEXTURE = false;

    GUIRenderer::GUIRenderer() :
            sceneWidth(0),
            sceneHeight(0) {
        guiShader = ShaderBuilder().createShader("gui.vert", "", "gui.frag");

        mProjectionShaderVar = ShaderVar(guiShader, "mProjection");
        translateDistanceShaderVar = ShaderVar(guiShader, "translateDistance");
        diffuseTexSamplerShaderVar = ShaderVar(guiShader, "diffuseTexture");

        int diffuseTexUnit = 0;
        ShaderDataSender().sendData(diffuseTexSamplerShaderVar, diffuseTexUnit);
    }

    GUIRenderer::~GUIRenderer() {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            delete widgets[i];
        }
    }

    void GUIRenderer::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //orthogonal matrix with origin at top left screen
        mProjection.setValues(2.0f/(float)sceneWidth, 0.0f, -1.0f,
            0.0f, -2.0f/(float)sceneHeight, 1.0f,
            0.0f, 0.0f, 1.0f);
        ShaderDataSender().sendData(mProjectionShaderVar, mProjection);

        //widgets resize
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            widgets[i]->onResize(sceneWidth, sceneHeight);
        }
    }

    void GUIRenderer::notify(Observable *observable, int notificationType) {
        if (auto *widget = dynamic_cast<Widget *>(observable)) {
            if (notificationType==Widget::SET_IN_FOREGROUND) {
                auto it = std::find(widgets.begin(), widgets.end(), widget);
                widgets.erase(it);
                widgets.push_back(widget);

                //reset the other widgets
                for (long i=(long)widgets.size()-2; i>=0; --i) {
                    if (widgets[i]->isVisible()) {
                        widgets[i]->reset();
                    }
                }
            }
        }
    }

    void GUIRenderer::setupSkin(const std::string &skinFilename) {
        GUISkinService::instance()->setSkin(skinFilename);
    }

    bool GUIRenderer::onKeyPress(unsigned int key) {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            if (widgets[i]->isVisible() && !widgets[i]->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool GUIRenderer::onKeyRelease(unsigned int key) {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            if (widgets[i]->isVisible() && !widgets[i]->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool GUIRenderer::onChar(unsigned int character) {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            if (widgets[i]->isVisible() && !widgets[i]->onChar(character)) {
                return false;
            }
        }
        return true;
    }

    bool GUIRenderer::onMouseMove(int mouseX, int mouseY) {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            if (widgets[i]->isVisible() && !widgets[i]->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    void GUIRenderer::onDisable() {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            if (widgets[i]->isVisible()) {
                widgets[i]->onDisable();
            }
        }
    }

    void GUIRenderer::addWidget(Widget *widget) {
        widgets.push_back(widget);

        widget->addObserver(this, Widget::SET_IN_FOREGROUND);
    }

    void GUIRenderer::removeWidget(Widget *widget) {
        auto it = std::find(widgets.begin(), widgets.end(), widget);
        delete widget;
        widgets.erase(it);
    }

    void GUIRenderer::display(float dt) {
        ScopeProfiler profiler("3d", "uiRenderDisplay");

        guiShader->bind();
        for (auto &widget : widgets) {
            if (widget->isVisible()) {
                Vector2<int> translateVector(widget->getGlobalPositionX(), widget->getGlobalPositionY());
                ShaderDataSender().sendData(translateDistanceShaderVar, translateVector);

                widget->display(translateDistanceShaderVar, dt);
            }
        }

        if (DEBUG_DISPLAY_FONT_TEXTURE) {
            Font *font = MediaManager::instance()->getMedia<Font>("UI/font/font.fnt");

            TextureRenderer textureDisplayer(font->getTextureID(), TextureRenderer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureRenderer::USER_DEFINED_X, TextureRenderer::USER_DEFINED_Y);
            textureDisplayer.setSize(20.0f, (float)font->getDimensionTexture() + 20.0f, 20.0f, (float)font->getDimensionTexture() + 20.0f);
            textureDisplayer.enableTransparency();
            textureDisplayer.initialize(sceneWidth, sceneHeight, -1.0f, -1.0f);
            textureDisplayer.display();
            font->release();
        }
    }

}
