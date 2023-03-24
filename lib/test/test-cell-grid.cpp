#include <spreader/cell-grid.h>


#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

using namespace Spreader;

static_assert(CellGrid::maxSize() == Spreader::Size{(1u << 16) - 1, (1u << 31) - 1});

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

TEST_CASE( "Empty grid works", "[cell_grid]" ) {

    CellGrid grid;

    REQUIRE(grid.size() == Spreader::Size{0, 0});

    grid.modifyCell({0, 0}, SetCell{});
    REQUIRE(grid.size() == Spreader::Size{1, 1});
    REQUIRE(grid.getCell({0, 0}) == nullptr);

    grid.modifyCell({0, 0}, SetCell{ValueCell::create(1.)});
    REQUIRE(grid.size() == Spreader::Size{1, 1});
    REQUIRE(grid.getCell({0, 0})->value() == 1.);

}

TEST_CASE( "Single cell access", "[cell_grid]" ) {

    CellGrid grid;

    grid.modifyCell({42, 642}, SetCell{ValueCell::create(0.)});
    REQUIRE(grid.size() == Spreader::Size{43, 643});
    REQUIRE(grid.getCell({42, 642})->value() == 0.);
    REQUIRE(grid.getCell({42, 641}) == nullptr);
    REQUIRE(grid.getCell({41, 642}) == nullptr);

    CellPtr cell = ValueCell::create(SPRS("abc"));
    Cell * cellPtr = cell.get();
    grid.modifyCell({13, 25}, SetCell{std::move(cell)});
    REQUIRE(grid.size() == Spreader::Size{43, 643});
    Cell * expected[9] = {
        nullptr, nullptr,  nullptr,
        nullptr, cellPtr,  nullptr,
        nullptr, nullptr,  nullptr
    };
    for(SizeType x = 12; x < 15; ++x)
        for(SizeType y = 24; y < 26; ++y)
            REQUIRE(grid.getCell({x, y}) == expected[(y - 24) * 3 + (x - 12)]);

}

#ifdef NDEBUG
TEST_CASE( "Speed test", "[cell_grid]" ) {

    CellGrid grid;
    
    BENCHMARK("setting cells") {
    //double beg = clock();
        for(SizeType x = 250; x < 300; ++x) {
            for(SizeType y = 4000; y < 5000; ++y) {
                grid.modifyCell(Spreader::Point{x, y}, SetCell{ValueCell::create(3.3)});
            }
        }
    //double end = clock();
    //printf("time: %lf\n", (end - beg) / CLOCKS_PER_SEC);
    };

    BENCHMARK("getting cells") {
        for(SizeType x = 250; x < 300; ++x) {
            for(SizeType y = 4000; y < 5000; ++y) {
                [[maybe_unused]] auto * volatile dummy = grid.getCell(Spreader::Point{x, y});
            }
        }
    };
}
#endif
