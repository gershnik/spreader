// Copyright (c) 2022, Eugene Gershnik
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SPR_HEADER_EXECUTION_CONTEXT_H_INCLUDED
#define SPR_HEADER_EXECUTION_CONTEXT_H_INCLUDED

#include <spreader/formula-references.h>
#include <spreader/cell-grid.h>

namespace Spreader {

    class CellGrid;
    class NameManager;

    struct ExecutionContext {
    public:
        class DependencyHandler {
        public:
            virtual void addDependency(FormulaCell *) = 0;
        protected:
            DependencyHandler() = default;
            ~DependencyHandler() = default;
        };
        template<class T>
        class InvocableHandler : public DependencyHandler {
        public:
            InvocableHandler(T && invocable): m_invocable(std::move(invocable))
            {}
            InvocableHandler(const T & invocable): m_invocable(invocable)
            {}
            void addDependency(FormulaCell * fc) override {
                m_invocable(fc);
            }
        private:
            T m_invocable;
        };
    public:
        //input "registers"
        bool suppressEvaluation = false;
        bool circularDependency = false;
        Point offset{0, 0};
        AstNode::ExecutionStackEntry * stackPointer = nullptr;
        
        //output "registers"
        ScalarGenerator returnedValue;
        Size returnedExtent;
    
    private:
        //environment
        DependencyHandler * m_dependencyHandler = nullptr;
        CellGrid * m_grid;
        NameManager * m_names;
        const FormulaReferences * m_references;
        Point m_at;
        bool m_generation;
        
    public:
        
        ExecutionContext(CellGrid & grid,
                         NameManager & names,
                         const FormulaReferences * refs,
                         Point at,
                         bool generation):
            m_grid(&grid),
            m_names(&names),
            m_references(refs),
            m_at(at),
            m_generation(generation)
        {}
        
        auto grid() const noexcept -> CellGrid & { return *this->m_grid; }
        auto names() const noexcept -> NameManager & { return *this->m_names; }
        auto references() const noexcept -> const FormulaReferences & { return *this->m_references; }
        auto at() const noexcept -> Point { return this->m_at; }
        
        auto incrementOffset(Size extent) noexcept -> bool {
            SPR_ASSERT_LOGIC(this->offset.x < extent.width && this->offset.y < extent.height);
            
            if (this->offset.x < extent.width - 1) {
                ++this->offset.x;
                return true;
            }
            if (this->offset.y < extent.height - 1) {
                ++this->offset.y;
                this->offset.x = 0;
                return true;
            }
            return false;
        }
        
        void setDependencyHandler(DependencyHandler * depHandler) {
            m_dependencyHandler = depHandler;
        }
        
        template<class SuccessHandler, class DependencyHandler>
        SPR_ALWAYS_INLINE auto evaluateCell(Point pt, SuccessHandler onSuccess, DependencyHandler onDependency) {
            
            if (auto * cell = this->m_grid->getCell(pt)) {
                if (auto dependency = getRecalcDependency(cell, this->m_generation)) {
                    if (dependency->isCircularDependency()) {
                        this->circularDependency = true;
                        return onDependency(true);
                    } else {
                        this->m_dependencyHandler->addDependency(dependency);
                        return onDependency(false);
                    }
                }
                return onSuccess(cell->value());
            }
            return onSuccess(Scalar{});
        }
        
        SPR_ALWAYS_INLINE static auto generateScalar(const ArrayPtr & arr, Point off) -> Scalar {
            if (arr->size().width == 1)
                off.x = 0;
            if (arr->size().height == 1)
                off.y = 0;
            return arr->eval(off);
        }
        
