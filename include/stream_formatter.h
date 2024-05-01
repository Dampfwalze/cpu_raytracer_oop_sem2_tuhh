#ifndef STREAM_FORMATTER_HPP
#define STREAM_FORMATTER_HPP

#include <iostream>
#include <ostream>
#include <sstream>

namespace rtstd
{
    template <class _Elem>
    struct basic_formatterargs
    {
        using char_type = _Elem;

        int       indentSize = 4;
        char_type indent = (char_type)' ';
        char_type openingBracket = (char_type)'{';
        char_type closingBracket = (char_type)'}';
        char_type separator = (char_type)',';
        char_type deactivator = (char_type)'(';
        char_type activator = (char_type)')';
        char_type lineBreak = (char_type)'\n';
    };

    template <class _Elem, class _Traits>
    class basic_formatterbuf : public std::basic_streambuf<_Elem, _Traits>
    {
    public:
        using child_type = std::basic_ostream<_Elem, _Traits>;
        using char_type = _Elem;
        using int_type = typename _Traits::int_type;

    private:
        using base = std::basic_streambuf<_Elem, _Traits>;

        basic_formatterargs<_Elem> m_args;

        int  m_indentCount = 0;
        bool m_drainWhitespace = false;
        int  m_deactivateCount = 0;

        child_type &m_outStream;

    public:
        basic_formatterbuf(child_type &outStream, int indentSize = 4)
            : m_outStream(outStream), m_args()
        {
            m_args.indentSize = indentSize;
        }
        basic_formatterbuf(child_type &outStream, const basic_formatterargs<_Elem> &args)
            : m_outStream(outStream), m_args(args) {}
        ~basic_formatterbuf() {}

    private:
        void writeIndent()
        {
            for (int64_t i = 0; i < m_indentCount * m_args.indentSize; i++)
                m_outStream << m_args.indent;
        }

        void pushIndent()
        {
            m_outStream << m_args.lineBreak;
            m_indentCount++;
            writeIndent();
            if (m_deactivateCount <= 0)
                m_drainWhitespace = true;
        }

        void popIndent()
        {
            m_outStream << m_args.lineBreak;
            m_indentCount--;
            writeIndent();
        }

        void breakLine()
        {
            m_outStream << m_args.lineBreak;
            writeIndent();
            if (m_deactivateCount <= 0)
                m_drainWhitespace = true;
        }

    protected:
        int_type overflow(int_type ch_i) override
        {
            char_type ch = (char_type)ch_i;

            if (m_drainWhitespace && std::isspace(ch))
                return ch;
            else
                m_drainWhitespace = false;

            if (ch == m_args.deactivator)
            {
                m_deactivateCount++;
                m_outStream << ch;
                return ch;
            }
            else if (ch == m_args.activator)
            {
                m_deactivateCount--;
                m_outStream << ch;
                return ch;
            }
            else if (ch == m_args.lineBreak)
            {
                m_outStream << ch;
                writeIndent();
                return ch;
            }
            else if (m_deactivateCount > 0)
            {
                m_outStream << ch;
                return ch;
            }
            else if (ch == m_args.openingBracket)
            {
                m_outStream << ch;
                pushIndent();
                return ch;
            }
            else if (ch == m_args.closingBracket)
            {
                popIndent();
                m_outStream << ch;
                return ch;
            }
            else if (ch == m_args.separator)
            {
                m_outStream << ch;
                breakLine();
                return ch;
            }
            else
            {
                m_outStream << ch;
                return ch;
            }
        }
    };

    template <class _Elem, class _Traits>
    class basic_formatterstream : public std::basic_ostream<_Elem, _Traits>
    {
    public:
        using child_type = std::basic_ostream<_Elem, _Traits>;
        using char_type = _Elem;
        using traits_type = _Traits;

        using _buffer_type = basic_formatterbuf<_Elem, _Traits>;

    private:
        using base = std::basic_ostream<_Elem, _Traits>;

        _buffer_type m_buffer;

    public:
        basic_formatterstream(child_type &outStream, int indentSize = 4)
            : base(std::addressof(m_buffer)), m_buffer(outStream, indentSize) {}
        basic_formatterstream(child_type &outStream, const basic_formatterargs<_Elem> &args)
            : base(std::addressof(m_buffer)), m_buffer(outStream, args) {}
        ~basic_formatterstream() {}
    };

    using formatterstream = basic_formatterstream<char, std::char_traits<char>>;
    using formatterargs = basic_formatterargs<char>;

} // namespace rtstd

#endif // STREAM_FORMATTER_HPP