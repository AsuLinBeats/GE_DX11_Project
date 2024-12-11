#include<iostream>
#include<fstream>
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
//#include"Player.h"
//#include"Enemy.h"
#include"Support_tools.h"

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	DXCore dxcore;
	//Triangle tri;
	Plane plane;
	//shader shad;
	//shader animationShad;
	//shader textureShad;
	//shader animationTextureShad;
	//shader textureAlphaShad;
	//shader skyDomeShad; // shader for skyDome to fix the mix buffer bug.
	// Shader for G-buffer
	shader textureShadG;
	shader animationTextureShadG;
	shader textureAlphaShadG;
	shader skyDomeShadG; // shader for skyDome to fix the mix buffer bug.
	shader shadowShad;


	ConstantBuffer constBuffer;
	Matrix vp1;
	Matrix planeWorld;
	Matrix vp;
	Matrix light;
	Cube cube;
	// Cube skyBox;
	Sphere sphere;  // will use this for now, if grounded is filled, will use hemisphere to improve performance
	// Hemisphere sphere; 
	// stuff in world
	Meshes tkp;
	Meshes tree;

	//Meshes pine;
	Meshes pines; // only need one instance for each tree
	Meshes junk;
	Meshes bamboo;
	Meshes test;
	// Dynamic stuff
	AnimationInstance dina;
	AnimationInstance testAni;
	AnimationInstance enemySolider;
	AnimationInstance Uzi;
	AnimationInstance AC5;
	TextureManager textureManager;
	TextureManager textureManager1;
	TextureManager textureManager2;
	TextureManager textureManagerGround;
	TextureManager textureManagerEnemy;
	TextureManager textureManagerWeapon;
	TextureManager textureManagerWeapon1;
	FPS fps;
	std::vector<Matrix> trees;
	std::vector<Matrix> dinasours(60);
	///////////////////////
	float reloadDuration = 5.f;
	float moveDistance = 30.f;
	float bulletSpeed = 100.f;
	float hitDistance = 5.f;
	//////////////////////////////////////////GAME OBJECTS/////////////
	Camera camera(20, 0.1f, 100);
	Player player(100.f,40,camera.position,camera.rotation, 4,0.1);
	Enemy dinasour(60);
	Bullet bullet(camera.position,camera.position, bulletSpeed,50.f);


	GamesEngineeringBase::SoundManager bgms; // try bgms again
	// Mesh mesh;
	// loading dinas;
	float t = 0.f;
	GamesEngineeringBase::Timer timer;
	win.init(1024, 1024, "Solider Cadillacs and Dinosaurs"); // Well, salute to FC game Cadillacs and Dinosaurs by capcon!(but this game did not add melee attack :( 
	dxcore.init(win.width, win.height, win.hwnd, false); // dx tool
	// initialise once----implement shader and example meshes
	//shad.initStatic("3D_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore); // basic 3d shader
	//animationShad.initAnim("Animation_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore); // 3d animation non texture shader
	//textureShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore); // 3d shader with texture
	//textureAlphaShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader_alpha.txt", &dxcore); // 3d texture shader with alpha test texture
	//animationTextureShad.initAnim("Animation_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore); // 3d animation texture shader
	//skyDomeShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore);

	//NOT WORKING SHADOW MAP
	//shadowShad.initAnim("vertex_shader_shadow.txt", "G_buffer_pixel_shader_shadow.txt", &dxcore); 
	// bulletShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore);
	// 
	
	// TODO initialise stuffs in world
	//tri.init(dxcore); // 2D example
	sphere.init(dxcore, 30, 30, 80, "SkyDome.png");
	enemySolider.initTexture("Soldier1.gem", dxcore, &textureManagerEnemy);
	Uzi.initTexture("Uzi.gem", dxcore, &textureManagerWeapon1);
	player.collider.vertices = Uzi.vertices;
	AC5.initTexture("Automatic_Carbine_5.gem", dxcore, &textureManagerWeapon);
	dina.initTexture("TRex.gem", dxcore, &textureManager); // vertices and animation for dina
	dinasour.collider.vertices = dina.vertices;
	plane.initTexture(dxcore, "riuvL_2K_BaseColor.jpg"); // USE THIS AS PLAYER'S MODEL

	//tree.initWithoutTexture("acacia_003.gem",dxcore);
	 pines.init("pine.gem", dxcore, &textureManager);

	//junk.init("teraccgda.gem", dxcore, &textureManager);

	// bamboo.init("bamboo.gem", dxcore, &textureManager);
	//skyBox.init(dxcore);

	// deferred shader 
	 textureShadG.initStatic("3D_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dxcore); // 3d shader with texture
	 textureAlphaShadG.initStatic("3D_vertex_shader.txt", "G_buffer_pixel_shader_alpha.txt", &dxcore); // 3d texture shader with alpha test texture
	 animationTextureShadG.initAnim("Animation_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dxcore); // 3d animation texture shader
	 skyDomeShadG.initStatic("3D_vertex_shader.txt", "G_buffer_pixel_shader.txt", &dxcore);



	 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	Vec3 from = Vec3(11, 5, 11);
	Vec3 object = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

	std::string filePath = "C:/Users/Davi/source/GE_DX11_Project/GE_Lecture3/Textures/Assorted1/bazaar/Models/filelist1.txt";
	std::vector<std::string> meshFiles = loadMeshFileNames(filePath);

	int treeCount = 20;
	float mouseSensitivity = 0.001f;
	bool running = true;
	if (!LoadTrees(trees,"world.dat")) {
		// if there is no file, create one and save
		// TODO one potential improvement: The previous dat file must be deleted if want to change number of trees, maybe can add a number check
		RandomCreation(trees, 10);
		SaveTrees(trees);
	}
	
	if (!LoadDina(dinasours, "worldEnemy.dat")) {
		// if there is no file, create one and save
		// TODO one potential improvement: The previous dat file must be deleted if want to change number of trees, maybe can add a number check
		RandomCreation(dinasours, 10);
		SaveDina(dinasours);
	}
	// bgms.loadMusic("bgm.wav"); // still not work....

	while (running)
	{
	//	 bgms.playMusic();
		dxcore.Clear();
		win.processMessages();
		float dt = timer.dt();

		t += dt;
		// camera control
		float speed = 6.f;
		fps.update(dt);
		// TODO REMEMBER TURN FPS DRAW ON AFTER FINISHING
		// fps.draw();

		////////////////////////////////CAMERA CONTROL RELATED///////////////////////////////////////////////
		camera.captureInput(win.hwnd, mouseSensitivity);

		// Process keyboard input for movement
		bool moveForward = win.keys['W'];
		bool moveBackward = win.keys['S'];
		bool moveLeft = win.keys['A'];
		bool moveRight = win.keys['D'];
		bool reset = win.keys['T'];
		bool mouseLeftPressed = (GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		if (win.keys['Q']) break; // put q in here to break the loop, more convenient than put everything in camera class
		camera.processInput(moveForward, moveBackward, moveLeft, moveRight, reset, speed, dt);
		if (mouseLeftPressed) {
			// generate bullet

			// render and draw fire animation
		}
		// Get matrices after updating camera
		Matrix lookAt = camera.getViewMatrix();
		Matrix proj = camera.getProjectionMatrix(1);
		Matrix resultMatrix = lookAt * proj;

		// consider parallel light.
		Matrix l;
		Matrix o;
		light = l.lookAt(from, object ,up) * o.OrthoPro(-10.0f, 10.0f, 10.0f, -10.0f, 100.0f, 0.1f);
		
		//Matrix lookAtLight = Matrix::lookAt(from, object,up);
		//Matrix projLight = Matrix::PerPro(1,1,M_PI/4, 0.1f, 100.f);
		//Matrix resultMatrixLight = lookAtLight * projLight;

		// tester
		//std::ostringstream logStream;
	//logStream << "from: " << from << "\n";
	//logStream << "object: " << object << "\n";
	//logStream << "Right: " << right.x << ", " << right.y << ", " << right.z << "\n";
	//DebugLog(logStream.str());
		//////////////////////////////////BASIC MODEL//////////////////////////////////////////////////////////



		Vec3 weaponOffset = Vec3(0.4f, -0.3f, 0.5f); // Right, Down, Forward

		// Compute world position for weapon based on camera position and orientation
		Vec3 weaponWorldPos = camera.position
			+ camera.right * weaponOffset.x
			+ camera.upLocal * weaponOffset.y
			+ camera.forward * weaponOffset.z;

	//	/////////////////////////////////////////////GAME LOGIC///////////////////////////////////////////////////
		/////////////LOGIC PART///////
		// This should be moved to place before control
		// player.movePlayer(camera, dt, dinasour);
		player.movePlayer(camera, dt);
		player.update(dt,hitDistance);

		if (mouseLeftPressed) {
			player.shoot(dt, reloadDuration, bulletSpeed);
		}

		dinasour.moveNormal(dt, moveDistance, animationTextureShadG, dxcore, resultMatrix, textureManager, player, bullet);
		
		//player.draw("Armature|08 Fire",animationTextureShadG, dt, dxcore, weaponWorldPos, textureManager,resultMatrix);

		//dinasour.draw(animationTextureShadG, dt, dxcore, resultMatrix, textureManager);





	//	//Matrix3 cameraRotation = Matrix3(
	//	//	camera.right.x, camera.upLocal.x, camera.forward.x,
	//	//	camera.right.y, camera.upLocal.y, camera.forward.y, 
	//	//	camera.right.z, camera.upLocal.z, camera.forward.z
	//	//);
	//	
	//	// TODO PLAYER'S HAND---- SO THE PLAYER'S MODEL CAN USE A SIMPLE RECGANGULAR TO REPRESENT?
	//	Matrix wn1;
	//	//  camera should match it
	//	Uzi.update("Armature|08 Fire", dt);
	//	//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
	//	animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", Uzi.matrices);
	//// 	w1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(M_PI/2, 0, M_PI / 2), Vec3(-4, 10, 0));
	//	//wn1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), cameraRotation * Vec3(M_PI/2, 0, M_PI/2), weaponWorldPos);
	//	wn1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(M_PI/2, 0, M_PI/2), weaponWorldPos);
	//	animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &wn1);
	//	animationTextureShad.apply(&dxcore);
	//	Uzi.drawTexture(&dxcore, animationTextureShad, &textureManagerWeapon1);
	//	

	//	// TODO MESHES REQUIRES ALPHA TEST: Grass, Trees, etc, Trees for now, also can try grass or stones
	//	// generate random trees---Roguelike! it's based on file so map for each time running will be same------can create a complex map generation system in future, seed based
	//	textureAlphaShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
	//	for (const Matrix& mat : trees) {
	//		pines.updateWorld(mat, textureAlphaShad, dxcore);
	//		pines.drawTexture(&dxcore, textureAlphaShad, &textureManager);
	//	 }

		/////////////////////////////////////////////////////////////////////////////////Deferred shading ///////////////
		// Implement deferred shading
		//dxcore.devicecontext->OMSetRenderTargets(1, &dxcore.renderTargetView, dxcore.depthStencilView);
		//const float clearColour[1] = { 0.0f}; // Clear to black with full alpha
		//dxcore.devicecontext->ClearRenderTargetView(dxcore.renderTargetView, clearColour);
		//dxcore.devicecontext->ClearDepthStencilView(dxcore.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		
		// Static model, SKY Box
		skyDomeShadG.apply(&dxcore);
		// set world matrix
		Matrix skyDomeWorldMatrix = Matrix::worldTrans(Vec3(1, 1, 1), Vec3(0, 0, 0), camera.position);
		skyDomeShadG.updateConstantVS("SkyDome", "staticMeshBuffer", "W", &skyDomeWorldMatrix);
		dxcore.devicecontext->PSSetShaderResources(0, 1, &sphere.text.srv);
		skyDomeShadG.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);

		// Bind texture (for mixing different G-Buffers, will be normal and depth here)
	
		// sphere.updateWorld(skyDomeWorldMatrix, skyDomeShad, dxcore);
		// Draw the model to G-buffer
		sphere.draw(dxcore);

		textureShadG.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		textureShadG.apply(&dxcore);
		dxcore.devicecontext->PSSetShaderResources(0, 1, &plane.textures.srv);
		Matrix w;
		w = Matrix::scaling(Vec3(50, 50, 50));
		textureShadG.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		plane.draw(&textureShadG, &dxcore);


		// TODO MESHES REQUIRES ALPHA TEST: Grass, Trees, etc, Trees for now, also can try grass or stones
		// generate random trees---Roguelike! it's based on file so map for each time running will be same------can create a complex map generation system in future, seed based
		textureAlphaShadG.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		textureAlphaShadG.apply(&dxcore);
		for (const Matrix& mat : trees) {
			pines.updateWorld(mat, textureAlphaShadG, dxcore);
			pines.drawTexture(&dxcore, textureAlphaShadG, &textureManager);
		 }
 
		/////////////////// Animated model

		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		Matrix w1;
		dina.update("Run", dt);
		//animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dina.matrices);
		w1 = Matrix::worldTrans(Vec3(0.9, 0.9, 0.9), Vec3(0, 0, 0), Vec3(-10, 0, 0));
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.apply(&dxcore);
		dina.drawTexture(&dxcore, animationTextureShadG, &textureManager);


		enemySolider.update("rifle aiming idle", dt);
		//animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", enemySolider.matrices);
		w1 = Matrix::worldTrans(Vec3(0.02, 0.02, 0.02), Vec3(0, 0, 0), Vec3(-4, 0, 0));
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.apply(&dxcore);
		enemySolider.drawTexture(&dxcore, animationTextureShadG, &textureManagerEnemy);

		Matrix wn1;
		//  camera should match it
		Uzi.update("Armature|08 Fire", dt);
		//animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", Uzi.matrices);
		wn1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(M_PI/2, 0, M_PI/2), weaponWorldPos);
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &wn1);
		animationTextureShadG.apply(&dxcore);
		Uzi.drawTexture(&dxcore, animationTextureShadG, &textureManagerWeapon1);
		

		//// shadow mapping and light

		//dxcore.devicecontext->OMSetRenderTargets(0, nullptr, dxcore.depthStencilView);
		//dxcore.devicecontext->ClearDepthStencilView(dxcore.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		//shadowShad.apply(&dxcore);
		//shadowShad.updateConstantVS(" staticMeshBuffer","staticMeshBuffer", "LightVP", &light);
		//// draw shadow
		//for (const Matrix& mat : trees) {
		//	pines.updateWorld(mat, shadowShad, dxcore);
		//	pines.drawTexture(&dxcore, shadowShad, &textureManager);
		//}

		//dina.drawTexture(&dxcore, shadowShad, &textureManager);

		//enemySolider.drawTexture(&dxcore, shadowShad, &textureManagerEnemy);

		//Uzi.drawTexture(&dxcore, shadowShad, &textureManagerWeapon1);
		// render all model using shadow shader to get their shadow
		
		//dxcore.devicecontext->OMSetRenderTargets(1, &dxcore.backbufferRenderTargetView, dxcore.depthStencilView);

		//// Bind G-buffer and shadow map
		//dxcore.devicecontext->PSSetShaderResources(0, 1, &dxcore.srv);
		//dxcore.devicecontext->PSSetShaderResources(1, 1, &dxcore.normalSRV);
		//dxcore.devicecontext->PSSetShaderResources(2, 1, &dxcore.shadowSRV);

		//// Full-screen quad rendering
		//shadowShad.updateConstantPS("LightData", "lightBuffer", "lightViewProj", &light);
		//shadowShad.apply(&dxcore);
		//dxcore.devicecontext->Draw(3, 0);
		// present G-buffer
		dxcore.Present();
	}
}
