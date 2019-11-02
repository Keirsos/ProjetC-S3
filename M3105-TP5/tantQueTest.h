/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   tantQueTest.h
 * Author: Flavien
 *
 * Created on 2 nov. 2019, 16:25:03
 */

#ifndef TANTQUETEST_H
#define TANTQUETEST_H

#include <cppunit/extensions/HelperMacros.h>

class tantQueTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(tantQueTest);

//    CPPUNIT_TEST(testMethod);
//    CPPUNIT_TEST(testFailedMethod);
    
    /******************************/
    
    CPPUNIT_TEST(testSyntaxe);
    CPPUNIT_TEST(testVariables);

    CPPUNIT_TEST_SUITE_END();

public:
    tantQueTest();
    virtual ~tantQueTest();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
    
    /******************************/
    
    void testSyntaxe();
    void testVariables();
};

#endif /* TANTQUETEST_H */

