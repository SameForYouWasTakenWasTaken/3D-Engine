#include <gtest/gtest.h>
#include <Engine/Systems/LightManager.hpp>

// Test LightManager CreateLight
TEST(LightManagerTest, CreateLight) {
    LightManager manager;

    // Create first light
    LightID id1 = manager.CreateLight(TODO).value();
    EXPECT_EQ(id1, 0u);

    // Create second light
    LightID id2 = manager.CreateLight(TODO).value();
    EXPECT_EQ(id2, 1u);

    // Create third light
    LightID id3 = manager.CreateLight(TODO).value();
    EXPECT_EQ(id3, 2u);

    // Verify IDs are sequential
    EXPECT_EQ(id2, id1 + 1);
    EXPECT_EQ(id3, id2 + 1);
}

// Test LightManager AddLight
TEST(LightManagerTest, AddLight) {
    LightManager manager;

    // Create a light with custom properties
    DirectionalLight light;
    light.position = glm::vec3(1.0f, 2.0f, 3.0f);
    light.color = glm::vec3(1.0f, 0.0f, 0.0f);
    light.intensity = 0.8f;

    // Add the light
    LightID id = manager.AddLight(light).value();
    EXPECT_EQ(id, 0u);

    // Retrieve the light and verify properties
    DirectionalLight* retrieved = manager.GetLight(id);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->position, light.position);
    EXPECT_EQ(retrieved->color, light.color);
    EXPECT_FLOAT_EQ(retrieved->intensity, 0.8f);
}

// Test LightManager GetLight
TEST(LightManagerTest, GetLight) {
    LightManager manager;

    // Create a light
    LightID id = manager.CreateLight(TODO).value();

    // Get the light
    DirectionalLight* light = manager.GetLight(id);
    ASSERT_NE(light, nullptr);

    // Verify default light properties
    EXPECT_EQ(light->position, glm::vec3(0.0f, 0.0f, 0.0f));
    EXPECT_EQ(light->color, glm::vec3(1.0f, 1.0f, 1.0f));
}

// Test LightManager GetLight with invalid ID
TEST(LightManagerTest, GetLightInvalidID) {
    LightManager manager;

    // Try to get a light that doesn't exist
    DirectionalLight* light = manager.GetLight(999);
    EXPECT_EQ(light, nullptr);
}

// Test LightManager RemoveLight
TEST(LightManagerTest, RemoveLight) {
    LightManager manager;

    // Create a light
    LightID id = manager.CreateLight(TODO).value();

    // Verify it exists
    DirectionalLight* light = manager.GetLight(id);
    ASSERT_NE(light, nullptr);

    // Remove the light
    manager.RemoveLight(id);

    // Verify it no longer exists
    light = manager.GetLight(id);
    EXPECT_EQ(light, nullptr);
}

// Test LightManager RemoveLight with invalid ID
TEST(LightManagerTest, RemoveLightInvalidID) {
    LightManager manager;

    // This should not crash
    manager.RemoveLight(999);

    // Manager should still work after attempting to remove non-existent light
    LightID id = manager.CreateLight(TODO).value();
    EXPECT_EQ(id, 0u);
}

// Test LightManager modifying light properties
TEST(LightManagerTest, ModifyLightProperties) {
    LightManager manager;

    // Create a light
    LightID id = manager.CreateLight(TODO).value();
    DirectionalLight* light = manager.GetLight(id);
    ASSERT_NE(light, nullptr);

    // Modify properties
    light->position = glm::vec3(5.0f, 10.0f, 15.0f);
    light->color = glm::vec3(0.5f, 0.5f, 1.0f);
    light->intensity = 0.9f;

    // Retrieve again and verify changes persisted
    DirectionalLight* modified = manager.GetLight(id);
    ASSERT_NE(modified, nullptr);
    EXPECT_EQ(modified->position, glm::vec3(5.0f, 10.0f, 15.0f));
    EXPECT_EQ(modified->color, glm::vec3(0.5f, 0.5f, 1.0f));
    EXPECT_FLOAT_EQ(modified->intensity, 0.9f);
}

