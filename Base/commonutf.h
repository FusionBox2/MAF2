#ifndef COMMONTYPESUTF_H
#define COMMONTYPESUTF_H

#include <stdint.h>
#include <iterator>
#include <string>

//the code taken from boost locale 
namespace utf {

#define UTF_LIKELY(x)   (x)
#define UTF_UNLIKELY(x) (x)

///
/// \brief The integral type that can hold a Unicode code point
///
    typedef uint32_t code_point;

    ///
    /// \brief Special constant that defines illegal code point
    ///
    static const code_point illegal = 0xFFFFFFFFu;

    ///
    /// \brief Special constant that defines incomplete code point
    ///
    static const code_point incomplete = 0xFFFFFFFEu;

    ///
    /// \brief the function checks if \a v is a valid code point
    ///
    inline bool is_valid_codepoint(code_point v)
    {
        if (v > 0x10FFFF)
            return false;
        if (0xD800 <= v && v <= 0xDFFF) // surragates
            return false;
        return true;
    }

    template<typename CharType, int size = sizeof(CharType)>
    struct utf_traits;

    template<typename CharType>
    struct utf_traits<CharType, 1> {

        typedef CharType char_type;

        static int trail_length(char_type ci)
        {
            unsigned char c = ci;
            if (c < 128)
                return 0;
            if (UTF_UNLIKELY(c < 194))
                return -1;
            if (c < 224)
                return 1;
            if (c < 240)
                return 2;
            if (UTF_LIKELY(c <= 244))
                return 3;
            return -1;
        }

        static const int max_width = 4;

        static int width(code_point value)
        {
            if (value <= 0x7F) {
                return 1;
            }
            else if (value <= 0x7FF) {
                return 2;
            }
            else if (UTF_LIKELY(value <= 0xFFFF)) {
                return 3;
            }
            else {
                return 4;
            }
        }

        static bool is_trail(char_type ci)
        {
            unsigned char c = ci;
            return (c & 0xC0) == 0x80;
        }

        static bool is_lead(char_type ci)
        {
            return !is_trail(ci);
        }

        template<typename It>
        static code_point decode(It& p, It e)
        {
            if (UTF_UNLIKELY(p == e))
                return incomplete;

            unsigned char lead = *p++;

            // First byte is fully validated here
            int trail_size = trail_length(lead);

            if (UTF_UNLIKELY(trail_size < 0))
                return illegal;

            //
            // Ok as only ASCII may be of size = 0
            // also optimize for ASCII text
            //
            if (trail_size == 0)
                return lead;

            code_point c = lead & ((1 << (6 - trail_size)) - 1);

            // Read the rest
            unsigned char tmp;
            switch (trail_size) {
            case 3:
                if (UTF_UNLIKELY(p == e))
                    return incomplete;
                tmp = *p++;
                if (!is_trail(tmp))
                    return illegal;
                c = (c << 6) | (tmp & 0x3F);
                [[fallthrough]]; // fallthrough is explicit
            case 2:
                if (UTF_UNLIKELY(p == e))
                    return incomplete;
                tmp = *p++;
                if (!is_trail(tmp))
                    return illegal;
                c = (c << 6) | (tmp & 0x3F);
                [[fallthrough]]; // fallthrough is explicit
            case 1:
                if (UTF_UNLIKELY(p == e))
                    return incomplete;
                tmp = *p++;
                if (!is_trail(tmp))
                    return illegal;
                c = (c << 6) | (tmp & 0x3F);
            }

            // Check code point validity: no surrogates and
            // valid range
            if (UTF_UNLIKELY(!is_valid_codepoint(c)))
                return illegal;

            // make sure it is the most compact representation
            if (UTF_UNLIKELY(width(c) != trail_size + 1))
                return illegal;

            return c;

        }

        template<typename It>
        static code_point decode_valid(It& p)
        {
            unsigned char lead = *p++;
            if (lead < 192)
                return lead;

            int trail_size;

            if (lead < 224)
                trail_size = 1;
            else if (UTF_LIKELY(lead < 240)) // non-BMP rare
                trail_size = 2;
            else
                trail_size = 3;

            code_point c = lead & ((1 << (6 - trail_size)) - 1);

            switch (trail_size) {
            case 3:
                c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
            case 2:
                c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
            case 1:
                c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
            }

            return c;
        }



        template<typename It>
        static It encode(code_point value, It out)
        {
            if (value <= 0x7F) {
                *out++ = static_cast<char_type>(value);
            }
            else if (value <= 0x7FF) {
                *out++ = static_cast<char_type>((value >> 6) | 0xC0);
                *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
            }
            else if (UTF_LIKELY(value <= 0xFFFF)) {
                *out++ = static_cast<char_type>((value >> 12) | 0xE0);
                *out++ = static_cast<char_type>(((value >> 6) & 0x3F) | 0x80);
                *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
            }
            else {
                *out++ = static_cast<char_type>((value >> 18) | 0xF0);
                *out++ = static_cast<char_type>(((value >> 12) & 0x3F) | 0x80);
                *out++ = static_cast<char_type>(((value >> 6) & 0x3F) | 0x80);
                *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
            }
            return out;
        }
    }; // utf8

