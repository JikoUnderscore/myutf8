///! Write test with `TEST_NOWIN` not defined
///!

#include <cassert>
#include <print>


// #define TEST_NOWIN


#ifdef TEST_NOWIN
#define UTF8_IMPLEMENTATION
#include "./utf8/utf8.nowin.hpp"
// this is only needed for tests
using LPVOID = void*;
using BOOL = int;
typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;
using LPDWORD = DWORD*;
using ULONG_PTR = unsigned long long;
using PVOID = void*;
typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct {
            DWORD Offset;
            DWORD OffsetHigh;
        } DUMMYSTRUCTNAME;
        PVOID Pointer;
    } DUMMYUNIONNAME;
    HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;


#define GENERIC_READ (0x80000000L)
#define GENERIC_WRITE (0x40000000L)
#define GENERIC_EXECUTE (0x20000000L)
#define GENERIC_ALL (0x10000000L)

#define CREATE_NEW 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define TRUNCATE_EXISTING 5

#define FILE_ATTRIBUTE_NORMAL 0x00000080


extern "C" __declspec(dllimport) HANDLE __stdcall CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                                              DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
#define CreateFile CreateFileW
extern "C" __declspec(dllimport) BOOL __stdcall ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
extern "C" __declspec(dllimport) BOOL __stdcall CloseHandle(HANDLE hObject);
#else
#define UTF8_IMPLEMENTATION
#include "./utf8/utf8.win.hpp"
#endif


/// PASS in vars, not rvalues function calls
#define ASSERT(bul, msg)                                                                                                                                                           \
    if ((bul))                                                                                                                                                                     \
        printf("%s... ok\n", msg);                                                                                                                                                 \
    assert(msg && (bul))
/// PASS in vars, not rvalues function calls
#define ASSERT2(bul)                                                                                                                                                               \
    if ((bul))                                                                                                                                                                     \
        printf("... ok\n");                                                                                                                                                        \
    assert((bul))

/// PASS in vars, not rvalues function calls
#define ASSERT_EQ(left, right, msg)                                                                                                                                                \
    if ((left) == (right))                                                                                                                                                         \
        printf("%s... ok\n", msg);                                                                                                                                                 \
    assert(msg && (left) == (right))
/// PASS in vars, not rvalues function calls
#define ASSERT_EQ2(left, right)                                                                                                                                                    \
    if ((left) == (right))                                                                                                                                                         \
        printf("... ok\n");                                                                                                                                                        \
    assert((left) == (right))


