#ifndef ___CMDLINE_H___
#   define ___CMDLINE_H___

#   if defined(_MSC_VER) && (_MSC_VER > 1000)
#       pragma once
#   endif

#   include <climits>
#   include <cfloat>
#   include <cstring>
#   include <cstdlib>
#   include <sstream>
#   include <stdexcept>
#   include <string>

///////////////////////////////////////////////////////////////////////////
///  Convert a string to a long integer.
///
///  @param str  The null-terminated string to convert.
///  @param base Number base to use (default: 10). 
///
///  @exception  std::invalid_argument Conversion unsuccessful.
///
///  @return The converted long integer.
///
///////////////////////////////////////////////////////////////////////////
inline long
str_to_long(const char* str, int base = 10) {
    char* endptr;
    long ans = strtol(str, &endptr, base);
    if (endptr == str || *endptr != '\0') {
        std::stringstream ss;
        ss << "Could not convert string '" << str 
            << "' to a long integer.";
        throw std::invalid_argument(ss.str());
    }
    return ans;
}

///////////////////////////////////////////////////////////////////////////
///  Convert a string to an unsigned long integer.
///
///  @param str  The null-terminated string to convert.
///  @param base Number base to use (default: 10). 
///
///  @exception  std::invalid_argument Conversion unsuccessful.
///
///  @return The converted unsigned long integer.
///
///////////////////////////////////////////////////////////////////////////
inline unsigned long
str_to_unsigned_long(const char* str, int base = 10) {
    char* endptr;
    unsigned long ans = strtoul(str, &endptr, base);
    if (endptr == str || *endptr != '\0') {
        std::stringstream ss;
        ss << "Could not convert string '" << str 
            << "' to an unsigned long integer.";
        throw std::invalid_argument(ss.str());
    }
    return ans;
}

///////////////////////////////////////////////////////////////////////////
///  Convert a string to a double-precision value.
///
///  @param str  The null-terminated string to convert.
///  @param base Number base to use. 
///
///  @exception  std::invalid_argument Conversion unsuccessful.
///
///  @return The converted double-precision value.
///
///////////////////////////////////////////////////////////////////////////
inline double
str_to_double(const char* str) {
    char* endptr;
    double ans = strtod(str, &endptr);
    if (endptr == str || *endptr != '\0') {
        std::stringstream ss;
        ss << "Could not convert string '" << str << "' to double.";
        throw std::invalid_argument(ss.str());
    }
    return ans;
}


///////////////////////////////////////////////////////////////////////////
class CmdLine
{
public:

    static const char* Empty;

