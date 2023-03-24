// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_INTERVAL_MAP_H
#define SPR_HEADER_INTERVAL_MAP_H

#include <spreader/error-handling.h>
#include <spreader/coro-generator.h>

#include <concepts>
#include <compare>
#include <set>
#include <optional>

#if SPR_TESTING
    #include <string>
#endif

namespace Spreader {

    template<class T>
    concept IntervalMapIndex = std::semiregular<T> && std::totally_ordered<T> && requires(T a, T b) {

        {a - b} -> std::integral;
        a += std::declval<decltype(a - b)>();
        a += std::declval<std::make_unsigned_t<decltype(a - b)>>();
        a -= std::declval<decltype(a - b)>();
        a -= std::declval<std::make_unsigned_t<decltype(a - b)>>();
    };

    template <class T>
    concept IntervalMapValue = std::copyable<T> && std::equality_comparable<T>;

    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    class IntervalMap {

    private:
        struct Interval {
            Interval(IndexType startIdx_, IndexType endIdx_, ValueType value_):
                startIdx(startIdx_),
                endIdx(endIdx_),
                value(value_)
            {}

            IndexType startIdx;
            mutable IndexType endIdx;
            mutable ValueType value;

            friend auto operator<=>(const Interval & lhs, const Interval & rhs) -> std::strong_ordering {
                return lhs.startIdx <=> rhs.startIdx;
            }
            friend auto operator<=>(const Interval & lhs, IndexType rhs) -> std::strong_ordering {
                return lhs.startIdx <=> rhs;
            }
            friend auto operator<=>(IndexType lhs, const Interval & rhs) -> std::strong_ordering {
                return lhs <=> rhs.startIdx;
            }
        };
        using IntervalsByStart = std::set<Interval, std::less<>>;
        using IntervalIterator = typename IntervalsByStart::iterator;

    public:
        using IndexDifferenceType = decltype(std::declval<IndexType>() - std::declval<IndexType>());
        using IndexSizeType = std::make_unsigned_t<IndexDifferenceType>;
        
    public:
        auto getValue(IndexType idx) const noexcept -> const ValueType * {
            return getValueWithHandler(idx, PointerValueHandler{});
        }
        
        auto getValueWithDefault(IndexType idx, const ValueType & defaultValue) const noexcept -> const ValueType & {
            return getValueWithHandler(idx, DefaultValueHandler{defaultValue});
        }

        void setValue(IndexType idx, const ValueType & value) {
            setValue(idx, idx + 1, value);
        }
        void setValue(IndexType startIdx, IndexType endIdx, const ValueType & value);

        void clearValue(IndexType startIdx) {
            clearValue(startIdx, startIdx + 1);
        }
        void clearValue(IndexType startIdx, IndexType endIdx);

        template<class Op>
        void modifyValue(IndexType startIdx, IndexType endIdx, Op && op);

        void insertIndices(IndexType beforeIdx, IndexSizeType count);
        void eraseIndices(IndexType startIdx, IndexType endIdx);

        template<class Func>
        requires(std::is_invocable_r_v<void, Func, IndexType, IndexType, const ValueType *> ||
                 std::is_invocable_r_v<bool, Func, IndexType, IndexType, const ValueType *>)
        auto forEachInterval(IndexType startIdx, IndexType endIdx, Func && func) const ->
            std::conditional_t<std::is_invocable_r_v<bool, Func, IndexType, IndexType, const ValueType *>, bool, void> {
            
            return forEachIntervalWithHandler(startIdx, endIdx, PointerValueHandler{}, std::forward<Func>(func));
        }

        template<class Func>
        requires(std::is_invocable_r_v<void, Func, IndexType, IndexType, const ValueType &> ||
                 std::is_invocable_r_v<bool, Func, IndexType, IndexType, const ValueType &>)
        auto forEachIntervalWithDefault(IndexType startIdx, IndexType endIdx, const ValueType & defaultValue, Func && func) const ->
            std::conditional_t<std::is_invocable_r_v<bool, Func, IndexType, IndexType, const ValueType &>, bool, void> {
            
            return forEachIntervalWithHandler(startIdx, endIdx, DefaultValueHandler{defaultValue}, std::forward<Func>(func));
        }

        auto intervalsGenerator(IndexType startIdx, IndexType endIdx, const ValueType & defaultValue) const -> 
                CoroGenerator<std::tuple<IndexType, IndexType, const ValueType &>>;

        #if SPR_TESTING

            auto toString() const -> std::string {

                std::string ret;
                for(auto & interval: m_intervals) {
                    ret += '[';
                    ret += std::to_string(interval.startIdx);
                    ret += ", ";
                    ret += std::to_string(interval.endIdx);
                    ret += ", ";
                    ret += std::to_string(interval.value);
                    ret += ']';
                }
                return ret;
            }


        #endif

    private:
        struct PointerValueHandler {
          
            auto operator()(std::nullptr_t) const -> const ValueType * { return nullptr; }
            auto operator()(const ValueType & val) const -> const ValueType * { return &val; }
        };
        struct DefaultValueHandler {
            
            const ValueType & defaultValue;
          
            auto operator()(std::nullptr_t) const -> const ValueType & { return defaultValue; }
            auto operator()(const ValueType & val) const -> const ValueType & { return val; }
        };

        auto getValueWithHandler(IndexType idx, auto handler) const noexcept -> decltype(handler(nullptr)) {

            auto it = m_intervals.lower_bound(idx);
            if (it != m_intervals.end() && idx == it->startIdx)
                return handler(it->value);
            if (it != m_intervals.begin() && idx < (--it)->endIdx) {
                return handler(it->value);
            }
            return handler(nullptr);
        }
        
        //Moronic GCC has problems with delctype in trailing return type
        //https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105908
        template<class Handler>
        using HandlerRetType = decltype(std::declval<Handler>()(nullptr));

        template<class Handler, class Func>
        auto forEachIntervalWithHandler(IndexType startIdx, IndexType endIdx, Handler handler, Func && func) const ->
            std::conditional_t<std::is_invocable_r_v<bool, Func, IndexType, IndexType, HandlerRetType<Handler>>, bool, void>
            requires(std::is_invocable_r_v<void, Func, IndexType, IndexType, decltype(handler(nullptr))> ||
                     std::is_invocable_r_v<bool, Func, IndexType, IndexType, decltype(handler(nullptr))>);
        
        
        template<class Val>
        requires(std::is_same_v<std::remove_cvref_t<Val>, ValueType>)
        auto insertOrMergeLeft(IntervalIterator next, IndexType startIdx, IndexType endIdx, Val && value) -> IntervalIterator {
            
            if (next != m_intervals.begin()) {
                auto prev = next;
                --prev;
                SPR_ASSERT_LOGIC(prev->startIdx < startIdx);
                if (prev->endIdx == startIdx && prev->value == value) {
                    prev->endIdx = endIdx;
                    return prev;
                }
            }
            
            return m_intervals.emplace_hint(next, startIdx, endIdx, std::forward<Val>(value));
        }
        
        template<class Val>
        requires(std::is_same_v<std::remove_cvref_t<Val>, ValueType>)
        auto insertOrMergeBothEnds(IntervalIterator next, IndexType startIdx, IndexType endIdx, Val && value) -> IntervalIterator {
            
            if (next != m_intervals.begin()) {
                auto prev = next;
                --prev;
                SPR_ASSERT_LOGIC(prev->startIdx < startIdx);
                if (prev->endIdx == startIdx && prev->value == value) {
                    prev->endIdx = endIdx;
                    
                    if (next != m_intervals.end()) {
                        if (next->startIdx == endIdx && next->value == value) {
                            prev->endIdx = next->endIdx;
                            m_intervals.erase(next);
                        }
                    }
                    
                    return prev;
                }
            }
            
            if (next != m_intervals.end()) {
                SPR_ASSERT_LOGIC(next->startIdx >= endIdx);
                if (next->startIdx == endIdx && next->value == value) {
                    
                    auto node = m_intervals.extract(next++);
                    node.value().startIdx = startIdx;
                    next = m_intervals.insert(next, std::move(node));
                    return next;
                }
            }
            
            return m_intervals.emplace_hint(next, startIdx, endIdx, std::forward<Val>(value));
        }
        
        
        auto mergeIfPossible(IntervalIterator it) -> IntervalIterator {
            
            SPR_ASSERT_LOGIC(it != m_intervals.end());

            if (it != m_intervals.end()) {
                auto next = it;
                ++next;

                if (next != m_intervals.end() && it->endIdx == next->startIdx && it->value == next->value) {
                    it->endIdx = next->endIdx;
                    m_intervals.erase(next);
                }
            }

            if (it != m_intervals.begin()) {
                auto prev = it;
                --prev;
                if (prev->endIdx == it->startIdx && prev->value == it->value) {
                    prev->endIdx = it->endIdx;
                    m_intervals.erase(it);
                    return prev;
                }
            }
            return it;
        }
        
    private:
        IntervalsByStart m_intervals;
    };


    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    void IntervalMap<IndexType, ValueType>::setValue(IndexType startIdx, IndexType endIdx, const ValueType & value) {

        SPR_ASSERT_INPUT(startIdx <= endIdx);

        if (startIdx == endIdx)
            return;

        auto it = m_intervals.lower_bound(startIdx);
        
        if (it != m_intervals.end() && startIdx == it->startIdx) {
            
            //if we found exact start
            
            //and the entire range fits in this entry let's handle it here and return
            if (endIdx <= it->endIdx) {
                
                if (static_cast<const ValueType &>(value) == it->value)
                    return;
                
                if (endIdx == it->endIdx) {
                    it->value = value;
                    mergeIfPossible(it);
                    return;
                }
                
                //truncate it and carve out new entry on its left
                auto current = it++;
                auto node = m_intervals.extract(current);
                node.value().startIdx = endIdx;
                it = m_intervals.insert(it, std::move(node));
                insertOrMergeLeft(it, startIdx, endIdx, value);
                return;
            }
            
        } else if (it != m_intervals.begin()) {
            
            //otherwise if we have something on the left

            //and startIdx falls into it
            if (auto prev = it; startIdx < (--prev)->endIdx) {

                //if we are making a simple insert in prev, carve it (if different) and return
                if (prev->endIdx > endIdx) {
                    if (static_cast<const ValueType &>(value) == prev->value)
                        return;
                    Interval old = *prev;
                    auto node = m_intervals.extract(prev);
                    node.value().startIdx = endIdx;
                    prev = m_intervals.insert(it, std::move(node));
                    prev = m_intervals.emplace_hint(prev, startIdx, endIdx, value);
                    m_intervals.emplace_hint(prev, old.startIdx, startIdx, old.value);
                    return;
                }

                //truncate prev and proceed to handle right side
                prev->endIdx = startIdx;
            }
        }

        //when we are here it means that startIdx is either at the beginning of 'it'
        //or in the empty space before it

        //remove any ranges wholy covered by the new entry
        while(it != m_intervals.end() && endIdx >= it->endIdx) {
            it = m_intervals.erase(it);
        }
    
        //truncate the remaining partial covered range
        if (it != m_intervals.end() && endIdx > it->startIdx) {
            
            auto node = m_intervals.extract(it++);
            node.value().startIdx = endIdx;
            it = m_intervals.insert(it, std::move(node));
        }
            
        insertOrMergeBothEnds(it, startIdx, endIdx, value);
    }

    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    void IntervalMap<IndexType, ValueType>::clearValue(IndexType startIdx, IndexType endIdx) {

        SPR_ASSERT_INPUT(startIdx <= endIdx);

        if (startIdx == endIdx)
            return;

        auto it = m_intervals.lower_bound(startIdx);

        //if it doesn't match startIdx and there is something on the left
        if ((it == m_intervals.end() || startIdx != it->startIdx) && it != m_intervals.begin()) {

            //and startIdx falls into one on the left
            if (auto prev = it; startIdx < (--prev)->endIdx) {

                //if we are making a simple hole in prev, punch it and return
                if (prev->endIdx > endIdx) {
                    
                    Interval old = *prev;
                    auto node = m_intervals.extract(prev);
                    node.value().startIdx = endIdx;
                    prev = m_intervals.insert(it, std::move(node));
                    m_intervals.emplace_hint(prev, old.startIdx, startIdx, old.value);
                    return;
                }

                //truncate prev and proceed to handle right side
                prev->endIdx = startIdx;
            }
        }

        //when we are here it means that startIdx is either at the beginning of 'it'
        //or in the empty space before it

        //remove any ranges wholy covered by the clearance
        while(it != m_intervals.end() && endIdx >= it->endIdx) {
            it = m_intervals.erase(it);
        }

        //truncate the partial range on the right if any
        if (it != m_intervals.end() && endIdx > it->startIdx) {
            auto node = m_intervals.extract(it++);
            node.value().startIdx = endIdx;
            it = m_intervals.insert(it, std::move(node));
        }
    }

    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    template<class Op>
    void IntervalMap<IndexType, ValueType>::modifyValue(IndexType startIdx, IndexType endIdx, Op && op) {
        
        SPR_ASSERT_INPUT(startIdx <= endIdx);

        if (startIdx == endIdx)
            return;

        auto it = m_intervals.lower_bound(startIdx);
        
        //if we have something on the left and startIdx falls into it
        if ((it == m_intervals.end() || startIdx != it->startIdx) && it != m_intervals.begin()) {
            if (auto prev = it; startIdx < (--prev)->endIdx) {
                
                std::optional<ValueType> outcome = std::forward<Op>(op)(const_cast<const ValueType *>(&prev->value));

                //if we are making a simple insert in prev, carve it and return
                if (prev->endIdx > endIdx) {
                    
                    if (outcome && *outcome == prev->value)
                        return;
                    
                    Interval old = *prev;
                    auto node = m_intervals.extract(prev);
                    node.value().startIdx = endIdx;
                    prev = m_intervals.insert(it, std::move(node));
                    if (outcome)
                        prev = m_intervals.emplace_hint(prev, startIdx, endIdx, *outcome);
                    m_intervals.emplace_hint(prev, old.startIdx, startIdx, old.value);
                    return;
                }

                
                if (outcome && *outcome == prev->value) {
                    //if its the same value proceed from after this one
                    startIdx = prev->endIdx;
                } else {
                    //otherwise truncate prev and proceed to handle right side
                    prev->endIdx = startIdx;
                }
            }
        }
        
        //when we are here it means that startIdx is either at the beginning of 'it'
        //or in the empty space before it
        while (startIdx < endIdx) {
            
            if (it == m_intervals.end()) {
                //we are to the right of all entries
                
                if (std::optional<ValueType> outcome = std::forward<Op>(op)(nullptr)) {
                    insertOrMergeLeft(it, startIdx, endIdx, std::move(*outcome));
                }
                break;
            
            } else if (startIdx < it->startIdx) {
                //we are to the left of next entry
                
                IndexType nextIdx = std::min(endIdx, it->startIdx);
                if (std::optional<ValueType> outcome = std::forward<Op>(op)(nullptr)) {
                    it = insertOrMergeBothEnds(it, startIdx, nextIdx, std::move(*outcome));
                    nextIdx = it->endIdx;
                    ++it;
                }
                startIdx = nextIdx;
                
            } else if (endIdx >= it->endIdx) {
                
                //we are in an entry and the range covers it completely
                IndexType nextIdx = it->endIdx;
                if (std::optional<ValueType> outcome = std::forward<Op>(op)(const_cast<const ValueType *>(&it->value))) {
                    if (*outcome != it->value) {
                        it->value = std::move(*outcome);
                        it = mergeIfPossible(it);
                        nextIdx = it->endIdx;
                    }
                    ++it;
                } else {
                    it = m_intervals.erase(it);
                }
                startIdx = nextIdx;
                
            } else {
                
                //we are in an entry and the range covers it partially => this is the last one
                
                auto node = m_intervals.extract(it++);
                node.value().startIdx = endIdx;
                it = m_intervals.insert(it, std::move(node));
                
                if (std::optional<ValueType> outcome = std::forward<Op>(op)(const_cast<const ValueType *>(&it->value))) {
                    insertOrMergeBothEnds(it, startIdx, endIdx, std::move(*outcome));
                }
                
                break;
            }
        }
    }
    
    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    void IntervalMap<IndexType, ValueType>::insertIndices(IndexType beforeIdx, IndexSizeType count) {
        
        if (count == 0)
            return;

        auto it = m_intervals.lower_bound(beforeIdx);
        if (it != m_intervals.end() && beforeIdx == it->startIdx) {
            
            //if we have exact hit extend current interval and proceed

            it->endIdx += count;
            ++it;

        } else if (it != m_intervals.begin()) {
            
            //if the hit is in previous extend it and proceed
            if (auto prev = it; beforeIdx < (--prev)->endIdx) {
                prev->endIdx += count;
            }
        }

        //for all the intervals to the right - move them
        
        IntervalsByStart temp;
        while(it != m_intervals.end()) {
            
            auto current = it++;
            auto node = m_intervals.extract(current);
            auto & interval = node.value();
            interval.startIdx += count;
            interval.endIdx += count;
            temp.insert(std::move(node));
        }
        m_intervals.merge(temp);
    }
    
    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    void IntervalMap<IndexType, ValueType>::eraseIndices(IndexType startIdx, IndexType endIdx) {
        
        SPR_ASSERT_INPUT(startIdx <= endIdx);
        
        if (startIdx == endIdx)
            return;
        
        auto count = endIdx - startIdx;
        
        auto it = m_intervals.lower_bound(startIdx);

        //if it doesn't match startIdx and there is something on the left
        if ((it == m_intervals.end() || startIdx != it->startIdx) && it != m_intervals.begin()) {

            //and startIdx falls into one on the left
            if (auto prev = it; startIdx < (--prev)->endIdx) {

                //if we are making a simple hole in prev, erase it
                if (prev->endIdx > endIdx) {
                    prev->endIdx -= count;
                } else {
                    //otherwise truncate prev and advance
                    prev->endIdx = startIdx;
                }
            }
        }

        //when we are here it means that startIdx is either at the beginning of 'it'
        //or in the empty space before it
        

        //remove any ranges wholy covered by the erasure
        while(it != m_intervals.end() && endIdx >= it->endIdx) {
            it = m_intervals.erase(it);
        }

        //truncate the partial range on the right if any
        if (it != m_intervals.end() && endIdx > it->startIdx) {
            auto current = it++;
            auto node = m_intervals.extract(current);
            node.value().startIdx = startIdx;  //this is where the erased hole starts
            node.value().endIdx -= count;
            m_intervals.insert(it, std::move(node));
        }
        
        //for all the intervals to the right - move them
        IntervalsByStart temp;
        while(it != m_intervals.end()) {
            
            auto current = it++;
            auto node = m_intervals.extract(current);
            auto & interval = node.value();
            interval.startIdx -= count;
            interval.endIdx -= count;
            temp.insert(std::move(node));
        }
        m_intervals.merge(temp);
    }
    
    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    template<class Handler, class Func>
    auto IntervalMap<IndexType, ValueType>::forEachIntervalWithHandler(IndexType startIdx, IndexType endIdx, Handler handler, Func && func) const ->
        std::conditional_t<std::is_invocable_r_v<bool, Func, IndexType, IndexType, HandlerRetType<Handler>>, bool, void>
    
