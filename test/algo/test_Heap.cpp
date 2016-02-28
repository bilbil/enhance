#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "Heap.h"

#include <vector>
#include <algorithm>
#include <set>

using namespace std;

TEST_CASE( "Heap", "[Heap]" ) {
    Heap< int, HeapMax > heap_max;
    vector< Heap< int, HeapMax >::Unit > unsorted { { 0, 100 }, { 9, 101 }, { -5, 102 }, { 6, 103 }, { -6, 104 }, { -8, 105 }, };
    vector< Heap< int, HeapMax >::Unit > empty;
    vector< Heap< int, HeapMax >::Unit > retrieve_sorted;
    Heap< int, HeapMax >::Unit retrieve;
    SECTION( "Heap Empty" ) {
	bool bRet;
	heap_max.BuildHeap( empty );
	bRet = heap_max.Heapify( 0 );
	CHECK( bRet );
	bRet = heap_max.GetMax( retrieve );
	CHECK( !bRet );
	bRet = heap_max.ExtractMax( retrieve );
	CHECK( !bRet );
	bRet = heap_max.IncreaseVal( 0, 110 );
	CHECK( !bRet );
    }
    SECTION( "Heap Max" ) {
	bool bRet;
	int val, data;
	heap_max.BuildHeap( unsorted );
	bRet = heap_max.Heapify( 0 );
	CHECK( bRet );
	bRet = heap_max.GetMax( retrieve );
	CHECK( bRet );
	val = retrieve._val;
	data = retrieve._data;
	CHECK( 9 == val );
	CHECK( 101 == data );
	bRet = heap_max.ExtractMax( retrieve );
	val = retrieve._val;
	data = retrieve._data;
	CHECK( 9 == val );
	CHECK( 101 == data );	
	bRet = heap_max.ExtractMax( retrieve );
	CHECK( bRet );
	val = retrieve._val;
	data = retrieve._data;
	CHECK( 6 == val );
	CHECK( 103 == data );
	bRet = heap_max.IncreaseVal( 3, 99 );
	bRet = heap_max.GetMax( retrieve );
	CHECK( bRet );
	val = retrieve._val;
	data = retrieve._data;
	CHECK( 99 == val );
	CHECK( 105 == data );
	heap_max.HeapSort( retrieve_sorted );
	CHECK( 4 == retrieve_sorted.size() );
	int val_prev;
	std::set< int > expected_vals { 99, 0, -5, -6 };
	for( int i = 0; i < retrieve_sorted.size(); ++i ){
	    if( 0 == i ){
		val_prev = retrieve_sorted[i]._val;
	    }
	    int val_current = retrieve_sorted[i]._val;
	    CHECK( val_prev <= val_current );
	    val_prev = val_current;
	    auto it_find = expected_vals.find( val_current );
	    if( it_find != expected_vals.end() ){
		expected_vals.erase( it_find );
	    }
	}
	int size_remain = expected_vals.size();
	CHECK( 0 == size_remain );
    }    
}
