#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <map>
#include <functional>
#include <bitset>
#include <string>
#include <vector>

struct KeyButtonWhich {
    enum Enum {
	MOUSE_L = 0,
	MOUSE_R,
	MOUSE_M,
	KEY_W,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_SPC,	
	KEY_J,
	KEY_K,
	KEY_L,
	ENUM_COUNT
    };
};

struct KeyButtonState {
    enum  Enum {
	UP = 0,
	DOWN = 1,
	REPEAT = 2,
	ENUM_COUNT
    };
};

class KeyButtonData {
public:
    union {
	struct {
	    unsigned char mouse_l;
	    unsigned char mouse_r;
	    unsigned char mouse_m;
	    unsigned char key_w;
	    unsigned char key_a;
	    unsigned char key_s;
	    unsigned char key_d;
	    unsigned char key_spc;
	    unsigned char key_j;
	    unsigned char key_k;
	    unsigned char key_l;
	} Single;
	unsigned char Array [ KeyButtonWhich::ENUM_COUNT ];
    };
    KeyButtonData(){
	Clear();
    }
    void Clear(){
	for( int i = 0; i < KeyButtonWhich::ENUM_COUNT; i++ ){
	    Array[i] = 0;
	}
    }
};

class WindowManager{
public:
    WindowManager() : _iId(-1) {
	_KeyButtonDataCurrent.Clear();
    }
    virtual bool CreateWindow( int & iId, unsigned int ui_width, unsigned int ui_height, std::string const strTitle )=0;
    virtual bool SetSize( unsigned int ui_width, unsigned int ui_height ) = 0;
    virtual bool GetSize( unsigned int & ui_width, unsigned int & ui_height ) = 0;
    virtual bool SetFullScreen( bool bFullScreen ) = 0;
    virtual bool CloseWindow() = 0;
    virtual bool GetCursorPos( double & xpos, double & ypos ) = 0;
    virtual bool GetCursorState( KeyButtonWhich::Enum which, KeyButtonState::Enum & state ) = 0;
    virtual bool SetKeyComboCallback( std::map<KeyButtonWhich::Enum, KeyButtonState::Enum> combo, std::function<bool(void)> cb ) = 0;
    virtual bool ProcessKeyButtonCombo() = 0;

protected:
    int _iId;
    //TODO: add trie data structure for storing combo key callbacks
    KeyButtonData _KeyButtonDataCurrent; //saves current state of keys and buttons
};

#endif