// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#include "scalar-xif-matcher.h"
#include "mini-trie.h"
#include "macro-map.h"
#include "whitespace.h"


using namespace Spreader;


#define SPR_X_IF_MATCHER_OPS \
    "<", \
    "<=", \
    "<>", \
    ">", \
    ">=", \
    "="

#define SPR_X_IF_MATCHER_LITERALS \
    "TRUE", \
    "FALSE", \
    "#DIV/0!", \
    "#N/A", \
    "#NAME?", \
    "#NULL!", \
    "#NUM!", \
    "#REF!", \
    "#VALUE!", \
    "#SPILL!"

#define SPR_X_IF_MATCHER_OPS_U16 SPR_MAP_LIST(SPR_U16, SPR_X_IF_MATCHER_OPS)
#define SPR_X_IF_MATCHER_LITERALS_U16 SPR_MAP_LIST(SPR_U16, SPR_X_IF_MATCHER_LITERALS)

#define SPR_X_IF_MATCHER_OPS_U32 SPR_MAP_LIST(SPR_U32, SPR_X_IF_MATCHER_OPS)
#define SPR_X_IF_MATCHER_LITERALS_U32 SPR_MAP_LIST(SPR_U32, SPR_X_IF_MATCHER_LITERALS)


template<>
struct XIfMatcher::Constants<char> {
    MiniTrie<char> opMatcher = {SPR_X_IF_MATCHER_OPS};
    MiniTrie<char> literalMatcher = {SPR_X_IF_MATCHER_LITERALS};
};

template<>
struct XIfMatcher::Constants<char16_t> {
    MiniTrie<char16_t> opMatcher = {SPR_X_IF_MATCHER_OPS_U16};
    MiniTrie<char16_t> literalMatcher = {SPR_X_IF_MATCHER_LITERALS_U16};
};

template<>
struct XIfMatcher::Constants<char32_t> {
    MiniTrie<char32_t> opMatcher = {SPR_X_IF_MATCHER_OPS_U32};
    MiniTrie<char32_t> literalMatcher = {SPR_X_IF_MATCHER_LITERALS_U32};
};

void XIfMatcher::parseMatchString(const String & str) {
    
    using Char = String::storage_type;
    
    static Constants<Char> constants;

    SPR_ASSERT_LOGIC(!str.empty());
    
    auto strUpper = str.to_upper();
    auto access = String::char_access(strUpper);
    
    auto cur = access.begin();
    const auto end = access.end();
    
    switch(constants.opMatcher.prefixMatch(cur, end)) {
        case 0: m_op = Less(); break;
        case 1: m_op = LessEqual(); break;
        case 2: m_op = NotEqualTo(); break;
        case 3: m_op = Greater(); break;
        case 4: m_op = GreaterEqual(); break;
        case 5: break; //m_op = EqualTo(); break; - this is the default init
        default: SPR_ASSERT_LOGIC(false);
    }
    
    auto valueStart = cur;
    
    cur = skipWhiteSpace(cur, end);
    
    if (auto idx = constants.literalMatcher.prefixMatch(cur, end); idx != constants.literalMatcher.noMatch && skipWhiteSpace(cur, end) == end) {
        
        switch(idx) {
            case 0: m_compValue = true; break;
            case 1: m_compValue = false; break;
            case 2: m_compValue = Error::DivisionByZero; break;
            case 3: m_compValue = Error::InvalidArgs; break;
            case 4: m_compValue = Error::InvalidName; break;
            case 5: m_compValue = Error::NullRange; break;
            case 6: m_compValue = Error::NotANumber; break;
            case 7: m_compValue = Error::InvalidReference; break;
            case 8: m_compValue = Error::InvalidValue; break;
            case 9: m_compValue = Error::Spill; break;
            default: SPR_ASSERT_LOGIC(false);
        }
        
    } else if (NumberMatcher<CharType> numberMatcher; numberMatcher.prefixMatch(cur, end) && skipWhiteSpace(cur, end) == end) {
        
        m_compValue = numberMatcher.value();
        
    } else {
        
        m_compValue = String(valueStart, end);
    }
}


