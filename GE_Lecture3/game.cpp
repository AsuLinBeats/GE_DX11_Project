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
// TODO THESE SUPPORT FUNCTIONS WILL BE PUT INTO SUPPORT_TOOLS.H AT END
std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
	os << "[";
	for (int i = 0; i < 4; ++i) {
		os << "[";
		for (int j = 0; j < 4; ++j) {
			os << matrix.a[i][j];
			if (j < 3) os << ", "; // Add commas between elements in a row
		}
		os << "]";
		if (i < 3) os << ",\n "; // Add a newline between rows
	}
	os << "]";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}

std::vector<std::string> loadMeshFileNames(const std::string& filePath) {
	// For testing model purpose, use with Windows powershell command to save name of model into a txt file and load them at once to see what they are
	std::vector<std::string> meshFiles;
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file: " << filePath << std::endl;
		return meshFiles;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty()) {
			meshFiles.push_back(line);
		}
	}
	file.close();
	return meshFiles;
}

void DebugLog(const std::string& message) {
	OutputDebugStringA(message.c_str()); // Send string to Output window
}

void RandomCreation(std::vector<Matrix>& matrices, unsigned int count) {
	for (unsigned int i = 0; i < count; i++) {
		float x = rand() % 30;
		float z = rand() % 30;
		Matrix w2;
		w2 = Matrix::worldTrans(Vec3(0.01, 0.01, 0.01), Vec3(0, 0, 0), Vec3(x, 0, z));
		matrices.push_back(w2);
	}
}

