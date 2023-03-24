// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_SCALAR_LOOKUP_H_INCLUDED
#define SPR_HEADER_SCALAR_LOOKUP_H_INCLUDED

#include <spreader/scalar.h>
#include <spreader/array.h>
#include <spreader/geometry.h>
#include <spreader/iteration.h>

#include "wildcarder.h"
#include "execution-context.h"

#include <optional>

namespace Spreader {

    class ScalarLookup {

    public:
        enum Direction {
            Horizontal,
            Vertical
        };

        struct RectParam {
            ExecutionContext & context;
            Rect rect;
        };

    private:
        template<Direction Dir>
        struct DirectionTraits {
            static auto extentOf(const ArrayPtr & array) noexcept -> SizeType
                { return dim(array->size()); }
            
            static auto extentOf(const RectParam & param) -> SizeType
                { return std::min(dim(param.context.grid().maxSize()) - dim(param.rect.origin), dim(param.rect.size)); }
            
            static auto comparisonPoint(SizeType n) -> Point
                { return makePoint(n, 0); }
            
            static auto lookupPoint(SizeType n, SizeType index) -> Point
                { return makePoint(n, index); }
            
            static auto extractorOf(const ArrayPtr & array) {
                return [&](SizeType idx) -> const Scalar & {
                    return (*array)[comparisonPoint(idx)];
                };
            }
            
            static auto extractorOf(const RectParam & param) {
                return [&](SizeType idx) -> std::optional<Scalar> {
                    Point pt = param.rect.origin + asSize(comparisonPoint(idx));
                    return param.context.evaluateCell(pt, [&](const Scalar & val) -> std::optional<Scalar> {
                        return val;
                    }, [&](bool /*isCircular*/) -> std::optional<Scalar> {
                        return std::nullopt;
                    });
                };
            }
            
        private:
            static constexpr auto dim(Size s) noexcept -> SizeType {
                if      constexpr (Dir == Horizontal)
                    return s.width;
                else if constexpr (Dir == Vertical)
                    return s.height;
            }
            static constexpr auto dim(Point p) noexcept -> SizeType {
                if      constexpr (Dir == Horizontal)
                    return p.x;
                else if constexpr (Dir == Vertical)
                    return p.y;
            }
            static auto makePoint(SizeType a, SizeType b) -> Point {
                if      constexpr (Dir == Horizontal)
                    return Point{a, b};
                else if constexpr (Dir == Vertical)
                    return Point{b, a};
            }
        };
        
        static auto sizeOf(const ArrayPtr & array) -> Size {
            return array->size();
        }

        static auto sizeOf(const RectParam & param) -> Size {
            return param.rect.size;
        }
        

        template<class Res, class T>
        static auto resultFromType(T && arg) {
            using ArgType = std::remove_cvref_t<decltype(arg)>;

            if constexpr (std::is_same_v<ArgType, RectParam>) {
                return std::optional<Res>();
            } else if constexpr (std::is_same_v<ArgType, ArrayPtr>) {
                return Res();
            } else {
                static_assert(dependentFalse<ArgType>, "invalid parameter type");
            }
        }
        
        enum IterationOrder {
            IterateForward,
            IterateBackward
        };
        
//        template<IterationOrder Order>
//        struct IterationTraits {
//
//            using Range = std::conditional_t<Order == IterateForward, DimensionRange, ReverseDimensionRange>;
//
//            static auto makeRange(SizeType extent) noexcept -> Range {
//                if constexpr (Order == IterateForward)
//                    return Range{0, extent};
//                else
//                    return Range{extent, 0};
//            }
//        };

    public:

        template<class Res, class T>
        using ResultType = decltype(resultFromType<Res>(std::declval<std::remove_cvref_t<T>>()));

