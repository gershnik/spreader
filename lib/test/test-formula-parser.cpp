#include "../code/src/formula-evaluator.h"
#include <spreader/name-manager.h>


#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

namespace {
    struct SetCell {
        auto modifiesMissing() noexcept -> bool { return bool(value); }
        auto operator()(CellPtr & cell) noexcept -> int {
            int ret = -int(bool(cell)) + int(bool(value));
            cell = std::move(value);
            return ret;
        }
        CellPtr value;
    };
}


TEST_CASE( "Formula Parser", "[formula]" ) {

    AstNode::ExecutionMemoryResource memory;
    CellGrid grid;
    NameManager nm;

    {
        auto [f, refs] = Formula::parse(SPRS("  1.234 -.45E7"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("1.234 - 4500000"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        REQUIRE(fev.extent() == Spreader::Size{1, 1});
        REQUIRE(fev.result() == SPRN(-4499998.766));
    }

    {
        auto [f, refs] = Formula::parse(SPRS("+5."), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("5"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        REQUIRE(fev.extent() == Spreader::Size{1, 1});
        REQUIRE(fev.result() == 5.);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("+5. + 000.23"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("5 + 0.23"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 5.23);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("+5. + -000.23"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("5 + -0.23"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 5 - .23);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("\"a😃\"\"c\""), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("\"a😃\"\"c\""));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == SPRS("a😃\"c"));
    }
    
    {
        auto [f, refs] = Formula::parse(SPRS("E1 + 3"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("F6 + 3"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        grid.modifyCell({4, 0}, SetCell{ValueCell::create(5.)});
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 8.);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("$C$1 + 3"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("$C$1 + 3"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 3.);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("6.2 * {1 , 2,3}"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("6.2 * {1,2,3}"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{3, 1});
        CHECK(fev.result() == 6.2);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("SUM(1,2, 5, E1:C3)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("SUM(1, 2, 5, F6:D8)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 13.);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("SUM(1 + E1:C3)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("SUM(1 + F6:D8)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 14.); //(1 + 0) * 8 + (1 + 5)
    }

    {
        auto [f, refs] = Formula::parse(SPRS("SUM(12, #REF!, 3)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("SUM(12, #REF!, 3)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == Scalar(Spreader::Error::InvalidReference));
    }

    {
        auto [f, refs] = Formula::parse(SPRS("SUM(12, \"abc\", 3)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("SUM(12, \"abc\", 3)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == Scalar(Spreader::Error::InvalidValue));
    }

    {
        auto [f, refs] = Formula::parse(SPRS("IF(1, 2, 5)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("IF(1, 2, 5)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 2.);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("IF(0, 2, 5)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("IF(0, 2, 5)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 5.);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("IF(\"abc\", 2, 5)"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("IF(\"abc\", 2, 5)"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == Error::InvalidValue);
    }

    {
        auto [f, refs] = Formula::parse(SPRS("(2+3)*4"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("(2 + 3) * 4"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        CHECK(fev.extent() == Spreader::Size{1, 1});
        CHECK(fev.result() == 20.);
    }
    
    {
        auto [f, refs] = Formula::parse(SPRS("1 + 17976931348623158"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("1 + 17976931348623158"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        REQUIRE(fev.extent() == Spreader::Size{1, 1});
        REQUIRE(fev.result() == Error::InvalidFormula);
    }
    
    {
        auto [f, refs] = Formula::parse(SPRS("1 + 1.7976931348623157E309"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("1 + 1.7976931348623157E309"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        REQUIRE(fev.extent() == Spreader::Size{1, 1});
        REQUIRE(fev.result() == Error::InvalidFormula);
    }
    
    {
        auto [f, refs] = Formula::parse(SPRS("1 + 1.7976931348623157E308"), {0, 0});
        CHECK(f->reconstructAt(refs, {1,5}) == SPRS("1 + 1.7976931348623157E308"));
        FormulaEvaluator fev(memory, grid, nm, f, refs, {0, 0}, false);
        REQUIRE(fev.eval());
        REQUIRE(fev.extent() == Spreader::Size{1, 1});
        REQUIRE(fev.result() == 1.7976931348623157E308);
    }

}
