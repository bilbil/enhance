#ifndef FILE_MD5_MESH_HPP
#define FILE_MD5_MESH_HPP

#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <iostream>
#include <map>

#include "file_md5_common.hpp"
#include "quat.hpp"

class file_md5_mesh {
public:
    struct joint {
	std::string _name;
	int _parent_index;
	float _pos[3];
	float _orient[3];
	e2::math::quat _rot; //computed for bind pose
    };
    struct vert {
	int _index;
	float _tex_coords[2];
	int _weight_start;
	int _weight_count;
	float _normal[3]; //computed for bind pose
	float _pos[3]; //computed for bind pose
    };
    struct tri {
	int _index;
	int _vert_indices[3];
    };
    struct weight {
	int _index;
	int _joint_index;
	float _weight_bias;
	float _pos[3];
    };
    struct mesh {
	std::string _shader;
	int _numverts;
	int _numtris;
	int _numweights;
	std::vector<vert> _verts;
	std::vector<tri> _tris;
	std::vector<weight> _weights;
	mesh() : _shader(""), _numverts(0), _numtris(0), _numweights(0) {}
    };
    struct data_mesh {
	int _md5version;
	std::string _commandline;
	int _numjoints;
	int _nummeshes;
	std::vector<joint> _joints;
	std::list<mesh> _meshes;
    };
    enum class process_type {
	md5version,
	commandline,
	numjoints,
	nummeshes,
	joints,
	joint,
	mesh,
	shader,
	numverts,
	vert,
	numtris,
	tri,
	numweights,
	weight,
    };

    file_md5_mesh();
	    
    static std::pair<bool, data_mesh> process( std::string file_path );
    
private:

    static bool check_consistency( data_mesh & );
    static bool calc_bind_pose_positions( data_mesh & );
    static bool calc_bind_pose_normals( data_mesh & );

    static bool process_token( std::pair<file_md5_common::token, std::string> t, std::fstream & f, void * );

    //keyword processing
    static bool process_md5version( std::fstream &, void * );
    static bool process_commandline( std::fstream &, void * );
    static bool process_numjoints( std::fstream &, void * );
    static bool process_nummeshes( std::fstream &, void * );
    static bool process_joints( std::fstream &, void * );
    static bool process_joint( std::fstream &, void * );
    static bool process_mesh( std::fstream &, void * );
    static bool process_shader( std::fstream &, void * );
    static bool process_numverts( std::fstream &, void * );
    static bool process_vert( std::fstream &, void * );
    static bool process_numtris( std::fstream &, void * );
    static bool process_tri( std::fstream &, void * );
    static bool process_numweights( std::fstream &, void * );
    static bool process_weight( std::fstream &, void * );

    //helpers
    static std::unordered_map<std::string, file_md5_mesh::process_type > _keyword_map;
    static std::map<file_md5_mesh::process_type, bool(*)(std::fstream &, void *) > _process_map;
};

#endif
