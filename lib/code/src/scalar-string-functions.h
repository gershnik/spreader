// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_STRING_FUNCTIONS_H_INCLUDED
#define SPR_HEADER_SCALAR_STRING_FUNCTIONS_H_INCLUDED

#include <spreader/types.h>
#include <spreader/scalar.h>
#include "scalar-operators.h"
#include "scalar-function.h"

namespace Spreader {

    struct StringFunctions {
        
        static constexpr bool OptimizeNativeStorage = true;

        struct Trim {
            auto operator()(const String & val) const -> String
                { return val.trim(); }
        };
        
        struct Upper {
            auto operator()(const String & val) const -> String
                { return val.to_upper(); }
        };
        
        struct Lower {
            auto operator()(const String & val) const -> String
                { return val.to_lower(); }
        };
        
        struct Len {
            auto operator()(const String & val) const noexcept -> Number {
                if constexpr (std::is_same_v<String::storage_type, char16_t> && OptimizeNativeStorage)
                    return val.storage_size();
                else {
                    using Conv = sysstr::utf_converter<sysstr::utf_encoding_of<String::storage_type> ,sysstr::utf16>;
                    auto access = String::char_access(val);
                    return Conv().converted_length(access.begin(), access.end());
                }
            }
        };
        
        struct Left {
          
            auto operator()(const String & val, Number count) const -> Scalar {
              
                if (count.value() < 0)
                    return Error::InvalidValue;
                
                auto intCount = String::size_type(count.value());

                if (intCount > String::max_size)
                    return Error::InvalidValue;
                
                String::utf32_view valView(val);
                auto cursor = valView.cursor_begin();
                auto startStorage = cursor.storage_cursor();
                for ( ; intCount > 0 && cursor; ++cursor, --intCount) {
                    ;
                }
                return String(startStorage, cursor.storage_pos());
                
            };
            
            auto operator()(const String & val) const -> Scalar {
                return operator()(val, 1);
            };
            
        };
        
        struct Right {
          
            auto operator()(const String & val, Number count) const -> Scalar {
              
                if (count.value() < 0)
                    return Error::InvalidValue;
                
                auto intCount = String::size_type(count.value());

                if (intCount > String::max_size)
                    return Error::InvalidValue;
                
                String::utf32_view valView(val);
                auto cursor = valView.cursor_begin<String::utf32_view::cursor_direction::backward>();
                auto startStorage = cursor.storage_cursor();
                for ( ; intCount > 0 && cursor; ++cursor, --intCount) {
                    ;
                }
                return String(startStorage, val.storage_size() - cursor.storage_pos());
                
            };
            
            auto operator()(const String & val) const -> Scalar {
                return operator()(val, 1);
            };
            
        };
        
        struct Mid {
            auto operator()(const String & val, Number start, Number count) const  -> Scalar {
                if (start.value() < 1)
                    return Error::InvalidValue;
                if (count.value() < 0)
                    return Error::InvalidValue;
                
                auto startIdx = String::size_type(start.value()) - 1;
                if (startIdx > String::max_size)
                    return Error::InvalidValue;
                auto intCount = String::size_type(count.value());
                if (intCount > String::max_size)
                    return Error::InvalidValue;
                
                if constexpr (std::is_same_v<String::storage_type, char16_t> && OptimizeNativeStorage) {
                    
                    auto length = val.storage_size();
                    
                    if (startIdx >= length)
                        return SPRS("");
                    
                    String::char_access valAccess(val);
                    auto startIt = valAccess.begin();
                    startIt += startIdx;
                    if (startIdx > 0) {
                        auto c = *startIt;
                        bool isTrail = ((c & 0xFC00) == 0xDC00);
                        if (isTrail) {
                            --startIt;
                        }
                    }
                    auto endIt = startIt;
                    for (auto end = valAccess.end(); endIt != end && intCount != 0; --intCount) {
                        auto c = *endIt++;
                        bool isLead = ((c & 0xFC00) == 0xD800);
                        if (isLead && endIt != end) {
                            ++endIt;
                        }
                    }
                    return String(startIt, endIt);
                } else {
                    
                    String::utf32_view valView(val);
                    auto cursor = valView.cursor_begin();
                    String::size_type consumed = 0;
                    for ( ; ; ) {
                        if (!cursor)
                            return SPRS("");
                        if (consumed == startIdx)
                            break;
                        char32_t c = *cursor;
                        consumed += (1 + (c >= 0x010000));
                        if (consumed > startIdx)
                            break;
                        ++cursor;
                    }
                    auto startCur = cursor.storage_cursor();
                    for( ; intCount > 0 && cursor; ++cursor, --intCount) {
                        ;
                    }
                    return String(startCur, cursor.storage_pos() - startCur.position());
                }
            }
        };
        
