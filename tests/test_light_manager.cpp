#include <gtest/gtest.h>
#include <Engine/Systems/LightManager.hpp>

// ─── Load ─────────────────────────────────────────────────────────────────

TEST(LightManagerTest, LoadDirectionalLightReturnsID) {
    LightManager manager;
    auto id = manager.Load<DirectionalLight>();
    ASSERT_TRUE(id.has_value());
    EXPECT_EQ(id.value(), 0u);
}

TEST(LightManagerTest, LoadAssignsSequentialIDs) {
    LightManager manager;
    auto id1 = manager.Load<DirectionalLight>().value();
    auto id2 = manager.Load<DirectionalLight>().value();
    auto id3 = manager.Load<PointLight>().value();
    EXPECT_EQ(id1, 0u);
    EXPECT_EQ(id2, 1u);
    EXPECT_EQ(id3, 2u);
}

TEST(LightManagerTest, LoadPointLight) {
    LightManager manager;
    auto id = manager.Load<PointLight>();
    ASSERT_TRUE(id.has_value());
}

TEST(LightManagerTest, LoadSpotLight) {
    LightManager manager;
    auto id = manager.Load<SpotLight>();
    ASSERT_TRUE(id.has_value());
}

// ─── GetLight ─────────────────────────────────────────────────────────────

TEST(LightManagerTest, GetDirectionalLightAfterLoad) {
    LightManager manager;
    LightID id = manager.Load<DirectionalLight>().value();
    DirectionalLight* light = manager.GetLight<DirectionalLight>(id);
    ASSERT_NE(light, nullptr);
}

TEST(LightManagerTest, GetLightReturnsDefaultDirection) {
    LightManager manager;
    LightID id = manager.Load<DirectionalLight>().value();
    DirectionalLight* light = manager.GetLight<DirectionalLight>(id);
    ASSERT_NE(light, nullptr);
    EXPECT_EQ(light->direction, glm::vec3(0.f, 0.f, 0.f));
    EXPECT_EQ(light->color, glm::vec3(1.f, 1.f, 1.f));
}

TEST(LightManagerTest, GetLightInvalidIDReturnsNull) {
    LightManager manager;
    DirectionalLight* light = manager.GetLight<DirectionalLight>(999u);
    EXPECT_EQ(light, nullptr);
}

TEST(LightManagerTest, GetLightWrongTypeReturnsNull) {
    LightManager manager;
    LightID id = manager.Load<DirectionalLight>().value();
    // Requesting as PointLight should return null (dynamic_cast fails)
    PointLight* light = manager.GetLight<PointLight>(id);
    EXPECT_EQ(light, nullptr);
}

TEST(LightManagerTest, GetLightViaBasePointer) {
    LightManager manager;
    LightID id = manager.Load<DirectionalLight>().value();
    LightBase* light = manager.GetLight<LightBase>(id);
    ASSERT_NE(light, nullptr);
    EXPECT_EQ(light->GetType(), LightType::DIRECTIONAL);
}

// ─── Modify light properties ───────────────────────────────────────────────

TEST(LightManagerTest, ModifyLightPropertiesPersist) {
    LightManager manager;
    LightID id = manager.Load<DirectionalLight>().value();
    DirectionalLight* light = manager.GetLight<DirectionalLight>(id);
    ASSERT_NE(light, nullptr);

    light->direction = glm::vec3(0.f, -1.f, 0.f);
    light->color = glm::vec3(0.5f, 0.5f, 1.f);
    light->intensity = 0.9f;

    DirectionalLight* modified = manager.GetLight<DirectionalLight>(id);
    ASSERT_NE(modified, nullptr);
    EXPECT_EQ(modified->direction, glm::vec3(0.f, -1.f, 0.f));
    EXPECT_EQ(modified->color, glm::vec3(0.5f, 0.5f, 1.f));
    EXPECT_FLOAT_EQ(modified->intensity, 0.9f);
}

// ─── RemoveLight ──────────────────────────────────────────────────────────

TEST(LightManagerTest, RemoveLightMakesItUnavailable) {
    LightManager manager;
    LightID id = manager.Load<DirectionalLight>().value();
    ASSERT_NE(manager.GetLight<DirectionalLight>(id), nullptr);

    manager.RemoveLight(id);

    EXPECT_EQ(manager.GetLight<DirectionalLight>(id), nullptr);
}

TEST(LightManagerTest, RemoveNonExistentLightDoesNotCrash) {
    LightManager manager;
    EXPECT_NO_THROW(manager.RemoveLight(999u));
}

TEST(LightManagerTest, ManagerStillFunctionalAfterRemove) {
    LightManager manager;
    LightID id1 = manager.Load<DirectionalLight>().value();
    manager.RemoveLight(id1);
    LightID id2 = manager.Load<DirectionalLight>().value();
    // New ID should increment from previous
    EXPECT_EQ(id2, 1u);
    EXPECT_NE(manager.GetLight<DirectionalLight>(id2), nullptr);
}

// ─── Multiple lights ──────────────────────────────────────────────────────

TEST(LightManagerTest, MultipleLightsDifferentTypes) {
    LightManager manager;
    LightID dirId   = manager.Load<DirectionalLight>().value();
    LightID pointId = manager.Load<PointLight>().value();
    LightID spotId  = manager.Load<SpotLight>().value();

    EXPECT_NE(dirId, pointId);
    EXPECT_NE(pointId, spotId);

    auto* dir   = manager.GetLight<DirectionalLight>(dirId);
    auto* point = manager.GetLight<PointLight>(pointId);
    auto* spot  = manager.GetLight<SpotLight>(spotId);

    ASSERT_NE(dir, nullptr);
    ASSERT_NE(point, nullptr);
    ASSERT_NE(spot, nullptr);

    EXPECT_EQ(dir->GetType(),   LightType::DIRECTIONAL);
    EXPECT_EQ(point->GetType(), LightType::POINT);
    EXPECT_EQ(spot->GetType(),  LightType::SPOT);
}

// ─── Stress test ──────────────────────────────────────────────────────────

TEST(LightManagerTest, ManyLightsAllRetrievable) {
    LightManager manager;
    const int N = 100;
    std::vector<LightID> ids;

    for (int i = 0; i < N; ++i) {
        ids.push_back(manager.Load<PointLight>().value());
        auto* light = manager.GetLight<PointLight>(ids.back());
        ASSERT_NE(light, nullptr);
        light->position = glm::vec3(float(i), 0.f, 0.f);
    }

    for (int i = 0; i < N; ++i) {
        auto* light = manager.GetLight<PointLight>(ids[i]);
        ASSERT_NE(light, nullptr);
        EXPECT_FLOAT_EQ(light->position.x, float(i));
    }
}

TEST(LightManagerTest, RemoveEvenIndexedLights) {
    LightManager manager;
    const int N = 10;
    std::vector<LightID> ids;

    for (int i = 0; i < N; ++i)
        ids.push_back(manager.Load<DirectionalLight>().value());

    for (int i = 0; i < N; i += 2)
        manager.RemoveLight(ids[i]);

    for (int i = 0; i < N; i += 2)
        EXPECT_EQ(manager.GetLight<DirectionalLight>(ids[i]), nullptr);

    for (int i = 1; i < N; i += 2)
        EXPECT_NE(manager.GetLight<DirectionalLight>(ids[i]), nullptr);
}