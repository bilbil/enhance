#include "file_md5_mesh.hpp"
#include <string>
#include <cassert>

int main( int argc, char** argv ){

    std::string path = argv[1];
    std::pair<bool, file_md5_mesh::data_mesh> ret = file_md5_mesh::process( path );
    assert( ret.first );
    file_md5_mesh::data_mesh d = std::move( ret.second );
    std::cout << "data_mesh: md5version: " << d._md5version << std::endl;
    std::cout << "data_mesh: commandline: " << d._commandline << std::endl;
    std::cout << "data_mesh: numjoints: " << d._numjoints << std::endl;
    std::cout << "data_mesh: nummeshes: " << d._nummeshes << std::endl;

    for( auto i : d._joints ){
	std::cout << "joint name: " << i._name << ", parent index: " << i._parent_index;
	std::cout << ", pos: ";
	for( auto j : i._pos ){
	    std::cout << j << " ";
	}
	std::cout << ", orient: ";
	for( auto j : i._orient ){
	    std::cout << j << " ";
	}
	std::cout << std::endl;
    }

    int index_mesh = 0;
    for( auto i : d._meshes ){
	std::cout << "mesh " <<  index_mesh << " {" << std::endl;
	++index_mesh;
	std::cout << "shader: " << i._shader << std::endl;
	std::cout << "numverts: " << i._numverts << std::endl;
	for( auto j : i._verts ){
	    std::cout << "vert index: " << j._index << ", tex coords: (" << j._tex_coords[0] << ", " << j._tex_coords[1] << "), weight start: " << j._weight_start << ", weight count: " << j._weight_count << std::endl;
	}
	std::cout << "numtris: " << i._numtris << std::endl;
	for( auto j : i._tris ){
	    std::cout << "tri index: " << j._index << ", vert indices: (" << j._vert_indices[0] << ", " << j._vert_indices[1] << ", " << j._vert_indices[2] << ")" << std::endl;
	}
	std::cout << "numweights: " << i._numweights << std::endl;
	for( auto j : i._weights ){
	    std::cout << "weight index: " << j._index << ", joint index: " << j._joint_index << ", weight bias: " << j._weight_bias << ", pos: (" << j._pos[0] << ", " << j._pos[1] << ", " << j._pos[2] << ")" << std::endl;
	}
	std::cout << "}" << std::endl;
    }
    
    return 0;
}