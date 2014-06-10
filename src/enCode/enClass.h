#ifndef EN_CLASS_H
#define EN_CLASS_H

#include <stdarg.h>
#include <string.h>
#include <vector>

using namespace std;

/*****************************************
enEventData
*****************************************/

union enEventData {
  int                                     valInt;
  float                                   valFloat;
  double                                  valDouble;
  char *                                  valStr;
  enEntity *                              valEntity;
};

/*****************************************
enEventDataType
*****************************************/

enum enEventDataType {
                                          INT = 0,
					  FLOAT,
					  DOUBLE,
					  Str,
					  ENTITY
};

/*****************************************
enEventArg
*****************************************/

class enEventArg {
 public:
  enEventData                             _value;
                                          enEventArg() { _type = INT };
					  enEventArg( int data ) { _type = INT; _value.valInt = data; };
					  enEventArg( float data ) { _type = FLOAT; _value.valFloat = data; };
					  enEventArg( double data ) { _type = DOUBLE; _value.valDouble = data; };
					  enEventArg( const char * data ) { _type = STR; _value.valStr = data; };
					  enEventArg( const enEntity * data ) { _type = ENTITY; _value.valEntity = data; };
  enEventDataType   			  GetType() const { return _type; };
 private:
  enEventDataType                         _type;
};

/*****************************************
enEventInfo
*****************************************/
class enEventInfo {
 public:
  char *                                  _name;
  char *                                  _format;
  int                                     _argcount;
  size_t                                  _argsize;
};

/*****************************************
enClassInfo
*****************************************/
class enClassInfo {
 public:
  const char *                            _class;
  const char *                            _superclass;
  enClassInfo *                           _super;
  enClassInfo *                           _next;
};

/****************************************
enClass
****************************************/
class enClass {
 public:
  static vector< enClassInfo * >          _classtypes;
  static enClassInfo                      _classtype;
  static enClassInfo *                    GetClassInfo( const char * );
  static enClass *                        CreateClassInstance( const char * ); 
  bool                                    PostMsgFloat( const enEventInfo * ev, float time, ...);
  bool                                    PostMsgMilli( const enEventInfo * ev, intt time, ...);
  bool                                    ProcessMsg( const enEventInfo * ev, ...); 
  bool                                    SaveFile( enSave * file ) const;
  bool                                    RestoreFile( enRestore * file );
  static void                             DisplayInfo( const char * );
};

#endif
