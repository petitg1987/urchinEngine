#include <algorithm>

#include "scene/UI/UIRenderer.h"
#include "resources/MediaManager.h"
#include "resources/font/Font.h"
#include "texture/render/TextureRenderer.h"
#include "graphic/shader/builder/ShaderBuilder.h"
#include "graphic/shader/data/ShaderDataSender.h"

namespace urchin {

    //Debug parameters
    bool DEBUG_DISPLAY_FONT_TEXTURE = false;

    UIRenderer::UIRenderer(const RenderTarget* renderTarget) :
            renderTarget(renderTarget),
            sceneWidth(0),
            sceneHeight(0) {
        uiShader = ShaderBuilder().createShader("ui.vert", "", "ui.frag");

        mProjectionShaderVar = ShaderVar(uiShader, "mProjection");
        translateDistanceShaderVar = ShaderVar(uiShader, "translateDistance");
        diffuseTexSamplerShaderVar = ShaderVar(uiShader, "diffuseTexture");

        int diffuseTexUnit = 0;
        ShaderDataSender().sendData(diffuseTexSamplerShaderVar, diffuseTexUnit);
    }

    UIRenderer::~UIRenderer() {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            delete widgets[(std::size_t)i];
        }
    }

    void UIRenderer::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        this->sceneWidth = sceneWidth;
        this->sceneHeight = sceneHeight;

        //orthogonal matrix with origin at top left screen
        mProjection.setValues(2.0f / (float)sceneWidth, 0.0f, -1.0f,
            0.0f, -2.0f / (float)sceneHeight, 1.0f,
            0.0f, 0.0f, 1.0f);
        ShaderDataSender().sendData(mProjectionShaderVar, mProjection);

        //widgets resize
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            widgets[(std::size_t)i]->onResize(sceneWidth, sceneHeight);
        }
    }

    void UIRenderer::notify(Observable* observable, int notificationType) {
        if (auto* widget = dynamic_cast<Widget*>(observable)) {
            if (notificationType == Widget::SET_IN_FOREGROUND) {
                auto it = std::find(widgets.begin(), widgets.end(), widget);
                widgets.erase(it);
                widgets.push_back(widget);

                //reset the other widgets
                for (long i = (long)widgets.size() - 2; i >= 0; --i) {
                    if (widgets[(std::size_t)i]->isVisible()) {
                        widgets[(std::size_t)i]->reset();
                    }
                }
            }
        }
    }

    bool UIRenderer::onKeyPress(unsigned int key) {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (widgets[(std::size_t)i]->isVisible() && !widgets[(std::size_t)i]->onKeyPress(key)) {
                return false;
            }
        }
        return true;
    }

    bool UIRenderer::onKeyRelease(unsigned int key) {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (widgets[(std::size_t)i]->isVisible() && !widgets[(std::size_t)i]->onKeyRelease(key)) {
                return false;
            }
        }
        return true;
    }

    bool UIRenderer::onChar(unsigned int character) {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (widgets[(std::size_t)i]->isVisible() && !widgets[(std::size_t)i]->onChar(character)) {
                return false;
            }
        }
        return true;
    }

    bool UIRenderer::onMouseMove(int mouseX, int mouseY) {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (widgets[(std::size_t)i]->isVisible() && !widgets[(std::size_t)i]->onMouseMove(mouseX, mouseY)) {
                return false;
            }
        }
        return true;
    }

    void UIRenderer::onDisable() {
        for (long i = (long)widgets.size() - 1; i >= 0; --i) {
            if (widgets[(std::size_t)i]->isVisible()) {
                widgets[(std::size_t)i]->onDisable();
            }
        }
    }

    void UIRenderer::addWidget(Widget* widget) {
        if(widget->getParent()) {
            throw std::runtime_error("Cannot add a widget having a parent to the UI renderer");
        }
        widgets.push_back(widget);

        widget->addObserver(this, Widget::SET_IN_FOREGROUND);
    }

    void UIRenderer::removeWidget(Widget* widget) {
        auto it = std::find(widgets.begin(), widgets.end(), widget);
        delete widget;
        widgets.erase(it);
    }

    void UIRenderer::display(float dt) {
        ScopeProfiler sp(Profiler::graphic(), "uiRenderDisplay");

        renderTarget->activeShader(uiShader);
        for (auto& widget : widgets) {
            if (widget->isVisible()) {
                Vector2<int> translateVector(widget->getGlobalPositionX(), widget->getGlobalPositionY());
                ShaderDataSender().sendData(translateDistanceShaderVar, translateVector);

                widget->display(renderTarget, translateDistanceShaderVar, dt);
            }
        }

        if (DEBUG_DISPLAY_FONT_TEXTURE) {
            Font* font = MediaManager::instance()->getMedia<Font>("UI/font/font.ttf", {{"fontSize", "16"}, {"fontColor", "1.0 1.0 1.0"}});

            TextureRenderer textureDisplayer(font->getTexture(), TextureRenderer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureRenderer::USER_DEFINED_X, TextureRenderer::USER_DEFINED_Y);
            textureDisplayer.setSize(20.0f, (float)font->getDimensionTexture() + 20.0f, 20.0f, (float)font->getDimensionTexture() + 20.0f);
            textureDisplayer.enableTransparency();
            textureDisplayer.initialize(sceneWidth, sceneHeight, -1.0f, -1.0f);
            textureDisplayer.display(renderTarget);
            font->release();
        }
    }

}
