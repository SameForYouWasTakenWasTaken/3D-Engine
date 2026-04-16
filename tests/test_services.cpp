#include <gtest/gtest.h>
#include "Engine/Systems/Services.hpp"
#include <stdexcept>

// ─── HashCombine ──────────────────────────────────────────────────────────

TEST(HashCombineTest, SeedChangesAfterCombine) {
    std::size_t seed = 0;
    std::size_t original = seed;
    HashCombine(seed, 42u);
    EXPECT_NE(seed, original);
}

TEST(HashCombineTest, DeterministicResult) {
    std::size_t seed1 = 0;
    std::size_t seed2 = 0;
    HashCombine(seed1, 12345u);
    HashCombine(seed2, 12345u);
    EXPECT_EQ(seed1, seed2);
}

TEST(HashCombineTest, DifferentHashInputsProduceDifferentSeeds) {
    std::size_t seed1 = 0;
    std::size_t seed2 = 0;
    HashCombine(seed1, 111u);
    HashCombine(seed2, 222u);
    EXPECT_NE(seed1, seed2);
}

TEST(HashCombineTest, OrderMatters) {
    // Combining A then B should differ from B then A
    std::size_t seedAB = 0;
    HashCombine(seedAB, 1u);
    HashCombine(seedAB, 2u);

    std::size_t seedBA = 0;
    HashCombine(seedBA, 2u);
    HashCombine(seedBA, 1u);

    EXPECT_NE(seedAB, seedBA);
}

TEST(HashCombineTest, ZeroHashStillMutatesSeed) {
    std::size_t seed = 100u;
    std::size_t before = seed;
    HashCombine(seed, 0u);
    EXPECT_NE(seed, before);
}

// ─── Hash template ────────────────────────────────────────────────────────

TEST(HashFunctionTest, SingleStringIsDeterministic) {
    std::size_t h1 = Hash(std::string("hello"));
    std::size_t h2 = Hash(std::string("hello"));
    EXPECT_EQ(h1, h2);
}

TEST(HashFunctionTest, DifferentStringsProduceDifferentHashes) {
    std::size_t h1 = Hash(std::string("foo"));
    std::size_t h2 = Hash(std::string("bar"));
    EXPECT_NE(h1, h2);
}

TEST(HashFunctionTest, SingleIntegerIsDeterministic) {
    std::size_t h1 = Hash(42);
    std::size_t h2 = Hash(42);
    EXPECT_EQ(h1, h2);
}

TEST(HashFunctionTest, DifferentIntegersProduceDifferentHashes) {
    std::size_t h1 = Hash(1);
    std::size_t h2 = Hash(2);
    EXPECT_NE(h1, h2);
}

TEST(HashFunctionTest, MultipleArgsIsDeterministic) {
    std::size_t h1 = Hash(std::string("key"), 42, 3.14f);
    std::size_t h2 = Hash(std::string("key"), 42, 3.14f);
    EXPECT_EQ(h1, h2);
}

TEST(HashFunctionTest, MultipleArgsDifferFromSingleArg) {
    std::size_t hSingle = Hash(std::string("key"));
    std::size_t hMulti  = Hash(std::string("key"), 0);
    EXPECT_NE(hSingle, hMulti);
}

TEST(HashFunctionTest, ReturnsCustomType) {
    uint32_t h = Hash<uint32_t>(std::string("texture_path"));
    // Should compile and return a uint32_t — just verify it doesn't throw
    (void)h;
    SUCCEED();
}

// ─── Services singleton ────────────────────────────────────────────────────

TEST(ServicesTest, GetReturnsSameInstance) {
    Services& a = Services::Get();
    Services& b = Services::Get();
    EXPECT_EQ(&a, &b);
}

// ─── RegisterService / GetService / ServiceExists ─────────────────────────
// Each test is self-contained: it uses service tag types that are unique to
// that test, so singleton state does not cause ordering issues.

// Unique tag types — each used by exactly one test group.
struct SvcCheck  : IService {};
struct SvcReg    : IService { int value = 42; };
struct SvcDouble : IService {};
struct SvcThrow  : IService {};
struct SvcArgs   : IService { int x; explicit SvcArgs(int v) : x(v) {} };
struct SvcMultiA : IService {};
struct SvcMultiB : IService {};

TEST(ServicesTest, ServiceExistsReturnsFalseBeforeRegister) {
    EXPECT_FALSE(Services::Get().ServiceExists<SvcCheck>());
}

TEST(ServicesTest, RegisterServiceMakesServiceExistsTrue) {
    ASSERT_FALSE(Services::Get().ServiceExists<SvcReg>());
    Services::Get().RegisterService<SvcReg>();
    EXPECT_TRUE(Services::Get().ServiceExists<SvcReg>());
}

TEST(ServicesTest, GetServiceReturnsRegisteredInstance) {
    ASSERT_FALSE(Services::Get().ServiceExists<SvcArgs>());
    Services::Get().RegisterService<SvcArgs>(99);
    EXPECT_EQ(Services::Get().GetService<SvcArgs>().x, 99);
}

TEST(ServicesTest, RegisterServiceThrowsIfAlreadyRegistered) {
    ASSERT_FALSE(Services::Get().ServiceExists<SvcDouble>());
    Services::Get().RegisterService<SvcDouble>();
    EXPECT_THROW(Services::Get().RegisterService<SvcDouble>(), std::runtime_error);
}

TEST(ServicesTest, GetServiceThrowsIfNotRegistered) {
    ASSERT_FALSE(Services::Get().ServiceExists<SvcThrow>());
    EXPECT_THROW(Services::Get().GetService<SvcThrow>(), std::runtime_error);
}

TEST(ServicesTest, RegisterMultipleDistinctServices) {
    ASSERT_FALSE(Services::Get().ServiceExists<SvcMultiA>());
    ASSERT_FALSE(Services::Get().ServiceExists<SvcMultiB>());
    Services::Get().RegisterService<SvcMultiA>();
    Services::Get().RegisterService<SvcMultiB>();
    EXPECT_TRUE(Services::Get().ServiceExists<SvcMultiA>());
    EXPECT_TRUE(Services::Get().ServiceExists<SvcMultiB>());
}

// Boundary: ServiceExists returns false for a type that was never registered.
TEST(ServicesTest, ServiceExistsReturnsFalseForNeverRegisteredType) {
    EXPECT_FALSE(Services::Get().ServiceExists<SvcCheck>());
}