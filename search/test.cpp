#include <catch.hpp>

#include "search.h"

TEST_CASE("Search") {
    const std::string_view text =
        "If you dream of adventures, you probably picture to yourself desert islands,\n"
        "wrecks of sunken ships and old chests filled with jewellery and coins.\n"
        "For some people these dreams have come true.\n"
        "It is because they really know where to look for treasure.\n"
        "One of the best places for treasure hunting is the Atlantic Ocean near the eastern coast of Florida\n"
        "This place is called “Treasure Coast”.\n"
        "Between the 16th and 18th century many Spanish ships sank near the coast\n"
        "and their wrecks were buried in the sand among the coral reefs.\n"
        "These ships sailed from Mexico to Spain and carried gold, silver and precious stones.\n"
        "They also transported Spanish soldiers and governors\n"
        "who were coming back home from the colonies with their own gold.\n"
        "Most ships sank not very far from the coast.\n"
        "There was only one narrow channel, which ran between the massive and dangerous coral reefs.\n"
        "Besides, tropical storms or hurricanes were very common in late summer or early autumn\n"
        "and the ships often broke into pieces.\n"
        "The exploration of the sea near the coast has begun in the twentieth century\n"
        "when records of shipwrecks were found in the Spanish archives.\n"
        "First, there came scuba divers with metal detectors.\n"
        "Then, with the discovery of the first Spanish ship in the early 1970s,\n"
        "many people started treasure hunting.\n"
        "They were looking for anchors, captains’ diaries and obviously for gold, silver and coins.\n"
        "Today, the treasure hunting is an expensive game, which requires professional equipment and expert divers.\n"
        "It is exhausting and quite dangerous.\n"
        "It takes divers long hours to search for a ship under water but if they are lucky, they feel excited.";

    const std::string_view query = "place for treasure hunting";
    const std::vector<std::string_view> expected = { "This place is called “Treasure Coast”.",
                                                    "many people started treasure hunting.",
                                                    "One of the best places for treasure hunting is the Atlantic Ocean "
                                                    "near the eastern coast of Florida",
                                                    "It is because they really know where to look for treasure.",
                                                    "Today, the treasure hunting is an expensive game, which requires "
                                                    "professional equipment and expert divers." };

    SearchEngine search_engine;
    search_engine.BuildIndex(text);

    REQUIRE(expected == search_engine.Search(query, 5));
}

