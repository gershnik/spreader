// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_ARRAY_H_INCLUDED
#define SPR_HEADER_ARRAY_H_INCLUDED

#include <spreader/scalar.h>
#include <spreader/geometry.h>
#include <spreader/compiler.h>

#include <new>

SPR_WARNING_PUSH

SPR_MSVC_WARNING_DISABLE(4324) //'Spreader::Array': structure was padded due to alignment specifier

namespace Spreader {

    class Array : public ref_counted<Array> {
    public:
        Array(const Array &) = delete;
        Array & operator=(const Array &) = delete;

        template<class Init>
        static auto create(Size size, Init init) -> refcnt_ptr<Array> {

            constexpr SizeType prefixSize = sizeof(Array) - sizeof(m_data);

            SPR_ASSERT_INPUT(size.width != 0 && size.height != 0);
            SPR_ASSERT_INPUT((std::numeric_limits<SizeType>::max() - prefixSize) / size.width >= size.height);

            auto * mem = ::operator new(prefixSize + (size.width * size.height) * sizeof(Scalar));
            auto ret = refcnt_attach(new (mem) Array(size));
            init(ret->begin());
            return ret;
        }

        template<SizeType Width, SizeType Height>
        static auto create(std::array<std::array<Scalar, Width>, Height> src) -> refcnt_ptr<Array> {

            Size size = {Width, Height};
            return create(size, [&src](Scalar * data) {
                Point pt;
                for(pt.y = 0; pt.y < Height; ++pt.y)
                    for(pt.x = 0; pt.x < Width; ++pt.x)
                        new (data + pt.y * Width + pt.x) Scalar(src[pt.y][pt.x]);
            });
        }

        auto size() const noexcept -> const Size & {
            return m_size;
        }
        
        auto begin() const noexcept -> const Scalar * {
            return reinterpret_cast<const Scalar *>(m_data);
        }
        auto begin() noexcept -> Scalar * {
            return reinterpret_cast<Scalar *>(m_data);
        }
        auto end() const noexcept -> const Scalar * {
            return reinterpret_cast<const Scalar *>(m_data) + m_size.width * m_size.height;
        }
        auto end() noexcept -> Scalar * {
            return reinterpret_cast<Scalar *>(m_data) + m_size.width * m_size.height;
        }

        auto operator[](Point pt) const noexcept -> const Scalar & {
            SPR_ASSERT_INPUT(pt.x < m_size.width && pt.y < m_size.height);
            return reinterpret_cast<const Scalar *>(m_data)[pt.y * m_size.width + pt.x];
        }
        auto operator[](Point pt) noexcept -> Scalar & {
            SPR_ASSERT_INPUT(pt.x < m_size.width && pt.y < m_size.height);
            return reinterpret_cast<Scalar *>(m_data)[pt.y * m_size.width + pt.x];
        }

        auto eval(Point pt) const noexcept -> Scalar {
            if (pt.x < m_size.width && pt.y < m_size.height)
                return reinterpret_cast<const Scalar *>(m_data)[pt.y * m_size.width + pt.x];
            else
                return Scalar(Error::InvalidArgs);
        }

        void reconstruct(StringBuilder & dest) const {
            dest.append(U'{');
            Point pt;
            for(pt.y = 0; pt.y < m_size.height; ++pt.y) {
                if (pt.y != 0) dest.append(U';');
                for(pt.x = 0; pt.x < m_size.width; ++pt.x) {
                    if (pt.x != 0) dest.append(U',');
                    (*this)[pt].reconstruct(dest);
                }
            }
            dest.append(U'}');
        }

        friend auto operator==(const Array & lhs, const Array & rhs) noexcept -> bool {
            if (lhs.m_size != rhs.m_size)
                return false;
            for (SizeType i = 0; i < lhs.m_size.width * lhs.m_size.height; ++i)
                if (lhs.begin()[i] != rhs.begin()[i])
                    return false;
            return true;
        }
        friend auto operator!=(const Array & lhs, const Array & rhs) noexcept -> bool {
            return !(lhs == rhs);
        }
    private:
        Array(Size size) noexcept : m_size(size) {
        }
    private:
        Size m_size;
        alignas(Scalar) uint8_t m_data[1];
    };
    using ArrayPtr = refcnt_ptr<Array>;
}

SPR_WARNING_POP

#endif