        template<Direction Dir, class Arg>
        static auto lookup(bool inexact, const Scalar & value, Arg && data, SizeType index) ->  ResultType<Scalar, Arg> {
            MakeValueResult<std::remove_cvref_t<Arg>> dest(index); 
            inexact ?
                    matchInexactForward<Dir>(value, data, dest) :
                    matchExact<Dir>(value, data, dest);
            return dest.result;
        }

        template<class Arg>
        static auto match(int type, const Scalar & value, Arg && data) ->  ResultType<Scalar, Arg> {
            Size size = sizeOf(data);

            SPR_ASSERT_INPUT(size.width == 1 || size.height == 1);

            MakeIndexResult<std::remove_cvref_t<Arg>> dest;

            if (type < 0) {
                size.width == 1 ? 
                        matchInexactBackward<Vertical>(value, data, dest) :
                        matchInexactBackward<Horizontal>(value, data, dest);
            }
            else if (type == 0) {
                size.width == 1 ? 
                        matchExact<Vertical>(value, data, dest) :
                        matchExact<Horizontal>(value, data, dest);
            }
            else { 
                size.width == 1 ? 
                        matchInexactForward<Vertical>(value, data, dest) :
                        matchInexactForward<Horizontal>(value, data, dest);
            }
            return dest.result;
        }

    private:
        template<class Arg>
        struct MakeValueResult {
            static constexpr bool optimizeSettingValue = true;
            
            MakeValueResult(SizeType index): m_index(index) 
            {}

            auto setFoundValue(const Scalar & val) {
                if (this->m_index == 0)
                    this->result = val;
            }

            template<Direction Dir>
            auto set(const Arg & arg, SizeType i) {

                using Traits = DirectionTraits<Dir>;
                
                Point offset = Traits::lookupPoint(i, this->m_index);

                if constexpr (std::is_same_v<Arg, ArrayPtr>)
                    this->result = (*arg)[offset];
                else if constexpr (std::is_same_v<Arg, RectParam>) {
                    Point pt = arg.rect.origin + asSize(offset);
                    arg.context.evaluateCell(pt, [&](const Scalar & val) {
                        this->result = val;
                    }, [&](bool /*isCircular*/) {});
                }
            }

            ResultType<Scalar, Arg> result;

        private:
            SizeType m_index;
        };

        template<class Arg>
        struct MakeIndexResult {
            static constexpr bool optimizeSettingValue = false;
            
            template<Direction Dir>
            auto set(const Arg & /*array*/, SizeType i) {
                this->result = i + 1;
            }

            ResultType<Scalar, Arg> result;
        };


        template<Direction Dir, class Dest>
        static void matchExact(const Scalar & value, const ArrayPtr & array, Dest & dest) {

            using Traits = DirectionTraits<Dir>;
            Match match;
            
            for(auto idx: DimensionRange(0, Traits::extentOf(array))) {
                const Scalar & val = (*array)[Traits::comparisonPoint(idx)];
                if (match(value, val)) {
                    dest.template set<Dir>(array, idx);
                    return;
                }
            }
            dest.result = Error::InvalidArgs;
        }

