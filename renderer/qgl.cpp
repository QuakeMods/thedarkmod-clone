#include "precompiled.h"
#include "qgl.h"
#include "tr_local.h"

#if defined(_WIN32)
#include "../sys/win32/win_local.h"
#else
#include "../sys/linux/local.h"
#endif


void GLimp_LoadFunctions(bool inContext) {
	bool GLok = gladLoadGL();
	if (inContext && !GLok) {
		common->Error("Failed to initialize OpenGL functions (glad)");
	}
#if defined(_WIN32)
	bool WGLok = gladLoadWGL(win32.hDC);
	if (inContext && !WGLok) {
		common->Error("Failed to initialize WGL functions (glad)");
	}
#elif defined(__linux__)
	bool GLXok = gladLoadGLX(dpy, scrnum);
	if (inContext && !GLXok) {
		common->Error("Failed to initialize GLX functions (glad)");
	}
#endif
}

#define CHECK_FEATURE(name) GLimp_CheckExtension(#name, GLAD_##name)
bool GLimp_CheckExtension( const char *name, int available ) {
	if ( !available ) {
		common->Printf("^1X^0 - %s not found\n", name);
		return false;
	}
	else {
		common->Printf("^2v^0 - using %s\n", name);
		return true;
	}
}

void GLimp_CheckRequiredFeatures( void ) {
	common->Printf( "Checking required OpenGL features...\n" );
	bool reqs = true;
	reqs = reqs && CHECK_FEATURE(GL_VERSION_3_1);
	reqs = reqs && CHECK_FEATURE(GL_EXT_texture_compression_s3tc);
#if defined(_WIN32)
	reqs = reqs && CHECK_FEATURE(WGL_VERSION_1_0);
	//reqs = reqs && CHECK_FEATURE(WGL_ARB_create_context);
	//reqs = reqs && CHECK_FEATURE(WGL_ARB_create_context_profile);
	reqs = reqs && CHECK_FEATURE(WGL_ARB_pixel_format);
#elif defined(__linux__)
	reqs = reqs && CHECK_FEATURE(GLX_VERSION_1_4);
	//reqs = reqs && CHECK_FEATURE(GLX_ARB_create_context);
	//reqs = reqs && CHECK_FEATURE(GLX_ARB_create_context_profile);
#endif
	if (!reqs) {
		common->Error("OpenGL minimum requirements not satisfied");
	}


	common->Printf( "Checking optional OpenGL extensions...\n" );

	qglGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &glConfig.maxTextureUnits );
	common->Printf( "Max texture units: %d\n", glConfig.maxTextureUnits );
	qglGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &glConfig.maxTextures );
	common->Printf( "Max active textures: %d\n", glConfig.maxTextures );
	if ( glConfig.maxTextures < MAX_MULTITEXTURE_UNITS ) {
		common->Error( "   Too few!\n" );
	}

	int n;
	qglGetIntegerv( GL_MAX_GEOMETRY_OUTPUT_VERTICES_ARB, &n );
	common->Printf( "Max geometry output vertices: %d\n", n );
	qglGetIntegerv( GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_ARB, &n );
	common->Printf( "Max geometry output components: %d\n", n );
	qglGetIntegerv( GL_MAX_VERTEX_ATTRIBS, &n );
	common->Printf( "Max vertex attribs: %d\n", n );

	glConfig.anisotropicAvailable = CHECK_FEATURE(GL_EXT_texture_filter_anisotropic);
	if ( glConfig.anisotropicAvailable ) {
		qglGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &glConfig.maxTextureAnisotropy );
		common->Printf( "    maxTextureAnisotropy: %f\n", glConfig.maxTextureAnisotropy );
	} else {
		glConfig.maxTextureAnisotropy = 1;
	}

	glConfig.arbAssemblyShadersAvailable = CHECK_FEATURE(GL_ARB_vertex_program) && CHECK_FEATURE(GL_ARB_fragment_program);
	glConfig.stencilTexturing = CHECK_FEATURE( GL_ARB_stencil_texturing );
	glConfig.depthBoundsTestAvailable = CHECK_FEATURE(GL_EXT_depth_bounds_test);
	glConfig.geometryShaderAvailable = CHECK_FEATURE(GL_ARB_geometry_shader4);
	glConfig.timerQueriesAvailable = CHECK_FEATURE(GL_ARB_timer_query);
	glConfig.debugGroupsAvailable = CHECK_FEATURE(GL_KHR_debug);
	glConfig.fenceSyncAvailable = CHECK_FEATURE( GL_ARB_sync );
	glConfig.drawBaseVertexAvailable = CHECK_FEATURE( GL_ARB_draw_elements_base_vertex );
	glConfig.textureSwizzleAvailable = CHECK_FEATURE( GL_ARB_texture_swizzle );
	glConfig.bufferStorageAvailable = CHECK_FEATURE( GL_ARB_buffer_storage );
#ifdef _WIN32
	CHECK_FEATURE(WGL_EXT_swap_control);
#endif
}


#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __attribute__ ((visibility ("default")))
#endif
//#4953 http://forums.thedarkmod.com/topic/19979-choose-gpu/
//hint driver to use discrete GPU on a laptop having both integrated and discrete graphics
extern "C" {
	DLLEXPORT int NvOptimusEnablement = 0x00000001;
	DLLEXPORT int AmdPowerXpressRequestHighPerformance = 0x00000001;
}
