#include <GL/glew.h>
#include "UrchinCommon.h"

#include "scene/GUI/widget/textbox/TextBox.h"
#include "scene/InputDevice.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

#define ADDITIONAL_LEFT_BORDER 1 //Additional border to outline->leftWidth
#define LETTER_SHIFT 5 //When the text box is full of text, we shift all letters to left
#define LETTER_AND_CURSOR_SHIFT 2 //Define space between the letters and cursor
#define CURSOR_BLINK_SPEED 1.75

namespace urchin
{

	TextBox::TextBox(Position position, Size size, const std::string &nameSkin) :
		Widget(position, size),
		nameSkin(nameSkin),
		text(nullptr),
		startTextIndex(0),
		cursorIndex(0),
		cursorBlink(0.0f),
		state(UNACTIVE),
		widgetOutline(new WidgetOutline())
	{
		glGenBuffers(1, &cursorLineBufferID);
		glGenVertexArrays(1, &cursorLineVAO);

		createOrUpdateWidget();
	}

	TextBox::~TextBox()
	{
		glDeleteVertexArrays(1, &cursorLineVAO);
		glDeleteBuffers(1, &cursorLineBufferID);

		delete widgetOutline;
	}

	void TextBox::createOrUpdateWidget()
	{
		//skin information
		std::shared_ptr<XmlChunk> textBoxChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textBox", XmlAttribute("nameSkin", nameSkin));

		std::shared_ptr<XmlChunk> skinChunkDefault = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), textBoxChunk);
		texTextBoxDefault = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinChunkDefault, widgetOutline);
		
		std::shared_ptr<XmlChunk> skinChunkFocus = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), textBoxChunk);
		texTextBoxFocus = GUISkinService::instance()->createTexWidget(getWidth(), getHeight(), skinChunkFocus);
		
		std::shared_ptr<XmlChunk> textChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), textBoxChunk);
		removeChild(text);
		text = new Text(Position(0, 0, Position::PIXEL),  textChunk->getStringValue());
		text->setPosition(Position(widgetOutline->leftWidth + ADDITIONAL_LEFT_BORDER, (int)(getHeight() - text->getHeight())/2, Position::PIXEL));;
		addChild(text);
		maxWidthText = getWidth() - (widgetOutline->leftWidth + widgetOutline->rightWidth + ADDITIONAL_LEFT_BORDER);
		refreshText(cursorIndex);

		//visual
		glBindVertexArray(cursorLineVAO);

		const unsigned int cursorLineVertexData[] = {0, widgetOutline->topWidth, 0, getHeight() - widgetOutline->bottomWidth};
		glBindBuffer(GL_ARRAY_BUFFER, cursorLineBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cursorLineVertexData), cursorLineVertexData, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 2, GL_UNSIGNED_INT, GL_FALSE, 0, nullptr);

		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->vertexData(GL_UNSIGNED_INT, new unsigned int[8]{0, 0, getWidth(), 0, getWidth(), getHeight(), 0, getHeight()})
				->textureData(GL_FLOAT, new float[8]{0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0})
				->build();

		textureID = texTextBoxDefault->getTextureID();
		computeCursorPosition();
	}

	std::string TextBox::getText()
	{
		return allText;
	}

	bool TextBox::onKeyDownEvent(unsigned int key)
	{
		if(key==InputDevice::Key::MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+getWidth(), getGlobalPositionY()+getHeight()));
			if (widgetRectangle.collideWithPoint(Point2<int>(getMouseX(), getMouseY())))
			{
				state = ACTIVE;
				textureID = texTextBoxFocus->getTextureID();

				int localMouseX = getMouseX() - text->getGlobalPositionX();
				computeCursorIndex(localMouseX);
			}else
			{
				state = UNACTIVE;
				textureID = texTextBoxDefault->getTextureID();
			}
		}else if(key==InputDevice::Key::LEFT_ARROW && state==ACTIVE)
		{
			refreshText(cursorIndex-1);
		}else if(key==InputDevice::Key::RIGHT_ARROW && state==ACTIVE)
		{
			refreshText(cursorIndex+1);
		}

		return true;
	}

	bool TextBox::onCharEvent(unsigned int character)
	{
		if(state==ACTIVE)
		{
			if(character==8 && cursorIndex>0)
			{ //key backspace
				std::string tmpRight = allText.substr(static_cast<unsigned long>(cursorIndex), allText.length()-cursorIndex);
				allText = allText.substr(0, static_cast<unsigned long>(cursorIndex-1L));
				allText.append(tmpRight);

				refreshText(cursorIndex-1);
			}else if(character==127 && allText.length()>0 && cursorIndex<(int)allText.length())
			{ //key delete
				std::string tmpRight = allText.substr(static_cast<unsigned long>(cursorIndex+1L), allText.length()-cursorIndex);
				allText = allText.substr(0, static_cast<unsigned long>(cursorIndex));
				allText.append(tmpRight);

				refreshText(cursorIndex);
			}else if(character<256 && character>30 && character!=127)
			{
				std::string tmpRight = allText.substr(static_cast<unsigned long>(cursorIndex), allText.length()-cursorIndex);
				allText = allText.substr(0, static_cast<unsigned long>(cursorIndex));
				allText.append(1, (char)character);
				allText.append(tmpRight);

				refreshText(cursorIndex+1);
			}

			return false;
		}
		
		return true;
	}

	void TextBox::reset()
	{
		state = UNACTIVE;
		textureID = texTextBoxDefault->getTextureID();

		Widget::reset();
	}

	void TextBox::refreshText(int newCursorIndex)
	{
		//refresh cursor index
		if(	(newCursorIndex > cursorIndex && cursorIndex < (int)allText.length()) ||
				(newCursorIndex < cursorIndex && cursorIndex!=0))
		{
			cursorIndex = newCursorIndex;
		}

		//refresh start index
		computeCursorPosition();
		if(cursorPosition > maxWidthText)
		{
			startTextIndex = (startTextIndex<=(int)allText.length()) ? startTextIndex+LETTER_SHIFT : (int)allText.length();
		}else if(cursorIndex <= startTextIndex)
		{
			startTextIndex = (startTextIndex>0) ? startTextIndex-LETTER_SHIFT : 0;
		}
		computeCursorPosition();

		//determine the text to display
		const Font *font = text->getFont();
		int widthText=0, endTextIndex;
		for(endTextIndex=startTextIndex; endTextIndex<(int)allText.length(); ++endTextIndex)
		{
			auto letter = static_cast<unsigned char>(allText[endTextIndex]);
			widthText += font->getGlyph(letter).width + font->getSpaceBetweenLetters();
			if(widthText>maxWidthText)
			{
				break;
			}
		}
		text->setText(allText.substr(static_cast<unsigned long>(startTextIndex), static_cast<unsigned long>(endTextIndex-startTextIndex)));
	}

	void TextBox::computeCursorPosition()
	{
		const Font *font = text->getFont();
		cursorPosition = ADDITIONAL_LEFT_BORDER;

		for(int i=startTextIndex;i<cursorIndex;++i)
		{
			auto letter = static_cast<unsigned char>(allText[i]);
			cursorPosition += font->getGlyph(letter).width + font->getSpaceBetweenLetters();
		}

		if(cursorPosition > ADDITIONAL_LEFT_BORDER)
		{
			cursorPosition -= font->getSpaceBetweenLetters(); //remove last space
			cursorPosition += LETTER_AND_CURSOR_SHIFT;
		}

		cursorPosition += widgetOutline->leftWidth;
	}

	void TextBox::computeCursorIndex(int approximateCursorPosition)
	{
		const Font *font = text->getFont();
		float widthText=0.0;

		for(cursorIndex=startTextIndex; cursorIndex<(int)allText.length(); ++cursorIndex)
		{
			auto letter = static_cast<unsigned char>(allText[cursorIndex]);
			widthText += (float)font->getGlyph(letter).width/2.0;
			if(widthText > approximateCursorPosition)
			{
				break;
			}

			widthText += (float)font->getGlyph(letter).width/2.0 + (float)font->getSpaceBetweenLetters();
		}

		//compute the correct cursor position
		computeCursorPosition();
	}

	void TextBox::display(int translateDistanceLoc, float invFrameRate)
	{
		//display the text box
		glBindTexture(GL_TEXTURE_2D, textureID);

		quadDisplayer->display();

		//displays the cursor
		cursorBlink+=invFrameRate*CURSOR_BLINK_SPEED;
		if(state==ACTIVE && ((int)cursorBlink%2)>0)
		{
			Vector2<int> widgetPosition(getGlobalPositionX(), getGlobalPositionY());
			glUniform2iv(translateDistanceLoc, 1, (const int*)(widgetPosition + Vector2<int>(cursorPosition, 0)));
			glBindTexture(GL_TEXTURE_2D, 0);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			glBindVertexArray(cursorLineVAO);
			glDrawArrays(GL_LINES, 0, 2);

			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);

			glUniform2iv(translateDistanceLoc, 1, (const int*)widgetPosition);
		}

		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