        template<Direction Dir, class Dest>
        static void matchExact(const Scalar & value, const RectParam & param, Dest & dest) {

            using Traits = DirectionTraits<Dir>;
            
            auto extent = Traits::extentOf(param);
            Match match;
            
            bool noDependencies = true;
            SizeType found = extent;
            for(auto idx: DimensionRange(0, extent)) {
                bool shouldContinue = true;
                Point pt = param.rect.origin + asSize(Traits::comparisonPoint(idx));
                param.context.evaluateCell(pt, [&](const Scalar & val) {
                    if (noDependencies && match(value, val)) {
                        if constexpr (Dest::optimizeSettingValue)
                            dest.setFoundValue(val);
                        found = idx;
                        shouldContinue = false;
                    }
                }, [&](bool isCircular) {
                    noDependencies = false;
                    shouldContinue = !isCircular;
                });
                if (!shouldContinue)
                    break;
            }
            if (!noDependencies)
                dest.result = std::nullopt;
            else if (found != extent) {
                if constexpr (!Dest::optimizeSettingValue)
                    dest.template set<Dir>(param, found);
                else {
                    if (!dest.result)
                        dest.template set<Dir>(param, found);
                }
                    
            } else {
                dest.result = Error::InvalidArgs;
            }
        }

//        template<Direction Dir, class Comp, class Dest>
//        static void matchInexact(const Scalar & value, const ArrayPtr & array, Dest & dest) {
//
//            using Traits = DirectionTraits<Dir>;
//
//            auto extent = Traits::extentOf(array);
//            SizeType found = extent;
//            for(auto idx: DimensionRange(0, extent)) {
//                const Scalar & val = (*array)[Traits::comparisonPoint(idx)];
//                const auto ordering = Comp()(val, value);
//                if (ordering == std::partial_ordering::unordered)
//                    continue;
//                if (ordering <= 0) {
//                    found = idx;
//                }
//            }
//            if (found != extent) {
//                dest.template set<Dir>(array, found);
//            } else {
//                dest.result = Error::InvalidArgs;
//            }
//        }
//
//        template<Direction Dir, class Comp, class Dest>
//        static void matchInexact(const Scalar & value, const RectParam & param, Dest & dest)  {
//
//            using Traits = DirectionTraits<Dir>;
//
//            auto extent = Traits::extentOf(param);
//
//            bool noDependencies = true;
//            SizeType found = extent;
//            for(auto idx: DimensionRange(0, extent)) {
//                bool shouldContinue = true;
//                Point pt = param.rect.origin + asSize(Traits::comparisonPoint(idx));
//                param.context.evaluateCell(pt, [&](const Scalar & val) {
//                    if (!noDependencies)
//                        return;
//
//                    const auto ordering = Comp()(val, value);
//                    if (ordering == std::partial_ordering::unordered)
//                        return;
//
//                    if (ordering <= 0) {
//                        found = idx;
//                    }
//
//                }, [&](bool isCircular) {
//                    noDependencies = false;
//                    shouldContinue = !isCircular;
//                });
//                if (!shouldContinue)
//                    break;
//            }
//            if (!noDependencies)
//                dest.result = std::nullopt;
//            else if (found != extent)
//                dest.template set<Dir>(param, found);
//            else
//                dest.result = Error::InvalidArgs;
//        }
        
        template<Direction Dir, class Arg, class Dest>
        static void matchInexactForward(const Scalar & value, const Arg & arg, Dest & dest) {
            
            using Traits = DirectionTraits<Dir>;
            
            auto extent = Traits::extentOf(arg);
            auto range = DimensionRange{0, extent};
            auto extractor = Traits::extractorOf(arg);
            Compare compare;
            
            constexpr bool isNullable = std::is_same_v<decltype(extractor(0)), std::optional<Scalar>>;
            
            auto res = upperBound(range, extractor, value, compare);
            DimensionIterator * upper;
            if constexpr (isNullable) {
                if (!res) {
                    dest.result = std::nullopt;
                    return;
                }
                upper = &*res;
            } else {
                upper = &res;
            }
            
            for (auto it = reverse(*upper); it != range.rend(); ++it) {
                auto val = extractor(*it);
                Scalar * pval;
                if constexpr (isNullable) {
                    if (!res) {
                        dest.result = std::nullopt;
                        return;
                    }
                    pval = &*val;
                } else {
                    pval = &val;
                }
                auto compRes = compare(*pval, value);
                if (compRes == std::partial_ordering::unordered)
                    continue;
                if (compRes <= 0) {
                    dest.template set<Dir>(arg, *it);
                    return;
                }
                break;
            }
            
            dest.result = Error::InvalidArgs;
        }
        
