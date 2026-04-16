#include <gtest/gtest.h>
#include "Engine/Systems/Texture2DManager.hpp"
#include <unordered_set>

// Test Texture2DManager Hash basic functionality
TEST(Texture2DManagerTest, HashBasic) {
    // Hash should produce consistent results
    std::string path1 = "assets/Textures2D/test.png";
    TextureID hash1a = Hash<TextureID>(path1);
    TextureID hash1b = Hash<TextureID>(path1);

    // Same path should produce same hash
    EXPECT_EQ(hash1a, hash1b);
}

// Test Texture2DManager Hash with different paths
TEST(Texture2DManagerTest, HashDifferentPaths) {
    std::string path1 = "assets/Textures2D/texture1.png";
    std::string path2 = "assets/Textures2D/texture2.png";

    TextureID hash1 = Hash<TextureID>(path1);
    TextureID hash2 = Hash<TextureID>(path2);

    // Different paths should (very likely) produce different hashes
    EXPECT_NE(hash1, hash2);
}

// Test Texture2DManager Hash with empty string
TEST(Texture2DManagerTest, HashEmptyString) {
    std::string empty = "";
    TextureID hash = Hash<TextureID>(empty);

    // Should not crash and should produce consistent hash
    TextureID hash2 = Hash<TextureID>(empty);
    EXPECT_EQ(hash, hash2);
}

// Test Texture2DManager Hash with single character
TEST(Texture2DManagerTest, HashSingleCharacter) {
    std::string single = "a";
    TextureID hash1 = Hash<TextureID>(single);
    TextureID hash2 = Hash<TextureID>(std::string("b"));

    // Different characters should produce different hashes
    EXPECT_NE(hash1, hash2);

    // Same character should produce same hash
    TextureID hash1_again = Hash<TextureID>(single);
    EXPECT_EQ(hash1, hash1_again);
}

// Test Texture2DManager Hash with long paths
TEST(Texture2DManagerTest, HashLongPaths) {
    std::string long_path = "assets/Textures2D/VeryLongDirectoryName/AnotherLongDirectory/SubDirectory/texture_with_very_long_name_for_testing.png";
    TextureID hash = Hash<TextureID>(long_path);

    // Should produce consistent hash
    TextureID hash2 = Hash<TextureID>(long_path);
    EXPECT_EQ(hash, hash2);
}

// Test Texture2DManager Hash with special characters
TEST(Texture2DManagerTest, HashSpecialCharacters) {
    std::string path_with_spaces = "assets/Textures2D/texture with spaces.png";
    std::string path_with_underscores = "assets/Textures2D/texture_with_underscores.png";
    std::string path_with_dashes = "assets/Textures2D/texture-with-dashes.png";

    TextureID hash1 = Hash<TextureID>(path_with_spaces);
    TextureID hash2 = Hash<TextureID>(path_with_underscores);
    TextureID hash3 = Hash<TextureID>(path_with_dashes);

    // All should be different
    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash2, hash3);
    EXPECT_NE(hash1, hash3);
}

// Test Texture2DManager Hash with case sensitivity
TEST(Texture2DManagerTest, HashCaseSensitivity) {
    std::string lower = "resources/textures2d/texture.png";
    std::string upper = "assets/Textures2D/Texture.png";
    std::string mixed = "assets/Textures2D/texture.png";

    TextureID hash_lower = Hash<TextureID>(lower);
    TextureID hash_upper = Hash<TextureID>(upper);
    TextureID hash_mixed = Hash<TextureID>(mixed);

    // Case matters - all should be different
    EXPECT_NE(hash_lower, hash_upper);
    EXPECT_NE(hash_lower, hash_mixed);
    EXPECT_NE(hash_upper, hash_mixed);
}

// Test Texture2DManager Hash with relative vs absolute paths
TEST(Texture2DManagerTest, HashRelativeVsAbsolute) {
    std::string relative = "assets/Textures2D/texture.png";
    std::string absolute = "/home/user/project/assets/Textures2D/texture.png";

    TextureID hash_relative = Hash<TextureID>(relative);
    TextureID hash_absolute = Hash<TextureID>(absolute);

    // Different paths should produce different hashes
    EXPECT_NE(hash_relative, hash_absolute);

    // Each should be consistent
    EXPECT_EQ(hash_relative, Hash<TextureID>(relative));
    EXPECT_EQ(hash_absolute, Hash<TextureID>(absolute));
}

// Test Texture2DManager Hash with similar paths
TEST(Texture2DManagerTest, HashSimilarPaths) {
    std::string path1 = "assets/Textures2D/texture.png";
    std::string path2 = "assets/Textures2D/texture.jpg";
    std::string path3 = "assets/Textures2D/textures.png";

    TextureID hash1 = Hash<TextureID>(path1);
    TextureID hash2 = Hash<TextureID>(path2);
    TextureID hash3 = Hash<TextureID>(path3);

    // All should be different despite similarity
    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash2, hash3);
    EXPECT_NE(hash1, hash3);
}