        SPR_ALWAYS_INLINE auto generatePoint(Rect rect, Point off) -> std::optional<Point> {
            if (rect.size.width == 1) {
                if (rect.size.height == 1) { //optimization with less comparisons for a common 1x1 case
                    return rect.origin;
                }
                off.x = 0;
            }
            else if (rect.size.height == 1) {
                off.y = 0;
            }
            if (off.x < rect.size.width && off.y < rect.size.height) {
                auto clampedOff = Size {
                    std::min(m_grid->maxSize().width - rect.origin.x, off.x),
                    std::min(m_grid->maxSize().height - rect.origin.y, off.y)
                };
                Point pt = rect.origin + clampedOff;
                return pt;
            }
            return std::nullopt;
        }

        template<class Op>
        requires(std::is_invocable_v<Op, Scalar> || std::is_invocable_v<Op, Scalar, bool>)
        auto generateScalar(const ScalarGenerator & gen, Op op) -> bool {
            Point off = this->offset;
            return applyVisitor([&](const auto & val) -> bool {

                using T = std::remove_cvref_t<decltype(val)>;
                
                auto invoke = [&](auto && arg, [[maybe_unused]] bool isSingle) {
                    if constexpr (std::is_invocable_v<Op, Scalar, bool>)
                        op(std::forward<decltype(arg)>(arg), isSingle);
                    else
                        op(std::forward<decltype(arg)>(arg));
                };

                if constexpr (std::is_same_v<T, Scalar>) {
                    invoke(val, true);
                    return true;
                } else if constexpr (std::is_same_v<T, ArrayPtr>) {
                    invoke(generateScalar(val, off), false);
                    return true;
                } else if constexpr (std::is_same_v<T, Point>) {
                    
                    return evaluateCell(val, [&](const Scalar & cellVal) {
                            invoke(cellVal, true);
                            return true;
                        }, [](bool /*isCurcular*/) {
                            return false;
                        });
                    
                } else if constexpr (std::is_same_v<T, Rect>) {
                    if (auto validPoint = generatePoint(val, off)) {
                        return evaluateCell(*validPoint, [&](const Scalar & cellVal) {
                                invoke(cellVal, false);
                                return true;
                            }, [](bool /*isCurcular*/) {
                                return false;
                            });
                    }
                        
                    invoke(Error::InvalidArgs, false);
                    return true;
                    
                }

            }, gen);
        }
        
        void singlifyIfNeeded(ScalarGenerator & gen, Size size) {
            
            applyVisitor([&](const auto & val) {
                
                using T = std::remove_cvref_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, Scalar> || std::is_same_v<T, Point>) {
                    return;
                } else if constexpr (std::is_same_v<T, ArrayPtr>) {
                    
                    if (size == val->size())
                        return;
                    gen = generateScalar(val, this->offset);
                    
                } else if constexpr (std::is_same_v<T, Rect>) {
                    
                    if (size == val.size)
                        return;
                    
                    if (auto validPoint = generatePoint(val, this->offset))
                        gen = *validPoint;
                    else
                        gen = Error::InvalidArgs;
                    
                } else {
                    static_assert(dependentFalse<T>, "unhandled type");
                }
                
            }, gen);
        }

        enum class AggregateRectResult {
            Success,
            Aborted,
            HasDependencies
        };

        template<class Op>
        auto aggregateRect(Rect rect, Op op) -> AggregateRectResult {

            auto clampedSize = Size {
                std::min(this->m_grid->maxSize().width - rect.origin.x, rect.size.width),
                std::min(this->m_grid->maxSize().height - rect.origin.y, rect.size.height)
            };
            AggregateRectResult ret = AggregateRectResult::Success;
            Point pt;
            for(pt.y = rect.origin.y; pt.y < rect.origin.y + clampedSize.height; ++pt.y) {
                for(pt.x = rect.origin.x; pt.x < rect.origin.x + clampedSize.width; ++pt.x) {
                    
                    bool shouldContinue = evaluateCell(pt, [&](const Scalar & cellVal) {
                        if ((ret == AggregateRectResult::Success) && !op(cellVal)) {
                            ret = AggregateRectResult::Aborted;
                            return false;
                        }
                        return true;
                    }, [&](bool isCircular) {
                        ret = AggregateRectResult::HasDependencies;
                        return !isCircular;
                    });
                    if (!shouldContinue)
                        return ret;
                    
                }
            }
            return ret;
        }

