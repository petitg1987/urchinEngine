template<class T> Loader<T>::~Loader()
{

}

template<class T> T *Loader<T>::loadFromFile(const std::string &filename)
{
    throw std::runtime_error("Impossible to import this type of file, filename: " + filename + ".");
}

template<class T> void Loader<T>::saveToFile(const T *, const std::string& filename)
{
    throw std::runtime_error("Impossible to export this type of file, filename: " + filename + ".");
}
