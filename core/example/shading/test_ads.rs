#![feature(use_extern_macros)]

extern crate gl;
extern crate glutin;
extern crate libc;
extern crate rand;

extern crate e2rcore;

use std::mem;
use std::fs::File;
use std::io::BufReader;
use std::str::FromStr;
use std::io::Read;
use std::ffi::CStr;
use std::os::raw::c_char;
use std::fmt;
use std::str;
use rand::Rng;
use std::any::Any;
use std::borrow::BorrowMut;
use std::ops::{ Deref, DerefMut };

use self::glutin::GlContext;

use self::e2rcore::interface::i_ele;
use self::e2rcore::interface::i_window::IWindow;
use self::e2rcore::interface::i_renderobj::IRenderBuffer;
use self::e2rcore::interface::i_renderobj::RenderDevice;
use self::e2rcore::interface::i_renderpass::IRenderPass;
use self::e2rcore::interface::i_renderpass;

use self::e2rcore::implement::window::winglutin::WinGlutin;
use self::e2rcore::implement::capability::capability_gl;
use self::e2rcore::implement::render::util_gl;
use self::e2rcore::implement::math;
use self::e2rcore::implement::render::camera;
use self::e2rcore::implement::render::light;
use self::e2rcore::implement::render::shader_collection;
use self::e2rcore::implement::render::router;
use self::e2rcore::implement::render::mesh;
use self::e2rcore::implement::render::renderdevice_gl;
use self::e2rcore::implement::render::primitive;
use self::e2rcore::implement::render::renderpass_default;

use self::e2rcore::implement::kernel::kernel_render;

pub fn file_open( file_path: & str ) -> Option<String> {
    let path = File::open( file_path ).expect("file path open invalid");
    let mut buf_reader = BufReader::new(path);
    let mut contents = String::new();
    buf_reader.read_to_string( & mut contents );
    Some(contents)
}

