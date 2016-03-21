#include <GL/gl.h>
#include <memory>

#include "scene/GUI/GUIRenderer.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/font/Font.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/texture/TextureDisplayer.h"

namespace urchin
{
	
	GUIRenderer::GUIRenderer() :
		GUIShader(0),
		mProjectionLoc(0),
		translateDistanceLoc(0),
		diffuseTexSamplerLoc(0)
	{
		GUIShader = ShaderManager::instance()->createProgram("gui.vert", "gui.frag");

		ShaderManager::instance()->bind(GUIShader);
		mProjectionLoc  = glGetUniformLocation(GUIShader, "mProjection");
		translateDistanceLoc = glGetUniformLocation(GUIShader, "translateDistance");
		diffuseTexSamplerLoc = glGetUniformLocation(GUIShader, "diffuseTexture");
	}

	GUIRenderer::~GUIRenderer()
	{
		for(int i=widgets.size()-1;i>=0;--i)
		{
			delete widgets[i];
		}

		ShaderManager::instance()->removeProgram(GUIShader);
	}

	void GUIRenderer::onResize(unsigned int width, unsigned int height)
	{
		//orthogonal matrix with origin at top left screen
		ShaderManager::instance()->bind(GUIShader);
		mProjection.setValues(2.0f/(float)width, 0.0, -1.0,
			0.0, -2.0f/(float)height, 1.0,
			0.0, 0.0, 1.0);
		glUniformMatrix3fv(mProjectionLoc, 1, false, (const float*)mProjection);

		//widgets resize
		for(int i=widgets.size()-1;i>=0;--i)
		{
			widgets[i]->onResize(width, height);
		}
	}

	void GUIRenderer::notify(Observable *observable, int notificationType)
	{
		if(Widget *widget = dynamic_cast<Widget *>(observable))
		{
			switch(notificationType)
			{
				case Widget::SET_IN_FOREGROUND:
				{
					std::vector<Widget *>::iterator it = std::find(widgets.begin(), widgets.end(), widget);
					widgets.erase(it);
					widgets.push_back(widget);

					//reset the other widgets
					for(int i=widgets.size()-2;i>=0;--i)
					{
						if(widgets[i]->isVisible())
						{
							widgets[i]->reset();
						}
					}

					break;
				}
			}
		}
	}

	void GUIRenderer::setupSkin(const std::string &skinFilename)
	{
		GUISkinService::instance()->setSkin(skinFilename);
	}

	bool GUIRenderer::onKeyDown(unsigned int key)
	{
		if(key<260)
		{
			for(int i=widgets.size()-1;i>=0;--i)
			{
				if(widgets[i]->isVisible() && !widgets[i]->onKeyDown(key))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool GUIRenderer::onKeyUp(unsigned int key)
	{
		if(key<260)
		{
			for(int i=widgets.size()-1;i>=0;--i)
			{
				if(widgets[i]->isVisible() && !widgets[i]->onKeyUp(key))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool GUIRenderer::onChar(unsigned int character)
	{
		for(int i=widgets.size()-1;i>=0;--i)
		{
			if(widgets[i]->isVisible() && !widgets[i]->onChar(character))
			{
				return false;
			}
		}
		return true;
	}

	bool GUIRenderer::onMouseMove(int mouseX, int mouseY)
	{
		for(int i=widgets.size()-1;i>=0;--i)
		{
			if(widgets[i]->isVisible() && !widgets[i]->onMouseMove(mouseX, mouseY))
			{
				return false;
			}
		}
		return true;
	}

	void GUIRenderer::addWidget(Widget *widget)
	{
		widgets.push_back(widget);

		widget->addObserver(this, Widget::SET_IN_FOREGROUND);
	}

	void GUIRenderer::removeWidget(Widget *widget)
	{
		std::vector<Widget *>::iterator it = std::find(widgets.begin(), widgets.end(), widget);
		delete widget;
		widgets.erase(it);
	}

	void GUIRenderer::display(float invFrameRate)
	{
		ShaderManager::instance()->bind(GUIShader);
		glUniform1i(diffuseTexSamplerLoc, 0);

		glActiveTexture(GL_TEXTURE0);

		for(unsigned int i=0;i<widgets.size();++i)
		{
			if(widgets[i]->isVisible())
			{
				Vector2<int> translateVector(widgets[i]->getGlobalPositionX(), widgets[i]->getGlobalPositionY());
				glUniform2iv(translateDistanceLoc, 1, (const int*)translateVector);

				widgets[i]->display(translateDistanceLoc, invFrameRate);
			}
		}

		#ifdef _DEBUG
			//display font texture
//			std::shared_ptr<XmlChunk> fontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "text", XmlAttribute("nameSkin", "defaultSkin"));
//			std::shared_ptr<XmlChunk> fileFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "ttf", XmlAttribute(), fontChunk);
//			std::shared_ptr<XmlChunk> sizeFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "size", XmlAttribute(), fontChunk);
//			Font *font = MediaManager::instance()->getMedia<Font>(fileFontChunk->getStringValue(), (void*) sizeFontChunk->getIntValue());
//
//			TextureDisplayer textureDisplayer(font->getTextureID(), TextureDisplayer::DEFAULT_VALUE);
//			textureDisplayer.setPosition(TextureDisplayer::USER_DEFINED_X, TextureDisplayer::USER_DEFINED_Y);
//			textureDisplayer.setSize(20.0, font->getDimensionTexture() + 20.0, 20.0, font->getDimensionTexture() + 20.0);
//			textureDisplayer.initialize(width, height, -1.0, -1.0);
//			textureDisplayer.display();
//			font->release();
		#endif
	}

}
