// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_FORMULA_REFERENCES_H_INCLUDED
#define SPR_HEADER_FORMULA_REFERENCES_H_INCLUDED

#include <spreader/types.h>
#include <spreader/error-handling.h>
#include <spreader/geometry.h>
#include <spreader/reference.h>

#include <variant>
#include <iterator>

namespace Spreader {

    class FormulaReferences final : public ref_counted<FormulaReferences> {
        
        friend ref_counted;

    public:
        using Entry = AnyReference;

        using iterator = Entry *;
        using const_iterator = const Entry *;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using value_type = Entry;
        using reference = Entry &;
        using const_reference = const Entry &;
        using pointer = Entry *;
        using const_pointer = const Entry *;

    private:
        class Adjuster;

    public:
        static auto create(const std::vector<Entry> & src) -> refcnt_ptr<FormulaReferences> {

            if (!src.size())
                return nullptr;

            auto * mem = ::operator new(sizeof(FormulaReferences) + src.size() * sizeof(Entry) - 1);
            return refcnt_attach(new (mem) FormulaReferences(src));
        }
        auto clone() const -> refcnt_ptr<FormulaReferences> {

            auto * mem = ::operator new(sizeof(FormulaReferences) + m_count * sizeof(Entry) - 1);
            return refcnt_attach(new (mem) FormulaReferences(*this));
        }

        auto begin() const noexcept -> const_iterator { return entries(); }
        auto begin() noexcept -> iterator { return entries(); }
        auto cbegin() const noexcept -> const_iterator { return entries(); }
        auto end() const noexcept -> const_iterator { return entries() + m_count; }
        auto end() noexcept -> iterator { return entries() + m_count; }
        auto cend() const noexcept -> const_iterator { return entries() + m_count; }

        auto rbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{end()}; }
        auto rbegin() noexcept -> reverse_iterator { return reverse_iterator{end()}; }
        auto crbegin() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cend()}; }
        auto rend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{begin()}; }
        auto rend() noexcept -> reverse_iterator { return reverse_iterator{begin()}; }
        auto crend() const noexcept -> const_reverse_iterator { return const_reverse_iterator{cbegin()}; }

        auto size() const noexcept -> size_t { return m_count; }
        auto empty() const noexcept -> bool { return m_count == 0; }

        auto operator[](size_t idx) const noexcept -> const Entry & { return entries()[idx]; }
        auto operator[](size_t idx) noexcept -> Entry & { return entries()[idx]; }


        auto adjustToCopy(Point dest) const -> refcnt_ptr<const FormulaReferences>;
        auto adjustToMove(Point from, Point to) const -> refcnt_ptr<const FormulaReferences>;
        auto adjustToRowDeletion(SizeType at, SizeType y, SizeType count) const -> refcnt_ptr<const FormulaReferences>;
        auto adjustToColumnDeletion(SizeType at, SizeType x, SizeType count) const -> refcnt_ptr<const FormulaReferences>;
        auto adjustToRowInsertion(SizeType at, SizeType y, SizeType count) const -> refcnt_ptr<const FormulaReferences>;
        auto adjustToColumnInsertion(SizeType at, SizeType x, SizeType count) const -> refcnt_ptr<const FormulaReferences>;

    private:
        FormulaReferences(const std::vector<Entry> & src) noexcept :
            m_count(src.size()) {
            for(size_t i = 0; i < m_count; ++i) {
                new (entries() + i) Entry(src[i]);
            }
        }
        FormulaReferences(const FormulaReferences & src) noexcept:
            m_count(src.m_count) {
            
            for(size_t i = 0; i < m_count; ++i) {
                new (entries() + i) Entry(src[i]);
            }
        }
        ~FormulaReferences() noexcept = default;

        FormulaReferences & operator=(const FormulaReferences &) = delete;

        template<class Func>
        auto adjust(Func func) const -> refcnt_ptr<const FormulaReferences>;

        auto entries() const noexcept -> const Entry * { return reinterpret_cast<const Entry *>(&m_entriesStart); }
        auto entries() noexcept -> Entry * { return reinterpret_cast<Entry *>(&m_entriesStart); }

    private:
        size_t m_count;
        alignas(alignof(Entry)) std::byte m_entriesStart;
    };

    using FormulaReferencesPtr = refcnt_ptr<FormulaReferences>;
    using ConstFormulaReferencesPtr = refcnt_ptr<const FormulaReferences>;

    class FormulaReferencesBuilder {
    public:

        template<class RefType>
        requires(std::is_constructible_v<FormulaReferences::Entry, RefType &&>)
        auto add(RefType && ref) -> size_t {
            FormulaReferences::Entry entry(std::forward<RefType>(ref));
            auto it = std::find(m_entries.begin(), m_entries.end(), entry);
            if (it != m_entries.end())
                return it - m_entries.begin();
            m_entries.push_back(entry);
            return m_entries.size() - 1;
        }
        
        void reset() {
            m_entries.clear();
        }

        auto build() const -> FormulaReferencesPtr {
            return FormulaReferences::create(m_entries);
        }
    private:
        std::vector<FormulaReferences::Entry> m_entries;
    };
}


#endif
