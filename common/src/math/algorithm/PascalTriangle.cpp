#include "math/algorithm/PascalTriangle.h"

#include <cmath>

namespace urchin
{

	/**
	 * @param lineNumber Line number zero represents the first line of pascal triangle having one value equals to 1.
	 */
	std::vector<unsigned int> PascalTriangle::lineValues(unsigned int lineNumber)
	{
		unsigned int nbColumns = lineNumber+1;
		std::vector<unsigned int> lineValues(nbColumns);

		lineValues[0] = 1;
		for(unsigned int column=1; column<nbColumns; ++column)
		{
			lineValues[column] = std::round(lineValues[column-1] * (lineNumber - (column-1.0f))/static_cast<float>(column));
		}

		return lineValues;
	}

}
