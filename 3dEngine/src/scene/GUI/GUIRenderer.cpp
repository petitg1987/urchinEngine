#include <GL/glew.h>
#include <memory>
#include <algorithm>

#include "scene/GUI/GUIRenderer.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/MediaManager.h"
#include "resources/font/Font.h"
#include "graphic/shader/ShaderManager.h"
#include "graphic/displayer/texture/TextureDisplayer.h"

namespace urchin {

    //Debug parameters
    bool DEBUG_DISPLAY_FONT_TEXTURE = false;

    GUIRenderer::GUIRenderer() :
        GUIShader(0),
        mProjectionLoc(0),
        translateDistanceLoc(0),
        diffuseTexSamplerLoc(0) {
        GUIShader = ShaderManager::instance()->createProgram("gui.vert", "", "gui.frag");

        ShaderManager::instance()->bind(GUIShader);
        mProjectionLoc  = glGetUniformLocation(GUIShader, "mProjection");
        translateDistanceLoc = glGetUniformLocation(GUIShader, "translateDistance");
        diffuseTexSamplerLoc = glGetUniformLocation(GUIShader, "diffuseTexture");
    }

    GUIRenderer::~GUIRenderer() {
        for (long i=(long)widgets.size()-1; i>=0; --i) {
            delete widgets[i];
        }

        ShaderManager::instance()->removeProgram(GUIShader);
    }

    void GUIRenderer::onResize(unsigned int sceneWidth, unsigned int sceneHeight) {
        //orthogonal matrix with origin at top left screen
        ShaderManager::instance()->bind(GUIShader);
        mProjection.setValues(2.0f/(float)sceneWidth, 0.0f, -1.0f,
            0.0f, -2.0f/(float)sceneHeight, 1.0f,
            0.0f, 0.0f, 1.0f);
        glUniformMatrix3fv(mProjectionLoc, 1, GL_FALSE, (const float*)mProjection);

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

        ShaderManager::instance()->bind(GUIShader);
        glUniform1i(diffuseTexSamplerLoc, 0);

        for (auto &widget : widgets) {
            if (widget->isVisible()) {
                Vector2<int> translateVector(widget->getGlobalPositionX(), widget->getGlobalPositionY());
                glUniform2iv(translateDistanceLoc, 1, (const int*)translateVector);

                widget->display(translateDistanceLoc, dt);
            }
        }

        if (DEBUG_DISPLAY_FONT_TEXTURE) {
            Font *font = MediaManager::instance()->getMedia<Font>("font/font.fnt");

            TextureDisplayer textureDisplayer(font->getTextureID(), TextureDisplayer::DEFAULT_VALUE);
            textureDisplayer.setPosition(TextureDisplayer::USER_DEFINED_X, TextureDisplayer::USER_DEFINED_Y);
            textureDisplayer.setSize(20.0f, (float)font->getDimensionTexture() + 20.0f, 20.0f, (float)font->getDimensionTexture() + 20.0f);
            textureDisplayer.initialize(512u, 512u, -1.0f, -1.0f);
            textureDisplayer.display();
            font->release();
        }
    }

}
