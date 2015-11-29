#include <sstream>
#include <algorithm>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "utils/shader/LoopUnrollerShader.h"

namespace urchin
{

	LoopUnrollerShader::LoopUnrollerShader()
	{

	}

	std::string LoopUnrollerShader::unrollLoops(const std::string &shaderSource)
	{
		std::string result = shaderSource;

		boost::regex startLoopRegex("#LOOP([0-9]*)_START\\(([0-9]*)\\)#");
		boost::match_results<std::string::const_iterator> loopWhat;
		boost::match_flag_type flags = boost::match_default;

		std::string::const_iterator start = result.begin();
		std::string::const_iterator end = result.end();
		while(regex_search(start, end, loopWhat, startLoopRegex, flags))
		{
			//loopWhat[1]: contain loop number
			unsigned int loopNumber;
			std::istringstream ssLoopNumber(loopWhat[1]);
			ssLoopNumber >> loopNumber;

			//loopWhat[2]: contain loop iteration number
			unsigned int loopIterationNumber;
			std::istringstream ssLoopIterationNumber(loopWhat[2]);
			ssLoopIterationNumber >> loopIterationNumber;

			//end of loop
			std::ostringstream endOfLoopString;
			endOfLoopString << "#LOOP" << loopNumber << "_END#";
			std::string::size_type endLoopPos = result.find(endOfLoopString.str());
			if(endLoopPos == std::string::npos)
			{
				throw std::runtime_error("End of loop number " + ssLoopNumber.str() + " not found.");
			}

			//unroll loop
			std::string::size_type startLoopPos = loopWhat[0].second - result.begin();
			std::string loopContent = result.substr(startLoopPos, endLoopPos - startLoopPos);
			std::string unrolledLoop = unrollLoop(loopContent, loopNumber, loopIterationNumber);

			//replace loop by unrolled loop
			std::string::size_type startLoopTokenPos = loopWhat[0].first - result.begin();
			std::string::size_type endLoopTokenPos = endLoopPos + endOfLoopString.str().length();
			result = result.replace(startLoopTokenPos, endLoopTokenPos - startLoopTokenPos, unrolledLoop);

			start = result.begin() + startLoopTokenPos;
			end = result.end();
		}

		return result;
	}

	std::string LoopUnrollerShader::unrollLoop(const std::string &loopContent, unsigned int loopNumber, unsigned int loopIterationNumber)
	{
		std::string result;

		std::string loopContentUpdated = loopContent;
		std::ostringstream loopStopString, loopNumberString, counterTokenString;

		//handle loop stop
		bool hasLoopStop = false;
		loopStopString << "#LOOP" << loopNumber << "_STOP#";
		if(loopContentUpdated.find(loopStopString.str()) != std::string::npos)
		{
			loopNumberString << loopNumber;
			result = "bool stopLoop" + loopNumberString.str() + " = false;\n";

			boost::replace_all(loopContentUpdated, loopStopString.str(), "stopLoop" + loopNumberString.str() + " = true;");

			hasLoopStop=true;
		}

		//unroll loop and replace loop counter token
		counterTokenString << "#LOOP" << loopNumber << "_COUNTER#";
		for(unsigned int index=0; index<loopIterationNumber; ++index)
		{
			//handle loop stop
			std::string loopIterationContent = "";
			if(hasLoopStop && index!=0)
			{
				loopIterationContent = "if(!stopLoop" + loopNumberString.str() + "){";
			}
			loopIterationContent = loopIterationContent + loopContentUpdated;

			//handle last iteration
			std::ostringstream ifLastIterationToken, elseLastIterationToken, endifLastIterationToken;
			ifLastIterationToken << "#LOOP" << loopNumber << "_IF_LAST_ITERATION";
			elseLastIterationToken << "#LOOP" << loopNumber << "_ELSE_LAST_ITERATION";
			endifLastIterationToken << "#LOOP" << loopNumber << "_ENDIF_LAST_ITERATION";
			if(loopIterationContent.find(ifLastIterationToken.str()) != std::string::npos)
			{ //last iteration token exist
				//content of conditional
				unsigned int startConditionalLastIterationContent = loopIterationContent.find(ifLastIterationToken.str());
				unsigned int endConditionalLastIterationContent =  loopIterationContent.find(endifLastIterationToken.str()) + endifLastIterationToken.str().length() + 1;

				//replace last iteration content
				if(index+1 >= loopIterationNumber)
				{
					//content of last iteration
					unsigned int startLastIterationContent = loopIterationContent.find(ifLastIterationToken.str()) + ifLastIterationToken.str().length() +1;
					unsigned int endLastIterationContent = -1;
					if(loopIterationContent.find(elseLastIterationToken.str()) != std::string::npos)
					{
						endLastIterationContent = loopIterationContent.find(elseLastIterationToken.str());
					}else
					{
						endLastIterationContent =loopIterationContent.find(endifLastIterationToken.str());
					}
					std::string lastIterationContent = loopIterationContent.substr(startLastIterationContent, endLastIterationContent - startLastIterationContent);

					loopIterationContent = loopIterationContent.replace(startConditionalLastIterationContent, endConditionalLastIterationContent - startConditionalLastIterationContent, lastIterationContent);
				}else
				{
					//content of not last iteration (else)
					std::string notLastIterationContent = "";
					if(loopIterationContent.find(elseLastIterationToken.str()) != std::string::npos)
					{
						unsigned int startNotLastIterationContent = loopIterationContent.find(elseLastIterationToken.str()) + elseLastIterationToken.str().length() + 1;
						unsigned int endNotLastIterationContent = loopIterationContent.find(endifLastIterationToken.str());
						notLastIterationContent = loopIterationContent.substr(startNotLastIterationContent, endNotLastIterationContent - startNotLastIterationContent);
					}

					loopIterationContent = loopIterationContent.replace(startConditionalLastIterationContent, endConditionalLastIterationContent - startConditionalLastIterationContent, notLastIterationContent);
				}
			}

			//replace loop counter by iteration number
			std::ostringstream iterationNumber;
			iterationNumber << index;
			boost::replace_all(loopIterationContent, counterTokenString.str(), iterationNumber.str());

			//add loop iteration content to result
			result = result + loopIterationContent;
		}

		//handle loop stop
		if(hasLoopStop)
		{
			for(unsigned int index=1; index<loopIterationNumber; ++index)
			{
				result = result + "\n}";
			}
		}

		return result;
	}

}
