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
// Game related
#include"camera.h"
#include"GamesEngineeringBase.h"
#include"Character.h"

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
	Cube skyBox;
	Sphere sphere;
	// stuff in world
	Meshes tkp;
	Meshes tree;
	AnimationInstance instance;
	Meshes pine;
	Meshes junk;
	Meshes bamboo;
	TextureManager textureManager;
	TextureManager textureManager1;
	Camera camera(20,0.1f,100);
	GamesEngineeringBase::SoundManager bgms;
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
	tkp.init("tkpmbcnjw.gem", dxcore, &textureManager);
	plane.init(dxcore); // USE THIS AS PLAYER'S MODEL
	skyBox.init(dxcore);
	//cube.init(dxcore);
	//sphere.init(dxcore, 30, 50, 3);
	tree.initWithoutTexture("acacia_003.gem",dxcore);
	instance.init("TRex.gem", dxcore); // vertices and animation for dina
	// pine.init("pine.gem", dxcore, &textureManager);
	
	junk.init("teraccgda.gem", dxcore, &textureManager);

	// bamboo.init("bamboo.gem", dxcore, &textureManager);
	// dina.init("TRex.gem", dxcore);
	// dinas.loadAnimation3D("TRex.gem", dxcore);
	// planeWorld.translation(1, 2, 1);
	Vec3 from = Vec3(11, 5, 11);
	Vec3 object = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);
	while (true)
	{
		dxcore.Clear();
		win.processMessages();
		float dt = timer.dt();

		t += dt;
		// camera control


		float speed = 10.f;
		Vec3 forward = (object - from).normalise();  // Camera's local forward direction
		Vec3 right = (up.Cross(forward).normalise()); // Camera's local right direction
		Vec3 cameraUp = forward.Cross(right);    // Recompute camera's local up

		if (GetAsyncKeyState('W') & 0x8000) from += forward * speed * dt; // Forward
		if (GetAsyncKeyState('S') & 0x8000) from -= forward * speed * dt; // Backward

		if (GetAsyncKeyState('A') & 0x8000) object += right*speed * dt; // Left
		if (GetAsyncKeyState('D') & 0x8000) object -= right * speed * dt; // Right

		// mouse control
		if (GetAsyncKeyState('J') & 0x8000) object += cameraUp*speed * dt; // Right
		if (GetAsyncKeyState('K') & 0x8000) object -= cameraUp*speed * dt; // Right


		camera.setPostion(from);
		camera.setRotation(object);

		// 检测键盘输入与鼠标输入,然后调用move或者setposition
		// 检测鼠标输入,完成开火操作(左键)
		
		Matrix lookAt = camera.updateCameraMat();
		Matrix perspProj = camera.updateProjectionMat();
		Matrix resultMatrix = lookAt * perspProj;

		//  skybox
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w5;
		w5 = Matrix::scaling(Vec3(100,100,100));
		skyBox.updateWorld(w5, shad, dxcore);
		skyBox.draw(&dxcore);
		//TODO Attatch textures for skybox




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
		w1 = Matrix::worldTrans(Vec3(0.8, 0.8, 0.8), Vec3(0, 0, 0), Vec3(-10, 0, 0));
		animationShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationShad.apply(&dxcore);
		instance.draw(&dxcore);

		// 用一个列表存下场景内部所有的模型以及位置等参数,然后调用他们

		// Working example for texture map
		textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w2;
		w2 = Matrix::worldTrans(Vec3(0.01, 0.01, 0.01), Vec3(0, 0, 0), Vec3(-10, 0, 10));
		//pine.updateWorld(w2, textureShad, dxcore);
		//// w2 = Matrix::translation(Vec3(5, 0, 0));
		//pine.drawTexture(&dxcore, textureShad,&textureManager);

		w2 = Matrix::worldTrans(Vec3(0.03, 0.03, 0.03), Vec3(0, 0, 0), Vec3(0, 0, 0));
		junk.updateWorld(w2, textureShad, dxcore);

		junk.drawTexture(&dxcore, textureShad, &textureManager);

		w2 = Matrix::worldTrans(Vec3(0.3, 0.3, 0.3), Vec3(0, 0, 0), Vec3(7, 0, 0));
		tkp.updateWorld(w2, textureShad, dxcore);
		tkp.drawTexture(&dxcore, textureShad, &textureManager);
		/*bamboo.updateWorld(w2, textureShad, dxcore);
		bamboo.drawTexture(&dxcore, textureShad, &textureManager);*/
		//! WORKING STATIC EXAMPLE
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w3;
		//Matrix temp;
		//Matrix test2;
		//w3 = Matrix::scaling(Vec3(0.01f, 0.01f, 0.01f));
		//tree.updateWorld(w3, shad, dxcore);
		//temp = Matrix::translation(Vec3(10, 0, 0));
		//w3 = w3 * temp;
		//tree.updateWorld(w3, shad, dxcore);

		w3 = w3.worldTrans(Vec3(0.01f, 0.01f, 0.01f), Vec3(), Vec3(10, 0, 0));
		tree.updateWorld(w3, shad, dxcore);

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