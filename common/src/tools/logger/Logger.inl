template<class T> Logger& Logger::operator <<(const T& toLog)
{
	std::ostringstream stream;
	stream << toLog;
	write(stream.str());
	return logger();
}
