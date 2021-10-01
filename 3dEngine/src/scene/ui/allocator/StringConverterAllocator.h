#pragma once

namespace urchin {

    template<class T> struct MemorySlot {
        std::shared_ptr<T> ptr;
        bool used;
    };

    template<class T> class StringConverterAllocator {
        public:
            using value_type = T;
            using char_type = T;

            StringConverterAllocator();
            template<class U> explicit StringConverterAllocator(const StringConverterAllocator<U>&);

            T* allocate(std::size_t);
            void deallocate(T*, std::size_t);

            template<class U> friend bool operator== (const StringConverterAllocator<U>&, const StringConverterAllocator<U>&);
            template<class U> friend bool operator!= (const StringConverterAllocator<U>&, const StringConverterAllocator<U>&);

        private:
            static constexpr std::size_t ALLOCATED_SIZE = 350;
            std::array<MemorySlot<T>, 2> memorySlots;
    };

    #include "StringConverterAllocator.inl"

    typedef std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t, StringConverterAllocator<char32_t>, StringConverterAllocator<char>> WStringConvertA;
    typedef std::basic_string<char32_t, std::char_traits<char32_t>, StringConverterAllocator<char32_t>> U32StringA; //identical to std::u32string but with a custom allocator
}
