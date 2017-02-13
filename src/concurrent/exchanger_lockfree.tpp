#include <chrono>
#include <cassert>
#include <iostream>

template< class T >
exchanger_lockfree<T>::exchanger_lockfree(){
    _status.store(exchanger_status::EMPTY);
}
template< class T >
exchanger_lockfree<T>::~exchanger_lockfree(){
}
template< class T >
bool exchanger_lockfree<T>::exchange( T & item, long timeout_us ){
    //try exchange with another thread via the exchanger node with specified timeout duration
    std::chrono::high_resolution_clock::time_point time_enter = std::chrono::high_resolution_clock::now();
    while(true){
	//test for timeout constraint
	std::chrono::high_resolution_clock::time_point time_now = std::chrono::high_resolution_clock::now();
	auto diff = time_now - time_enter;
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
	if( duration > timeout_us ){
#ifdef DEBUG_VERBOSE
	    std::cout << "entering: timeout segment 1." << std::endl;
#endif
	    return false;
	}

	switch( _status.load( std::memory_order_acquire ) ){
	case exchanger_status::EMPTY:
	{
	    exchanger_status expected_empty = exchanger_status::EMPTY;
	    if( _status.compare_exchange_weak( expected_empty, exchanger_status::EMPTY_2, std::memory_order_acq_rel ) ){
		//at this point, current active thread is depositing value to be exchanged with 2nd arriving thread
		_val = item;
		_status.store( exchanger_status::WAITING, std::memory_order_release );
		//wait for 2nd thread to arrive
		while(true){
		    time_now = std::chrono::high_resolution_clock::now();
		    diff = time_now - time_enter;
		    duration = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
		    if( duration > timeout_us ){
			break;
		    }
		    //wait for partner thread to exchange
		    exchanger_status expected_exchanging_2 = exchanger_status::EXCHANGING_2;
		    if( _status.compare_exchange_weak( expected_exchanging_2, exchanger_status::EXCHANGING_3, std::memory_order_acq_rel ) ){
			//active thread received deposit from 2nd thread
			item = _val;
			_status.store( exchanger_status::EMPTY, std::memory_order_release );
#ifdef DEBUG_VERBOSE
			std::cout << "entering: exchanged with partner segment 1." << std::endl;
#endif
			return true;
		    }
		}
		exchanger_status expected_exchanging_complete = exchanger_status::EXCHANGING_2;
		if( _status.compare_exchange_weak( expected_exchanging_complete, exchanger_status::EXCHANGING_3, std::memory_order_acq_rel ) ){
		    //active thread received deposit from 2nd thread
		    item = _val;
		    _status.store( exchanger_status::EMPTY, std::memory_order_release );
#ifdef DEBUG_VERBOSE
		    std::cout << "entering: exchanged with partner segment 1." << std::endl;
#endif
		    return true;
		}else{
		    //active thread times out and gives up resource
#ifdef DEBUG_VERBOSE
		    std::cout << "entering: timeout segment 2." << std::endl;
#endif
		    _status.store( exchanger_status::EMPTY, std::memory_order_release );
		    return false;
		}
	    }
	}
	break;
	case exchanger_status::WAITING:
	{
	    exchanger_status expected = exchanger_status::WAITING;
	    if( _status.compare_exchange_weak( expected, exchanger_status::EXCHANGING, std::memory_order_acq_rel ) ){
		//at this point, this is the 2nd thread arriving, exchanging with an active thread
		T val_exchanged = _val;
		_val = item;
		item = val_exchanged;
		_status.store( exchanger_status::EXCHANGING_2, std::memory_order_release ); //signal exchange partner that object has been exchanged
#ifdef DEBUG_VERBOSE
		std::cout << "entering: exchanged with partner segment 2." << std::endl;
#endif
		return true;
	    }
	}
	break;
	default:
	    //retry
	    break;
	}
    }    
    return false;
}