        struct Replace {
          
            auto operator()(const String & val, Number start, Number count, const String & replacement) const  -> Scalar {
                
                if (start.value() < 1)
                    return Error::InvalidValue;
                if (count.value() < 0)
                    return Error::InvalidValue;
                
                auto startIdx = String::size_type(start.value()) - 1;
                if (startIdx > String::max_size)
                    return Error::InvalidValue;
                auto intCount = String::size_type(count.value());
                if (intCount > String::max_size)
                    return Error::InvalidValue;
                
                StringBuilder builder;
                if constexpr (std::is_same_v<String::storage_type, char16_t> && OptimizeNativeStorage) {
                    
                    auto length = val.storage_size();
                    
                    if (startIdx >= length)
                        return String(val + replacement);
                    
                    String::char_access valAccess(val);
                    
                    String::size_type suffixStart = startIdx;
                    if (startIdx > 0) {
                        auto c = valAccess[startIdx];
                        bool isTrail = ((c & 0xFC00) == 0xDC00);
                        if (isTrail) {
                            --startIdx;
                            ++suffixStart;
                            if (intCount)
                                --intCount;
                        }
                        copyChars(valAccess, 0, startIdx, builder);
                    }
                    builder.append(replacement);
                    for ( ; ; ) {
                        if (suffixStart == length)
                            return builder.build();
                        if (intCount == 0)
                            break;
                        auto c = valAccess[suffixStart++];
                        bool isLead = ((c & 0xFC00) == 0xD800);
                        if (isLead && suffixStart < length) {
                            ++suffixStart;
                        }
                        --intCount;
                    }
                    copyChars(valAccess, suffixStart, length - suffixStart, builder);
                    return builder.build();
                } else {
                    
                    String::utf32_view valView(val);
                    auto cursor = valView.cursor_begin();
                    String::size_type consumed = 0;
                    for ( ; ; ) {
                        if (!cursor) {
                            builder.append(replacement);
                            return builder.build();
                        }
                        if (consumed == startIdx)
                            break;
                        char32_t c = *cursor;
                        ++cursor;
                        consumed += (1 + (c >= 0x010000));
                        if (consumed > startIdx) {
                            if (intCount)
                                --intCount;
                            break;
                        }
                        builder.append(c);
                    }
                    builder.append(replacement);
                    for( ; intCount > 0 && cursor; ++cursor, --intCount) {
                        ;
                    }
                    for( ; cursor; ++cursor ) {
                        builder.append(*cursor);
                    }
                    return builder.build();
                }
            }
            
        };
        
        struct Substitute {
          
            auto operator()(const String & val, const String & oldText, const String & newText, Number index) const  -> Scalar {
                
                if (index.value() < 1 || index.value() > std::numeric_limits<unsigned>::max())
                    return Error::InvalidValue;
                auto intIndex = unsigned(index.value()); //1-based
                
                String::char_access oldAccess(oldText);
                if (oldAccess.size() == 0)
                    return val;
                auto oldBegin = oldAccess.begin();
                const auto oldEnd = oldAccess.end();
                
                
                String::char_access valAccess(val);
                const auto start = valAccess.begin();
                auto first = start;
                const auto last = valAccess.end();
                
                StringBuilder builder;
                
                for( ; ; ) {
                    auto found = std::search(first, last, oldBegin, oldEnd);
                    copyChars(valAccess, first - start, found - first, builder);
                    if (found == last)
                        break;
                    first = found + oldAccess.size();
                    if (--intIndex == 0) {
                        builder.append(newText);
                        copyChars(valAccess, first - start, last - first, builder);
                        break;
                    } else {
                        builder.append(oldText);
                    }
                }
                
                return builder.build();
            }
            
            auto operator()(const String & val, const String & oldText, const String & newText) const  -> String {
                
                return val.replace(oldText, newText);
            }
            
        };

        struct Find {
            auto operator()(const String & val, const String & findIn) const  -> Scalar {
                return (*this)(val, findIn, 1);
            }

