#include <spreader/interval-map.h>

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;

using Map = IntervalMap<unsigned, unsigned>;

TEST_CASE( "Empty length map", "[length-map]" ) {

    Map map;
    CHECK(map.toString() == "");

    CHECK(map.getValueWithDefault(0, 42) == 42);
    CHECK(map.getValueWithDefault(70, 42) == 42);
    
    map.clearValue(0);
    CHECK(map.toString() == "");
    map.clearValue(25);
    CHECK(map.toString() == "");
}

TEST_CASE( "Setting single index", "[length-map]" ) {

    {
        Map map;

        map.setValue(0, 3);
        CHECK(map.toString() == "[0, 1, 3]");
        CHECK(map.getValueWithDefault(0, 42) == 3);
        CHECK(map.getValueWithDefault(1, 42) == 42);
        CHECK(map.getValueWithDefault(2, 42) == 42);

        map.setValue(0, 4);
        CHECK(map.toString() == "[0, 1, 4]");
        CHECK(map.getValueWithDefault(0, 42) == 4);
        CHECK(map.getValueWithDefault(1, 42) == 42);
        CHECK(map.getValueWithDefault(2, 42) == 42);
        
        map.clearValue(25);
        CHECK(map.toString() == "[0, 1, 4]");
        
        map.clearValue(0);
        CHECK(map.toString() == "");
        CHECK(map.getValueWithDefault(0, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(15, 3);
        CHECK(map.toString() == "[15, 16, 3]");
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
        CHECK(map.getValueWithDefault(17, 42) == 42);

        map.setValue(15, 4);
        CHECK(map.toString() == "[15, 16, 4]");
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 4);
        CHECK(map.getValueWithDefault(16, 42) == 42);
        CHECK(map.getValueWithDefault(17, 42) == 42);
        
        map.clearValue(0);
        CHECK(map.toString() == "[15, 16, 4]");
        
        map.clearValue(15);
        CHECK(map.toString() == "");
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 42);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(15, 3);
        map.setValue(13, 3);
        
        CHECK(map.toString() == "[13, 14, 3][15, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(13, 3);
        map.setValue(15, 3);
        
        CHECK(map.toString() == "[13, 14, 3][15, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(15, 3);
        map.setValue(13, 4);
        
        CHECK(map.toString() == "[13, 14, 4][15, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 4);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(13, 3);
        map.setValue(15, 4);
        
        CHECK(map.toString() == "[13, 14, 3][15, 16, 4]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 4);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(15, 3);
        map.clearValue(13);
        
        CHECK(map.toString() == "[15, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 42);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(13, 3);
        map.clearValue(15);
        
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 42);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }
    
    {
        Map map;
        
        map.setValue(15, 3);
        map.setValue(13, 3);
        map.setValue(14, 3);
        
        CHECK(map.toString() == "[13, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 3);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
        
        map.clearValue(14);
        CHECK(map.toString() == "[13, 14, 3][15, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 42);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
        
        map.setValue(14, 3);
        map.clearValue(15);
        CHECK(map.toString() == "[13, 15, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 3);
        CHECK(map.getValueWithDefault(14, 42) == 3);
        CHECK(map.getValueWithDefault(15, 42) == 42);
        CHECK(map.getValueWithDefault(16, 42) == 42);
        
        map.setValue(15, 3);
        map.clearValue(13);
        CHECK(map.toString() == "[14, 16, 3]");
        CHECK(map.getValueWithDefault(12, 42) == 42);
        CHECK(map.getValueWithDefault(13, 42) == 42);
        CHECK(map.getValueWithDefault(14, 42) == 3);
        CHECK(map.getValueWithDefault(15, 42) == 3);
        CHECK(map.getValueWithDefault(16, 42) == 42);
    }

}

TEST_CASE( "Setting index range", "[length-map]" ) {
    
    {
        Map map;
     
        map.setValue(0, 0, 13);
        CHECK(map.toString() == "");
        map.setValue(10, 10, 13);
        CHECK(map.toString() == "");
    }
    
    {
        Map map;
     
        map.setValue(5, 10, 13);
        CHECK(map.toString() == "[5, 10, 13]");
        
        map.setValue(5, 10, 12);
        CHECK(map.toString() == "[5, 10, 12]");
        
        map.setValue(8, 13, 12);
        CHECK(map.toString() == "[5, 13, 12]");
        
        map.setValue(3, 9, 12);
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.setValue(3, 9, 12);
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.setValue(7, 11, 12);
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.setValue(6, 13, 12);
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.setValue(3, 5, 3);
        CHECK(map.toString() == "[3, 5, 3][5, 13, 12]");
        
        map.setValue(10, 13, 8);
        CHECK(map.toString() == "[3, 5, 3][5, 10, 12][10, 13, 8]");
        
        map.setValue(9, 11, 6);
        CHECK(map.toString() == "[3, 5, 3][5, 9, 12][9, 11, 6][11, 13, 8]");
        
        map.clearValue(9, 11);
        CHECK(map.toString() == "[3, 5, 3][5, 9, 12][11, 13, 8]");
        
        map.setValue(9, 11, 6);
        CHECK(map.toString() == "[3, 5, 3][5, 9, 12][9, 11, 6][11, 13, 8]");
        
        map.clearValue(9, 11);
        map.setValue(8, 12, 65);
        CHECK(map.toString() == "[3, 5, 3][5, 8, 12][8, 12, 65][12, 13, 8]");
        
        map.setValue(9, 10, 100);
        CHECK(map.toString() == "[3, 5, 3][5, 8, 12][8, 9, 65][9, 10, 100][10, 12, 65][12, 13, 8]");
        
        map.setValue(3, 13, 4);
        CHECK(map.toString() == "[3, 13, 4]");
    }
    
    {
        Map map;
        
        map.setValue(0, 4, 2);
        map.setValue(4, 6, 3);
        map.setValue(6, 8, 4);
        map.setValue(8, 10, 2);
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.clearValue(4, 9);
        CHECK(map.toString() == "[0, 4, 2][9, 10, 2]");
    }
    
    {
        Map map;
        
        map.setValue(0, 4, 2);
        map.setValue(4, 6, 3);
        map.setValue(6, 8, 4);
        map.setValue(8, 10, 2);
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.setValue(4, 9, 2);
        CHECK(map.toString() == "[0, 10, 2]");
    }
    
    {
        Map map;
        
        map.setValue(0, 4, 2);
        map.setValue(4, 6, 3);
        map.setValue(6, 8, 4);
        map.setValue(8, 10, 2);
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.setValue(3, 9, 2);
        CHECK(map.toString() == "[0, 10, 2]");
    }
    
    {
        Map map;
        
        map.setValue(0, 4, 2);
        map.setValue(4, 6, 3);
        map.setValue(6, 8, 4);
        map.setValue(8, 10, 2);
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.setValue(0, 9, 2);
        CHECK(map.toString() == "[0, 10, 2]");
    }
    
    {
        Map map;
        
        map.setValue(0, 5, 3);
        map.setValue(5, 7, 4);
        map.setValue(4, 5, 4);
        CHECK(map.toString() == "[0, 4, 3][4, 7, 4]");
        
        map.setValue(3, 5, 4);
        CHECK(map.toString() == "[0, 3, 3][3, 7, 4]");
        
        map.setValue(2, 5, 3);
        CHECK(map.toString() == "[0, 5, 3][5, 7, 4]");
    }
}

TEST_CASE( "Modifying index range", "[length-map]" ) {
    
    auto valueSetter = [](unsigned val) {
        return [val](const unsigned *) {
            return val;
        };
    };
    auto valueClearer = [](const unsigned *) {
            return std::nullopt;
    };
    
    {
        Map map;
     
        map.modifyValue(0, 0, valueSetter(13));
        CHECK(map.toString() == "");
        map.modifyValue(10, 10, valueSetter(13));
        CHECK(map.toString() == "");
    }
    
    {
        Map map;
     
        map.modifyValue(5, 10, valueSetter(13));
        CHECK(map.toString() == "[5, 10, 13]");
        
        map.modifyValue(5, 10, valueSetter(12));
        CHECK(map.toString() == "[5, 10, 12]");
        
        map.modifyValue(8, 13, valueSetter(12));
        CHECK(map.toString() == "[5, 13, 12]");
        
        map.modifyValue(3, 9, valueSetter(12));
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.modifyValue(3, 9, valueSetter(12));
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.modifyValue(7, 11, valueSetter(12));
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.modifyValue(6, 13, valueSetter(12));
        CHECK(map.toString() == "[3, 13, 12]");
        
        map.modifyValue(3, 5, valueSetter(3));
        CHECK(map.toString() == "[3, 5, 3][5, 13, 12]");
        
        map.modifyValue(10, 13, valueSetter(8));
        CHECK(map.toString() == "[3, 5, 3][5, 10, 12][10, 13, 8]");
        
        map.modifyValue(9, 11, valueSetter(6));
        CHECK(map.toString() == "[3, 5, 3][5, 9, 12][9, 11, 6][11, 13, 8]");
        
        map.modifyValue(9, 11, valueClearer);
        CHECK(map.toString() == "[3, 5, 3][5, 9, 12][11, 13, 8]");
        
        map.modifyValue(9, 11, valueSetter(6));
        CHECK(map.toString() == "[3, 5, 3][5, 9, 12][9, 11, 6][11, 13, 8]");
        
        map.modifyValue(9, 11, valueClearer);
        map.modifyValue(8, 12, valueSetter(65));
        CHECK(map.toString() == "[3, 5, 3][5, 8, 12][8, 12, 65][12, 13, 8]");
        
        map.modifyValue(9, 10, valueSetter(100));
        CHECK(map.toString() == "[3, 5, 3][5, 8, 12][8, 9, 65][9, 10, 100][10, 12, 65][12, 13, 8]");
        
        map.modifyValue(3, 13, valueSetter(4));
        CHECK(map.toString() == "[3, 13, 4]");
    }
    
    {
        Map map;
        
        map.modifyValue(0, 4, valueSetter(2));
        map.modifyValue(4, 6, valueSetter(3));
        map.modifyValue(6, 8, valueSetter(4));
        map.modifyValue(8, 10, valueSetter(2));
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.modifyValue(4, 9, valueClearer);
        CHECK(map.toString() == "[0, 4, 2][9, 10, 2]");
    }
    
    {
        Map map;
        
        map.modifyValue(0, 4, valueSetter(2));
        map.modifyValue(4, 6, valueSetter(3));
        map.modifyValue(6, 8, valueSetter(4));
        map.modifyValue(8, 10, valueSetter(2));
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.modifyValue(4, 9, valueSetter(2));
        CHECK(map.toString() == "[0, 10, 2]");
    }
    
    {
        Map map;
        
        map.modifyValue(0, 4, valueSetter(2));
        map.modifyValue(4, 6, valueSetter(3));
        map.modifyValue(6, 8, valueSetter(4));
        map.modifyValue(8, 10, valueSetter(2));
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.modifyValue(3, 9, valueSetter(2));
        CHECK(map.toString() == "[0, 10, 2]");
    }
    
    {
        Map map;
        
        map.modifyValue(0, 4, valueSetter(2));
        map.modifyValue(4, 6, valueSetter(3));
        map.modifyValue(6, 8, valueSetter(4));
        map.modifyValue(8, 10, valueSetter(2));
        CHECK(map.toString() == "[0, 4, 2][4, 6, 3][6, 8, 4][8, 10, 2]");
        
        map.modifyValue(0, 9, valueSetter(2));
        CHECK(map.toString() == "[0, 10, 2]");
    }
    
    {
        Map map;
        
        map.modifyValue(0, 5, valueSetter(3));
        map.modifyValue(5, 7, valueSetter(4));
        map.modifyValue(4, 5, valueSetter(4));
        CHECK(map.toString() == "[0, 4, 3][4, 7, 4]");
        
        map.modifyValue(3, 5, valueSetter(4));
        CHECK(map.toString() == "[0, 3, 3][3, 7, 4]");
        
        map.modifyValue(2, 5, valueSetter(3));
        CHECK(map.toString() == "[0, 5, 3][5, 7, 4]");
    }
    
}

TEST_CASE( "For each", "[length-map]" ) {
    
    std::string res;
    auto stringizer = [&res](unsigned startIdx, unsigned endIdx, unsigned value) {
        res += '[';
        res += std::to_string(startIdx);
        res += ", ";
        res += std::to_string(endIdx);
        res += ", ";
        res += std::to_string(value);
        res += ']';
    };
    
    {
        Map map;
        
        map.forEachIntervalWithDefault(0, 0, 42, stringizer);
        CHECK(res == "");
        
        map.forEachIntervalWithDefault(90, 90, 42, stringizer);
        CHECK(res == "");
        
        map.forEachIntervalWithDefault(10, 15, 42, stringizer);
        CHECK(res == "[10, 15, 42]");
    }
    
    {
        Map map;
        
        map.setValue(0, 7, 12);
        map.setValue(13, 45, 22);
        
        res.clear();
        map.forEachIntervalWithDefault(5, 9, 42, stringizer);
        CHECK(res == "[5, 7, 12][7, 9, 42]");
        
        res.clear();
        map.forEachIntervalWithDefault(10, 12, 42, stringizer);
        CHECK(res == "[10, 12, 42]");
        
        res.clear();
        map.forEachIntervalWithDefault(10, 27, 42, stringizer);
        CHECK(res == "[10, 13, 42][13, 27, 22]");
        
        res.clear();
        map.forEachIntervalWithDefault(10, 48, 42, stringizer);
        CHECK(res == "[10, 13, 42][13, 45, 22][45, 48, 42]");
    }
    
}

TEST_CASE( "Inserting indices", "[length-map]" ) {
    
    {
        Map map;
        
        map.insertIndices(0, 0);
        CHECK(map.toString() == "");
        
        map.insertIndices(0, 10);
        CHECK(map.toString() == "");
        
        map.insertIndices(5, 12);
        CHECK(map.toString() == "");
    }
    
    {
        Map map;
        
        map.setValue(4, 6, 5);
        map.insertIndices(3, 7);
        CHECK(map.toString() == "[11, 13, 5]");
        
        map.insertIndices(11, 2);
        CHECK(map.toString() == "[11, 15, 5]");
        
        map.insertIndices(12, 2);
        CHECK(map.toString() == "[11, 17, 5]");
        
        map.insertIndices(17, 2);
        CHECK(map.toString() == "[11, 17, 5]");
    }
    
    {
        Map map;
        map.setValue(2, 5, 16);
        map.setValue(8, 11, 17);
        map.setValue(14, 17, 18);
        
        map.insertIndices(6, 1);
        CHECK(map.toString() == "[2, 5, 16][9, 12, 17][15, 18, 18]");
        
        map.insertIndices(9, 1);
        CHECK(map.toString() == "[2, 5, 16][9, 13, 17][16, 19, 18]");
        
        map.insertIndices(10, 1);
        CHECK(map.toString() == "[2, 5, 16][9, 14, 17][17, 20, 18]");
    }
}

TEST_CASE( "Erasing indices", "[length-map]" ) {
    
    {
        Map map;
        
        map.eraseIndices(0, 0);
        CHECK(map.toString() == "");
        
        map.eraseIndices(0, 10);
        CHECK(map.toString() == "");
        
        map.eraseIndices(5, 12);
        CHECK(map.toString() == "");
    }
    
    {
        Map map;
        
        map.setValue(8, 24, 5);
        map.eraseIndices(2, 4);
        CHECK(map.toString() == "[6, 22, 5]");
        
        map.eraseIndices(6, 8);
        CHECK(map.toString() == "[6, 20, 5]");
        
        map.eraseIndices(8, 10);
        CHECK(map.toString() == "[6, 18, 5]");
        
        map.eraseIndices(10, 18);
        CHECK(map.toString() == "[6, 10, 5]");
    }
    
    {
        Map map;
        
        map.setValue(8, 16, 5);
        map.setValue(20, 36, 6);
        map.setValue(40, 41, 7);
        
        map.eraseIndices(8, 10);
        CHECK(map.toString() == "[8, 14, 5][18, 34, 6][38, 39, 7]");
        
        map.eraseIndices(12, 19);
        CHECK(map.toString() == "[8, 12, 5][12, 27, 6][31, 32, 7]");
        
        map.eraseIndices(11, 31);
        CHECK(map.toString() == "[8, 11, 5][11, 12, 7]");
    }
}
