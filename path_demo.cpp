#include <iostream>
#include <map>
#include <unordered_set>

#include "./filesystem.hpp"

using namespace std;
using namespace std::filesystem;

// TAP-compliant test
int failed_nr = 0;
#define DIAG(out) std::cout << "# " << out << std::endl
#define _OK(cond, has_diag, diag) \
    if (cond) \
        std::cout << "ok line:" << __LINE__ << std::endl; \
    else { \
        failed_nr++; \
        std::cout << "not ok line:" << __LINE__ << std::endl; \
        DIAG("  Failed test at " << __FILE__ << " line " << __LINE__ << "."); \
        if (has_diag) DIAG(diag); \
    }

#define OK(cond)        _OK((cond),     0, "")
#define NOK(cond)       _OK(!(cond),    0, "")
#define IS(a, b)        _OK((a) == (b), 1, a << "==" << b)
#define ISNT(a, b)      _OK((a) != (b), 1, a << "!=" << b)
#define PASS()          _OK(true,       0, "")
#define FAIL()          _OK(false,      0, "")

// Platform specifics
#ifdef _WIN32
#define VOL     "c:"
#define SEP     "\\"
#else
#define VOL     ""
#define SEP     "/"
#endif

int main(int argc, char **argv) {
	path path1(VOL SEP "dir 1" SEP "dir 2" SEP);
	path path2("dir 3");

	// check empty path
	path p;
	IS(p.length(), 0);
	IS(p, "");

	p = path("");
	IS(p.length(), 0);
	IS(p, "");

#ifdef _WIN32
	IS(path1.length(), 3);
	IS(path1[0], "c:");
	IS(path1[1], "dir 1");
	IS(path1[2], "dir 2");
#else
	IS(path1.length(), 2);
	IS(path1[0], "dir 1");
	IS(path1[1], "dir 2");
#endif

	NOK(path1.exists());

	p = path("a/b/c/d");
	IS(p.slice(1), path("b/c/d"));
	IS(p.slice(2), path("c/d"));
	IS(p.slice(0, 0), path());
	IS(p.slice(0, 1), path("a"));
	IS(p.slice(1, 2), path("b"));
	IS(p.slice(0, 2), path("a/b"));
	IS(p.slice(0, 3), path("a/b/c"));

	IS(path1, VOL SEP "dir 1" SEP "dir 2");

	p = (path1 / path2);
	IS(p, VOL SEP "dir 1" SEP "dir 2" SEP "dir 3");

	p = (path1 / path2).dirname();
	IS(p, VOL SEP "dir 1" SEP "dir 2");

	p = (path1 / path2).dirname().dirname();
	IS(p, VOL SEP "dir 1");

#ifdef _WIN32
	p = (path1 / path2).dirname().dirname().dirname();
	IS(p, VOL);

	p = (path1 / path2).dirname().dirname().dirname().dirname();
	IS(p, "");
#else
	p = (path1 / path2).dirname().dirname().dirname();
	IS(p, SEP);

	p = (path1 / path2).dirname().dirname().dirname().dirname();
	IS(p, SEP);
#endif

	p = path().dirname();
	IS(p, "..");

	p = "foo/bar";
#ifdef _WIN32
	IS(p.native(), "foo\\bar");
	IS(p.string(), "foo\\bar");
	IS((string)p, "foo\\bar");
	OK(strcmp(p.c_str(), "foo\\bar") == 0);
#else
	IS(p.native(), "foo/bar");
	IS(p.string(), "foo/bar");
	IS(p, "foo/bar");
	OK(strcmp(p.c_str(), "foo/bar") == 0);
#endif

	IS(path(".").stem(), ".");
	IS(path("..").stem(), "..");
	IS(path(".file").stem(), ".file");
	IS(path("file").stem(), "file");
	IS(path("file.").stem(), "file");
	IS(path("file.ext").stem(), "file");
	IS(path("dir/.file").stem(), ".file");
	IS(path("dir/file").stem(), "file");
	IS(path("dir/file.").stem(), "file");
	IS(path("dir/file.ext").stem(), "file");
	IS(path("/dir/.file").stem(), ".file");
	IS(path("/dir/file").stem(), "file");
	IS(path("/dir/file.").stem(), "file");
	IS(path("/dir/file.ext").stem(), "file");

	auto sourcePath = path(__FILE__);
	OK(sourcePath.exists());

	auto sourceDir = sourcePath.dirname();
	OK((sourceDir / "path_demo.cpp").exists());

	chdir(sourceDir.c_str());

	IS(path::getcwd(), sourceDir);

	IS(sourcePath.make_absolute(), __FILE__);
	IS(path("path_demo.cpp").make_absolute(), __FILE__);
	IS(path("./path_demo.cpp").make_absolute(), __FILE__);

	cout << "some/path.ext:operator==() = " << (path("some/path.ext") == path("some/path.ext")) << endl;
	cout << "some/path.ext:operator==() (unequal) = " << (path("some/path.ext") == path("another/path.ext")) << endl;

	map<path, int> pathInts;
	pathInts[path::getcwd()] = 125;
	pathInts[path::getcwd() / "foo"] = 1337;
	cout << "map[cwd] (125) = " << pathInts[path::getcwd()] << endl;
	cout << "map[cwd/foo] (1337) = " << pathInts[path::getcwd() / "foo"] << endl;

	unordered_set<path> pathSet;
	pathSet.insert("foo/bar");
	pathSet.insert("qux/qix");
	pathSet.insert("foo/bar");
	cout << "pathSet size (2) = " << pathSet.size() << endl;

	cout << "nonexistant:exists = " << path("nonexistant").exists() << endl;
	cout << "nonexistant:is_file = " << path("nonexistant").is_file() << endl;
	cout << "nonexistant:is_directory = " << path("nonexistant").is_directory() << endl;
	cout << "nonexistant:filename = " << path("nonexistant").filename() << endl;
	cout << "nonexistant:extension = " << path("nonexistant").extension() << endl;
	cout << "filesystem.hpp:exists = " << path("filesystem.hpp").exists() << endl;
	cout << "filesystem.hpp:is_file = " << path("filesystem.hpp").is_file() << endl;
	cout << "filesystem.hpp:is_directory = " << path("filesystem.hpp").is_directory() << endl;
	cout << "filesystem.hpp:filename = " << path("filesystem.hpp").filename() << endl;
	cout << "filesystem.hpp:extension = " << path("filesystem.hpp").extension() << endl;
	cout << "a/../../foo.c:resolve = " << path("a/../../foo.c").resolve() << endl;
	cout << "filesystem.hpp:make_absolute = " << path("filesystem.hpp").make_absolute() << endl;
	cout << "missing:make_absolute = " << path("missing").make_absolute() << endl;
	cout << "./missing:make_absolute = " << path("./missing").make_absolute() << endl;
	cout << "../missing:make_absolute = " << path("../missing").make_absolute() << endl;
	cout << "filesystem.hpp:dirname = " << path("filesystem.hpp").dirname() << endl;
	cout << "filesystem.hpp:filename = " << path("filesystem.hpp").filename() << endl;
	cout << "filesystem.hpp:resolve = " << path("filesystem.hpp").resolve() << endl;
	cout << "/a/b/c/../../d/e/foo/../././././bar.h:resolve = " << path("/a/b/c/../../d/e/foo/../././././bar.h").resolve() << endl;
	cout << "../filesystem.hpp:resolve = " << path("../filesystem.hpp").resolve() << endl;
	cout << "filesystem.hpp:resolve /a/b/c = " << path("/a/b/c").resolve("filesystem.hpp") << endl;
	cout << "../filesystem.hpp:resolve /a/b/c = " << path("/a/b/c").resolve("../filesystem.hpp") << endl;
	cout << "../filesystem.hpp:resolve /a/b/../c = " << path("/a/b/../c").resolve("../filesystem.hpp") << endl;
	cout << "../filesystem:exists = " << path("../filesystem").exists() << endl;
	cout << "../filesystem:is_file = " << path("../filesystem").is_file() << endl;
	cout << "../filesystem:is_directory = " << path("../filesystem").is_directory() << endl;
	cout << "../filesystem:extension = " << path("../filesystem").extension() << endl;
	cout << "../filesystem:filename = " << path("../filesystem").filename() << endl;
	cout << "../filesystem.hpp:resolve = " << path("../filesystem.hpp").resolve() << endl;
	if (path("../filesystem").exists()) {
		cout << "../filesystem:make_absolute = " << path("../filesystem").make_absolute() << endl;
	}
	cout << "../../a/b/c/../d:resolve = " << path("../../a/b/c/../d").resolve() << endl;
	cout << "/../../a/b/c/../d:resolve = " << path("/../../a/b/c/../d").resolve() << endl;

	cout << "resolve(filesystem.hpp) = " << resolver().resolve("filesystem.hpp") << endl;
	cout << "resolve(nonexistant) = " << resolver().resolve("nonexistant") << endl;

	cout << "with_extension(foo.bar -> foo.qix) = " << path("a/b/c/foo.bar").with_extension(".qix") << endl;
	cout << "relative(base=/a/b/c/d, target=/a/b/f/g/e) = " << path("/a/b/f/g/e").relative("/a/b/c/d") << endl;

	cout << "temporary directory = " << temp_directory_path() << endl;

	int exitCode = 0;

	if (failed_nr)
	{
		DIAG("Some tests failed!");
		exitCode = 1;
	}
	else
	{
		std::cout << "All OK" << std::endl;
	}

	//cout << "Press ENTER to exit" << endl;

	std::cin.get();

	return exitCode;
}
