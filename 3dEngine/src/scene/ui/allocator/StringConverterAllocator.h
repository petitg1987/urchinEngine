#pragma once

namespace urchin {

    template<class T> struct MemorySlot {
        T* ptr;
        bool used;
    };

    template<class T> class StringConverterAllocator { //TODO remove me
        public:
            using value_type = T;

            StringConverterAllocator();
            StringConverterAllocator(const StringConverterAllocator<T>&);
            template<class U> explicit StringConverterAllocator(const StringConverterAllocator<U>&);
            StringConverterAllocator& operator=(const StringConverterAllocator&) = delete;
            ~StringConverterAllocator();

            T* allocate(std::size_t);
            void deallocate(T*, std::size_t);

            template<class U> friend bool operator== (const StringConverterAllocator<U>&, const StringConverterAllocator<U>&);
            template<class U> friend bool operator!= (const StringConverterAllocator<U>&, const StringConverterAllocator<U>&);

        private:
            static constexpr std::size_t ALLOCATED_SIZE = 400;
            std::array<MemorySlot<T>, 2> memorySlots;
            int* usageCount;
    };

    #include "StringConverterAllocator.inl"

    using WStringConvertA = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t, StringConverterAllocator<char32_t>, StringConverterAllocator<char>>;
    using U32StringA = std::basic_string<char32_t, std::char_traits<char32_t>, StringConverterAllocator<char32_t>>; //identical to std::u32string but with a custom allocator
}
