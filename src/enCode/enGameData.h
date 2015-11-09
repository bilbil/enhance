#ifndef EN_GAME_DATA_H
#define EN_GAME_DATA_H

//opengl includes
#include <GLFW/glfw3.h>

#include "GLSceneManager.h"
#include "GLSLProgram.h"
#include "WindowManagerGlfw.h"
#include "WingedEdge.h"
#include "enTPCommon.h"
#include "enScene.h"
#include "enSceneSample.h"

class enGameData {
public:
    enGameData( int iWindowId, int iWidth, int iHeight, std::string strWindowName ){
	//create window
	_ManagerWindow = new WindowManagerGlfw;
	_id_window = iWindowId;
	if( !_ManagerWindow->CreateWindow( _id_window, iWidth, iHeight, strWindowName ) ){
	    cout << "Error creating window" << endl;
	    glfwTerminate();
	    exit( EXIT_FAILURE );
	}
	if( !_ManagerWindow->GetWindow( _Window ) ){
	    cout << "Error getting window" << endl;
	    glfwTerminate();
	    exit( EXIT_FAILURE );
	}
	//initialize thread pool
	_Threadpool = new enTPCommon;
	_Threadpool->SetNumThreads(4);
	_SceneManagers["DEFAULT"] = new GLSceneManager;
	_Scenes["DEFAULT"] = new enScene;
	_ProgramGlsl = new GLSLProgram;
    }
    ~enGameData(){
	delete _ManagerWindow;
	_ManagerWindow = nullptr;
	delete _Threadpool;
	_Threadpool = nullptr;
	for( auto & i : _SceneManagers ){
	    i.second->RunCleanup();
	    delete i.second;
	    i.second = nullptr;
	}
	for( auto & i : _Scenes ){
	    delete i.second;
	    i.second = nullptr;
	}
	delete _ProgramGlsl;
	_ProgramGlsl = nullptr;
    }
    GLFWwindow * _Window;
    enTPCommon * _Threadpool;
    WindowManagerGlfw * _ManagerWindow;
    GLSLProgram * _ProgramGlsl;
    map< std::string, GLSceneManager * > _SceneManagers;
    map< std::string, enScene * > _Scenes;
    int _id_window;
    string _strPathPolyMesh;
};

#endif