        requires(std::is_invocable_r_v<void, Func, IndexType, IndexType, decltype(handler(nullptr))> ||
                 std::is_invocable_r_v<bool, Func, IndexType, IndexType, decltype(handler(nullptr))>)
    {

        SPR_ASSERT_INPUT(startIdx <= endIdx);
        
        constexpr bool returnsBool = std::is_invocable_r_v<bool, Func, IndexType, IndexType, HandlerRetType<Handler>>;

        auto it = m_intervals.lower_bound(startIdx);

        if ((it == m_intervals.end() || startIdx != it->startIdx) && it != m_intervals.begin()) {
            if (auto prev = it; startIdx < (--prev)->endIdx)
                it = prev;
        }

        for (IndexType nextIdx; startIdx != endIdx; startIdx = nextIdx) {

            if (it == m_intervals.end()) {
                //we are to the right of all entries: use default to the end
                nextIdx = endIdx;
                if constexpr (returnsBool) {
                    if (!std::forward<Func>(func)(startIdx, nextIdx, handler(nullptr)))
                        return false;
                } else {
                    std::forward<Func>(func)(startIdx, nextIdx, handler(nullptr));
                }
            } else if (startIdx < it->startIdx) {
                //we are to the left of next entry: use default
                nextIdx = std::min(endIdx, it->startIdx);
                if constexpr (returnsBool) {
                    if (!std::forward<Func>(func)(startIdx, nextIdx, handler(nullptr)))
                        return false;
                } else {
                    std::forward<Func>(func)(startIdx, nextIdx, handler(nullptr));
                }
            } else {
                //we are in an entry: use its value
                nextIdx = std::min(endIdx, it->endIdx);
                if constexpr (returnsBool) {
                    if (!std::forward<Func>(func)(startIdx, nextIdx, handler(it->value)))
                        return false;
                } else {
                    std::forward<Func>(func)(startIdx, nextIdx, handler(it->value));
                }
                ++it;
            }
        }

        if constexpr (returnsBool)
            return true;
    }

