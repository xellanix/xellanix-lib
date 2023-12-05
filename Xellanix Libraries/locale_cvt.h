#pragma once

#ifndef XELLANIX_LOCALE_CVT_H
#define XELLANIX_LOCALE_CVT_H
#endif // !LOCALE_CVT_H

#include <locale>

namespace xellanix::cvt
{
    using _Statype = ::mbstate_t;

    enum cvt_mode { consume_header = 4, generate_header = 2, little_endian = 1 };

    template <class _Elem, unsigned long _Mymax = 0x10ffff, cvt_mode _Mymode = cvt_mode{} >
    class utf8cvt : public std::codecvt<_Elem, char, _Statype>
    {
        // facet for converting between _Elem and UTF-8 byte sequences
    public:
        using _Mybase = std::codecvt<_Elem, char, _Statype>;
        using result = typename _Mybase::result;
        using _Byte = char;
        using intern_type = _Elem;
        using extern_type = _Byte;
        using state_type = _Statype;

        explicit utf8cvt(size_t _Refs = 0) : _Mybase(_Refs) {}

        __CLR_OR_THIS_CALL ~utf8cvt() noexcept override {}

    protected:
        result __CLR_OR_THIS_CALL do_in(_Statype& _State, const _Byte* _First1, const _Byte* _Last1, const _Byte*& _Mid1,
                                        _Elem* _First2, _Elem* _Last2, _Elem*& _Mid2) const override
        {
            // convert bytes [_First1, _Last1) to [_First2, _Last2)
            char* _Pstate = reinterpret_cast<char*>(&_State);
            _Mid1 = _First1;
            _Mid2 = _First2;

            while (_Mid1 != _Last1 && _Mid2 != _Last2)
            { // convert a multibyte sequence
                unsigned long _By = static_cast<unsigned char>(*_Mid1);
                unsigned long _Ch;
                int _Nextra;

                if (_By < 0x80u)
                {
                    _Ch = _By;
                    _Nextra = 0;
                }
                else if (_By < 0xc0u)
                { // 0x80-0xbf not first byte
                    ++_Mid1;
                    return _Mybase::error;
                }
                else if (_By < 0xe0u)
                {
                    _Ch = _By & 0x1f;
                    _Nextra = 1;
                }
                else if (_By < 0xf0u)
                {
                    _Ch = _By & 0x0f;
                    _Nextra = 2;
                }
                else if (_By < 0xf8u)
                {
                    _Ch = _By & 0x07;
                    _Nextra = 3;
                }
                else
                {
                    _Ch = _By & 0x03;
                    _Nextra = _By < 0xfc ? 4 : 5;
                }

                if (_Nextra == 0)
                {
                    ++_Mid1;
                }
                else if (_Last1 - _Mid1 < _Nextra + 1)
                {
                    break; // not enough input
                }
                else
                {
                    for (++_Mid1; 0 < _Nextra; --_Nextra, ++_Mid1)
                    {
                        if ((_By = static_cast<unsigned char>(*_Mid1)) < 0x80u || 0xc0u <= _By)
                        {
                            return _Mybase::error; // not continuation byte
                        }
                        else
                        {
                            _Ch = _Ch << 6 | (_By & 0x3f);
                        }
                    }
                }

                if (*_Pstate == 0)
                { // first time, maybe look for and consume header
                    *_Pstate = 1;

                    constexpr bool _Consuming = (_Mymode & consume_header) != 0;
                    if constexpr (_Consuming)
                    {
                        if (_Ch == 0xfeff)
                        { // drop header and retry
                            const result _Ans = do_in(_State, _Mid1, _Last1, _Mid1, _First2, _Last2, _Mid2);

                            if (_Ans == _Mybase::partial)
                            { // roll back header determination
                                *_Pstate = 0;
                                _Mid1 = _First1;
                            }

                            return _Ans;
                        }
                    }
                }

                if (_Mymax < _Ch)
                {
                    return _Mybase::error; // code too large
                }

                *_Mid2++ = static_cast<_Elem>(_Ch);
            }

            return _First1 == _Mid1 ? _Mybase::partial : _Mybase::ok;
        }

