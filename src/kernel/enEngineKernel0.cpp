#include "enEngineKernel0.hpp"

#include "enComponentClock.hpp"
#include "enComponentLogger.hpp"
#include "enComponentScheduler.hpp"
#include "enComponentStat.hpp"
#include "enComponentThread.hpp"
#include "enComponentInit.hpp"
#include "enComponentRenderdraw.hpp"
#include "enComponentRendercompute.hpp"

void enEngineKernel0::init(){
    register_component( new enComponentClock0( new Clock0 ) );
    register_component( new enComponentLoggerStdout( new LoggerStdout ) );
    register_component( new enComponentScheduler0( new Scheduler0 ) );
    register_component( new enComponentStat0( new Stat0 ) );
    register_component( new enComponentThread0( new Thread0 ) );
    register_component( new enComponentInitGL( new InitGL ) );
    register_component( new enComponentRenderdraw0( new Renderdraw0 ) );
    register_component( new enComponentRendercompute0( new Rendercompute0 ) );
}

void enEngineKernel0::deinit(){
    std::vector< enComponentMeta * > accum;
    accumulate_components( [](enComponentMeta * x){
                               if( x != nullptr )
				   return true;
			       else
				   return false;
	                     },
	                     accum );
    for( auto & i : accum ){
        delete i;
    }
    remove_component_all();
}