// Test Texture2DManager Hash with forward vs backward slashes
TEST(Texture2DManagerTest, HashSlashTypes) {
    std::string forward = "assets/Textures2D/texture.png";
    std::string backward = "assets\\Textures2D\\texture.png";

    TextureID hash_forward = Hash<TextureID>(forward);
    TextureID hash_backward = Hash<TextureID>(backward);

    // Different slash types should produce different hashes
    EXPECT_NE(hash_forward, hash_backward);
}

/**
 * @brief Verifies the hash function produces unique TextureID values for many distinct paths.
 *
 * Generates 100 distinct texture file paths, computes Hash<TextureID> for each,
 * and asserts that all resulting TextureID values are unique (no collisions).
 */
TEST(Texture2DManagerTest, HashDistribution) {
    std::unordered_set<TextureID> hashes;
    const int NUM_PATHS = 100;

    // Generate many different paths
    for (int i = 0; i < NUM_PATHS; ++i) {
        std::string path = "assets/Textures2D/texture_" + std::to_string(i) + ".png";
        TextureID hash = Hash<TextureID>(path);
        hashes.insert(hash);
    }

    // All hashes should be unique (no collisions expected for different paths)
    EXPECT_EQ(hashes.size(), NUM_PATHS);
}

// Test Texture2DManager Hash with common texture file extensions
TEST(Texture2DManagerTest, HashCommonExtensions) {
    std::string base = "assets/Textures2D/texture";
    std::vector<std::string> extensions = {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds"};
    std::unordered_set<TextureID> hashes;

    for (const auto& ext : extensions) {
        std::string path = base + ext;
        TextureID hash = Hash<TextureID>(path);
        hashes.insert(hash);
    }

    // All should produce unique hashes
    EXPECT_EQ(hashes.size(), extensions.size());
}

// Test Texture2DManager Hash with trailing/leading whitespace
TEST(Texture2DManagerTest, HashWhitespace) {
    std::string normal = "assets/Textures2D/texture.png";
    std::string leading = " assets/Textures2D/texture.png";
    std::string trailing = "assets/Textures2D/texture.png ";
    std::string both = " assets/Textures2D/texture.png ";

    TextureID hash_normal = Hash<TextureID>(normal);
    TextureID hash_leading = Hash<TextureID>(leading);
    TextureID hash_trailing = Hash<TextureID>(trailing);
    TextureID hash_both = Hash<TextureID>(both);

    // Whitespace matters - all should be different
    EXPECT_NE(hash_normal, hash_leading);
    EXPECT_NE(hash_normal, hash_trailing);
    EXPECT_NE(hash_normal, hash_both);
}

// Test Texture2DManager Hash determinism across calls
TEST(Texture2DManagerTest, HashDeterminism) {
    std::string path = "assets/Textures2D/test_texture.png";
    std::vector<TextureID> hashes;

    // Hash the same path multiple times
    for (int i = 0; i < 10; ++i) {
        hashes.push_back(Hash<TextureID>(path));
    }

    // All hashes should be identical
    for (size_t i = 1; i < hashes.size(); ++i) {
        EXPECT_EQ(hashes[0], hashes[i]);
    }
}

/**
 * @brief Ensures numeric-only path strings produce deterministic and distinct hashes.
 *
 * Verifies that two different numeric strings yield different TextureID hashes
 * and that hashing the same numeric string twice produces identical TextureID values.
 */
TEST(Texture2DManagerTest, HashNumericPaths) {
    std::string path1 = "123456";
    std::string path2 = "654321";
    std::string path3 = "123456";

    TextureID hash1 = Hash<TextureID>(path1);
    TextureID hash2 = Hash<TextureID>(path2);
    TextureID hash3 = Hash<TextureID>(path3);

    EXPECT_NE(hash1, hash2);
    EXPECT_EQ(hash1, hash3);
}

// Test Texture2DManager Hash with Unicode characters (if supported)
TEST(Texture2DManagerTest, HashUnicode) {
    std::string path_ascii = "assets/Textures2D/texture.png";
    std::string path_unicode = "assets/Textures2D/テクスチャ.png";

    TextureID hash_ascii = Hash<TextureID>(path_ascii);
    TextureID hash_unicode = Hash<TextureID>(path_unicode);

    // Should produce different hashes
    EXPECT_NE(hash_ascii, hash_unicode);

    // Unicode path should hash consistently
    EXPECT_EQ(hash_unicode, Hash<TextureID>(path_unicode));
}