        result __CLR_OR_THIS_CALL do_out(_Statype& _State, const _Elem* _First1, const _Elem* _Last1, const _Elem*& _Mid1,
                                         _Byte* _First2, _Byte* _Last2, _Byte*& _Mid2) const override
        {
            // convert [_First1, _Last1) to bytes [_First2, _Last2)
            char* _Pstate = reinterpret_cast<char*>(&_State);
            _Mid1 = _First1;
            _Mid2 = _First2;

            while (_Mid1 != _Last1 && _Mid2 != _Last2)
            { // convert and put a wide char
                _Byte _By;
                int _Nextra;
                unsigned long _Ch = static_cast<unsigned long>(*_Mid1);

                if (_Mymax < _Ch)
                {
                    return _Mybase::error;
                }

                if (_Ch < 0x0080u)
                {
                    _By = static_cast<_Byte>(_Ch);
                    _Nextra = 0;
                }
                else if (_Ch < 0x0800u)
                {
                    _By = static_cast<_Byte>(0xc0 | _Ch >> 6);
                    _Nextra = 1;
                }
                else if (_Ch < 0x00010000u)
                {
                    _By = static_cast<_Byte>(0xe0 | _Ch >> 12);
                    _Nextra = 2;
                }
                else if (_Ch < 0x00200000u)
                {
                    _By = static_cast<_Byte>(0xf0 | _Ch >> 18);
                    _Nextra = 3;
                }
                else if (_Ch < 0x04000000u)
                {
                    _By = static_cast<_Byte>(0xf8 | _Ch >> 24);
                    _Nextra = 4;
                }
                else
                {
                    _By = static_cast<_Byte>(0xfc | (_Ch >> 30 & 0x03));
                    _Nextra = 5;
                }

                if (*_Pstate == 0)
                { // first time, maybe generate header
                    *_Pstate = 1;
                    constexpr bool _Generating = (_Mymode & generate_header) != 0;
                    if constexpr (_Generating)
                    {
                        if (_Last2 - _Mid2 < 3 + 1 + _Nextra)
                        {
                            return _Mybase::partial; // not enough room for both
                        }

                        // prepend header
                        *_Mid2++ = '\xef';
                        *_Mid2++ = '\xbb';
                        *_Mid2++ = '\xbf';
                    }
                }

                if (_Last2 - _Mid2 < 1 + _Nextra)
                {
                    break; // not enough room for output
                }

                ++_Mid1;
                for (*_Mid2++ = _By; 0 < _Nextra;)
                {
                    *_Mid2++ = static_cast<_Byte>((_Ch >> 6 * --_Nextra & 0x3f) | 0x80);
                }
            }

            return _First1 == _Mid1 ? _Mybase::partial : _Mybase::ok;
        }

        result __CLR_OR_THIS_CALL do_unshift(_Statype&, _Byte* _First2, _Byte*, _Byte*& _Mid2) const override
        {
            // generate bytes to return to default shift state
            _Mid2 = _First2;
            return _Mybase::noconv;
        }

        // friend int _Codecvt_do_length<>(const utf8cvt&, _Statype&, const _Byte*, const _Byte*, size_t);
        _NODISCARD int _Codecvt_do_length(
            const utf8cvt& _Cvt, _Statype& _State, const _Byte* _First1, const _Byte* _Last1, size_t _Count)
        {
            // return p - _First1, for the largest value p in [_First1, _Last1] such that _Cvt will successfully convert
            // [_First1, p) to at most _Count wide characters

            _STL_DISABLE_DEPRECATED_WARNING
                using _Elem = utf8cvt::intern_type;
            _STL_RESTORE_DEPRECATED_WARNING

                std::_Adl_verify_range(_First1, _Last1);
            const auto _Old_first1 = _First1;

            while (_Count > 0 && _First1 != _Last1)
            { // convert another wide character
                const _Byte* _Mid1;
                _Elem* _Mid2;
                _Elem _Ch;

                // test result of single wide-char conversion

                _STL_DISABLE_DEPRECATED_WARNING
                    const auto _Result = _Cvt.utf8cvt::do_in(_State, _First1, _Last1, _Mid1, &_Ch, &_Ch + 1, _Mid2);
                _STL_RESTORE_DEPRECATED_WARNING

                    if (_Result != std::codecvt_base::ok)
                    {
                        if (_Result == std::codecvt_base::noconv)
                        {
                            _First1 += (_STD min)(static_cast<size_t>(_Last1 - _First1), _Count);
                        }

                        break; // error, noconv, or partial
                    }

                if (_Mid2 == &_Ch + 1)
                {
                    --_Count; // do_in converted an output character
                }

                _First1 = _Mid1;
            }

            return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{ INT_MAX }));
        }

        int __CLR_OR_THIS_CALL do_length(
            _Statype& _State, const _Byte* _First1, const _Byte* _Last1, size_t _Count) const noexcept override
        {
            // return p - _First1, for the largest value p in [_First1, _Last1] such that _Cvt will successfully convert
            // [_First1, p) to at most _Count wide characters

            std::_Adl_verify_range(_First1, _Last1);
            const auto _Old_first1 = _First1;

            while (_Count > 0 && _First1 != _Last1)
            { // convert another wide character
                const _Byte* _Mid1;
                _Elem* _Mid2;
                _Elem _Ch;

                // test result of single wide-char conversion

                _STL_DISABLE_DEPRECATED_WARNING
                    const auto _Result = (*this).utf8cvt::do_in(_State, _First1, _Last1, _Mid1, &_Ch, &_Ch + 1, _Mid2);
                _STL_RESTORE_DEPRECATED_WARNING

                    if (_Result != std::codecvt_base::ok)
                    {
                        if (_Result == std::codecvt_base::noconv)
                        {
                            _First1 += (std::min)(static_cast<size_t>(_Last1 - _First1), _Count);
                        }

                        break; // error, noconv, or partial
                    }

                if (_Mid2 == &_Ch + 1)
                {
                    --_Count; // do_in converted an output character
                }

                _First1 = _Mid1;
            }

            return static_cast<int>((_STD min)(_First1 - _Old_first1, ptrdiff_t{ INT_MAX }));
        }

        bool __CLR_OR_THIS_CALL do_always_noconv() const noexcept override
        {
            // return true if conversions never change input
            return false;
        }

        int __CLR_OR_THIS_CALL do_max_length() const noexcept override
        {
            // return maximum length required for a conversion
            return (_Mymode & (consume_header | generate_header)) != 0 ? 9 : 6;
        }

        int __CLR_OR_THIS_CALL do_encoding() const noexcept override
        {
            // return length of code sequence (from codecvt)
            return (_Mymode & (consume_header | generate_header)) != 0 ? -1
                : 0; // -1 => state dependent, 0 => varying length
        }
    };
}