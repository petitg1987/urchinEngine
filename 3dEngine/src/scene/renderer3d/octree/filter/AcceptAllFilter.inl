template<class TOctreeable> AcceptAllFilter<TOctreeable>::AcceptAllFilter()
{

}

template<class TOctreeable> AcceptAllFilter<TOctreeable>::~AcceptAllFilter()
{

}

template<class TOctreeable> inline bool AcceptAllFilter<TOctreeable>::isAccepted(const TOctreeable *const octreeable) const
{
	return true;
}