fn main() {

    let mut kr = kernel_render::Renderer::init().unwrap();
    
    let vs_src = file_open( "core/example/shading/ads.vs" ).expect("vertex shader not retrieved");
    let fs_src = file_open( "core/example/shading/ads.fs" ).expect("fragment shader not retrieved");

    let mut shader_program = kr.load_shader( &[ ( vs_src.as_str(), util_gl::ShaderType::VERTEX ),
                                                ( fs_src.as_str(), util_gl::ShaderType::FRAGMENT ) ] ).unwrap();
    
    let rp_index = kr.add_renderpass( String::from("ads_pass"), renderpass_default::RenderPassDefault{} );
    
    //camera
    let fov = 120f32;
    let aspect = 1f32;
    let near = 0.001f32;
    let far = 1000f32;
    let cam_foc_pos = math::mat::Mat3x1 { _val: [0f32, 0f32, 5f32] };
    let cam_up = math::mat::Mat3x1 { _val: [0f32, 1f32, 0f32] };
    let cam_pos = math::mat::Mat3x1 { _val: [5f32, 5f32, 20f32] };
    let cam_id = 0;
    let cam = camera::Cam::init( cam_id, fov, aspect, near, far, cam_pos, cam_foc_pos, cam_up );
    
    //create lights
    let mut lights : Vec< light::LightAdsPoint > = vec![];
    let mut rng = rand::thread_rng();
    for i in 0..50 {
        let pos_x = ( (rng.gen::<u8>() % 100) as f32 / 100f32 ) * 6f32 - 3f32;
        let pos_y = ( (rng.gen::<u8>() % 100) as f32 / 100f32 ) * 6f32 - 4f32;
        let pos_z = ( (rng.gen::<u8>() % 100) as f32 / 100f32 ) * 6f32 + 10f32;
        let colour_r = ( (rng.gen::<u8>() % 100) as f32 / 100f32 ) * 1f32;
        let colour_g = ( (rng.gen::<u8>() % 100) as f32 / 100f32 ) * 1f32;
        let colour_b = ( (rng.gen::<u8>() % 100) as f32 / 100f32 ) * 1f32;
        let l = light::LightAdsPoint {
            _id: i as u64,
            _pos: math::mat::Mat3x1 { _val: [ pos_x, pos_y, pos_z ] },
            _ads_val_spec: math::mat::Mat3x1 { _val: [ colour_r, colour_g, colour_b ] },
            _ads_val_diff: math::mat::Mat3x1 { _val: [ colour_r, colour_g, colour_b ] },
            _ads_val_amb: math::mat::Mat3x1 { _val: [ colour_r, colour_g, colour_b ] },
        };
        lights.push( l );
    }

    let ( vao, vbo, draw_group ) = kr.create_draw_group().unwrap();
    
    //set triangle vert positions and normals
    let mut mesh = mesh::Mesh::init( 0 );
    mesh._pos.extend_from_slice( &[ math::mat::Mat3x1 { _val: [-1f32, -1f32, -1f32 ] },
                                    math::mat::Mat3x1 { _val: [ 5f32, -1f32, -1f32 ] },
                                    math::mat::Mat3x1 { _val: [-1f32,  1f32, -1f32 ] },
                                    math::mat::Mat3x1 { _val: [ 4f32, -1f32, 15f32 ] },
                                    math::mat::Mat3x1 { _val: [ 6f32, -1f32, 15f32 ] },
                                    math::mat::Mat3x1 { _val: [ 4f32,  1f32, 15f32 ] }, ] );

    mesh._normal.extend_from_slice( &[ math::mat::Mat3x1 { _val: [ 0f32, 0f32, 1f32 ] },
                                       math::mat::Mat3x1 { _val: [ 0f32, 0f32, 1f32 ] },
                                       math::mat::Mat3x1 { _val: [ 0f32, 0f32, 1f32 ] },
                                       math::mat::Mat3x1 { _val: [ 0f32, 0f32, 1f32 ] },
                                       math::mat::Mat3x1 { _val: [ 0f32, 0f32, 1f32 ] },
                                       math::mat::Mat3x1 { _val: [ 0f32, 0f32, 1f32 ] }, ] );
    
    mesh._tc.extend_from_slice( &[ math::mat::Mat2x1 { _val: [ 0f32, 0f32 ] },
                                   math::mat::Mat2x1 { _val: [ 0f32, 0f32 ] },
                                   math::mat::Mat2x1 { _val: [ 0f32, 0f32 ] },
                                   math::mat::Mat2x1 { _val: [ 0f32, 0f32 ] },
                                   math::mat::Mat2x1 { _val: [ 0f32, 0f32 ] },
                                   math::mat::Mat2x1 { _val: [ 0f32, 0f32 ] }, ] );

    
    let obj_triangle = kr.add_obj( "mesh_triangles", i_ele::Ele::init( mesh ) );
    
    //primitives
    let mut prim_box = primitive::Poly6 { _pos: math::mat::Mat3x1 { _val: [ -5f32, -10f32, 5f32 ] },
                                           _radius: 5f32 };

    let obj_box = kr.add_obj( "box", i_ele::Ele::init( prim_box ) );

    let mut prim_sphere = primitive::SphereIcosahedron::init( math::mat::Mat3x1 { _val: [ -20f32, -10f32, 0f32 ] }, 5f32 );

    let obj_sphere = kr.add_obj( "sphere", i_ele::Ele::init( prim_sphere ) );

    //configure uniform variables
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Material.Ka\0", renderdevice_gl::UniformType::VEC, &[ 0.2f32, 0.2f32, 0.2f32 ] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Material.Kd\0", renderdevice_gl::UniformType::VEC, &[ 0.1f32, 0.1f32, 0.9f32 ] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Material.Ks\0", renderdevice_gl::UniformType::VEC, &[ 0.9f32, 0.1f32, 0.1f32 ] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Material.Shininess\0", renderdevice_gl::UniformType::VEC, &[ 3f32 ] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Light.Position\0", renderdevice_gl::UniformType::VEC, &lights[0]._pos._val[..] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Light.La\0", renderdevice_gl::UniformType::VEC, &lights[0]._ads_val_amb._val[..] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Light.Ld\0", renderdevice_gl::UniformType::VEC, &lights[0]._ads_val_diff._val[..] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"Light.Ls\0", renderdevice_gl::UniformType::VEC, &lights[0]._ads_val_spec._val[..] );
    kr.uniforms_ref().set_group( shader_program as _, 0, [ "Material.Ka\0", "Material.Kd\0", "Material.Ks\0", "Material.Shininess\0",
                                                           "Light.Position\0", "Light.La\0", "Light.Ld\0", "Light.Ls\0" ].into_iter().map(|&x| x.into() ).collect() ).is_ok();
    util_gl::check_last_op();
    
    let model_transform = math::mat::Mat4::<f32> { _val: [ 1f32, 0f32, 0f32, 0f32,
                                                           0f32, 1f32, 0f32, 0f32,
                                                           0f32, 0f32, 1f32, 0f32,
                                                           0f32, 0f32, 0f32, 1f32 ],
                                                    _is_row_major: true };


    let mvp_transform = cam._proj_xform.mul( &cam._view_xform ).unwrap().mul( &model_transform ).unwrap();
    let model_view_transform = cam._view_xform.mul( &model_transform ).unwrap();
    let normal_inv_transpose = model_view_transform.submat_mat3().inverse().unwrap().transpose();

    println!( "mv: {:?}", model_view_transform );
    println!( "mv submat: {:?}", model_view_transform.submat_mat3() );
    println!( "mvp: {:?}", mvp_transform );
    println!( "normal matrix: {:?}", normal_inv_transpose );

    kr.uniforms_ref().set_uniform_f( shader_program as _, &"ModelViewMatrix\0", renderdevice_gl::UniformType::MAT4, &model_view_transform._val[..] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"NormalMatrix\0", renderdevice_gl::UniformType::MAT3, &normal_inv_transpose._val[..] );
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"ProjectionMatrix\0", renderdevice_gl::UniformType::MAT4, &cam._proj_xform._val[..] );        
    kr.uniforms_ref().set_uniform_f( shader_program as _, &"MVP\0", renderdevice_gl::UniformType::MAT4, &mvp_transform._val[..] );
    kr.uniforms_ref().set_group( shader_program as _, 1, [ "ModelViewMatrix\0", "NormalMatrix\0", "ProjectionMatrix\0", "MVP\0" ].into_iter().map(|&x| x.into() ).collect() ).is_ok();

    //todo: move any manipulation of draw groups, binding actions into specific render pass routine (eg: renderpass_default::RenderPassDefault)
    kr.load_objs_to_draw_group( &[ obj_triangle, obj_box, obj_sphere ], draw_group ).is_ok();
    kr.bind_draw_group_data( &[ draw_group ] ).is_ok();
    kr.add_draw_group_uniforms( draw_group, &[0u64,1u64] ).is_ok();
        
    let mut running = true;
    while running {
        let mut new_win_dim = None;
        kr.win_ref().handle_events( |event| {
            match event {
                glutin::Event::WindowEvent{ event, .. } => match event {
                    glutin::WindowEvent::Closed => running = false,
                    glutin::WindowEvent::Resized(w, h) => new_win_dim = Some( (w,h) ),
                    glutin::WindowEvent::KeyboardInput {
                        input: glutin::KeyboardInput {
                            state: glutin::ElementState::Pressed,
                            virtual_keycode: Some( glutin::VirtualKeyCode::Q ),
                            ..
                        }, ..
                    } => running = false,
                    _ => (),
                },
                _ => ()
            }
        } );
        if let Some( ( w, h ) ) = new_win_dim {
            kr.win_ref()._win._wingl.resize(w, h);
        }
        unsafe {
            gl::ClearColor( 0.9, 0.9, 0.9, 1.0 );
            gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
            {
                kr.drawcall_draw_group( &[ draw_group ] ).is_ok();
                kr.reset_draw_group_data( &[ draw_group ] ).is_ok();
                kr.load_objs_to_draw_group( &[ obj_triangle, obj_box, obj_sphere ], draw_group ).is_ok();
                kr.bind_draw_group_data( &[ draw_group ] ).is_ok();
            }
        }
        kr.win_ref().swap_buf();
    }
}