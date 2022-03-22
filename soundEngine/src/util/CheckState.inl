template <typename... Args> void CheckState::check(std::string_view action, const Args&... parameters) {
    static unsigned int numErrorsLogged = 0;
    ALenum err;
    while ((err = alGetError()) != AL_NO_ERROR && numErrorsLogged < MAX_ERRORS_LOG) {
        std::string parametersString;
        (parametersString.append(StringUtil::toString(parameters) + "; "), ...);
        Logger::instance().logError("OpenAL error detected: '" + std::to_string(err) + "' for action '" + std::string(action) + "' with parameters '" + parametersString + "'");

        numErrorsLogged++;
    }
}
