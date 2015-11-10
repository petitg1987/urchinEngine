#include <GL/gl.h>
#include <memory>

#include "scene/GUI/GUIManager.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/font/Font.h"
#include "resources/MediaManager.h"
#include "utils/shader/ShaderManager.h"
#include "utils/display/texture/DisplayTexture.h"

namespace urchin
{
	
	GUIManager::GUIManager() :
		isInitialized(false),
		width(0),
		height(0),
		lastUpdatedWidget(nullptr),
		GUIShader(0),
		mProjectionLoc(0),
		translateDistanceLoc(0),
		diffuseTexSamplerLoc(0),
		stopPropagateEvents(false)
	{

	}

	GUIManager::~GUIManager() 
	{
		for(int i=widgets.size()-1;i>=0;--i)
		{
			delete widgets[i];
		}

		ShaderManager::instance()->removeProgram(GUIShader);
	}

	void GUIManager::initialize()
	{
		if(isInitialized)
		{
			throw std::runtime_error("GUI manager is already initialized.");
		}

		GUIShader = ShaderManager::instance()->createProgram("gui.vert", "gui.frag");

		mProjectionLoc  = glGetUniformLocation(GUIShader, "mProjection");
		translateDistanceLoc = glGetUniformLocation(GUIShader, "translateDistance");
		diffuseTexSamplerLoc = glGetUniformLocation(GUIShader, "diffuseTexture");

		isInitialized = true;
	}

	void GUIManager::onResize(int width, int height)
	{
		//is initialized
		if(!isInitialized)
		{
			return;
		}
		this->width = width;
		this->height = height;

		//orthogonal matrix with origin at top left screen
		ShaderManager::instance()->bind(GUIShader);
		mProjection.setValues(2.0f/(float)width, 0.0, -1.0,
			0.0, -2.0f/(float)height, 1.0,
			0.0, 0.0, 1.0);
		glUniformMatrix3fv(mProjectionLoc, 1, false, (const float*)mProjection);
	}

	void GUIManager::notify(Observable *observable, int notificationType)
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

					stopPropagateEvents=true;

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

	void GUIManager::onWidgetEvent(Widget *const widget, NotificationType notificationType)
	{
		lastUpdatedWidget = widget;
		notifyObservers(this, notificationType);
	}

	void GUIManager::setupSkin(const std::string &skinFilename)
	{
		GUISkinService::instance()->setSkin(skinFilename);
	}

	Widget *GUIManager::getLastUpdatedWidget()
	{
		return lastUpdatedWidget;
	}

	void GUIManager::onKeyDown(unsigned int key)
	{
		if(key<260)
		{
			for(int i=widgets.size()-1;i>=0 && !stopPropagateEvents;--i)
			{
				if(widgets[i]->isVisible())
				{
					widgets[i]->onKeyDown(key);
				}
			}
			stopPropagateEvents=false;
		}
	}

	void GUIManager::onKeyUp(unsigned int key)
	{
		if(key<260)
		{
			for(int i=widgets.size()-1;i>=0 && !stopPropagateEvents;--i)
			{
				if(widgets[i]->isVisible())
				{
					widgets[i]->onKeyUp(key);
				}
			}
			stopPropagateEvents=false;
		}
	}

	void GUIManager::onChar(unsigned int character)
	{
		for(int i=widgets.size()-1;i>=0 && !stopPropagateEvents;--i)
		{
			if(widgets[i]->isVisible())
			{
				widgets[i]->onChar(character);
			}
		}
		stopPropagateEvents=false;
	}

	void GUIManager::onMouseMove(int mouseX, int mouseY)
	{
		for(int i=widgets.size()-1;i>=0 && !stopPropagateEvents;--i)
		{
			if(widgets[i]->isVisible())
			{
				widgets[i]->onMouseMove(mouseX, mouseY);
			}
		}
		stopPropagateEvents=false;
	}

	void GUIManager::addWidget(Widget *widget)
	{
		widgets.push_back(widget);

		widget->addObserver(this, Widget::SET_IN_FOREGROUND);
		onWidgetEvent(widget, GUIManager::ADD_WIDGET);
	}

	void GUIManager::removeWidget(Widget *widget)
	{
		std::vector<Widget *>::iterator it = std::find(widgets.begin(), widgets.end(), widget);
		delete widget;
		widgets.erase(it);

		onWidgetEvent(widget, GUIManager::REMOVE_WIDGET);
	}

	void GUIManager::display(float invFrameRate)
	{
		ShaderManager::instance()->bind(GUIShader);
		glUniform1i(diffuseTexSamplerLoc, 0);

		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glActiveTexture(GL_TEXTURE0);

		for(unsigned int i=0;i<widgets.size();++i)
		{
			if(widgets[i]->isVisible())
			{
				glUniform2iv(translateDistanceLoc, 1, (const int*)widgets[i]->getTranslateDistance());
				widgets[i]->display(translateDistanceLoc, invFrameRate);
			}
		}

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);

		#ifdef _DEBUG
			//display font texture
//			std::shared_ptr<ChunkXml> fontChunk = GUISkinService::instance()->getXmlSkin()->getChunk("text", "nameSkin", "defaultSkin");
//			std::shared_ptr<ChunkXml> fileFontChunk = GUISkinService::instance()->getXmlSkin()->getChunk("ttf", "", "", fontChunk.get());
//			std::shared_ptr<ChunkXml> sizeFontChunk = GUISkinService::instance()->getXmlSkin()->getChunk("size", "", "", fontChunk.get());
//			Font *font = MediaManager::instance()->getMedia<Font>(fileFontChunk->getStringValue(), (void*) sizeFontChunk->getIntValue());
//
//			DisplayTexture displayTexture(font->getTextureID(), DisplayTexture::DEFAULT_FACTOR);
//			displayTexture.setPosition(DisplayTexture::USER_DEFINED_X, DisplayTexture::USER_DEFINED_Y);
//			displayTexture.setSize(20.0, font->getDimensionTexture() + 20.0, 20.0, font->getDimensionTexture() + 20.0);
//			displayTexture.display(width, height);
//			font->release();
		#endif
	}

}