    template<typename CharType>
    struct utf_traits<CharType, 2> {
        typedef CharType char_type;

        // See RFC 2781
        static bool is_first_surrogate(uint16_t x)
        {
            return 0xD800 <= x && x <= 0xDBFF;
        }
        static bool is_second_surrogate(uint16_t x)
        {
            return 0xDC00 <= x && x <= 0xDFFF;
        }
        static code_point combine_surrogate(uint16_t w1, uint16_t w2)
        {
            return ((code_point(w1 & 0x3FF) << 10) | (w2 & 0x3FF)) + 0x10000;
        }
        static int trail_length(char_type c)
        {
            if (is_first_surrogate(c))
                return 1;
            if (is_second_surrogate(c))
                return -1;
            return 0;
        }
        ///
        /// Returns true if c is trail code unit, always false for UTF-32
        ///
        static bool is_trail(char_type c)
        {
            return is_second_surrogate(c);
        }
        ///
        /// Returns true if c is lead code unit, always true of UTF-32
        ///
        static bool is_lead(char_type c)
        {
            return !is_second_surrogate(c);
        }

        template<typename It>
        static code_point decode(It& current, It last)
        {
            if (UTF_UNLIKELY(current == last))
                return incomplete;
            uint16_t w1 = *current++;
            if (UTF_LIKELY(w1 < 0xD800 || 0xDFFF < w1)) {
                return w1;
            }
            if (w1 > 0xDBFF)
                return illegal;
            if (current == last)
                return incomplete;
            uint16_t w2 = *current++;
            if (w2 < 0xDC00 || 0xDFFF < w2)
                return illegal;
            return combine_surrogate(w1, w2);
        }
        template<typename It>
        static code_point decode_valid(It& current)
        {
            uint16_t w1 = *current++;
            if (UTF_LIKELY(w1 < 0xD800 || 0xDFFF < w1)) {
                return w1;
            }
            uint16_t w2 = *current++;
            return combine_surrogate(w1, w2);
        }

        static const int max_width = 2;
        static int width(code_point u)
        {
            return u >= 0x10000 ? 2 : 1;
        }
        template<typename It>
        static It encode(code_point u, It out)
        {
            if (UTF_LIKELY(u <= 0xFFFF)) {
                *out++ = static_cast<char_type>(u);
            }
            else {
                u -= 0x10000;
                *out++ = static_cast<char_type>(0xD800 | (u >> 10));
                *out++ = static_cast<char_type>(0xDC00 | (u & 0x3FF));
            }
            return out;
        }
    }; // utf16;


    template<typename CharType>
    struct utf_traits<CharType, 4> {
        typedef CharType char_type;
        static int trail_length(char_type c)
        {
            if (is_valid_codepoint(c))
                return 0;
            return -1;
        }
        static bool is_trail(char_type /*c*/)
        {
            return false;
        }
        static bool is_lead(char_type /*c*/)
        {
            return true;
        }

        template<typename It>
        static code_point decode_valid(It& current)
        {
            return *current++;
        }

        template<typename It>
        static code_point decode(It& current, It last)
        {
            if (UTF_UNLIKELY(current == last))
                return utf::incomplete;
            code_point c = *current++;
            if (UTF_UNLIKELY(!is_valid_codepoint(c)))
                return utf::illegal;
            return c;
        }
        static const int max_width = 1;
        static int width(code_point /*u*/)
        {
            return 1;
        }
        template<typename It>
        static It encode(code_point u, It out)
        {
            *out++ = static_cast<char_type>(u);
            return out;
        }

    }; // utf32

    template<typename InsertType>
    class insert_count_iterator { // wrap pushes to back of container as output iterator
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type        = void;
        using difference_type   = void;
        using pointer           = void;
        using reference         = void;

        explicit insert_count_iterator(size_t& val) : counter(&val) {
        }

        insert_count_iterator& operator=(const InsertType& ) {
            ++(*counter);
            return *this;
        }

        insert_count_iterator& operator=(InsertType&& ) {
            ++(*counter);
            return *this;
        }

        insert_count_iterator& operator*() { // pretend to return designated value
            return *this;
        }

        insert_count_iterator& operator++() { // pretend to preincrement
            return *this;
        }

        insert_count_iterator operator++(int) { // pretend to postincrement
            return *this;
        }

    protected:
        size_t *counter;
    };

    template<typename CharOut, typename ItIn, typename ItOut>
    ItOut utf_to_utf(ItIn first1, ItIn last1, ItOut first2, bool stopOnError = true)
    {
        using CharIn = typename std::iterator_traits<ItIn>::value_type;
        code_point c;
        while (first1 != last1)
        {
            c = utf::utf_traits<CharIn>::decode(first1, last1);
            if (c == utf::illegal || c == utf::incomplete)
            {
                if (stopOnError)
                    return first2;
            }
            else
            {
                first2 = utf::utf_traits<CharOut>::encode(c, first2);
            }
        }
        return first2;
    }

