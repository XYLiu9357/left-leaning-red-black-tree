/**main.cpp
 *
 * Invoke Google test for unit testing the red-black tree.
 */

#include <gtest/gtest.h>

int main()
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}