        template<Direction Dir, class Arg, class Dest>
        static void matchInexactBackward(const Scalar & value, const Arg & arg, Dest & dest) {
            
            using Traits = DirectionTraits<Dir>;
            
            auto extent = Traits::extentOf(arg);
            auto range = ReverseDimensionRange{extent, 0};
            auto extractor = Traits::extractorOf(arg);
            Compare compare;
            
            constexpr bool isNullable = std::is_same_v<decltype(extractor(0)), std::optional<Scalar>>;
            
            auto res = upperBound(range, extractor, value, compare);
            ReverseDimensionIterator * upper;
            if constexpr (isNullable) {
                if (!res) {
                    dest.result = std::nullopt;
                    return;
                }
                upper = &*res;
            } else {
                upper = &res;
            }
            
            for (auto it = reverse(*upper), end = range.rend(); it != end; ++it) {
                auto val = extractor(*it);
                Scalar * pval;
                if constexpr (isNullable) {
                    if (!res) {
                        dest.result = std::nullopt;
                        return;
                    }
                    pval = &*val;
                } else {
                    pval = &val;
                }
                auto compRes = compare(*pval, value);
                if (compRes == std::partial_ordering::unordered)
                    continue;
                if (compRes == 0) {
                    dest.template set<Dir>(arg, *it);
                    return;
                }
                break;
            }
            
            for (auto it = *upper, end = range.end(); it != end; ++it) {
                auto val = extractor(*it);
                Scalar * pval;
                if constexpr (isNullable) {
                    if (!res) {
                        dest.result = std::nullopt;
                        return;
                    }
                    pval = &*val;
                } else {
                    pval = &val;
                }
                auto compRes = compare(*pval, value);
                if (compRes != std::partial_ordering::unordered) {
                    dest.template set<Dir>(arg, *it);
                    return;
                }
            }
                
            dest.result = Error::InvalidArgs;
        }
        
        
        template<class Range>
        struct Bisector {
            
            using SizeType = typename Range::size_type;
            using Iterator = typename Range::iterator;
            using ValueType = typename Range::value_type;
            
            struct FoundInfo {
                Iterator middle;
                std::strong_ordering result;
            };
            
            using FindResult = std::optional<FoundInfo>;
            
            template<class Extractor>
            using Result = std::conditional_t<std::is_invocable_r_v<Scalar, Extractor, ValueType>,
                                                    FindResult,
                                                    std::optional<FindResult>>;
            
            template<bool ShrinkUp, class Extractor, class Comp>
            auto bisect(Extractor extractor, const Scalar & value, Comp compare) -> Result<Extractor> {
                SPR_ASSERT_LOGIC(!this->range.empty());
                
                auto halfSize = this->range.size() / 2;
                auto mid = this->range.begin() + halfSize;
                
                for ( ; ; ) {
                    const auto & val = extractor(*mid);
                    const Scalar * pval;
                    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(val)>, std::optional<Scalar>>) {
                        if (!val)
                            return std::nullopt;
                        pval = &*val;
                    } else {
                        pval = &val;
                    }
                    
                    std::partial_ordering res = compare(*pval, value);
                    
                    if (res != std::partial_ordering::unordered) {
                        return FindResult{FoundInfo{mid, *reinterpret_cast<std::strong_ordering*>(&res)}};
                    }
                    
                    if constexpr (ShrinkUp) {
                        
                        if (mid != this->range.begin()) {
                            --mid;
                            continue;
                        }
                        
                        ++halfSize;
                        this->range = Range(this->range.begin() + halfSize, this->range.end());
                        
                    } else {
                        
                        if (++mid != this->range.end()) {
                            continue;
                        }
                        
                        this->range = Range(this->range.begin(), this->range.begin() + halfSize);
                    }
                    
                    if (this->range.empty()) {
                        return FindResult{};
                    }
                    
                    halfSize = this->range.size() / 2;
                    mid = this->range.begin() + halfSize;
                }
            }
            
            void moveAfter(Iterator it) {
                this->range = Range(it + 1, this->range.end());
            }
            void moveBefore(Iterator it) {
                this->range = Range(this->range.begin(), it);
            }
            
