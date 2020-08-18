template<class T> T* ResourceManager::getResource(const std::string &name) const
{
    std::map<std::string, Resource*>::const_iterator it = mResources.find(name);
    if(it!=mResources.end())
    {
        it->second->addRef();
        return static_cast<T*>(it->second);
    }

    return nullptr;
}