    template<IntervalMapIndex IndexType, IntervalMapValue ValueType>
    auto IntervalMap<IndexType, ValueType>::intervalsGenerator(IndexType startIdx, IndexType endIdx, const ValueType & defaultValue) const -> 
        CoroGenerator<std::tuple<IndexType, IndexType, const ValueType &>> {

        SPR_ASSERT_INPUT(startIdx <= endIdx);

        using RetType = std::tuple<IndexType, IndexType, const ValueType &>;

        auto it = m_intervals.lower_bound(startIdx);

        if ((it == m_intervals.end() || startIdx != it->startIdx) && it != m_intervals.begin()) {
            if (auto prev = it; startIdx < (--prev)->endIdx)
                it = prev;
        }

        for (IndexType nextIdx; startIdx != endIdx; startIdx = nextIdx) {

            if (it == m_intervals.end()) {
                //we are to the right of all entries: use default to the end
                nextIdx = endIdx;
                co_yield RetType(startIdx, nextIdx, defaultValue);
            } else if (startIdx < it->startIdx) {
                //we are to the left of next entry: use default
                nextIdx = std::min(endIdx, it->startIdx);
                co_yield RetType(startIdx, nextIdx, defaultValue);
            } else {
                //we are in an entry: use its value
                nextIdx = std::min(endIdx, it->endIdx);
                co_yield RetType(startIdx, nextIdx, it->value);
                ++it;
            }
        }
    }

}


#endif 
