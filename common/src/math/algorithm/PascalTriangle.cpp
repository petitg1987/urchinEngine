#include "math/algorithm/PascalTriangle.h"
#include "math/algorithm/MathFunction.h"

namespace urchin {

    /**
     * @param lineNumber Line number zero represents the first line of pascal triangle having one value equals to 1.
     */
    std::vector<unsigned int> PascalTriangle::lineValues(unsigned int lineNumber) {
        unsigned int nbColumns = lineNumber+1;
        std::vector<unsigned int> lineValues(nbColumns);

        lineValues[0] = 1;
        for (unsigned int column = 1; column < nbColumns; ++column) {
            auto columnFloat = (float)column;
            auto lineValue = (float)lineValues[column - 1] * (((float)lineNumber - (columnFloat - 1.0f)) / columnFloat);
            lineValues[column] = MathFunction::instance()->roundToUInt(lineValue);
        }

        return lineValues;
    }

}
