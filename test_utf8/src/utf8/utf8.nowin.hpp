#ifndef INCLUDE_UTF8_HPP_
#define INCLUDE_UTF8_HPP_


#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

#ifndef _WINDOWS_

using UINT = unsigned int;
using DWORD = unsigned long;
using LPCCH = const char *;
using PCCH = const char *;
using WCHAR = wchar_t; // wc,   16-bit UNICODE character
using LPWSTR = WCHAR *;
using LPCWCH = const WCHAR *;
using PCWCH = const WCHAR *;
using LPSTR = char *;
using LPBOOL = int *;
using LPCWSTR = const WCHAR *;
#define CP_UTF8 65001 // UTF-8 translation

using HANDLE = void *;
using HLOCAL = HANDLE;

extern "C" __declspec(dllimport) 
int __stdcall MultiByteToWideChar(UINT CodePage, DWORD dwFlags, LPCCH lpMultiByteStr, int cbMultiByte, LPWSTR lpWideCharStr, int cchWideChar);
extern "C" __declspec(dllimport) 
int __stdcall WideCharToMultiByte(UINT CodePage, DWORD dwFlags, LPCWCH lpWideCharStr, int cchWideChar, LPSTR lpMultiByteStr, int cbMultiByte, LPCCH lpDefaultChar,
                                  LPBOOL lpUsedDefaultChar);
                                  
extern "C" __declspec(dllimport) LPWSTR* __stdcall CommandLineToArgvW(LPCWSTR lpCmdLine, int* pNumArgs);
extern "C" __declspec(dllimport) LPWSTR __stdcall GetCommandLineW();
extern "C" __declspec(dllimport) HLOCAL __stdcall LocalFree(HLOCAL hMem);

#endif /* _WINDOWS_ */


namespace utf8 {

/// Exception thrown on encoding/decoding failure
struct exception : public std::exception {
    /// Possible causes
    enum reason { invalid_utf8, invalid_char32 };

    /// Constructor
    explicit exception(reason p_cause)
        : std::exception(p_cause == reason::invalid_utf8     ? "Invalid UTF-8 encoding"
                         : p_cause == reason::invalid_char32 ? "Invalid code-point value"
                                                       : "Other UTF-8 exception"),
          cause(p_cause) {
    }

    /// What triggered the exception
    reason cause;
};

/// Replacement character used for invalid encodings
constexpr char32_t REPLACEMENT_CHARACTER = 0xfffd;


/// \addtogroup basecvt
/// @{
[[nodiscard]] auto narrow(const wchar_t* input_s, size_t nch = 0) -> std::string;
[[nodiscard]] auto narrow(std::wstring const& input_s) -> std::string;
[[nodiscard]] auto narrow(const char32_t* input_s, size_t nch = 0) -> std::string;
[[nodiscard]] auto narrow(std::u32string const& input_s) -> std::string;
[[nodiscard]] auto narrow(char32_t input_r) -> std::string;

[[nodiscard]] auto widen(const char* input_s, size_t nch = 0) -> std::wstring;
[[nodiscard]] auto widen(std::string const& input_s) -> std::wstring;
[[nodiscard]] auto runes(const char* input_s, size_t nch = 0) -> std::u32string;
[[nodiscard]] auto runes(std::string const& input_s) -> std::u32string;

[[nodiscard]] auto rune(const char* ptr) -> char32_t;
[[nodiscard]] auto rune(const std::string::const_iterator& p_check) -> char32_t;
/// @}

[[nodiscard]] auto is_valid(const char* ptr) -> bool;
[[nodiscard]] auto is_valid(std::string::const_iterator ptr, const std::string::const_iterator last) -> bool;
[[nodiscard]] auto valid_str(const char* input_s, size_t nch = 0) -> bool;
[[nodiscard]] auto valid_str(std::string const& input_s) -> bool;

[[nodiscard]] auto next(std::string::const_iterator& ptr, const std::string::const_iterator last) -> char32_t;
[[nodiscard]] auto next(const char*& ptr) -> char32_t;
[[nodiscard]] auto next(char*& ptr) -> char32_t;

[[nodiscard]] auto prev(const char*& ptr) -> char32_t;
[[nodiscard]] auto prev(char*& ptr) -> char32_t;
[[nodiscard]] auto prev(std::string::const_iterator& ptr, const std::string::const_iterator first) -> char32_t;

[[nodiscard]] auto length(std::string const& input_s) -> size_t;
[[nodiscard]] auto length(const char* input_s) -> size_t;


[[nodiscard]] auto get_argv() -> std::vector<std::string>;
[[nodiscard]] auto get_argv(int32_t* argc) -> char**;
void free_argv(int32_t argc, char** argv);

[[nodiscard]] auto mkdir(const char* dirname) -> bool;
[[nodiscard]] auto mkdir(std::string const& dirname) -> bool;

[[nodiscard]] auto rmdir(const char* dirname) -> bool;
[[nodiscard]] auto rmdir(std::string const& dirname) -> bool;

[[nodiscard]] auto chdir(const char* dirname) -> bool;
[[nodiscard]] auto chdir(std::string const& dirname) -> bool;

[[nodiscard]] auto chmod(const char* filename, int32_t mode) -> bool;
[[nodiscard]] auto chmod(std::string const& filename, int32_t mode) -> bool;

[[nodiscard]] auto getcwd() -> std::string;

[[nodiscard]] auto access(const char* filename, int32_t mode) -> bool;
[[nodiscard]] auto access(std::string const& filename, int32_t mode) -> bool;

[[nodiscard]] auto remove(const char* filename) -> bool;
[[nodiscard]] auto remove(std::string const& filename) -> bool;

[[nodiscard]] auto rename(const char* oldname, const char* newname) -> bool;
[[nodiscard]] auto rename(std::string const& oldname, std::string const& newname) -> bool;

[[nodiscard]] auto fopen(const char* filename, const char* mode) -> FILE*;
[[nodiscard]] auto fopen(std::string const& filename, std::string const& mode) -> FILE*;

[[nodiscard]] auto splitpath(std::string const& path, char* drive, char* dir, char* fname, char* ext) -> bool;
[[nodiscard]] auto splitpath(std::string const& path, std::string& drive, std::string& dir, std::string& fname, std::string& ext) -> bool;
[[nodiscard]] auto makepath(std::string& path, std::string const& drive, std::string const& dir, std::string const& fname, std::string const& ext) -> bool;
[[nodiscard]] auto fullpath(std::string const& relpath) -> std::string;

[[nodiscard]] auto getenv(std::string const& var) -> std::string;
[[nodiscard]] auto putenv(std::string const& str) -> bool;
[[nodiscard]] auto putenv(std::string const& var, std::string const& val) -> bool;

[[nodiscard]] auto symlink(const char* path, const char* link, bool directory) -> bool;
[[nodiscard]] auto symlink(std::string const& path, std::string const& target, bool directory) -> bool;

[[nodiscard]] auto system(std::string const& cmd) -> int32_t;

/*!
  \addtogroup folding
  @{
*/
void make_lower(std::string& str);
void make_upper(std::string& str);
[[nodiscard]] auto tolower(std::string const& str) -> std::string;
[[nodiscard]] auto toupper(std::string const& str) -> std::string;
[[nodiscard]] auto icompare(std::string const& str1, std::string const& str2) -> int32_t;
/// @}

/*!
  \addtogroup charclass
  @{
*/

[[nodiscard]] auto isspace(const char* p_check) -> bool;
[[nodiscard]] auto isspace(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto isblank(const char* p_check) -> bool;
[[nodiscard]] auto isblank(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto isdigit(const char* p_check) -> bool;
[[nodiscard]] auto isdigit(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto isalnum(const char* p_check) -> bool;
[[nodiscard]] auto isalnum(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto isalpha(const char* p_check) -> bool;
[[nodiscard]] auto isalpha(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto isxdigit(const char* p_check) -> bool;
[[nodiscard]] auto isxdigit(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto isupper(const char* p_char) -> bool;
[[nodiscard]] auto isupper(std::string::const_iterator p_check) -> bool;
[[nodiscard]] auto islower(const char* p_char) -> bool;
[[nodiscard]] auto islower(std::string::const_iterator p_check) -> bool;
/// @}

/// Input stream class using UTF-8 filename
class ifstream : public std::ifstream {
public:
    ifstream() = default;
    auto operator=(const ifstream&) -> ifstream& = delete;
    auto operator=(ifstream&&) -> ifstream& = delete;
    explicit ifstream(const char* filename, std::ios_base::openmode mode = ios_base::in) : std::ifstream(utf8::widen(filename), mode){};
    explicit ifstream(std::string const& filename, std::ios_base::openmode mode = ios_base::in) : std::ifstream(utf8::widen(filename), mode){};
    ifstream(ifstream&& other) noexcept : std::ifstream((std::ifstream&&)other){};
    ifstream(const ifstream& rhs) = delete;

    void open(const char* filename, ios_base::openmode mode = ios_base::in, int32_t prot = static_cast<int>(ios_base::_Openprot)) {
        std::ifstream::open(utf8::widen(filename), mode, prot);
    }
    void open(std::string const& filename, ios_base::openmode mode = ios_base::in, int32_t prot = static_cast<int>(ios_base::_Openprot)) {
        std::ifstream::open(utf8::widen(filename), mode, prot);
    }
};

/// Output stream class using UTF-8 filename
class ofstream : public std::ofstream {
public:
    ofstream() = default;
    auto operator=(const ofstream&) -> ofstream& = delete;
    auto operator=(ofstream&&) -> ofstream& = delete;
    explicit ofstream(const char* filename, std::ios_base::openmode mode = ios_base::out) : std::ofstream(utf8::widen(filename), mode){};
    explicit ofstream(std::string const& filename, std::ios_base::openmode mode = ios_base::out) : std::ofstream(utf8::widen(filename), mode){};
    ofstream(ofstream&& other) noexcept : std::ofstream((std::ofstream&&)other){};
    ofstream(const ofstream& rhs) = delete;

    void open(const char* filename, ios_base::openmode mode = ios_base::out, int32_t prot = static_cast<int>(ios_base::_Openprot)) {
        std::ofstream::open(utf8::widen(filename), mode, prot);
    }
    void open(std::string const& filename, ios_base::openmode mode = ios_base::out, int32_t prot = static_cast<int>(ios_base::_Openprot)) {
        std::ofstream::open(utf8::widen(filename), mode, prot);
    }
};

/// Bidirectional stream class using UTF-8 filename
class fstream : public std::fstream {
public:
    fstream() = default;
    auto operator=(const fstream&) -> fstream& = delete;
    auto operator=(fstream&&) -> fstream& = delete;
    explicit fstream(const char* filename, std::ios_base::openmode mode = ios_base::in | ios_base::out) : std::fstream(utf8::widen(filename), mode){};
    explicit fstream(std::string const& filename, std::ios_base::openmode mode = ios_base::in | ios_base::out) : std::fstream(utf8::widen(filename), mode){};
    fstream(fstream&& other) noexcept : std::fstream((std::fstream&&)other){};
    fstream(const fstream& rhs) = delete;

    void open(const char* filename, ios_base::openmode mode = ios_base::in | ios_base::out, int32_t prot = static_cast<int>(ios_base::_Openprot)) {
        std::fstream::open(utf8::widen(filename), mode, prot);
    }
    void open(std::string const& filename, ios_base::openmode mode = ios_base::in | ios_base::out, int32_t prot = static_cast<int>(ios_base::_Openprot)) {
        std::fstream::open(utf8::widen(filename), mode, prot);
    }
};


// INLINES --------------------------------------------------------------------

/*!
  Check if pointer points to a valid UTF-8 encoding
  \param ptr pointer to string
  \return `true` if there is a valid UTF-8 encoding at the current pointer position,
          `false` otherwise.
*/
[[nodiscard]] inline auto is_valid(const char* ptr) -> bool {
    return next(ptr) != REPLACEMENT_CHARACTER;
}

/*!
  Check if iterator points to a valid UTF-8 encoding
  \param ptr    Iterator
  \param last Iterator pointing to end of range
  \return `true` if there is a valid UTF-8 encoding at the current iterator position,
          `false` otherwise.
*/
[[nodiscard]] inline auto is_valid(std::string::const_iterator ptr, const std::string::const_iterator last) -> bool {
    return next(ptr, last) != REPLACEMENT_CHARACTER;
}

/*!
  Conversion from UTF-8 to UTF-32

  \param ptr pointer to character
  \return UTF-32 encoded character or utf8::REPLACEMENT_CHARACTER (0xfffd)
          if it is an invalid UTF-8 encoding
*/
[[nodiscard]] inline auto rune(const char* ptr) -> char32_t {
    return next(ptr);
}


/*!
  Decodes a UTF-8 encoded character and advances pointer to next character

  \param ptr    <b>Reference</b> to character pointer to be advanced
  \return     decoded character

  If the string contains an invalid UTF-8 encoding, the function returns
  utf8::REPLACEMENT_CHARACTER (0xfffd) and advances pointer to beginning of
  next character or end of string.
*/
[[nodiscard]] inline auto next(char*& ptr) -> char32_t {
    return next(const_cast<const char*&>(ptr));
}

/*!
  Decrements a character pointer to previous UTF-8 character

  \param ptr    <b>Reference</b> to character pointer to be decremented
  \return       previous UTF-8 encoded character

  If the string contains an invalid UTF-8 encoding, the function returns
  REPLACEMENT_CHARACTER (0xfffd) and pointer remains unchanged.
*/
[[nodiscard]] inline char32_t prev(char*& ptr) {
    return prev(const_cast<const char*&>(ptr));
}

/*!
  Creates a new directory

  \param dirname UTF-8 path for new directory
  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto mkdir(const char* dirname) -> bool {
    return (_wmkdir(widen(dirname).c_str()) == 0);
}

/// \copydoc utf8::mkdir()
[[nodiscard]] inline auto mkdir(std::string const& dirname) -> bool {
    return (_wmkdir(widen(dirname).c_str()) == 0);
}

/*!
  Deletes a directory

  \param dirname UTF-8 path of directory to be removed
  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto rmdir(const char* dirname) -> bool {
    return (_wrmdir(widen(dirname).c_str()) == 0);
}

/// \copydoc utf8::rmdir()
[[nodiscard]] inline auto rmdir(std::string const& dirname) -> bool {
    return (_wrmdir(widen(dirname).c_str()) == 0);
}


/*!
  Changes the current working directory

  \param dirname UTF-8 path of new working directory
  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto chdir(const char* dirname) -> bool {
    return (_wchdir(widen(dirname).c_str()) == 0);
}

/// \copydoc utf8::chdir()
[[nodiscard]] inline auto chdir(std::string const& dirname) -> bool {
    return (_wchdir(widen(dirname).c_str()) == 0);
}

/*!
  Changes the file access permissions

  \param filename UTF-8 name of file
  \param mode access permissions. Or'ed combination of:
              - _S_IWRITE write permission
              - _S_IREAD  read permission

  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto chmod(const char* filename, int32_t mode) -> bool {
    return (_wchmod(widen(filename).c_str(), mode) == 0);
}

/// \copydoc utf8::chmod()
[[nodiscard]] inline auto chmod(std::string const& filename, int32_t mode) -> bool {
    return (_wchmod(widen(filename).c_str(), mode) == 0);
}


/*!
  Determines if a file has the requested access permissions

  \param filename UTF-8 file path of new working directory
  \param mode required access:
              - 0 existence only
              - 2 write permission
              - 4 read permission
              - 6 read/write permission

  \return true if successful, false otherwise

*/
[[nodiscard]] inline auto access(const char* filename, int32_t mode) -> bool {
    return (_waccess(widen(filename).c_str(), mode) == 0);
}

/// \copydoc utf8::access()
[[nodiscard]] inline auto access(std::string const& filename, int32_t mode) -> bool {
    return (_waccess(widen(filename).c_str(), mode) == 0);
}


/*!
  Delete a file

  \param filename UTF-8 name of file to be deleted
  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto remove(const char* filename) -> bool {
    return (_wremove(widen(filename).c_str()) == 0);
}

/// \copydoc utf8::remove()
[[nodiscard]] inline auto remove(std::string const& filename) -> bool {
    return (_wremove(widen(filename).c_str()) == 0);
}

/*!
  Rename a file or directory

  \param oldname current UTF-8 encoded name of file or directory
  \param newname new UTF-8 name
  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto rename(const char* oldname, const char* newname) -> bool {
    return (_wrename(widen(oldname).c_str(), widen(newname).c_str()) == 0);
}

/// \copydoc utf8::rename()
[[nodiscard]] inline auto rename(std::string const& oldname, std::string const& newname) -> bool {
    return (_wrename(widen(oldname).c_str(), widen(newname).c_str()) == 0);
}

/*!
  Open a file

  \param filename UTF-8 encoded file name
  \param mode access mode
  \return pointer to the opened file or NULL if an error occurs
 */
[[nodiscard]] inline auto fopen(const char* filename, const char* mode) -> FILE* {
    FILE* h = nullptr;
    _wfopen_s(&h, widen(filename).c_str(), widen(mode).c_str());
    return h;
}

/// \copydoc utf8::fopen()
[[nodiscard]] inline auto fopen(std::string const& filename, std::string const& mode) -> FILE* {
    FILE* h = nullptr;
    _wfopen_s(&h, widen(filename).c_str(), widen(mode).c_str());
    return h;
}

/*!
  Verifies if string is a valid UTF-8 encoded string
  \param input_s character string to verify
  \return `true` if string is a valid UTF-8 encoded string, `false` otherwise
*/
[[nodiscard]] inline auto valid_str(std::string const& input_s) -> bool {
    return valid_str(input_s.c_str(), input_s.size());
}

/// @copydoc rune()
[[nodiscard]] inline auto rune(const std::string::const_iterator& p_check) -> char32_t {
    return rune(&(*p_check));
}


/// \copydoc isspace()
[[nodiscard]] inline auto isspace(std::string::const_iterator p_check) -> bool {
    return isspace(&*p_check);
}


/*!
  Return true if character is blank or tab
  \param p_check pointer to character to check
  \return true if character is blank, false otherwise
*/
[[nodiscard]] inline auto isblank(const char* p_check) -> bool {
    return (*p_check == ' ' || *p_check == '\t');
}

/// \copydoc isblank()
[[nodiscard]] inline auto isblank(std::string::const_iterator p_check) -> bool {
    return isblank(&*p_check);
}

/*!
  Return true if character is a decimal digit (0-9)
  \param p_check pointer to character to check
  \return true if character is a digit, false otherwise
*/
[[nodiscard]] inline auto isdigit(const char* p_check) -> bool {
    char c = *p_check;
    return '0' <= c && c <= '9';
}

/// \copydoc isdigit()
[[nodiscard]] inline auto isdigit(std::string::const_iterator p_check) -> bool {
    return isdigit(&*p_check);
}

/*!
  Return true if character is an alphanumeric character (0-9 or A-Z or a-z)
  \param p_check pointer to character to check
  \return true if character is alphanumeric, false otherwise
*/
[[nodiscard]] inline auto isalnum(const char* p_check) -> bool {
    char c = *p_check;
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

/// \copydoc isalnum()
[[nodiscard]] inline auto isalnum(std::string::const_iterator p_check) -> bool {
    return isalnum(&*p_check);
}

/*!
  Return true if character is an alphabetic character (A-Z or a-z)
  \param p_check pointer to character to check
  \return true if character is alphabetic, false otherwise
*/
[[nodiscard]] inline auto isalpha(const char* p_check) -> bool {
    char c = *p_check;
    return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

/// \copydoc isalpha()
[[nodiscard]] inline auto isalpha(std::string::const_iterator p_check) -> bool {
    return isalpha(&*p_check);
}

/*!
  Return true if character is a hexadecimal digit (0-9 or A-F or a-f)
  \param p_check pointer to character to check
  \return true if character is hexadecimal, false otherwise
*/
[[nodiscard]] inline auto isxdigit(const char* p_check) -> bool {
    char c = *p_check;
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F') || ('a' <= c && c <= 'f');
}

/// \copydoc isxdigit()
[[nodiscard]] inline auto isxdigit(std::string::const_iterator p_check) -> bool {
    return isxdigit(&*p_check);
}

/// \copydoc isupper()
[[nodiscard]] inline auto isupper(std::string::const_iterator p_check) -> bool {
    return isupper(&*p_check);
}

/// \copydoc islower()
[[nodiscard]] inline auto islower(std::string::const_iterator p_check) -> bool {
    return isupper(&*p_check);
}

/*!
  Creates, modifies, or removes environment variables.
  This is a wrapper for [_wputenv](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-wputenv)
  function.

  \param str environment string to modify
  \return true if successful, false otherwise.
*/
[[nodiscard]] inline auto putenv(std::string const& str) -> bool {
    return (_wputenv(utf8::widen(str).c_str()) == 0);
}

/*!
  Creates, modifies, or removes environment variables.
  This is a wrapper for [_wputenv_s](https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/putenv-s-wputenv-s)
  function.

  \param var  name of environment variable
  \param val  new value of environment variable. If empty, the environment
              variable is removed
  \return true if successful, false otherwise
*/
[[nodiscard]] inline auto putenv(std::string const& var, std::string const& val) -> bool {
    return (_wputenv_s(widen(var).c_str(), widen(val).c_str()) == 0);
}

/*!
  Passes command to command interpreter

  \param cmd UTF-8 encoded command to pass

  This is a wrapper for [_wsystem](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/system-wsystem)
  function.
*/
[[nodiscard]] inline auto system(std::string const& cmd) -> int32_t {
    std::wstring wcmd = utf8::widen(cmd);
    return _wsystem(wcmd.c_str());
}


}; // namespace utf8


//
//
////   end header file   /////////////////////////////////////////////////////
#endif // INCLUDE_UTF8_HPP_

#define UTF8_IMPLEMENTATION

////   start impementanion fle   /////////////////////////////////////////////////////
//
//
#ifdef UTF8_IMPLEMENTATION


namespace utf8 {

static void encode(char32_t input_char, std::string& input_s);

/*!
  \defgroup basecvt Narrowing/Widening Functions
  Basic conversion functions between UTF-8, UTF-16 and UTF-32
*/

/*!
  Conversion from wide character to UTF-8

  \param  input_s   input string
  \param  nch number of character to convert or 0 if string is null-terminated
  \return UTF-8 character string
*/
[[nodiscard]] auto narrow(const wchar_t* input_s, size_t nch) -> std::string {
    int32_t nsz{ WideCharToMultiByte(CP_UTF8, 0, input_s, (nch != 0U ? static_cast<int>(nch) : -1), nullptr, 0, nullptr, nullptr) };
    if ((input_s == nullptr) || (nsz == 0)) {
        return std::string();
    }

    auto out{ std::string(static_cast<size_t>(nsz), 0) };
    WideCharToMultiByte(CP_UTF8, 0, input_s, -1, out.data(), nsz, nullptr, nullptr);
    if (nch == 0U) {
        out.resize(static_cast<size_t>(nsz - 1)); // output is null-terminated
    }
    return out;
}

/*!
  Conversion from wide character to UTF-8

  \param  input_s input string
  \return UTF-8 character string
*/
[[nodiscard]] auto narrow(std::wstring const& input_s) -> std::string {
    int32_t nsz{ WideCharToMultiByte(CP_UTF8, 0, input_s.c_str(), static_cast<int>(input_s.size()), nullptr, 0, nullptr, nullptr) };
    if (nsz == 0) {
        return std::string();
    }
    auto out{ std::string(static_cast<size_t>(nsz), 0) };
    WideCharToMultiByte(CP_UTF8, 0, input_s.c_str(), static_cast<int>(input_s.size()), out.data(), static_cast<int>(nsz), nullptr, nullptr);
    return out;
}

/*!
  Conversion from UTF32 to UTF8
  \param input_s UTF-32 encoded string
  \param  nch number of character to convert or 0 if string is null-terminated
  \return UTF-8 encoded string

  Each character in the input string should be a valid UTF-32 code point
  ( <0x10FFFF)
*/
[[nodiscard]] auto narrow(const char32_t* input_s, size_t nch) -> std::string {
    auto str{ std::string() };
    const char32_t* p{ input_s };
    if (nch == 0U) {
        // null terminated; count characters now
        while (*p++) {
            nch++;
        }
        p = input_s;
    }

    for (; nch != 0U; nch--, ++p) {
        assert(*p < 0x10ffff);
        encode(*p, str);
    }
    return str;
}

/*!
  Conversion from UTF32 to UTF8
  \param input_s UTF-32 encoded string
  \return UTF-8 encoded string

  Each character in the input string should be a valid UTF-32 code point
  ( <0x10FFFF)
*/
[[nodiscard]] auto narrow(std::u32string const& input_s) -> std::string {
    auto str{ std::string() };
    for (auto p = input_s.begin(); p != input_s.end(); ++p) {
        assert(*p < 0x10ffff);
        encode(*p, str);
    }
    return str;
}

/*!
  Conversion from UTF32 to UTF8
  \param input_r UTF-32 encoded character
  \return UTF-8 encoded string

  Input parameter must be a valid UTF-32 code point
  ( <0x10FFFF)
*/
[[nodiscard]] auto narrow(const char32_t input_r) -> std::string {
    assert(input_r < 0x10ffff);
    auto str{ std::string() };
    encode(input_r, str);
    return str;
}

/*!
  Conversion from UTF-8 to wide character

  \param  input_s input string
  \param nch number of characters to convert or 0 if string is null-terminated
  \return wide character string
*/
[[nodiscard]] auto widen(const char* input_s, size_t nch) -> std::wstring {
    int32_t wsz{ MultiByteToWideChar(CP_UTF8, 0, input_s, (nch != 0U ? static_cast<int>(nch) : -1), nullptr, 0) };
    if ((input_s == nullptr) || (wsz == 0)) {
        return std::wstring();
    }

    auto out{ std::wstring(static_cast<size_t>(wsz), 0) };
    MultiByteToWideChar(CP_UTF8, 0, input_s, -1, out.data(), wsz);
    if (nch == 0U) {
        out.resize(static_cast<size_t>(wsz - 1)); // output is null-terminated
    }
    return out;
}

/*!
  Conversion from UTF-8 to wide character

  \param  input_s input string
  \return wide character string
*/
[[nodiscard]] auto widen(std::string const& input_s) -> std::wstring {
    int32_t wsz{ MultiByteToWideChar(CP_UTF8, 0, input_s.c_str(), static_cast<int>(input_s.size()), nullptr, 0) };
    if (wsz == 0) {
        return std::wstring();
    }

    auto out{ std::wstring(static_cast<size_t>(wsz), 0) };
    MultiByteToWideChar(CP_UTF8, 0, input_s.c_str(), static_cast<int>(input_s.size()), out.data(), static_cast<int>(wsz));
    return out;
}

/*!
  Conversion from UTF-8 to UTF-32

  \param input_s UTF-8 encoded string
  \param nch number of characters to convert or 0 if string is null-terminated
  \return UTF-32 encoded string

  The function throws an exception if it encounters an invalid UTF-8 encoding.
*/
[[nodiscard]] auto runes(const char* input_s, size_t nch) -> std::u32string {
    std::u32string str;
    if (nch == 0U) {
        nch = strlen(input_s);
    }

    const char* end = input_s + nch;

    while (input_s < end) {
        char32_t r = next(input_s);
        if (r == REPLACEMENT_CHARACTER) {
            throw exception(exception::reason::invalid_utf8);
        }
        str.push_back(r);
    }
    return str;
}

/*!
  Converts a string of characters from UTF-8 to UTF-32

  \param input_s UTF-8 encoded string
  \return UTF-32 encoded string

  The function throws an exception if it encounters an invalid UTF-8 encoding.
*/
[[nodiscard]] auto runes(std::string const& input_s) -> std::u32string {
    std::u32string str;
    auto ptr = input_s.cbegin();
    while (ptr != input_s.cend()) {
        char32_t r = next(ptr, input_s.cend());
        if (r != REPLACEMENT_CHARACTER) {
            str.push_back(r);
        }
        else {
            throw exception(exception::reason::invalid_utf8);
        }
    }
    return str;
}


/*!
  Verifies if string is a valid UTF-8 string

  \param input_s pointer to character string to verify
  \param nch number of characters to verify or 0 if string is null-terminated
  \return `true` if string is a valid UTF-8 encoded string, `false` otherwise
*/
[[nodiscard]] auto valid_str(const char* input_s, size_t nch) -> bool {
    if (nch == 0U) {
        nch = strlen(input_s);
    }

    const char* last = input_s + nch;
    while (input_s < last) {
        if (next(input_s) == REPLACEMENT_CHARACTER) {
            return false;
        }
    }
    return (input_s == last);
}

/*!
  Decodes a UTF-8 encoded character and advances iterator to next code point

  \param ptr    Reference to iterator to be advanced
  \param last   Iterator pointing to the end of range
  \return       decoded character

  If the string contains an invalid UTF-8 encoding, the function returns
  REPLACEMENT_CHARACTER (0xfffd) and advances pointer to beginning of next
  character or end of string.
*/
[[nodiscard]] auto next(std::string::const_iterator& ptr, const std::string::const_iterator last) -> char32_t {
    char32_t rune = 0;
    if (ptr == last) {
        return REPLACEMENT_CHARACTER;
    }

    if ((*ptr & 0x80) == 0) {
        return static_cast<char32_t>(*ptr++);
    }
    else if ((*ptr & 0xC0) == 0x80) {
        rune = REPLACEMENT_CHARACTER;
        do {
            ++ptr;
        }
        while (ptr != last && (*ptr & 0x80) == 0x80);
    }
    else {
        size_t cont = 0;
        if ((*ptr & 0xE0) == 0xC0) {
            cont = 1;
            rune = static_cast<char32_t>(*ptr++ & 0x1f);
        }
        else if ((*ptr & 0xF0) == 0xE0) {
            cont = 2;
            rune = static_cast<char32_t>(*ptr++ & 0x0f);
        }
        else if ((*ptr & 0xF8) == 0xF0) {
            cont = 3;
            rune = static_cast<char32_t>(*ptr++ & 0x07);
        }
        else {
            do {
                ++ptr;
            }
            while (ptr != last && (*ptr & 0xC0) == 0x80);
            return REPLACEMENT_CHARACTER; // code points > U+0x10FFFF are invalid
        }
        size_t i;
        for (i = 0; i < cont && ptr != last && (*ptr & 0xC0) == 0x80; i++) {
            rune <<= 6;
            rune += *ptr++ & 0x3f;
        }

        // sanity checks
        if (i != cont) {
            return REPLACEMENT_CHARACTER; // short encoding
        }
        if (0xD800 <= rune && rune <= 0xdfff) {
            return REPLACEMENT_CHARACTER; // surrogates (U+D000 to U+DFFF) are invalid
        }

        if (rune < 0x80 || (cont > 1 && rune < 0x800) || (cont > 2 && rune < 0x10000)) {
            return REPLACEMENT_CHARACTER; // overlong encoding
        }
    }
    return rune;
}

/*!
  Decodes a UTF-8 encoded character and Advances pointer to next character

  \param ptr    <b>Reference</b> to character pointer to be advanced
  \return       decoded character

  If the string contains an invalid UTF-8 encoding, the function returns
  REPLACEMENT_CHARACTER (0xfffd) and advances pointer to beginning of next
  character or end of string.
*/
[[nodiscard]] auto next(const char*& ptr) -> char32_t {
    char32_t rune = 0;
    if ((*ptr & 0x80) == 0) {
        if ((rune = static_cast<char32_t>(*ptr)) != 0) {
            ++ptr;
        }
    }
    else if ((*ptr & 0xC0) == 0x80) {
        rune = REPLACEMENT_CHARACTER;
        do {
            ptr++;
        }
        while (*ptr && (*ptr & 0x80) == 0x80);
    }
    else {
        size_t cont = 0;
        if ((*ptr & 0xE0) == 0xC0) {
            cont = 1;
            rune = static_cast<char32_t>(*ptr++ & 0x1f);
        }
        else if ((*ptr & 0xF0) == 0xE0) {
            cont = 2;
            rune = static_cast<char32_t>(*ptr++ & 0x0f);
        }
        else if ((*ptr & 0xF8) == 0xF0) {
            cont = 3;
            rune = static_cast<char32_t>(*ptr++ & 0x07);
        }
        else {
            do {
                ptr++;
            }
            while (*ptr && (*ptr & 0xC0) == 0x80);
            return REPLACEMENT_CHARACTER; // code points > U+0x10FFFF are invalid
        }
        size_t i;
        for (i = 0; i < cont && (*ptr & 0xC0) == 0x80; i++) {
            rune <<= 6;
            rune += *ptr++ & 0x3f;
        }

        // sanity checks
        if (i != cont) {
            return REPLACEMENT_CHARACTER; // short encoding
        }
        if (0xD800 <= rune && rune <= 0xdfff) {
            return REPLACEMENT_CHARACTER; // surrogates (U+D000 to U+DFFF) are invalid
        }

        if (rune < 0x80 || (cont > 1 && rune < 0x800) || (cont > 2 && rune < 0x10000)) {
            return REPLACEMENT_CHARACTER; // overlong encoding
        }
    }
    return rune;
}

/*!
  Decrements a character pointer to previous UTF-8 character

  \param ptr    <b>Reference</b> to character pointer to be decremented
  \return       previous UTF-8 encoded character

  If the string contains an invalid UTF-8 encoding, the function returns
  REPLACEMENT_CHARACTER (0xfffd) and pointer remains unchanged.
*/
[[nodiscard]] auto prev(const char*& ptr) -> char32_t {
    int32_t cont{};
    const char* in_ptr = ptr;
    char32_t rune{};
    uint8_t ch{};
    while (((ch = static_cast<uint8_t>(*--ptr)) & 0xc0) == 0x80 && cont < 3) {
        rune += static_cast<char32_t>(ch & 0x3f) << cont++ * 6;
    }
    if (cont == 3 && (ch & 0xF0) == 0xF0) {
        rune += static_cast<char32_t>(ch & 0x0f) << 18;
    }
    else if (cont == 2 && (ch & 0xE0) == 0xE0) {
        rune += static_cast<char32_t>(ch & 0x1f) << 12;
    }
    else if (cont == 1 && (ch & 0xC0) == 0xC0) {
        rune += static_cast<char32_t>(ch & 0x3f) << 6;
    }
    else if (cont == 0 && ch < 0x7f) {
        rune += ch;
    }
    else {
        ptr = in_ptr;
        return REPLACEMENT_CHARACTER;
    }

    if ((0xD800 <= rune && rune <= 0xdfff) // surrogate
        || (cont >= 0 && rune < 0x80) || (cont >= 1 && rune < 0x800) || (cont >= 2 && rune < 0x10000)) // overlong encoding
    {
        ptr = in_ptr;
        return REPLACEMENT_CHARACTER;
    }

    return rune;
}

/*!
  Decrements an iterator to previous UTF-8 character

  \param ptr    iterator to be decremented
  \param first  iterator pointing to beginning of string
  \return       previous UTF-8 encoded character

  If the string contains an invalid UTF-8 encoding, the function returns
  REPLACEMENT_CHARACTER (0xfffd) and iterator remains unchanged.
*/
[[nodiscard]] auto prev(std::string::const_iterator& ptr, const std::string::const_iterator first) -> char32_t {
    int32_t cont{};
    auto in_ptr = ptr;
    char32_t rune{};
    uint8_t ch{};
    while (((ch = static_cast<uint8_t>(*--ptr)) & 0xc0) == 0x80 && cont < 3 && ptr > first) {
        rune += static_cast<char32_t>(ch & 0x3f) << cont++ * 6;
    }
    if (cont == 3 && (ch & 0xF0) == 0xF0) {
        rune += static_cast<char32_t>(ch & 0x0f) << 18;
    }
    else if (cont == 2 && (ch & 0xE0) == 0xE0) {
        rune += static_cast<char32_t>(ch & 0x1f) << 12;
    }
    else if (cont == 1 && (ch & 0xC0) == 0xC0) {
        rune += static_cast<char32_t>(ch & 0x3f) << 6;
    }
    else if (cont == 0 && ch < 0x7f) {
        rune += ch;
    }
    else {
        ptr = in_ptr;
        return REPLACEMENT_CHARACTER;
    }

    if ((0xD800 <= rune && rune <= 0xdfff) // surrogate
        || (cont >= 0 && rune < 0x80) || (cont >= 1 && rune < 0x800) || (cont >= 2 && rune < 0x10000)) // overlong encoding
    {
        ptr = in_ptr;
        return REPLACEMENT_CHARACTER;
    }

    return rune;
}

/*!
  Counts number of characters in an UTF8 encoded string

  \param input_s UTF8-encoded string
  \return number of characters in string

  \note Algorithm from http://canonical.org/~kragen/strlen-utf8.html
*/
[[nodiscard]] auto length(std::string const& input_s) -> size_t {
    size_t nc = 0;
    auto p = input_s.begin();
    while (p != input_s.end()) {
        if ((*p++ & 0xC0) != 0x80) {
            nc++;
        }
    }
    return nc;
}

/// \copydoc utf8::length()
[[nodiscard]] auto length(const char* input_s) -> size_t {
    size_t nc = 0;
    while (*input_s) {
        if ((*input_s++ & 0xC0) != 0x80) {
            nc++;
        }
    }
    return nc;
}

/*!
  Gets the current working directory
  \return UTF-8 encoded name of working directory
*/
[[nodiscard]] auto getcwd() -> std::string {
    wchar_t tmp[_MAX_PATH];
    const wchar_t* get_curent_working_dir = _wgetcwd(tmp, _countof(tmp));
    if (get_curent_working_dir != nullptr) {
        return narrow(tmp);
    }
    else {
        return std::string();
    }
}

/*!
  Breaks a path name into components

  \param path   UTF-8 encoded full path
  \param drive  drive letter followed by colon (or NULL if not needed)
  \param dir    directory path (or NULL if not needed)
  \param fname  base filename (or NULL if not needed)
  \param ext    file extension including the leading period (.)
                (or NULL if not needed)
  \return       true if successful, false otherwise
  Returned strings are converted to UTF-8.
*/
[[nodiscard]] auto splitpath(std::string const& path, char* drive, char* dir, char* fname, char* ext) -> bool {
    std::wstring wpath = widen(path);
    wchar_t wdrive[_MAX_DRIVE];
    wchar_t wdir[_MAX_DIR];
    wchar_t wfname[_MAX_FNAME];
    wchar_t wext[_MAX_EXT];
    const auto split_path_safe = _wsplitpath_s(wpath.c_str(), wdrive, wdir, wfname, wext);
    if (split_path_safe != 0) {
        return false;
    }

    if (drive != nullptr) {
        strncpy_s(drive, _MAX_DRIVE, narrow(wdrive).c_str(), _MAX_DRIVE - 1);
    }
    if (dir != nullptr) {
        strncpy_s(dir, _MAX_DIR, narrow(wdir).c_str(), _MAX_DIR - 1);
    }
    if (fname != nullptr) {
        strncpy_s(fname, _MAX_FNAME, narrow(wfname).c_str(), _MAX_FNAME - 1);
    }
    if (ext != nullptr) {
        strncpy_s(ext, _MAX_EXT, narrow(wext).c_str(), _MAX_EXT - 1);
    }

    return true;
}

/*!
  Breaks a path name into components

  \param path   UTF-8 encoded full path
  \param drive  drive letter followed by colon
  \param dir    directory path
  \param fname  base filename
  \param ext    file extension including the leading period (.)

  Returned strings are converted to UTF-8.
*/
[[nodiscard]] auto splitpath(std::string const& path, std::string& drive, std::string& dir, std::string& fname, std::string& ext) -> bool {
    std::wstring wpath = widen(path);
    wchar_t wdrive[_MAX_DRIVE];
    wchar_t wdir[_MAX_DIR];
    wchar_t wfname[_MAX_FNAME];
    wchar_t wext[_MAX_EXT];

    const auto split_path_safe = _wsplitpath_s(wpath.c_str(), wdrive, wdir, wfname, wext);
    if (split_path_safe != 0) {
        return false;
    }

    drive = narrow(wdrive);
    dir = narrow(wdir);
    fname = narrow(wfname);
    ext = narrow(wext);
    return true;
}

/*!
  Creates a path from UTF-8 encoded components.

  \param path   Resulting path (UTF-8 encoded)
  \param drive  drive letter
  \param dir    directory path
  \param fname  filename
  \param ext    extension
  \return       True if successful; false otherwise

  If any required syntactic element (colon after drive letter, '\' at end of
  directory path, colon before extension) is missing, it is automatically added.
*/
[[nodiscard]] auto makepath(std::string& path, std::string const& drive, std::string const& dir, std::string const& fname, std::string const& ext) -> bool {
    wchar_t wpath[_MAX_PATH];
    const auto make_path_safe = _wmakepath_s(wpath, widen(drive).c_str(), widen(dir).c_str(), widen(fname).c_str(), widen(ext).c_str());
    if (make_path_safe != 0) {
        return false;
    }

    path = narrow(wpath);
    return true;
}

/*!
  Returns the absolute (full) path of a filename
  \param relpath relative path
*/
[[nodiscard]] auto fullpath(std::string const& relpath) -> std::string {
    wchar_t wpath[_MAX_PATH];
    const wchar_t* full_path = _wfullpath(wpath, widen(relpath).c_str(), _MAX_PATH);
    if (full_path != nullptr) {
        return narrow(wpath);
    }
    return std::string();
}

/*!
  Retrieves the value of an environment variable
  \param  var name of environment variable
  \return value of environment variable or an empty string if there is no such
          environment variable
*/
[[nodiscard]] auto getenv(std::string const& var) -> std::string {
    size_t nsz{};
    std::wstring wvar = widen(var);
    _wgetenv_s(&nsz, 0, 0, wvar.c_str());
    if (nsz == 0U) {
        return std::string();
    }

    std::wstring wval(nsz, L'\0');
    _wgetenv_s(&nsz, wval.data(), nsz, wvar.c_str());
    wval.resize(nsz - 1);
    return narrow(wval);
}

/*!
  Converts wide byte command arguments to an array of pointers
  to UTF-8 strings.

  \param  argc Pointer to an integer that contains number of parameters
  \return array of pointers to each command line parameter or NULL if an error
  occurred.

  The space allocated for strings and array of pointers should be freed
  by calling free_utf8argv()
*/
[[nodiscard]] auto get_argv(int* argc) -> char** {
    char** uargv = nullptr;
    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), argc);
    if (wargv != nullptr) {
        uargv = new char*[static_cast<size_t>(*argc)];
        for (int32_t i = 0; i < *argc; i++) {
            int32_t nc = WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, 0, 0, 0, 0);
            uargv[i] = new char[static_cast<size_t>(nc + 1)];
            WideCharToMultiByte(CP_UTF8, 0, wargv[i], -1, uargv[i], nc, 0, 0);
        }
        LocalFree(wargv);
    }
    return uargv;
}

/*!
  Frees the memory allocated by get_argv(int *argc)

  \param  argc  number of arguments
  \param  argv  array of pointers to arguments
*/
void free_argv(int32_t argc, char** argv) {
    for (int32_t i = 0; i < argc; i++) {
        delete argv[i];
    }
    delete argv;
}

/*!
  Converts wide byte command arguments to UTF-8 to a vector of UTF-8 strings.

  \return vector of UTF-8 strings. The vector is empty if an error occurred.
*/
[[nodiscard]] auto get_argv() -> std::vector<std::string> {
    int32_t argc{};
    std::vector<std::string> uargv;

    wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (wargv != nullptr) {
        for (int32_t i = 0; i < argc; i++) {
            uargv.push_back(narrow(wargv[i]));
        }
        LocalFree(wargv);
    }
    return uargv;
}

/*!
  \defgroup charclass Character Classification Functions
  Replacements for character classification functions.

  According to C standard, the [is...](https://en.cppreference.com/w/cpp/header/cctype)
  family of functions have undefined behavior if the argument is outside the
  range of printable characters. These replacement functions are well-behaved
  for any input string.

  The argument is a character pointer or a string iterator. Use them as in the
  following example:
\code
  //skip spaces in UTF-8 string
  string s{ u8" \xA0日本語" };
  auto p = s.begin ();
  int blanks = 0;
  while (p != s.end () && utf8::isspace (p))
  {
    blanks++;
    utf8::next (p, s.end ());
  }
  assert (blanks == 2); //both space and "no-break space" are space characters
  //...
\endcode

*/

/*!
  Return true if character is blank(-ish).
  \param p_check pointer to character to check
  \return `true` if character is blank, `false` otherwise

  Returns `true` if Unicode character has the "White_Space=yes" property in the
  [Unicode Character Database](https://www.unicode.org/Public/UCD/latest/ucd/PropList.txt)
*/
[[nodiscard]] auto isspace(const char* p_check) -> bool {
    const char32_t spacetab[]{ 0x20,   0x09,   0x0d,   0x0a,   0x0b,   0x0c,   0x85,   0xA0,   0x1680, 0x2000, 0x2001, 0x2002,
                               0x2004, 0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x2028, 0x2020, 0x202f, 0x205f, 0x3000 };

    char32_t c = rune(p_check);
    for (auto i = 0; i < _countof(spacetab); i++) {
        if (c == spacetab[i]) {
            return true;
        }
    }

    return false;
}

// ----------------------- Low level internal functions -----------------------

/// Encode a character and append it to a string
void encode(char32_t input_char, std::string& input_s) {
    if (input_char < 0x7f) {
        input_s.push_back(static_cast<char>(input_char));
    }
    else if (input_char < 0x7ff) {
        input_s.push_back(static_cast<char>(0xC0 | input_char >> 6));
        input_s.push_back(static_cast<char>(0x80 | input_char & 0x3f));
    }
    else if (input_char < 0xFFFF) {
        if (input_char >= 0xD800 && input_char <= 0xdfff) {
            throw exception(exception::reason::invalid_char32);
        }
        input_s.push_back(static_cast<char>(0xE0 | input_char >> 12));
        input_s.push_back(static_cast<char>(0x80 | input_char >> 6 & 0x3f));
        input_s.push_back(static_cast<char>(0x80 | input_char & 0x3f));
    }
    else if (input_char < 0x10ffff) {
        input_s.push_back(static_cast<char>(0xF0 | input_char >> 18));
        input_s.push_back(static_cast<char>(0x80 | input_char >> 12 & 0x3f));
        input_s.push_back(static_cast<char>(0x80 | input_char >> 6 & 0x3f));
        input_s.push_back(static_cast<char>(0x80 | input_char & 0x3f));
    }
    else {
        throw exception(exception::reason::invalid_char32);
    }
}


/*!
  \class exception

  Most UTF8 functions will throw an exception if input string is not a valid
  encoding. So far there are two possible causes:
  - `invalid_utf8` if the string is not a valid UTF-8 encoding
  - `invalid_char32` if the string is not a valid UTF-32 codepoint.

  You can handle a utf8::exception using code like this:
\code

  //...
  catch (utf8::exception& e) {
    if (e.cause == utf8::exception::invalid_utf8) {
      // do something
    }
  }
\endcode
  or you can simply use the exception message:
\code
  //...
  catch (utf8::exception& e) {
    cout << e.what() << endl;
  }
\endcode

*/


/*!
  \defgroup folding Character Folding Functions
  Conversion between upper case and lower case letters.

  toupper() and tolower() functions and their in-place counterparts
  make_upper() and make_lower(), use standard tables published by Unicode
  Consortium to perform case folding.
  There is also a function, icompare(), that performs string comparison ignoring
  the case.

  If input strings are not valid UTF-8 encoded strings, these function will
  throw a utf8::exception.

  A small ancillary program (gen_casetab) converts the original table
  in two tables of equal size, one with the upper case letters and the other
  with the lower case ones. The upper case table is sorted to allow for
  binary searching. If a code is found in the upper case table, it is replaced
  with the matching code from the lower case.

  Case folding tables take about 22k. Finding a code takes at most 11 comparisons.
*/

// definition of 'u2l' and 'lc' tables
// Upper case table
static constexpr char32_t u2l[1454] = { 0x00041, //  LATIN CAPITAL LETTER A
                                        0x00042, //  LATIN CAPITAL LETTER B
                                        0x00043, //  LATIN CAPITAL LETTER C
                                        0x00044, //  LATIN CAPITAL LETTER D
                                        0x00045, //  LATIN CAPITAL LETTER E
                                        0x00046, //  LATIN CAPITAL LETTER F
                                        0x00047, //  LATIN CAPITAL LETTER G
                                        0x00048, //  LATIN CAPITAL LETTER H
                                        0x00049, //  LATIN CAPITAL LETTER I
                                        0x0004a, //  LATIN CAPITAL LETTER J
                                        0x0004b, //  LATIN CAPITAL LETTER K
                                        0x0004c, //  LATIN CAPITAL LETTER L
                                        0x0004d, //  LATIN CAPITAL LETTER M
                                        0x0004e, //  LATIN CAPITAL LETTER N
                                        0x0004f, //  LATIN CAPITAL LETTER O
                                        0x00050, //  LATIN CAPITAL LETTER P
                                        0x00051, //  LATIN CAPITAL LETTER Q
                                        0x00052, //  LATIN CAPITAL LETTER R
                                        0x00053, //  LATIN CAPITAL LETTER S
                                        0x00054, //  LATIN CAPITAL LETTER T
                                        0x00055, //  LATIN CAPITAL LETTER U
                                        0x00056, //  LATIN CAPITAL LETTER V
                                        0x00057, //  LATIN CAPITAL LETTER W
                                        0x00058, //  LATIN CAPITAL LETTER X
                                        0x00059, //  LATIN CAPITAL LETTER Y
                                        0x0005a, //  LATIN CAPITAL LETTER Z
                                        0x000b5, //  MICRO SIGN
                                        0x000c0, //  LATIN CAPITAL LETTER A WITH GRAVE
                                        0x000c1, //  LATIN CAPITAL LETTER A WITH ACUTE
                                        0x000c2, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX
                                        0x000c3, //  LATIN CAPITAL LETTER A WITH TILDE
                                        0x000c4, //  LATIN CAPITAL LETTER A WITH DIAERESIS
                                        0x000c5, //  LATIN CAPITAL LETTER A WITH RING ABOVE
                                        0x000c6, //  LATIN CAPITAL LETTER AE
                                        0x000c7, //  LATIN CAPITAL LETTER C WITH CEDILLA
                                        0x000c8, //  LATIN CAPITAL LETTER E WITH GRAVE
                                        0x000c9, //  LATIN CAPITAL LETTER E WITH ACUTE
                                        0x000ca, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX
                                        0x000cb, //  LATIN CAPITAL LETTER E WITH DIAERESIS
                                        0x000cc, //  LATIN CAPITAL LETTER I WITH GRAVE
                                        0x000cd, //  LATIN CAPITAL LETTER I WITH ACUTE
                                        0x000ce, //  LATIN CAPITAL LETTER I WITH CIRCUMFLEX
                                        0x000cf, //  LATIN CAPITAL LETTER I WITH DIAERESIS
                                        0x000d0, //  LATIN CAPITAL LETTER ETH
                                        0x000d1, //  LATIN CAPITAL LETTER N WITH TILDE
                                        0x000d2, //  LATIN CAPITAL LETTER O WITH GRAVE
                                        0x000d3, //  LATIN CAPITAL LETTER O WITH ACUTE
                                        0x000d4, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX
                                        0x000d5, //  LATIN CAPITAL LETTER O WITH TILDE
                                        0x000d6, //  LATIN CAPITAL LETTER O WITH DIAERESIS
                                        0x000d8, //  LATIN CAPITAL LETTER O WITH STROKE
                                        0x000d9, //  LATIN CAPITAL LETTER U WITH GRAVE
                                        0x000da, //  LATIN CAPITAL LETTER U WITH ACUTE
                                        0x000db, //  LATIN CAPITAL LETTER U WITH CIRCUMFLEX
                                        0x000dc, //  LATIN CAPITAL LETTER U WITH DIAERESIS
                                        0x000dd, //  LATIN CAPITAL LETTER Y WITH ACUTE
                                        0x000de, //  LATIN CAPITAL LETTER THORN
                                        0x00100, //  LATIN CAPITAL LETTER A WITH MACRON
                                        0x00102, //  LATIN CAPITAL LETTER A WITH BREVE
                                        0x00104, //  LATIN CAPITAL LETTER A WITH OGONEK
                                        0x00106, //  LATIN CAPITAL LETTER C WITH ACUTE
                                        0x00108, //  LATIN CAPITAL LETTER C WITH CIRCUMFLEX
                                        0x0010a, //  LATIN CAPITAL LETTER C WITH DOT ABOVE
                                        0x0010c, //  LATIN CAPITAL LETTER C WITH CARON
                                        0x0010e, //  LATIN CAPITAL LETTER D WITH CARON
                                        0x00110, //  LATIN CAPITAL LETTER D WITH STROKE
                                        0x00112, //  LATIN CAPITAL LETTER E WITH MACRON
                                        0x00114, //  LATIN CAPITAL LETTER E WITH BREVE
                                        0x00116, //  LATIN CAPITAL LETTER E WITH DOT ABOVE
                                        0x00118, //  LATIN CAPITAL LETTER E WITH OGONEK
                                        0x0011a, //  LATIN CAPITAL LETTER E WITH CARON
                                        0x0011c, //  LATIN CAPITAL LETTER G WITH CIRCUMFLEX
                                        0x0011e, //  LATIN CAPITAL LETTER G WITH BREVE
                                        0x00120, //  LATIN CAPITAL LETTER G WITH DOT ABOVE
                                        0x00122, //  LATIN CAPITAL LETTER G WITH CEDILLA
                                        0x00124, //  LATIN CAPITAL LETTER H WITH CIRCUMFLEX
                                        0x00126, //  LATIN CAPITAL LETTER H WITH STROKE
                                        0x00128, //  LATIN CAPITAL LETTER I WITH TILDE
                                        0x0012a, //  LATIN CAPITAL LETTER I WITH MACRON
                                        0x0012c, //  LATIN CAPITAL LETTER I WITH BREVE
                                        0x0012e, //  LATIN CAPITAL LETTER I WITH OGONEK
                                        0x00132, //  LATIN CAPITAL LIGATURE IJ
                                        0x00134, //  LATIN CAPITAL LETTER J WITH CIRCUMFLEX
                                        0x00136, //  LATIN CAPITAL LETTER K WITH CEDILLA
                                        0x00139, //  LATIN CAPITAL LETTER L WITH ACUTE
                                        0x0013b, //  LATIN CAPITAL LETTER L WITH CEDILLA
                                        0x0013d, //  LATIN CAPITAL LETTER L WITH CARON
                                        0x0013f, //  LATIN CAPITAL LETTER L WITH MIDDLE DOT
                                        0x00141, //  LATIN CAPITAL LETTER L WITH STROKE
                                        0x00143, //  LATIN CAPITAL LETTER N WITH ACUTE
                                        0x00145, //  LATIN CAPITAL LETTER N WITH CEDILLA
                                        0x00147, //  LATIN CAPITAL LETTER N WITH CARON
                                        0x0014a, //  LATIN CAPITAL LETTER ENG
                                        0x0014c, //  LATIN CAPITAL LETTER O WITH MACRON
                                        0x0014e, //  LATIN CAPITAL LETTER O WITH BREVE
                                        0x00150, //  LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
                                        0x00152, //  LATIN CAPITAL LIGATURE OE
                                        0x00154, //  LATIN CAPITAL LETTER R WITH ACUTE
                                        0x00156, //  LATIN CAPITAL LETTER R WITH CEDILLA
                                        0x00158, //  LATIN CAPITAL LETTER R WITH CARON
                                        0x0015a, //  LATIN CAPITAL LETTER S WITH ACUTE
                                        0x0015c, //  LATIN CAPITAL LETTER S WITH CIRCUMFLEX
                                        0x0015e, //  LATIN CAPITAL LETTER S WITH CEDILLA
                                        0x00160, //  LATIN CAPITAL LETTER S WITH CARON
                                        0x00162, //  LATIN CAPITAL LETTER T WITH CEDILLA
                                        0x00164, //  LATIN CAPITAL LETTER T WITH CARON
                                        0x00166, //  LATIN CAPITAL LETTER T WITH STROKE
                                        0x00168, //  LATIN CAPITAL LETTER U WITH TILDE
                                        0x0016a, //  LATIN CAPITAL LETTER U WITH MACRON
                                        0x0016c, //  LATIN CAPITAL LETTER U WITH BREVE
                                        0x0016e, //  LATIN CAPITAL LETTER U WITH RING ABOVE
                                        0x00170, //  LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
                                        0x00172, //  LATIN CAPITAL LETTER U WITH OGONEK
                                        0x00174, //  LATIN CAPITAL LETTER W WITH CIRCUMFLEX
                                        0x00176, //  LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
                                        0x00178, //  LATIN CAPITAL LETTER Y WITH DIAERESIS
                                        0x00179, //  LATIN CAPITAL LETTER Z WITH ACUTE
                                        0x0017b, //  LATIN CAPITAL LETTER Z WITH DOT ABOVE
                                        0x0017d, //  LATIN CAPITAL LETTER Z WITH CARON
                                        0x0017f, //  LATIN SMALL LETTER LONG S
                                        0x00181, //  LATIN CAPITAL LETTER B WITH HOOK
                                        0x00182, //  LATIN CAPITAL LETTER B WITH TOPBAR
                                        0x00184, //  LATIN CAPITAL LETTER TONE SIX
                                        0x00186, //  LATIN CAPITAL LETTER OPEN O
                                        0x00187, //  LATIN CAPITAL LETTER C WITH HOOK
                                        0x00189, //  LATIN CAPITAL LETTER AFRICAN D
                                        0x0018a, //  LATIN CAPITAL LETTER D WITH HOOK
                                        0x0018b, //  LATIN CAPITAL LETTER D WITH TOPBAR
                                        0x0018e, //  LATIN CAPITAL LETTER REVERSED E
                                        0x0018f, //  LATIN CAPITAL LETTER SCHWA
                                        0x00190, //  LATIN CAPITAL LETTER OPEN E
                                        0x00191, //  LATIN CAPITAL LETTER F WITH HOOK
                                        0x00193, //  LATIN CAPITAL LETTER G WITH HOOK
                                        0x00194, //  LATIN CAPITAL LETTER GAMMA
                                        0x00196, //  LATIN CAPITAL LETTER IOTA
                                        0x00197, //  LATIN CAPITAL LETTER I WITH STROKE
                                        0x00198, //  LATIN CAPITAL LETTER K WITH HOOK
                                        0x0019c, //  LATIN CAPITAL LETTER TURNED M
                                        0x0019d, //  LATIN CAPITAL LETTER N WITH LEFT HOOK
                                        0x0019f, //  LATIN CAPITAL LETTER O WITH MIDDLE TILDE
                                        0x001a0, //  LATIN CAPITAL LETTER O WITH HORN
                                        0x001a2, //  LATIN CAPITAL LETTER OI
                                        0x001a4, //  LATIN CAPITAL LETTER P WITH HOOK
                                        0x001a6, //  LATIN LETTER YR
                                        0x001a7, //  LATIN CAPITAL LETTER TONE TWO
                                        0x001a9, //  LATIN CAPITAL LETTER ESH
                                        0x001ac, //  LATIN CAPITAL LETTER T WITH HOOK
                                        0x001ae, //  LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
                                        0x001af, //  LATIN CAPITAL LETTER U WITH HORN
                                        0x001b1, //  LATIN CAPITAL LETTER UPSILON
                                        0x001b2, //  LATIN CAPITAL LETTER V WITH HOOK
                                        0x001b3, //  LATIN CAPITAL LETTER Y WITH HOOK
                                        0x001b5, //  LATIN CAPITAL LETTER Z WITH STROKE
                                        0x001b7, //  LATIN CAPITAL LETTER EZH
                                        0x001b8, //  LATIN CAPITAL LETTER EZH REVERSED
                                        0x001bc, //  LATIN CAPITAL LETTER TONE FIVE
                                        0x001c4, //  LATIN CAPITAL LETTER DZ WITH CARON
                                        0x001c5, //  LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
                                        0x001c7, //  LATIN CAPITAL LETTER LJ
                                        0x001c8, //  LATIN CAPITAL LETTER L WITH SMALL LETTER J
                                        0x001ca, //  LATIN CAPITAL LETTER NJ
                                        0x001cb, //  LATIN CAPITAL LETTER N WITH SMALL LETTER J
                                        0x001cd, //  LATIN CAPITAL LETTER A WITH CARON
                                        0x001cf, //  LATIN CAPITAL LETTER I WITH CARON
                                        0x001d1, //  LATIN CAPITAL LETTER O WITH CARON
                                        0x001d3, //  LATIN CAPITAL LETTER U WITH CARON
                                        0x001d5, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
                                        0x001d7, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
                                        0x001d9, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
                                        0x001db, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
                                        0x001de, //  LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
                                        0x001e0, //  LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
                                        0x001e2, //  LATIN CAPITAL LETTER AE WITH MACRON
                                        0x001e4, //  LATIN CAPITAL LETTER G WITH STROKE
                                        0x001e6, //  LATIN CAPITAL LETTER G WITH CARON
                                        0x001e8, //  LATIN CAPITAL LETTER K WITH CARON
                                        0x001ea, //  LATIN CAPITAL LETTER O WITH OGONEK
                                        0x001ec, //  LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
                                        0x001ee, //  LATIN CAPITAL LETTER EZH WITH CARON
                                        0x001f1, //  LATIN CAPITAL LETTER DZ
                                        0x001f2, //  LATIN CAPITAL LETTER D WITH SMALL LETTER Z
                                        0x001f4, //  LATIN CAPITAL LETTER G WITH ACUTE
                                        0x001f6, //  LATIN CAPITAL LETTER HWAIR
                                        0x001f7, //  LATIN CAPITAL LETTER WYNN
                                        0x001f8, //  LATIN CAPITAL LETTER N WITH GRAVE
                                        0x001fa, //  LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
                                        0x001fc, //  LATIN CAPITAL LETTER AE WITH ACUTE
                                        0x001fe, //  LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
                                        0x00200, //  LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
                                        0x00202, //  LATIN CAPITAL LETTER A WITH INVERTED BREVE
                                        0x00204, //  LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
                                        0x00206, //  LATIN CAPITAL LETTER E WITH INVERTED BREVE
                                        0x00208, //  LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
                                        0x0020a, //  LATIN CAPITAL LETTER I WITH INVERTED BREVE
                                        0x0020c, //  LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
                                        0x0020e, //  LATIN CAPITAL LETTER O WITH INVERTED BREVE
                                        0x00210, //  LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
                                        0x00212, //  LATIN CAPITAL LETTER R WITH INVERTED BREVE
                                        0x00214, //  LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
                                        0x00216, //  LATIN CAPITAL LETTER U WITH INVERTED BREVE
                                        0x00218, //  LATIN CAPITAL LETTER S WITH COMMA BELOW
                                        0x0021a, //  LATIN CAPITAL LETTER T WITH COMMA BELOW
                                        0x0021c, //  LATIN CAPITAL LETTER YOGH
                                        0x0021e, //  LATIN CAPITAL LETTER H WITH CARON
                                        0x00220, //  LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
                                        0x00222, //  LATIN CAPITAL LETTER OU
                                        0x00224, //  LATIN CAPITAL LETTER Z WITH HOOK
                                        0x00226, //  LATIN CAPITAL LETTER A WITH DOT ABOVE
                                        0x00228, //  LATIN CAPITAL LETTER E WITH CEDILLA
                                        0x0022a, //  LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
                                        0x0022c, //  LATIN CAPITAL LETTER O WITH TILDE AND MACRON
                                        0x0022e, //  LATIN CAPITAL LETTER O WITH DOT ABOVE
                                        0x00230, //  LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
                                        0x00232, //  LATIN CAPITAL LETTER Y WITH MACRON
                                        0x0023a, //  LATIN CAPITAL LETTER A WITH STROKE
                                        0x0023b, //  LATIN CAPITAL LETTER C WITH STROKE
                                        0x0023d, //  LATIN CAPITAL LETTER L WITH BAR
                                        0x0023e, //  LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
                                        0x00241, //  LATIN CAPITAL LETTER GLOTTAL STOP
                                        0x00243, //  LATIN CAPITAL LETTER B WITH STROKE
                                        0x00244, //  LATIN CAPITAL LETTER U BAR
                                        0x00245, //  LATIN CAPITAL LETTER TURNED V
                                        0x00246, //  LATIN CAPITAL LETTER E WITH STROKE
                                        0x00248, //  LATIN CAPITAL LETTER J WITH STROKE
                                        0x0024a, //  LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL
                                        0x0024c, //  LATIN CAPITAL LETTER R WITH STROKE
                                        0x0024e, //  LATIN CAPITAL LETTER Y WITH STROKE
                                        0x00345, //  COMBINING GREEK YPOGEGRAMMENI
                                        0x00370, //  GREEK CAPITAL LETTER HETA
                                        0x00372, //  GREEK CAPITAL LETTER ARCHAIC SAMPI
                                        0x00376, //  GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
                                        0x0037f, //  GREEK CAPITAL LETTER YOT
                                        0x00386, //  GREEK CAPITAL LETTER ALPHA WITH TONOS
                                        0x00388, //  GREEK CAPITAL LETTER EPSILON WITH TONOS
                                        0x00389, //  GREEK CAPITAL LETTER ETA WITH TONOS
                                        0x0038a, //  GREEK CAPITAL LETTER IOTA WITH TONOS
                                        0x0038c, //  GREEK CAPITAL LETTER OMICRON WITH TONOS
                                        0x0038e, //  GREEK CAPITAL LETTER UPSILON WITH TONOS
                                        0x0038f, //  GREEK CAPITAL LETTER OMEGA WITH TONOS
                                        0x00391, //  GREEK CAPITAL LETTER ALPHA
                                        0x00392, //  GREEK CAPITAL LETTER BETA
                                        0x00393, //  GREEK CAPITAL LETTER GAMMA
                                        0x00394, //  GREEK CAPITAL LETTER DELTA
                                        0x00395, //  GREEK CAPITAL LETTER EPSILON
                                        0x00396, //  GREEK CAPITAL LETTER ZETA
                                        0x00397, //  GREEK CAPITAL LETTER ETA
                                        0x00398, //  GREEK CAPITAL LETTER THETA
                                        0x00399, //  GREEK CAPITAL LETTER IOTA
                                        0x0039a, //  GREEK CAPITAL LETTER KAPPA
                                        0x0039b, //  GREEK CAPITAL LETTER LAMDA
                                        0x0039c, //  GREEK CAPITAL LETTER MU
                                        0x0039d, //  GREEK CAPITAL LETTER NU
                                        0x0039e, //  GREEK CAPITAL LETTER XI
                                        0x0039f, //  GREEK CAPITAL LETTER OMICRON
                                        0x003a0, //  GREEK CAPITAL LETTER PI
                                        0x003a1, //  GREEK CAPITAL LETTER RHO
                                        0x003a3, //  GREEK CAPITAL LETTER SIGMA
                                        0x003a4, //  GREEK CAPITAL LETTER TAU
                                        0x003a5, //  GREEK CAPITAL LETTER UPSILON
                                        0x003a6, //  GREEK CAPITAL LETTER PHI
                                        0x003a7, //  GREEK CAPITAL LETTER CHI
                                        0x003a8, //  GREEK CAPITAL LETTER PSI
                                        0x003a9, //  GREEK CAPITAL LETTER OMEGA
                                        0x003aa, //  GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
                                        0x003ab, //  GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
                                        0x003c2, //  GREEK SMALL LETTER FINAL SIGMA
                                        0x003cf, //  GREEK CAPITAL KAI SYMBOL
                                        0x003d0, //  GREEK BETA SYMBOL
                                        0x003d1, //  GREEK THETA SYMBOL
                                        0x003d5, //  GREEK PHI SYMBOL
                                        0x003d6, //  GREEK PI SYMBOL
                                        0x003d8, //  GREEK LETTER ARCHAIC KOPPA
                                        0x003da, //  GREEK LETTER STIGMA
                                        0x003dc, //  GREEK LETTER DIGAMMA
                                        0x003de, //  GREEK LETTER KOPPA
                                        0x003e0, //  GREEK LETTER SAMPI
                                        0x003e2, //  COPTIC CAPITAL LETTER SHEI
                                        0x003e4, //  COPTIC CAPITAL LETTER FEI
                                        0x003e6, //  COPTIC CAPITAL LETTER KHEI
                                        0x003e8, //  COPTIC CAPITAL LETTER HORI
                                        0x003ea, //  COPTIC CAPITAL LETTER GANGIA
                                        0x003ec, //  COPTIC CAPITAL LETTER SHIMA
                                        0x003ee, //  COPTIC CAPITAL LETTER DEI
                                        0x003f0, //  GREEK KAPPA SYMBOL
                                        0x003f1, //  GREEK RHO SYMBOL
                                        0x003f4, //  GREEK CAPITAL THETA SYMBOL
                                        0x003f5, //  GREEK LUNATE EPSILON SYMBOL
                                        0x003f7, //  GREEK CAPITAL LETTER SHO
                                        0x003f9, //  GREEK CAPITAL LUNATE SIGMA SYMBOL
                                        0x003fa, //  GREEK CAPITAL LETTER SAN
                                        0x003fd, //  GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
                                        0x003fe, //  GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL
                                        0x003ff, //  GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL
                                        0x00400, //  CYRILLIC CAPITAL LETTER IE WITH GRAVE
                                        0x00401, //  CYRILLIC CAPITAL LETTER IO
                                        0x00402, //  CYRILLIC CAPITAL LETTER DJE
                                        0x00403, //  CYRILLIC CAPITAL LETTER GJE
                                        0x00404, //  CYRILLIC CAPITAL LETTER UKRAINIAN IE
                                        0x00405, //  CYRILLIC CAPITAL LETTER DZE
                                        0x00406, //  CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
                                        0x00407, //  CYRILLIC CAPITAL LETTER YI
                                        0x00408, //  CYRILLIC CAPITAL LETTER JE
                                        0x00409, //  CYRILLIC CAPITAL LETTER LJE
                                        0x0040a, //  CYRILLIC CAPITAL LETTER NJE
                                        0x0040b, //  CYRILLIC CAPITAL LETTER TSHE
                                        0x0040c, //  CYRILLIC CAPITAL LETTER KJE
                                        0x0040d, //  CYRILLIC CAPITAL LETTER I WITH GRAVE
                                        0x0040e, //  CYRILLIC CAPITAL LETTER SHORT U
                                        0x0040f, //  CYRILLIC CAPITAL LETTER DZHE
                                        0x00410, //  CYRILLIC CAPITAL LETTER A
                                        0x00411, //  CYRILLIC CAPITAL LETTER BE
                                        0x00412, //  CYRILLIC CAPITAL LETTER VE
                                        0x00413, //  CYRILLIC CAPITAL LETTER GHE
                                        0x00414, //  CYRILLIC CAPITAL LETTER DE
                                        0x00415, //  CYRILLIC CAPITAL LETTER IE
                                        0x00416, //  CYRILLIC CAPITAL LETTER ZHE
                                        0x00417, //  CYRILLIC CAPITAL LETTER ZE
                                        0x00418, //  CYRILLIC CAPITAL LETTER I
                                        0x00419, //  CYRILLIC CAPITAL LETTER SHORT I
                                        0x0041a, //  CYRILLIC CAPITAL LETTER KA
                                        0x0041b, //  CYRILLIC CAPITAL LETTER EL
                                        0x0041c, //  CYRILLIC CAPITAL LETTER EM
                                        0x0041d, //  CYRILLIC CAPITAL LETTER EN
                                        0x0041e, //  CYRILLIC CAPITAL LETTER O
                                        0x0041f, //  CYRILLIC CAPITAL LETTER PE
                                        0x00420, //  CYRILLIC CAPITAL LETTER ER
                                        0x00421, //  CYRILLIC CAPITAL LETTER ES
                                        0x00422, //  CYRILLIC CAPITAL LETTER TE
                                        0x00423, //  CYRILLIC CAPITAL LETTER U
                                        0x00424, //  CYRILLIC CAPITAL LETTER EF
                                        0x00425, //  CYRILLIC CAPITAL LETTER HA
                                        0x00426, //  CYRILLIC CAPITAL LETTER TSE
                                        0x00427, //  CYRILLIC CAPITAL LETTER CHE
                                        0x00428, //  CYRILLIC CAPITAL LETTER SHA
                                        0x00429, //  CYRILLIC CAPITAL LETTER SHCHA
                                        0x0042a, //  CYRILLIC CAPITAL LETTER HARD SIGN
                                        0x0042b, //  CYRILLIC CAPITAL LETTER YERU
                                        0x0042c, //  CYRILLIC CAPITAL LETTER SOFT SIGN
                                        0x0042d, //  CYRILLIC CAPITAL LETTER E
                                        0x0042e, //  CYRILLIC CAPITAL LETTER YU
                                        0x0042f, //  CYRILLIC CAPITAL LETTER YA
                                        0x00460, //  CYRILLIC CAPITAL LETTER OMEGA
                                        0x00462, //  CYRILLIC CAPITAL LETTER YAT
                                        0x00464, //  CYRILLIC CAPITAL LETTER IOTIFIED E
                                        0x00466, //  CYRILLIC CAPITAL LETTER LITTLE YUS
                                        0x00468, //  CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS
                                        0x0046a, //  CYRILLIC CAPITAL LETTER BIG YUS
                                        0x0046c, //  CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS
                                        0x0046e, //  CYRILLIC CAPITAL LETTER KSI
                                        0x00470, //  CYRILLIC CAPITAL LETTER PSI
                                        0x00472, //  CYRILLIC CAPITAL LETTER FITA
                                        0x00474, //  CYRILLIC CAPITAL LETTER IZHITSA
                                        0x00476, //  CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
                                        0x00478, //  CYRILLIC CAPITAL LETTER UK
                                        0x0047a, //  CYRILLIC CAPITAL LETTER ROUND OMEGA
                                        0x0047c, //  CYRILLIC CAPITAL LETTER OMEGA WITH TITLO
                                        0x0047e, //  CYRILLIC CAPITAL LETTER OT
                                        0x00480, //  CYRILLIC CAPITAL LETTER KOPPA
                                        0x0048a, //  CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
                                        0x0048c, //  CYRILLIC CAPITAL LETTER SEMISOFT SIGN
                                        0x0048e, //  CYRILLIC CAPITAL LETTER ER WITH TICK
                                        0x00490, //  CYRILLIC CAPITAL LETTER GHE WITH UPTURN
                                        0x00492, //  CYRILLIC CAPITAL LETTER GHE WITH STROKE
                                        0x00494, //  CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK
                                        0x00496, //  CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
                                        0x00498, //  CYRILLIC CAPITAL LETTER ZE WITH DESCENDER
                                        0x0049a, //  CYRILLIC CAPITAL LETTER KA WITH DESCENDER
                                        0x0049c, //  CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
                                        0x0049e, //  CYRILLIC CAPITAL LETTER KA WITH STROKE
                                        0x004a0, //  CYRILLIC CAPITAL LETTER BASHKIR KA
                                        0x004a2, //  CYRILLIC CAPITAL LETTER EN WITH DESCENDER
                                        0x004a4, //  CYRILLIC CAPITAL LIGATURE EN GHE
                                        0x004a6, //  CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK
                                        0x004a8, //  CYRILLIC CAPITAL LETTER ABKHASIAN HA
                                        0x004aa, //  CYRILLIC CAPITAL LETTER ES WITH DESCENDER
                                        0x004ac, //  CYRILLIC CAPITAL LETTER TE WITH DESCENDER
                                        0x004ae, //  CYRILLIC CAPITAL LETTER STRAIGHT U
                                        0x004b0, //  CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
                                        0x004b2, //  CYRILLIC CAPITAL LETTER HA WITH DESCENDER
                                        0x004b4, //  CYRILLIC CAPITAL LIGATURE TE TSE
                                        0x004b6, //  CYRILLIC CAPITAL LETTER CHE WITH DESCENDER
                                        0x004b8, //  CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
                                        0x004ba, //  CYRILLIC CAPITAL LETTER SHHA
                                        0x004bc, //  CYRILLIC CAPITAL LETTER ABKHASIAN CHE
                                        0x004be, //  CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER
                                        0x004c0, //  CYRILLIC LETTER PALOCHKA
                                        0x004c1, //  CYRILLIC CAPITAL LETTER ZHE WITH BREVE
                                        0x004c3, //  CYRILLIC CAPITAL LETTER KA WITH HOOK
                                        0x004c5, //  CYRILLIC CAPITAL LETTER EL WITH TAIL
                                        0x004c7, //  CYRILLIC CAPITAL LETTER EN WITH HOOK
                                        0x004c9, //  CYRILLIC CAPITAL LETTER EN WITH TAIL
                                        0x004cb, //  CYRILLIC CAPITAL LETTER KHAKASSIAN CHE
                                        0x004cd, //  CYRILLIC CAPITAL LETTER EM WITH TAIL
                                        0x004d0, //  CYRILLIC CAPITAL LETTER A WITH BREVE
                                        0x004d2, //  CYRILLIC CAPITAL LETTER A WITH DIAERESIS
                                        0x004d4, //  CYRILLIC CAPITAL LIGATURE A IE
                                        0x004d6, //  CYRILLIC CAPITAL LETTER IE WITH BREVE
                                        0x004d8, //  CYRILLIC CAPITAL LETTER SCHWA
                                        0x004da, //  CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
                                        0x004dc, //  CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
                                        0x004de, //  CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
                                        0x004e0, //  CYRILLIC CAPITAL LETTER ABKHASIAN DZE
                                        0x004e2, //  CYRILLIC CAPITAL LETTER I WITH MACRON
                                        0x004e4, //  CYRILLIC CAPITAL LETTER I WITH DIAERESIS
                                        0x004e6, //  CYRILLIC CAPITAL LETTER O WITH DIAERESIS
                                        0x004e8, //  CYRILLIC CAPITAL LETTER BARRED O
                                        0x004ea, //  CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
                                        0x004ec, //  CYRILLIC CAPITAL LETTER E WITH DIAERESIS
                                        0x004ee, //  CYRILLIC CAPITAL LETTER U WITH MACRON
                                        0x004f0, //  CYRILLIC CAPITAL LETTER U WITH DIAERESIS
                                        0x004f2, //  CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
                                        0x004f4, //  CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
                                        0x004f6, //  CYRILLIC CAPITAL LETTER GHE WITH DESCENDER
                                        0x004f8, //  CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
                                        0x004fa, //  CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK
                                        0x004fc, //  CYRILLIC CAPITAL LETTER HA WITH HOOK
                                        0x004fe, //  CYRILLIC CAPITAL LETTER HA WITH STROKE
                                        0x00500, //  CYRILLIC CAPITAL LETTER KOMI DE
                                        0x00502, //  CYRILLIC CAPITAL LETTER KOMI DJE
                                        0x00504, //  CYRILLIC CAPITAL LETTER KOMI ZJE
                                        0x00506, //  CYRILLIC CAPITAL LETTER KOMI DZJE
                                        0x00508, //  CYRILLIC CAPITAL LETTER KOMI LJE
                                        0x0050a, //  CYRILLIC CAPITAL LETTER KOMI NJE
                                        0x0050c, //  CYRILLIC CAPITAL LETTER KOMI SJE
                                        0x0050e, //  CYRILLIC CAPITAL LETTER KOMI TJE
                                        0x00510, //  CYRILLIC CAPITAL LETTER REVERSED ZE
                                        0x00512, //  CYRILLIC CAPITAL LETTER EL WITH HOOK
                                        0x00514, //  CYRILLIC CAPITAL LETTER LHA
                                        0x00516, //  CYRILLIC CAPITAL LETTER RHA
                                        0x00518, //  CYRILLIC CAPITAL LETTER YAE
                                        0x0051a, //  CYRILLIC CAPITAL LETTER QA
                                        0x0051c, //  CYRILLIC CAPITAL LETTER WE
                                        0x0051e, //  CYRILLIC CAPITAL LETTER ALEUT KA
                                        0x00520, //  CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK
                                        0x00522, //  CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK
                                        0x00524, //  CYRILLIC CAPITAL LETTER PE WITH DESCENDER
                                        0x00526, //  CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER
                                        0x00528, //  CYRILLIC CAPITAL LETTER EN WITH LEFT HOOK
                                        0x0052a, //  CYRILLIC CAPITAL LETTER DZZHE
                                        0x0052c, //  CYRILLIC CAPITAL LETTER DCHE
                                        0x0052e, //  CYRILLIC CAPITAL LETTER EL WITH DESCENDER
                                        0x00531, //  ARMENIAN CAPITAL LETTER AYB
                                        0x00532, //  ARMENIAN CAPITAL LETTER BEN
                                        0x00533, //  ARMENIAN CAPITAL LETTER GIM
                                        0x00534, //  ARMENIAN CAPITAL LETTER DA
                                        0x00535, //  ARMENIAN CAPITAL LETTER ECH
                                        0x00536, //  ARMENIAN CAPITAL LETTER ZA
                                        0x00537, //  ARMENIAN CAPITAL LETTER EH
                                        0x00538, //  ARMENIAN CAPITAL LETTER ET
                                        0x00539, //  ARMENIAN CAPITAL LETTER TO
                                        0x0053a, //  ARMENIAN CAPITAL LETTER ZHE
                                        0x0053b, //  ARMENIAN CAPITAL LETTER INI
                                        0x0053c, //  ARMENIAN CAPITAL LETTER LIWN
                                        0x0053d, //  ARMENIAN CAPITAL LETTER XEH
                                        0x0053e, //  ARMENIAN CAPITAL LETTER CA
                                        0x0053f, //  ARMENIAN CAPITAL LETTER KEN
                                        0x00540, //  ARMENIAN CAPITAL LETTER HO
                                        0x00541, //  ARMENIAN CAPITAL LETTER JA
                                        0x00542, //  ARMENIAN CAPITAL LETTER GHAD
                                        0x00543, //  ARMENIAN CAPITAL LETTER CHEH
                                        0x00544, //  ARMENIAN CAPITAL LETTER MEN
                                        0x00545, //  ARMENIAN CAPITAL LETTER YI
                                        0x00546, //  ARMENIAN CAPITAL LETTER NOW
                                        0x00547, //  ARMENIAN CAPITAL LETTER SHA
                                        0x00548, //  ARMENIAN CAPITAL LETTER VO
                                        0x00549, //  ARMENIAN CAPITAL LETTER CHA
                                        0x0054a, //  ARMENIAN CAPITAL LETTER PEH
                                        0x0054b, //  ARMENIAN CAPITAL LETTER JHEH
                                        0x0054c, //  ARMENIAN CAPITAL LETTER RA
                                        0x0054d, //  ARMENIAN CAPITAL LETTER SEH
                                        0x0054e, //  ARMENIAN CAPITAL LETTER VEW
                                        0x0054f, //  ARMENIAN CAPITAL LETTER TIWN
                                        0x00550, //  ARMENIAN CAPITAL LETTER REH
                                        0x00551, //  ARMENIAN CAPITAL LETTER CO
                                        0x00552, //  ARMENIAN CAPITAL LETTER YIWN
                                        0x00553, //  ARMENIAN CAPITAL LETTER PIWR
                                        0x00554, //  ARMENIAN CAPITAL LETTER KEH
                                        0x00555, //  ARMENIAN CAPITAL LETTER OH
                                        0x00556, //  ARMENIAN CAPITAL LETTER FEH
                                        0x010a0, //  GEORGIAN CAPITAL LETTER AN
                                        0x010a1, //  GEORGIAN CAPITAL LETTER BAN
                                        0x010a2, //  GEORGIAN CAPITAL LETTER GAN
                                        0x010a3, //  GEORGIAN CAPITAL LETTER DON
                                        0x010a4, //  GEORGIAN CAPITAL LETTER EN
                                        0x010a5, //  GEORGIAN CAPITAL LETTER VIN
                                        0x010a6, //  GEORGIAN CAPITAL LETTER ZEN
                                        0x010a7, //  GEORGIAN CAPITAL LETTER TAN
                                        0x010a8, //  GEORGIAN CAPITAL LETTER IN
                                        0x010a9, //  GEORGIAN CAPITAL LETTER KAN
                                        0x010aa, //  GEORGIAN CAPITAL LETTER LAS
                                        0x010ab, //  GEORGIAN CAPITAL LETTER MAN
                                        0x010ac, //  GEORGIAN CAPITAL LETTER NAR
                                        0x010ad, //  GEORGIAN CAPITAL LETTER ON
                                        0x010ae, //  GEORGIAN CAPITAL LETTER PAR
                                        0x010af, //  GEORGIAN CAPITAL LETTER ZHAR
                                        0x010b0, //  GEORGIAN CAPITAL LETTER RAE
                                        0x010b1, //  GEORGIAN CAPITAL LETTER SAN
                                        0x010b2, //  GEORGIAN CAPITAL LETTER TAR
                                        0x010b3, //  GEORGIAN CAPITAL LETTER UN
                                        0x010b4, //  GEORGIAN CAPITAL LETTER PHAR
                                        0x010b5, //  GEORGIAN CAPITAL LETTER KHAR
                                        0x010b6, //  GEORGIAN CAPITAL LETTER GHAN
                                        0x010b7, //  GEORGIAN CAPITAL LETTER QAR
                                        0x010b8, //  GEORGIAN CAPITAL LETTER SHIN
                                        0x010b9, //  GEORGIAN CAPITAL LETTER CHIN
                                        0x010ba, //  GEORGIAN CAPITAL LETTER CAN
                                        0x010bb, //  GEORGIAN CAPITAL LETTER JIL
                                        0x010bc, //  GEORGIAN CAPITAL LETTER CIL
                                        0x010bd, //  GEORGIAN CAPITAL LETTER CHAR
                                        0x010be, //  GEORGIAN CAPITAL LETTER XAN
                                        0x010bf, //  GEORGIAN CAPITAL LETTER JHAN
                                        0x010c0, //  GEORGIAN CAPITAL LETTER HAE
                                        0x010c1, //  GEORGIAN CAPITAL LETTER HE
                                        0x010c2, //  GEORGIAN CAPITAL LETTER HIE
                                        0x010c3, //  GEORGIAN CAPITAL LETTER WE
                                        0x010c4, //  GEORGIAN CAPITAL LETTER HAR
                                        0x010c5, //  GEORGIAN CAPITAL LETTER HOE
                                        0x010c7, //  GEORGIAN CAPITAL LETTER YN
                                        0x010cd, //  GEORGIAN CAPITAL LETTER AEN
                                        0x013f8, //  CHEROKEE SMALL LETTER YE
                                        0x013f9, //  CHEROKEE SMALL LETTER YI
                                        0x013fa, //  CHEROKEE SMALL LETTER YO
                                        0x013fb, //  CHEROKEE SMALL LETTER YU
                                        0x013fc, //  CHEROKEE SMALL LETTER YV
                                        0x013fd, //  CHEROKEE SMALL LETTER MV
                                        0x01c80, //  CYRILLIC SMALL LETTER ROUNDED VE
                                        0x01c81, //  CYRILLIC SMALL LETTER LONG-LEGGED DE
                                        0x01c82, //  CYRILLIC SMALL LETTER NARROW O
                                        0x01c83, //  CYRILLIC SMALL LETTER WIDE ES
                                        0x01c84, //  CYRILLIC SMALL LETTER TALL TE
                                        0x01c85, //  CYRILLIC SMALL LETTER THREE-LEGGED TE
                                        0x01c86, //  CYRILLIC SMALL LETTER TALL HARD SIGN
                                        0x01c87, //  CYRILLIC SMALL LETTER TALL YAT
                                        0x01c88, //  CYRILLIC SMALL LETTER UNBLENDED UK
                                        0x01c90, //  GEORGIAN MTAVRULI CAPITAL LETTER AN
                                        0x01c91, //  GEORGIAN MTAVRULI CAPITAL LETTER BAN
                                        0x01c92, //  GEORGIAN MTAVRULI CAPITAL LETTER GAN
                                        0x01c93, //  GEORGIAN MTAVRULI CAPITAL LETTER DON
                                        0x01c94, //  GEORGIAN MTAVRULI CAPITAL LETTER EN
                                        0x01c95, //  GEORGIAN MTAVRULI CAPITAL LETTER VIN
                                        0x01c96, //  GEORGIAN MTAVRULI CAPITAL LETTER ZEN
                                        0x01c97, //  GEORGIAN MTAVRULI CAPITAL LETTER TAN
                                        0x01c98, //  GEORGIAN MTAVRULI CAPITAL LETTER IN
                                        0x01c99, //  GEORGIAN MTAVRULI CAPITAL LETTER KAN
                                        0x01c9a, //  GEORGIAN MTAVRULI CAPITAL LETTER LAS
                                        0x01c9b, //  GEORGIAN MTAVRULI CAPITAL LETTER MAN
                                        0x01c9c, //  GEORGIAN MTAVRULI CAPITAL LETTER NAR
                                        0x01c9d, //  GEORGIAN MTAVRULI CAPITAL LETTER ON
                                        0x01c9e, //  GEORGIAN MTAVRULI CAPITAL LETTER PAR
                                        0x01c9f, //  GEORGIAN MTAVRULI CAPITAL LETTER ZHAR
                                        0x01ca0, //  GEORGIAN MTAVRULI CAPITAL LETTER RAE
                                        0x01ca1, //  GEORGIAN MTAVRULI CAPITAL LETTER SAN
                                        0x01ca2, //  GEORGIAN MTAVRULI CAPITAL LETTER TAR
                                        0x01ca3, //  GEORGIAN MTAVRULI CAPITAL LETTER UN
                                        0x01ca4, //  GEORGIAN MTAVRULI CAPITAL LETTER PHAR
                                        0x01ca5, //  GEORGIAN MTAVRULI CAPITAL LETTER KHAR
                                        0x01ca6, //  GEORGIAN MTAVRULI CAPITAL LETTER GHAN
                                        0x01ca7, //  GEORGIAN MTAVRULI CAPITAL LETTER QAR
                                        0x01ca8, //  GEORGIAN MTAVRULI CAPITAL LETTER SHIN
                                        0x01ca9, //  GEORGIAN MTAVRULI CAPITAL LETTER CHIN
                                        0x01caa, //  GEORGIAN MTAVRULI CAPITAL LETTER CAN
                                        0x01cab, //  GEORGIAN MTAVRULI CAPITAL LETTER JIL
                                        0x01cac, //  GEORGIAN MTAVRULI CAPITAL LETTER CIL
                                        0x01cad, //  GEORGIAN MTAVRULI CAPITAL LETTER CHAR
                                        0x01cae, //  GEORGIAN MTAVRULI CAPITAL LETTER XAN
                                        0x01caf, //  GEORGIAN MTAVRULI CAPITAL LETTER JHAN
                                        0x01cb0, //  GEORGIAN MTAVRULI CAPITAL LETTER HAE
                                        0x01cb1, //  GEORGIAN MTAVRULI CAPITAL LETTER HE
                                        0x01cb2, //  GEORGIAN MTAVRULI CAPITAL LETTER HIE
                                        0x01cb3, //  GEORGIAN MTAVRULI CAPITAL LETTER WE
                                        0x01cb4, //  GEORGIAN MTAVRULI CAPITAL LETTER HAR
                                        0x01cb5, //  GEORGIAN MTAVRULI CAPITAL LETTER HOE
                                        0x01cb6, //  GEORGIAN MTAVRULI CAPITAL LETTER FI
                                        0x01cb7, //  GEORGIAN MTAVRULI CAPITAL LETTER YN
                                        0x01cb8, //  GEORGIAN MTAVRULI CAPITAL LETTER ELIFI
                                        0x01cb9, //  GEORGIAN MTAVRULI CAPITAL LETTER TURNED GAN
                                        0x01cba, //  GEORGIAN MTAVRULI CAPITAL LETTER AIN
                                        0x01cbd, //  GEORGIAN MTAVRULI CAPITAL LETTER AEN
                                        0x01cbe, //  GEORGIAN MTAVRULI CAPITAL LETTER HARD SIGN
                                        0x01cbf, //  GEORGIAN MTAVRULI CAPITAL LETTER LABIAL SIGN
                                        0x01e00, //  LATIN CAPITAL LETTER A WITH RING BELOW
                                        0x01e02, //  LATIN CAPITAL LETTER B WITH DOT ABOVE
                                        0x01e04, //  LATIN CAPITAL LETTER B WITH DOT BELOW
                                        0x01e06, //  LATIN CAPITAL LETTER B WITH LINE BELOW
                                        0x01e08, //  LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
                                        0x01e0a, //  LATIN CAPITAL LETTER D WITH DOT ABOVE
                                        0x01e0c, //  LATIN CAPITAL LETTER D WITH DOT BELOW
                                        0x01e0e, //  LATIN CAPITAL LETTER D WITH LINE BELOW
                                        0x01e10, //  LATIN CAPITAL LETTER D WITH CEDILLA
                                        0x01e12, //  LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
                                        0x01e14, //  LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
                                        0x01e16, //  LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
                                        0x01e18, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
                                        0x01e1a, //  LATIN CAPITAL LETTER E WITH TILDE BELOW
                                        0x01e1c, //  LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
                                        0x01e1e, //  LATIN CAPITAL LETTER F WITH DOT ABOVE
                                        0x01e20, //  LATIN CAPITAL LETTER G WITH MACRON
                                        0x01e22, //  LATIN CAPITAL LETTER H WITH DOT ABOVE
                                        0x01e24, //  LATIN CAPITAL LETTER H WITH DOT BELOW
                                        0x01e26, //  LATIN CAPITAL LETTER H WITH DIAERESIS
                                        0x01e28, //  LATIN CAPITAL LETTER H WITH CEDILLA
                                        0x01e2a, //  LATIN CAPITAL LETTER H WITH BREVE BELOW
                                        0x01e2c, //  LATIN CAPITAL LETTER I WITH TILDE BELOW
                                        0x01e2e, //  LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
                                        0x01e30, //  LATIN CAPITAL LETTER K WITH ACUTE
                                        0x01e32, //  LATIN CAPITAL LETTER K WITH DOT BELOW
                                        0x01e34, //  LATIN CAPITAL LETTER K WITH LINE BELOW
                                        0x01e36, //  LATIN CAPITAL LETTER L WITH DOT BELOW
                                        0x01e38, //  LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
                                        0x01e3a, //  LATIN CAPITAL LETTER L WITH LINE BELOW
                                        0x01e3c, //  LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
                                        0x01e3e, //  LATIN CAPITAL LETTER M WITH ACUTE
                                        0x01e40, //  LATIN CAPITAL LETTER M WITH DOT ABOVE
                                        0x01e42, //  LATIN CAPITAL LETTER M WITH DOT BELOW
                                        0x01e44, //  LATIN CAPITAL LETTER N WITH DOT ABOVE
                                        0x01e46, //  LATIN CAPITAL LETTER N WITH DOT BELOW
                                        0x01e48, //  LATIN CAPITAL LETTER N WITH LINE BELOW
                                        0x01e4a, //  LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
                                        0x01e4c, //  LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
                                        0x01e4e, //  LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
                                        0x01e50, //  LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
                                        0x01e52, //  LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
                                        0x01e54, //  LATIN CAPITAL LETTER P WITH ACUTE
                                        0x01e56, //  LATIN CAPITAL LETTER P WITH DOT ABOVE
                                        0x01e58, //  LATIN CAPITAL LETTER R WITH DOT ABOVE
                                        0x01e5a, //  LATIN CAPITAL LETTER R WITH DOT BELOW
                                        0x01e5c, //  LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
                                        0x01e5e, //  LATIN CAPITAL LETTER R WITH LINE BELOW
                                        0x01e60, //  LATIN CAPITAL LETTER S WITH DOT ABOVE
                                        0x01e62, //  LATIN CAPITAL LETTER S WITH DOT BELOW
                                        0x01e64, //  LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
                                        0x01e66, //  LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
                                        0x01e68, //  LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
                                        0x01e6a, //  LATIN CAPITAL LETTER T WITH DOT ABOVE
                                        0x01e6c, //  LATIN CAPITAL LETTER T WITH DOT BELOW
                                        0x01e6e, //  LATIN CAPITAL LETTER T WITH LINE BELOW
                                        0x01e70, //  LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
                                        0x01e72, //  LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
                                        0x01e74, //  LATIN CAPITAL LETTER U WITH TILDE BELOW
                                        0x01e76, //  LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
                                        0x01e78, //  LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
                                        0x01e7a, //  LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
                                        0x01e7c, //  LATIN CAPITAL LETTER V WITH TILDE
                                        0x01e7e, //  LATIN CAPITAL LETTER V WITH DOT BELOW
                                        0x01e80, //  LATIN CAPITAL LETTER W WITH GRAVE
                                        0x01e82, //  LATIN CAPITAL LETTER W WITH ACUTE
                                        0x01e84, //  LATIN CAPITAL LETTER W WITH DIAERESIS
                                        0x01e86, //  LATIN CAPITAL LETTER W WITH DOT ABOVE
                                        0x01e88, //  LATIN CAPITAL LETTER W WITH DOT BELOW
                                        0x01e8a, //  LATIN CAPITAL LETTER X WITH DOT ABOVE
                                        0x01e8c, //  LATIN CAPITAL LETTER X WITH DIAERESIS
                                        0x01e8e, //  LATIN CAPITAL LETTER Y WITH DOT ABOVE
                                        0x01e90, //  LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
                                        0x01e92, //  LATIN CAPITAL LETTER Z WITH DOT BELOW
                                        0x01e94, //  LATIN CAPITAL LETTER Z WITH LINE BELOW
                                        0x01e9b, //  LATIN SMALL LETTER LONG S WITH DOT ABOVE
                                        0x01e9e, //  LATIN CAPITAL LETTER SHARP S
                                        0x01ea0, //  LATIN CAPITAL LETTER A WITH DOT BELOW
                                        0x01ea2, //  LATIN CAPITAL LETTER A WITH HOOK ABOVE
                                        0x01ea4, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
                                        0x01ea6, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
                                        0x01ea8, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
                                        0x01eaa, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
                                        0x01eac, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
                                        0x01eae, //  LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
                                        0x01eb0, //  LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
                                        0x01eb2, //  LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
                                        0x01eb4, //  LATIN CAPITAL LETTER A WITH BREVE AND TILDE
                                        0x01eb6, //  LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
                                        0x01eb8, //  LATIN CAPITAL LETTER E WITH DOT BELOW
                                        0x01eba, //  LATIN CAPITAL LETTER E WITH HOOK ABOVE
                                        0x01ebc, //  LATIN CAPITAL LETTER E WITH TILDE
                                        0x01ebe, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
                                        0x01ec0, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
                                        0x01ec2, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
                                        0x01ec4, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
                                        0x01ec6, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
                                        0x01ec8, //  LATIN CAPITAL LETTER I WITH HOOK ABOVE
                                        0x01eca, //  LATIN CAPITAL LETTER I WITH DOT BELOW
                                        0x01ecc, //  LATIN CAPITAL LETTER O WITH DOT BELOW
                                        0x01ece, //  LATIN CAPITAL LETTER O WITH HOOK ABOVE
                                        0x01ed0, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
                                        0x01ed2, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
                                        0x01ed4, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
                                        0x01ed6, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
                                        0x01ed8, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
                                        0x01eda, //  LATIN CAPITAL LETTER O WITH HORN AND ACUTE
                                        0x01edc, //  LATIN CAPITAL LETTER O WITH HORN AND GRAVE
                                        0x01ede, //  LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
                                        0x01ee0, //  LATIN CAPITAL LETTER O WITH HORN AND TILDE
                                        0x01ee2, //  LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
                                        0x01ee4, //  LATIN CAPITAL LETTER U WITH DOT BELOW
                                        0x01ee6, //  LATIN CAPITAL LETTER U WITH HOOK ABOVE
                                        0x01ee8, //  LATIN CAPITAL LETTER U WITH HORN AND ACUTE
                                        0x01eea, //  LATIN CAPITAL LETTER U WITH HORN AND GRAVE
                                        0x01eec, //  LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
                                        0x01eee, //  LATIN CAPITAL LETTER U WITH HORN AND TILDE
                                        0x01ef0, //  LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
                                        0x01ef2, //  LATIN CAPITAL LETTER Y WITH GRAVE
                                        0x01ef4, //  LATIN CAPITAL LETTER Y WITH DOT BELOW
                                        0x01ef6, //  LATIN CAPITAL LETTER Y WITH HOOK ABOVE
                                        0x01ef8, //  LATIN CAPITAL LETTER Y WITH TILDE
                                        0x01efa, //  LATIN CAPITAL LETTER MIDDLE-WELSH LL
                                        0x01efc, //  LATIN CAPITAL LETTER MIDDLE-WELSH V
                                        0x01efe, //  LATIN CAPITAL LETTER Y WITH LOOP
                                        0x01f08, //  GREEK CAPITAL LETTER ALPHA WITH PSILI
                                        0x01f09, //  GREEK CAPITAL LETTER ALPHA WITH DASIA
                                        0x01f0a, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
                                        0x01f0b, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
                                        0x01f0c, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
                                        0x01f0d, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
                                        0x01f0e, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
                                        0x01f0f, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
                                        0x01f18, //  GREEK CAPITAL LETTER EPSILON WITH PSILI
                                        0x01f19, //  GREEK CAPITAL LETTER EPSILON WITH DASIA
                                        0x01f1a, //  GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
                                        0x01f1b, //  GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
                                        0x01f1c, //  GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
                                        0x01f1d, //  GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
                                        0x01f28, //  GREEK CAPITAL LETTER ETA WITH PSILI
                                        0x01f29, //  GREEK CAPITAL LETTER ETA WITH DASIA
                                        0x01f2a, //  GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
                                        0x01f2b, //  GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
                                        0x01f2c, //  GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
                                        0x01f2d, //  GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
                                        0x01f2e, //  GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
                                        0x01f2f, //  GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
                                        0x01f38, //  GREEK CAPITAL LETTER IOTA WITH PSILI
                                        0x01f39, //  GREEK CAPITAL LETTER IOTA WITH DASIA
                                        0x01f3a, //  GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
                                        0x01f3b, //  GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
                                        0x01f3c, //  GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
                                        0x01f3d, //  GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
                                        0x01f3e, //  GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
                                        0x01f3f, //  GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
                                        0x01f48, //  GREEK CAPITAL LETTER OMICRON WITH PSILI
                                        0x01f49, //  GREEK CAPITAL LETTER OMICRON WITH DASIA
                                        0x01f4a, //  GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
                                        0x01f4b, //  GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
                                        0x01f4c, //  GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
                                        0x01f4d, //  GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
                                        0x01f59, //  GREEK CAPITAL LETTER UPSILON WITH DASIA
                                        0x01f5b, //  GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
                                        0x01f5d, //  GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
                                        0x01f5f, //  GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
                                        0x01f68, //  GREEK CAPITAL LETTER OMEGA WITH PSILI
                                        0x01f69, //  GREEK CAPITAL LETTER OMEGA WITH DASIA
                                        0x01f6a, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
                                        0x01f6b, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
                                        0x01f6c, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
                                        0x01f6d, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
                                        0x01f6e, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
                                        0x01f6f, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
                                        0x01f88, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
                                        0x01f89, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
                                        0x01f8a, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                                        0x01f8b, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                                        0x01f8c, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                                        0x01f8d, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                                        0x01f8e, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                                        0x01f8f, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                                        0x01f98, //  GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
                                        0x01f99, //  GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
                                        0x01f9a, //  GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                                        0x01f9b, //  GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                                        0x01f9c, //  GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                                        0x01f9d, //  GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                                        0x01f9e, //  GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                                        0x01f9f, //  GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                                        0x01fa8, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
                                        0x01fa9, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
                                        0x01faa, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                                        0x01fab, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                                        0x01fac, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                                        0x01fad, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                                        0x01fae, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                                        0x01faf, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                                        0x01fb8, //  GREEK CAPITAL LETTER ALPHA WITH VRACHY
                                        0x01fb9, //  GREEK CAPITAL LETTER ALPHA WITH MACRON
                                        0x01fba, //  GREEK CAPITAL LETTER ALPHA WITH VARIA
                                        0x01fbb, //  GREEK CAPITAL LETTER ALPHA WITH OXIA
                                        0x01fbc, //  GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
                                        0x01fbe, //  GREEK PROSGEGRAMMENI
                                        0x01fc8, //  GREEK CAPITAL LETTER EPSILON WITH VARIA
                                        0x01fc9, //  GREEK CAPITAL LETTER EPSILON WITH OXIA
                                        0x01fca, //  GREEK CAPITAL LETTER ETA WITH VARIA
                                        0x01fcb, //  GREEK CAPITAL LETTER ETA WITH OXIA
                                        0x01fcc, //  GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
                                        0x01fd8, //  GREEK CAPITAL LETTER IOTA WITH VRACHY
                                        0x01fd9, //  GREEK CAPITAL LETTER IOTA WITH MACRON
                                        0x01fda, //  GREEK CAPITAL LETTER IOTA WITH VARIA
                                        0x01fdb, //  GREEK CAPITAL LETTER IOTA WITH OXIA
                                        0x01fe8, //  GREEK CAPITAL LETTER UPSILON WITH VRACHY
                                        0x01fe9, //  GREEK CAPITAL LETTER UPSILON WITH MACRON
                                        0x01fea, //  GREEK CAPITAL LETTER UPSILON WITH VARIA
                                        0x01feb, //  GREEK CAPITAL LETTER UPSILON WITH OXIA
                                        0x01fec, //  GREEK CAPITAL LETTER RHO WITH DASIA
                                        0x01ff8, //  GREEK CAPITAL LETTER OMICRON WITH VARIA
                                        0x01ff9, //  GREEK CAPITAL LETTER OMICRON WITH OXIA
                                        0x01ffa, //  GREEK CAPITAL LETTER OMEGA WITH VARIA
                                        0x01ffb, //  GREEK CAPITAL LETTER OMEGA WITH OXIA
                                        0x01ffc, //  GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
                                        0x02126, //  OHM SIGN
                                        0x0212a, //  KELVIN SIGN
                                        0x0212b, //  ANGSTROM SIGN
                                        0x02132, //  TURNED CAPITAL F
                                        0x02160, //  ROMAN NUMERAL ONE
                                        0x02161, //  ROMAN NUMERAL TWO
                                        0x02162, //  ROMAN NUMERAL THREE
                                        0x02163, //  ROMAN NUMERAL FOUR
                                        0x02164, //  ROMAN NUMERAL FIVE
                                        0x02165, //  ROMAN NUMERAL SIX
                                        0x02166, //  ROMAN NUMERAL SEVEN
                                        0x02167, //  ROMAN NUMERAL EIGHT
                                        0x02168, //  ROMAN NUMERAL NINE
                                        0x02169, //  ROMAN NUMERAL TEN
                                        0x0216a, //  ROMAN NUMERAL ELEVEN
                                        0x0216b, //  ROMAN NUMERAL TWELVE
                                        0x0216c, //  ROMAN NUMERAL FIFTY
                                        0x0216d, //  ROMAN NUMERAL ONE HUNDRED
                                        0x0216e, //  ROMAN NUMERAL FIVE HUNDRED
                                        0x0216f, //  ROMAN NUMERAL ONE THOUSAND
                                        0x02183, //  ROMAN NUMERAL REVERSED ONE HUNDRED
                                        0x024b6, //  CIRCLED LATIN CAPITAL LETTER A
                                        0x024b7, //  CIRCLED LATIN CAPITAL LETTER B
                                        0x024b8, //  CIRCLED LATIN CAPITAL LETTER C
                                        0x024b9, //  CIRCLED LATIN CAPITAL LETTER D
                                        0x024ba, //  CIRCLED LATIN CAPITAL LETTER E
                                        0x024bb, //  CIRCLED LATIN CAPITAL LETTER F
                                        0x024bc, //  CIRCLED LATIN CAPITAL LETTER G
                                        0x024bd, //  CIRCLED LATIN CAPITAL LETTER H
                                        0x024be, //  CIRCLED LATIN CAPITAL LETTER I
                                        0x024bf, //  CIRCLED LATIN CAPITAL LETTER J
                                        0x024c0, //  CIRCLED LATIN CAPITAL LETTER K
                                        0x024c1, //  CIRCLED LATIN CAPITAL LETTER L
                                        0x024c2, //  CIRCLED LATIN CAPITAL LETTER M
                                        0x024c3, //  CIRCLED LATIN CAPITAL LETTER N
                                        0x024c4, //  CIRCLED LATIN CAPITAL LETTER O
                                        0x024c5, //  CIRCLED LATIN CAPITAL LETTER P
                                        0x024c6, //  CIRCLED LATIN CAPITAL LETTER Q
                                        0x024c7, //  CIRCLED LATIN CAPITAL LETTER R
                                        0x024c8, //  CIRCLED LATIN CAPITAL LETTER S
                                        0x024c9, //  CIRCLED LATIN CAPITAL LETTER T
                                        0x024ca, //  CIRCLED LATIN CAPITAL LETTER U
                                        0x024cb, //  CIRCLED LATIN CAPITAL LETTER V
                                        0x024cc, //  CIRCLED LATIN CAPITAL LETTER W
                                        0x024cd, //  CIRCLED LATIN CAPITAL LETTER X
                                        0x024ce, //  CIRCLED LATIN CAPITAL LETTER Y
                                        0x024cf, //  CIRCLED LATIN CAPITAL LETTER Z
                                        0x02c00, //  GLAGOLITIC CAPITAL LETTER AZU
                                        0x02c01, //  GLAGOLITIC CAPITAL LETTER BUKY
                                        0x02c02, //  GLAGOLITIC CAPITAL LETTER VEDE
                                        0x02c03, //  GLAGOLITIC CAPITAL LETTER GLAGOLI
                                        0x02c04, //  GLAGOLITIC CAPITAL LETTER DOBRO
                                        0x02c05, //  GLAGOLITIC CAPITAL LETTER YESTU
                                        0x02c06, //  GLAGOLITIC CAPITAL LETTER ZHIVETE
                                        0x02c07, //  GLAGOLITIC CAPITAL LETTER DZELO
                                        0x02c08, //  GLAGOLITIC CAPITAL LETTER ZEMLJA
                                        0x02c09, //  GLAGOLITIC CAPITAL LETTER IZHE
                                        0x02c0a, //  GLAGOLITIC CAPITAL LETTER INITIAL IZHE
                                        0x02c0b, //  GLAGOLITIC CAPITAL LETTER I
                                        0x02c0c, //  GLAGOLITIC CAPITAL LETTER DJERVI
                                        0x02c0d, //  GLAGOLITIC CAPITAL LETTER KAKO
                                        0x02c0e, //  GLAGOLITIC CAPITAL LETTER LJUDIJE
                                        0x02c0f, //  GLAGOLITIC CAPITAL LETTER MYSLITE
                                        0x02c10, //  GLAGOLITIC CAPITAL LETTER NASHI
                                        0x02c11, //  GLAGOLITIC CAPITAL LETTER ONU
                                        0x02c12, //  GLAGOLITIC CAPITAL LETTER POKOJI
                                        0x02c13, //  GLAGOLITIC CAPITAL LETTER RITSI
                                        0x02c14, //  GLAGOLITIC CAPITAL LETTER SLOVO
                                        0x02c15, //  GLAGOLITIC CAPITAL LETTER TVRIDO
                                        0x02c16, //  GLAGOLITIC CAPITAL LETTER UKU
                                        0x02c17, //  GLAGOLITIC CAPITAL LETTER FRITU
                                        0x02c18, //  GLAGOLITIC CAPITAL LETTER HERU
                                        0x02c19, //  GLAGOLITIC CAPITAL LETTER OTU
                                        0x02c1a, //  GLAGOLITIC CAPITAL LETTER PE
                                        0x02c1b, //  GLAGOLITIC CAPITAL LETTER SHTA
                                        0x02c1c, //  GLAGOLITIC CAPITAL LETTER TSI
                                        0x02c1d, //  GLAGOLITIC CAPITAL LETTER CHRIVI
                                        0x02c1e, //  GLAGOLITIC CAPITAL LETTER SHA
                                        0x02c1f, //  GLAGOLITIC CAPITAL LETTER YERU
                                        0x02c20, //  GLAGOLITIC CAPITAL LETTER YERI
                                        0x02c21, //  GLAGOLITIC CAPITAL LETTER YATI
                                        0x02c22, //  GLAGOLITIC CAPITAL LETTER SPIDERY HA
                                        0x02c23, //  GLAGOLITIC CAPITAL LETTER YU
                                        0x02c24, //  GLAGOLITIC CAPITAL LETTER SMALL YUS
                                        0x02c25, //  GLAGOLITIC CAPITAL LETTER SMALL YUS WITH TAIL
                                        0x02c26, //  GLAGOLITIC CAPITAL LETTER YO
                                        0x02c27, //  GLAGOLITIC CAPITAL LETTER IOTATED SMALL YUS
                                        0x02c28, //  GLAGOLITIC CAPITAL LETTER BIG YUS
                                        0x02c29, //  GLAGOLITIC CAPITAL LETTER IOTATED BIG YUS
                                        0x02c2a, //  GLAGOLITIC CAPITAL LETTER FITA
                                        0x02c2b, //  GLAGOLITIC CAPITAL LETTER IZHITSA
                                        0x02c2c, //  GLAGOLITIC CAPITAL LETTER SHTAPIC
                                        0x02c2d, //  GLAGOLITIC CAPITAL LETTER TROKUTASTI A
                                        0x02c2e, //  GLAGOLITIC CAPITAL LETTER LATINATE MYSLITE
                                        0x02c2f, //  GLAGOLITIC CAPITAL LETTER CAUDATE CHRIVI
                                        0x02c60, //  LATIN CAPITAL LETTER L WITH DOUBLE BAR
                                        0x02c62, //  LATIN CAPITAL LETTER L WITH MIDDLE TILDE
                                        0x02c63, //  LATIN CAPITAL LETTER P WITH STROKE
                                        0x02c64, //  LATIN CAPITAL LETTER R WITH TAIL
                                        0x02c67, //  LATIN CAPITAL LETTER H WITH DESCENDER
                                        0x02c69, //  LATIN CAPITAL LETTER K WITH DESCENDER
                                        0x02c6b, //  LATIN CAPITAL LETTER Z WITH DESCENDER
                                        0x02c6d, //  LATIN CAPITAL LETTER ALPHA
                                        0x02c6e, //  LATIN CAPITAL LETTER M WITH HOOK
                                        0x02c6f, //  LATIN CAPITAL LETTER TURNED A
                                        0x02c70, //  LATIN CAPITAL LETTER TURNED ALPHA
                                        0x02c72, //  LATIN CAPITAL LETTER W WITH HOOK
                                        0x02c75, //  LATIN CAPITAL LETTER HALF H
                                        0x02c7e, //  LATIN CAPITAL LETTER S WITH SWASH TAIL
                                        0x02c7f, //  LATIN CAPITAL LETTER Z WITH SWASH TAIL
                                        0x02c80, //  COPTIC CAPITAL LETTER ALFA
                                        0x02c82, //  COPTIC CAPITAL LETTER VIDA
                                        0x02c84, //  COPTIC CAPITAL LETTER GAMMA
                                        0x02c86, //  COPTIC CAPITAL LETTER DALDA
                                        0x02c88, //  COPTIC CAPITAL LETTER EIE
                                        0x02c8a, //  COPTIC CAPITAL LETTER SOU
                                        0x02c8c, //  COPTIC CAPITAL LETTER ZATA
                                        0x02c8e, //  COPTIC CAPITAL LETTER HATE
                                        0x02c90, //  COPTIC CAPITAL LETTER THETHE
                                        0x02c92, //  COPTIC CAPITAL LETTER IAUDA
                                        0x02c94, //  COPTIC CAPITAL LETTER KAPA
                                        0x02c96, //  COPTIC CAPITAL LETTER LAULA
                                        0x02c98, //  COPTIC CAPITAL LETTER MI
                                        0x02c9a, //  COPTIC CAPITAL LETTER NI
                                        0x02c9c, //  COPTIC CAPITAL LETTER KSI
                                        0x02c9e, //  COPTIC CAPITAL LETTER O
                                        0x02ca0, //  COPTIC CAPITAL LETTER PI
                                        0x02ca2, //  COPTIC CAPITAL LETTER RO
                                        0x02ca4, //  COPTIC CAPITAL LETTER SIMA
                                        0x02ca6, //  COPTIC CAPITAL LETTER TAU
                                        0x02ca8, //  COPTIC CAPITAL LETTER UA
                                        0x02caa, //  COPTIC CAPITAL LETTER FI
                                        0x02cac, //  COPTIC CAPITAL LETTER KHI
                                        0x02cae, //  COPTIC CAPITAL LETTER PSI
                                        0x02cb0, //  COPTIC CAPITAL LETTER OOU
                                        0x02cb2, //  COPTIC CAPITAL LETTER DIALECT-P ALEF
                                        0x02cb4, //  COPTIC CAPITAL LETTER OLD COPTIC AIN
                                        0x02cb6, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC EIE
                                        0x02cb8, //  COPTIC CAPITAL LETTER DIALECT-P KAPA
                                        0x02cba, //  COPTIC CAPITAL LETTER DIALECT-P NI
                                        0x02cbc, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC NI
                                        0x02cbe, //  COPTIC CAPITAL LETTER OLD COPTIC OOU
                                        0x02cc0, //  COPTIC CAPITAL LETTER SAMPI
                                        0x02cc2, //  COPTIC CAPITAL LETTER CROSSED SHEI
                                        0x02cc4, //  COPTIC CAPITAL LETTER OLD COPTIC SHEI
                                        0x02cc6, //  COPTIC CAPITAL LETTER OLD COPTIC ESH
                                        0x02cc8, //  COPTIC CAPITAL LETTER AKHMIMIC KHEI
                                        0x02cca, //  COPTIC CAPITAL LETTER DIALECT-P HORI
                                        0x02ccc, //  COPTIC CAPITAL LETTER OLD COPTIC HORI
                                        0x02cce, //  COPTIC CAPITAL LETTER OLD COPTIC HA
                                        0x02cd0, //  COPTIC CAPITAL LETTER L-SHAPED HA
                                        0x02cd2, //  COPTIC CAPITAL LETTER OLD COPTIC HEI
                                        0x02cd4, //  COPTIC CAPITAL LETTER OLD COPTIC HAT
                                        0x02cd6, //  COPTIC CAPITAL LETTER OLD COPTIC GANGIA
                                        0x02cd8, //  COPTIC CAPITAL LETTER OLD COPTIC DJA
                                        0x02cda, //  COPTIC CAPITAL LETTER OLD COPTIC SHIMA
                                        0x02cdc, //  COPTIC CAPITAL LETTER OLD NUBIAN SHIMA
                                        0x02cde, //  COPTIC CAPITAL LETTER OLD NUBIAN NGI
                                        0x02ce0, //  COPTIC CAPITAL LETTER OLD NUBIAN NYI
                                        0x02ce2, //  COPTIC CAPITAL LETTER OLD NUBIAN WAU
                                        0x02ceb, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
                                        0x02ced, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC GANGIA
                                        0x02cf2, //  COPTIC CAPITAL LETTER BOHAIRIC KHEI
                                        0x0a640, //  CYRILLIC CAPITAL LETTER ZEMLYA
                                        0x0a642, //  CYRILLIC CAPITAL LETTER DZELO
                                        0x0a644, //  CYRILLIC CAPITAL LETTER REVERSED DZE
                                        0x0a646, //  CYRILLIC CAPITAL LETTER IOTA
                                        0x0a648, //  CYRILLIC CAPITAL LETTER DJERV
                                        0x0a64a, //  CYRILLIC CAPITAL LETTER MONOGRAPH UK
                                        0x0a64c, //  CYRILLIC CAPITAL LETTER BROAD OMEGA
                                        0x0a64e, //  CYRILLIC CAPITAL LETTER NEUTRAL YER
                                        0x0a650, //  CYRILLIC CAPITAL LETTER YERU WITH BACK YER
                                        0x0a652, //  CYRILLIC CAPITAL LETTER IOTIFIED YAT
                                        0x0a654, //  CYRILLIC CAPITAL LETTER REVERSED YU
                                        0x0a656, //  CYRILLIC CAPITAL LETTER IOTIFIED A
                                        0x0a658, //  CYRILLIC CAPITAL LETTER CLOSED LITTLE YUS
                                        0x0a65a, //  CYRILLIC CAPITAL LETTER BLENDED YUS
                                        0x0a65c, //  CYRILLIC CAPITAL LETTER IOTIFIED CLOSED LITTLE YUS
                                        0x0a65e, //  CYRILLIC CAPITAL LETTER YN
                                        0x0a660, //  CYRILLIC CAPITAL LETTER REVERSED TSE
                                        0x0a662, //  CYRILLIC CAPITAL LETTER SOFT DE
                                        0x0a664, //  CYRILLIC CAPITAL LETTER SOFT EL
                                        0x0a666, //  CYRILLIC CAPITAL LETTER SOFT EM
                                        0x0a668, //  CYRILLIC CAPITAL LETTER MONOCULAR O
                                        0x0a66a, //  CYRILLIC CAPITAL LETTER BINOCULAR O
                                        0x0a66c, //  CYRILLIC CAPITAL LETTER DOUBLE MONOCULAR O
                                        0x0a680, //  CYRILLIC CAPITAL LETTER DWE
                                        0x0a682, //  CYRILLIC CAPITAL LETTER DZWE
                                        0x0a684, //  CYRILLIC CAPITAL LETTER ZHWE
                                        0x0a686, //  CYRILLIC CAPITAL LETTER CCHE
                                        0x0a688, //  CYRILLIC CAPITAL LETTER DZZE
                                        0x0a68a, //  CYRILLIC CAPITAL LETTER TE WITH MIDDLE HOOK
                                        0x0a68c, //  CYRILLIC CAPITAL LETTER TWE
                                        0x0a68e, //  CYRILLIC CAPITAL LETTER TSWE
                                        0x0a690, //  CYRILLIC CAPITAL LETTER TSSE
                                        0x0a692, //  CYRILLIC CAPITAL LETTER TCHE
                                        0x0a694, //  CYRILLIC CAPITAL LETTER HWE
                                        0x0a696, //  CYRILLIC CAPITAL LETTER SHWE
                                        0x0a698, //  CYRILLIC CAPITAL LETTER DOUBLE O
                                        0x0a69a, //  CYRILLIC CAPITAL LETTER CROSSED O
                                        0x0a722, //  LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
                                        0x0a724, //  LATIN CAPITAL LETTER EGYPTOLOGICAL AIN
                                        0x0a726, //  LATIN CAPITAL LETTER HENG
                                        0x0a728, //  LATIN CAPITAL LETTER TZ
                                        0x0a72a, //  LATIN CAPITAL LETTER TRESILLO
                                        0x0a72c, //  LATIN CAPITAL LETTER CUATRILLO
                                        0x0a72e, //  LATIN CAPITAL LETTER CUATRILLO WITH COMMA
                                        0x0a732, //  LATIN CAPITAL LETTER AA
                                        0x0a734, //  LATIN CAPITAL LETTER AO
                                        0x0a736, //  LATIN CAPITAL LETTER AU
                                        0x0a738, //  LATIN CAPITAL LETTER AV
                                        0x0a73a, //  LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR
                                        0x0a73c, //  LATIN CAPITAL LETTER AY
                                        0x0a73e, //  LATIN CAPITAL LETTER REVERSED C WITH DOT
                                        0x0a740, //  LATIN CAPITAL LETTER K WITH STROKE
                                        0x0a742, //  LATIN CAPITAL LETTER K WITH DIAGONAL STROKE
                                        0x0a744, //  LATIN CAPITAL LETTER K WITH STROKE AND DIAGONAL STROKE
                                        0x0a746, //  LATIN CAPITAL LETTER BROKEN L
                                        0x0a748, //  LATIN CAPITAL LETTER L WITH HIGH STROKE
                                        0x0a74a, //  LATIN CAPITAL LETTER O WITH LONG STROKE OVERLAY
                                        0x0a74c, //  LATIN CAPITAL LETTER O WITH LOOP
                                        0x0a74e, //  LATIN CAPITAL LETTER OO
                                        0x0a750, //  LATIN CAPITAL LETTER P WITH STROKE THROUGH DESCENDER
                                        0x0a752, //  LATIN CAPITAL LETTER P WITH FLOURISH
                                        0x0a754, //  LATIN CAPITAL LETTER P WITH SQUIRREL TAIL
                                        0x0a756, //  LATIN CAPITAL LETTER Q WITH STROKE THROUGH DESCENDER
                                        0x0a758, //  LATIN CAPITAL LETTER Q WITH DIAGONAL STROKE
                                        0x0a75a, //  LATIN CAPITAL LETTER R ROTUNDA
                                        0x0a75c, //  LATIN CAPITAL LETTER RUM ROTUNDA
                                        0x0a75e, //  LATIN CAPITAL LETTER V WITH DIAGONAL STROKE
                                        0x0a760, //  LATIN CAPITAL LETTER VY
                                        0x0a762, //  LATIN CAPITAL LETTER VISIGOTHIC Z
                                        0x0a764, //  LATIN CAPITAL LETTER THORN WITH STROKE
                                        0x0a766, //  LATIN CAPITAL LETTER THORN WITH STROKE THROUGH DESCENDER
                                        0x0a768, //  LATIN CAPITAL LETTER VEND
                                        0x0a76a, //  LATIN CAPITAL LETTER ET
                                        0x0a76c, //  LATIN CAPITAL LETTER IS
                                        0x0a76e, //  LATIN CAPITAL LETTER CON
                                        0x0a779, //  LATIN CAPITAL LETTER INSULAR D
                                        0x0a77b, //  LATIN CAPITAL LETTER INSULAR F
                                        0x0a77d, //  LATIN CAPITAL LETTER INSULAR G
                                        0x0a77e, //  LATIN CAPITAL LETTER TURNED INSULAR G
                                        0x0a780, //  LATIN CAPITAL LETTER TURNED L
                                        0x0a782, //  LATIN CAPITAL LETTER INSULAR R
                                        0x0a784, //  LATIN CAPITAL LETTER INSULAR S
                                        0x0a786, //  LATIN CAPITAL LETTER INSULAR T
                                        0x0a78b, //  LATIN CAPITAL LETTER SALTILLO
                                        0x0a78d, //  LATIN CAPITAL LETTER TURNED H
                                        0x0a790, //  LATIN CAPITAL LETTER N WITH DESCENDER
                                        0x0a792, //  LATIN CAPITAL LETTER C WITH BAR
                                        0x0a796, //  LATIN CAPITAL LETTER B WITH FLOURISH
                                        0x0a798, //  LATIN CAPITAL LETTER F WITH STROKE
                                        0x0a79a, //  LATIN CAPITAL LETTER VOLAPUK AE
                                        0x0a79c, //  LATIN CAPITAL LETTER VOLAPUK OE
                                        0x0a79e, //  LATIN CAPITAL LETTER VOLAPUK UE
                                        0x0a7a0, //  LATIN CAPITAL LETTER G WITH OBLIQUE STROKE
                                        0x0a7a2, //  LATIN CAPITAL LETTER K WITH OBLIQUE STROKE
                                        0x0a7a4, //  LATIN CAPITAL LETTER N WITH OBLIQUE STROKE
                                        0x0a7a6, //  LATIN CAPITAL LETTER R WITH OBLIQUE STROKE
                                        0x0a7a8, //  LATIN CAPITAL LETTER S WITH OBLIQUE STROKE
                                        0x0a7aa, //  LATIN CAPITAL LETTER H WITH HOOK
                                        0x0a7ab, //  LATIN CAPITAL LETTER REVERSED OPEN E
                                        0x0a7ac, //  LATIN CAPITAL LETTER SCRIPT G
                                        0x0a7ad, //  LATIN CAPITAL LETTER L WITH BELT
                                        0x0a7ae, //  LATIN CAPITAL LETTER SMALL CAPITAL I
                                        0x0a7b0, //  LATIN CAPITAL LETTER TURNED K
                                        0x0a7b1, //  LATIN CAPITAL LETTER TURNED T
                                        0x0a7b2, //  LATIN CAPITAL LETTER J WITH CROSSED-TAIL
                                        0x0a7b3, //  LATIN CAPITAL LETTER CHI
                                        0x0a7b4, //  LATIN CAPITAL LETTER BETA
                                        0x0a7b6, //  LATIN CAPITAL LETTER OMEGA
                                        0x0a7b8, //  LATIN CAPITAL LETTER U WITH STROKE
                                        0x0a7ba, //  LATIN CAPITAL LETTER GLOTTAL A
                                        0x0a7bc, //  LATIN CAPITAL LETTER GLOTTAL I
                                        0x0a7be, //  LATIN CAPITAL LETTER GLOTTAL U
                                        0x0a7c0, //  LATIN CAPITAL LETTER OLD POLISH O
                                        0x0a7c2, //  LATIN CAPITAL LETTER ANGLICANA W
                                        0x0a7c4, //  LATIN CAPITAL LETTER C WITH PALATAL HOOK
                                        0x0a7c5, //  LATIN CAPITAL LETTER S WITH HOOK
                                        0x0a7c6, //  LATIN CAPITAL LETTER Z WITH PALATAL HOOK
                                        0x0a7c7, //  LATIN CAPITAL LETTER D WITH SHORT STROKE OVERLAY
                                        0x0a7c9, //  LATIN CAPITAL LETTER S WITH SHORT STROKE OVERLAY
                                        0x0a7d0, //  LATIN CAPITAL LETTER CLOSED INSULAR G
                                        0x0a7d6, //  LATIN CAPITAL LETTER MIDDLE SCOTS S
                                        0x0a7d8, //  LATIN CAPITAL LETTER SIGMOID S
                                        0x0a7f5, //  LATIN CAPITAL LETTER REVERSED HALF H
                                        0x0ab70, //  CHEROKEE SMALL LETTER A
                                        0x0ab71, //  CHEROKEE SMALL LETTER E
                                        0x0ab72, //  CHEROKEE SMALL LETTER I
                                        0x0ab73, //  CHEROKEE SMALL LETTER O
                                        0x0ab74, //  CHEROKEE SMALL LETTER U
                                        0x0ab75, //  CHEROKEE SMALL LETTER V
                                        0x0ab76, //  CHEROKEE SMALL LETTER GA
                                        0x0ab77, //  CHEROKEE SMALL LETTER KA
                                        0x0ab78, //  CHEROKEE SMALL LETTER GE
                                        0x0ab79, //  CHEROKEE SMALL LETTER GI
                                        0x0ab7a, //  CHEROKEE SMALL LETTER GO
                                        0x0ab7b, //  CHEROKEE SMALL LETTER GU
                                        0x0ab7c, //  CHEROKEE SMALL LETTER GV
                                        0x0ab7d, //  CHEROKEE SMALL LETTER HA
                                        0x0ab7e, //  CHEROKEE SMALL LETTER HE
                                        0x0ab7f, //  CHEROKEE SMALL LETTER HI
                                        0x0ab80, //  CHEROKEE SMALL LETTER HO
                                        0x0ab81, //  CHEROKEE SMALL LETTER HU
                                        0x0ab82, //  CHEROKEE SMALL LETTER HV
                                        0x0ab83, //  CHEROKEE SMALL LETTER LA
                                        0x0ab84, //  CHEROKEE SMALL LETTER LE
                                        0x0ab85, //  CHEROKEE SMALL LETTER LI
                                        0x0ab86, //  CHEROKEE SMALL LETTER LO
                                        0x0ab87, //  CHEROKEE SMALL LETTER LU
                                        0x0ab88, //  CHEROKEE SMALL LETTER LV
                                        0x0ab89, //  CHEROKEE SMALL LETTER MA
                                        0x0ab8a, //  CHEROKEE SMALL LETTER ME
                                        0x0ab8b, //  CHEROKEE SMALL LETTER MI
                                        0x0ab8c, //  CHEROKEE SMALL LETTER MO
                                        0x0ab8d, //  CHEROKEE SMALL LETTER MU
                                        0x0ab8e, //  CHEROKEE SMALL LETTER NA
                                        0x0ab8f, //  CHEROKEE SMALL LETTER HNA
                                        0x0ab90, //  CHEROKEE SMALL LETTER NAH
                                        0x0ab91, //  CHEROKEE SMALL LETTER NE
                                        0x0ab92, //  CHEROKEE SMALL LETTER NI
                                        0x0ab93, //  CHEROKEE SMALL LETTER NO
                                        0x0ab94, //  CHEROKEE SMALL LETTER NU
                                        0x0ab95, //  CHEROKEE SMALL LETTER NV
                                        0x0ab96, //  CHEROKEE SMALL LETTER QUA
                                        0x0ab97, //  CHEROKEE SMALL LETTER QUE
                                        0x0ab98, //  CHEROKEE SMALL LETTER QUI
                                        0x0ab99, //  CHEROKEE SMALL LETTER QUO
                                        0x0ab9a, //  CHEROKEE SMALL LETTER QUU
                                        0x0ab9b, //  CHEROKEE SMALL LETTER QUV
                                        0x0ab9c, //  CHEROKEE SMALL LETTER SA
                                        0x0ab9d, //  CHEROKEE SMALL LETTER S
                                        0x0ab9e, //  CHEROKEE SMALL LETTER SE
                                        0x0ab9f, //  CHEROKEE SMALL LETTER SI
                                        0x0aba0, //  CHEROKEE SMALL LETTER SO
                                        0x0aba1, //  CHEROKEE SMALL LETTER SU
                                        0x0aba2, //  CHEROKEE SMALL LETTER SV
                                        0x0aba3, //  CHEROKEE SMALL LETTER DA
                                        0x0aba4, //  CHEROKEE SMALL LETTER TA
                                        0x0aba5, //  CHEROKEE SMALL LETTER DE
                                        0x0aba6, //  CHEROKEE SMALL LETTER TE
                                        0x0aba7, //  CHEROKEE SMALL LETTER DI
                                        0x0aba8, //  CHEROKEE SMALL LETTER TI
                                        0x0aba9, //  CHEROKEE SMALL LETTER DO
                                        0x0abaa, //  CHEROKEE SMALL LETTER DU
                                        0x0abab, //  CHEROKEE SMALL LETTER DV
                                        0x0abac, //  CHEROKEE SMALL LETTER DLA
                                        0x0abad, //  CHEROKEE SMALL LETTER TLA
                                        0x0abae, //  CHEROKEE SMALL LETTER TLE
                                        0x0abaf, //  CHEROKEE SMALL LETTER TLI
                                        0x0abb0, //  CHEROKEE SMALL LETTER TLO
                                        0x0abb1, //  CHEROKEE SMALL LETTER TLU
                                        0x0abb2, //  CHEROKEE SMALL LETTER TLV
                                        0x0abb3, //  CHEROKEE SMALL LETTER TSA
                                        0x0abb4, //  CHEROKEE SMALL LETTER TSE
                                        0x0abb5, //  CHEROKEE SMALL LETTER TSI
                                        0x0abb6, //  CHEROKEE SMALL LETTER TSO
                                        0x0abb7, //  CHEROKEE SMALL LETTER TSU
                                        0x0abb8, //  CHEROKEE SMALL LETTER TSV
                                        0x0abb9, //  CHEROKEE SMALL LETTER WA
                                        0x0abba, //  CHEROKEE SMALL LETTER WE
                                        0x0abbb, //  CHEROKEE SMALL LETTER WI
                                        0x0abbc, //  CHEROKEE SMALL LETTER WO
                                        0x0abbd, //  CHEROKEE SMALL LETTER WU
                                        0x0abbe, //  CHEROKEE SMALL LETTER WV
                                        0x0abbf, //  CHEROKEE SMALL LETTER YA
                                        0x0ff21, //  FULLWIDTH LATIN CAPITAL LETTER A
                                        0x0ff22, //  FULLWIDTH LATIN CAPITAL LETTER B
                                        0x0ff23, //  FULLWIDTH LATIN CAPITAL LETTER C
                                        0x0ff24, //  FULLWIDTH LATIN CAPITAL LETTER D
                                        0x0ff25, //  FULLWIDTH LATIN CAPITAL LETTER E
                                        0x0ff26, //  FULLWIDTH LATIN CAPITAL LETTER F
                                        0x0ff27, //  FULLWIDTH LATIN CAPITAL LETTER G
                                        0x0ff28, //  FULLWIDTH LATIN CAPITAL LETTER H
                                        0x0ff29, //  FULLWIDTH LATIN CAPITAL LETTER I
                                        0x0ff2a, //  FULLWIDTH LATIN CAPITAL LETTER J
                                        0x0ff2b, //  FULLWIDTH LATIN CAPITAL LETTER K
                                        0x0ff2c, //  FULLWIDTH LATIN CAPITAL LETTER L
                                        0x0ff2d, //  FULLWIDTH LATIN CAPITAL LETTER M
                                        0x0ff2e, //  FULLWIDTH LATIN CAPITAL LETTER N
                                        0x0ff2f, //  FULLWIDTH LATIN CAPITAL LETTER O
                                        0x0ff30, //  FULLWIDTH LATIN CAPITAL LETTER P
                                        0x0ff31, //  FULLWIDTH LATIN CAPITAL LETTER Q
                                        0x0ff32, //  FULLWIDTH LATIN CAPITAL LETTER R
                                        0x0ff33, //  FULLWIDTH LATIN CAPITAL LETTER S
                                        0x0ff34, //  FULLWIDTH LATIN CAPITAL LETTER T
                                        0x0ff35, //  FULLWIDTH LATIN CAPITAL LETTER U
                                        0x0ff36, //  FULLWIDTH LATIN CAPITAL LETTER V
                                        0x0ff37, //  FULLWIDTH LATIN CAPITAL LETTER W
                                        0x0ff38, //  FULLWIDTH LATIN CAPITAL LETTER X
                                        0x0ff39, //  FULLWIDTH LATIN CAPITAL LETTER Y
                                        0x0ff3a, //  FULLWIDTH LATIN CAPITAL LETTER Z
                                        0x10400, //  DESERET CAPITAL LETTER LONG I
                                        0x10401, //  DESERET CAPITAL LETTER LONG E
                                        0x10402, //  DESERET CAPITAL LETTER LONG A
                                        0x10403, //  DESERET CAPITAL LETTER LONG AH
                                        0x10404, //  DESERET CAPITAL LETTER LONG O
                                        0x10405, //  DESERET CAPITAL LETTER LONG OO
                                        0x10406, //  DESERET CAPITAL LETTER SHORT I
                                        0x10407, //  DESERET CAPITAL LETTER SHORT E
                                        0x10408, //  DESERET CAPITAL LETTER SHORT A
                                        0x10409, //  DESERET CAPITAL LETTER SHORT AH
                                        0x1040a, //  DESERET CAPITAL LETTER SHORT O
                                        0x1040b, //  DESERET CAPITAL LETTER SHORT OO
                                        0x1040c, //  DESERET CAPITAL LETTER AY
                                        0x1040d, //  DESERET CAPITAL LETTER OW
                                        0x1040e, //  DESERET CAPITAL LETTER WU
                                        0x1040f, //  DESERET CAPITAL LETTER YEE
                                        0x10410, //  DESERET CAPITAL LETTER H
                                        0x10411, //  DESERET CAPITAL LETTER PEE
                                        0x10412, //  DESERET CAPITAL LETTER BEE
                                        0x10413, //  DESERET CAPITAL LETTER TEE
                                        0x10414, //  DESERET CAPITAL LETTER DEE
                                        0x10415, //  DESERET CAPITAL LETTER CHEE
                                        0x10416, //  DESERET CAPITAL LETTER JEE
                                        0x10417, //  DESERET CAPITAL LETTER KAY
                                        0x10418, //  DESERET CAPITAL LETTER GAY
                                        0x10419, //  DESERET CAPITAL LETTER EF
                                        0x1041a, //  DESERET CAPITAL LETTER VEE
                                        0x1041b, //  DESERET CAPITAL LETTER ETH
                                        0x1041c, //  DESERET CAPITAL LETTER THEE
                                        0x1041d, //  DESERET CAPITAL LETTER ES
                                        0x1041e, //  DESERET CAPITAL LETTER ZEE
                                        0x1041f, //  DESERET CAPITAL LETTER ESH
                                        0x10420, //  DESERET CAPITAL LETTER ZHEE
                                        0x10421, //  DESERET CAPITAL LETTER ER
                                        0x10422, //  DESERET CAPITAL LETTER EL
                                        0x10423, //  DESERET CAPITAL LETTER EM
                                        0x10424, //  DESERET CAPITAL LETTER EN
                                        0x10425, //  DESERET CAPITAL LETTER ENG
                                        0x10426, //  DESERET CAPITAL LETTER OI
                                        0x10427, //  DESERET CAPITAL LETTER EW
                                        0x104b0, //  OSAGE CAPITAL LETTER A
                                        0x104b1, //  OSAGE CAPITAL LETTER AI
                                        0x104b2, //  OSAGE CAPITAL LETTER AIN
                                        0x104b3, //  OSAGE CAPITAL LETTER AH
                                        0x104b4, //  OSAGE CAPITAL LETTER BRA
                                        0x104b5, //  OSAGE CAPITAL LETTER CHA
                                        0x104b6, //  OSAGE CAPITAL LETTER EHCHA
                                        0x104b7, //  OSAGE CAPITAL LETTER E
                                        0x104b8, //  OSAGE CAPITAL LETTER EIN
                                        0x104b9, //  OSAGE CAPITAL LETTER HA
                                        0x104ba, //  OSAGE CAPITAL LETTER HYA
                                        0x104bb, //  OSAGE CAPITAL LETTER I
                                        0x104bc, //  OSAGE CAPITAL LETTER KA
                                        0x104bd, //  OSAGE CAPITAL LETTER EHKA
                                        0x104be, //  OSAGE CAPITAL LETTER KYA
                                        0x104bf, //  OSAGE CAPITAL LETTER LA
                                        0x104c0, //  OSAGE CAPITAL LETTER MA
                                        0x104c1, //  OSAGE CAPITAL LETTER NA
                                        0x104c2, //  OSAGE CAPITAL LETTER O
                                        0x104c3, //  OSAGE CAPITAL LETTER OIN
                                        0x104c4, //  OSAGE CAPITAL LETTER PA
                                        0x104c5, //  OSAGE CAPITAL LETTER EHPA
                                        0x104c6, //  OSAGE CAPITAL LETTER SA
                                        0x104c7, //  OSAGE CAPITAL LETTER SHA
                                        0x104c8, //  OSAGE CAPITAL LETTER TA
                                        0x104c9, //  OSAGE CAPITAL LETTER EHTA
                                        0x104ca, //  OSAGE CAPITAL LETTER TSA
                                        0x104cb, //  OSAGE CAPITAL LETTER EHTSA
                                        0x104cc, //  OSAGE CAPITAL LETTER TSHA
                                        0x104cd, //  OSAGE CAPITAL LETTER DHA
                                        0x104ce, //  OSAGE CAPITAL LETTER U
                                        0x104cf, //  OSAGE CAPITAL LETTER WA
                                        0x104d0, //  OSAGE CAPITAL LETTER KHA
                                        0x104d1, //  OSAGE CAPITAL LETTER GHA
                                        0x104d2, //  OSAGE CAPITAL LETTER ZA
                                        0x104d3, //  OSAGE CAPITAL LETTER ZHA
                                        0x10570, //  VITHKUQI CAPITAL LETTER A
                                        0x10571, //  VITHKUQI CAPITAL LETTER BBE
                                        0x10572, //  VITHKUQI CAPITAL LETTER BE
                                        0x10573, //  VITHKUQI CAPITAL LETTER CE
                                        0x10574, //  VITHKUQI CAPITAL LETTER CHE
                                        0x10575, //  VITHKUQI CAPITAL LETTER DE
                                        0x10576, //  VITHKUQI CAPITAL LETTER DHE
                                        0x10577, //  VITHKUQI CAPITAL LETTER EI
                                        0x10578, //  VITHKUQI CAPITAL LETTER E
                                        0x10579, //  VITHKUQI CAPITAL LETTER FE
                                        0x1057a, //  VITHKUQI CAPITAL LETTER GA
                                        0x1057c, //  VITHKUQI CAPITAL LETTER HA
                                        0x1057d, //  VITHKUQI CAPITAL LETTER HHA
                                        0x1057e, //  VITHKUQI CAPITAL LETTER I
                                        0x1057f, //  VITHKUQI CAPITAL LETTER IJE
                                        0x10580, //  VITHKUQI CAPITAL LETTER JE
                                        0x10581, //  VITHKUQI CAPITAL LETTER KA
                                        0x10582, //  VITHKUQI CAPITAL LETTER LA
                                        0x10583, //  VITHKUQI CAPITAL LETTER LLA
                                        0x10584, //  VITHKUQI CAPITAL LETTER ME
                                        0x10585, //  VITHKUQI CAPITAL LETTER NE
                                        0x10586, //  VITHKUQI CAPITAL LETTER NJE
                                        0x10587, //  VITHKUQI CAPITAL LETTER O
                                        0x10588, //  VITHKUQI CAPITAL LETTER PE
                                        0x10589, //  VITHKUQI CAPITAL LETTER QA
                                        0x1058a, //  VITHKUQI CAPITAL LETTER RE
                                        0x1058c, //  VITHKUQI CAPITAL LETTER SE
                                        0x1058d, //  VITHKUQI CAPITAL LETTER SHE
                                        0x1058e, //  VITHKUQI CAPITAL LETTER TE
                                        0x1058f, //  VITHKUQI CAPITAL LETTER THE
                                        0x10590, //  VITHKUQI CAPITAL LETTER U
                                        0x10591, //  VITHKUQI CAPITAL LETTER VE
                                        0x10592, //  VITHKUQI CAPITAL LETTER XE
                                        0x10594, //  VITHKUQI CAPITAL LETTER Y
                                        0x10595, //  VITHKUQI CAPITAL LETTER ZE
                                        0x10c80, //  OLD HUNGARIAN CAPITAL LETTER A
                                        0x10c81, //  OLD HUNGARIAN CAPITAL LETTER AA
                                        0x10c82, //  OLD HUNGARIAN CAPITAL LETTER EB
                                        0x10c83, //  OLD HUNGARIAN CAPITAL LETTER AMB
                                        0x10c84, //  OLD HUNGARIAN CAPITAL LETTER EC
                                        0x10c85, //  OLD HUNGARIAN CAPITAL LETTER ENC
                                        0x10c86, //  OLD HUNGARIAN CAPITAL LETTER ECS
                                        0x10c87, //  OLD HUNGARIAN CAPITAL LETTER ED
                                        0x10c88, //  OLD HUNGARIAN CAPITAL LETTER AND
                                        0x10c89, //  OLD HUNGARIAN CAPITAL LETTER E
                                        0x10c8a, //  OLD HUNGARIAN CAPITAL LETTER CLOSE E
                                        0x10c8b, //  OLD HUNGARIAN CAPITAL LETTER EE
                                        0x10c8c, //  OLD HUNGARIAN CAPITAL LETTER EF
                                        0x10c8d, //  OLD HUNGARIAN CAPITAL LETTER EG
                                        0x10c8e, //  OLD HUNGARIAN CAPITAL LETTER EGY
                                        0x10c8f, //  OLD HUNGARIAN CAPITAL LETTER EH
                                        0x10c90, //  OLD HUNGARIAN CAPITAL LETTER I
                                        0x10c91, //  OLD HUNGARIAN CAPITAL LETTER II
                                        0x10c92, //  OLD HUNGARIAN CAPITAL LETTER EJ
                                        0x10c93, //  OLD HUNGARIAN CAPITAL LETTER EK
                                        0x10c94, //  OLD HUNGARIAN CAPITAL LETTER AK
                                        0x10c95, //  OLD HUNGARIAN CAPITAL LETTER UNK
                                        0x10c96, //  OLD HUNGARIAN CAPITAL LETTER EL
                                        0x10c97, //  OLD HUNGARIAN CAPITAL LETTER ELY
                                        0x10c98, //  OLD HUNGARIAN CAPITAL LETTER EM
                                        0x10c99, //  OLD HUNGARIAN CAPITAL LETTER EN
                                        0x10c9a, //  OLD HUNGARIAN CAPITAL LETTER ENY
                                        0x10c9b, //  OLD HUNGARIAN CAPITAL LETTER O
                                        0x10c9c, //  OLD HUNGARIAN CAPITAL LETTER OO
                                        0x10c9d, //  OLD HUNGARIAN CAPITAL LETTER NIKOLSBURG OE
                                        0x10c9e, //  OLD HUNGARIAN CAPITAL LETTER RUDIMENTA OE
                                        0x10c9f, //  OLD HUNGARIAN CAPITAL LETTER OEE
                                        0x10ca0, //  OLD HUNGARIAN CAPITAL LETTER EP
                                        0x10ca1, //  OLD HUNGARIAN CAPITAL LETTER EMP
                                        0x10ca2, //  OLD HUNGARIAN CAPITAL LETTER ER
                                        0x10ca3, //  OLD HUNGARIAN CAPITAL LETTER SHORT ER
                                        0x10ca4, //  OLD HUNGARIAN CAPITAL LETTER ES
                                        0x10ca5, //  OLD HUNGARIAN CAPITAL LETTER ESZ
                                        0x10ca6, //  OLD HUNGARIAN CAPITAL LETTER ET
                                        0x10ca7, //  OLD HUNGARIAN CAPITAL LETTER ENT
                                        0x10ca8, //  OLD HUNGARIAN CAPITAL LETTER ETY
                                        0x10ca9, //  OLD HUNGARIAN CAPITAL LETTER ECH
                                        0x10caa, //  OLD HUNGARIAN CAPITAL LETTER U
                                        0x10cab, //  OLD HUNGARIAN CAPITAL LETTER UU
                                        0x10cac, //  OLD HUNGARIAN CAPITAL LETTER NIKOLSBURG UE
                                        0x10cad, //  OLD HUNGARIAN CAPITAL LETTER RUDIMENTA UE
                                        0x10cae, //  OLD HUNGARIAN CAPITAL LETTER EV
                                        0x10caf, //  OLD HUNGARIAN CAPITAL LETTER EZ
                                        0x10cb0, //  OLD HUNGARIAN CAPITAL LETTER EZS
                                        0x10cb1, //  OLD HUNGARIAN CAPITAL LETTER ENT-SHAPED SIGN
                                        0x10cb2, //  OLD HUNGARIAN CAPITAL LETTER US
                                        0x118a0, //  WARANG CITI CAPITAL LETTER NGAA
                                        0x118a1, //  WARANG CITI CAPITAL LETTER A
                                        0x118a2, //  WARANG CITI CAPITAL LETTER WI
                                        0x118a3, //  WARANG CITI CAPITAL LETTER YU
                                        0x118a4, //  WARANG CITI CAPITAL LETTER YA
                                        0x118a5, //  WARANG CITI CAPITAL LETTER YO
                                        0x118a6, //  WARANG CITI CAPITAL LETTER II
                                        0x118a7, //  WARANG CITI CAPITAL LETTER UU
                                        0x118a8, //  WARANG CITI CAPITAL LETTER E
                                        0x118a9, //  WARANG CITI CAPITAL LETTER O
                                        0x118aa, //  WARANG CITI CAPITAL LETTER ANG
                                        0x118ab, //  WARANG CITI CAPITAL LETTER GA
                                        0x118ac, //  WARANG CITI CAPITAL LETTER KO
                                        0x118ad, //  WARANG CITI CAPITAL LETTER ENY
                                        0x118ae, //  WARANG CITI CAPITAL LETTER YUJ
                                        0x118af, //  WARANG CITI CAPITAL LETTER UC
                                        0x118b0, //  WARANG CITI CAPITAL LETTER ENN
                                        0x118b1, //  WARANG CITI CAPITAL LETTER ODD
                                        0x118b2, //  WARANG CITI CAPITAL LETTER TTE
                                        0x118b3, //  WARANG CITI CAPITAL LETTER NUNG
                                        0x118b4, //  WARANG CITI CAPITAL LETTER DA
                                        0x118b5, //  WARANG CITI CAPITAL LETTER AT
                                        0x118b6, //  WARANG CITI CAPITAL LETTER AM
                                        0x118b7, //  WARANG CITI CAPITAL LETTER BU
                                        0x118b8, //  WARANG CITI CAPITAL LETTER PU
                                        0x118b9, //  WARANG CITI CAPITAL LETTER HIYO
                                        0x118ba, //  WARANG CITI CAPITAL LETTER HOLO
                                        0x118bb, //  WARANG CITI CAPITAL LETTER HORR
                                        0x118bc, //  WARANG CITI CAPITAL LETTER HAR
                                        0x118bd, //  WARANG CITI CAPITAL LETTER SSUU
                                        0x118be, //  WARANG CITI CAPITAL LETTER SII
                                        0x118bf, //  WARANG CITI CAPITAL LETTER VIYO
                                        0x16e40, //  MEDEFAIDRIN CAPITAL LETTER M
                                        0x16e41, //  MEDEFAIDRIN CAPITAL LETTER S
                                        0x16e42, //  MEDEFAIDRIN CAPITAL LETTER V
                                        0x16e43, //  MEDEFAIDRIN CAPITAL LETTER W
                                        0x16e44, //  MEDEFAIDRIN CAPITAL LETTER ATIU
                                        0x16e45, //  MEDEFAIDRIN CAPITAL LETTER Z
                                        0x16e46, //  MEDEFAIDRIN CAPITAL LETTER KP
                                        0x16e47, //  MEDEFAIDRIN CAPITAL LETTER P
                                        0x16e48, //  MEDEFAIDRIN CAPITAL LETTER T
                                        0x16e49, //  MEDEFAIDRIN CAPITAL LETTER G
                                        0x16e4a, //  MEDEFAIDRIN CAPITAL LETTER F
                                        0x16e4b, //  MEDEFAIDRIN CAPITAL LETTER I
                                        0x16e4c, //  MEDEFAIDRIN CAPITAL LETTER K
                                        0x16e4d, //  MEDEFAIDRIN CAPITAL LETTER A
                                        0x16e4e, //  MEDEFAIDRIN CAPITAL LETTER J
                                        0x16e4f, //  MEDEFAIDRIN CAPITAL LETTER E
                                        0x16e50, //  MEDEFAIDRIN CAPITAL LETTER B
                                        0x16e51, //  MEDEFAIDRIN CAPITAL LETTER C
                                        0x16e52, //  MEDEFAIDRIN CAPITAL LETTER U
                                        0x16e53, //  MEDEFAIDRIN CAPITAL LETTER YU
                                        0x16e54, //  MEDEFAIDRIN CAPITAL LETTER L
                                        0x16e55, //  MEDEFAIDRIN CAPITAL LETTER Q
                                        0x16e56, //  MEDEFAIDRIN CAPITAL LETTER HP
                                        0x16e57, //  MEDEFAIDRIN CAPITAL LETTER NY
                                        0x16e58, //  MEDEFAIDRIN CAPITAL LETTER X
                                        0x16e59, //  MEDEFAIDRIN CAPITAL LETTER D
                                        0x16e5a, //  MEDEFAIDRIN CAPITAL LETTER OE
                                        0x16e5b, //  MEDEFAIDRIN CAPITAL LETTER N
                                        0x16e5c, //  MEDEFAIDRIN CAPITAL LETTER R
                                        0x16e5d, //  MEDEFAIDRIN CAPITAL LETTER O
                                        0x16e5e, //  MEDEFAIDRIN CAPITAL LETTER AI
                                        0x16e5f, //  MEDEFAIDRIN CAPITAL LETTER Y
                                        0x1e900, //  ADLAM CAPITAL LETTER ALIF
                                        0x1e901, //  ADLAM CAPITAL LETTER DAALI
                                        0x1e902, //  ADLAM CAPITAL LETTER LAAM
                                        0x1e903, //  ADLAM CAPITAL LETTER MIIM
                                        0x1e904, //  ADLAM CAPITAL LETTER BA
                                        0x1e905, //  ADLAM CAPITAL LETTER SINNYIIYHE
                                        0x1e906, //  ADLAM CAPITAL LETTER PE
                                        0x1e907, //  ADLAM CAPITAL LETTER BHE
                                        0x1e908, //  ADLAM CAPITAL LETTER RA
                                        0x1e909, //  ADLAM CAPITAL LETTER E
                                        0x1e90a, //  ADLAM CAPITAL LETTER FA
                                        0x1e90b, //  ADLAM CAPITAL LETTER I
                                        0x1e90c, //  ADLAM CAPITAL LETTER O
                                        0x1e90d, //  ADLAM CAPITAL LETTER DHA
                                        0x1e90e, //  ADLAM CAPITAL LETTER YHE
                                        0x1e90f, //  ADLAM CAPITAL LETTER WAW
                                        0x1e910, //  ADLAM CAPITAL LETTER NUN
                                        0x1e911, //  ADLAM CAPITAL LETTER KAF
                                        0x1e912, //  ADLAM CAPITAL LETTER YA
                                        0x1e913, //  ADLAM CAPITAL LETTER U
                                        0x1e914, //  ADLAM CAPITAL LETTER JIIM
                                        0x1e915, //  ADLAM CAPITAL LETTER CHI
                                        0x1e916, //  ADLAM CAPITAL LETTER HA
                                        0x1e917, //  ADLAM CAPITAL LETTER QAAF
                                        0x1e918, //  ADLAM CAPITAL LETTER GA
                                        0x1e919, //  ADLAM CAPITAL LETTER NYA
                                        0x1e91a, //  ADLAM CAPITAL LETTER TU
                                        0x1e91b, //  ADLAM CAPITAL LETTER NHA
                                        0x1e91c, //  ADLAM CAPITAL LETTER VA
                                        0x1e91d, //  ADLAM CAPITAL LETTER KHA
                                        0x1e91e, //  ADLAM CAPITAL LETTER GBE
                                        0x1e91f, //  ADLAM CAPITAL LETTER ZAL
                                        0x1e920, //  ADLAM CAPITAL LETTER KPO
                                        0x1e921 }; //  ADLAM CAPITAL LETTER SHA

// Lower case equivalents
static constexpr char32_t lc[1454] = {
    0x00061, 0x00062, 0x00063, 0x00064, 0x00065, 0x00066, 0x00067, 0x00068, 0x00069, 0x0006a, 0x0006b, 0x0006c, 0x0006d, 0x0006e, 0x0006f, 0x00070, 0x00071, 0x00072, 0x00073,
    0x00074, 0x00075, 0x00076, 0x00077, 0x00078, 0x00079, 0x0007a, 0x003bc, 0x000e0, 0x000e1, 0x000e2, 0x000e3, 0x000e4, 0x000e5, 0x000e6, 0x000e7, 0x000e8, 0x000e9, 0x000ea,
    0x000eb, 0x000ec, 0x000ed, 0x000ee, 0x000ef, 0x000f0, 0x000f1, 0x000f2, 0x000f3, 0x000f4, 0x000f5, 0x000f6, 0x000f8, 0x000f9, 0x000fa, 0x000fb, 0x000fc, 0x000fd, 0x000fe,
    0x00101, 0x00103, 0x00105, 0x00107, 0x00109, 0x0010b, 0x0010d, 0x0010f, 0x00111, 0x00113, 0x00115, 0x00117, 0x00119, 0x0011b, 0x0011d, 0x0011f, 0x00121, 0x00123, 0x00125,
    0x00127, 0x00129, 0x0012b, 0x0012d, 0x0012f, 0x00133, 0x00135, 0x00137, 0x0013a, 0x0013c, 0x0013e, 0x00140, 0x00142, 0x00144, 0x00146, 0x00148, 0x0014b, 0x0014d, 0x0014f,
    0x00151, 0x00153, 0x00155, 0x00157, 0x00159, 0x0015b, 0x0015d, 0x0015f, 0x00161, 0x00163, 0x00165, 0x00167, 0x00169, 0x0016b, 0x0016d, 0x0016f, 0x00171, 0x00173, 0x00175,
    0x00177, 0x000ff, 0x0017a, 0x0017c, 0x0017e, 0x00073, 0x00253, 0x00183, 0x00185, 0x00254, 0x00188, 0x00256, 0x00257, 0x0018c, 0x001dd, 0x00259, 0x0025b, 0x00192, 0x00260,
    0x00263, 0x00269, 0x00268, 0x00199, 0x0026f, 0x00272, 0x00275, 0x001a1, 0x001a3, 0x001a5, 0x00280, 0x001a8, 0x00283, 0x001ad, 0x00288, 0x001b0, 0x0028a, 0x0028b, 0x001b4,
    0x001b6, 0x00292, 0x001b9, 0x001bd, 0x001c6, 0x001c6, 0x001c9, 0x001c9, 0x001cc, 0x001cc, 0x001ce, 0x001d0, 0x001d2, 0x001d4, 0x001d6, 0x001d8, 0x001da, 0x001dc, 0x001df,
    0x001e1, 0x001e3, 0x001e5, 0x001e7, 0x001e9, 0x001eb, 0x001ed, 0x001ef, 0x001f3, 0x001f3, 0x001f5, 0x00195, 0x001bf, 0x001f9, 0x001fb, 0x001fd, 0x001ff, 0x00201, 0x00203,
    0x00205, 0x00207, 0x00209, 0x0020b, 0x0020d, 0x0020f, 0x00211, 0x00213, 0x00215, 0x00217, 0x00219, 0x0021b, 0x0021d, 0x0021f, 0x0019e, 0x00223, 0x00225, 0x00227, 0x00229,
    0x0022b, 0x0022d, 0x0022f, 0x00231, 0x00233, 0x02c65, 0x0023c, 0x0019a, 0x02c66, 0x00242, 0x00180, 0x00289, 0x0028c, 0x00247, 0x00249, 0x0024b, 0x0024d, 0x0024f, 0x003b9,
    0x00371, 0x00373, 0x00377, 0x003f3, 0x003ac, 0x003ad, 0x003ae, 0x003af, 0x003cc, 0x003cd, 0x003ce, 0x003b1, 0x003b2, 0x003b3, 0x003b4, 0x003b5, 0x003b6, 0x003b7, 0x003b8,
    0x003b9, 0x003ba, 0x003bb, 0x003bc, 0x003bd, 0x003be, 0x003bf, 0x003c0, 0x003c1, 0x003c3, 0x003c4, 0x003c5, 0x003c6, 0x003c7, 0x003c8, 0x003c9, 0x003ca, 0x003cb, 0x003c3,
    0x003d7, 0x003b2, 0x003b8, 0x003c6, 0x003c0, 0x003d9, 0x003db, 0x003dd, 0x003df, 0x003e1, 0x003e3, 0x003e5, 0x003e7, 0x003e9, 0x003eb, 0x003ed, 0x003ef, 0x003ba, 0x003c1,
    0x003b8, 0x003b5, 0x003f8, 0x003f2, 0x003fb, 0x0037b, 0x0037c, 0x0037d, 0x00450, 0x00451, 0x00452, 0x00453, 0x00454, 0x00455, 0x00456, 0x00457, 0x00458, 0x00459, 0x0045a,
    0x0045b, 0x0045c, 0x0045d, 0x0045e, 0x0045f, 0x00430, 0x00431, 0x00432, 0x00433, 0x00434, 0x00435, 0x00436, 0x00437, 0x00438, 0x00439, 0x0043a, 0x0043b, 0x0043c, 0x0043d,
    0x0043e, 0x0043f, 0x00440, 0x00441, 0x00442, 0x00443, 0x00444, 0x00445, 0x00446, 0x00447, 0x00448, 0x00449, 0x0044a, 0x0044b, 0x0044c, 0x0044d, 0x0044e, 0x0044f, 0x00461,
    0x00463, 0x00465, 0x00467, 0x00469, 0x0046b, 0x0046d, 0x0046f, 0x00471, 0x00473, 0x00475, 0x00477, 0x00479, 0x0047b, 0x0047d, 0x0047f, 0x00481, 0x0048b, 0x0048d, 0x0048f,
    0x00491, 0x00493, 0x00495, 0x00497, 0x00499, 0x0049b, 0x0049d, 0x0049f, 0x004a1, 0x004a3, 0x004a5, 0x004a7, 0x004a9, 0x004ab, 0x004ad, 0x004af, 0x004b1, 0x004b3, 0x004b5,
    0x004b7, 0x004b9, 0x004bb, 0x004bd, 0x004bf, 0x004cf, 0x004c2, 0x004c4, 0x004c6, 0x004c8, 0x004ca, 0x004cc, 0x004ce, 0x004d1, 0x004d3, 0x004d5, 0x004d7, 0x004d9, 0x004db,
    0x004dd, 0x004df, 0x004e1, 0x004e3, 0x004e5, 0x004e7, 0x004e9, 0x004eb, 0x004ed, 0x004ef, 0x004f1, 0x004f3, 0x004f5, 0x004f7, 0x004f9, 0x004fb, 0x004fd, 0x004ff, 0x00501,
    0x00503, 0x00505, 0x00507, 0x00509, 0x0050b, 0x0050d, 0x0050f, 0x00511, 0x00513, 0x00515, 0x00517, 0x00519, 0x0051b, 0x0051d, 0x0051f, 0x00521, 0x00523, 0x00525, 0x00527,
    0x00529, 0x0052b, 0x0052d, 0x0052f, 0x00561, 0x00562, 0x00563, 0x00564, 0x00565, 0x00566, 0x00567, 0x00568, 0x00569, 0x0056a, 0x0056b, 0x0056c, 0x0056d, 0x0056e, 0x0056f,
    0x00570, 0x00571, 0x00572, 0x00573, 0x00574, 0x00575, 0x00576, 0x00577, 0x00578, 0x00579, 0x0057a, 0x0057b, 0x0057c, 0x0057d, 0x0057e, 0x0057f, 0x00580, 0x00581, 0x00582,
    0x00583, 0x00584, 0x00585, 0x00586, 0x02d00, 0x02d01, 0x02d02, 0x02d03, 0x02d04, 0x02d05, 0x02d06, 0x02d07, 0x02d08, 0x02d09, 0x02d0a, 0x02d0b, 0x02d0c, 0x02d0d, 0x02d0e,
    0x02d0f, 0x02d10, 0x02d11, 0x02d12, 0x02d13, 0x02d14, 0x02d15, 0x02d16, 0x02d17, 0x02d18, 0x02d19, 0x02d1a, 0x02d1b, 0x02d1c, 0x02d1d, 0x02d1e, 0x02d1f, 0x02d20, 0x02d21,
    0x02d22, 0x02d23, 0x02d24, 0x02d25, 0x02d27, 0x02d2d, 0x013f0, 0x013f1, 0x013f2, 0x013f3, 0x013f4, 0x013f5, 0x00432, 0x00434, 0x0043e, 0x00441, 0x00442, 0x00442, 0x0044a,
    0x00463, 0x0a64b, 0x010d0, 0x010d1, 0x010d2, 0x010d3, 0x010d4, 0x010d5, 0x010d6, 0x010d7, 0x010d8, 0x010d9, 0x010da, 0x010db, 0x010dc, 0x010dd, 0x010de, 0x010df, 0x010e0,
    0x010e1, 0x010e2, 0x010e3, 0x010e4, 0x010e5, 0x010e6, 0x010e7, 0x010e8, 0x010e9, 0x010ea, 0x010eb, 0x010ec, 0x010ed, 0x010ee, 0x010ef, 0x010f0, 0x010f1, 0x010f2, 0x010f3,
    0x010f4, 0x010f5, 0x010f6, 0x010f7, 0x010f8, 0x010f9, 0x010fa, 0x010fd, 0x010fe, 0x010ff, 0x01e01, 0x01e03, 0x01e05, 0x01e07, 0x01e09, 0x01e0b, 0x01e0d, 0x01e0f, 0x01e11,
    0x01e13, 0x01e15, 0x01e17, 0x01e19, 0x01e1b, 0x01e1d, 0x01e1f, 0x01e21, 0x01e23, 0x01e25, 0x01e27, 0x01e29, 0x01e2b, 0x01e2d, 0x01e2f, 0x01e31, 0x01e33, 0x01e35, 0x01e37,
    0x01e39, 0x01e3b, 0x01e3d, 0x01e3f, 0x01e41, 0x01e43, 0x01e45, 0x01e47, 0x01e49, 0x01e4b, 0x01e4d, 0x01e4f, 0x01e51, 0x01e53, 0x01e55, 0x01e57, 0x01e59, 0x01e5b, 0x01e5d,
    0x01e5f, 0x01e61, 0x01e63, 0x01e65, 0x01e67, 0x01e69, 0x01e6b, 0x01e6d, 0x01e6f, 0x01e71, 0x01e73, 0x01e75, 0x01e77, 0x01e79, 0x01e7b, 0x01e7d, 0x01e7f, 0x01e81, 0x01e83,
    0x01e85, 0x01e87, 0x01e89, 0x01e8b, 0x01e8d, 0x01e8f, 0x01e91, 0x01e93, 0x01e95, 0x01e61, 0x000df, 0x01ea1, 0x01ea3, 0x01ea5, 0x01ea7, 0x01ea9, 0x01eab, 0x01ead, 0x01eaf,
    0x01eb1, 0x01eb3, 0x01eb5, 0x01eb7, 0x01eb9, 0x01ebb, 0x01ebd, 0x01ebf, 0x01ec1, 0x01ec3, 0x01ec5, 0x01ec7, 0x01ec9, 0x01ecb, 0x01ecd, 0x01ecf, 0x01ed1, 0x01ed3, 0x01ed5,
    0x01ed7, 0x01ed9, 0x01edb, 0x01edd, 0x01edf, 0x01ee1, 0x01ee3, 0x01ee5, 0x01ee7, 0x01ee9, 0x01eeb, 0x01eed, 0x01eef, 0x01ef1, 0x01ef3, 0x01ef5, 0x01ef7, 0x01ef9, 0x01efb,
    0x01efd, 0x01eff, 0x01f00, 0x01f01, 0x01f02, 0x01f03, 0x01f04, 0x01f05, 0x01f06, 0x01f07, 0x01f10, 0x01f11, 0x01f12, 0x01f13, 0x01f14, 0x01f15, 0x01f20, 0x01f21, 0x01f22,
    0x01f23, 0x01f24, 0x01f25, 0x01f26, 0x01f27, 0x01f30, 0x01f31, 0x01f32, 0x01f33, 0x01f34, 0x01f35, 0x01f36, 0x01f37, 0x01f40, 0x01f41, 0x01f42, 0x01f43, 0x01f44, 0x01f45,
    0x01f51, 0x01f53, 0x01f55, 0x01f57, 0x01f60, 0x01f61, 0x01f62, 0x01f63, 0x01f64, 0x01f65, 0x01f66, 0x01f67, 0x01f80, 0x01f81, 0x01f82, 0x01f83, 0x01f84, 0x01f85, 0x01f86,
    0x01f87, 0x01f90, 0x01f91, 0x01f92, 0x01f93, 0x01f94, 0x01f95, 0x01f96, 0x01f97, 0x01fa0, 0x01fa1, 0x01fa2, 0x01fa3, 0x01fa4, 0x01fa5, 0x01fa6, 0x01fa7, 0x01fb0, 0x01fb1,
    0x01f70, 0x01f71, 0x01fb3, 0x003b9, 0x01f72, 0x01f73, 0x01f74, 0x01f75, 0x01fc3, 0x01fd0, 0x01fd1, 0x01f76, 0x01f77, 0x01fe0, 0x01fe1, 0x01f7a, 0x01f7b, 0x01fe5, 0x01f78,
    0x01f79, 0x01f7c, 0x01f7d, 0x01ff3, 0x003c9, 0x0006b, 0x000e5, 0x0214e, 0x02170, 0x02171, 0x02172, 0x02173, 0x02174, 0x02175, 0x02176, 0x02177, 0x02178, 0x02179, 0x0217a,
    0x0217b, 0x0217c, 0x0217d, 0x0217e, 0x0217f, 0x02184, 0x024d0, 0x024d1, 0x024d2, 0x024d3, 0x024d4, 0x024d5, 0x024d6, 0x024d7, 0x024d8, 0x024d9, 0x024da, 0x024db, 0x024dc,
    0x024dd, 0x024de, 0x024df, 0x024e0, 0x024e1, 0x024e2, 0x024e3, 0x024e4, 0x024e5, 0x024e6, 0x024e7, 0x024e8, 0x024e9, 0x02c30, 0x02c31, 0x02c32, 0x02c33, 0x02c34, 0x02c35,
    0x02c36, 0x02c37, 0x02c38, 0x02c39, 0x02c3a, 0x02c3b, 0x02c3c, 0x02c3d, 0x02c3e, 0x02c3f, 0x02c40, 0x02c41, 0x02c42, 0x02c43, 0x02c44, 0x02c45, 0x02c46, 0x02c47, 0x02c48,
    0x02c49, 0x02c4a, 0x02c4b, 0x02c4c, 0x02c4d, 0x02c4e, 0x02c4f, 0x02c50, 0x02c51, 0x02c52, 0x02c53, 0x02c54, 0x02c55, 0x02c56, 0x02c57, 0x02c58, 0x02c59, 0x02c5a, 0x02c5b,
    0x02c5c, 0x02c5d, 0x02c5e, 0x02c5f, 0x02c61, 0x0026b, 0x01d7d, 0x0027d, 0x02c68, 0x02c6a, 0x02c6c, 0x00251, 0x00271, 0x00250, 0x00252, 0x02c73, 0x02c76, 0x0023f, 0x00240,
    0x02c81, 0x02c83, 0x02c85, 0x02c87, 0x02c89, 0x02c8b, 0x02c8d, 0x02c8f, 0x02c91, 0x02c93, 0x02c95, 0x02c97, 0x02c99, 0x02c9b, 0x02c9d, 0x02c9f, 0x02ca1, 0x02ca3, 0x02ca5,
    0x02ca7, 0x02ca9, 0x02cab, 0x02cad, 0x02caf, 0x02cb1, 0x02cb3, 0x02cb5, 0x02cb7, 0x02cb9, 0x02cbb, 0x02cbd, 0x02cbf, 0x02cc1, 0x02cc3, 0x02cc5, 0x02cc7, 0x02cc9, 0x02ccb,
    0x02ccd, 0x02ccf, 0x02cd1, 0x02cd3, 0x02cd5, 0x02cd7, 0x02cd9, 0x02cdb, 0x02cdd, 0x02cdf, 0x02ce1, 0x02ce3, 0x02cec, 0x02cee, 0x02cf3, 0x0a641, 0x0a643, 0x0a645, 0x0a647,
    0x0a649, 0x0a64b, 0x0a64d, 0x0a64f, 0x0a651, 0x0a653, 0x0a655, 0x0a657, 0x0a659, 0x0a65b, 0x0a65d, 0x0a65f, 0x0a661, 0x0a663, 0x0a665, 0x0a667, 0x0a669, 0x0a66b, 0x0a66d,
    0x0a681, 0x0a683, 0x0a685, 0x0a687, 0x0a689, 0x0a68b, 0x0a68d, 0x0a68f, 0x0a691, 0x0a693, 0x0a695, 0x0a697, 0x0a699, 0x0a69b, 0x0a723, 0x0a725, 0x0a727, 0x0a729, 0x0a72b,
    0x0a72d, 0x0a72f, 0x0a733, 0x0a735, 0x0a737, 0x0a739, 0x0a73b, 0x0a73d, 0x0a73f, 0x0a741, 0x0a743, 0x0a745, 0x0a747, 0x0a749, 0x0a74b, 0x0a74d, 0x0a74f, 0x0a751, 0x0a753,
    0x0a755, 0x0a757, 0x0a759, 0x0a75b, 0x0a75d, 0x0a75f, 0x0a761, 0x0a763, 0x0a765, 0x0a767, 0x0a769, 0x0a76b, 0x0a76d, 0x0a76f, 0x0a77a, 0x0a77c, 0x01d79, 0x0a77f, 0x0a781,
    0x0a783, 0x0a785, 0x0a787, 0x0a78c, 0x00265, 0x0a791, 0x0a793, 0x0a797, 0x0a799, 0x0a79b, 0x0a79d, 0x0a79f, 0x0a7a1, 0x0a7a3, 0x0a7a5, 0x0a7a7, 0x0a7a9, 0x00266, 0x0025c,
    0x00261, 0x0026c, 0x0026a, 0x0029e, 0x00287, 0x0029d, 0x0ab53, 0x0a7b5, 0x0a7b7, 0x0a7b9, 0x0a7bb, 0x0a7bd, 0x0a7bf, 0x0a7c1, 0x0a7c3, 0x0a794, 0x00282, 0x01d8e, 0x0a7c8,
    0x0a7ca, 0x0a7d1, 0x0a7d7, 0x0a7d9, 0x0a7f6, 0x013a0, 0x013a1, 0x013a2, 0x013a3, 0x013a4, 0x013a5, 0x013a6, 0x013a7, 0x013a8, 0x013a9, 0x013aa, 0x013ab, 0x013ac, 0x013ad,
    0x013ae, 0x013af, 0x013b0, 0x013b1, 0x013b2, 0x013b3, 0x013b4, 0x013b5, 0x013b6, 0x013b7, 0x013b8, 0x013b9, 0x013ba, 0x013bb, 0x013bc, 0x013bd, 0x013be, 0x013bf, 0x013c0,
    0x013c1, 0x013c2, 0x013c3, 0x013c4, 0x013c5, 0x013c6, 0x013c7, 0x013c8, 0x013c9, 0x013ca, 0x013cb, 0x013cc, 0x013cd, 0x013ce, 0x013cf, 0x013d0, 0x013d1, 0x013d2, 0x013d3,
    0x013d4, 0x013d5, 0x013d6, 0x013d7, 0x013d8, 0x013d9, 0x013da, 0x013db, 0x013dc, 0x013dd, 0x013de, 0x013df, 0x013e0, 0x013e1, 0x013e2, 0x013e3, 0x013e4, 0x013e5, 0x013e6,
    0x013e7, 0x013e8, 0x013e9, 0x013ea, 0x013eb, 0x013ec, 0x013ed, 0x013ee, 0x013ef, 0x0ff41, 0x0ff42, 0x0ff43, 0x0ff44, 0x0ff45, 0x0ff46, 0x0ff47, 0x0ff48, 0x0ff49, 0x0ff4a,
    0x0ff4b, 0x0ff4c, 0x0ff4d, 0x0ff4e, 0x0ff4f, 0x0ff50, 0x0ff51, 0x0ff52, 0x0ff53, 0x0ff54, 0x0ff55, 0x0ff56, 0x0ff57, 0x0ff58, 0x0ff59, 0x0ff5a, 0x10428, 0x10429, 0x1042a,
    0x1042b, 0x1042c, 0x1042d, 0x1042e, 0x1042f, 0x10430, 0x10431, 0x10432, 0x10433, 0x10434, 0x10435, 0x10436, 0x10437, 0x10438, 0x10439, 0x1043a, 0x1043b, 0x1043c, 0x1043d,
    0x1043e, 0x1043f, 0x10440, 0x10441, 0x10442, 0x10443, 0x10444, 0x10445, 0x10446, 0x10447, 0x10448, 0x10449, 0x1044a, 0x1044b, 0x1044c, 0x1044d, 0x1044e, 0x1044f, 0x104d8,
    0x104d9, 0x104da, 0x104db, 0x104dc, 0x104dd, 0x104de, 0x104df, 0x104e0, 0x104e1, 0x104e2, 0x104e3, 0x104e4, 0x104e5, 0x104e6, 0x104e7, 0x104e8, 0x104e9, 0x104ea, 0x104eb,
    0x104ec, 0x104ed, 0x104ee, 0x104ef, 0x104f0, 0x104f1, 0x104f2, 0x104f3, 0x104f4, 0x104f5, 0x104f6, 0x104f7, 0x104f8, 0x104f9, 0x104fa, 0x104fb, 0x10597, 0x10598, 0x10599,
    0x1059a, 0x1059b, 0x1059c, 0x1059d, 0x1059e, 0x1059f, 0x105a0, 0x105a1, 0x105a3, 0x105a4, 0x105a5, 0x105a6, 0x105a7, 0x105a8, 0x105a9, 0x105aa, 0x105ab, 0x105ac, 0x105ad,
    0x105ae, 0x105af, 0x105b0, 0x105b1, 0x105b3, 0x105b4, 0x105b5, 0x105b6, 0x105b7, 0x105b8, 0x105b9, 0x105bb, 0x105bc, 0x10cc0, 0x10cc1, 0x10cc2, 0x10cc3, 0x10cc4, 0x10cc5,
    0x10cc6, 0x10cc7, 0x10cc8, 0x10cc9, 0x10cca, 0x10ccb, 0x10ccc, 0x10ccd, 0x10cce, 0x10ccf, 0x10cd0, 0x10cd1, 0x10cd2, 0x10cd3, 0x10cd4, 0x10cd5, 0x10cd6, 0x10cd7, 0x10cd8,
    0x10cd9, 0x10cda, 0x10cdb, 0x10cdc, 0x10cdd, 0x10cde, 0x10cdf, 0x10ce0, 0x10ce1, 0x10ce2, 0x10ce3, 0x10ce4, 0x10ce5, 0x10ce6, 0x10ce7, 0x10ce8, 0x10ce9, 0x10cea, 0x10ceb,
    0x10cec, 0x10ced, 0x10cee, 0x10cef, 0x10cf0, 0x10cf1, 0x10cf2, 0x118c0, 0x118c1, 0x118c2, 0x118c3, 0x118c4, 0x118c5, 0x118c6, 0x118c7, 0x118c8, 0x118c9, 0x118ca, 0x118cb,
    0x118cc, 0x118cd, 0x118ce, 0x118cf, 0x118d0, 0x118d1, 0x118d2, 0x118d3, 0x118d4, 0x118d5, 0x118d6, 0x118d7, 0x118d8, 0x118d9, 0x118da, 0x118db, 0x118dc, 0x118dd, 0x118de,
    0x118df, 0x16e60, 0x16e61, 0x16e62, 0x16e63, 0x16e64, 0x16e65, 0x16e66, 0x16e67, 0x16e68, 0x16e69, 0x16e6a, 0x16e6b, 0x16e6c, 0x16e6d, 0x16e6e, 0x16e6f, 0x16e70, 0x16e71,
    0x16e72, 0x16e73, 0x16e74, 0x16e75, 0x16e76, 0x16e77, 0x16e78, 0x16e79, 0x16e7a, 0x16e7b, 0x16e7c, 0x16e7d, 0x16e7e, 0x16e7f, 0x1e922, 0x1e923, 0x1e924, 0x1e925, 0x1e926,
    0x1e927, 0x1e928, 0x1e929, 0x1e92a, 0x1e92b, 0x1e92c, 0x1e92d, 0x1e92e, 0x1e92f, 0x1e930, 0x1e931, 0x1e932, 0x1e933, 0x1e934, 0x1e935, 0x1e936, 0x1e937, 0x1e938, 0x1e939,
    0x1e93a, 0x1e93b, 0x1e93c, 0x1e93d, 0x1e93e, 0x1e93f, 0x1e940, 0x1e941, 0x1e942, 0x1e943
};

// definition of 'l2u' and 'uc' tables
// Lower case table
static constexpr char32_t l2u[1427] = {
    0x00061, 0x00062, 0x00063, 0x00064, 0x00065, 0x00066, 0x00067, 0x00068, 0x00069, 0x0006a, 0x0006b, 0x0006c, 0x0006d, 0x0006e, 0x0006f, 0x00070, 0x00071, 0x00072, 0x00073,
    0x00074, 0x00075, 0x00076, 0x00077, 0x00078, 0x00079, 0x0007a, 0x000df, 0x000e0, 0x000e1, 0x000e2, 0x000e3, 0x000e4, 0x000e5, 0x000e6, 0x000e7, 0x000e8, 0x000e9, 0x000ea,
    0x000eb, 0x000ec, 0x000ed, 0x000ee, 0x000ef, 0x000f0, 0x000f1, 0x000f2, 0x000f3, 0x000f4, 0x000f5, 0x000f6, 0x000f8, 0x000f9, 0x000fa, 0x000fb, 0x000fc, 0x000fd, 0x000fe,
    0x000ff, 0x00101, 0x00103, 0x00105, 0x00107, 0x00109, 0x0010b, 0x0010d, 0x0010f, 0x00111, 0x00113, 0x00115, 0x00117, 0x00119, 0x0011b, 0x0011d, 0x0011f, 0x00121, 0x00123,
    0x00125, 0x00127, 0x00129, 0x0012b, 0x0012d, 0x0012f, 0x00133, 0x00135, 0x00137, 0x0013a, 0x0013c, 0x0013e, 0x00140, 0x00142, 0x00144, 0x00146, 0x00148, 0x0014b, 0x0014d,
    0x0014f, 0x00151, 0x00153, 0x00155, 0x00157, 0x00159, 0x0015b, 0x0015d, 0x0015f, 0x00161, 0x00163, 0x00165, 0x00167, 0x00169, 0x0016b, 0x0016d, 0x0016f, 0x00171, 0x00173,
    0x00175, 0x00177, 0x0017a, 0x0017c, 0x0017e, 0x00180, 0x00183, 0x00185, 0x00188, 0x0018c, 0x00192, 0x00195, 0x00199, 0x0019a, 0x0019e, 0x001a1, 0x001a3, 0x001a5, 0x001a8,
    0x001ad, 0x001b0, 0x001b4, 0x001b6, 0x001b9, 0x001bd, 0x001bf, 0x001c6, 0x001c9, 0x001cc, 0x001ce, 0x001d0, 0x001d2, 0x001d4, 0x001d6, 0x001d8, 0x001da, 0x001dc, 0x001dd,
    0x001df, 0x001e1, 0x001e3, 0x001e5, 0x001e7, 0x001e9, 0x001eb, 0x001ed, 0x001ef, 0x001f3, 0x001f5, 0x001f9, 0x001fb, 0x001fd, 0x001ff, 0x00201, 0x00203, 0x00205, 0x00207,
    0x00209, 0x0020b, 0x0020d, 0x0020f, 0x00211, 0x00213, 0x00215, 0x00217, 0x00219, 0x0021b, 0x0021d, 0x0021f, 0x00223, 0x00225, 0x00227, 0x00229, 0x0022b, 0x0022d, 0x0022f,
    0x00231, 0x00233, 0x0023c, 0x0023f, 0x00240, 0x00242, 0x00247, 0x00249, 0x0024b, 0x0024d, 0x0024f, 0x00250, 0x00251, 0x00252, 0x00253, 0x00254, 0x00256, 0x00257, 0x00259,
    0x0025b, 0x0025c, 0x00260, 0x00261, 0x00263, 0x00265, 0x00266, 0x00268, 0x00269, 0x0026a, 0x0026b, 0x0026c, 0x0026f, 0x00271, 0x00272, 0x00275, 0x0027d, 0x00280, 0x00282,
    0x00283, 0x00287, 0x00288, 0x00289, 0x0028a, 0x0028b, 0x0028c, 0x00292, 0x0029d, 0x0029e, 0x00371, 0x00373, 0x00377, 0x0037b, 0x0037c, 0x0037d, 0x003ac, 0x003ad, 0x003ae,
    0x003af, 0x003b1, 0x003b2, 0x003b3, 0x003b4, 0x003b5, 0x003b6, 0x003b7, 0x003b8, 0x003b8, 0x003b9, 0x003b9, 0x003ba, 0x003bb, 0x003bc, 0x003bd, 0x003be, 0x003bf, 0x003c0,
    0x003c1, 0x003c3, 0x003c4, 0x003c5, 0x003c6, 0x003c7, 0x003c8, 0x003c9, 0x003ca, 0x003cb, 0x003cc, 0x003cd, 0x003ce, 0x003d7, 0x003d9, 0x003db, 0x003dd, 0x003df, 0x003e1,
    0x003e3, 0x003e5, 0x003e7, 0x003e9, 0x003eb, 0x003ed, 0x003ef, 0x003f2, 0x003f3, 0x003f8, 0x003fb, 0x00430, 0x00431, 0x00432, 0x00433, 0x00434, 0x00435, 0x00436, 0x00437,
    0x00438, 0x00439, 0x0043a, 0x0043b, 0x0043c, 0x0043d, 0x0043e, 0x0043f, 0x00440, 0x00441, 0x00442, 0x00442, 0x00443, 0x00444, 0x00445, 0x00446, 0x00447, 0x00448, 0x00449,
    0x0044a, 0x0044b, 0x0044c, 0x0044d, 0x0044e, 0x0044f, 0x00450, 0x00451, 0x00452, 0x00453, 0x00454, 0x00455, 0x00456, 0x00457, 0x00458, 0x00459, 0x0045a, 0x0045b, 0x0045c,
    0x0045d, 0x0045e, 0x0045f, 0x00461, 0x00463, 0x00465, 0x00467, 0x00469, 0x0046b, 0x0046d, 0x0046f, 0x00471, 0x00473, 0x00475, 0x00477, 0x00479, 0x0047b, 0x0047d, 0x0047f,
    0x00481, 0x0048b, 0x0048d, 0x0048f, 0x00491, 0x00493, 0x00495, 0x00497, 0x00499, 0x0049b, 0x0049d, 0x0049f, 0x004a1, 0x004a3, 0x004a5, 0x004a7, 0x004a9, 0x004ab, 0x004ad,
    0x004af, 0x004b1, 0x004b3, 0x004b5, 0x004b7, 0x004b9, 0x004bb, 0x004bd, 0x004bf, 0x004c2, 0x004c4, 0x004c6, 0x004c8, 0x004ca, 0x004cc, 0x004ce, 0x004cf, 0x004d1, 0x004d3,
    0x004d5, 0x004d7, 0x004d9, 0x004db, 0x004dd, 0x004df, 0x004e1, 0x004e3, 0x004e5, 0x004e7, 0x004e9, 0x004eb, 0x004ed, 0x004ef, 0x004f1, 0x004f3, 0x004f5, 0x004f7, 0x004f9,
    0x004fb, 0x004fd, 0x004ff, 0x00501, 0x00503, 0x00505, 0x00507, 0x00509, 0x0050b, 0x0050d, 0x0050f, 0x00511, 0x00513, 0x00515, 0x00517, 0x00519, 0x0051b, 0x0051d, 0x0051f,
    0x00521, 0x00523, 0x00525, 0x00527, 0x00529, 0x0052b, 0x0052d, 0x0052f, 0x00561, 0x00562, 0x00563, 0x00564, 0x00565, 0x00566, 0x00567, 0x00568, 0x00569, 0x0056a, 0x0056b,
    0x0056c, 0x0056d, 0x0056e, 0x0056f, 0x00570, 0x00571, 0x00572, 0x00573, 0x00574, 0x00575, 0x00576, 0x00577, 0x00578, 0x00579, 0x0057a, 0x0057b, 0x0057c, 0x0057d, 0x0057e,
    0x0057f, 0x00580, 0x00581, 0x00582, 0x00583, 0x00584, 0x00585, 0x00586, 0x010d0, 0x010d1, 0x010d2, 0x010d3, 0x010d4, 0x010d5, 0x010d6, 0x010d7, 0x010d8, 0x010d9, 0x010da,
    0x010db, 0x010dc, 0x010dd, 0x010de, 0x010df, 0x010e0, 0x010e1, 0x010e2, 0x010e3, 0x010e4, 0x010e5, 0x010e6, 0x010e7, 0x010e8, 0x010e9, 0x010ea, 0x010eb, 0x010ec, 0x010ed,
    0x010ee, 0x010ef, 0x010f0, 0x010f1, 0x010f2, 0x010f3, 0x010f4, 0x010f5, 0x010f6, 0x010f7, 0x010f8, 0x010f9, 0x010fa, 0x010fd, 0x010fe, 0x010ff, 0x013a0, 0x013a1, 0x013a2,
    0x013a3, 0x013a4, 0x013a5, 0x013a6, 0x013a7, 0x013a8, 0x013a9, 0x013aa, 0x013ab, 0x013ac, 0x013ad, 0x013ae, 0x013af, 0x013b0, 0x013b1, 0x013b2, 0x013b3, 0x013b4, 0x013b5,
    0x013b6, 0x013b7, 0x013b8, 0x013b9, 0x013ba, 0x013bb, 0x013bc, 0x013bd, 0x013be, 0x013bf, 0x013c0, 0x013c1, 0x013c2, 0x013c3, 0x013c4, 0x013c5, 0x013c6, 0x013c7, 0x013c8,
    0x013c9, 0x013ca, 0x013cb, 0x013cc, 0x013cd, 0x013ce, 0x013cf, 0x013d0, 0x013d1, 0x013d2, 0x013d3, 0x013d4, 0x013d5, 0x013d6, 0x013d7, 0x013d8, 0x013d9, 0x013da, 0x013db,
    0x013dc, 0x013dd, 0x013de, 0x013df, 0x013e0, 0x013e1, 0x013e2, 0x013e3, 0x013e4, 0x013e5, 0x013e6, 0x013e7, 0x013e8, 0x013e9, 0x013ea, 0x013eb, 0x013ec, 0x013ed, 0x013ee,
    0x013ef, 0x013f0, 0x013f1, 0x013f2, 0x013f3, 0x013f4, 0x013f5, 0x01d79, 0x01d7d, 0x01d8e, 0x01e01, 0x01e03, 0x01e05, 0x01e07, 0x01e09, 0x01e0b, 0x01e0d, 0x01e0f, 0x01e11,
    0x01e13, 0x01e15, 0x01e17, 0x01e19, 0x01e1b, 0x01e1d, 0x01e1f, 0x01e21, 0x01e23, 0x01e25, 0x01e27, 0x01e29, 0x01e2b, 0x01e2d, 0x01e2f, 0x01e31, 0x01e33, 0x01e35, 0x01e37,
    0x01e39, 0x01e3b, 0x01e3d, 0x01e3f, 0x01e41, 0x01e43, 0x01e45, 0x01e47, 0x01e49, 0x01e4b, 0x01e4d, 0x01e4f, 0x01e51, 0x01e53, 0x01e55, 0x01e57, 0x01e59, 0x01e5b, 0x01e5d,
    0x01e5f, 0x01e61, 0x01e63, 0x01e65, 0x01e67, 0x01e69, 0x01e6b, 0x01e6d, 0x01e6f, 0x01e71, 0x01e73, 0x01e75, 0x01e77, 0x01e79, 0x01e7b, 0x01e7d, 0x01e7f, 0x01e81, 0x01e83,
    0x01e85, 0x01e87, 0x01e89, 0x01e8b, 0x01e8d, 0x01e8f, 0x01e91, 0x01e93, 0x01e95, 0x01ea1, 0x01ea3, 0x01ea5, 0x01ea7, 0x01ea9, 0x01eab, 0x01ead, 0x01eaf, 0x01eb1, 0x01eb3,
    0x01eb5, 0x01eb7, 0x01eb9, 0x01ebb, 0x01ebd, 0x01ebf, 0x01ec1, 0x01ec3, 0x01ec5, 0x01ec7, 0x01ec9, 0x01ecb, 0x01ecd, 0x01ecf, 0x01ed1, 0x01ed3, 0x01ed5, 0x01ed7, 0x01ed9,
    0x01edb, 0x01edd, 0x01edf, 0x01ee1, 0x01ee3, 0x01ee5, 0x01ee7, 0x01ee9, 0x01eeb, 0x01eed, 0x01eef, 0x01ef1, 0x01ef3, 0x01ef5, 0x01ef7, 0x01ef9, 0x01efb, 0x01efd, 0x01eff,
    0x01f00, 0x01f01, 0x01f02, 0x01f03, 0x01f04, 0x01f05, 0x01f06, 0x01f07, 0x01f10, 0x01f11, 0x01f12, 0x01f13, 0x01f14, 0x01f15, 0x01f20, 0x01f21, 0x01f22, 0x01f23, 0x01f24,
    0x01f25, 0x01f26, 0x01f27, 0x01f30, 0x01f31, 0x01f32, 0x01f33, 0x01f34, 0x01f35, 0x01f36, 0x01f37, 0x01f40, 0x01f41, 0x01f42, 0x01f43, 0x01f44, 0x01f45, 0x01f51, 0x01f53,
    0x01f55, 0x01f57, 0x01f60, 0x01f61, 0x01f62, 0x01f63, 0x01f64, 0x01f65, 0x01f66, 0x01f67, 0x01f70, 0x01f71, 0x01f72, 0x01f73, 0x01f74, 0x01f75, 0x01f76, 0x01f77, 0x01f78,
    0x01f79, 0x01f7a, 0x01f7b, 0x01f7c, 0x01f7d, 0x01f80, 0x01f81, 0x01f82, 0x01f83, 0x01f84, 0x01f85, 0x01f86, 0x01f87, 0x01f90, 0x01f91, 0x01f92, 0x01f93, 0x01f94, 0x01f95,
    0x01f96, 0x01f97, 0x01fa0, 0x01fa1, 0x01fa2, 0x01fa3, 0x01fa4, 0x01fa5, 0x01fa6, 0x01fa7, 0x01fb0, 0x01fb1, 0x01fb3, 0x01fc3, 0x01fd0, 0x01fd1, 0x01fe0, 0x01fe1, 0x01fe5,
    0x01ff3, 0x0214e, 0x02170, 0x02171, 0x02172, 0x02173, 0x02174, 0x02175, 0x02176, 0x02177, 0x02178, 0x02179, 0x0217a, 0x0217b, 0x0217c, 0x0217d, 0x0217e, 0x0217f, 0x02184,
    0x024d0, 0x024d1, 0x024d2, 0x024d3, 0x024d4, 0x024d5, 0x024d6, 0x024d7, 0x024d8, 0x024d9, 0x024da, 0x024db, 0x024dc, 0x024dd, 0x024de, 0x024df, 0x024e0, 0x024e1, 0x024e2,
    0x024e3, 0x024e4, 0x024e5, 0x024e6, 0x024e7, 0x024e8, 0x024e9, 0x02c30, 0x02c31, 0x02c32, 0x02c33, 0x02c34, 0x02c35, 0x02c36, 0x02c37, 0x02c38, 0x02c39, 0x02c3a, 0x02c3b,
    0x02c3c, 0x02c3d, 0x02c3e, 0x02c3f, 0x02c40, 0x02c41, 0x02c42, 0x02c43, 0x02c44, 0x02c45, 0x02c46, 0x02c47, 0x02c48, 0x02c49, 0x02c4a, 0x02c4b, 0x02c4c, 0x02c4d, 0x02c4e,
    0x02c4f, 0x02c50, 0x02c51, 0x02c52, 0x02c53, 0x02c54, 0x02c55, 0x02c56, 0x02c57, 0x02c58, 0x02c59, 0x02c5a, 0x02c5b, 0x02c5c, 0x02c5d, 0x02c5e, 0x02c5f, 0x02c61, 0x02c65,
    0x02c66, 0x02c68, 0x02c6a, 0x02c6c, 0x02c73, 0x02c76, 0x02c81, 0x02c83, 0x02c85, 0x02c87, 0x02c89, 0x02c8b, 0x02c8d, 0x02c8f, 0x02c91, 0x02c93, 0x02c95, 0x02c97, 0x02c99,
    0x02c9b, 0x02c9d, 0x02c9f, 0x02ca1, 0x02ca3, 0x02ca5, 0x02ca7, 0x02ca9, 0x02cab, 0x02cad, 0x02caf, 0x02cb1, 0x02cb3, 0x02cb5, 0x02cb7, 0x02cb9, 0x02cbb, 0x02cbd, 0x02cbf,
    0x02cc1, 0x02cc3, 0x02cc5, 0x02cc7, 0x02cc9, 0x02ccb, 0x02ccd, 0x02ccf, 0x02cd1, 0x02cd3, 0x02cd5, 0x02cd7, 0x02cd9, 0x02cdb, 0x02cdd, 0x02cdf, 0x02ce1, 0x02ce3, 0x02cec,
    0x02cee, 0x02cf3, 0x02d00, 0x02d01, 0x02d02, 0x02d03, 0x02d04, 0x02d05, 0x02d06, 0x02d07, 0x02d08, 0x02d09, 0x02d0a, 0x02d0b, 0x02d0c, 0x02d0d, 0x02d0e, 0x02d0f, 0x02d10,
    0x02d11, 0x02d12, 0x02d13, 0x02d14, 0x02d15, 0x02d16, 0x02d17, 0x02d18, 0x02d19, 0x02d1a, 0x02d1b, 0x02d1c, 0x02d1d, 0x02d1e, 0x02d1f, 0x02d20, 0x02d21, 0x02d22, 0x02d23,
    0x02d24, 0x02d25, 0x02d27, 0x02d2d, 0x0a641, 0x0a643, 0x0a645, 0x0a647, 0x0a649, 0x0a64b, 0x0a64d, 0x0a64f, 0x0a651, 0x0a653, 0x0a655, 0x0a657, 0x0a659, 0x0a65b, 0x0a65d,
    0x0a65f, 0x0a661, 0x0a663, 0x0a665, 0x0a667, 0x0a669, 0x0a66b, 0x0a66d, 0x0a681, 0x0a683, 0x0a685, 0x0a687, 0x0a689, 0x0a68b, 0x0a68d, 0x0a68f, 0x0a691, 0x0a693, 0x0a695,
    0x0a697, 0x0a699, 0x0a69b, 0x0a723, 0x0a725, 0x0a727, 0x0a729, 0x0a72b, 0x0a72d, 0x0a72f, 0x0a733, 0x0a735, 0x0a737, 0x0a739, 0x0a73b, 0x0a73d, 0x0a73f, 0x0a741, 0x0a743,
    0x0a745, 0x0a747, 0x0a749, 0x0a74b, 0x0a74d, 0x0a74f, 0x0a751, 0x0a753, 0x0a755, 0x0a757, 0x0a759, 0x0a75b, 0x0a75d, 0x0a75f, 0x0a761, 0x0a763, 0x0a765, 0x0a767, 0x0a769,
    0x0a76b, 0x0a76d, 0x0a76f, 0x0a77a, 0x0a77c, 0x0a77f, 0x0a781, 0x0a783, 0x0a785, 0x0a787, 0x0a78c, 0x0a791, 0x0a793, 0x0a794, 0x0a797, 0x0a799, 0x0a79b, 0x0a79d, 0x0a79f,
    0x0a7a1, 0x0a7a3, 0x0a7a5, 0x0a7a7, 0x0a7a9, 0x0a7b5, 0x0a7b7, 0x0a7b9, 0x0a7bb, 0x0a7bd, 0x0a7bf, 0x0a7c1, 0x0a7c3, 0x0a7c8, 0x0a7ca, 0x0a7d1, 0x0a7d7, 0x0a7d9, 0x0a7f6,
    0x0ab53, 0x0ff41, 0x0ff42, 0x0ff43, 0x0ff44, 0x0ff45, 0x0ff46, 0x0ff47, 0x0ff48, 0x0ff49, 0x0ff4a, 0x0ff4b, 0x0ff4c, 0x0ff4d, 0x0ff4e, 0x0ff4f, 0x0ff50, 0x0ff51, 0x0ff52,
    0x0ff53, 0x0ff54, 0x0ff55, 0x0ff56, 0x0ff57, 0x0ff58, 0x0ff59, 0x0ff5a, 0x10428, 0x10429, 0x1042a, 0x1042b, 0x1042c, 0x1042d, 0x1042e, 0x1042f, 0x10430, 0x10431, 0x10432,
    0x10433, 0x10434, 0x10435, 0x10436, 0x10437, 0x10438, 0x10439, 0x1043a, 0x1043b, 0x1043c, 0x1043d, 0x1043e, 0x1043f, 0x10440, 0x10441, 0x10442, 0x10443, 0x10444, 0x10445,
    0x10446, 0x10447, 0x10448, 0x10449, 0x1044a, 0x1044b, 0x1044c, 0x1044d, 0x1044e, 0x1044f, 0x104d8, 0x104d9, 0x104da, 0x104db, 0x104dc, 0x104dd, 0x104de, 0x104df, 0x104e0,
    0x104e1, 0x104e2, 0x104e3, 0x104e4, 0x104e5, 0x104e6, 0x104e7, 0x104e8, 0x104e9, 0x104ea, 0x104eb, 0x104ec, 0x104ed, 0x104ee, 0x104ef, 0x104f0, 0x104f1, 0x104f2, 0x104f3,
    0x104f4, 0x104f5, 0x104f6, 0x104f7, 0x104f8, 0x104f9, 0x104fa, 0x104fb, 0x10597, 0x10598, 0x10599, 0x1059a, 0x1059b, 0x1059c, 0x1059d, 0x1059e, 0x1059f, 0x105a0, 0x105a1,
    0x105a3, 0x105a4, 0x105a5, 0x105a6, 0x105a7, 0x105a8, 0x105a9, 0x105aa, 0x105ab, 0x105ac, 0x105ad, 0x105ae, 0x105af, 0x105b0, 0x105b1, 0x105b3, 0x105b4, 0x105b5, 0x105b6,
    0x105b7, 0x105b8, 0x105b9, 0x105bb, 0x105bc, 0x10cc0, 0x10cc1, 0x10cc2, 0x10cc3, 0x10cc4, 0x10cc5, 0x10cc6, 0x10cc7, 0x10cc8, 0x10cc9, 0x10cca, 0x10ccb, 0x10ccc, 0x10ccd,
    0x10cce, 0x10ccf, 0x10cd0, 0x10cd1, 0x10cd2, 0x10cd3, 0x10cd4, 0x10cd5, 0x10cd6, 0x10cd7, 0x10cd8, 0x10cd9, 0x10cda, 0x10cdb, 0x10cdc, 0x10cdd, 0x10cde, 0x10cdf, 0x10ce0,
    0x10ce1, 0x10ce2, 0x10ce3, 0x10ce4, 0x10ce5, 0x10ce6, 0x10ce7, 0x10ce8, 0x10ce9, 0x10cea, 0x10ceb, 0x10cec, 0x10ced, 0x10cee, 0x10cef, 0x10cf0, 0x10cf1, 0x10cf2, 0x118c0,
    0x118c1, 0x118c2, 0x118c3, 0x118c4, 0x118c5, 0x118c6, 0x118c7, 0x118c8, 0x118c9, 0x118ca, 0x118cb, 0x118cc, 0x118cd, 0x118ce, 0x118cf, 0x118d0, 0x118d1, 0x118d2, 0x118d3,
    0x118d4, 0x118d5, 0x118d6, 0x118d7, 0x118d8, 0x118d9, 0x118da, 0x118db, 0x118dc, 0x118dd, 0x118de, 0x118df, 0x16e60, 0x16e61, 0x16e62, 0x16e63, 0x16e64, 0x16e65, 0x16e66,
    0x16e67, 0x16e68, 0x16e69, 0x16e6a, 0x16e6b, 0x16e6c, 0x16e6d, 0x16e6e, 0x16e6f, 0x16e70, 0x16e71, 0x16e72, 0x16e73, 0x16e74, 0x16e75, 0x16e76, 0x16e77, 0x16e78, 0x16e79,
    0x16e7a, 0x16e7b, 0x16e7c, 0x16e7d, 0x16e7e, 0x16e7f, 0x1e922, 0x1e923, 0x1e924, 0x1e925, 0x1e926, 0x1e927, 0x1e928, 0x1e929, 0x1e92a, 0x1e92b, 0x1e92c, 0x1e92d, 0x1e92e,
    0x1e92f, 0x1e930, 0x1e931, 0x1e932, 0x1e933, 0x1e934, 0x1e935, 0x1e936, 0x1e937, 0x1e938, 0x1e939, 0x1e93a, 0x1e93b, 0x1e93c, 0x1e93d, 0x1e93e, 0x1e93f, 0x1e940, 0x1e941,
    0x1e942, 0x1e943
};
// Upper case equivalents
static constexpr char32_t uc[1427] = { 0x00041, //  LATIN CAPITAL LETTER A
                                       0x00042, //  LATIN CAPITAL LETTER B
                                       0x00043, //  LATIN CAPITAL LETTER C
                                       0x00044, //  LATIN CAPITAL LETTER D
                                       0x00045, //  LATIN CAPITAL LETTER E
                                       0x00046, //  LATIN CAPITAL LETTER F
                                       0x00047, //  LATIN CAPITAL LETTER G
                                       0x00048, //  LATIN CAPITAL LETTER H
                                       0x00049, //  LATIN CAPITAL LETTER I
                                       0x0004a, //  LATIN CAPITAL LETTER J
                                       0x0004b, //  LATIN CAPITAL LETTER K
                                       0x0004c, //  LATIN CAPITAL LETTER L
                                       0x0004d, //  LATIN CAPITAL LETTER M
                                       0x0004e, //  LATIN CAPITAL LETTER N
                                       0x0004f, //  LATIN CAPITAL LETTER O
                                       0x00050, //  LATIN CAPITAL LETTER P
                                       0x00051, //  LATIN CAPITAL LETTER Q
                                       0x00052, //  LATIN CAPITAL LETTER R
                                       0x00053, //  LATIN CAPITAL LETTER S
                                       0x00054, //  LATIN CAPITAL LETTER T
                                       0x00055, //  LATIN CAPITAL LETTER U
                                       0x00056, //  LATIN CAPITAL LETTER V
                                       0x00057, //  LATIN CAPITAL LETTER W
                                       0x00058, //  LATIN CAPITAL LETTER X
                                       0x00059, //  LATIN CAPITAL LETTER Y
                                       0x0005a, //  LATIN CAPITAL LETTER Z
                                       0x01e9e, //  LATIN CAPITAL LETTER SHARP S
                                       0x000c0, //  LATIN CAPITAL LETTER A WITH GRAVE
                                       0x000c1, //  LATIN CAPITAL LETTER A WITH ACUTE
                                       0x000c2, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX
                                       0x000c3, //  LATIN CAPITAL LETTER A WITH TILDE
                                       0x000c4, //  LATIN CAPITAL LETTER A WITH DIAERESIS
                                       0x000c5, //  LATIN CAPITAL LETTER A WITH RING ABOVE
                                       0x000c6, //  LATIN CAPITAL LETTER AE
                                       0x000c7, //  LATIN CAPITAL LETTER C WITH CEDILLA
                                       0x000c8, //  LATIN CAPITAL LETTER E WITH GRAVE
                                       0x000c9, //  LATIN CAPITAL LETTER E WITH ACUTE
                                       0x000ca, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX
                                       0x000cb, //  LATIN CAPITAL LETTER E WITH DIAERESIS
                                       0x000cc, //  LATIN CAPITAL LETTER I WITH GRAVE
                                       0x000cd, //  LATIN CAPITAL LETTER I WITH ACUTE
                                       0x000ce, //  LATIN CAPITAL LETTER I WITH CIRCUMFLEX
                                       0x000cf, //  LATIN CAPITAL LETTER I WITH DIAERESIS
                                       0x000d0, //  LATIN CAPITAL LETTER ETH
                                       0x000d1, //  LATIN CAPITAL LETTER N WITH TILDE
                                       0x000d2, //  LATIN CAPITAL LETTER O WITH GRAVE
                                       0x000d3, //  LATIN CAPITAL LETTER O WITH ACUTE
                                       0x000d4, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX
                                       0x000d5, //  LATIN CAPITAL LETTER O WITH TILDE
                                       0x000d6, //  LATIN CAPITAL LETTER O WITH DIAERESIS
                                       0x000d8, //  LATIN CAPITAL LETTER O WITH STROKE
                                       0x000d9, //  LATIN CAPITAL LETTER U WITH GRAVE
                                       0x000da, //  LATIN CAPITAL LETTER U WITH ACUTE
                                       0x000db, //  LATIN CAPITAL LETTER U WITH CIRCUMFLEX
                                       0x000dc, //  LATIN CAPITAL LETTER U WITH DIAERESIS
                                       0x000dd, //  LATIN CAPITAL LETTER Y WITH ACUTE
                                       0x000de, //  LATIN CAPITAL LETTER THORN
                                       0x00178, //  LATIN CAPITAL LETTER Y WITH DIAERESIS
                                       0x00100, //  LATIN CAPITAL LETTER A WITH MACRON
                                       0x00102, //  LATIN CAPITAL LETTER A WITH BREVE
                                       0x00104, //  LATIN CAPITAL LETTER A WITH OGONEK
                                       0x00106, //  LATIN CAPITAL LETTER C WITH ACUTE
                                       0x00108, //  LATIN CAPITAL LETTER C WITH CIRCUMFLEX
                                       0x0010a, //  LATIN CAPITAL LETTER C WITH DOT ABOVE
                                       0x0010c, //  LATIN CAPITAL LETTER C WITH CARON
                                       0x0010e, //  LATIN CAPITAL LETTER D WITH CARON
                                       0x00110, //  LATIN CAPITAL LETTER D WITH STROKE
                                       0x00112, //  LATIN CAPITAL LETTER E WITH MACRON
                                       0x00114, //  LATIN CAPITAL LETTER E WITH BREVE
                                       0x00116, //  LATIN CAPITAL LETTER E WITH DOT ABOVE
                                       0x00118, //  LATIN CAPITAL LETTER E WITH OGONEK
                                       0x0011a, //  LATIN CAPITAL LETTER E WITH CARON
                                       0x0011c, //  LATIN CAPITAL LETTER G WITH CIRCUMFLEX
                                       0x0011e, //  LATIN CAPITAL LETTER G WITH BREVE
                                       0x00120, //  LATIN CAPITAL LETTER G WITH DOT ABOVE
                                       0x00122, //  LATIN CAPITAL LETTER G WITH CEDILLA
                                       0x00124, //  LATIN CAPITAL LETTER H WITH CIRCUMFLEX
                                       0x00126, //  LATIN CAPITAL LETTER H WITH STROKE
                                       0x00128, //  LATIN CAPITAL LETTER I WITH TILDE
                                       0x0012a, //  LATIN CAPITAL LETTER I WITH MACRON
                                       0x0012c, //  LATIN CAPITAL LETTER I WITH BREVE
                                       0x0012e, //  LATIN CAPITAL LETTER I WITH OGONEK
                                       0x00132, //  LATIN CAPITAL LIGATURE IJ
                                       0x00134, //  LATIN CAPITAL LETTER J WITH CIRCUMFLEX
                                       0x00136, //  LATIN CAPITAL LETTER K WITH CEDILLA
                                       0x00139, //  LATIN CAPITAL LETTER L WITH ACUTE
                                       0x0013b, //  LATIN CAPITAL LETTER L WITH CEDILLA
                                       0x0013d, //  LATIN CAPITAL LETTER L WITH CARON
                                       0x0013f, //  LATIN CAPITAL LETTER L WITH MIDDLE DOT
                                       0x00141, //  LATIN CAPITAL LETTER L WITH STROKE
                                       0x00143, //  LATIN CAPITAL LETTER N WITH ACUTE
                                       0x00145, //  LATIN CAPITAL LETTER N WITH CEDILLA
                                       0x00147, //  LATIN CAPITAL LETTER N WITH CARON
                                       0x0014a, //  LATIN CAPITAL LETTER ENG
                                       0x0014c, //  LATIN CAPITAL LETTER O WITH MACRON
                                       0x0014e, //  LATIN CAPITAL LETTER O WITH BREVE
                                       0x00150, //  LATIN CAPITAL LETTER O WITH DOUBLE ACUTE
                                       0x00152, //  LATIN CAPITAL LIGATURE OE
                                       0x00154, //  LATIN CAPITAL LETTER R WITH ACUTE
                                       0x00156, //  LATIN CAPITAL LETTER R WITH CEDILLA
                                       0x00158, //  LATIN CAPITAL LETTER R WITH CARON
                                       0x0015a, //  LATIN CAPITAL LETTER S WITH ACUTE
                                       0x0015c, //  LATIN CAPITAL LETTER S WITH CIRCUMFLEX
                                       0x0015e, //  LATIN CAPITAL LETTER S WITH CEDILLA
                                       0x00160, //  LATIN CAPITAL LETTER S WITH CARON
                                       0x00162, //  LATIN CAPITAL LETTER T WITH CEDILLA
                                       0x00164, //  LATIN CAPITAL LETTER T WITH CARON
                                       0x00166, //  LATIN CAPITAL LETTER T WITH STROKE
                                       0x00168, //  LATIN CAPITAL LETTER U WITH TILDE
                                       0x0016a, //  LATIN CAPITAL LETTER U WITH MACRON
                                       0x0016c, //  LATIN CAPITAL LETTER U WITH BREVE
                                       0x0016e, //  LATIN CAPITAL LETTER U WITH RING ABOVE
                                       0x00170, //  LATIN CAPITAL LETTER U WITH DOUBLE ACUTE
                                       0x00172, //  LATIN CAPITAL LETTER U WITH OGONEK
                                       0x00174, //  LATIN CAPITAL LETTER W WITH CIRCUMFLEX
                                       0x00176, //  LATIN CAPITAL LETTER Y WITH CIRCUMFLEX
                                       0x00179, //  LATIN CAPITAL LETTER Z WITH ACUTE
                                       0x0017b, //  LATIN CAPITAL LETTER Z WITH DOT ABOVE
                                       0x0017d, //  LATIN CAPITAL LETTER Z WITH CARON
                                       0x00243, //  LATIN CAPITAL LETTER B WITH STROKE
                                       0x00182, //  LATIN CAPITAL LETTER B WITH TOPBAR
                                       0x00184, //  LATIN CAPITAL LETTER TONE SIX
                                       0x00187, //  LATIN CAPITAL LETTER C WITH HOOK
                                       0x0018b, //  LATIN CAPITAL LETTER D WITH TOPBAR
                                       0x00191, //  LATIN CAPITAL LETTER F WITH HOOK
                                       0x001f6, //  LATIN CAPITAL LETTER HWAIR
                                       0x00198, //  LATIN CAPITAL LETTER K WITH HOOK
                                       0x0023d, //  LATIN CAPITAL LETTER L WITH BAR
                                       0x00220, //  LATIN CAPITAL LETTER N WITH LONG RIGHT LEG
                                       0x001a0, //  LATIN CAPITAL LETTER O WITH HORN
                                       0x001a2, //  LATIN CAPITAL LETTER OI
                                       0x001a4, //  LATIN CAPITAL LETTER P WITH HOOK
                                       0x001a7, //  LATIN CAPITAL LETTER TONE TWO
                                       0x001ac, //  LATIN CAPITAL LETTER T WITH HOOK
                                       0x001af, //  LATIN CAPITAL LETTER U WITH HORN
                                       0x001b3, //  LATIN CAPITAL LETTER Y WITH HOOK
                                       0x001b5, //  LATIN CAPITAL LETTER Z WITH STROKE
                                       0x001b8, //  LATIN CAPITAL LETTER EZH REVERSED
                                       0x001bc, //  LATIN CAPITAL LETTER TONE FIVE
                                       0x001f7, //  LATIN CAPITAL LETTER WYNN
                                       0x001c5, //  LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON
                                       0x001c7, //  LATIN CAPITAL LETTER LJ
                                       0x001ca, //  LATIN CAPITAL LETTER NJ
                                       0x001cd, //  LATIN CAPITAL LETTER A WITH CARON
                                       0x001cf, //  LATIN CAPITAL LETTER I WITH CARON
                                       0x001d1, //  LATIN CAPITAL LETTER O WITH CARON
                                       0x001d3, //  LATIN CAPITAL LETTER U WITH CARON
                                       0x001d5, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
                                       0x001d7, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
                                       0x001d9, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
                                       0x001db, //  LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
                                       0x0018e, //  LATIN CAPITAL LETTER REVERSED E
                                       0x001de, //  LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON
                                       0x001e0, //  LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON
                                       0x001e2, //  LATIN CAPITAL LETTER AE WITH MACRON
                                       0x001e4, //  LATIN CAPITAL LETTER G WITH STROKE
                                       0x001e6, //  LATIN CAPITAL LETTER G WITH CARON
                                       0x001e8, //  LATIN CAPITAL LETTER K WITH CARON
                                       0x001ea, //  LATIN CAPITAL LETTER O WITH OGONEK
                                       0x001ec, //  LATIN CAPITAL LETTER O WITH OGONEK AND MACRON
                                       0x001ee, //  LATIN CAPITAL LETTER EZH WITH CARON
                                       0x001f2, //  LATIN CAPITAL LETTER D WITH SMALL LETTER Z
                                       0x001f4, //  LATIN CAPITAL LETTER G WITH ACUTE
                                       0x001f8, //  LATIN CAPITAL LETTER N WITH GRAVE
                                       0x001fa, //  LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE
                                       0x001fc, //  LATIN CAPITAL LETTER AE WITH ACUTE
                                       0x001fe, //  LATIN CAPITAL LETTER O WITH STROKE AND ACUTE
                                       0x00200, //  LATIN CAPITAL LETTER A WITH DOUBLE GRAVE
                                       0x00202, //  LATIN CAPITAL LETTER A WITH INVERTED BREVE
                                       0x00204, //  LATIN CAPITAL LETTER E WITH DOUBLE GRAVE
                                       0x00206, //  LATIN CAPITAL LETTER E WITH INVERTED BREVE
                                       0x00208, //  LATIN CAPITAL LETTER I WITH DOUBLE GRAVE
                                       0x0020a, //  LATIN CAPITAL LETTER I WITH INVERTED BREVE
                                       0x0020c, //  LATIN CAPITAL LETTER O WITH DOUBLE GRAVE
                                       0x0020e, //  LATIN CAPITAL LETTER O WITH INVERTED BREVE
                                       0x00210, //  LATIN CAPITAL LETTER R WITH DOUBLE GRAVE
                                       0x00212, //  LATIN CAPITAL LETTER R WITH INVERTED BREVE
                                       0x00214, //  LATIN CAPITAL LETTER U WITH DOUBLE GRAVE
                                       0x00216, //  LATIN CAPITAL LETTER U WITH INVERTED BREVE
                                       0x00218, //  LATIN CAPITAL LETTER S WITH COMMA BELOW
                                       0x0021a, //  LATIN CAPITAL LETTER T WITH COMMA BELOW
                                       0x0021c, //  LATIN CAPITAL LETTER YOGH
                                       0x0021e, //  LATIN CAPITAL LETTER H WITH CARON
                                       0x00222, //  LATIN CAPITAL LETTER OU
                                       0x00224, //  LATIN CAPITAL LETTER Z WITH HOOK
                                       0x00226, //  LATIN CAPITAL LETTER A WITH DOT ABOVE
                                       0x00228, //  LATIN CAPITAL LETTER E WITH CEDILLA
                                       0x0022a, //  LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON
                                       0x0022c, //  LATIN CAPITAL LETTER O WITH TILDE AND MACRON
                                       0x0022e, //  LATIN CAPITAL LETTER O WITH DOT ABOVE
                                       0x00230, //  LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON
                                       0x00232, //  LATIN CAPITAL LETTER Y WITH MACRON
                                       0x0023b, //  LATIN CAPITAL LETTER C WITH STROKE
                                       0x02c7e, //  LATIN CAPITAL LETTER S WITH SWASH TAIL
                                       0x02c7f, //  LATIN CAPITAL LETTER Z WITH SWASH TAIL
                                       0x00241, //  LATIN CAPITAL LETTER GLOTTAL STOP
                                       0x00246, //  LATIN CAPITAL LETTER E WITH STROKE
                                       0x00248, //  LATIN CAPITAL LETTER J WITH STROKE
                                       0x0024a, //  LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL
                                       0x0024c, //  LATIN CAPITAL LETTER R WITH STROKE
                                       0x0024e, //  LATIN CAPITAL LETTER Y WITH STROKE
                                       0x02c6f, //  LATIN CAPITAL LETTER TURNED A
                                       0x02c6d, //  LATIN CAPITAL LETTER ALPHA
                                       0x02c70, //  LATIN CAPITAL LETTER TURNED ALPHA
                                       0x00181, //  LATIN CAPITAL LETTER B WITH HOOK
                                       0x00186, //  LATIN CAPITAL LETTER OPEN O
                                       0x00189, //  LATIN CAPITAL LETTER AFRICAN D
                                       0x0018a, //  LATIN CAPITAL LETTER D WITH HOOK
                                       0x0018f, //  LATIN CAPITAL LETTER SCHWA
                                       0x00190, //  LATIN CAPITAL LETTER OPEN E
                                       0x0a7ab, //  LATIN CAPITAL LETTER REVERSED OPEN E
                                       0x00193, //  LATIN CAPITAL LETTER G WITH HOOK
                                       0x0a7ac, //  LATIN CAPITAL LETTER SCRIPT G
                                       0x00194, //  LATIN CAPITAL LETTER GAMMA
                                       0x0a78d, //  LATIN CAPITAL LETTER TURNED H
                                       0x0a7aa, //  LATIN CAPITAL LETTER H WITH HOOK
                                       0x00197, //  LATIN CAPITAL LETTER I WITH STROKE
                                       0x00196, //  LATIN CAPITAL LETTER IOTA
                                       0x0a7ae, //  LATIN CAPITAL LETTER SMALL CAPITAL I
                                       0x02c62, //  LATIN CAPITAL LETTER L WITH MIDDLE TILDE
                                       0x0a7ad, //  LATIN CAPITAL LETTER L WITH BELT
                                       0x0019c, //  LATIN CAPITAL LETTER TURNED M
                                       0x02c6e, //  LATIN CAPITAL LETTER M WITH HOOK
                                       0x0019d, //  LATIN CAPITAL LETTER N WITH LEFT HOOK
                                       0x0019f, //  LATIN CAPITAL LETTER O WITH MIDDLE TILDE
                                       0x02c64, //  LATIN CAPITAL LETTER R WITH TAIL
                                       0x001a6, //  LATIN LETTER YR
                                       0x0a7c5, //  LATIN CAPITAL LETTER S WITH HOOK
                                       0x001a9, //  LATIN CAPITAL LETTER ESH
                                       0x0a7b1, //  LATIN CAPITAL LETTER TURNED T
                                       0x001ae, //  LATIN CAPITAL LETTER T WITH RETROFLEX HOOK
                                       0x00244, //  LATIN CAPITAL LETTER U BAR
                                       0x001b1, //  LATIN CAPITAL LETTER UPSILON
                                       0x001b2, //  LATIN CAPITAL LETTER V WITH HOOK
                                       0x00245, //  LATIN CAPITAL LETTER TURNED V
                                       0x001b7, //  LATIN CAPITAL LETTER EZH
                                       0x0a7b2, //  LATIN CAPITAL LETTER J WITH CROSSED-TAIL
                                       0x0a7b0, //  LATIN CAPITAL LETTER TURNED K
                                       0x00370, //  GREEK CAPITAL LETTER HETA
                                       0x00372, //  GREEK CAPITAL LETTER ARCHAIC SAMPI
                                       0x00376, //  GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA
                                       0x003fd, //  GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL
                                       0x003fe, //  GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL
                                       0x003ff, //  GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL
                                       0x00386, //  GREEK CAPITAL LETTER ALPHA WITH TONOS
                                       0x00388, //  GREEK CAPITAL LETTER EPSILON WITH TONOS
                                       0x00389, //  GREEK CAPITAL LETTER ETA WITH TONOS
                                       0x0038a, //  GREEK CAPITAL LETTER IOTA WITH TONOS
                                       0x00391, //  GREEK CAPITAL LETTER ALPHA
                                       0x00392, //  GREEK CAPITAL LETTER BETA
                                       0x00393, //  GREEK CAPITAL LETTER GAMMA
                                       0x00394, //  GREEK CAPITAL LETTER DELTA
                                       0x003f5, //  GREEK LUNATE EPSILON SYMBOL
                                       0x00396, //  GREEK CAPITAL LETTER ZETA
                                       0x00397, //  GREEK CAPITAL LETTER ETA
                                       0x003d1, //  GREEK THETA SYMBOL
                                       0x003f4, //  GREEK CAPITAL THETA SYMBOL
                                       0x00345, //  COMBINING GREEK YPOGEGRAMMENI
                                       0x00399, //  GREEK CAPITAL LETTER IOTA
                                       0x0039a, //  GREEK CAPITAL LETTER KAPPA
                                       0x0039b, //  GREEK CAPITAL LETTER LAMDA
                                       0x000b5, //  MICRO SIGN
                                       0x0039d, //  GREEK CAPITAL LETTER NU
                                       0x0039e, //  GREEK CAPITAL LETTER XI
                                       0x0039f, //  GREEK CAPITAL LETTER OMICRON
                                       0x003d6, //  GREEK PI SYMBOL
                                       0x003a1, //  GREEK CAPITAL LETTER RHO
                                       0x003c2, //  GREEK SMALL LETTER FINAL SIGMA
                                       0x003a4, //  GREEK CAPITAL LETTER TAU
                                       0x003a5, //  GREEK CAPITAL LETTER UPSILON
                                       0x003a6, //  GREEK CAPITAL LETTER PHI
                                       0x003a7, //  GREEK CAPITAL LETTER CHI
                                       0x003a8, //  GREEK CAPITAL LETTER PSI
                                       0x003a9, //  GREEK CAPITAL LETTER OMEGA
                                       0x003aa, //  GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
                                       0x003ab, //  GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
                                       0x0038c, //  GREEK CAPITAL LETTER OMICRON WITH TONOS
                                       0x0038e, //  GREEK CAPITAL LETTER UPSILON WITH TONOS
                                       0x0038f, //  GREEK CAPITAL LETTER OMEGA WITH TONOS
                                       0x003cf, //  GREEK CAPITAL KAI SYMBOL
                                       0x003d8, //  GREEK LETTER ARCHAIC KOPPA
                                       0x003da, //  GREEK LETTER STIGMA
                                       0x003dc, //  GREEK LETTER DIGAMMA
                                       0x003de, //  GREEK LETTER KOPPA
                                       0x003e0, //  GREEK LETTER SAMPI
                                       0x003e2, //  COPTIC CAPITAL LETTER SHEI
                                       0x003e4, //  COPTIC CAPITAL LETTER FEI
                                       0x003e6, //  COPTIC CAPITAL LETTER KHEI
                                       0x003e8, //  COPTIC CAPITAL LETTER HORI
                                       0x003ea, //  COPTIC CAPITAL LETTER GANGIA
                                       0x003ec, //  COPTIC CAPITAL LETTER SHIMA
                                       0x003ee, //  COPTIC CAPITAL LETTER DEI
                                       0x003f9, //  GREEK CAPITAL LUNATE SIGMA SYMBOL
                                       0x0037f, //  GREEK CAPITAL LETTER YOT
                                       0x003f7, //  GREEK CAPITAL LETTER SHO
                                       0x003fa, //  GREEK CAPITAL LETTER SAN
                                       0x00410, //  CYRILLIC CAPITAL LETTER A
                                       0x00411, //  CYRILLIC CAPITAL LETTER BE
                                       0x00412, //  CYRILLIC CAPITAL LETTER VE
                                       0x00413, //  CYRILLIC CAPITAL LETTER GHE
                                       0x01c81, //  CYRILLIC SMALL LETTER LONG-LEGGED DE
                                       0x00415, //  CYRILLIC CAPITAL LETTER IE
                                       0x00416, //  CYRILLIC CAPITAL LETTER ZHE
                                       0x00417, //  CYRILLIC CAPITAL LETTER ZE
                                       0x00418, //  CYRILLIC CAPITAL LETTER I
                                       0x00419, //  CYRILLIC CAPITAL LETTER SHORT I
                                       0x0041a, //  CYRILLIC CAPITAL LETTER KA
                                       0x0041b, //  CYRILLIC CAPITAL LETTER EL
                                       0x0041c, //  CYRILLIC CAPITAL LETTER EM
                                       0x0041d, //  CYRILLIC CAPITAL LETTER EN
                                       0x01c82, //  CYRILLIC SMALL LETTER NARROW O
                                       0x0041f, //  CYRILLIC CAPITAL LETTER PE
                                       0x00420, //  CYRILLIC CAPITAL LETTER ER
                                       0x00421, //  CYRILLIC CAPITAL LETTER ES
                                       0x00422, //  CYRILLIC CAPITAL LETTER TE
                                       0x01c84, //  CYRILLIC SMALL LETTER TALL TE
                                       0x00423, //  CYRILLIC CAPITAL LETTER U
                                       0x00424, //  CYRILLIC CAPITAL LETTER EF
                                       0x00425, //  CYRILLIC CAPITAL LETTER HA
                                       0x00426, //  CYRILLIC CAPITAL LETTER TSE
                                       0x00427, //  CYRILLIC CAPITAL LETTER CHE
                                       0x00428, //  CYRILLIC CAPITAL LETTER SHA
                                       0x00429, //  CYRILLIC CAPITAL LETTER SHCHA
                                       0x0042a, //  CYRILLIC CAPITAL LETTER HARD SIGN
                                       0x0042b, //  CYRILLIC CAPITAL LETTER YERU
                                       0x0042c, //  CYRILLIC CAPITAL LETTER SOFT SIGN
                                       0x0042d, //  CYRILLIC CAPITAL LETTER E
                                       0x0042e, //  CYRILLIC CAPITAL LETTER YU
                                       0x0042f, //  CYRILLIC CAPITAL LETTER YA
                                       0x00400, //  CYRILLIC CAPITAL LETTER IE WITH GRAVE
                                       0x00401, //  CYRILLIC CAPITAL LETTER IO
                                       0x00402, //  CYRILLIC CAPITAL LETTER DJE
                                       0x00403, //  CYRILLIC CAPITAL LETTER GJE
                                       0x00404, //  CYRILLIC CAPITAL LETTER UKRAINIAN IE
                                       0x00405, //  CYRILLIC CAPITAL LETTER DZE
                                       0x00406, //  CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I
                                       0x00407, //  CYRILLIC CAPITAL LETTER YI
                                       0x00408, //  CYRILLIC CAPITAL LETTER JE
                                       0x00409, //  CYRILLIC CAPITAL LETTER LJE
                                       0x0040a, //  CYRILLIC CAPITAL LETTER NJE
                                       0x0040b, //  CYRILLIC CAPITAL LETTER TSHE
                                       0x0040c, //  CYRILLIC CAPITAL LETTER KJE
                                       0x0040d, //  CYRILLIC CAPITAL LETTER I WITH GRAVE
                                       0x0040e, //  CYRILLIC CAPITAL LETTER SHORT U
                                       0x0040f, //  CYRILLIC CAPITAL LETTER DZHE
                                       0x00460, //  CYRILLIC CAPITAL LETTER OMEGA
                                       0x00462, //  CYRILLIC CAPITAL LETTER YAT
                                       0x00464, //  CYRILLIC CAPITAL LETTER IOTIFIED E
                                       0x00466, //  CYRILLIC CAPITAL LETTER LITTLE YUS
                                       0x00468, //  CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS
                                       0x0046a, //  CYRILLIC CAPITAL LETTER BIG YUS
                                       0x0046c, //  CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS
                                       0x0046e, //  CYRILLIC CAPITAL LETTER KSI
                                       0x00470, //  CYRILLIC CAPITAL LETTER PSI
                                       0x00472, //  CYRILLIC CAPITAL LETTER FITA
                                       0x00474, //  CYRILLIC CAPITAL LETTER IZHITSA
                                       0x00476, //  CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT
                                       0x00478, //  CYRILLIC CAPITAL LETTER UK
                                       0x0047a, //  CYRILLIC CAPITAL LETTER ROUND OMEGA
                                       0x0047c, //  CYRILLIC CAPITAL LETTER OMEGA WITH TITLO
                                       0x0047e, //  CYRILLIC CAPITAL LETTER OT
                                       0x00480, //  CYRILLIC CAPITAL LETTER KOPPA
                                       0x0048a, //  CYRILLIC CAPITAL LETTER SHORT I WITH TAIL
                                       0x0048c, //  CYRILLIC CAPITAL LETTER SEMISOFT SIGN
                                       0x0048e, //  CYRILLIC CAPITAL LETTER ER WITH TICK
                                       0x00490, //  CYRILLIC CAPITAL LETTER GHE WITH UPTURN
                                       0x00492, //  CYRILLIC CAPITAL LETTER GHE WITH STROKE
                                       0x00494, //  CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK
                                       0x00496, //  CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER
                                       0x00498, //  CYRILLIC CAPITAL LETTER ZE WITH DESCENDER
                                       0x0049a, //  CYRILLIC CAPITAL LETTER KA WITH DESCENDER
                                       0x0049c, //  CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE
                                       0x0049e, //  CYRILLIC CAPITAL LETTER KA WITH STROKE
                                       0x004a0, //  CYRILLIC CAPITAL LETTER BASHKIR KA
                                       0x004a2, //  CYRILLIC CAPITAL LETTER EN WITH DESCENDER
                                       0x004a4, //  CYRILLIC CAPITAL LIGATURE EN GHE
                                       0x004a6, //  CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK
                                       0x004a8, //  CYRILLIC CAPITAL LETTER ABKHASIAN HA
                                       0x004aa, //  CYRILLIC CAPITAL LETTER ES WITH DESCENDER
                                       0x004ac, //  CYRILLIC CAPITAL LETTER TE WITH DESCENDER
                                       0x004ae, //  CYRILLIC CAPITAL LETTER STRAIGHT U
                                       0x004b0, //  CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE
                                       0x004b2, //  CYRILLIC CAPITAL LETTER HA WITH DESCENDER
                                       0x004b4, //  CYRILLIC CAPITAL LIGATURE TE TSE
                                       0x004b6, //  CYRILLIC CAPITAL LETTER CHE WITH DESCENDER
                                       0x004b8, //  CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE
                                       0x004ba, //  CYRILLIC CAPITAL LETTER SHHA
                                       0x004bc, //  CYRILLIC CAPITAL LETTER ABKHASIAN CHE
                                       0x004be, //  CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER
                                       0x004c1, //  CYRILLIC CAPITAL LETTER ZHE WITH BREVE
                                       0x004c3, //  CYRILLIC CAPITAL LETTER KA WITH HOOK
                                       0x004c5, //  CYRILLIC CAPITAL LETTER EL WITH TAIL
                                       0x004c7, //  CYRILLIC CAPITAL LETTER EN WITH HOOK
                                       0x004c9, //  CYRILLIC CAPITAL LETTER EN WITH TAIL
                                       0x004cb, //  CYRILLIC CAPITAL LETTER KHAKASSIAN CHE
                                       0x004cd, //  CYRILLIC CAPITAL LETTER EM WITH TAIL
                                       0x004c0, //  CYRILLIC LETTER PALOCHKA
                                       0x004d0, //  CYRILLIC CAPITAL LETTER A WITH BREVE
                                       0x004d2, //  CYRILLIC CAPITAL LETTER A WITH DIAERESIS
                                       0x004d4, //  CYRILLIC CAPITAL LIGATURE A IE
                                       0x004d6, //  CYRILLIC CAPITAL LETTER IE WITH BREVE
                                       0x004d8, //  CYRILLIC CAPITAL LETTER SCHWA
                                       0x004da, //  CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS
                                       0x004dc, //  CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS
                                       0x004de, //  CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS
                                       0x004e0, //  CYRILLIC CAPITAL LETTER ABKHASIAN DZE
                                       0x004e2, //  CYRILLIC CAPITAL LETTER I WITH MACRON
                                       0x004e4, //  CYRILLIC CAPITAL LETTER I WITH DIAERESIS
                                       0x004e6, //  CYRILLIC CAPITAL LETTER O WITH DIAERESIS
                                       0x004e8, //  CYRILLIC CAPITAL LETTER BARRED O
                                       0x004ea, //  CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS
                                       0x004ec, //  CYRILLIC CAPITAL LETTER E WITH DIAERESIS
                                       0x004ee, //  CYRILLIC CAPITAL LETTER U WITH MACRON
                                       0x004f0, //  CYRILLIC CAPITAL LETTER U WITH DIAERESIS
                                       0x004f2, //  CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE
                                       0x004f4, //  CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS
                                       0x004f6, //  CYRILLIC CAPITAL LETTER GHE WITH DESCENDER
                                       0x004f8, //  CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS
                                       0x004fa, //  CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK
                                       0x004fc, //  CYRILLIC CAPITAL LETTER HA WITH HOOK
                                       0x004fe, //  CYRILLIC CAPITAL LETTER HA WITH STROKE
                                       0x00500, //  CYRILLIC CAPITAL LETTER KOMI DE
                                       0x00502, //  CYRILLIC CAPITAL LETTER KOMI DJE
                                       0x00504, //  CYRILLIC CAPITAL LETTER KOMI ZJE
                                       0x00506, //  CYRILLIC CAPITAL LETTER KOMI DZJE
                                       0x00508, //  CYRILLIC CAPITAL LETTER KOMI LJE
                                       0x0050a, //  CYRILLIC CAPITAL LETTER KOMI NJE
                                       0x0050c, //  CYRILLIC CAPITAL LETTER KOMI SJE
                                       0x0050e, //  CYRILLIC CAPITAL LETTER KOMI TJE
                                       0x00510, //  CYRILLIC CAPITAL LETTER REVERSED ZE
                                       0x00512, //  CYRILLIC CAPITAL LETTER EL WITH HOOK
                                       0x00514, //  CYRILLIC CAPITAL LETTER LHA
                                       0x00516, //  CYRILLIC CAPITAL LETTER RHA
                                       0x00518, //  CYRILLIC CAPITAL LETTER YAE
                                       0x0051a, //  CYRILLIC CAPITAL LETTER QA
                                       0x0051c, //  CYRILLIC CAPITAL LETTER WE
                                       0x0051e, //  CYRILLIC CAPITAL LETTER ALEUT KA
                                       0x00520, //  CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK
                                       0x00522, //  CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK
                                       0x00524, //  CYRILLIC CAPITAL LETTER PE WITH DESCENDER
                                       0x00526, //  CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER
                                       0x00528, //  CYRILLIC CAPITAL LETTER EN WITH LEFT HOOK
                                       0x0052a, //  CYRILLIC CAPITAL LETTER DZZHE
                                       0x0052c, //  CYRILLIC CAPITAL LETTER DCHE
                                       0x0052e, //  CYRILLIC CAPITAL LETTER EL WITH DESCENDER
                                       0x00531, //  ARMENIAN CAPITAL LETTER AYB
                                       0x00532, //  ARMENIAN CAPITAL LETTER BEN
                                       0x00533, //  ARMENIAN CAPITAL LETTER GIM
                                       0x00534, //  ARMENIAN CAPITAL LETTER DA
                                       0x00535, //  ARMENIAN CAPITAL LETTER ECH
                                       0x00536, //  ARMENIAN CAPITAL LETTER ZA
                                       0x00537, //  ARMENIAN CAPITAL LETTER EH
                                       0x00538, //  ARMENIAN CAPITAL LETTER ET
                                       0x00539, //  ARMENIAN CAPITAL LETTER TO
                                       0x0053a, //  ARMENIAN CAPITAL LETTER ZHE
                                       0x0053b, //  ARMENIAN CAPITAL LETTER INI
                                       0x0053c, //  ARMENIAN CAPITAL LETTER LIWN
                                       0x0053d, //  ARMENIAN CAPITAL LETTER XEH
                                       0x0053e, //  ARMENIAN CAPITAL LETTER CA
                                       0x0053f, //  ARMENIAN CAPITAL LETTER KEN
                                       0x00540, //  ARMENIAN CAPITAL LETTER HO
                                       0x00541, //  ARMENIAN CAPITAL LETTER JA
                                       0x00542, //  ARMENIAN CAPITAL LETTER GHAD
                                       0x00543, //  ARMENIAN CAPITAL LETTER CHEH
                                       0x00544, //  ARMENIAN CAPITAL LETTER MEN
                                       0x00545, //  ARMENIAN CAPITAL LETTER YI
                                       0x00546, //  ARMENIAN CAPITAL LETTER NOW
                                       0x00547, //  ARMENIAN CAPITAL LETTER SHA
                                       0x00548, //  ARMENIAN CAPITAL LETTER VO
                                       0x00549, //  ARMENIAN CAPITAL LETTER CHA
                                       0x0054a, //  ARMENIAN CAPITAL LETTER PEH
                                       0x0054b, //  ARMENIAN CAPITAL LETTER JHEH
                                       0x0054c, //  ARMENIAN CAPITAL LETTER RA
                                       0x0054d, //  ARMENIAN CAPITAL LETTER SEH
                                       0x0054e, //  ARMENIAN CAPITAL LETTER VEW
                                       0x0054f, //  ARMENIAN CAPITAL LETTER TIWN
                                       0x00550, //  ARMENIAN CAPITAL LETTER REH
                                       0x00551, //  ARMENIAN CAPITAL LETTER CO
                                       0x00552, //  ARMENIAN CAPITAL LETTER YIWN
                                       0x00553, //  ARMENIAN CAPITAL LETTER PIWR
                                       0x00554, //  ARMENIAN CAPITAL LETTER KEH
                                       0x00555, //  ARMENIAN CAPITAL LETTER OH
                                       0x00556, //  ARMENIAN CAPITAL LETTER FEH
                                       0x01c90, //  GEORGIAN MTAVRULI CAPITAL LETTER AN
                                       0x01c91, //  GEORGIAN MTAVRULI CAPITAL LETTER BAN
                                       0x01c92, //  GEORGIAN MTAVRULI CAPITAL LETTER GAN
                                       0x01c93, //  GEORGIAN MTAVRULI CAPITAL LETTER DON
                                       0x01c94, //  GEORGIAN MTAVRULI CAPITAL LETTER EN
                                       0x01c95, //  GEORGIAN MTAVRULI CAPITAL LETTER VIN
                                       0x01c96, //  GEORGIAN MTAVRULI CAPITAL LETTER ZEN
                                       0x01c97, //  GEORGIAN MTAVRULI CAPITAL LETTER TAN
                                       0x01c98, //  GEORGIAN MTAVRULI CAPITAL LETTER IN
                                       0x01c99, //  GEORGIAN MTAVRULI CAPITAL LETTER KAN
                                       0x01c9a, //  GEORGIAN MTAVRULI CAPITAL LETTER LAS
                                       0x01c9b, //  GEORGIAN MTAVRULI CAPITAL LETTER MAN
                                       0x01c9c, //  GEORGIAN MTAVRULI CAPITAL LETTER NAR
                                       0x01c9d, //  GEORGIAN MTAVRULI CAPITAL LETTER ON
                                       0x01c9e, //  GEORGIAN MTAVRULI CAPITAL LETTER PAR
                                       0x01c9f, //  GEORGIAN MTAVRULI CAPITAL LETTER ZHAR
                                       0x01ca0, //  GEORGIAN MTAVRULI CAPITAL LETTER RAE
                                       0x01ca1, //  GEORGIAN MTAVRULI CAPITAL LETTER SAN
                                       0x01ca2, //  GEORGIAN MTAVRULI CAPITAL LETTER TAR
                                       0x01ca3, //  GEORGIAN MTAVRULI CAPITAL LETTER UN
                                       0x01ca4, //  GEORGIAN MTAVRULI CAPITAL LETTER PHAR
                                       0x01ca5, //  GEORGIAN MTAVRULI CAPITAL LETTER KHAR
                                       0x01ca6, //  GEORGIAN MTAVRULI CAPITAL LETTER GHAN
                                       0x01ca7, //  GEORGIAN MTAVRULI CAPITAL LETTER QAR
                                       0x01ca8, //  GEORGIAN MTAVRULI CAPITAL LETTER SHIN
                                       0x01ca9, //  GEORGIAN MTAVRULI CAPITAL LETTER CHIN
                                       0x01caa, //  GEORGIAN MTAVRULI CAPITAL LETTER CAN
                                       0x01cab, //  GEORGIAN MTAVRULI CAPITAL LETTER JIL
                                       0x01cac, //  GEORGIAN MTAVRULI CAPITAL LETTER CIL
                                       0x01cad, //  GEORGIAN MTAVRULI CAPITAL LETTER CHAR
                                       0x01cae, //  GEORGIAN MTAVRULI CAPITAL LETTER XAN
                                       0x01caf, //  GEORGIAN MTAVRULI CAPITAL LETTER JHAN
                                       0x01cb0, //  GEORGIAN MTAVRULI CAPITAL LETTER HAE
                                       0x01cb1, //  GEORGIAN MTAVRULI CAPITAL LETTER HE
                                       0x01cb2, //  GEORGIAN MTAVRULI CAPITAL LETTER HIE
                                       0x01cb3, //  GEORGIAN MTAVRULI CAPITAL LETTER WE
                                       0x01cb4, //  GEORGIAN MTAVRULI CAPITAL LETTER HAR
                                       0x01cb5, //  GEORGIAN MTAVRULI CAPITAL LETTER HOE
                                       0x01cb6, //  GEORGIAN MTAVRULI CAPITAL LETTER FI
                                       0x01cb7, //  GEORGIAN MTAVRULI CAPITAL LETTER YN
                                       0x01cb8, //  GEORGIAN MTAVRULI CAPITAL LETTER ELIFI
                                       0x01cb9, //  GEORGIAN MTAVRULI CAPITAL LETTER TURNED GAN
                                       0x01cba, //  GEORGIAN MTAVRULI CAPITAL LETTER AIN
                                       0x01cbd, //  GEORGIAN MTAVRULI CAPITAL LETTER AEN
                                       0x01cbe, //  GEORGIAN MTAVRULI CAPITAL LETTER HARD SIGN
                                       0x01cbf, //  GEORGIAN MTAVRULI CAPITAL LETTER LABIAL SIGN
                                       0x0ab70, //  CHEROKEE SMALL LETTER A
                                       0x0ab71, //  CHEROKEE SMALL LETTER E
                                       0x0ab72, //  CHEROKEE SMALL LETTER I
                                       0x0ab73, //  CHEROKEE SMALL LETTER O
                                       0x0ab74, //  CHEROKEE SMALL LETTER U
                                       0x0ab75, //  CHEROKEE SMALL LETTER V
                                       0x0ab76, //  CHEROKEE SMALL LETTER GA
                                       0x0ab77, //  CHEROKEE SMALL LETTER KA
                                       0x0ab78, //  CHEROKEE SMALL LETTER GE
                                       0x0ab79, //  CHEROKEE SMALL LETTER GI
                                       0x0ab7a, //  CHEROKEE SMALL LETTER GO
                                       0x0ab7b, //  CHEROKEE SMALL LETTER GU
                                       0x0ab7c, //  CHEROKEE SMALL LETTER GV
                                       0x0ab7d, //  CHEROKEE SMALL LETTER HA
                                       0x0ab7e, //  CHEROKEE SMALL LETTER HE
                                       0x0ab7f, //  CHEROKEE SMALL LETTER HI
                                       0x0ab80, //  CHEROKEE SMALL LETTER HO
                                       0x0ab81, //  CHEROKEE SMALL LETTER HU
                                       0x0ab82, //  CHEROKEE SMALL LETTER HV
                                       0x0ab83, //  CHEROKEE SMALL LETTER LA
                                       0x0ab84, //  CHEROKEE SMALL LETTER LE
                                       0x0ab85, //  CHEROKEE SMALL LETTER LI
                                       0x0ab86, //  CHEROKEE SMALL LETTER LO
                                       0x0ab87, //  CHEROKEE SMALL LETTER LU
                                       0x0ab88, //  CHEROKEE SMALL LETTER LV
                                       0x0ab89, //  CHEROKEE SMALL LETTER MA
                                       0x0ab8a, //  CHEROKEE SMALL LETTER ME
                                       0x0ab8b, //  CHEROKEE SMALL LETTER MI
                                       0x0ab8c, //  CHEROKEE SMALL LETTER MO
                                       0x0ab8d, //  CHEROKEE SMALL LETTER MU
                                       0x0ab8e, //  CHEROKEE SMALL LETTER NA
                                       0x0ab8f, //  CHEROKEE SMALL LETTER HNA
                                       0x0ab90, //  CHEROKEE SMALL LETTER NAH
                                       0x0ab91, //  CHEROKEE SMALL LETTER NE
                                       0x0ab92, //  CHEROKEE SMALL LETTER NI
                                       0x0ab93, //  CHEROKEE SMALL LETTER NO
                                       0x0ab94, //  CHEROKEE SMALL LETTER NU
                                       0x0ab95, //  CHEROKEE SMALL LETTER NV
                                       0x0ab96, //  CHEROKEE SMALL LETTER QUA
                                       0x0ab97, //  CHEROKEE SMALL LETTER QUE
                                       0x0ab98, //  CHEROKEE SMALL LETTER QUI
                                       0x0ab99, //  CHEROKEE SMALL LETTER QUO
                                       0x0ab9a, //  CHEROKEE SMALL LETTER QUU
                                       0x0ab9b, //  CHEROKEE SMALL LETTER QUV
                                       0x0ab9c, //  CHEROKEE SMALL LETTER SA
                                       0x0ab9d, //  CHEROKEE SMALL LETTER S
                                       0x0ab9e, //  CHEROKEE SMALL LETTER SE
                                       0x0ab9f, //  CHEROKEE SMALL LETTER SI
                                       0x0aba0, //  CHEROKEE SMALL LETTER SO
                                       0x0aba1, //  CHEROKEE SMALL LETTER SU
                                       0x0aba2, //  CHEROKEE SMALL LETTER SV
                                       0x0aba3, //  CHEROKEE SMALL LETTER DA
                                       0x0aba4, //  CHEROKEE SMALL LETTER TA
                                       0x0aba5, //  CHEROKEE SMALL LETTER DE
                                       0x0aba6, //  CHEROKEE SMALL LETTER TE
                                       0x0aba7, //  CHEROKEE SMALL LETTER DI
                                       0x0aba8, //  CHEROKEE SMALL LETTER TI
                                       0x0aba9, //  CHEROKEE SMALL LETTER DO
                                       0x0abaa, //  CHEROKEE SMALL LETTER DU
                                       0x0abab, //  CHEROKEE SMALL LETTER DV
                                       0x0abac, //  CHEROKEE SMALL LETTER DLA
                                       0x0abad, //  CHEROKEE SMALL LETTER TLA
                                       0x0abae, //  CHEROKEE SMALL LETTER TLE
                                       0x0abaf, //  CHEROKEE SMALL LETTER TLI
                                       0x0abb0, //  CHEROKEE SMALL LETTER TLO
                                       0x0abb1, //  CHEROKEE SMALL LETTER TLU
                                       0x0abb2, //  CHEROKEE SMALL LETTER TLV
                                       0x0abb3, //  CHEROKEE SMALL LETTER TSA
                                       0x0abb4, //  CHEROKEE SMALL LETTER TSE
                                       0x0abb5, //  CHEROKEE SMALL LETTER TSI
                                       0x0abb6, //  CHEROKEE SMALL LETTER TSO
                                       0x0abb7, //  CHEROKEE SMALL LETTER TSU
                                       0x0abb8, //  CHEROKEE SMALL LETTER TSV
                                       0x0abb9, //  CHEROKEE SMALL LETTER WA
                                       0x0abba, //  CHEROKEE SMALL LETTER WE
                                       0x0abbb, //  CHEROKEE SMALL LETTER WI
                                       0x0abbc, //  CHEROKEE SMALL LETTER WO
                                       0x0abbd, //  CHEROKEE SMALL LETTER WU
                                       0x0abbe, //  CHEROKEE SMALL LETTER WV
                                       0x0abbf, //  CHEROKEE SMALL LETTER YA
                                       0x013f8, //  CHEROKEE SMALL LETTER YE
                                       0x013f9, //  CHEROKEE SMALL LETTER YI
                                       0x013fa, //  CHEROKEE SMALL LETTER YO
                                       0x013fb, //  CHEROKEE SMALL LETTER YU
                                       0x013fc, //  CHEROKEE SMALL LETTER YV
                                       0x013fd, //  CHEROKEE SMALL LETTER MV
                                       0x0a77d, //  LATIN CAPITAL LETTER INSULAR G
                                       0x02c63, //  LATIN CAPITAL LETTER P WITH STROKE
                                       0x0a7c6, //  LATIN CAPITAL LETTER Z WITH PALATAL HOOK
                                       0x01e00, //  LATIN CAPITAL LETTER A WITH RING BELOW
                                       0x01e02, //  LATIN CAPITAL LETTER B WITH DOT ABOVE
                                       0x01e04, //  LATIN CAPITAL LETTER B WITH DOT BELOW
                                       0x01e06, //  LATIN CAPITAL LETTER B WITH LINE BELOW
                                       0x01e08, //  LATIN CAPITAL LETTER C WITH CEDILLA AND ACUTE
                                       0x01e0a, //  LATIN CAPITAL LETTER D WITH DOT ABOVE
                                       0x01e0c, //  LATIN CAPITAL LETTER D WITH DOT BELOW
                                       0x01e0e, //  LATIN CAPITAL LETTER D WITH LINE BELOW
                                       0x01e10, //  LATIN CAPITAL LETTER D WITH CEDILLA
                                       0x01e12, //  LATIN CAPITAL LETTER D WITH CIRCUMFLEX BELOW
                                       0x01e14, //  LATIN CAPITAL LETTER E WITH MACRON AND GRAVE
                                       0x01e16, //  LATIN CAPITAL LETTER E WITH MACRON AND ACUTE
                                       0x01e18, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX BELOW
                                       0x01e1a, //  LATIN CAPITAL LETTER E WITH TILDE BELOW
                                       0x01e1c, //  LATIN CAPITAL LETTER E WITH CEDILLA AND BREVE
                                       0x01e1e, //  LATIN CAPITAL LETTER F WITH DOT ABOVE
                                       0x01e20, //  LATIN CAPITAL LETTER G WITH MACRON
                                       0x01e22, //  LATIN CAPITAL LETTER H WITH DOT ABOVE
                                       0x01e24, //  LATIN CAPITAL LETTER H WITH DOT BELOW
                                       0x01e26, //  LATIN CAPITAL LETTER H WITH DIAERESIS
                                       0x01e28, //  LATIN CAPITAL LETTER H WITH CEDILLA
                                       0x01e2a, //  LATIN CAPITAL LETTER H WITH BREVE BELOW
                                       0x01e2c, //  LATIN CAPITAL LETTER I WITH TILDE BELOW
                                       0x01e2e, //  LATIN CAPITAL LETTER I WITH DIAERESIS AND ACUTE
                                       0x01e30, //  LATIN CAPITAL LETTER K WITH ACUTE
                                       0x01e32, //  LATIN CAPITAL LETTER K WITH DOT BELOW
                                       0x01e34, //  LATIN CAPITAL LETTER K WITH LINE BELOW
                                       0x01e36, //  LATIN CAPITAL LETTER L WITH DOT BELOW
                                       0x01e38, //  LATIN CAPITAL LETTER L WITH DOT BELOW AND MACRON
                                       0x01e3a, //  LATIN CAPITAL LETTER L WITH LINE BELOW
                                       0x01e3c, //  LATIN CAPITAL LETTER L WITH CIRCUMFLEX BELOW
                                       0x01e3e, //  LATIN CAPITAL LETTER M WITH ACUTE
                                       0x01e40, //  LATIN CAPITAL LETTER M WITH DOT ABOVE
                                       0x01e42, //  LATIN CAPITAL LETTER M WITH DOT BELOW
                                       0x01e44, //  LATIN CAPITAL LETTER N WITH DOT ABOVE
                                       0x01e46, //  LATIN CAPITAL LETTER N WITH DOT BELOW
                                       0x01e48, //  LATIN CAPITAL LETTER N WITH LINE BELOW
                                       0x01e4a, //  LATIN CAPITAL LETTER N WITH CIRCUMFLEX BELOW
                                       0x01e4c, //  LATIN CAPITAL LETTER O WITH TILDE AND ACUTE
                                       0x01e4e, //  LATIN CAPITAL LETTER O WITH TILDE AND DIAERESIS
                                       0x01e50, //  LATIN CAPITAL LETTER O WITH MACRON AND GRAVE
                                       0x01e52, //  LATIN CAPITAL LETTER O WITH MACRON AND ACUTE
                                       0x01e54, //  LATIN CAPITAL LETTER P WITH ACUTE
                                       0x01e56, //  LATIN CAPITAL LETTER P WITH DOT ABOVE
                                       0x01e58, //  LATIN CAPITAL LETTER R WITH DOT ABOVE
                                       0x01e5a, //  LATIN CAPITAL LETTER R WITH DOT BELOW
                                       0x01e5c, //  LATIN CAPITAL LETTER R WITH DOT BELOW AND MACRON
                                       0x01e5e, //  LATIN CAPITAL LETTER R WITH LINE BELOW
                                       0x01e9b, //  LATIN SMALL LETTER LONG S WITH DOT ABOVE
                                       0x01e62, //  LATIN CAPITAL LETTER S WITH DOT BELOW
                                       0x01e64, //  LATIN CAPITAL LETTER S WITH ACUTE AND DOT ABOVE
                                       0x01e66, //  LATIN CAPITAL LETTER S WITH CARON AND DOT ABOVE
                                       0x01e68, //  LATIN CAPITAL LETTER S WITH DOT BELOW AND DOT ABOVE
                                       0x01e6a, //  LATIN CAPITAL LETTER T WITH DOT ABOVE
                                       0x01e6c, //  LATIN CAPITAL LETTER T WITH DOT BELOW
                                       0x01e6e, //  LATIN CAPITAL LETTER T WITH LINE BELOW
                                       0x01e70, //  LATIN CAPITAL LETTER T WITH CIRCUMFLEX BELOW
                                       0x01e72, //  LATIN CAPITAL LETTER U WITH DIAERESIS BELOW
                                       0x01e74, //  LATIN CAPITAL LETTER U WITH TILDE BELOW
                                       0x01e76, //  LATIN CAPITAL LETTER U WITH CIRCUMFLEX BELOW
                                       0x01e78, //  LATIN CAPITAL LETTER U WITH TILDE AND ACUTE
                                       0x01e7a, //  LATIN CAPITAL LETTER U WITH MACRON AND DIAERESIS
                                       0x01e7c, //  LATIN CAPITAL LETTER V WITH TILDE
                                       0x01e7e, //  LATIN CAPITAL LETTER V WITH DOT BELOW
                                       0x01e80, //  LATIN CAPITAL LETTER W WITH GRAVE
                                       0x01e82, //  LATIN CAPITAL LETTER W WITH ACUTE
                                       0x01e84, //  LATIN CAPITAL LETTER W WITH DIAERESIS
                                       0x01e86, //  LATIN CAPITAL LETTER W WITH DOT ABOVE
                                       0x01e88, //  LATIN CAPITAL LETTER W WITH DOT BELOW
                                       0x01e8a, //  LATIN CAPITAL LETTER X WITH DOT ABOVE
                                       0x01e8c, //  LATIN CAPITAL LETTER X WITH DIAERESIS
                                       0x01e8e, //  LATIN CAPITAL LETTER Y WITH DOT ABOVE
                                       0x01e90, //  LATIN CAPITAL LETTER Z WITH CIRCUMFLEX
                                       0x01e92, //  LATIN CAPITAL LETTER Z WITH DOT BELOW
                                       0x01e94, //  LATIN CAPITAL LETTER Z WITH LINE BELOW
                                       0x01ea0, //  LATIN CAPITAL LETTER A WITH DOT BELOW
                                       0x01ea2, //  LATIN CAPITAL LETTER A WITH HOOK ABOVE
                                       0x01ea4, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND ACUTE
                                       0x01ea6, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND GRAVE
                                       0x01ea8, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND HOOK ABOVE
                                       0x01eaa, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND TILDE
                                       0x01eac, //  LATIN CAPITAL LETTER A WITH CIRCUMFLEX AND DOT BELOW
                                       0x01eae, //  LATIN CAPITAL LETTER A WITH BREVE AND ACUTE
                                       0x01eb0, //  LATIN CAPITAL LETTER A WITH BREVE AND GRAVE
                                       0x01eb2, //  LATIN CAPITAL LETTER A WITH BREVE AND HOOK ABOVE
                                       0x01eb4, //  LATIN CAPITAL LETTER A WITH BREVE AND TILDE
                                       0x01eb6, //  LATIN CAPITAL LETTER A WITH BREVE AND DOT BELOW
                                       0x01eb8, //  LATIN CAPITAL LETTER E WITH DOT BELOW
                                       0x01eba, //  LATIN CAPITAL LETTER E WITH HOOK ABOVE
                                       0x01ebc, //  LATIN CAPITAL LETTER E WITH TILDE
                                       0x01ebe, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND ACUTE
                                       0x01ec0, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND GRAVE
                                       0x01ec2, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND HOOK ABOVE
                                       0x01ec4, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND TILDE
                                       0x01ec6, //  LATIN CAPITAL LETTER E WITH CIRCUMFLEX AND DOT BELOW
                                       0x01ec8, //  LATIN CAPITAL LETTER I WITH HOOK ABOVE
                                       0x01eca, //  LATIN CAPITAL LETTER I WITH DOT BELOW
                                       0x01ecc, //  LATIN CAPITAL LETTER O WITH DOT BELOW
                                       0x01ece, //  LATIN CAPITAL LETTER O WITH HOOK ABOVE
                                       0x01ed0, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND ACUTE
                                       0x01ed2, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND GRAVE
                                       0x01ed4, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND HOOK ABOVE
                                       0x01ed6, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND TILDE
                                       0x01ed8, //  LATIN CAPITAL LETTER O WITH CIRCUMFLEX AND DOT BELOW
                                       0x01eda, //  LATIN CAPITAL LETTER O WITH HORN AND ACUTE
                                       0x01edc, //  LATIN CAPITAL LETTER O WITH HORN AND GRAVE
                                       0x01ede, //  LATIN CAPITAL LETTER O WITH HORN AND HOOK ABOVE
                                       0x01ee0, //  LATIN CAPITAL LETTER O WITH HORN AND TILDE
                                       0x01ee2, //  LATIN CAPITAL LETTER O WITH HORN AND DOT BELOW
                                       0x01ee4, //  LATIN CAPITAL LETTER U WITH DOT BELOW
                                       0x01ee6, //  LATIN CAPITAL LETTER U WITH HOOK ABOVE
                                       0x01ee8, //  LATIN CAPITAL LETTER U WITH HORN AND ACUTE
                                       0x01eea, //  LATIN CAPITAL LETTER U WITH HORN AND GRAVE
                                       0x01eec, //  LATIN CAPITAL LETTER U WITH HORN AND HOOK ABOVE
                                       0x01eee, //  LATIN CAPITAL LETTER U WITH HORN AND TILDE
                                       0x01ef0, //  LATIN CAPITAL LETTER U WITH HORN AND DOT BELOW
                                       0x01ef2, //  LATIN CAPITAL LETTER Y WITH GRAVE
                                       0x01ef4, //  LATIN CAPITAL LETTER Y WITH DOT BELOW
                                       0x01ef6, //  LATIN CAPITAL LETTER Y WITH HOOK ABOVE
                                       0x01ef8, //  LATIN CAPITAL LETTER Y WITH TILDE
                                       0x01efa, //  LATIN CAPITAL LETTER MIDDLE-WELSH LL
                                       0x01efc, //  LATIN CAPITAL LETTER MIDDLE-WELSH V
                                       0x01efe, //  LATIN CAPITAL LETTER Y WITH LOOP
                                       0x01f08, //  GREEK CAPITAL LETTER ALPHA WITH PSILI
                                       0x01f09, //  GREEK CAPITAL LETTER ALPHA WITH DASIA
                                       0x01f0a, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA
                                       0x01f0b, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA
                                       0x01f0c, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA
                                       0x01f0d, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA
                                       0x01f0e, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI
                                       0x01f0f, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI
                                       0x01f18, //  GREEK CAPITAL LETTER EPSILON WITH PSILI
                                       0x01f19, //  GREEK CAPITAL LETTER EPSILON WITH DASIA
                                       0x01f1a, //  GREEK CAPITAL LETTER EPSILON WITH PSILI AND VARIA
                                       0x01f1b, //  GREEK CAPITAL LETTER EPSILON WITH DASIA AND VARIA
                                       0x01f1c, //  GREEK CAPITAL LETTER EPSILON WITH PSILI AND OXIA
                                       0x01f1d, //  GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
                                       0x01f28, //  GREEK CAPITAL LETTER ETA WITH PSILI
                                       0x01f29, //  GREEK CAPITAL LETTER ETA WITH DASIA
                                       0x01f2a, //  GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA
                                       0x01f2b, //  GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA
                                       0x01f2c, //  GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA
                                       0x01f2d, //  GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA
                                       0x01f2e, //  GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI
                                       0x01f2f, //  GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI
                                       0x01f38, //  GREEK CAPITAL LETTER IOTA WITH PSILI
                                       0x01f39, //  GREEK CAPITAL LETTER IOTA WITH DASIA
                                       0x01f3a, //  GREEK CAPITAL LETTER IOTA WITH PSILI AND VARIA
                                       0x01f3b, //  GREEK CAPITAL LETTER IOTA WITH DASIA AND VARIA
                                       0x01f3c, //  GREEK CAPITAL LETTER IOTA WITH PSILI AND OXIA
                                       0x01f3d, //  GREEK CAPITAL LETTER IOTA WITH DASIA AND OXIA
                                       0x01f3e, //  GREEK CAPITAL LETTER IOTA WITH PSILI AND PERISPOMENI
                                       0x01f3f, //  GREEK CAPITAL LETTER IOTA WITH DASIA AND PERISPOMENI
                                       0x01f48, //  GREEK CAPITAL LETTER OMICRON WITH PSILI
                                       0x01f49, //  GREEK CAPITAL LETTER OMICRON WITH DASIA
                                       0x01f4a, //  GREEK CAPITAL LETTER OMICRON WITH PSILI AND VARIA
                                       0x01f4b, //  GREEK CAPITAL LETTER OMICRON WITH DASIA AND VARIA
                                       0x01f4c, //  GREEK CAPITAL LETTER OMICRON WITH PSILI AND OXIA
                                       0x01f4d, //  GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
                                       0x01f59, //  GREEK CAPITAL LETTER UPSILON WITH DASIA
                                       0x01f5b, //  GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
                                       0x01f5d, //  GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
                                       0x01f5f, //  GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI
                                       0x01f68, //  GREEK CAPITAL LETTER OMEGA WITH PSILI
                                       0x01f69, //  GREEK CAPITAL LETTER OMEGA WITH DASIA
                                       0x01f6a, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA
                                       0x01f6b, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA
                                       0x01f6c, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA
                                       0x01f6d, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA
                                       0x01f6e, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI
                                       0x01f6f, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI
                                       0x01fba, //  GREEK CAPITAL LETTER ALPHA WITH VARIA
                                       0x01fbb, //  GREEK CAPITAL LETTER ALPHA WITH OXIA
                                       0x01fc8, //  GREEK CAPITAL LETTER EPSILON WITH VARIA
                                       0x01fc9, //  GREEK CAPITAL LETTER EPSILON WITH OXIA
                                       0x01fca, //  GREEK CAPITAL LETTER ETA WITH VARIA
                                       0x01fcb, //  GREEK CAPITAL LETTER ETA WITH OXIA
                                       0x01fda, //  GREEK CAPITAL LETTER IOTA WITH VARIA
                                       0x01fdb, //  GREEK CAPITAL LETTER IOTA WITH OXIA
                                       0x01ff8, //  GREEK CAPITAL LETTER OMICRON WITH VARIA
                                       0x01ff9, //  GREEK CAPITAL LETTER OMICRON WITH OXIA
                                       0x01fea, //  GREEK CAPITAL LETTER UPSILON WITH VARIA
                                       0x01feb, //  GREEK CAPITAL LETTER UPSILON WITH OXIA
                                       0x01ffa, //  GREEK CAPITAL LETTER OMEGA WITH VARIA
                                       0x01ffb, //  GREEK CAPITAL LETTER OMEGA WITH OXIA
                                       0x01f88, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND PROSGEGRAMMENI
                                       0x01f89, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND PROSGEGRAMMENI
                                       0x01f8a, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                                       0x01f8b, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                                       0x01f8c, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                                       0x01f8d, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                                       0x01f8e, //  GREEK CAPITAL LETTER ALPHA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                                       0x01f8f, //  GREEK CAPITAL LETTER ALPHA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                                       0x01f98, //  GREEK CAPITAL LETTER ETA WITH PSILI AND PROSGEGRAMMENI
                                       0x01f99, //  GREEK CAPITAL LETTER ETA WITH DASIA AND PROSGEGRAMMENI
                                       0x01f9a, //  GREEK CAPITAL LETTER ETA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                                       0x01f9b, //  GREEK CAPITAL LETTER ETA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                                       0x01f9c, //  GREEK CAPITAL LETTER ETA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                                       0x01f9d, //  GREEK CAPITAL LETTER ETA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                                       0x01f9e, //  GREEK CAPITAL LETTER ETA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                                       0x01f9f, //  GREEK CAPITAL LETTER ETA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                                       0x01fa8, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND PROSGEGRAMMENI
                                       0x01fa9, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND PROSGEGRAMMENI
                                       0x01faa, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND VARIA AND PROSGEGRAMMENI
                                       0x01fab, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND VARIA AND PROSGEGRAMMENI
                                       0x01fac, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND OXIA AND PROSGEGRAMMENI
                                       0x01fad, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND OXIA AND PROSGEGRAMMENI
                                       0x01fae, //  GREEK CAPITAL LETTER OMEGA WITH PSILI AND PERISPOMENI AND PROSGEGRAMMENI
                                       0x01faf, //  GREEK CAPITAL LETTER OMEGA WITH DASIA AND PERISPOMENI AND PROSGEGRAMMENI
                                       0x01fb8, //  GREEK CAPITAL LETTER ALPHA WITH VRACHY
                                       0x01fb9, //  GREEK CAPITAL LETTER ALPHA WITH MACRON
                                       0x01fbc, //  GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
                                       0x01fcc, //  GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
                                       0x01fd8, //  GREEK CAPITAL LETTER IOTA WITH VRACHY
                                       0x01fd9, //  GREEK CAPITAL LETTER IOTA WITH MACRON
                                       0x01fe8, //  GREEK CAPITAL LETTER UPSILON WITH VRACHY
                                       0x01fe9, //  GREEK CAPITAL LETTER UPSILON WITH MACRON
                                       0x01fec, //  GREEK CAPITAL LETTER RHO WITH DASIA
                                       0x01ffc, //  GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
                                       0x02132, //  TURNED CAPITAL F
                                       0x02160, //  ROMAN NUMERAL ONE
                                       0x02161, //  ROMAN NUMERAL TWO
                                       0x02162, //  ROMAN NUMERAL THREE
                                       0x02163, //  ROMAN NUMERAL FOUR
                                       0x02164, //  ROMAN NUMERAL FIVE
                                       0x02165, //  ROMAN NUMERAL SIX
                                       0x02166, //  ROMAN NUMERAL SEVEN
                                       0x02167, //  ROMAN NUMERAL EIGHT
                                       0x02168, //  ROMAN NUMERAL NINE
                                       0x02169, //  ROMAN NUMERAL TEN
                                       0x0216a, //  ROMAN NUMERAL ELEVEN
                                       0x0216b, //  ROMAN NUMERAL TWELVE
                                       0x0216c, //  ROMAN NUMERAL FIFTY
                                       0x0216d, //  ROMAN NUMERAL ONE HUNDRED
                                       0x0216e, //  ROMAN NUMERAL FIVE HUNDRED
                                       0x0216f, //  ROMAN NUMERAL ONE THOUSAND
                                       0x02183, //  ROMAN NUMERAL REVERSED ONE HUNDRED
                                       0x024b6, //  CIRCLED LATIN CAPITAL LETTER A
                                       0x024b7, //  CIRCLED LATIN CAPITAL LETTER B
                                       0x024b8, //  CIRCLED LATIN CAPITAL LETTER C
                                       0x024b9, //  CIRCLED LATIN CAPITAL LETTER D
                                       0x024ba, //  CIRCLED LATIN CAPITAL LETTER E
                                       0x024bb, //  CIRCLED LATIN CAPITAL LETTER F
                                       0x024bc, //  CIRCLED LATIN CAPITAL LETTER G
                                       0x024bd, //  CIRCLED LATIN CAPITAL LETTER H
                                       0x024be, //  CIRCLED LATIN CAPITAL LETTER I
                                       0x024bf, //  CIRCLED LATIN CAPITAL LETTER J
                                       0x024c0, //  CIRCLED LATIN CAPITAL LETTER K
                                       0x024c1, //  CIRCLED LATIN CAPITAL LETTER L
                                       0x024c2, //  CIRCLED LATIN CAPITAL LETTER M
                                       0x024c3, //  CIRCLED LATIN CAPITAL LETTER N
                                       0x024c4, //  CIRCLED LATIN CAPITAL LETTER O
                                       0x024c5, //  CIRCLED LATIN CAPITAL LETTER P
                                       0x024c6, //  CIRCLED LATIN CAPITAL LETTER Q
                                       0x024c7, //  CIRCLED LATIN CAPITAL LETTER R
                                       0x024c8, //  CIRCLED LATIN CAPITAL LETTER S
                                       0x024c9, //  CIRCLED LATIN CAPITAL LETTER T
                                       0x024ca, //  CIRCLED LATIN CAPITAL LETTER U
                                       0x024cb, //  CIRCLED LATIN CAPITAL LETTER V
                                       0x024cc, //  CIRCLED LATIN CAPITAL LETTER W
                                       0x024cd, //  CIRCLED LATIN CAPITAL LETTER X
                                       0x024ce, //  CIRCLED LATIN CAPITAL LETTER Y
                                       0x024cf, //  CIRCLED LATIN CAPITAL LETTER Z
                                       0x02c00, //  GLAGOLITIC CAPITAL LETTER AZU
                                       0x02c01, //  GLAGOLITIC CAPITAL LETTER BUKY
                                       0x02c02, //  GLAGOLITIC CAPITAL LETTER VEDE
                                       0x02c03, //  GLAGOLITIC CAPITAL LETTER GLAGOLI
                                       0x02c04, //  GLAGOLITIC CAPITAL LETTER DOBRO
                                       0x02c05, //  GLAGOLITIC CAPITAL LETTER YESTU
                                       0x02c06, //  GLAGOLITIC CAPITAL LETTER ZHIVETE
                                       0x02c07, //  GLAGOLITIC CAPITAL LETTER DZELO
                                       0x02c08, //  GLAGOLITIC CAPITAL LETTER ZEMLJA
                                       0x02c09, //  GLAGOLITIC CAPITAL LETTER IZHE
                                       0x02c0a, //  GLAGOLITIC CAPITAL LETTER INITIAL IZHE
                                       0x02c0b, //  GLAGOLITIC CAPITAL LETTER I
                                       0x02c0c, //  GLAGOLITIC CAPITAL LETTER DJERVI
                                       0x02c0d, //  GLAGOLITIC CAPITAL LETTER KAKO
                                       0x02c0e, //  GLAGOLITIC CAPITAL LETTER LJUDIJE
                                       0x02c0f, //  GLAGOLITIC CAPITAL LETTER MYSLITE
                                       0x02c10, //  GLAGOLITIC CAPITAL LETTER NASHI
                                       0x02c11, //  GLAGOLITIC CAPITAL LETTER ONU
                                       0x02c12, //  GLAGOLITIC CAPITAL LETTER POKOJI
                                       0x02c13, //  GLAGOLITIC CAPITAL LETTER RITSI
                                       0x02c14, //  GLAGOLITIC CAPITAL LETTER SLOVO
                                       0x02c15, //  GLAGOLITIC CAPITAL LETTER TVRIDO
                                       0x02c16, //  GLAGOLITIC CAPITAL LETTER UKU
                                       0x02c17, //  GLAGOLITIC CAPITAL LETTER FRITU
                                       0x02c18, //  GLAGOLITIC CAPITAL LETTER HERU
                                       0x02c19, //  GLAGOLITIC CAPITAL LETTER OTU
                                       0x02c1a, //  GLAGOLITIC CAPITAL LETTER PE
                                       0x02c1b, //  GLAGOLITIC CAPITAL LETTER SHTA
                                       0x02c1c, //  GLAGOLITIC CAPITAL LETTER TSI
                                       0x02c1d, //  GLAGOLITIC CAPITAL LETTER CHRIVI
                                       0x02c1e, //  GLAGOLITIC CAPITAL LETTER SHA
                                       0x02c1f, //  GLAGOLITIC CAPITAL LETTER YERU
                                       0x02c20, //  GLAGOLITIC CAPITAL LETTER YERI
                                       0x02c21, //  GLAGOLITIC CAPITAL LETTER YATI
                                       0x02c22, //  GLAGOLITIC CAPITAL LETTER SPIDERY HA
                                       0x02c23, //  GLAGOLITIC CAPITAL LETTER YU
                                       0x02c24, //  GLAGOLITIC CAPITAL LETTER SMALL YUS
                                       0x02c25, //  GLAGOLITIC CAPITAL LETTER SMALL YUS WITH TAIL
                                       0x02c26, //  GLAGOLITIC CAPITAL LETTER YO
                                       0x02c27, //  GLAGOLITIC CAPITAL LETTER IOTATED SMALL YUS
                                       0x02c28, //  GLAGOLITIC CAPITAL LETTER BIG YUS
                                       0x02c29, //  GLAGOLITIC CAPITAL LETTER IOTATED BIG YUS
                                       0x02c2a, //  GLAGOLITIC CAPITAL LETTER FITA
                                       0x02c2b, //  GLAGOLITIC CAPITAL LETTER IZHITSA
                                       0x02c2c, //  GLAGOLITIC CAPITAL LETTER SHTAPIC
                                       0x02c2d, //  GLAGOLITIC CAPITAL LETTER TROKUTASTI A
                                       0x02c2e, //  GLAGOLITIC CAPITAL LETTER LATINATE MYSLITE
                                       0x02c2f, //  GLAGOLITIC CAPITAL LETTER CAUDATE CHRIVI
                                       0x02c60, //  LATIN CAPITAL LETTER L WITH DOUBLE BAR
                                       0x0023a, //  LATIN CAPITAL LETTER A WITH STROKE
                                       0x0023e, //  LATIN CAPITAL LETTER T WITH DIAGONAL STROKE
                                       0x02c67, //  LATIN CAPITAL LETTER H WITH DESCENDER
                                       0x02c69, //  LATIN CAPITAL LETTER K WITH DESCENDER
                                       0x02c6b, //  LATIN CAPITAL LETTER Z WITH DESCENDER
                                       0x02c72, //  LATIN CAPITAL LETTER W WITH HOOK
                                       0x02c75, //  LATIN CAPITAL LETTER HALF H
                                       0x02c80, //  COPTIC CAPITAL LETTER ALFA
                                       0x02c82, //  COPTIC CAPITAL LETTER VIDA
                                       0x02c84, //  COPTIC CAPITAL LETTER GAMMA
                                       0x02c86, //  COPTIC CAPITAL LETTER DALDA
                                       0x02c88, //  COPTIC CAPITAL LETTER EIE
                                       0x02c8a, //  COPTIC CAPITAL LETTER SOU
                                       0x02c8c, //  COPTIC CAPITAL LETTER ZATA
                                       0x02c8e, //  COPTIC CAPITAL LETTER HATE
                                       0x02c90, //  COPTIC CAPITAL LETTER THETHE
                                       0x02c92, //  COPTIC CAPITAL LETTER IAUDA
                                       0x02c94, //  COPTIC CAPITAL LETTER KAPA
                                       0x02c96, //  COPTIC CAPITAL LETTER LAULA
                                       0x02c98, //  COPTIC CAPITAL LETTER MI
                                       0x02c9a, //  COPTIC CAPITAL LETTER NI
                                       0x02c9c, //  COPTIC CAPITAL LETTER KSI
                                       0x02c9e, //  COPTIC CAPITAL LETTER O
                                       0x02ca0, //  COPTIC CAPITAL LETTER PI
                                       0x02ca2, //  COPTIC CAPITAL LETTER RO
                                       0x02ca4, //  COPTIC CAPITAL LETTER SIMA
                                       0x02ca6, //  COPTIC CAPITAL LETTER TAU
                                       0x02ca8, //  COPTIC CAPITAL LETTER UA
                                       0x02caa, //  COPTIC CAPITAL LETTER FI
                                       0x02cac, //  COPTIC CAPITAL LETTER KHI
                                       0x02cae, //  COPTIC CAPITAL LETTER PSI
                                       0x02cb0, //  COPTIC CAPITAL LETTER OOU
                                       0x02cb2, //  COPTIC CAPITAL LETTER DIALECT-P ALEF
                                       0x02cb4, //  COPTIC CAPITAL LETTER OLD COPTIC AIN
                                       0x02cb6, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC EIE
                                       0x02cb8, //  COPTIC CAPITAL LETTER DIALECT-P KAPA
                                       0x02cba, //  COPTIC CAPITAL LETTER DIALECT-P NI
                                       0x02cbc, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC NI
                                       0x02cbe, //  COPTIC CAPITAL LETTER OLD COPTIC OOU
                                       0x02cc0, //  COPTIC CAPITAL LETTER SAMPI
                                       0x02cc2, //  COPTIC CAPITAL LETTER CROSSED SHEI
                                       0x02cc4, //  COPTIC CAPITAL LETTER OLD COPTIC SHEI
                                       0x02cc6, //  COPTIC CAPITAL LETTER OLD COPTIC ESH
                                       0x02cc8, //  COPTIC CAPITAL LETTER AKHMIMIC KHEI
                                       0x02cca, //  COPTIC CAPITAL LETTER DIALECT-P HORI
                                       0x02ccc, //  COPTIC CAPITAL LETTER OLD COPTIC HORI
                                       0x02cce, //  COPTIC CAPITAL LETTER OLD COPTIC HA
                                       0x02cd0, //  COPTIC CAPITAL LETTER L-SHAPED HA
                                       0x02cd2, //  COPTIC CAPITAL LETTER OLD COPTIC HEI
                                       0x02cd4, //  COPTIC CAPITAL LETTER OLD COPTIC HAT
                                       0x02cd6, //  COPTIC CAPITAL LETTER OLD COPTIC GANGIA
                                       0x02cd8, //  COPTIC CAPITAL LETTER OLD COPTIC DJA
                                       0x02cda, //  COPTIC CAPITAL LETTER OLD COPTIC SHIMA
                                       0x02cdc, //  COPTIC CAPITAL LETTER OLD NUBIAN SHIMA
                                       0x02cde, //  COPTIC CAPITAL LETTER OLD NUBIAN NGI
                                       0x02ce0, //  COPTIC CAPITAL LETTER OLD NUBIAN NYI
                                       0x02ce2, //  COPTIC CAPITAL LETTER OLD NUBIAN WAU
                                       0x02ceb, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI
                                       0x02ced, //  COPTIC CAPITAL LETTER CRYPTOGRAMMIC GANGIA
                                       0x02cf2, //  COPTIC CAPITAL LETTER BOHAIRIC KHEI
                                       0x010a0, //  GEORGIAN CAPITAL LETTER AN
                                       0x010a1, //  GEORGIAN CAPITAL LETTER BAN
                                       0x010a2, //  GEORGIAN CAPITAL LETTER GAN
                                       0x010a3, //  GEORGIAN CAPITAL LETTER DON
                                       0x010a4, //  GEORGIAN CAPITAL LETTER EN
                                       0x010a5, //  GEORGIAN CAPITAL LETTER VIN
                                       0x010a6, //  GEORGIAN CAPITAL LETTER ZEN
                                       0x010a7, //  GEORGIAN CAPITAL LETTER TAN
                                       0x010a8, //  GEORGIAN CAPITAL LETTER IN
                                       0x010a9, //  GEORGIAN CAPITAL LETTER KAN
                                       0x010aa, //  GEORGIAN CAPITAL LETTER LAS
                                       0x010ab, //  GEORGIAN CAPITAL LETTER MAN
                                       0x010ac, //  GEORGIAN CAPITAL LETTER NAR
                                       0x010ad, //  GEORGIAN CAPITAL LETTER ON
                                       0x010ae, //  GEORGIAN CAPITAL LETTER PAR
                                       0x010af, //  GEORGIAN CAPITAL LETTER ZHAR
                                       0x010b0, //  GEORGIAN CAPITAL LETTER RAE
                                       0x010b1, //  GEORGIAN CAPITAL LETTER SAN
                                       0x010b2, //  GEORGIAN CAPITAL LETTER TAR
                                       0x010b3, //  GEORGIAN CAPITAL LETTER UN
                                       0x010b4, //  GEORGIAN CAPITAL LETTER PHAR
                                       0x010b5, //  GEORGIAN CAPITAL LETTER KHAR
                                       0x010b6, //  GEORGIAN CAPITAL LETTER GHAN
                                       0x010b7, //  GEORGIAN CAPITAL LETTER QAR
                                       0x010b8, //  GEORGIAN CAPITAL LETTER SHIN
                                       0x010b9, //  GEORGIAN CAPITAL LETTER CHIN
                                       0x010ba, //  GEORGIAN CAPITAL LETTER CAN
                                       0x010bb, //  GEORGIAN CAPITAL LETTER JIL
                                       0x010bc, //  GEORGIAN CAPITAL LETTER CIL
                                       0x010bd, //  GEORGIAN CAPITAL LETTER CHAR
                                       0x010be, //  GEORGIAN CAPITAL LETTER XAN
                                       0x010bf, //  GEORGIAN CAPITAL LETTER JHAN
                                       0x010c0, //  GEORGIAN CAPITAL LETTER HAE
                                       0x010c1, //  GEORGIAN CAPITAL LETTER HE
                                       0x010c2, //  GEORGIAN CAPITAL LETTER HIE
                                       0x010c3, //  GEORGIAN CAPITAL LETTER WE
                                       0x010c4, //  GEORGIAN CAPITAL LETTER HAR
                                       0x010c5, //  GEORGIAN CAPITAL LETTER HOE
                                       0x010c7, //  GEORGIAN CAPITAL LETTER YN
                                       0x010cd, //  GEORGIAN CAPITAL LETTER AEN
                                       0x0a640, //  CYRILLIC CAPITAL LETTER ZEMLYA
                                       0x0a642, //  CYRILLIC CAPITAL LETTER DZELO
                                       0x0a644, //  CYRILLIC CAPITAL LETTER REVERSED DZE
                                       0x0a646, //  CYRILLIC CAPITAL LETTER IOTA
                                       0x0a648, //  CYRILLIC CAPITAL LETTER DJERV
                                       0x0a64a, //  CYRILLIC CAPITAL LETTER MONOGRAPH UK
                                       0x0a64c, //  CYRILLIC CAPITAL LETTER BROAD OMEGA
                                       0x0a64e, //  CYRILLIC CAPITAL LETTER NEUTRAL YER
                                       0x0a650, //  CYRILLIC CAPITAL LETTER YERU WITH BACK YER
                                       0x0a652, //  CYRILLIC CAPITAL LETTER IOTIFIED YAT
                                       0x0a654, //  CYRILLIC CAPITAL LETTER REVERSED YU
                                       0x0a656, //  CYRILLIC CAPITAL LETTER IOTIFIED A
                                       0x0a658, //  CYRILLIC CAPITAL LETTER CLOSED LITTLE YUS
                                       0x0a65a, //  CYRILLIC CAPITAL LETTER BLENDED YUS
                                       0x0a65c, //  CYRILLIC CAPITAL LETTER IOTIFIED CLOSED LITTLE YUS
                                       0x0a65e, //  CYRILLIC CAPITAL LETTER YN
                                       0x0a660, //  CYRILLIC CAPITAL LETTER REVERSED TSE
                                       0x0a662, //  CYRILLIC CAPITAL LETTER SOFT DE
                                       0x0a664, //  CYRILLIC CAPITAL LETTER SOFT EL
                                       0x0a666, //  CYRILLIC CAPITAL LETTER SOFT EM
                                       0x0a668, //  CYRILLIC CAPITAL LETTER MONOCULAR O
                                       0x0a66a, //  CYRILLIC CAPITAL LETTER BINOCULAR O
                                       0x0a66c, //  CYRILLIC CAPITAL LETTER DOUBLE MONOCULAR O
                                       0x0a680, //  CYRILLIC CAPITAL LETTER DWE
                                       0x0a682, //  CYRILLIC CAPITAL LETTER DZWE
                                       0x0a684, //  CYRILLIC CAPITAL LETTER ZHWE
                                       0x0a686, //  CYRILLIC CAPITAL LETTER CCHE
                                       0x0a688, //  CYRILLIC CAPITAL LETTER DZZE
                                       0x0a68a, //  CYRILLIC CAPITAL LETTER TE WITH MIDDLE HOOK
                                       0x0a68c, //  CYRILLIC CAPITAL LETTER TWE
                                       0x0a68e, //  CYRILLIC CAPITAL LETTER TSWE
                                       0x0a690, //  CYRILLIC CAPITAL LETTER TSSE
                                       0x0a692, //  CYRILLIC CAPITAL LETTER TCHE
                                       0x0a694, //  CYRILLIC CAPITAL LETTER HWE
                                       0x0a696, //  CYRILLIC CAPITAL LETTER SHWE
                                       0x0a698, //  CYRILLIC CAPITAL LETTER DOUBLE O
                                       0x0a69a, //  CYRILLIC CAPITAL LETTER CROSSED O
                                       0x0a722, //  LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF
                                       0x0a724, //  LATIN CAPITAL LETTER EGYPTOLOGICAL AIN
                                       0x0a726, //  LATIN CAPITAL LETTER HENG
                                       0x0a728, //  LATIN CAPITAL LETTER TZ
                                       0x0a72a, //  LATIN CAPITAL LETTER TRESILLO
                                       0x0a72c, //  LATIN CAPITAL LETTER CUATRILLO
                                       0x0a72e, //  LATIN CAPITAL LETTER CUATRILLO WITH COMMA
                                       0x0a732, //  LATIN CAPITAL LETTER AA
                                       0x0a734, //  LATIN CAPITAL LETTER AO
                                       0x0a736, //  LATIN CAPITAL LETTER AU
                                       0x0a738, //  LATIN CAPITAL LETTER AV
                                       0x0a73a, //  LATIN CAPITAL LETTER AV WITH HORIZONTAL BAR
                                       0x0a73c, //  LATIN CAPITAL LETTER AY
                                       0x0a73e, //  LATIN CAPITAL LETTER REVERSED C WITH DOT
                                       0x0a740, //  LATIN CAPITAL LETTER K WITH STROKE
                                       0x0a742, //  LATIN CAPITAL LETTER K WITH DIAGONAL STROKE
                                       0x0a744, //  LATIN CAPITAL LETTER K WITH STROKE AND DIAGONAL STROKE
                                       0x0a746, //  LATIN CAPITAL LETTER BROKEN L
                                       0x0a748, //  LATIN CAPITAL LETTER L WITH HIGH STROKE
                                       0x0a74a, //  LATIN CAPITAL LETTER O WITH LONG STROKE OVERLAY
                                       0x0a74c, //  LATIN CAPITAL LETTER O WITH LOOP
                                       0x0a74e, //  LATIN CAPITAL LETTER OO
                                       0x0a750, //  LATIN CAPITAL LETTER P WITH STROKE THROUGH DESCENDER
                                       0x0a752, //  LATIN CAPITAL LETTER P WITH FLOURISH
                                       0x0a754, //  LATIN CAPITAL LETTER P WITH SQUIRREL TAIL
                                       0x0a756, //  LATIN CAPITAL LETTER Q WITH STROKE THROUGH DESCENDER
                                       0x0a758, //  LATIN CAPITAL LETTER Q WITH DIAGONAL STROKE
                                       0x0a75a, //  LATIN CAPITAL LETTER R ROTUNDA
                                       0x0a75c, //  LATIN CAPITAL LETTER RUM ROTUNDA
                                       0x0a75e, //  LATIN CAPITAL LETTER V WITH DIAGONAL STROKE
                                       0x0a760, //  LATIN CAPITAL LETTER VY
                                       0x0a762, //  LATIN CAPITAL LETTER VISIGOTHIC Z
                                       0x0a764, //  LATIN CAPITAL LETTER THORN WITH STROKE
                                       0x0a766, //  LATIN CAPITAL LETTER THORN WITH STROKE THROUGH DESCENDER
                                       0x0a768, //  LATIN CAPITAL LETTER VEND
                                       0x0a76a, //  LATIN CAPITAL LETTER ET
                                       0x0a76c, //  LATIN CAPITAL LETTER IS
                                       0x0a76e, //  LATIN CAPITAL LETTER CON
                                       0x0a779, //  LATIN CAPITAL LETTER INSULAR D
                                       0x0a77b, //  LATIN CAPITAL LETTER INSULAR F
                                       0x0a77e, //  LATIN CAPITAL LETTER TURNED INSULAR G
                                       0x0a780, //  LATIN CAPITAL LETTER TURNED L
                                       0x0a782, //  LATIN CAPITAL LETTER INSULAR R
                                       0x0a784, //  LATIN CAPITAL LETTER INSULAR S
                                       0x0a786, //  LATIN CAPITAL LETTER INSULAR T
                                       0x0a78b, //  LATIN CAPITAL LETTER SALTILLO
                                       0x0a790, //  LATIN CAPITAL LETTER N WITH DESCENDER
                                       0x0a792, //  LATIN CAPITAL LETTER C WITH BAR
                                       0x0a7c4, //  LATIN CAPITAL LETTER C WITH PALATAL HOOK
                                       0x0a796, //  LATIN CAPITAL LETTER B WITH FLOURISH
                                       0x0a798, //  LATIN CAPITAL LETTER F WITH STROKE
                                       0x0a79a, //  LATIN CAPITAL LETTER VOLAPUK AE
                                       0x0a79c, //  LATIN CAPITAL LETTER VOLAPUK OE
                                       0x0a79e, //  LATIN CAPITAL LETTER VOLAPUK UE
                                       0x0a7a0, //  LATIN CAPITAL LETTER G WITH OBLIQUE STROKE
                                       0x0a7a2, //  LATIN CAPITAL LETTER K WITH OBLIQUE STROKE
                                       0x0a7a4, //  LATIN CAPITAL LETTER N WITH OBLIQUE STROKE
                                       0x0a7a6, //  LATIN CAPITAL LETTER R WITH OBLIQUE STROKE
                                       0x0a7a8, //  LATIN CAPITAL LETTER S WITH OBLIQUE STROKE
                                       0x0a7b4, //  LATIN CAPITAL LETTER BETA
                                       0x0a7b6, //  LATIN CAPITAL LETTER OMEGA
                                       0x0a7b8, //  LATIN CAPITAL LETTER U WITH STROKE
                                       0x0a7ba, //  LATIN CAPITAL LETTER GLOTTAL A
                                       0x0a7bc, //  LATIN CAPITAL LETTER GLOTTAL I
                                       0x0a7be, //  LATIN CAPITAL LETTER GLOTTAL U
                                       0x0a7c0, //  LATIN CAPITAL LETTER OLD POLISH O
                                       0x0a7c2, //  LATIN CAPITAL LETTER ANGLICANA W
                                       0x0a7c7, //  LATIN CAPITAL LETTER D WITH SHORT STROKE OVERLAY
                                       0x0a7c9, //  LATIN CAPITAL LETTER S WITH SHORT STROKE OVERLAY
                                       0x0a7d0, //  LATIN CAPITAL LETTER CLOSED INSULAR G
                                       0x0a7d6, //  LATIN CAPITAL LETTER MIDDLE SCOTS S
                                       0x0a7d8, //  LATIN CAPITAL LETTER SIGMOID S
                                       0x0a7f5, //  LATIN CAPITAL LETTER REVERSED HALF H
                                       0x0a7b3, //  LATIN CAPITAL LETTER CHI
                                       0x0ff21, //  FULLWIDTH LATIN CAPITAL LETTER A
                                       0x0ff22, //  FULLWIDTH LATIN CAPITAL LETTER B
                                       0x0ff23, //  FULLWIDTH LATIN CAPITAL LETTER C
                                       0x0ff24, //  FULLWIDTH LATIN CAPITAL LETTER D
                                       0x0ff25, //  FULLWIDTH LATIN CAPITAL LETTER E
                                       0x0ff26, //  FULLWIDTH LATIN CAPITAL LETTER F
                                       0x0ff27, //  FULLWIDTH LATIN CAPITAL LETTER G
                                       0x0ff28, //  FULLWIDTH LATIN CAPITAL LETTER H
                                       0x0ff29, //  FULLWIDTH LATIN CAPITAL LETTER I
                                       0x0ff2a, //  FULLWIDTH LATIN CAPITAL LETTER J
                                       0x0ff2b, //  FULLWIDTH LATIN CAPITAL LETTER K
                                       0x0ff2c, //  FULLWIDTH LATIN CAPITAL LETTER L
                                       0x0ff2d, //  FULLWIDTH LATIN CAPITAL LETTER M
                                       0x0ff2e, //  FULLWIDTH LATIN CAPITAL LETTER N
                                       0x0ff2f, //  FULLWIDTH LATIN CAPITAL LETTER O
                                       0x0ff30, //  FULLWIDTH LATIN CAPITAL LETTER P
                                       0x0ff31, //  FULLWIDTH LATIN CAPITAL LETTER Q
                                       0x0ff32, //  FULLWIDTH LATIN CAPITAL LETTER R
                                       0x0ff33, //  FULLWIDTH LATIN CAPITAL LETTER S
                                       0x0ff34, //  FULLWIDTH LATIN CAPITAL LETTER T
                                       0x0ff35, //  FULLWIDTH LATIN CAPITAL LETTER U
                                       0x0ff36, //  FULLWIDTH LATIN CAPITAL LETTER V
                                       0x0ff37, //  FULLWIDTH LATIN CAPITAL LETTER W
                                       0x0ff38, //  FULLWIDTH LATIN CAPITAL LETTER X
                                       0x0ff39, //  FULLWIDTH LATIN CAPITAL LETTER Y
                                       0x0ff3a, //  FULLWIDTH LATIN CAPITAL LETTER Z
                                       0x10400, //  DESERET CAPITAL LETTER LONG I
                                       0x10401, //  DESERET CAPITAL LETTER LONG E
                                       0x10402, //  DESERET CAPITAL LETTER LONG A
                                       0x10403, //  DESERET CAPITAL LETTER LONG AH
                                       0x10404, //  DESERET CAPITAL LETTER LONG O
                                       0x10405, //  DESERET CAPITAL LETTER LONG OO
                                       0x10406, //  DESERET CAPITAL LETTER SHORT I
                                       0x10407, //  DESERET CAPITAL LETTER SHORT E
                                       0x10408, //  DESERET CAPITAL LETTER SHORT A
                                       0x10409, //  DESERET CAPITAL LETTER SHORT AH
                                       0x1040a, //  DESERET CAPITAL LETTER SHORT O
                                       0x1040b, //  DESERET CAPITAL LETTER SHORT OO
                                       0x1040c, //  DESERET CAPITAL LETTER AY
                                       0x1040d, //  DESERET CAPITAL LETTER OW
                                       0x1040e, //  DESERET CAPITAL LETTER WU
                                       0x1040f, //  DESERET CAPITAL LETTER YEE
                                       0x10410, //  DESERET CAPITAL LETTER H
                                       0x10411, //  DESERET CAPITAL LETTER PEE
                                       0x10412, //  DESERET CAPITAL LETTER BEE
                                       0x10413, //  DESERET CAPITAL LETTER TEE
                                       0x10414, //  DESERET CAPITAL LETTER DEE
                                       0x10415, //  DESERET CAPITAL LETTER CHEE
                                       0x10416, //  DESERET CAPITAL LETTER JEE
                                       0x10417, //  DESERET CAPITAL LETTER KAY
                                       0x10418, //  DESERET CAPITAL LETTER GAY
                                       0x10419, //  DESERET CAPITAL LETTER EF
                                       0x1041a, //  DESERET CAPITAL LETTER VEE
                                       0x1041b, //  DESERET CAPITAL LETTER ETH
                                       0x1041c, //  DESERET CAPITAL LETTER THEE
                                       0x1041d, //  DESERET CAPITAL LETTER ES
                                       0x1041e, //  DESERET CAPITAL LETTER ZEE
                                       0x1041f, //  DESERET CAPITAL LETTER ESH
                                       0x10420, //  DESERET CAPITAL LETTER ZHEE
                                       0x10421, //  DESERET CAPITAL LETTER ER
                                       0x10422, //  DESERET CAPITAL LETTER EL
                                       0x10423, //  DESERET CAPITAL LETTER EM
                                       0x10424, //  DESERET CAPITAL LETTER EN
                                       0x10425, //  DESERET CAPITAL LETTER ENG
                                       0x10426, //  DESERET CAPITAL LETTER OI
                                       0x10427, //  DESERET CAPITAL LETTER EW
                                       0x104b0, //  OSAGE CAPITAL LETTER A
                                       0x104b1, //  OSAGE CAPITAL LETTER AI
                                       0x104b2, //  OSAGE CAPITAL LETTER AIN
                                       0x104b3, //  OSAGE CAPITAL LETTER AH
                                       0x104b4, //  OSAGE CAPITAL LETTER BRA
                                       0x104b5, //  OSAGE CAPITAL LETTER CHA
                                       0x104b6, //  OSAGE CAPITAL LETTER EHCHA
                                       0x104b7, //  OSAGE CAPITAL LETTER E
                                       0x104b8, //  OSAGE CAPITAL LETTER EIN
                                       0x104b9, //  OSAGE CAPITAL LETTER HA
                                       0x104ba, //  OSAGE CAPITAL LETTER HYA
                                       0x104bb, //  OSAGE CAPITAL LETTER I
                                       0x104bc, //  OSAGE CAPITAL LETTER KA
                                       0x104bd, //  OSAGE CAPITAL LETTER EHKA
                                       0x104be, //  OSAGE CAPITAL LETTER KYA
                                       0x104bf, //  OSAGE CAPITAL LETTER LA
                                       0x104c0, //  OSAGE CAPITAL LETTER MA
                                       0x104c1, //  OSAGE CAPITAL LETTER NA
                                       0x104c2, //  OSAGE CAPITAL LETTER O
                                       0x104c3, //  OSAGE CAPITAL LETTER OIN
                                       0x104c4, //  OSAGE CAPITAL LETTER PA
                                       0x104c5, //  OSAGE CAPITAL LETTER EHPA
                                       0x104c6, //  OSAGE CAPITAL LETTER SA
                                       0x104c7, //  OSAGE CAPITAL LETTER SHA
                                       0x104c8, //  OSAGE CAPITAL LETTER TA
                                       0x104c9, //  OSAGE CAPITAL LETTER EHTA
                                       0x104ca, //  OSAGE CAPITAL LETTER TSA
                                       0x104cb, //  OSAGE CAPITAL LETTER EHTSA
                                       0x104cc, //  OSAGE CAPITAL LETTER TSHA
                                       0x104cd, //  OSAGE CAPITAL LETTER DHA
                                       0x104ce, //  OSAGE CAPITAL LETTER U
                                       0x104cf, //  OSAGE CAPITAL LETTER WA
                                       0x104d0, //  OSAGE CAPITAL LETTER KHA
                                       0x104d1, //  OSAGE CAPITAL LETTER GHA
                                       0x104d2, //  OSAGE CAPITAL LETTER ZA
                                       0x104d3, //  OSAGE CAPITAL LETTER ZHA
                                       0x10570, //  VITHKUQI CAPITAL LETTER A
                                       0x10571, //  VITHKUQI CAPITAL LETTER BBE
                                       0x10572, //  VITHKUQI CAPITAL LETTER BE
                                       0x10573, //  VITHKUQI CAPITAL LETTER CE
                                       0x10574, //  VITHKUQI CAPITAL LETTER CHE
                                       0x10575, //  VITHKUQI CAPITAL LETTER DE
                                       0x10576, //  VITHKUQI CAPITAL LETTER DHE
                                       0x10577, //  VITHKUQI CAPITAL LETTER EI
                                       0x10578, //  VITHKUQI CAPITAL LETTER E
                                       0x10579, //  VITHKUQI CAPITAL LETTER FE
                                       0x1057a, //  VITHKUQI CAPITAL LETTER GA
                                       0x1057c, //  VITHKUQI CAPITAL LETTER HA
                                       0x1057d, //  VITHKUQI CAPITAL LETTER HHA
                                       0x1057e, //  VITHKUQI CAPITAL LETTER I
                                       0x1057f, //  VITHKUQI CAPITAL LETTER IJE
                                       0x10580, //  VITHKUQI CAPITAL LETTER JE
                                       0x10581, //  VITHKUQI CAPITAL LETTER KA
                                       0x10582, //  VITHKUQI CAPITAL LETTER LA
                                       0x10583, //  VITHKUQI CAPITAL LETTER LLA
                                       0x10584, //  VITHKUQI CAPITAL LETTER ME
                                       0x10585, //  VITHKUQI CAPITAL LETTER NE
                                       0x10586, //  VITHKUQI CAPITAL LETTER NJE
                                       0x10587, //  VITHKUQI CAPITAL LETTER O
                                       0x10588, //  VITHKUQI CAPITAL LETTER PE
                                       0x10589, //  VITHKUQI CAPITAL LETTER QA
                                       0x1058a, //  VITHKUQI CAPITAL LETTER RE
                                       0x1058c, //  VITHKUQI CAPITAL LETTER SE
                                       0x1058d, //  VITHKUQI CAPITAL LETTER SHE
                                       0x1058e, //  VITHKUQI CAPITAL LETTER TE
                                       0x1058f, //  VITHKUQI CAPITAL LETTER THE
                                       0x10590, //  VITHKUQI CAPITAL LETTER U
                                       0x10591, //  VITHKUQI CAPITAL LETTER VE
                                       0x10592, //  VITHKUQI CAPITAL LETTER XE
                                       0x10594, //  VITHKUQI CAPITAL LETTER Y
                                       0x10595, //  VITHKUQI CAPITAL LETTER ZE
                                       0x10c80, //  OLD HUNGARIAN CAPITAL LETTER A
                                       0x10c81, //  OLD HUNGARIAN CAPITAL LETTER AA
                                       0x10c82, //  OLD HUNGARIAN CAPITAL LETTER EB
                                       0x10c83, //  OLD HUNGARIAN CAPITAL LETTER AMB
                                       0x10c84, //  OLD HUNGARIAN CAPITAL LETTER EC
                                       0x10c85, //  OLD HUNGARIAN CAPITAL LETTER ENC
                                       0x10c86, //  OLD HUNGARIAN CAPITAL LETTER ECS
                                       0x10c87, //  OLD HUNGARIAN CAPITAL LETTER ED
                                       0x10c88, //  OLD HUNGARIAN CAPITAL LETTER AND
                                       0x10c89, //  OLD HUNGARIAN CAPITAL LETTER E
                                       0x10c8a, //  OLD HUNGARIAN CAPITAL LETTER CLOSE E
                                       0x10c8b, //  OLD HUNGARIAN CAPITAL LETTER EE
                                       0x10c8c, //  OLD HUNGARIAN CAPITAL LETTER EF
                                       0x10c8d, //  OLD HUNGARIAN CAPITAL LETTER EG
                                       0x10c8e, //  OLD HUNGARIAN CAPITAL LETTER EGY
                                       0x10c8f, //  OLD HUNGARIAN CAPITAL LETTER EH
                                       0x10c90, //  OLD HUNGARIAN CAPITAL LETTER I
                                       0x10c91, //  OLD HUNGARIAN CAPITAL LETTER II
                                       0x10c92, //  OLD HUNGARIAN CAPITAL LETTER EJ
                                       0x10c93, //  OLD HUNGARIAN CAPITAL LETTER EK
                                       0x10c94, //  OLD HUNGARIAN CAPITAL LETTER AK
                                       0x10c95, //  OLD HUNGARIAN CAPITAL LETTER UNK
                                       0x10c96, //  OLD HUNGARIAN CAPITAL LETTER EL
                                       0x10c97, //  OLD HUNGARIAN CAPITAL LETTER ELY
                                       0x10c98, //  OLD HUNGARIAN CAPITAL LETTER EM
                                       0x10c99, //  OLD HUNGARIAN CAPITAL LETTER EN
                                       0x10c9a, //  OLD HUNGARIAN CAPITAL LETTER ENY
                                       0x10c9b, //  OLD HUNGARIAN CAPITAL LETTER O
                                       0x10c9c, //  OLD HUNGARIAN CAPITAL LETTER OO
                                       0x10c9d, //  OLD HUNGARIAN CAPITAL LETTER NIKOLSBURG OE
                                       0x10c9e, //  OLD HUNGARIAN CAPITAL LETTER RUDIMENTA OE
                                       0x10c9f, //  OLD HUNGARIAN CAPITAL LETTER OEE
                                       0x10ca0, //  OLD HUNGARIAN CAPITAL LETTER EP
                                       0x10ca1, //  OLD HUNGARIAN CAPITAL LETTER EMP
                                       0x10ca2, //  OLD HUNGARIAN CAPITAL LETTER ER
                                       0x10ca3, //  OLD HUNGARIAN CAPITAL LETTER SHORT ER
                                       0x10ca4, //  OLD HUNGARIAN CAPITAL LETTER ES
                                       0x10ca5, //  OLD HUNGARIAN CAPITAL LETTER ESZ
                                       0x10ca6, //  OLD HUNGARIAN CAPITAL LETTER ET
                                       0x10ca7, //  OLD HUNGARIAN CAPITAL LETTER ENT
                                       0x10ca8, //  OLD HUNGARIAN CAPITAL LETTER ETY
                                       0x10ca9, //  OLD HUNGARIAN CAPITAL LETTER ECH
                                       0x10caa, //  OLD HUNGARIAN CAPITAL LETTER U
                                       0x10cab, //  OLD HUNGARIAN CAPITAL LETTER UU
                                       0x10cac, //  OLD HUNGARIAN CAPITAL LETTER NIKOLSBURG UE
                                       0x10cad, //  OLD HUNGARIAN CAPITAL LETTER RUDIMENTA UE
                                       0x10cae, //  OLD HUNGARIAN CAPITAL LETTER EV
                                       0x10caf, //  OLD HUNGARIAN CAPITAL LETTER EZ
                                       0x10cb0, //  OLD HUNGARIAN CAPITAL LETTER EZS
                                       0x10cb1, //  OLD HUNGARIAN CAPITAL LETTER ENT-SHAPED SIGN
                                       0x10cb2, //  OLD HUNGARIAN CAPITAL LETTER US
                                       0x118a0, //  WARANG CITI CAPITAL LETTER NGAA
                                       0x118a1, //  WARANG CITI CAPITAL LETTER A
                                       0x118a2, //  WARANG CITI CAPITAL LETTER WI
                                       0x118a3, //  WARANG CITI CAPITAL LETTER YU
                                       0x118a4, //  WARANG CITI CAPITAL LETTER YA
                                       0x118a5, //  WARANG CITI CAPITAL LETTER YO
                                       0x118a6, //  WARANG CITI CAPITAL LETTER II
                                       0x118a7, //  WARANG CITI CAPITAL LETTER UU
                                       0x118a8, //  WARANG CITI CAPITAL LETTER E
                                       0x118a9, //  WARANG CITI CAPITAL LETTER O
                                       0x118aa, //  WARANG CITI CAPITAL LETTER ANG
                                       0x118ab, //  WARANG CITI CAPITAL LETTER GA
                                       0x118ac, //  WARANG CITI CAPITAL LETTER KO
                                       0x118ad, //  WARANG CITI CAPITAL LETTER ENY
                                       0x118ae, //  WARANG CITI CAPITAL LETTER YUJ
                                       0x118af, //  WARANG CITI CAPITAL LETTER UC
                                       0x118b0, //  WARANG CITI CAPITAL LETTER ENN
                                       0x118b1, //  WARANG CITI CAPITAL LETTER ODD
                                       0x118b2, //  WARANG CITI CAPITAL LETTER TTE
                                       0x118b3, //  WARANG CITI CAPITAL LETTER NUNG
                                       0x118b4, //  WARANG CITI CAPITAL LETTER DA
                                       0x118b5, //  WARANG CITI CAPITAL LETTER AT
                                       0x118b6, //  WARANG CITI CAPITAL LETTER AM
                                       0x118b7, //  WARANG CITI CAPITAL LETTER BU
                                       0x118b8, //  WARANG CITI CAPITAL LETTER PU
                                       0x118b9, //  WARANG CITI CAPITAL LETTER HIYO
                                       0x118ba, //  WARANG CITI CAPITAL LETTER HOLO
                                       0x118bb, //  WARANG CITI CAPITAL LETTER HORR
                                       0x118bc, //  WARANG CITI CAPITAL LETTER HAR
                                       0x118bd, //  WARANG CITI CAPITAL LETTER SSUU
                                       0x118be, //  WARANG CITI CAPITAL LETTER SII
                                       0x118bf, //  WARANG CITI CAPITAL LETTER VIYO
                                       0x16e40, //  MEDEFAIDRIN CAPITAL LETTER M
                                       0x16e41, //  MEDEFAIDRIN CAPITAL LETTER S
                                       0x16e42, //  MEDEFAIDRIN CAPITAL LETTER V
                                       0x16e43, //  MEDEFAIDRIN CAPITAL LETTER W
                                       0x16e44, //  MEDEFAIDRIN CAPITAL LETTER ATIU
                                       0x16e45, //  MEDEFAIDRIN CAPITAL LETTER Z
                                       0x16e46, //  MEDEFAIDRIN CAPITAL LETTER KP
                                       0x16e47, //  MEDEFAIDRIN CAPITAL LETTER P
                                       0x16e48, //  MEDEFAIDRIN CAPITAL LETTER T
                                       0x16e49, //  MEDEFAIDRIN CAPITAL LETTER G
                                       0x16e4a, //  MEDEFAIDRIN CAPITAL LETTER F
                                       0x16e4b, //  MEDEFAIDRIN CAPITAL LETTER I
                                       0x16e4c, //  MEDEFAIDRIN CAPITAL LETTER K
                                       0x16e4d, //  MEDEFAIDRIN CAPITAL LETTER A
                                       0x16e4e, //  MEDEFAIDRIN CAPITAL LETTER J
                                       0x16e4f, //  MEDEFAIDRIN CAPITAL LETTER E
                                       0x16e50, //  MEDEFAIDRIN CAPITAL LETTER B
                                       0x16e51, //  MEDEFAIDRIN CAPITAL LETTER C
                                       0x16e52, //  MEDEFAIDRIN CAPITAL LETTER U
                                       0x16e53, //  MEDEFAIDRIN CAPITAL LETTER YU
                                       0x16e54, //  MEDEFAIDRIN CAPITAL LETTER L
                                       0x16e55, //  MEDEFAIDRIN CAPITAL LETTER Q
                                       0x16e56, //  MEDEFAIDRIN CAPITAL LETTER HP
                                       0x16e57, //  MEDEFAIDRIN CAPITAL LETTER NY
                                       0x16e58, //  MEDEFAIDRIN CAPITAL LETTER X
                                       0x16e59, //  MEDEFAIDRIN CAPITAL LETTER D
                                       0x16e5a, //  MEDEFAIDRIN CAPITAL LETTER OE
                                       0x16e5b, //  MEDEFAIDRIN CAPITAL LETTER N
                                       0x16e5c, //  MEDEFAIDRIN CAPITAL LETTER R
                                       0x16e5d, //  MEDEFAIDRIN CAPITAL LETTER O
                                       0x16e5e, //  MEDEFAIDRIN CAPITAL LETTER AI
                                       0x16e5f, //  MEDEFAIDRIN CAPITAL LETTER Y
                                       0x1e900, //  ADLAM CAPITAL LETTER ALIF
                                       0x1e901, //  ADLAM CAPITAL LETTER DAALI
                                       0x1e902, //  ADLAM CAPITAL LETTER LAAM
                                       0x1e903, //  ADLAM CAPITAL LETTER MIIM
                                       0x1e904, //  ADLAM CAPITAL LETTER BA
                                       0x1e905, //  ADLAM CAPITAL LETTER SINNYIIYHE
                                       0x1e906, //  ADLAM CAPITAL LETTER PE
                                       0x1e907, //  ADLAM CAPITAL LETTER BHE
                                       0x1e908, //  ADLAM CAPITAL LETTER RA
                                       0x1e909, //  ADLAM CAPITAL LETTER E
                                       0x1e90a, //  ADLAM CAPITAL LETTER FA
                                       0x1e90b, //  ADLAM CAPITAL LETTER I
                                       0x1e90c, //  ADLAM CAPITAL LETTER O
                                       0x1e90d, //  ADLAM CAPITAL LETTER DHA
                                       0x1e90e, //  ADLAM CAPITAL LETTER YHE
                                       0x1e90f, //  ADLAM CAPITAL LETTER WAW
                                       0x1e910, //  ADLAM CAPITAL LETTER NUN
                                       0x1e911, //  ADLAM CAPITAL LETTER KAF
                                       0x1e912, //  ADLAM CAPITAL LETTER YA
                                       0x1e913, //  ADLAM CAPITAL LETTER U
                                       0x1e914, //  ADLAM CAPITAL LETTER JIIM
                                       0x1e915, //  ADLAM CAPITAL LETTER CHI
                                       0x1e916, //  ADLAM CAPITAL LETTER HA
                                       0x1e917, //  ADLAM CAPITAL LETTER QAAF
                                       0x1e918, //  ADLAM CAPITAL LETTER GA
                                       0x1e919, //  ADLAM CAPITAL LETTER NYA
                                       0x1e91a, //  ADLAM CAPITAL LETTER TU
                                       0x1e91b, //  ADLAM CAPITAL LETTER NHA
                                       0x1e91c, //  ADLAM CAPITAL LETTER VA
                                       0x1e91d, //  ADLAM CAPITAL LETTER KHA
                                       0x1e91e, //  ADLAM CAPITAL LETTER GBE
                                       0x1e91f, //  ADLAM CAPITAL LETTER ZAL
                                       0x1e920, //  ADLAM CAPITAL LETTER KPO
                                       0x1e921 }; //  ADLAM CAPITAL LETTER SHA


/// Return `true` if character is a lowercase character
/// \param p_char pointer to character to check
[[nodiscard]] auto islower(const char* p_char) -> bool {
    // trivial case - default to standard function
    if (static_cast<unsigned char>(*p_char) <= 0x7f) {
        return ::islower(*p_char) != 0;
    }

    // search character in lowercase table
    char32_t r = rune(p_char);
    const char32_t* f = std::lower_bound(std::begin(l2u), std::end(l2u), r);
    return (f != std::end(l2u) && *f == r);
}

/*!
  Convert UTF-8 string to lower case.

  \param str UTF-8 string to convert to lowercase.
  \return lower case UTF-8 string

  Uses case folding table published by Unicode Consortium
  (https://www.unicode.org/Public/UCD/latest/ucd/CaseFolding.txt)
*/

[[nodiscard]] auto tolower(std::string const& str) -> std::string {
    std::u32string wstr;
    auto ptr = str.begin();
    while (ptr < str.end()) {
        char32_t c = next(ptr, str.end());
        const char32_t* f = std::lower_bound(std::begin(u2l), std::end(u2l), c);
        if (f != std::end(u2l) && *f == c) {
            wstr.push_back(lc[f - u2l]);
        }
        else {
            wstr.push_back(c);
        }
    }
    return narrow(wstr);
}

/*!
  In place version converts a UTF-8 encoded string to lowercase
  \param str  UTF-8 encoded string to be converted

  Note that, in general, the size of the resulting string will be different from
  that of the original string.
*/
void make_lower(std::string& str) {
    str = tolower(str);
}

/// Return `true` if character is an uppercase character
/// \param p_char pointer to character to check
[[nodiscard]] auto isupper(const char* p_char) -> bool {
    // trivial case - default to standard function
    if (static_cast<unsigned char>(*p_char) <= 0x7f) {
        return ::isupper(*p_char) != 0;
    }

    // search character in uppercase table
    char32_t r = rune(p_char);
    const char32_t* f = std::lower_bound(std::begin(u2l), std::end(u2l), r);
    return (f != std::end(u2l) && *f == r);
}

/*!
  Convert a UTF-8 string to upper case.

  \param str UTF-8 string to convert to uppercase.
  \return upper case UTF-8 string

  Uses case folding table published by Unicode Consortium
  (http://www.unicode.org/Public/UCD/latest/ucd/CaseFolding.txt)
*/
[[nodiscard]] auto toupper(std::string const& str) -> std::string {
    std::u32string wstr;
    auto ptr = str.begin();
    while (ptr < str.end()) {
        char32_t c = next(ptr, str.end());
        const char32_t* f = std::lower_bound(std::begin(l2u), std::end(l2u), c);
        if (f != std::end(l2u) && *f == c) {
            wstr.push_back(uc[f - l2u]);
        }
        else {
            wstr.push_back(c);
        }
    }
    return narrow(wstr);
}

/*!
  In place version converts a UTF-8 encoded string to lowercase.
  \param str  string to be converted

  Note that, in general, the size of the resulting string will be different from
  that of the original string.
*/
void make_upper(std::string& str) {
    str = toupper(str);
}

/*!
  Compare two strings in a case-insensitive way.

  \param str1 first string
  \param str2 second string
  \return <0 if first string is lexicographically before the second one
  \return >0 if first string is lexicographically after the second string
  \return =0 if the two strings are equal

  Strings must be valid UTF-8 strings.
*/
[[nodiscard]] auto icompare(std::string const& str1, std::string const& str2) -> int32_t {
    assert(valid_str(str1) && valid_str(str2));

    auto p1 = begin(str1);
    auto p2 = begin(str2);
    while (p1 < end(str1) && p2 < end(str2)) {
        char32_t lc1;
        char32_t lc2;
        char32_t c1 = next(p1, end(str1));
        char32_t c2 = next(p2, end(str2));

        const char32_t* f = std::lower_bound(std::begin(u2l), std::end(u2l), c1);
        if (f != std::end(u2l) && *f == c1) {
            lc1 = lc[f - u2l];
        }
        else {
            lc1 = c1;
        }
        f = std::lower_bound(std::begin(u2l), std::end(u2l), c2);
        if (f != std::end(u2l) && *f == c2) {
            lc2 = lc[f - u2l];
        }
        else {
            lc2 = c2;
        }
        if ((lc1 != lc2)) {
            return (lc1 < lc2) ? -1 : 1;
        }
    }
    if (p1 != end(str1)) {
        return 1;
    }
    if (p2 != end(str2)) {
        return -1;
    }
    return 0;
}


} // namespace utf8


//
//
////   end impementanion file   /////////////////////////////////////////////////////
#endif // UTF8_IMPLEMENTATION
