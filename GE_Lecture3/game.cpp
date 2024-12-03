#include<iostream>
#include"window.h"
#include"DXCore.h"
// able to also use windows.h
//! MAIN ENTRY POINTS
#include"mesh.h"
#include"shaders.h"
#include"GraphicMath.h"
#include"anime.h"
#include<string>
#include"Texture.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	DXCore dxcore;
	//Triangle tri;
	Plane plane;
	shader shad;
	shader animationShad;
	shader textureShad;
	ConstantBuffer constBuffer;
	Matrix vp1;
	Matrix planeWorld;
	Matrix vp;
	Cube cube;
	Sphere sphere;
	Meshes tree;
	AnimationInstance instance;
	Meshes pine;
	TextureManager textureManager;

	// Mesh mesh;
	// loading dinas;
	float t = 0.f;
	GamesEngineeringBase::Timer timer;
	win.init(1024, 1024, "mywindow");
	dxcore.init(win.width, win.height, win.hwnd, false);
	// initialise once----implement shader and example meshes
	animationShad.initAnim("Animation_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore);
	shad.initStatic("3D_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore);
	textureShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore);
	//tri.init(dxcore); // 2D example
	plane.init(dxcore);
	cube.init(dxcore);
	//sphere.init(dxcore, 30, 50, 3);
	tree.initWithoutTexture("acacia_003.gem",dxcore);
	instance.init("TRex.gem", dxcore); // vertices and animation for dina
	pine.init("pine.gem", dxcore, &textureManager);
	
	//dina.init("TRex.gem", dxcore);
	// dinas.loadAnimation3D("TRex.gem", dxcore);
	// planeWorld.translation(1, 2, 1);
	while (true)
	{
		dxcore.Clear();
		win.processMessages();
		float dt = timer.dt();

		t += dt;
		// camera control
		float fov = 20;
		float nearPlane = 0.1f;
		float farPlane = 100.f;

		Vec3 from = Vec3(11*cos(t), 5, 11*sin(t));
		Vec3 object = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

		Matrix lookAtMatrix = vp.lookAt(from, object, up);
		Matrix perspProjMatrix = vp1.PerPro(1.f, 1.f, 20.f, 100.f, 0.1f);
		Matrix resultMatrix = lookAtMatrix * perspProjMatrix; // Check here for type issues

		Matrix w;
		w = Matrix::scaling(Vec3(10, 10, 10));
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		shad.apply(&dxcore);
		plane.draw(&shad, &dxcore);


		// Working animation example
		animationShad.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		Matrix w1;
		instance.update("Run", dt);
		animationShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", instance.matrices);
		w1 = Matrix::translation(Vec3(2, 0, 0));
		animationShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationShad.apply(&dxcore);
		instance.draw(&dxcore);


		// Working example for texture map
		textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w2;
		w2 = Matrix::scaling(Vec3(0.01f, 0.01f, 0.01f));
		pine.updateWorld(w2, textureShad, dxcore);
		// w2 = Matrix::translation(Vec3(5, 0, 0));
		pine.updateWorld(w2, textureShad, dxcore);
		pine.drawTexture(&dxcore, textureShad,&textureManager);

		//! WORKING STATIC EXAMPLE
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w3;
		w3 = Matrix::scaling(Vec3(0.01f, 0.01f, 0.01f));
		tree.updateWorld(w3, shad, dxcore);
		w3 = Matrix::translation(Vec3(5, 0, 0));
		tree.draw(&dxcore);

		// working example: cube
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w4;
		w4 = Matrix::translation(Vec3(-6, 0, 0));
		cube.updateWorld(w4, shad, dxcore);
		cube.draw(&dxcore);

		dxcore.Present();
	}
}