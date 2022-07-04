#ifndef STREAM_FORMATTER_HPP
#define STREAM_FORMATTER_HPP

#include <iostream>
#include <ostream>
#include <sstream>

namespace rtstd
{
    template <typename _CharT>
    struct _SetIndent
    {
        unsigned int indentSize;
        _CharT indentChar;
    };

    struct _PopIndent
    {
    };

    template <typename _CharT>
    class indent_streambuf : public std::basic_streambuf<_CharT>
    {
    private:
        typedef std::basic_streambuf<_CharT> base;

        using typename base::int_type;

        unsigned int m_indentSize;
        _CharT m_indentChar;
        std::ostream &m_oldStream;

        std::stringstream m_str;

    public:
        indent_streambuf(unsigned int indentSize, _CharT indentChar, std::ostream &oldStream)
            : m_indentSize(indentSize), m_indentChar(indentChar), m_oldStream(oldStream) {}
        virtual ~indent_streambuf() {}

    protected:
        int_type overflow(int_type ch) override
        {
            static std::basic_string<_CharT> str = "\n" + std::basic_string<_CharT>(m_indentSize, m_indentChar);
            if (ch == (_CharT)'\n')
                m_str << str;
            else
                m_str << (_CharT)ch;
            return ch;
        }

        inline std::string str() { return m_str.str(); }

        friend std::basic_ostream<_CharT> &operator<< <_CharT>(std::basic_ostream<_CharT> &__os, _PopIndent __p);
    };

    template <typename _CharT = char>
    inline _SetIndent<_CharT> pushIndent(unsigned int indentSize = 4, _CharT indentChar = ' ')
    {
        return _SetIndent{indentSize, indentChar};
    }

    inline _PopIndent popIndent()
    {
        return _PopIndent();
    }

    template <typename _CharT>
    inline std::basic_ostream<_CharT> operator<<(std::basic_ostream<_CharT> &__os, _SetIndent<_CharT> __i)
    {
        return std::basic_ostream<_CharT>(new indent_streambuf(__i.indentSize, __i.indentChar, __os));
    }

    template <typename _CharT>
    inline std::basic_ostream<_CharT> &operator<<(std::basic_ostream<_CharT> &__os, _PopIndent __p)
    {
        indent_streambuf<_CharT> *ios = dynamic_cast<indent_streambuf<_CharT> *>(__os.rdbuf());
        if (ios == nullptr)
            throw std::bad_cast();
        ios->m_oldStream << ios->str();
        delete ios;
        return ios->m_oldStream;
    }

} // namespace rtstd

#endif // STREAM_FORMATTER_HPP