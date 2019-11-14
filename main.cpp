//#include <iostream>
//#include <typeinfo>
//
//using value_type = char;
//using pointer = value_type*;
//using const_pointer_1 = const pointer; // pointer const --> value_type* const --> char* const
//using const_pointer_2 = const value_type*; // const char *
//
//int main()
//{
//	const_pointer_1 a1 = nullptr;
//	const_pointer_2 a2 = nullptr;
//
//	std::cout << typeid(a1).name() << '\n';
//	std::cout << typeid(a2).name() << '\n';
//	
//	return 0;
//}


#include "buffer.h"
#include <string_view>
#include <gmock/gmock.h>

using namespace std::literals;

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

auto result = "abc"sv;

TEST(Buffer, iterators)
{
    Buffer b(result);
    EXPECT_EQ(result, b.toString());

    auto pos = 0;
    for (auto c = b.begin(); c != b.end(); ++c)
        EXPECT_EQ(result[pos++], char(*c));

    for (auto c = b.rbegin(); c != b.rend(); ++c)
        EXPECT_EQ(result[--pos], char(*c));
}

TEST(Buffer, clear)
{
    Buffer b(result);
    EXPECT_EQ(result, b.toString());
    EXPECT_FALSE(b.empty());

    b.clear();
    EXPECT_TRUE(b.empty());
}

TEST(Buffer, reserve)
{
    Buffer b(result);
    EXPECT_EQ(result, b.toString());
    EXPECT_EQ(result.size(), b.capacity());

    b.reserve(200);
    EXPECT_EQ(result, b.toString());
    EXPECT_EQ(200, b.capacity());

    b.reserve(100); // do nothing
    EXPECT_EQ(result, b.toString());
    EXPECT_EQ(200, b.capacity());
}

TEST(Buffer, resize)
{
    Buffer b(result);
    EXPECT_EQ(result, b.toString());
    EXPECT_EQ(result.size(), b.capacity());

    b.resize(200);
    EXPECT_EQ(200, b.size());
    EXPECT_EQ(200, b.capacity());

    b.resize(result.size());
    EXPECT_EQ(200, b.capacity());
    EXPECT_EQ(result, b.toString());

    b.resize(result.size(), Buffer::Options::Shrink);
    EXPECT_EQ(result.size(), b.capacity());
    EXPECT_EQ(result, b.toString());

    // test `Buffer::Options::Shrink` ??
}

TEST(Buffer, assign)
{
    Buffer b;

    b.assign(result);
    EXPECT_EQ(result, b.toString());
    EXPECT_EQ(result.size(), b.capacity());

    auto r2 = "a"sv;
    b.assign(r2);
    EXPECT_EQ(r2, b.toString());
    EXPECT_EQ(result.size(), b.capacity());

    auto r3 = "aaaaaaaaaaa"sv;
    b.assign(r3);
    EXPECT_EQ(r3, b.toString());
    EXPECT_EQ(r3.size(), b.capacity());
}

TEST(Buffer, insert)
{

}

TEST(Buffer, erase)
{

}

TEST(Buffer, swap)
{
    Buffer b1, b2(result);
    EXPECT_TRUE(b1.empty());
    EXPECT_EQ(result, b2.toString());

    swap(b1, b2);
    EXPECT_TRUE(b2.empty());
    EXPECT_EQ(result, b1.toString());
}