    ///////////////////////////////////////////////////////////////////////
    ///  Constructor.
    ///
    ///  @param argc  Number of arguments.
    ///  @param argv  Array of argument strings.
    ///////////////////////////////////////////////////////////////////////
    CmdLine(int argc, char** argv) :
        _argi(1), _argc(argc), _argv(argv), _option(NULL), _value(NULL)
    {
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Gets the next argument.
    ///
    ///  @return Returns false if the last argument is reached,
    ///          true otherwise.
    ///////////////////////////////////////////////////////////////////////
    bool
    next() {
        if (_argi >= _argc) {
            _option = NULL;
            return false;
        }

        _arg = _argv[_argi];

        if (_arg[0] == (char)('-')) {

            // Short option begins with '-', long option begins with '--'.
            _option = _argv[_argi] + 1;
            if (*_option == (char)('-')) ++_option;

            _value = _option;
            while (*_value != (char)('\0') &&
                *_value != (char)('=')) {
                ++_value;
            }

            if (*_value == (char)('=')) {
                *_value++ = (char)('\0');
                if (*_value == (char)('\"')) {
                    // Resolve quoted strings.
                    char* p = ++_value;
                    while (*p != (char)('\0') &&
                        *p != (char)('\"')) {
                        ++p;
                    }
                    *p = (char)('\0');
                }
            }
            else {
                _value = const_cast<char*>(Empty);
            }
        }
        else {
            _option = NULL;
            _value  = NULL;
        }

        ++_argi;

        return true;
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Determines if the current argument matches the specified pattern.
    ///////////////////////////////////////////////////////////////////////
    bool
    match(const char* short_option,
          const char* long_option,
          bool        has_value = false
          ) const
    {
        return (str_match(short_option, _option) ||
               str_match(long_option, _option)) &&
               (!has_value || 0 != _value);
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current argument.
    ///////////////////////////////////////////////////////////////////////
    inline const char* argument() const { return _arg.c_str(); }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option.
    ///////////////////////////////////////////////////////////////////////
    inline const char* option()   const { return _option; }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option value.
    ///////////////////////////////////////////////////////////////////////
    inline const char* value()    const { return _value; }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option value as a double.
    ///////////////////////////////////////////////////////////////////////
    double
    value_as_double() const {
        double v;
        try {
            v = str_to_double(_value);
        }
        catch(std::exception& e) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " << e.what();
            throw std::invalid_argument(ss.str());
        }
        return v;
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option value as a double.
    ///////////////////////////////////////////////////////////////////////
    double
    value_as_double(double min_val, double max_val = DBL_MAX) const {
        double v = value_as_double();
        if (v < min_val) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " 
                << "Value must be >= " 
                << min_val << ".";
            throw std::invalid_argument(ss.str());
        }
        else if (v > max_val) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " 
                << "Value must be <= " 
                << max_val << ".";
            throw std::invalid_argument(ss.str());
        }
        return v;
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option value as a long integer.
    ///////////////////////////////////////////////////////////////////////
    long
    value_as_long() const {
        long v;
        try {
            v = str_to_long(_value);
        }
        catch(std::exception& e) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " << e.what();
            throw std::invalid_argument(ss.str());
        }
        return v;
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option value as a long integer.
    ///////////////////////////////////////////////////////////////////////
    long
    value_as_long(long min_val, long max_val = LONG_MAX) const {
        long v = value_as_long();
        if (v < min_val) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " 
                << "Value must be >= " 
                << min_val << ".";
            throw std::invalid_argument(ss.str());
        }
        else if (v > max_val) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " 
                << "Value must be <= " 
                << max_val << ".";
            throw std::invalid_argument(ss.str());
        }
        return v;
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Returns the current option value as an unsigned long integer.
    ///////////////////////////////////////////////////////////////////////
    unsigned long
    value_as_unsigned_long() const {
        unsigned long v;
        try {
            v = str_to_unsigned_long(_value);
        }
        catch(std::exception& e) {
            std::stringstream ss;
            ss << "Bad argument '" << _arg << "': " << e.what();
            throw std::invalid_argument(ss.str());
        }
        return v;
    }

    ///////////////////////////////////////////////////////////////////////
    ///  Determents if the current argument is an option.
    ///////////////////////////////////////////////////////////////////////
    inline bool is_option() const { return (NULL != _option); }

    ///////////////////////////////////////////////////////////////////////
    inline const char*
    app_name() const {
        const char* p = 0;
        if (_argv) {
            const char* p0;
            p = p0 = _argv[0];
            while (p && *p != '\0') ++p;
            while (p >= p0 && *p != '/' && *p != '\\')
                --p;
            ++p;
        }
        return p;
    }

private:
    
    ///////////////////////////////////////////////////////////////////////
    bool
    str_match(const char* str1, const char* str2) const {
        if (str1 == NULL || str2 == NULL)
            return false;

        while (*str1 == *str2 &&
               *str2 != (char)('\0')) {
           ++str1; ++str2;
        }

        return (*str1 == *str2);
    }

    ///////////////////////////////////////////////////////////////////////
    std::string _arg;
    int         _argi, _argc;
    char        **_argv, *_option, *_value;
};

const char* CmdLine::Empty = "";

#endif //___CMDLINE_H___
