#include <spreader/sheet.h>
#include "test-util.h"

#include <catch2/catch_test_macros.hpp>

using namespace Spreader;


TEST_CASE( "Row deletion", "[sheet-insert-delete]" ) {
    
    {
        Sheet s;
        s.deleteRows(0, 0);
        CHECK(s.size() == Spreader::Size{0, 0});
        
        s.deleteRows(0, 15);
        CHECK(s.size() == Spreader::Size{0, 0});
    }
    
    {
        Sheet s;
        s.setFormulaCell(PT("A1"), SPRS("A20 + 3"));
        s.deleteRows(5, 4);
        CHECK(s.size() == Spreader::Size{1, 1});
        CHECK(s.getFormulaInfo(PT("A1")) == Sheet::FormulaInfo{SPRS("A16 + 3"), Spreader::Size{1,1}});
    }
    

    {
        Sheet s;
        s.setFormulaCell(PT("A2147483647"), SPRS("B2147483647 + 3"));
        CHECK(s.size() == Spreader::Size{1, 2147483647});
        s.deleteRows(ROW(1), 2147483647);
        CHECK(s.size() == Spreader::Size{1, 0});

        s.setFormulaCell(PT("A2147483647"), SPRS("B2147483647 + 3"));
        s.deleteRows(ROW(1), 2147483646);
        CHECK(s.getFormulaInfo(PT("A1")).value() == Sheet::FormulaInfo{.text= "B1 + 3", .extent= {1, 1}});
    }


    {
        Sheet s;
        s.deleteRows(ROW(3), 1);
        s.deleteRows(ROW(10), 25);
        s.setValueCell(PT("A4096"), 25);
        s.setValueCell(PT("B4096"), 4);
        s.setFormulaCell(PT("C4096"), SPRS("A4096 * B4096"));
        s.deleteRows(ROW(1), 4095);
        CHECK(s.getValue(PT("A1")) == 25.);
        CHECK(s.getValue(PT("B1")) == 4.);
        CHECK(s.getValue(PT("C1")) == 100.);
    }
 
    {
        Sheet s;
        
        s.setValueCell(PT("A4"), 0);
        
        s.setFormulaCell(PT("A1"), SPRS("A4 + 3"));
        s.setFormulaCell(PT("A2"), SPRS("A3 + 3"));
        s.setFormulaCell(PT("B1"), SPRS("B$4 + 3"));
        s.setFormulaCell(PT("B2"), SPRS("B$3 + 3"));
        s.setFormulaCell(PT("C1"), SPRS("C4:C4 + 3"));
        s.setFormulaCell(PT("D1"), SPRS("SUM(4:4)"));
        s.deleteRows(ROW(4), 1);
        CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("A2")) == 3.);
        CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("B2")) == 3.);
        CHECK(s.getValue(PT("C1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("D1")) == Error::InvalidReference);
    }
    
    {
        Sheet s;
        
        s.setFormulaCell(PT("A1"), SPRS("A20 + 3"));
        s.setFormulaCell(PT("A2"), SPRS("A19 + 3"));
        s.setFormulaCell(PT("B1"), SPRS("B$20 + 3"));
        s.setFormulaCell(PT("B2"), SPRS("B$19 + 3"));
        s.setFormulaCell(PT("C1"), SPRS("C20:C24 + 3"));
        s.setFormulaCell(PT("D1"), SPRS("SUM(20:24)"));
        s.setFormulaCell(PT("E1"), SPRS("SUM(19:24)"));
        s.setFormulaCell(PT("F1"), SPRS("SUM(20:25)"));
        s.setFormulaCell(PT("G1"), SPRS("C19:C24 + 3"));
        s.setFormulaCell(PT("H1"), SPRS("C20:C25 + 3"));
        
        s.setFormulaCell(PT("A30"), SPRS("A20 + 3"));
        s.setFormulaCell(PT("A31"), SPRS("A19 + 3"));
        s.setFormulaCell(PT("B30"), SPRS("B$20 + 3"));
        s.setFormulaCell(PT("B31"), SPRS("B$19 + 3"));
        s.setFormulaCell(PT("C30"), SPRS("C20:C24 + 3"));
        s.setFormulaCell(PT("D30"), SPRS("SUM(20:24)"));
        s.setFormulaCell(PT("E30"), SPRS("SUM(19:24)"));
        s.setFormulaCell(PT("F30"), SPRS("SUM(20:25)"));
        s.setFormulaCell(PT("G30"), SPRS("C19:C24 + 3"));
        s.setFormulaCell(PT("H30"), SPRS("C20:C25 + 3"));
        
        s.deleteRows(ROW(20), 5);
        
        CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("A2")) == 3.);
        CHECK(s.getValue(PT("B1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("B2")) == 3.);
        CHECK(s.getValue(PT("C1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("D1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("E1")) == 0.);
        CHECK(s.getValue(PT("F1")) == 0.);
        CHECK(s.getValue(PT("G1")) == 3.);
        CHECK(s.getValue(PT("H1")) == 3.);
        
        CHECK(s.getValue(PT("A25")) == Error::InvalidReference);
        CHECK(s.getValue(PT("A26")) == 3.);
        CHECK(s.getValue(PT("B25")) == Error::InvalidReference);
        CHECK(s.getValue(PT("B26")) == 3.);
        CHECK(s.getValue(PT("C25")) == Error::InvalidReference);
        CHECK(s.getValue(PT("D25")) == Error::InvalidReference);
        CHECK(s.getValue(PT("E25")) == 0.);
        CHECK(s.getValue(PT("F25")) == 0.);
        CHECK(s.getValue(PT("G25")) == 3.);
        CHECK(s.getValue(PT("H25")) == 3.);
    }

    {
        Sheet s;

        s.setFormulaCell(PT("A5030"), SPRS("A5020 + 3"));
        s.setFormulaCell(PT("A5031"), SPRS("A5026 + 3"));
        s.setFormulaCell(PT("B5030"), SPRS("B$5020 + 3"));
        s.setFormulaCell(PT("B5031"), SPRS("B$5026 + 3"));
        s.setFormulaCell(PT("C5030"), SPRS("C5020:C5024 + 3"));
        s.setFormulaCell(PT("D5030"), SPRS("SUM(5020:5024)"));
        s.setFormulaCell(PT("E5030"), SPRS("SUM(5020:5026)"));
        s.setFormulaCell(PT("H5030"), SPRS("C5020:C5026 + 3"));
        
        s.deleteRows(ROW(1), 5025);

        CHECK(s.getValue(PT("A5")) == Error::InvalidReference);
        CHECK(s.getValue(PT("A6")) == 3.);
        CHECK(s.getValue(PT("B5")) == Error::InvalidReference);
        CHECK(s.getValue(PT("B6")) == 3.);
        CHECK(s.getValue(PT("C5")) == Error::InvalidReference);
        CHECK(s.getValue(PT("D5")) == Error::InvalidReference);
        CHECK(s.getValue(PT("E5")) == 0.);
        CHECK(s.getValue(PT("H5")) == 3.);
    }
    
    {
        Sheet s;
        
        s.setFormulaCell(PT("A4"), SPRS("A14:A17 + 5"));
        s.setFormulaCell(PT("B5"), SPRS("B14:B17 + 5"));
        s.setFormulaCell(PT("D9"), SPRS("D20:C21 + 5"));
        
        s.deleteRows(ROW(5), 4);
        
        CHECK(s.getValue(PT("A4")) == 5.);
        CHECK(s.getValue(PT("A5")) == 5.);
        CHECK(s.getValue(PT("A6")) == 5.);
        CHECK(s.getValue(PT("A7")) == 5.);
        CHECK(s.getValue(PT("B5")) == Scalar{});
        CHECK(s.getValue(PT("B6")) == Scalar{});
        CHECK(s.getValue(PT("D5")) == 5.);
        CHECK(s.getValue(PT("D6")) == 5.);
        CHECK(s.getValue(PT("D7")) == Scalar{});
        
    }
    
}

TEST_CASE( "Column deletion", "[sheet-insert-delete]" ) {

    {
        Sheet s;
        s.deleteColumns(COLUMN("C"), 1);
        s.deleteColumns(COLUMN("Q"), 25);
        CHECK(s.nonNullCellCount() == 0);
        CHECK(s.size() == Spreader::Size{0, 0});
        CHECK(s.size() == Spreader::Size{0, 0});
        s.setValueCell(PT("QXX1"), 25);
        s.setValueCell(PT("QXX2"), 4);
        s.setFormulaCell(PT("QXX3"), SPRS("QXX1 * QXX2"));
        CHECK(s.nonNullCellCount() == 3);
        s.deleteColumns(COLUMN("A"), COLUMN("QXX") - COLUMN("A"));
        CHECK(s.nonNullCellCount() == 3);
        CHECK(s.size() == Spreader::Size{1, 3});
        CHECK(s.getValue(PT("A1")) == 25.);
        CHECK(s.getValue(PT("A2")) == 4.);
        CHECK(s.getValue(PT("A3")) == 100.);
        CHECK(s.getFormulaInfo(PT("A3")).value() == Sheet::FormulaInfo{SPRS("A1 * A2"), Spreader::Size{1, 1}});
    }

    {
        Sheet s;
        
        s.setValueCell(PT("D1"), 0);
        
        s.setFormulaCell(PT("A1"), SPRS("D1 + 3"));
        s.setFormulaCell(PT("B1"), SPRS("C1 + 3"));
        s.setFormulaCell(PT("A2"), SPRS("$D2 + 3"));
        s.setFormulaCell(PT("B2"), SPRS("$C2 + 3"));
        s.setFormulaCell(PT("A3"), SPRS("D3:D3 + 3"));
        s.setFormulaCell(PT("A4"), SPRS("SUM(D:D)"));
        CHECK(s.nonNullCellCount() == 7);
        s.deleteColumns(COLUMN("D"), 1);
        CHECK(s.nonNullCellCount() == 6);
        CHECK(s.size() == Spreader::Size{3, 4});
        CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A1")).value() == Sheet::FormulaInfo{SPRS("#REF! + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("B1")) == 3.);
        CHECK(s.getFormulaInfo(PT("B1")).value() == Sheet::FormulaInfo{SPRS("C1 + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("A2")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A2")).value() == Sheet::FormulaInfo{SPRS("#REF! + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("B2")) == 3.);
        CHECK(s.getFormulaInfo(PT("B2")).value() == Sheet::FormulaInfo{SPRS("$C2 + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A3")).value() == Sheet::FormulaInfo{SPRS("#REF! + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("A4")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A4")).value() == Sheet::FormulaInfo{SPRS("SUM(#REF!)"), Spreader::Size{1,1}});
    }

    {
        Sheet s;
        
        s.setFormulaCell(PT("A1"), SPRS("T1 + 3"));
        s.setFormulaCell(PT("B1"), SPRS("S1 + 3"));
        s.setFormulaCell(PT("A2"), SPRS("$T2 + 3"));
        s.setFormulaCell(PT("B2"), SPRS("$S2 + 3"));
        s.setFormulaCell(PT("A3"), SPRS("T3:X3 + 3"));
        s.setFormulaCell(PT("A4"), SPRS("SUM(T:X)"));
        s.setFormulaCell(PT("A5"), SPRS("SUM(S:X)"));
        s.setFormulaCell(PT("A6"), SPRS("SUM(T:Y)"));
        s.setFormulaCell(PT("A7"), SPRS("S3:X3 + 3"));
        s.setFormulaCell(PT("A8"), SPRS("T3:Y3 + 3"));
        
        s.setFormulaCell(PT("AD1"), SPRS("T1 + 3"));
        s.setFormulaCell(PT("AE1"), SPRS("S1 + 3"));
        s.setFormulaCell(PT("AD2"), SPRS("$T2 + 3"));
        s.setFormulaCell(PT("AE2"), SPRS("$S2 + 3"));
        s.setFormulaCell(PT("AD3"), SPRS("T3:X3 + 3"));
        s.setFormulaCell(PT("AD4"), SPRS("SUM(T:X)"));
        s.setFormulaCell(PT("AD5"), SPRS("SUM(S:X)"));
        s.setFormulaCell(PT("AD6"), SPRS("SUM(T:Y)"));
        s.setFormulaCell(PT("AD7"), SPRS("S3:X3 + 3"));
        s.setFormulaCell(PT("AD8"), SPRS("T3:Y3 + 3"));
        
        CHECK(s.nonNullCellCount() == 48);
        CHECK(s.size() == Spreader::Size{35, 8});
        s.deleteColumns(COLUMN("T"), 5);
        CHECK(s.nonNullCellCount() == 22);
        CHECK(s.size() == Spreader::Size{30, 8});
        
        CHECK(s.getValue(PT("A1")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A1")).value() == Sheet::FormulaInfo{SPRS("#REF! + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("B1")) == 3.);
        CHECK(s.getFormulaInfo(PT("B1")).value() == Sheet::FormulaInfo{SPRS("S1 + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("A2")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A2")).value() == Sheet::FormulaInfo{SPRS("#REF! + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("B2")) == 3.);
        CHECK(s.getValue(PT("A3")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A3")).value() == Sheet::FormulaInfo{SPRS("#REF! + 3"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("A4")) == Error::InvalidReference);
        CHECK(s.getFormulaInfo(PT("A4")).value() == Sheet::FormulaInfo{SPRS("SUM(#REF!)"), Spreader::Size{1,1}});
        CHECK(s.getValue(PT("A5")) == 0.);
        CHECK(s.getValue(PT("A6")) == 0.);
        CHECK(s.getValue(PT("A7")) == 3.);
        CHECK(s.getValue(PT("A8")) == 3.);
        
        CHECK(s.getValue(PT("Y1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("Z1")) == 3.);
        CHECK(s.getValue(PT("Y2")) == Error::InvalidReference);
        CHECK(s.getValue(PT("Z2")) == 3.);
        CHECK(s.getValue(PT("Y3")) == Error::InvalidReference);
        CHECK(s.getValue(PT("Y4")) == Error::InvalidReference);
        CHECK(s.getValue(PT("Y5")) == 0.);
        CHECK(s.getValue(PT("Y6")) == 0.);
        CHECK(s.getValue(PT("Y7")) == 3.);
        CHECK(s.getValue(PT("Y8")) == 3.);
    }

    {
        Sheet s;

        s.setFormulaCell(PT("GKL1"), SPRS("GKB1 + 3"));
        s.setFormulaCell(PT("GKM1"), SPRS("GKH1 + 3"));
        s.setFormulaCell(PT("GKL2"), SPRS("$GKB2 + 3"));
        s.setFormulaCell(PT("GKM2"), SPRS("$GKH2 + 3"));
        s.setFormulaCell(PT("GKL3"), SPRS("GKB3:GKF3 + 3"));
        s.setFormulaCell(PT("GKL4"), SPRS("SUM(GKB:GKF)"));
        s.setFormulaCell(PT("GKL5"), SPRS("SUM(GKB:GKH)"));
        s.setFormulaCell(PT("GKL6"), SPRS("GKB3:GKH3 + 3"));
        
        s.deleteColumns(COLUMN("A"), 5025);

        CHECK(s.getValue(PT("E1")) == Error::InvalidReference);
        CHECK(s.getValue(PT("F1")) == 3.);
        CHECK(s.getValue(PT("E2")) == Error::InvalidReference);
        CHECK(s.getValue(PT("F2")) == 3.);
        CHECK(s.getValue(PT("E3")) == Error::InvalidReference);
        CHECK(s.getValue(PT("E4")) == Error::InvalidReference);
        CHECK(s.getValue(PT("E5")) == 0.);
        CHECK(s.getValue(PT("E6")) == 3.);
    }

    {
        Sheet s;
        
        s.setFormulaCell(PT("D1"), SPRS("N1:R1 + 5"));
        s.setFormulaCell(PT("E2"), SPRS("N2:R2 + 5"));
        s.setFormulaCell(PT("I4"), SPRS("T4:U3 + 5"));
        
        s.deleteColumns(COLUMN("E"), 4);
        
        CHECK(s.getValue(PT("D1")) == 5.);
        CHECK(s.getValue(PT("E1")) == 5.);
        CHECK(s.getValue(PT("F1")) == 5.);
        CHECK(s.getValue(PT("G1")) == 5.);
        CHECK(s.getValue(PT("E2")) == Scalar{});
        CHECK(s.getValue(PT("F2")) == Scalar{});
        CHECK(s.getValue(PT("E4")) == 5.);
        CHECK(s.getValue(PT("F4")) == 5.);
        CHECK(s.getValue(PT("G4")) == Scalar{});
        
    }

}

TEST_CASE( "Row insertion", "[sheet-insert-delete]" ) {

    {
        Sheet s;
        s.insertRows(0, 17);
        CHECK(s.nonNullCellCount() == 0);
        CHECK(s.size() == Spreader::Size{0, 17});
        s.insertRows(5089, 32);
        CHECK(s.nonNullCellCount() == 0);
        CHECK(s.size() == Spreader::Size{0, 5089 + 32});
    }

    {
        Sheet s;
        s.setValueCell(PT("A1"), 12);
        s.setValueCell(PT("C1"), 32);
        CHECK(s.nonNullCellCount() == 2);
        s.insertRows(ROW(1), 5);
        CHECK(s.nonNullCellCount() == 2);
        CHECK(s.size() == Spreader::Size{3, 6});
        CHECK(s.getValue(PT("A1")) == Scalar{});
        CHECK(s.getValue(PT("A6")) == 12.);
        CHECK(s.getValue(PT("C1")) == Scalar{});
        CHECK(s.getValue(PT("C6")) == 32.);
    }

    {
        Sheet s;
        s.setValueCell(PT("A1"), 12);
        s.setValueCell(PT("C1"), 32);
        CHECK(s.nonNullCellCount() == 2);
        s.insertRows(ROW(2), 5);
        CHECK(s.nonNullCellCount() == 2);
        CHECK(s.size() == Spreader::Size{3, 6});
        CHECK(s.getValue(PT("A1")) == 12.);
        CHECK(s.getValue(PT("A7")) == Scalar{});
        CHECK(s.getValue(PT("C1")) == 32.);
        CHECK(s.getValue(PT("C7")) == Scalar{});
    }

    {
        Sheet s;
        s.setValueCell(PT("A1"), 12);
        s.setValueCell(PT("C1"), 32);
        s.setValueCell(PT("A2"), 15);
        s.setValueCell(PT("C2"), 35);
        CHECK(s.nonNullCellCount() == 4);
        s.insertRows(ROW(2), 5);
        CHECK(s.nonNullCellCount() == 4);
        CHECK(s.size() == Spreader::Size{3, 7});
        CHECK(s.getValue(PT("A1")) == 12.);
        CHECK(s.getValue(PT("A7")) == 15.);
        CHECK(s.getValue(PT("C1")) == 32.);
        CHECK(s.getValue(PT("C7")) == 35.);
    }

    {
        Sheet s;
        s.setFormulaCell(PT("A1"), SPRS("SUM(C1:D3)"));
        s.setFormulaCell(PT("A2"), SPRS("SUM(C1:D3)"));
        s.setValueCell(PT("C1"), 1);
        s.setValueCell(PT("C2"), 1);
        s.setValueCell(PT("C3"), 1);
        s.setValueCell(PT("D1"), 1);
        s.setValueCell(PT("D2"), 1);
        s.setValueCell(PT("D3"), 1);
        CHECK(s.nonNullCellCount() == 8);
        s.insertRows(ROW(2), 3);
        CHECK(s.nonNullCellCount() == 8);
        CHECK(s.size() == Spreader::Size{4, 6});
        CHECK(s.getValue(PT("A1")) == 6.);
        CHECK(s.getValue(PT("A5")) == 6.);
        CHECK(s.getFormulaInfo(PT("A1")).value().text == SPRS("SUM(C1:D6)"));
        CHECK(s.getFormulaInfo(PT("A5")).value().text == SPRS("SUM(C1:D6)"));
    }
}

TEST_CASE( "Column insertion", "[sheet-insert-delete]" ) {

    {
        Sheet s;
        s.insertColumns(COLUMN("A"), 5);
        CHECK(s.nonNullCellCount() == 0);
        CHECK(s.size() == Spreader::Size{5, 0});
        s.insertColumns(COLUMN("Q"), 32);
        CHECK(s.nonNullCellCount() == 0);
        CHECK(s.size() == Spreader::Size{COLUMN("Q") + 32, 0});
    }
}
