#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <set>
#include <chrono>

#include "catch.hpp"
#include "stack_lockfree_split_reference.hpp"
#include "stress_pool.hpp"

using namespace std;

TEST_CASE( "stack_lockfree_split_reference stress", "[stress]" ) { 

    e2::dsc::stack_lockfree_split_reference<int> p;
    unsigned int num_threads = 10000;
    stress_pool::stress_put_get_int( num_threads, p );
}