void RenderRandom() {

}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
	Window win;
	DXCore dxcore;
	//Triangle tri;
	Plane plane;
	shader shad;
	shader animationShad;
	shader textureShad;
	shader animationTextureShad;
	shader textureAlphaShad;
	shader skyDomeShad; // shader for skyDome to fix the mix buffer bug.
	ConstantBuffer constBuffer;
	Matrix vp1;
	Matrix planeWorld;
	Matrix vp;
	Cube cube;
	// Cube skyBox;
	Sphere sphere;  // will use this for now, if grounded is filled, will use hemisphere to improve performance
	// Hemisphere sphere; 
	// stuff in world
	Meshes tkp;
	Meshes tree;

	Meshes pine;
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

	std::vector<Matrix> trees;
	Camera camera(20, 0.1f, 100);
	GamesEngineeringBase::SoundManager bgms; // try bgms again
	// Mesh mesh;
	// loading dinas;
	float t = 0.f;
	GamesEngineeringBase::Timer timer;
	win.init(1024, 1024, "mywindow");
	dxcore.init(win.width, win.height, win.hwnd, false); // dx tool
	// initialise once----implement shader and example meshes
	shad.initStatic("3D_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore); // basic 3d shader
	animationShad.initAnim("Animation_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore); // 3d animation non texture shader
	textureShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore); // 3d shader with texture
	textureAlphaShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader_alpha.txt", &dxcore); // 3d texture shader with alpha test texture
	animationTextureShad.initAnim("Animation_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore); // 3d animation texture shader
	skyDomeShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore);
	// TODO initialise stuffs in world
	//tri.init(dxcore); // 2D example
	sphere.init(dxcore, 30, 30, 80, "SkyDome.png");
	enemySolider.initTexture("Soldier1.gem", dxcore, &textureManagerEnemy);
	//Uzi.initTexture("Uzi.gem", dxcore, &textureManagerWeapon);
	AC5.initTexture("Automatic_Carbine_5.gem", dxcore, &textureManagerWeapon);
	dina.initTexture("TRex.gem", dxcore, &textureManager); // vertices and animation for dina
	// tkp.init("tkpmbcnjw.gem", dxcore, &textureManager);
	plane.initTexture(dxcore, "riuvL_2K_BaseColor.jpg"); // USE THIS AS PLAYER'S MODEL
	//skyBox.init(dxcore);

	// wclie.init("wcliegz.gem", dxcore, &textureManager); // a spring
	//test.init("Uzi.gem", dxcore, &textureManager); // a spring
	//testAni.initTexture("Uzi.gem", dxcore, &textureManager);
	//cube.init(dxcore);
	//sky.init(dxcore, 10, 10, 10, "SkyDome.png");
	//tree.initWithoutTexture("acacia_003.gem",dxcore);
	 pine.init("pine.gem", dxcore, &textureManager);

	//junk.init("teraccgda.gem", dxcore, &textureManager);

	// bamboo.init("bamboo.gem", dxcore, &textureManager);

	Vec3 from = Vec3(11, 5, 11);
	Vec3 object = Vec3(0.0f, 0.0f, 0.0f);
	Vec3 up = Vec3(0.0f, 1.0f, 0.0f);

	std::string filePath = "C:/Users/Davi/source/GE_DX11_Project/GE_Lecture3/Textures/Assorted1/bazaar/Models/filelist1.txt";
	std::vector<std::string> meshFiles = loadMeshFileNames(filePath);

	int treeCount = 20;
	float mouseSensitivity = 0.001f;
	bool running = true;
	RandomCreation(trees, 3);

	// bgms.loadMusic("bgm.wav");

	while (running)
	{
	//	 bgms.playMusic();
		dxcore.Clear();
		win.processMessages();
		float dt = timer.dt();

		t += dt;
		// camera control
		float speed = 6.f;

		
		///////////////////////////////////////////////////////////////////////////////////////////////////
		// TODO DEBUG TO FIND ERROR OF MATRIX, WHICH CAUSES CHAOS DISPLAY AND BLUE SCREEN
		// TODO ERROR HAPPENS INSIDE THE FORWARD. AND NOT RELATED WITH NORMALISE
		// TODO idea: replace hemisphere with cube skybox or hemisphere, but sky box for now works.
		win.capture(from, object, up, mouseSensitivity, camera);

		Vec3 forward = (object - from);
		if (forward.Length() == 0 || std::isnan(forward.x)) {
			forward = Vec3(0, 0, -1); // Default forward direction
		}
		else {
			forward = forward.normalise();
		}

		Vec3 right = up.Cross(forward);
		if (right.Length() == 0 || std::isnan(right.x)) {
			right = Vec3(1, 0, 0); // Default right vector
		}
		else {
			right = right.normalise();
		}

		Vec3 cameraUp = forward.Cross(right);
		// Process keyboard input for movement
		if (win.keys['W']) from += forward * speed * dt;
		if (win.keys['S']) from -= forward * speed * dt;

		if (win.keys['A']) {
			Vec3 delta = right * speed * dt;
			from += delta;
			object += delta; 
		}
		if (win.keys['D']) {
			Vec3 delta = right * speed * dt;
			from -= delta;
			object -= delta; 
		}

		if (win.keys['T']) {
			camera.resetCamera(from, object, up);
		}
		if (win.keys['Q']) break;

		// Update camera
		Matrix lookAt = camera.updateCameraMat();
		Matrix perspProj = camera.updateProjectionMat();
		Matrix resultMatrix = lookAt * perspProj;

		std::ostringstream logStream;
		logStream << "from: " << from << "\n";
		logStream << "object: " << object << "\n";
		logStream << "Right: " << right.x << ", " << right.y << ", " << right.z << "\n";
		DebugLog(logStream.str());

		skyDomeShad.apply(&dxcore);
		dxcore.devicecontext->PSSetShaderResources(0, 1, &sphere.text.srv);
		// make sure skybox following camera all time
		Matrix skyDomeWorldMatrix = Matrix::translation(camera.position); // get camera position
		skyDomeShad.updateConstantVS("SkyDome", "staticMeshBuffer", "W", &skyDomeWorldMatrix);
		skyDomeShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		Matrix w0;
		w0 = Matrix::translation(Vec3(1, -10, -10));
		sphere.updateWorld(skyDomeWorldMatrix, skyDomeShad, dxcore);
		sphere.draw(dxcore);
		//////////////////////////////////////////////////////////////////////////////////////////////////
		//for (unsigned i = 0; i < 86; i++) {
		//	// Tester, to load the mesh and confirm their appearance
		//	Meshes meshes;
		//	meshes.init(meshFiles[i], dxcore, &textureManager);

		//	textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		//	Matrix w2;

		//	w2 = Matrix::worldTrans(Vec3(0.03, 0.03, 0.03), Vec3(0, 0, 0), Vec3(0+i, 0, 0+i));
		//	meshes.updateWorld(w2, textureShad, dxcore);

		//	meshes.drawTexture(&dxcore, textureShad, &textureManager);
		//}




		 // skybox
		//shad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		//Matrix w5;
		//w5 = Matrix::scaling(Vec3(100, 100, 100));
		//sky.updateWorld(w5, shad, dxcore);
		//sky.draw(dxcore);

		//TODO Attatch textures for skybox
		// TODO add collider first.
		// TODO velocity should be -9.8 for players and enemies, others is unchange
		// TODO 

		textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		textureShad.apply(&dxcore);
		dxcore.devicecontext->PSSetShaderResources(0, 1, &plane.textures.srv);
		Matrix w;
		w = Matrix::scaling(Vec3(50, 50, 50));
		textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		plane.draw(&textureShad, &dxcore);

		
		// Working animation example
		// TODO MESHES WITH ANIMATION
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		Matrix w1;
		dina.update("Run", dt);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dina.matrices);
		w1 = Matrix::worldTrans(Vec3(0.9, 0.9, 0.9), Vec3(0, 0, 0), Vec3(-10, 0, 0));
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.apply(&dxcore);
		dina.drawTexture(&dxcore, animationTextureShad, &textureManager);


		enemySolider.update("idle", dt);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", enemySolider.matrices);
		w1 = Matrix::worldTrans(Vec3(0.02, 0.02, 0.02), Vec3(0, 0, 0), Vec3(-4, 0, 0));
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.apply(&dxcore);
		enemySolider.drawTexture(&dxcore, animationTextureShad, &textureManagerEnemy);


		//Uzi.update("Armature|00 Pose", dt);
		////animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", Uzi.matrices);
		//w1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(0, 0, 0), Vec3(6, 0, 0));
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		//animationTextureShad.apply(&dxcore);
		//Uzi.drawTexture(&dxcore, animationTextureShad, &textureManagerWeapon);

		//AC5.update("Armature|00 Pose", dt);
		////animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", AC5.matrices);
		//w1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(0, 0, 0), Vec3(6, 0, 0));
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		//animationTextureShad.apply(&dxcore);
		//AC5.drawTexture(&dxcore, animationTextureShad, &textureManagerWeapon);

		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		//enemySolider.update("Talking", dt);
		////animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", enemySolider.matrices);
		//w1 = Matrix::worldTrans(Vec3(0.5, 0.5, 0.5), Vec3(0, 0, 0), Vec3(7, 0, 0));
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuSffer", "W", &w1);
		//animationTextureShad.apply(&dxcore);
		//enemySolider.drawTexture(&dxcore, animationTextureShad, &textureManager2);
		// 用一个列表存下场景内部所有的模型以及位置等参数,然后调用他们

		// TODO MESHES REQUIRES ALPHA TEST: Grass, Trees, etc
		

		// generate random trees---Roguelike! 
		// TODO SAVE THEM IN A FILE AND LOAD WHEN RENDERING
		for (const Matrix& mat : trees) {

			Meshes pines;
			pines.init("pine.gem", dxcore, &textureManager);

			textureAlphaShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);

			pines.updateWorld(mat, textureAlphaShad, dxcore);
			// w2 = Matrix::translation(Vec3(5, 0, 0));
			pines.drawTexture(&dxcore, textureAlphaShad, &textureManager);
		}
		

		// TODO STATIC MESHES WITH TEXTURE
		/*textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		w2 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(0, 0, 0), Vec3(5, 0, 5));

		test.updateWorld(w2, textureShad, dxcore);
		test.drawTexture(&dxcore, textureShad, &textureManager);*/
		// TODO PLAYER RELATED MESHES
		// TODO PLAYER MODEL
		//w2 = Matrix::worldTrans(Vec3(0.03, 0.03, 0.03), Vec3(0, 0, 0), Vec3(0, 0, 0));
		//enemySolider.updateWorld(w2, textureShad, dxcore);

		//enemySolider.drawTexture(&dxcore, textureShad, &textureManager);

		//w2 = Matrix::worldTrans(Vec3(0.3, 0.3, 0.3), Vec3(0, 0, 0), Vec3(7, 0, 0));
		//tkp.updateWorld(w2, textureShad, dxcore);
		//tkp.drawTexture(&dxcore, textureShad, &textureManager);
		/*bamboo.updateWorld(w2, textureShad, dxcore);
		bamboo.drawTexture(&dxcore, textureShad, &textureManager);*/

		dxcore.Present();
	}
}