// Test LightManager multiple lights
TEST(LightManagerTest, MultipleLights) {
    LightManager manager;

    // Create multiple lights with different properties
    DirectionalLight light1, light2, light3;
    light1.position = glm::vec3(1.0f, 0.0f, 0.0f);
    light1.color = glm::vec3(1.0f, 0.0f, 0.0f);

    light2.position = glm::vec3(0.0f, 1.0f, 0.0f);
    light2.color = glm::vec3(0.0f, 1.0f, 0.0f);

    light3.position = glm::vec3(0.0f, 0.0f, 1.0f);
    light3.color = glm::vec3(0.0f, 0.0f, 1.0f);

    LightID id1 = manager.AddLight(light1).value();
    LightID id2 = manager.AddLight(light2).value();
    LightID id3 = manager.AddLight(light3).value();

    // Verify all lights are different
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);

    // Verify each light has correct properties
    DirectionalLight* retrieved1 = manager.GetLight(id1);
    DirectionalLight* retrieved2 = manager.GetLight(id2);
    DirectionalLight* retrieved3 = manager.GetLight(id3);

    ASSERT_NE(retrieved1, nullptr);
    ASSERT_NE(retrieved2, nullptr);
    ASSERT_NE(retrieved3, nullptr);

    EXPECT_EQ(retrieved1->color, glm::vec3(1.0f, 0.0f, 0.0f));
    EXPECT_EQ(retrieved2->color, glm::vec3(0.0f, 1.0f, 0.0f));
    EXPECT_EQ(retrieved3->color, glm::vec3(0.0f, 0.0f, 1.0f));
}

// Test LightManager create after remove
TEST(LightManagerTest, CreateAfterRemove) {
    LightManager manager;

    // Create three lights
    LightID id1 = manager.CreateLight(TODO).value();
    LightID id2 = manager.CreateLight(TODO).value();
    LightID id3 = manager.CreateLight(TODO).value();

    // Remove middle light
    manager.RemoveLight(id2);

    // Create a new light (should get ID 3, not reuse ID 2)
    LightID id4 = manager.CreateLight(TODO).value();
    EXPECT_EQ(id4, 3u);

    // Verify id2 still doesn't exist
    EXPECT_EQ(manager.GetLight(id2), nullptr);

    // Verify others still exist
    EXPECT_NE(manager.GetLight(id1), nullptr);
    EXPECT_NE(manager.GetLight(id3), nullptr);
    EXPECT_NE(manager.GetLight(id4), nullptr);
}

// Test LightManager stress test with many lights
TEST(LightManagerTest, ManyLights) {
    LightManager manager;

    const int NUM_LIGHTS = 100;
    std::vector<LightID> ids;

    // Create many lights
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        DirectionalLight light;
        light.position = glm::vec3(i * 1.0f, i * 2.0f, i * 3.0f);
        light.intensity = i / 100.0f;
        LightID id = manager.AddLight(light).value();
        ids.push_back(id);
    }

    // Verify all lights exist with correct properties
    for (int i = 0; i < NUM_LIGHTS; ++i) {
        DirectionalLight* light = manager.GetLight(ids[i]);
        ASSERT_NE(light, nullptr);
        EXPECT_FLOAT_EQ(light->position.x, i * 1.0f);
        EXPECT_FLOAT_EQ(light->position.y, i * 2.0f);
        EXPECT_FLOAT_EQ(light->position.z, i * 3.0f);
        EXPECT_FLOAT_EQ(light->intensity, i / 100.0f);
    }

    // Remove every other light
    for (int i = 0; i < NUM_LIGHTS; i += 2) {
        manager.RemoveLight(ids[i]);
    }

    // Verify removed lights don't exist
    for (int i = 0; i < NUM_LIGHTS; i += 2) {
        EXPECT_EQ(manager.GetLight(ids[i]), nullptr);
    }

    // Verify remaining lights still exist
    for (int i = 1; i < NUM_LIGHTS; i += 2) {
        EXPECT_NE(manager.GetLight(ids[i]), nullptr);
    }
}

// Test LightManager light with all properties set
TEST(LightManagerTest, CompleteLight) {
    LightManager manager;

    // Create a light with all properties customized
    DirectionalLight light;
    light.position = glm::vec3(10.0f, 20.0f, 30.0f);
    light.color = glm::vec3(0.8f, 0.6f, 0.4f);
    light.diffuse = glm::vec3(0.9f, 0.8f, 0.7f);
    light.specular = glm::vec3(0.7f, 0.7f, 0.7f);
    light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light.intensity = 0.85f;

    LightID id = manager.AddLight(light).value();
    DirectionalLight* retrieved = manager.GetLight(id);

    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->position, light.position);
    EXPECT_EQ(retrieved->color, light.color);
    EXPECT_EQ(retrieved->diffuse, light.diffuse);
    EXPECT_EQ(retrieved->specular, light.specular);
    EXPECT_EQ(retrieved->ambient, light.ambient);
    EXPECT_FLOAT_EQ(retrieved->intensity, light.intensity);
}

// Test LightManager add and remove same light multiple times
TEST(LightManagerTest, AddRemoveCycle) {
    LightManager manager;

    for (int cycle = 0; cycle < 5; ++cycle) {
        // Create a light
        LightID id = manager.CreateLight(TODO).value();
        EXPECT_NE(manager.GetLight(id), nullptr);

        // Remove it
        manager.RemoveLight(id);
        EXPECT_EQ(manager.GetLight(id), nullptr);
    }

    // Manager should still be functional
    LightID final_id = manager.CreateLight(TODO).value();
    EXPECT_NE(manager.GetLight(final_id), nullptr);
}