        template<class Op>
        auto aggregateRects(Rect main, Rect second, Op op) -> AggregateRectResult {

            auto clampedMainSize = Size {
                std::min(this->m_grid->maxSize().width - main.origin.x, main.size.width),
                std::min(this->m_grid->maxSize().height - main.origin.y, main.size.height)
            };
            auto clampedSecondSize = Size {
                std::min(this->m_grid->maxSize().width - second.origin.x, second.size.width),
                std::min(this->m_grid->maxSize().height - second.origin.y, second.size.height)
            };
            auto commonSize = Size {
                std::min(clampedMainSize.width, clampedSecondSize.width),
                std::min(clampedMainSize.height, clampedSecondSize.height)
            };
            bool hasDependencies = false;
            Size off;
            for(off.height = 0; off.height < commonSize.height; ++off.height) {

                for(off.width = 0; off.width < commonSize.width; ++off.width) {
                    auto * mainCell = this->m_grid->getCell(main.origin + off);
                    auto * secondCell = this->m_grid->getCell(second.origin + off);

                    if (mainCell) {
                        if (auto dependency = getRecalcDependency(mainCell, this->m_generation)) {
                            if (dependency->isCircularDependency()) {
                                this->circularDependency = true;
                                return AggregateRectResult::HasDependencies;
                            }
                            this->m_dependencyHandler->addDependency(dependency);
                            hasDependencies = true;
                        }
                    }
                    if (secondCell) {
                        if (auto dependency = getRecalcDependency(secondCell, this->m_generation)) {
                            if (dependency->isCircularDependency()) {
                                this->circularDependency = true;
                                return AggregateRectResult::HasDependencies;
                            }
                            this->m_dependencyHandler->addDependency(dependency);
                            hasDependencies = true;
                        }
                    }

                    if (!hasDependencies) {

                        if (!op(mainCell ? mainCell->value() : Scalar(), secondCell ? secondCell->value() : Scalar()))
                            return AggregateRectResult::Aborted;
                    }
                }

                for( ; off.width < clampedMainSize.width; ++off.width) {

                    auto * mainCell = this->m_grid->getCell(main.origin + off);
                    if (mainCell) {
                        if (auto dependency = getRecalcDependency(mainCell, this->m_generation)) {
                            if (dependency->isCircularDependency()) {
                                this->circularDependency = true;
                                return AggregateRectResult::HasDependencies;
                            }
                            this->m_dependencyHandler->addDependency(dependency);
                            hasDependencies = true;
                        }
                    }

                    if (!hasDependencies) {

                        if (!op(mainCell ? mainCell->value() : Scalar(), Scalar()))
                            return AggregateRectResult::Aborted;
                    }
                }
            }
            for ( ; off.height < clampedMainSize.height; ++off.height) {
                for(off.width = 0; off.width < clampedMainSize.width; ++off.width) {

                    auto * mainCell = this->m_grid->getCell(main.origin + off);
                    if (mainCell) {
                        if (auto dependency = getRecalcDependency(mainCell, this->m_generation)) {
                            if (dependency->isCircularDependency()) {
                                this->circularDependency = true;
                                return AggregateRectResult::HasDependencies;
                            }
                            this->m_dependencyHandler->addDependency(dependency);
                            hasDependencies = true;
                        }
                    }

                    if (!hasDependencies) {

                        if (!op(mainCell ? mainCell->value() : Scalar(), Scalar()))
                            return AggregateRectResult::Aborted;
                    }
                }
            }

            return hasDependencies ? AggregateRectResult::HasDependencies : AggregateRectResult::Success;
        }
    };
    
    static_assert(std::is_move_assignable_v<ExecutionContext> && std::is_move_constructible_v<ExecutionContext>,
                  "ExecutionContext must be movable");
}

#endif  