            Range range;
        };

//        template<class Comp, class Range, class Extractor>
//        requires(std::is_invocable_r_v<Scalar, Extractor, typename Range::value_type> ||
//                 std::is_invocable_r_v<std::optional<Scalar>, Extractor, typename Range::value_type>)
//        static auto lowerBound(Range range, Extractor extractor, const Scalar & value) ->
//            std::conditional_t<std::is_invocable_r_v<Scalar, Extractor, typename Range::value_type>,
//                                    typename Range::iterator, std::optional<typename Range::iterator>> {
//
//            using Bisector = Bisector<Range, Comp>;
//            using FindResult = typename Bisector::FindResult;
//
//            Bisector bisector{range};
//            while (!bisector.range.empty()) {
//                auto bisectRes = bisector.template bisect<true>(extractor, value);
//                FindResult * res;
//
//                if constexpr (std::is_same_v<std::optional<FindResult>, std::remove_cvref_t<decltype(bisectRes)>>) {
//                    if (!bisectRes) {
//                        return std::nullopt;
//                    }
//                    res = &*bisectRes;
//                } else {
//                    res = &bisectRes;
//                }
//                if (!*res) {
//                    SPR_ASSERT_LOGIC(bisector.range.empty());
//                    break;
//                }
//
//                auto [mid, result] = **res;
//
//                if (result == std::strong_ordering::less) {
//                    bisector.moveAfter(mid);
//                } else {
//                    bisector.moveBefore(mid);
//                }
//            }
//            return bisector.range.begin();
//        }
        
        template<class Range, class Extractor, class Comp>
        requires(std::is_invocable_r_v<Scalar, Extractor, typename Range::value_type> ||
                 std::is_invocable_r_v<std::optional<Scalar>, Extractor, typename Range::value_type>)
        static auto upperBound(Range range, Extractor extractor, const Scalar & value, Comp compare) ->
            std::conditional_t<std::is_invocable_r_v<Scalar, Extractor, typename Range::value_type>,
                                        typename Range::iterator, std::optional<typename Range::iterator>> {
            
            using Bisector = Bisector<Range>;
            using FindResult = typename Bisector::FindResult;
            
            Bisector bisector{range};
                
            while (!bisector.range.empty()) {
                auto bisectRes = bisector.template bisect<false>(extractor, value, compare);
                FindResult * res;
                
                if constexpr (std::is_same_v<std::optional<FindResult>, std::remove_cvref_t<decltype(bisectRes)>>) {
                    if (!bisectRes) {
                        return std::nullopt;
                    }
                    res = &*bisectRes;
                } else {
                    res = &bisectRes;
                }
                
                if (!*res) {
                    SPR_ASSERT_LOGIC(bisector.range.empty());
                    break;
                }
                
                auto [mid, result] = **res;
                
                if (result == std::strong_ordering::greater) {
                    bisector.moveBefore(mid);
                } else {
                    bisector.moveAfter(mid);
                }
            }
            return bisector.range.begin();
        }
        
//        template<class Comp, class Range, class Extractor>
//        requires(std::is_invocable_r_v<Scalar, Extractor, typename Range::value_type> ||
//                 std::is_invocable_r_v<std::optional<Scalar>, Extractor, typename Range::value_type>)
//        static auto equalRange(Range range, Extractor extractor, const Scalar & value) ->
//            std::conditional_t<std::is_invocable_r_v<Scalar, Extractor, typename Range::value_type>, Range, std::optional<Range>> {
//
//            using Bisector = Bisector<Range, Comp>;
//            using FindResult = typename Bisector::FindResult;
//
//            Bisector bisector{range};
//
//            while (!bisector.range.empty()) {
//                auto bisectRes = bisector.bisect(extractor, value);
//                FindResult * res;
//
//                if constexpr (std::is_same_v<std::optional<FindResult>, std::remove_cvref_t<decltype(bisectRes)>>) {
//                    if (!bisectRes) {
//                        return std::nullopt;
//                    }
//                    res = &*bisectRes;
//                } else {
//                    res = &bisectRes;
//                }
//
//                if (!*res) {
//                    SPR_ASSERT_LOGIC(bisector.range.empty());
//                    break;
//                }
//
//                auto [mid, result] = **res;
//
//                if (result == std::strong_ordering::less) {
//                    bisector.moveAfter(mid);
//                } else if (result == std::strong_ordering::greater) {
//                    bisector.moveBefore(mid);
//                } else {
//                    auto start = lowerBound<Comp>(Range{bisector.range.begin(), mid}, extractor, value);
//                    auto end = upperBound<Comp>(Range{mid + 1, bisector.range.end()}, extractor, value);
//                    if constexpr (std::is_same_v<decltype(start), std::optional<typename Range::iterator>>) {
//                        if (!start || !end)
//                            return std::nullopt;
//                        return Range{*start, *end};
//                    } else {
//                        return Range{start, end};
//                    }
//                }
//            }
//            return bisector.range;
//        }


