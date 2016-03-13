#include <GL/gl.h>
#include <sstream>
#include <memory>

#include "scene/GUI/widget/text/Text.h"
#include "scene/GUI/GUISkinService.h"
#include "resources/MediaManager.h"
#include "utils/display/quad/QuadDisplayerBuilder.h"

namespace urchin
{

	Text::Text(Position position, const std::string &nameSkin) :
		Widget(position, Size(0, Size::PIXEL, 0, Size::PIXEL)),
		text(""),
		maxLength(-1)
	{
		//skin informations
		std::shared_ptr<XmlChunk> fontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "text", XmlAttribute("nameSkin", nameSkin));
		std::shared_ptr<XmlChunk> fileFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "ttf", XmlAttribute(), fontChunk);
		std::shared_ptr<XmlChunk> sizeFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "size", XmlAttribute(), fontChunk);
		std::shared_ptr<XmlChunk> colorFontChunk = GUISkinService::instance()->getXmlSkin()->getUniqueChunk(true, "color", XmlAttribute(), fontChunk);

		//visual
		FontParameters textParameters;
		textParameters.fontSize = sizeFontChunk->getIntValue();
		textParameters.fontColor = colorFontChunk->getVector3Value();
		font = MediaManager::instance()->getMedia<Font>(fileFontChunk->getStringValue(), (void*)(&textParameters));

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
		std::vector<std::string> cuttedTextLines;
		if(maxLength>0)
		{
			//cuts the text all the "maxLength" pixels
			std::string cuttedText = cutText(text, maxLength);

			//splits the text at each newline
			std::stringstream cuttedTextStream(cuttedText);
			std::string item;
			while (std::getline(cuttedTextStream, item, '\n')) {
				cuttedTextLines.push_back(item);
			}
		}else
		{
			//splits the text at each newline
			std::stringstream cuttedTextStream(text);
			std::string item;
			while (std::getline(cuttedTextStream, item, '\n')) {
				cuttedTextLines.push_back(item);
			}
		}

		unsigned int numberOfInterLines = static_cast<unsigned int>(std::max(0, ((int)cuttedTextLines.size()-1)));
		setHeight(cuttedTextLines.size()*font->getHeight() + numberOfInterLines*font->getSpaceBetweenLines(), Size::SizeType::PIXEL);
		setWidth(0, Size::SizeType::PIXEL);
		int numberLetter = 0;

		//creates the vertex array and texture array
		std::vector<int> vertexArray;
		std::vector<float> stArray;
		int offsetY = 0;
		for(unsigned int numLine=0; numLine<cuttedTextLines.size(); ++numLine)
		{ //each lines
			unsigned int offsetX = 0;
			for(unsigned int numLetter=0; numLetter<cuttedTextLines[numLine].size(); ++numLetter)
			{ //each letters
				unsigned char letter = cuttedTextLines[numLine][numLetter];
				numberLetter++;

				float t = (float)((letter)>>4)/16.0;
				float s = (float)((letter)%16)/16.0;

				vertexArray.push_back(offsetX);
				vertexArray.push_back(-font->getGlyph(letter).shift + offsetY);
				vertexArray.push_back(font->getGlyph(letter).width + offsetX);
				vertexArray.push_back(-font->getGlyph(letter).shift + offsetY);
				vertexArray.push_back(font->getGlyph(letter).width + offsetX);
				vertexArray.push_back(font->getGlyph(letter).height - font->getGlyph(letter).shift + offsetY);
				vertexArray.push_back(offsetX);
				vertexArray.push_back(font->getGlyph(letter).height - font->getGlyph(letter).shift + offsetY);

				stArray.push_back(s);
				stArray.push_back(t);
				stArray.push_back(s+((float)font->getGlyph(letter).width/font->getDimensionTexture()));
				stArray.push_back(t);
				stArray.push_back(s+((float)font->getGlyph(letter).width/font->getDimensionTexture()));
				stArray.push_back(t+((float)font->getGlyph(letter).height/font->getDimensionTexture()));
				stArray.push_back(s);
				stArray.push_back(t+((float)font->getGlyph(letter).height/font->getDimensionTexture()));

				offsetX += font->getGlyph(letter).width + font->getSpaceBetweenLetters();

				if(getWidth() < (offsetX - font->getSpaceBetweenLetters()))
				{
					setWidth(offsetX - font->getSpaceBetweenLetters(), Size::SizeType::PIXEL);
				}
			}
			offsetY += font->getSpaceBetweenLines();
		}

		int *vertexData = new int[numberLetter*8];
		std::copy(vertexArray.begin(), vertexArray.end(), vertexData);

		float *textureData = new float[numberLetter*8];
		std::copy(stArray.begin(), stArray.end(), textureData);

		quadDisplayer = std::make_unique<QuadDisplayerBuilder>()
				->numberOfQuad(numberLetter)
				->vertexData(GL_INT, vertexData)
				->textureData(GL_FLOAT, textureData)
				->build();
	}

	std::string Text::cutText(const std::string &constText, int maxLength)
	{
		std::string text(constText);

		int lineLength = 0;
		int indexLastSpace = 0;
		int lengthFromLastSpace = 0;

		for(int numLetter=0; text[numLetter]!=0; numLetter++)
		{ //each letters
			unsigned char letter = text[numLetter];

			if(letter=='\n')
			{
				lineLength = 0;
				lengthFromLastSpace = 0;
			}else	if(letter==' ')
			{
				indexLastSpace = numLetter;
				lengthFromLastSpace = 0;
			}

			int lengthLetter = font->getGlyph(letter).width + font->getSpaceBetweenLetters();
			if(lineLength+lengthLetter >= maxLength)
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

	const Font *Text::getFont()
	{
		return font;
	}

	void Text::display(int translateDistanceLoc, float invFrameRate)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, font->getTextureID());

		quadDisplayer->display();
		
		glDisable(GL_BLEND);

		//displays children
		Widget::display(translateDistanceLoc, invFrameRate);
	}

}
