#include <iostream>
#include <string>
#include <algorithm>

#define TYTI_NO_L_UNDEF
#include <vdf_parser.hpp>
#define T_L(x) TYTI_L(charT,x)
using namespace tyti;

#include <catch.hpp>

#include <experimental/filesystem>

template<typename charT>
void check_DST_AST(const vdf::basic_object<charT>& obj)
{
    CHECK(obj.name == T_L("AppState"));
    REQUIRE(obj.attribs.size() == 17);
    REQUIRE(obj.childs.size() == 4);

    CHECK(obj.attribs.at(T_L("appid")) == T_L("343050"));

    CHECK(obj.attribs.at(T_L("buildid")) == T_L("1101428"));
    CHECK(obj.attribs.at(T_L("#1_attrib")) == T_L("1"));
    CHECK(obj.attribs.at(T_L("emptyAttrib")) == T_L(""));

    CHECK(obj.childs.at(T_L("UserConfig"))->name == T_L("UserConfig"));
    CHECK(obj.childs.at(T_L("UserConfig"))->childs.empty());

    const auto inc = obj.childs.at(T_L("IncludedStuff"));
    CHECK(inc->name == T_L("IncludedStuff"));
    const auto base = obj.childs.at(T_L("BaseInclude"));
    REQUIRE(base->attribs.size() == 1);
    CHECK(base->attribs.at(T_L("BaseAttrib")) == T_L("Yes"));
    CHECK(obj.attribs.at(T_L("another attribute with fancy space")) == T_L("yay"));
}

template<typename charT>
void read_check_DST_file()
{
    auto path = std::basic_string<charT>(T_L("DST_Manifest.acf"));
    std::basic_ifstream<charT> file(path);
    bool ok;
    auto object = vdf::read(file, &ok);

    REQUIRE(ok);

    check_DST_AST(object);
}

TEST_CASE("Read File", "[read]")
{
    std::experimental::filesystem::current_path(std::string(SOURCE_DIR)+"/testdata/");
    read_check_DST_file<char>();
    read_check_DST_file<wchar_t>();
}

template<typename charT>
void read_string()
{
    std::basic_string<charT> attribs{ T_L("\"firstNode\"{\"SecondNode\"{\"Key\" \"Value\" //myComment\n}}") };
    bool ok;
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(ok);

    CHECK(obj.name == T_L("firstNode"));

    CHECK(obj.attribs.empty() == true);
    REQUIRE(obj.childs.size() == 1);
    auto secondNode = obj.childs.at(T_L("SecondNode"));

    CHECK(secondNode->name == T_L("SecondNode"));
    REQUIRE(secondNode->attribs.size() == 1);
    CHECK(secondNode->childs.empty() == true);
    CHECK(secondNode->attribs.at(T_L("Key")) == T_L("Value"));
}

TEST_CASE("Read String","[read]")
{
    read_string<char>();
    read_string<wchar_t>();
}

template<typename charT>
void check_fail()
{
    bool ok;
    std::basic_string<charT> attribs{ T_L("\"firstNode\"{\"SecondNode\"{\"Key\" //myComment\n}}") };
    auto obj = vdf::read(attribs.begin(), attribs.end(), &ok);

    REQUIRE(!ok);
}
//todo: error checking
TEST_CASE("Find Error","[read_error]")
{
    check_fail<char>();
    check_fail<wchar_t>();
}