        struct Match {
            auto operator()(const Scalar & test, const Scalar & value) -> bool {
                
                return applyVisitor([&](auto && lhs, auto && rhs)  {
                    
                    using LHS = std::remove_cvref_t<decltype(lhs)>;
                    using RHS = std::remove_cvref_t<decltype(rhs)>;
                    
                    if constexpr (std::is_same_v<LHS, RHS>) {
                        //happy case: same type
                        if constexpr (std::is_same_v<LHS, String>) {
                            
                            Wildcarder<String::storage_type, String::char_access::iterator, String::char_access::iterator> wildcarder('~');
                            return wildcarder.match(String::char_access(lhs), String::char_access(rhs));
                            
                        } else if constexpr (std::is_same_v<LHS, Number> || std::is_same_v<LHS, bool>) {
                            
                            return lhs == rhs;
                            
                        } else {
                            
                            return false;
                        }
                        
                    } else if constexpr (std::is_same_v<LHS, Number> && std::is_same_v<RHS, Scalar::Blank>) {
                        //number and blank works
                        return lhs == 0;
                    } else if constexpr (std::is_same_v<LHS, Scalar::Blank> && std::is_same_v<RHS, Number>) {
                        //blank and number works
                        return rhs == 0;
                    } else {
                        //otherwise no match
                        return false;
                    }
                    
                }, test, value);
            }
        };
        
        struct Compare {
            auto operator()(const Scalar & lhs, const Scalar & rhs) -> std::partial_ordering {
                return applyVisitor([&](auto && lhs, auto && rhs)  -> std::partial_ordering {
                    
                    using LHS = std::remove_cvref_t<decltype(lhs)>;
                    using RHS = std::remove_cvref_t<decltype(rhs)>;
                    
                    if constexpr (std::is_same_v<LHS, RHS>) {
                        //happy case: same type
                        if constexpr (std::is_same_v<LHS, Number> || std::is_same_v<LHS, String> || std::is_same_v<LHS, bool>)
                            return lhs <=> rhs;
                        else
                            return std::partial_ordering::unordered;
                    } else if constexpr (std::is_same_v<LHS, Number> && std::is_same_v<RHS, Scalar::Blank>) {
                        //number and blank works
                        return lhs <=> 0;
                    } else if constexpr (std::is_same_v<LHS, Scalar::Blank> && std::is_same_v<RHS, Number>) {
                        //blank and number works
                        return 0 <=> rhs;
                    } else {
                        //otherwise inconmensurate
                        return std::partial_ordering::unordered;
                    }
                    
                }, lhs, rhs);
            }
        };
        
//        struct ReverseCompare {
//            auto operator()(const Scalar & lhs, const Scalar & rhs) -> std::partial_ordering {
//                return Compare()(rhs, lhs);
//            }
//        };

    };

}


#endif 
