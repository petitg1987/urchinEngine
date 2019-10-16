#include <GL/glew.h>
#include <memory>

#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/widget/Size.h"
#include "resources/MediaManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	Text::Text(Position position, const std::string &fontFilename) :
		Widget(position, Size(0, 0, Size::PIXEL)),
		text(""),
		maxLength(-1)
	{
		font = MediaManager::instance()->getMedia<Font>(fontFilename);
		quadDisplayerBuilder = std::make_unique<QuadDisplayerBuilder>();
		quadDisplayer = quadDisplayerBuilder->numberOfQuad(0)->build();

		setText(text);
	}

	Text::~Text()
	{
		font->release();
	}

	void Text::createOrUpdateWidget()
	{
		//nothing to do: text cannot be defined in percentage (Size::SizeType::PERCENTAGE).
	}

	void Text::setText(const std::string &text, int maxLength)
	{
		this->text = text;
		this->maxLength = maxLength;

		//cut the text if needed
		unsigned int numLetters = 0;
		std::stringstream cuttedTextStream((maxLength>0) ? cutText(text, maxLength) : text);
		std::string item;
		cutTextLines.clear();
		while (std::getline(cuttedTextStream, item, '\n'))
		{
			cutTextLines.push_back(item);
			numLetters += item.size();
		}

		//creates the vertex array and texture array
		vertexData.resize(numLetters*8);
		textureData.resize(numLetters*8);
		unsigned int vertexIndex = 0, stIndex = 0;
		unsigned int width = 0;
		unsigned int offsetY = 0;
		for (auto &cutTextLine : cutTextLines)
		{ //each lines
			unsigned int offsetX = 0;
			for (char charLetter : cutTextLine)
			{ //each letters
				auto letter = static_cast<unsigned char>(charLetter);

				float t = (float)(letter >> 4u) / 16.0f;
				float s = (float)(letter % 16) / 16.0f;

				vertexData[vertexIndex++] = offsetX;
				vertexData[vertexIndex++] = -font->getGlyph(letter).shift + offsetY;
				vertexData[vertexIndex++] = font->getGlyph(letter).width + offsetX;
				vertexData[vertexIndex++] = -font->getGlyph(letter).shift + offsetY;
				vertexData[vertexIndex++] = font->getGlyph(letter).width + offsetX;
				vertexData[vertexIndex++] = font->getGlyph(letter).height - font->getGlyph(letter).shift + offsetY;
				vertexData[vertexIndex++] = offsetX;
				vertexData[vertexIndex++] = font->getGlyph(letter).height - font->getGlyph(letter).shift + offsetY;

				textureData[stIndex++] = s;
				textureData[stIndex++] = t;
				textureData[stIndex++] = s+((float)font->getGlyph(letter).width / (float)font->getDimensionTexture());
				textureData[stIndex++] = t;
				textureData[stIndex++] = s+((float)font->getGlyph(letter).width / (float)font->getDimensionTexture());
				textureData[stIndex++] = t+((float)font->getGlyph(letter).height / (float)font->getDimensionTexture());
				textureData[stIndex++] = s;
				textureData[stIndex++] = t+((float)font->getGlyph(letter).height / (float)font->getDimensionTexture());

				offsetX += font->getGlyph(letter).width + font->getSpaceBetweenLetters();

				width = std::max(width, offsetX - font->getSpaceBetweenLetters());
			}
			offsetY += font->getSpaceBetweenLines();
		}

        if(cutTextLines.empty())
        { //add fake line to compute height
            cutTextLines.emplace_back("");
        }
		unsigned int numberOfInterLines = cutTextLines.size() - 1;
		setSize(Size((float)width, (float)(cutTextLines.size() * font->getHeight() + numberOfInterLines * font->getSpaceBetweenLines()), Size::SizeType::PIXEL));

		quadDisplayerBuilder
				->numberOfQuad(numLetters)
				->vertexData(GL_INT, &vertexData[0], false)
				->textureData(GL_FLOAT, &textureData[0], false);
		quadDisplayer->update(quadDisplayerBuilder.get());
	}

	std::string Text::cutText(const std::string &constText, int maxLength)
	{
		std::string text(constText);

		unsigned int lineLength = 0;
		unsigned int indexLastSpace = 0;
		unsigned int lengthFromLastSpace = 0;

		for(unsigned int numLetter=0; text[numLetter]!=0; numLetter++)
		{ //each letters
			auto letter = static_cast<unsigned char>(text[numLetter]);

			if(letter=='\n')
			{
				lineLength = 0;
				lengthFromLastSpace = 0;
			}else if(letter==' ')
			{
				indexLastSpace = numLetter;
				lengthFromLastSpace = 0;
			}

			unsigned int lengthLetter = font->getGlyph(letter).width + font->getSpaceBetweenLetters();
			if(lineLength + lengthLetter >= maxLength)
			{ //cut line
				text[indexLastSpace] = '\n';
				lineLength = lengthFromLastSpace;
			}else
			{
				lineLength += lengthLetter;
				lengthFromLastSpace += lengthLetter;
			}
		}

		return text;
	}

	const std::string &Text::getText() const
	{
		return text;
	}

	const Font *Text::getFont()
	{
		return font;
	}

	void Text::display(int translateDistanceLoc, float dt)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, font->getTextureID());

		quadDisplayer->display();
		
		glDisable(GL_BLEND);

		Widget::display(translateDistanceLoc, dt);
	}

}
