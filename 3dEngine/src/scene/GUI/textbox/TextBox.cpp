#include <GL/glew.h>
#include <GL/gl.h>
#include "UrchinCommon.h"

#include "scene/GUI/textbox/TextBox.h"
#include "scene/SceneManager.h"

#define ADDITIONAL_LEFT_BORDER 1 //Additional border to outline->leftWidth
#define LETTER_SHIFT 5 //When the text box is full of text, we shift all letters to left
#define LETTER_AND_CURSOR_SHIFT 2 //Define space between the letters and cursor

namespace urchin
{

	TextBox::TextBox(int positionX, int positionY, int width, int height, const std::string &nameSkin) :
		Widget(positionX, positionY, width, height), startTextIndex(0), cursorIndex(0), state(UNACTIVE), widgetOutline(new WidgetOutline())
	{
		//skin information
		std::shared_ptr<XmlChunk> textBoxChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textBox", XmlAttribute("nameSkin", nameSkin));

		std::shared_ptr<XmlChunk> skinChunkDefault = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "default"), textBoxChunk);
		texTextBoxDefault = GUISkinService::instance()->createTexWidget(width, height, skinChunkDefault, widgetOutline);
		
		std::shared_ptr<XmlChunk> skinChunkFocus = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "skin", XmlAttribute("type", "focus"), textBoxChunk);
		texTextBoxFocus = GUISkinService::instance()->createTexWidget(width, height, skinChunkFocus);
		
		std::shared_ptr<XmlChunk> textChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "textSkin", XmlAttribute(), textBoxChunk);
		text = new Text(0, 0,  textChunk->getStringValue());
		addChild(text);
		text->setPosition(widgetOutline->leftWidth + ADDITIONAL_LEFT_BORDER, (int)(height - text->getHeight())/2);
		maxWidthText = width - (widgetOutline->leftWidth + widgetOutline->rightWidth + ADDITIONAL_LEFT_BORDER);
		
		//cursor information
		cursorBlinkSpeed = ConfigService::instance()->getFloatValue("GUI.cursorBlinkSpeed");

		//visual
		glGenBuffers(2, bufferIDs);
		glGenVertexArrays(1, &vertexArrayObject);
		glBindVertexArray(vertexArrayObject);

		const int vertexArray[] = {0, 0, width, 0, width, height, 0, height, 0, widgetOutline->topWidth, 0, height - widgetOutline->bottomWidth};
		const float stArray[] = {0.0, 0.0, texTextBoxDefault->getMaxCoordS(), 0.0, texTextBoxDefault->getMaxCoordS(), texTextBoxDefault->getMaxCoordT(), 0.0, texTextBoxDefault->getMaxCoordT()};

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_VERTEX_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_VERTEX_POSITION);
		glVertexAttribPointer(SHADER_VERTEX_POSITION, 2, GL_INT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[VAO_TEX_COORD]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(stArray), stArray, GL_STATIC_DRAW);
		glEnableVertexAttribArray(SHADER_TEX_COORD);
		glVertexAttribPointer(SHADER_TEX_COORD, 2, GL_FLOAT, false, 0, 0);

		textureID = texTextBoxDefault->getTextureID();
		computeCursorPosition();
	}

	TextBox::~TextBox()
	{
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(2, bufferIDs);

		delete widgetOutline;
		texTextBoxDefault->release();
		texTextBoxFocus->release();
	}

	std::string TextBox::getText()
	{
		return allText;
	}

	void TextBox::onKeyDown(unsigned int key)
	{
		if(key==KEY_MOUSE_LEFT)
		{
			Rectangle<int> widgetRectangle(Point2<int>(getGlobalPositionX(), getGlobalPositionY()), Point2<int>(getGlobalPositionX()+width, getGlobalPositionY()+height));
			if (widgetRectangle.collideWithPoint(Point2<int>(mouseX, mouseY)))
			{
				state = ACTIVE;
				notifyObservers(this, KEYBOARD_LOCKED);
				textureID = texTextBoxFocus->getTextureID();

				int localMouseX = mouseX - text->getGlobalPositionX();
				computeCursorIndex(localMouseX);
			}else
			{
				state = UNACTIVE;
				notifyObservers(this, KEYBOARD_UNLOCKED);
				textureID = texTextBoxDefault->getTextureID();
			}
		}else if(key==KEY_LEFT_ARROW && state==ACTIVE)
		{
			refreshText(cursorIndex-1);
		}else if(key==KEY_RIGHT_ARROW && state==ACTIVE)
		{
			refreshText(cursorIndex+1);
		}

		Widget::onKeyDown(key);
	}

	void TextBox::onChar(unsigned int character)
	{
		if(state==ACTIVE)
		{
			if(character==8 && cursorIndex>0)
			{ //key backspace
				std::string tmpRight = allText.substr(cursorIndex, allText.length()-cursorIndex);
				allText = allText.substr(0, cursorIndex-1);
				allText.append(tmpRight);

				refreshText(cursorIndex-1);
			}else if(character==127 && allText.length()>0 && cursorIndex<(int)allText.length())
			{ //key delete
				std::string tmpRight = allText.substr(cursorIndex+1, allText.length()-cursorIndex);
				allText = allText.substr(0, cursorIndex);
				allText.append(tmpRight);

				refreshText(cursorIndex);
			}else if(character<256 && character>30 && character!=127)
			{
				std::string tmpRight = allText.substr(cursorIndex, allText.length()-cursorIndex);
				allText = allText.substr(0, cursorIndex);
				allText.append(1, (char)character);
				allText.append(tmpRight);

				refreshText(cursorIndex+1);
			}
		}
		
		Widget::onChar(character);
	}

	void TextBox::reset()
	{
		state = UNACTIVE;
		notifyObservers(this, KEYBOARD_UNLOCKED);
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
			startTextIndex = (startTextIndex<=(int)allText.length()) ? startTextIndex+LETTER_SHIFT : allText.length();
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
			char letter = allText[endTextIndex];
			widthText += font->getGlyph(letter).width + font->getSpaceBetweenLetters();
			if(widthText>maxWidthText)
			{
				break;
			}
		}
		text->setText(allText.substr(startTextIndex, endTextIndex-startTextIndex));
	}

	void TextBox::computeCursorPosition()
	{
		const Font *font = text->getFont();
		cursorPosition = ADDITIONAL_LEFT_BORDER;

		for(int i=startTextIndex;i<cursorIndex;++i)
		{
			char letter = allText[i];
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
			char letter = allText[cursorIndex];
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

		glBindVertexArray(vertexArrayObject);
		glDrawArrays(GL_QUADS, 0, 4);

		//displays the cursor
		static float blink=0.0;
		blink+=invFrameRate*cursorBlinkSpeed;
		if(state==ACTIVE && ((int)blink%2)>0)
		{
			glUniform2iv(translateDistanceLoc, 1, (const int*)(getTranslateDistance() + Vector2<int>(cursorPosition, 0)));
			glBindTexture(GL_TEXTURE_2D, 0);

			glDrawArrays(GL_LINES, 4, 2);
			glUniform2iv(translateDistanceLoc, 1, (const int*)getTranslateDistance());
		}

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