            auto operator()(const String & val, const String & findIn, Number start) const  -> Scalar {

                if (start.value() < 1)
                    return Error::InvalidValue;

                auto startIdx = String::size_type(start.value()) - 1;
                if (startIdx > String::max_size)
                    return Error::InvalidValue;
                

                String::char_access valAccess(val);
                String::char_access findInAccess(findIn);
                
                if constexpr (std::is_same_v<String::storage_type, char16_t> && OptimizeNativeStorage) {

                    auto valLength = valAccess.size();
                    auto findInLength = findInAccess.size();
                    
                    if (valLength + startIdx > findInLength)
                        return Error::InvalidValue;

                    if (valLength == 0)
                        return startIdx + 1;
                    
                    auto findInBegin = findInAccess.begin();
                    findInBegin += startIdx;
                    auto findInEnd = findInAccess.end();
                    auto it = std::search(findInBegin, findInEnd, valAccess.begin(), valAccess.end());
                    if (it == findInEnd)
                        return Error::InvalidValue;

                    return startIdx + (it - findInBegin) + 1;
                    
                } else {
                    String::utf16_view findInView(findIn);
                    auto findInCursor = findInView.cursor_begin();
                    bool isLastLead = false;
                    for (auto count = startIdx ; count > 0; ++findInCursor, --count) {
                        if (!findInCursor) {
                            return Error::InvalidValue;
                        }
                        isLastLead = ((*findInCursor & 0xFC00) == 0xD800);
                    }
                    
                    if (valAccess.size() == 0) {
                        return startIdx + 1;
                    }
                    
                    auto findInBegin = findInAccess.begin();
                    findInBegin += findInCursor.storage_pos();
                    if (isLastLead)
                        findInBegin += findInCursor.storage_size();
                    auto findInEnd = findInAccess.end();
                    
                    auto it = std::search(findInBegin, findInEnd, valAccess.begin(), valAccess.end());
                    if (it == findInEnd)
                        return Error::InvalidValue;
                    
                    
                    using Conv = sysstr::utf_converter<sysstr::utf_encoding_of<String::storage_type> ,sysstr::utf16>;
                    auto len = Conv().converted_length(findInBegin, it);
                    return startIdx + isLastLead + len + 1;
                }
            }
        };
    
    private:
        template<class CharAccess>
        static auto dummyCharAccessHasData(decltype(std::declval<CharAccess>().data())) -> std::true_type;
        template<class CharAccess>
        static auto dummyCharAccessHasData(...) -> std::false_type;
        
        template<class CharAccess>
        static constexpr bool CharAccessHasData = decltype(dummyCharAccessHasData<CharAccess>(nullptr))::value;

        template<class CharAccess = String::char_access>
        requires(CharAccessHasData<CharAccess>)
        static void copyChars(const CharAccess & chars, String::size_type from, String::size_type count, StringBuilder & builder) {
            
            builder.chars().append(chars.data() + from, count);
        }
        
        template<class CharAccess = String::char_access>
        requires(!CharAccessHasData<CharAccess>)
        static void copyChars(const CharAccess & chars, String::size_type from, String::size_type count, StringBuilder & builder) {
            
            for (String::size_type i = 0; i < count; ++i)
                builder.chars().push_back(chars[from + i]);
        }

    };

    using ScalarAmpersand =     ScalarDetail::BinaryOperator<std::plus<>,         String>;
    
    using ScalarFind =          ScalarDetail::ScalarFunction<StringFunctions::Find,       Typelist<String, String>, Typelist<Number>>;
    using ScalarLeft =          ScalarDetail::ScalarFunction<StringFunctions::Left,       Typelist<String>, Typelist<Number>>;
    using ScalarLen =           ScalarDetail::ScalarFunction<StringFunctions::Len,        Typelist<String>>;
    using ScalarLower =         ScalarDetail::ScalarFunction<StringFunctions::Lower,      Typelist<String>>;
    using ScalarMid =           ScalarDetail::ScalarFunction<StringFunctions::Mid,        Typelist<String, Number, Number>>;
    using ScalarReplace =       ScalarDetail::ScalarFunction<StringFunctions::Replace,    Typelist<String, Number, Number, String>>;
    using ScalarRight =         ScalarDetail::ScalarFunction<StringFunctions::Right,      Typelist<String>, Typelist<Number>>;
    using ScalarSubstitute =    ScalarDetail::ScalarFunction<StringFunctions::Substitute, Typelist<String, String, String>, Typelist<Number>>;
    using ScalarTrim =          ScalarDetail::ScalarFunction<StringFunctions::Trim,       Typelist<String>>;
    using ScalarUpper =         ScalarDetail::ScalarFunction<StringFunctions::Upper,      Typelist<String>>;
    
}

#endif 
