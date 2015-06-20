#ifndef ENGINE_SHADERMANAGER_H
#define ENGINE_SHADERMANAGER_H

#include <string>
#include <list>
#include <map>
#include "UrchinCommon.h"

#include "utils/shader/TokenReplacerShader.h"
#include "utils/shader/LoopUnrollerShader.h"

namespace urchin
{

	class ShaderManager : public Singleton<ShaderManager>
	{
		public:
			friend class Singleton<ShaderManager>;

			void replaceShadersWorkingDirectoryBy(const std::string &);

			unsigned int createProgram(const std::string &, const std::string &);
			unsigned int createProgram(const std::string &, const std::string &, const std::map<TokenReplacerShader::ShaderToken, std::string> &);
			void removeProgram(unsigned int);

			void setGeometryShader(unsigned int, const std::string &);
			void setGeometryShader(unsigned int, const std::string &, const std::map<TokenReplacerShader::ShaderToken, std::string> &);

			void bind(unsigned int);

			unsigned int getCurrentProgram() const;

		private:
			ShaderManager();
			virtual ~ShaderManager();

			std::string readEntireFile(const std::string &);
			void clearProgram(unsigned int);

			void checkShader(unsigned int, const std::string &);
			void checkProgram(unsigned int, const std::string &);

			std::string shadersWorkingDirectory;
			std::string shadersLocation;

			TokenReplacerShader tokenReplacerShader;
			LoopUnrollerShader loopUnrollerShader;
			std::list<unsigned int> programs;
			unsigned int currentProgramID;
	};

}

#endif
