#include <Windows.h>
#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include <memory>
#include "..//Test/qt.h"
using namespace Catch;

TEST_CASE( "Factorials are computed", "[factorial]"){

	REQUIRE(1 == 1);
}

TEST_CASE("Qt can insert", "[Qt]") {

}

TEST_CASE("QuadTree Test", "[Quad Tree]") {

	SECTION("Test Adding") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
	}
	SECTION("Test Adding / Remove") {
		auto qt = factory::create_qt();
		{
			auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		}

	}

	SECTION("Test Adding / Remove x2 ") {
		auto qt = factory::create_qt();
		{
			auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
			auto ast1 = Astroid_Factory::create(postion_t{ 0.1, 0.1 }, 0.05, qt.get());
		}
	}

	SECTION("No double entry") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());

		CHECK_THROWS(qt->add(player.get()));
	}

	SECTION("Test Search") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());

		auto result = qt->get(player->position, player->radius);
		REQUIRE(result.size() == 1); // "Must have something");
		REQUIRE(result[0] == player.get()); // "Must have the player found");		
	}
	SECTION("Test Search Nowhere") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());

		auto result = qt->get(postion_t{ 0.9, 0.9 }, 0.1);
		REQUIRE(result.size() == 0); // "Must have something");
	}

	SECTION("Test Search Nowhere with negs") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());

		auto result = qt->get(postion_t{ -0.9, 0.9 }, -0.1);
		REQUIRE(result.size() == 0); // "Must have something");
	}
	SECTION("Test Search with mulitple junk") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		auto player2 = Player_Factory::create(postion_t{ 0.4, 0.4 }, 0.1, qt.get());
		auto ast1 = Astroid_Factory::create(postion_t{ 0.1, 0.1 }, 0.05, qt.get());
		auto ast2 = Astroid_Factory::create(postion_t{ 0.6, 0.2 }, 0.1, qt.get());
		//auto ast3 = Astroid_Factory::create(postion_t{ 0.3, 0.3 }, 0.4, qt.get());

		auto result = qt->get(postion_t{ 0.2, 0.2 }, 0.1);
		REQUIRE(result.size() == 2); // "Must have something");
	}

	SECTION("Test Search with mulitple junk 2 results") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		auto player2 = Player_Factory::create(postion_t{ 0.4, 0.4 }, 0.1, qt.get());
		auto ast1 = Astroid_Factory::create(postion_t{ 0.2, 0.21 }, 0.05, qt.get());
		auto ast2 = Astroid_Factory::create(postion_t{ 0.6, 0.2 }, 0.1, qt.get());
		auto ast3 = Astroid_Factory::create(postion_t{ 0.3, 0.3 }, 0.4, qt.get());

		auto result = qt->get(postion_t{ 0.2, 0.2 }, 0.1);
		REQUIRE(result.size() == 3); // "Must have something");
	}




	SECTION("Test Remove nothing") {
		auto qt = factory::create_qt();
		auto func = [&qt]() {
			// life scope ofplayer should auto remove.
			{
				auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
				qt->remove(player.get());
			}

			throw std::runtime_error("wut");
		};
		CHECK_THROWS(func());
	}

	SECTION("Test Update") {
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		/*auto player2 = Player_Factory::create(postion_t{ 0.4, 0.4 }, 0.1, qt.get());
		auto ast1 = Astroid_Factory::create(postion_t{ 0.1, 0.1 }, 0.05, qt.get());
		auto ast2 = Astroid_Factory::create(postion_t{ 0.6, 0.2 }, 0.1, qt.get());
		auto ast3 = Astroid_Factory::create(postion_t{ 0.3, 0.3 }, 0.4, qt.get());
		*/
		player->position = { 0.4, 0.4 };
		qt->update(player.get());
	}

	SECTION("Test Updates Correctly")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());


		//qt->add(player.get());
		auto result = qt->get({ 0.0, 0.0 }, 1.0);
		auto inst = result.at(0);

		REQUIRE(result.at(0)->position.x == player.get()->position.x);
		REQUIRE(result.at(0)->position.y == player.get()->position.y);
		player->position = { 0.4, 0.4 };
		qt->update(player.get());
		auto result_two = qt->get({ 0.40, 0.40 }, 0.05);
		REQUIRE(result_two.size() == 1);
		auto inst_two = result_two.at(0);
		REQUIRE(inst_two->position.x == 0.4);
		REQUIRE(inst_two->position.y == player.get()->position.y);
		REQUIRE(inst_two == inst);
	}

	SECTION("Test multplies updates Correctly")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		auto result = qt->get({ 0.0, 0.0 }, 1.0);
		player->position = { 0.4, 0.4 };
		qt->update(player.get());
		result = qt->get({ 0.4, 0.4 }, 0.05);
		player->position = { 0.6, 0.4 };
		qt->update(player.get());
		result = qt->get({ 0.6, 0.4 }, 0.05);
		player->position = { 0.1, 0.4 };
		qt->update(player.get());
		result = qt->get({ 0.1, 0.4 }, 0.05);
		REQUIRE(result.size() == 1);
	}

	SECTION("Test negative values")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ -0.2, -0.2 }, 0.1, qt.get());
		//auto result = qt->get({ 0.0, 0.0 }, 1.0);
		player->position = { -0.05, -0.05 }; // outside center but still inside edge
		qt->update(player.get());
		auto result = qt->get({ -0.05,-0.05 }, 0.5);
		REQUIRE(result.size() == 1);
	}
	SECTION("Test negative values More")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		// auto result = qt->get({ 0.0, 0.0 }, 1.0);
		player->position = { -0.5, -0.5 };  // way outside
		qt->update(player.get());
		auto result = qt->get({ -0.15,-0.05 }, 0.5);
		REQUIRE(result.size() == 1);
	}
	SECTION("Test Multiple Negatives")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ -0.2, 0.2 }, 0.1, qt.get());
		//auto player = Player_Factory::create(postion_t{ 0.2, 0.2 }, 0.1, qt.get());
		auto player2 = Player_Factory::create(postion_t{ 0.4, -0.4 }, 0.1, qt.get());
		player->position = { 0.3, -1.6 };
		player2->position = { -0.8, 1.3 };
		qt->update(player.get());
		qt->update(player2.get());
		auto result = qt->get({ 0,0 }, 2.0);
		REQUIRE(result.size() == 2);
	}

	SECTION("Test negative radius")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ -0.2, 0.2 }, -0.1, qt.get());
		auto result = qt->get({ 0,0 }, -2.0);
		//REQUIRE(result.size() == 1);
	}

	SECTION("Edge Casing")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.5, 0.5 }, 0.1, qt.get());
		auto result = qt->get({ 0.5,0.5 }, 0.05);
		REQUIRE(result.size() == 1);
	}

	SECTION("Radius 0 Test")
	{
		auto qt = factory::create_qt();
		auto player = Player_Factory::create(postion_t{ 0.5, 0.5 }, 0.0, qt.get());
		auto result = qt->get({ 0.5,0.5 }, 0.05);
		REQUIRE(result.size() == 1);
	}

}
