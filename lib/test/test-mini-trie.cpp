#include "../src/mini-trie.h"
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

TEST_CASE( "Empty MiniTrie", "[mini-trie]" ) {

    MiniTrie<char> trie{};
    {
        const char * str = "";
        const char * first = str;
        auto res = trie.prefixMatch(first, first + strlen(first));
        CHECK(res == trie.noMatch);
        CHECK(first == str);
    }
    {
        const char * str = "a";
        const char * first = str;
        auto res = trie.prefixMatch(first, first + strlen(first));
        CHECK(res == trie.noMatch);
        CHECK(first == str);
    }

}

TEST_CASE( "MiniTrie with only empty string", "[mini-trie]" ) {

    MiniTrie<char> trie({""});
    {
        const char * str = "";
        const char * first = str;
        auto res = trie.prefixMatch(first, first + strlen(first));
        CHECK(res == 0);
        CHECK(first == str);
    }
    {
        const char * str = "a";
        const char * first = str;
        auto res = trie.prefixMatch(first, first + strlen(first));
        CHECK(res == 0);
        CHECK(first == str);
    }

}

TEST_CASE( "MiniTrie with disjoing strings", "[mini-trie]" ) {

    {

        MiniTrie<char> trie{{"b", "c", "a"}};
        {
            const char * str = "";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == trie.noMatch);
            CHECK(first == str);
        }
        {
            const char * str = "a";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == 2);
            CHECK(first == str + strlen(str));
        }
        {
            const char * str = "b";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == 0);
            CHECK(first == str + strlen(str));
        }
        {
            const char * str = "c";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == 1);
            CHECK(first == str + strlen(str));
        }
    }

    {
        MiniTrie<char> trie{{"ef", "cd", "ab"}};
        {
            const char * str = "";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == trie.noMatch);
            CHECK(first == str);
        }
        {
            const char * str = "a";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == trie.noMatch);
            CHECK(first == str);
        }
        {
            const char * str = "b";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == trie.noMatch);
            CHECK(first == str);
        }
        {
            const char * str = "cd";
            const char * first = str;
            auto res = trie.prefixMatch(first, first + strlen(first));
            CHECK(res == 1);
            CHECK(first == str + strlen(str));
        }
    }

}