int main() {
    { // narrow_with_null
        auto w1{ std::wstring(L"ABC") };
        w1 += L'\0';
        w1 += L"DEF";
        auto n1{ utf8::narrow(w1) };
        size_t szw{ w1.size() };
        size_t szn{ std::size(n1) };

        ASSERT_EQ(szw, szn, "narrow_with_null");
    }

    { // widen_with_null
        auto n1{ std::string("ABC") };
        n1 += '\0';
        n1 += "DEF";
        auto w1{ utf8::widen(n1) };
        size_t szw{ w1.size() };
        size_t szn{ std::size(n1) };

        ASSERT_EQ(szn, szw, "widen_with_null");
    }

    { // widen_string
        auto s1{ std::string("ABCD") };
        auto l1{ std::wstring(L"ABCD") };

        auto l2{ utf8::widen(s1) };

        ASSERT_EQ(l1, l2, "widen_string");
    }

    { // widen_ptr
        const char* s1 = "ABCD";
        auto l1{ std::wstring(L"ABCD") };

        auto l2{ utf8::widen(s1) };

        ASSERT_EQ(l1, l2, "widen_ptr");
    }


    { // widen_count
        const char* s1 = "ABCDEFGH";
        auto l1{ std::wstring(L"ABCD") };

        auto l2{ utf8::widen(s1, 4) };

        ASSERT_EQ(l1, l2, "widen_count");
    }


    { // narrow_string
        auto l1{ std::wstring(L"ABCD") };
        auto s1{ utf8::narrow(l1) };

        ASSERT_EQ("ABCD", s1, "narrow_string");
    }


    { // narrow_ptr
        const wchar_t* l1 = L"ABCD";
        std::string s1 = utf8::narrow(l1);

        ASSERT_EQ("ABCD", s1, "narrow_ptr");
    }


    {
        const wchar_t* l1 = L"ABCDEFGH";
        std::string s1 = utf8::narrow(l1, 4);

        ASSERT_EQ("ABCD", s1, "narrow_count");
    }


    // { // narrow_char32
    //     const char32_t* u1 = U"A😀BCDEFGH";
    //     std::string s1 = utf8::narrow(u1, 4);
    //     ASSERT(s1.compare(0, 4, reinterpret_cast<const char*>(u1), 4) == 0  , "narrow_char32");
    // }


    { // widen_narrow
        const char* ptr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        auto s = utf8::narrow(utf8::widen(ptr));
        ASSERT(strcmp(s.c_str(), ptr) == 0, "widen_narrow");
    }


    { // surrogates
        // example from RFC8259
        const wchar_t* gclef{ L"\xd834\xdd1e" };
        const char32_t rune_gclef = 0x1d11e;
        std::string u8_1, u8_2;
        u8_1 = utf8::narrow(gclef);
        u8_2 = utf8::narrow(&rune_gclef, 1);
        ASSERT_EQ(u8_1, u8_2, "surrogates");
    }


    { // greek_letters
        const wchar_t* greek = L"ελληνικό αλφάβητο";
        std::string s = utf8::narrow(greek);
        ASSERT_EQ(utf8::widen(s), greek, "greek_letters");
    }


    { // string_len
        const wchar_t* greek = L"ελληνικό αλφάβητο";
        std::string s = utf8::narrow(greek);
        size_t l = utf8::length(s);
        ASSERT_EQ(wcslen(greek), l, "string_len");
    }


    { // wemoji
        const wchar_t* wsmiley = L"😄";
        size_t wlen = std::wcslen(wsmiley);
        ASSERT_EQ(2, wlen, "wemoji 1");
        std::string smiley = utf8::narrow(wsmiley);
        ASSERT_EQ("\xF0\x9f\x98\x84", smiley, "wemoji 2");
    }


    { // rune
        std::string smiley{ "😀" };
        char32_t rune_smiley = utf8::rune(smiley.begin());
        ASSERT_EQ(0x1f600, rune_smiley, "rune");
    }


    { // rune2
        const char* smiley{ "😀" };
        char32_t rune_smiley = utf8::rune(smiley);
        ASSERT_EQ((int)U'😀', (int)rune_smiley, "rune2");
    }


    // check that next function advances with one code point

    { // next
        std::string emojis{ "😃😎😛" };
        int i = 0;
        auto ptr = emojis.begin();
        while (utf8::next(ptr, emojis.end()) != utf8::REPLACEMENT_CHARACTER) {
            i++;
        }

        ASSERT_EQ(3, i, "next");
    }

    // same test but using a character pointer instead of a string iterator

    { // next_ptr
        std::string emojis{ "😃😎😛" };
        int i = 0;
        const char* ptr = emojis.c_str();
        while (utf8::next(ptr)) {
            i++;
        }

        ASSERT_EQ(3, i, "next_ptr");
    }


    { // next_non_const
        char emojis[20];
        strcpy(emojis, "😃😎😛");
        int i = 0;
        char* ptr = emojis;
        while (utf8::next(ptr)) {
            i++;
        }

        ASSERT_EQ(*ptr, 0, "next_non_const 1");
        ASSERT_EQ(3, i, "next_non_const 2");
    }


    { // valid_str_funcs
        std::string emojis{ "😃😎😛" };
        ASSERT(utf8::valid_str(emojis), "valid_str_funcs");
        ASSERT(utf8::valid_str(emojis.c_str()), "valid_str_funcs");
        ASSERT(utf8::valid_str(emojis.c_str(), emojis.length()), "valid_str_funcs");

        ASSERT(!utf8::valid_str(emojis.c_str(), emojis.length() - 1), "valid_str_funcs");
        emojis[8] = 0;
        ASSERT(utf8::valid_str(emojis.c_str()), "valid_str_funcs");
        emojis[6] = 0;
        ASSERT(!utf8::valid_str(emojis.c_str(), emojis.length()), "valid_str_funcs");
    }


    { // is_valid_yes
        std::string s1 = "a";
        std::string s2 = "°";
        std::string s3 = "€";
        std::string s4 = "😃";

        ASSERT(utf8::is_valid(s1.c_str()), "is_valid_yes");
        ASSERT(utf8::is_valid(s2.c_str()), "is_valid_yes");
        ASSERT(utf8::is_valid(s3.c_str()), "is_valid_yes");
        ASSERT(utf8::is_valid(s4.c_str()), "is_valid_yes");

        // same tests with string iterators
        ASSERT(utf8::is_valid(s1.begin(), s1.end()), "is_valid_yes");
        ASSERT(utf8::is_valid(s2.begin(), s2.end()), "is_valid_yes");
        ASSERT(utf8::is_valid(s3.begin(), s3.end()), "is_valid_yes");
        ASSERT(utf8::is_valid(s4.begin(), s4.end()), "is_valid_yes");

        ASSERT(utf8::is_valid("\xEF\xBB\xBF"), "is_valid_yes"); // BOM
        ASSERT(utf8::is_valid(""), "is_valid_yes"); // empty string
    }


    { // is_valid_no
        ASSERT(!utf8::is_valid("\xC1\xA1"), "overlong 'a'"); // overlong 'a'
        ASSERT(!utf8::is_valid("\xE0\x82\xB0"), "overlong '°'"); // overlong '°'
        ASSERT(!utf8::is_valid("\xF0\x82\x82\xAC"), "overlong '€'"); // overlong '€'
        ASSERT(!utf8::is_valid("\xFE\xFF"), "UTF-16 BOM BE"); // UTF-16 BOM BE
        ASSERT(!utf8::is_valid("\xFF\xFE"), "UTF-16 BOM LE"); // UTF-16 BOM LE
        ASSERT(!utf8::is_valid("\xED\xA0\x80"), "0xD800 surrogate code point"); // 0xD800 surrogate code point
    }


    // { // prev_ptr
    //   const char* emojis{ u8"😃😎😛" };
    //   auto ptr = emojis + strlen(emojis);

    //   int count = 0;
    //   while (ptr > emojis)
    //   {
    //     prev (ptr);
    //     count++;
    //   }
    //   CHECK_EQUAL (3, count);
    // }


    // { // prev_string
    //   const std::u8string emojis{ u8"😃😎😛" };
    //   std::u32string runes;
    //   auto ptr = emojis.end ();
    //   int count = 0;
    //   while (ptr > emojis.begin())
    //   {
    //     runes.insert (runes.begin(), utf8::prev (ptr, emojis.begin()));
    //     count++;
    //   }
    //   ASSERT_EQ (3, count, "prev_string");
    //   ASSERT_EQ (emojis, utf8::narrow (runes), "prev_string");
    // }


    { // prev_invalid
        const char* invalid_1 = "\xC1\xA1"; // overlong 'a'
        const char* invalid_2 = "\xE0\x82\xB0"; // overlong '°'
        const char* invalid_3 = "\xF0\x82\x82\xAC"; // overlong '€'
        const char* invalid_4 = "\xFE\xFF"; // UTF-16 BOM BE
        const char* invalid_5 = "\xFF\xFE"; // UTF-16 BOM LE
        const char* invalid_6 = "\xED\xA0\x80"; // 0xD800 surrogate code point

        const char* ptr = invalid_1 + strlen(invalid_1);
        ASSERT_EQ(utf8::REPLACEMENT_CHARACTER, utf8::prev(ptr), "overlong 'a'");
        ptr = invalid_2 + strlen(invalid_2);
        ASSERT_EQ(utf8::REPLACEMENT_CHARACTER, utf8::prev(ptr), "overlong '°'");
        ptr = invalid_3 + strlen(invalid_3);
        ASSERT_EQ(utf8::REPLACEMENT_CHARACTER, utf8::prev(ptr), "overlong '€'");
        ptr = invalid_4 + strlen(invalid_4);
        ASSERT_EQ(utf8::REPLACEMENT_CHARACTER, utf8::prev(ptr), "UTF-16 BOM BE");
        ptr = invalid_5 + strlen(invalid_5);
        ASSERT_EQ(utf8::REPLACEMENT_CHARACTER, utf8::prev(ptr), "UTF-16 BOM LE");
        ptr = invalid_6 + strlen(invalid_6);
        ASSERT_EQ(utf8::REPLACEMENT_CHARACTER, utf8::prev(ptr), "0xD800 surrogate code point");
    }


    { // invalid_utf8
        const char invalid[]{ "\xFE\xFF\xFF\xFE" }; // UTF-16 BOM markers
        bool thrown = false;

        try {
            auto s = utf8::runes(invalid);
        }
        catch (utf8::exception& e) {
            ASSERT_EQ(utf8::exception::invalid_utf8, e.cause, "invalid_utf8 exception");
            thrown = true;
            printf("Exception caught: %s\n", e.what());
        }
        ASSERT(thrown, "invalid_utf8");
    }


    { // throw_invalid_char32
        bool thrown = false;
        try {
            utf8::narrow(0xd800);
        }
        catch (utf8::exception& e) {
            ASSERT_EQ(utf8::exception::invalid_char32, e.cause, "throw_invalid_char32 exception");
            thrown = true;
            printf("Exception caught: %s\n", e.what());
        }
        ASSERT(thrown, "throw_invalid_char32");
    }

    // test for runes function (conversion from UTF8 to UTF32)

    { // runes
        std::string emojis{ "😃😎😛" };
        std::u32string emojis32 = utf8::runes(emojis);
        ASSERT_EQ(3, emojis32.size(), "runes");
        ASSERT_EQ(0x1f603, (int)emojis32[0], "runes");
    }


    { // dir
        /* Make a folder using Greek alphabet, change current directory into it,
        obtain the current working directory and verify that it matches the name
        of the newly created folder */

        std::string dirname = "ελληνικό";
        auto a = utf8::mkdir(dirname);

        ASSERT(a, "dir"); // mkdir returns true  for success

        // enter newly created directory
        auto b = utf8::chdir(dirname);
        ASSERT(b, "dir"); // chdir returns true for success

        // Path returned by getcwd should end in our Greek string
        std::string cwd = utf8::getcwd();
        size_t idx = cwd.rfind("\\"); // last backslash
        std::string last = cwd.substr(idx + 1);
        ASSERT_EQ(dirname, last, "dir");

        // Move out of directory and remove it
        utf8::chdir("..");
        auto c = utf8::rmdir(dirname);
        ASSERT(c, "dir"); // rmdir returrs true for success
    }


    // { // symlink
    //   /* Make a folder using Greek alphabet, and another one using Armenian.
    //   enter the 2nd directory and create a link to the first. It verifies a file
    //   created in the first directory is visible through the symlink.*/

    //   //make first directory
    //   ASSERT (utf8::mkdir ("ελληνικό") ,"symlink");
    //   utf8::chdir ("ελληνικό");
    //   //and a file in it
    //   utf8::ofstream file_out ("f.txt");
    //   file_out << "text" << std::endl;
    //   file_out.close ();
    //   utf8::chdir ("..");
    //   //make 2nd directory
    //   ASSERT (utf8::mkdir ("Հայերեն"),"symlink");
    //   utf8::chdir ("Հայերեն");
    //   //and symlink to first
    //   ASSERT (utf8::symlink (u8"..\\ελληνικό", "पंजाबी", true),"symlink");
    //   //change into symlink
    //   utf8::chdir ("पंजाबी");
    //   utf8::ifstream in ("f.txt");
    //   std::string str;
    //   in >> str;
    //   ASSERT_EQ ("text", str,"symlink");
    //   in.close ();

    //   //cleanup
    //   utf8::remove ("f.txt");
    //   utf8::chdir ("..");
    //   utf8::rmdir ("पंजाबी");
    //   utf8::chdir ("..");
    //   utf8::rmdir ("Հայերեն");
    //   utf8::rmdir ("ελληνικό");
    // }


    { // out_stream
        /* Write some text in a file with a UTF8 encoded filename. Verifies using
        standard Windows file reading that content was written. */

        std::string filename = "ελληνικό";
        std::string filetext{ "😃😎😛" };

        utf8::ofstream file_u8strm(filename);

        file_u8strm << filetext << std::endl;
        file_u8strm.close();

        HANDLE f = CreateFile(utf8::widen(filename).c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        ASSERT2(f);

        char read_back[80];
        memset(read_back, 0, sizeof(read_back));

        size_t len = filetext.size();
        DWORD nr;
        ReadFile(f, read_back, (DWORD)len, &nr, NULL);
        CloseHandle(f);

        auto a = utf8::remove(filename);
        ASSERT(a, "out_stream");
        ASSERT_EQ(len, nr, "out_stream");
        ASSERT_EQ(filetext, read_back, "out_stream");
    }


    { // in_stream
        /* write some stuff in file using utf8::ofstream object and read it
        back using utf8::ifstream. Verify read back matches original.*/

        std::string filetext{ "ελληνικό" };
        std::string filename{ "😃😎😛" };

        utf8::ofstream u8out(filename);

        u8out << filetext << std::endl;
        u8out.close();

        utf8::ifstream u8in(filename);

        char read_back[80];
        memset(read_back, 0, sizeof(read_back));
        u8in.getline(read_back, sizeof(read_back));

        ASSERT_EQ(filetext, read_back, "in_stream");

        u8in.close();
        auto a = utf8::remove(filename);
        ASSERT(a, "in_stream");
    }


    { // fopen_write
        /* Write some text in a file with a UTF8 encoded filename. Verifies using
        standard Windows file reading that content was written. */

        std::string filename = "ελληνικό";
        std::string filetext{ "😃😎😛" };
        FILE* u8file = utf8::fopen(filename, "w");
        ASSERT(u8file, "Failed to create output file");

        fwrite(filetext.c_str(), sizeof(char), filetext.length(), u8file);
        fclose(u8file);

        HANDLE f = CreateFile(utf8::widen(filename).c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        ASSERT(f, "fopen_write");

        char read_back[80];
        memset(read_back, 0, sizeof(read_back));

        size_t len = filetext.size();
        DWORD nr;
        ReadFile(f, read_back, (DWORD)len, &nr, NULL);
        CloseHandle(f);
        auto a = utf8::remove(filename);
        ASSERT(a, "fopen_write");
        ASSERT_EQ(len, nr, "fopen_write");
        ASSERT_EQ(filetext, read_back, "fopen_write");
    }


    { // full_path
        const char* fname = "file.txt";
        FILE* f = ::fopen(fname, "w");
        ::fclose(f);

        char full[_MAX_PATH];
        _fullpath(full, fname, sizeof(full));
        ASSERT_EQ(full, utf8::fullpath(fname), "full_path");
        ::remove(fname);
    }


    { // make_splitpath
        const std::string dir{ "ελληνικό αλφάβητο" }, fname{ "😃😎😛" };
        std::string path;
        ASSERT(utf8::makepath(path, "C", dir, fname, ".txt"), "make_splitpath");
        std::wstring wpath = utf8::widen(path);
        ASSERT_EQ("C:ελληνικό αλφάβητο\\😃😎😛.txt", path, "make_splitpath");
        std::string drv1, dir1, fname1, ext1;
        ASSERT(utf8::splitpath(path, drv1, dir1, fname1, ext1), "make_splitpath");

        ASSERT_EQ("C:", drv1, "make_splitpath");
        ASSERT_EQ(dir + "\\", dir1, "make_splitpath");
        ASSERT_EQ(fname, fname1, "make_splitpath");
        ASSERT_EQ(".txt", ext1, "make_splitpath");
    }


    { // get_putenv
        // a long variable
        std::string path = utf8::getenv("PATH");
        ASSERT(!path.empty(), "get_putenv");

        utf8::putenv("ελληνικό=😃😎😛");
        ASSERT_EQ("😃😎😛", utf8::getenv("ελληνικό"), "get_putenv");

        utf8::putenv("ελληνικό", std::string());
        ASSERT(utf8::getenv("ελληνικό").empty(), "get_putenv");
    }

    // TEST (msgbox)
    // {
    // #if 0
    //   //requires user's intervention
    //   utf8::MessageBox (NULL, "ελληνικό", "😃😎😛", MB_ICONINFORMATION);
    // #endif
    // }


    // { // buffer_test
    //   utf8::buffer buf (_MAX_PATH);

    //   // You can set a buffer to a an initial value through the string assignment
    //   // operator
    //   std::string tmp{ "Some initial value" };
    //   buf = tmp;

    //   ASSERT_EQ (tmp, (std::string)buf ,"buffer_test");
    //   // size doesn't shrink when assigning a string
    //   ASSERT_EQ (_MAX_PATH, buf.size (),"buffer_test");

    //   //Copy ctor
    //   utf8::buffer buf1(buf);
    //   ASSERT_EQ (tmp, (string)buf1,"buffer_test");
    //   ASSERT_EQ (_MAX_PATH, buf1.size (),"buffer_test");

    //   //Principal assignment operator
    //   utf8::buffer buf2 (50);
    //   buf2 = buf1;
    //   ASSERT_EQ (tmp, (string)buf2,"buffer_test");
    //   //After buffer assignment, size matches the right-hand side size
    //   ASSERT_EQ (_MAX_PATH, buf2.size (),"buffer_test");
    // }


    // { // Temp_FileName
    //   std::wstring wpath (_MAX_PATH, L'\0');
    //   std::wstring wfname (_MAX_PATH, L'\0');

    //   GetTempPathW ((DWORD)wpath.size (), const_cast<wchar_t*>(wpath.data ()));

    //   UINT ret = ::GetTempFileNameW (wpath.c_str(), L"ÄñΩ",
    //     1, const_cast<wchar_t*>(wfname.data ()));
    //   ASSERT (ret > 0 ,"Temp_FileName");


    //   //Do the same thing with utf8::GetTempPath and utf8::GetTempFileName
    //   auto fname =  utf8::GetTempFileName (utf8::GetTempPath (), "ÄñΩ", 1);

    //   //see that we get the same result
    //   ASSERT_EQ (utf8::narrow (wfname.c_str()), fname ,"Temp_FileName");
    // }

    // check in-place versions of case folding functions

    { // case_conversion_inplace
        std::string lc{ "mircea neacșu ăâățî" };
        std::string uc{ "MIRCEA NEACȘU ĂÂĂȚÎ" };
        std::string t = lc;
        utf8::make_upper(t);
        ASSERT_EQ(uc, t, "case_conversion_inplace");
        t = uc;
        utf8::make_lower(t);
        ASSERT_EQ(lc, t, "case_conversion_inplace");
    }

    // check string-returning versions of case folding functions

    { // case_conversion_ret
        std::string uc = utf8::toupper("αλφάβητο");
        ASSERT_EQ("ΑΛΦΆΒΗΤΟ", uc, "case_conversion_ret");
        ASSERT_EQ("αλφάβητο", utf8::tolower("ΑΛΦΆΒΗΤΟ"), "case_conversion_ret");
    }

    // check case-insensitive comparison

    { // icompare_equal
        std::string lc{ "mircea neacșu ăâățî" };
        std::string uc{ "MIRCEA NEACȘU ĂÂĂȚÎ" };
        ASSERT(utf8::icompare(lc, uc) == 0, "icompare_equal");
    }


    { // icompare_less
        std::string lc{ "mircea neacșu ăâățî" };
        std::string uc{ "MIRCEA NEACȘU ĂÂĂȚÎ " };
        ASSERT(utf8::icompare(lc, uc) < 0, "icompare_less");
    }


    { // icompare_greater
        std::string lc{ "mircea neacșu ăâățî" };
        std::string uc{ "MIRCEA NEACȘU ĂÂ2ȚÎ" };
        ASSERT(utf8::icompare(lc, uc) > 0, "icompare_greater");
    }


    //     auto mydir = []()  -> std::string
    // {
    //   auto fname = GetModuleFileName ();
    //   std::string drive, dir, name, ext;
    //   utf8:: splitpath (fname, drive, dir, name, ext);
    //   return drive+dir;
    // }
    // find files named "test*" using find_first/find_next functions
    // { // find
    //   find_data fd;
    //   bool ret = find_first (mydir()+"test*", fd);
    //   cout << "find_first: " << fd.filename << " - " << fd.size/1024 << "kb" << endl;
    //   CHECK (ret);
    //   while (ret)
    //   {
    //     ret = find_next (fd);
    //     if (ret)
    //       cout << "find_next: " << fd.filename << " - " << fd.size / 1024 << "kb" << endl;
    //   }
    //   cout << endl;
    //   CHECK_EQUAL (ERROR_NO_MORE_FILES, GetLastError ());
    //   find_close (fd);
    // }

    // same thing as above using the file_enumerator class
    // { // find_with_finder
    //   file_enumerator f(mydir()+"test*");
    //   ASSERT (f.ok() ,"find_with_finder");
    //   while (f.ok ())
    //   {
    //     std::cout << "finder: " << f.filename << " - " << f.size / 1024 << "kb" << std::endl;
    //     f.next ();
    //   }
    // }


    // { // find_missing_file
    //   file_enumerator f ("no such file");
    //   ASSERT (!f.ok () ,"find_missing_file");
    // }


    // { // bool_op_missing_file
    //   file_enumerator found ("no such file");
    //   ASSERT (!found ,"bool_op_missing_file");
    // }

    // test character classes in 0-127 range match standard functions
    { // char_class
        char chartab[128];
        for (int i = 0; i < 128; i++)
            chartab[i] = (char)i;

        for (int i = 0; i < 128; i++) {
            char temp[2];
            temp[0] = chartab[i];
            temp[1] = 0;
            char tst[80];
            sprintf_s(tst, "testing char %d", i);
            ASSERT_EQ((bool)isalpha(chartab[i]), utf8::isalpha(temp), tst);
            ASSERT_EQ((bool)isalnum(chartab[i]), utf8::isalnum(temp), tst);
            ASSERT_EQ((bool)(isdigit)(chartab[i]), utf8::isdigit(temp), tst);
            ASSERT_EQ((bool)(isspace)(chartab[i]), utf8::isspace(temp), tst);
            ASSERT_EQ((bool)(isblank)(chartab[i]), utf8::isblank(temp), tst);
            ASSERT_EQ((bool)(isxdigit)(chartab[i]), utf8::isxdigit(temp), tst);
            ASSERT_EQ((bool)isupper(chartab[i]), utf8::isupper(temp), tst);
            ASSERT_EQ((bool)islower(chartab[i]), utf8::islower(temp), tst);
        }
    }

    // skip spaces in UTF-8 string
    { // skip_spaces
        const char* ccp = (const char*)u8" \xC2\xA0日本語";
        std::string s{ ccp }; 
        // std::string s{ " В ж—Ґжњ¬иЄћ" }; 
        auto p = s.begin();
        int blanks = 0;
        while (p != s.end() && utf8::isspace(p)) {
            blanks++;
            ASSERT2(utf8::next(p, s.end()));
        }
        ASSERT_EQ(2, blanks, "skip_spaces"); // both space and "no-break space" are space characters
    }


    // test character classes outside the 0-127 range
    { // is_upper_lower
        const char* uc{ "MIRCEANEACȘUĂÂȚÎ" };
        const char* lc{ "mirceaneacșuăâțî" };

        for (auto p = uc; *p; utf8::next(p)) {
            ASSERT(utf8::isupper(p), "is_upper_lower");
        }

        for (auto p = lc; *p; utf8::next(p)) {
            ASSERT(utf8::islower(p), "is_upper_lower");
        }
    }

    { // lower_substring
        const std::string uc{ "ȚEPUȘ nicolae" };
        const std::string lc{ "Țepuș nicolae" };

        auto p = uc.begin();
        std::string s = utf8::narrow(utf8::rune(p));
        utf8::next(p, uc.end());
        s += utf8::tolower(std::string(p, uc.end()));

        ASSERT_EQ(lc, s, "lower_substring");
    }


    // { // func_load_string
    //   auto uc = utf8::LoadString (IDS_UC);
    //   auto lc = utf8::LoadString (IDS_LC);
    //   ASSERT_EQ (lc, utf8::tolower (uc) ,"func_load_string");
    // }


    // { // func_get_module_filename
    //   std::string exe_name;
    //   ASSERT (utf8::GetModuleFileName (nullptr, exe_name), "func_get_module_filename");
    //   ASSERT_EQ (exe_name, utf8::GetModuleFileName (), "func_get_module_filename");
    //   exe_name.erase (exe_name.begin(), std::find_if (exe_name.rbegin (), exe_name.rend (),
    //     [](char ch) {return ch == '\\'; }).base());
    //   ASSERT_EQ ("tests.exe", exe_name ,"func_get_module_filename");
    // }
}