    template<typename CharOut, typename ItIn, typename ItOut>
    ItOut utf_to_utf(ItIn first1, ItIn last1, ItOut first2, size_t maxSize, bool stopOnError = true)
    {
        using CharIn = typename std::iterator_traits<ItIn>::value_type;
        code_point c;
        while (first1 != last1)
        {
            c = utf::utf_traits<CharIn>::decode(first1, last1);
            if (c == utf::illegal || c == utf::incomplete)
            {
                if (stopOnError)
                    return first2;
            }
            else
            {
                size_t count = 0;
                insert_count_iterator<CharOut> count_it(count);
                utf::utf_traits<CharOut>::encode(c, count_it);
                if (count > maxSize)
                    return first2;
                first2 = utf::utf_traits<CharOut>::encode(c, first2);
                maxSize -= count;
            }
        }
        return first2;
    }
} // utf

template <typename CharOut, typename ItIn>
std::basic_string<CharOut> transcodeString(ItIn first, ItIn last)
{
    std::basic_string<CharOut> outString;
    utf::utf_to_utf<CharOut>(first, last, std::back_inserter(outString));
    return outString;
}

template <typename CharOut, typename CharIn>
std::basic_string<CharOut> transcodeString(const CharIn* inString, size_t len)
{
    std::basic_string<CharOut> outString;
    utf::utf_to_utf<CharOut>(inString, inString + len, std::back_inserter(outString));
    return outString;
}

template <typename CharOut, typename CharIn>
std::basic_string<CharOut> transcodeString(const std::basic_string<CharIn>& inString)
{
    return transcodeString<CharOut>(inString.c_str(), inString.length());
}

template <typename CharOut, typename CharIn>
std::basic_string<CharOut> transcodeString(const CharIn* inString)
{
    size_t len = std::char_traits<CharIn>::length(inString);
    return transcodeString<CharOut>(inString, len);
}

template <typename CharOut, typename CharIn>
size_t transcodeString(const CharIn* inString, size_t len, CharOut *outString)
{
    return utf::utf_to_utf<CharOut>(inString, inString + len, outString) - outString;
}

template <typename CharOut, typename CharIn>
size_t transcodeString(const CharIn* inString, CharOut* outString)
{
    size_t len = std::char_traits<CharIn>::length(inString);
    return transcodeString(inString, len + 1, outString);
}

template <typename CharOut, typename CharIn>
size_t getTranscodedSize(const CharIn* inString, size_t len)
{
    size_t count = 0;
    utf::insert_count_iterator<CharOut> count_it(count);
    utf::utf_to_utf<CharOut>(inString, inString + len, count_it);
    return count;
}

template <typename CharOut, typename CharIn>
size_t getTranscodedSize(const CharIn* inString)
{
    size_t len = std::char_traits<CharIn>::length(inString);
    return getTranscodedSize<CharOut, CharIn>(inString, len + 1);
}

template <typename CharOut, typename CharIn>
size_t getTranscodedLen(const CharIn* inString)
{
    size_t len = std::char_traits<CharIn>::length(inString);
    return getTranscodedSize<CharOut, CharIn>(inString, len + 1) - 1;
}

template <typename CharOut, typename CharIn>
size_t transcodeStringN(const CharIn* inString, size_t len, CharOut* outString, size_t maxSize)
{
    return utf::utf_to_utf<CharOut>(inString, inString + len, outString, maxSize) - outString;
}

template <typename CharOut, typename CharIn>
size_t transcodeStringN(const CharIn* inString, CharOut* outString, size_t maxSize)
{
    if (maxSize == 0)
        return 0;
    size_t len = std::char_traits<CharIn>::length(inString);
    size_t tsz = getTranscodedSize<CharOut>(inString, len + 1);
    if (tsz <= maxSize)
    {
        return transcodeStringN(inString, len + 1, outString, maxSize);
    }
    size_t res = transcodeStringN(inString, len, outString, maxSize - 1);
    transcodeStringN("", 1, outString + res, 1);
    return res + 1;
}

template <typename CharT>
CharT* nextCodePoint(CharT* str)
{
    for (++str; !utf::utf_traits<CharT>::is_lead(*str); ++str)
    {
    }
    return str;
}

template <typename CharT>
const CharT* nextCodePoint(const CharT* str)
{
    for (++str; !utf::utf_traits<CharT>::is_lead(*str); ++str)
    {
    }
    return str;
}

template <typename CharT>
CharT* prevCodePoint(CharT* str)
{
    for (--str; !utf::utf_traits<CharT>::is_lead(*str); --str)
    {
    }
    return str;
}

template <typename CharT>
const CharT* prevCodePoint(const CharT* str)
{
    for (--str; !utf::utf_traits<CharT>::is_lead(*str); --str)
    {
    }
    return str;
}

#endif /* #ifndef COMMONTYPESCHAR_H */
