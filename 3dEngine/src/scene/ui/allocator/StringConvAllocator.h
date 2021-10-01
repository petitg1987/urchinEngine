#pragma once

namespace urchin {

    template<class T> struct MemorySlot {
        std::shared_ptr<T> ptr;
        bool used;
    };

    template<class T> class StringConvAllocator {
        public:
            using value_type = T;
            using char_type = T;

            StringConvAllocator();
            template<class U> explicit StringConvAllocator(const StringConvAllocator<U>&);

            T* allocate(std::size_t);
            void deallocate(T*, std::size_t);

        private:
            static constexpr std::size_t ALLOCATED_SIZE = 300;
            std::array<MemorySlot<T>, 2> memorySlots;
    };

    #include "StringConvAllocator.inl"

    typedef std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t, StringConvAllocator<char32_t>, StringConvAllocator<char>> WStringConvertA;
    typedef std::basic_string<char32_t, std::char_traits<char32_t>, StringConvAllocator<char32_t>> U32StringA; //identical to std::u32string but with a custom allocator
}
