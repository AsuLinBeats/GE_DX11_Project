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
	matrices.clear(); // clear matrices to avoid some bugs
	for (unsigned int i = 0; i < count; i++) {
		float x = static_cast<float>(rand() % 30);
		float z = static_cast<float>(rand() % 30);
		Matrix w2;
		w2 = Matrix::worldTrans(Vec3(0.01, 0.01, 0.01), Vec3(0, 0, 0), Vec3(x, 0, z));
		matrices.push_back(w2);
	}
}

void SaveTrees( std::vector<Matrix>& matrices) {
	// use fstream to save random tree position into binary file
	std::ofstream file("world.dat", std::ios::binary);
	if (!file.is_open()) return;

	size_t count = matrices.size();
	file.write(reinterpret_cast<const char*>(&count), sizeof(count));
	file.write(reinterpret_cast<const char*>(matrices.data()), sizeof(Matrix) * count);
	file.close();
}

bool LoadTrees(std::vector<Matrix>& matrices, const std::string& filename) {
	// load file, set it to bool so the game can check if world file exist to decide generate a new one or just load
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) return false;

	size_t count;
	file.read(reinterpret_cast<char*>(&count), sizeof(count));
	matrices.resize(count);
	file.read(reinterpret_cast<char*>(matrices.data()), sizeof(Matrix) * count);
	file.close();
	return true;
}

class FPS {
public:
	int frame = 0;
	float elapsed = 0;
	int fps = 0;

	void update(float dt) {
		frame++;
		elapsed += dt;

		if (elapsed >= 1.0f) { // Update every second
			fps = frame;
			frame = 0;
			elapsed = 0.0f;
		}
	}

	int getFps() {
		return fps;
	}

	void draw() {
		std::ostringstream logStream;
		logStream << "FPS: " << fps << "\n";
		DebugLog(logStream.str());
	}
};


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
	shader bulletShad;
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
	shad.initStatic("3D_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore); // basic 3d shader
	animationShad.initAnim("Animation_vertex_shader.txt", "3D_pixel_shader.txt", &dxcore); // 3d animation non texture shader
	textureShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore); // 3d shader with texture
	textureAlphaShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader_alpha.txt", &dxcore); // 3d texture shader with alpha test texture
	animationTextureShad.initAnim("Animation_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore); // 3d animation texture shader
	skyDomeShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore);
	bulletShad.initStatic("3D_vertex_shader.txt", "texture_pixel_shader.txt", &dxcore);
	// TODO initialise stuffs in world
	//tri.init(dxcore); // 2D example
	sphere.init(dxcore, 30, 30, 80, "SkyDome.png");
	enemySolider.initTexture("Soldier1.gem", dxcore, &textureManagerEnemy);
	Uzi.initTexture("Uzi.gem", dxcore, &textureManagerWeapon1);
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
	 pines.init("pine.gem", dxcore, &textureManager);

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
	if (!LoadTrees(trees,"world.dat")) {
		// if there is no file, create one and save
		// TODO one potential improvement: The previous dat file must be deleted if want to change number of trees, maybe can add a number check
		RandomCreation(trees, 10);
		SaveTrees(trees);
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
		fps.draw();

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

		// Get matrices after updating camera
		Matrix lookAt = camera.getViewMatrix();
		Matrix proj = camera.getProjectionMatrix(1);
		Matrix resultMatrix = lookAt * proj;

		//std::ostringstream logStream;
	//logStream << "from: " << from << "\n";
	//logStream << "object: " << object << "\n";
	//logStream << "Right: " << right.x << ", " << right.y << ", " << right.z << "\n";
	//DebugLog(logStream.str());
		//////////////////////////////////SKY DOME//////////////////////////////////////////////////////////

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
		
		textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		textureShad.apply(&dxcore);
		dxcore.devicecontext->PSSetShaderResources(0, 1, &plane.textures.srv);
		Matrix w;
		w = Matrix::scaling(Vec3(50, 50, 50));
		textureShad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		plane.draw(&textureShad, &dxcore);

		/////////////////////////////////////////////GAME LOGIC///////////////////////////////////////////////////
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
		/////////////LOGIC PART///////
		//player.movePlayer(camera, dt);
		//player.update(dt,hitDistance);

		//if (mouseLeftPressed) {
		//	player.shoot(dt, reloadDuration, bulletSpeed);
		//}

		//dinasour.moveNormal(dt, moveDistance, animationTextureShad, dxcore, resultMatrix, textureManager, player, bullet);
		//
		//player.draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);
		//for (auto& b : player.bullets) {
		//	b.render(bulletShad, dxcore, resultMatrix);
		//}
		//dinasour.draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);


		enemySolider.update("rifle aiming idle", dt);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", enemySolider.matrices);
		w1 = Matrix::worldTrans(Vec3(0.02, 0.02, 0.02), Vec3(0, 0, 0), Vec3(-4, 0, 0));
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.apply(&dxcore);
		enemySolider.drawTexture(&dxcore, animationTextureShad, &textureManagerEnemy);



		Vec3 weaponOffset = Vec3(0.4f, -0.3f, 0.5f); // Right, Down, Forward

		// Compute world position for weapon based on camera position and orientation
		Vec3 weaponWorldPos = camera.position
			+ camera.right * weaponOffset.x
			+ camera.upLocal * weaponOffset.y
			+ camera.forward * weaponOffset.z;

		//Matrix3 cameraRotation = Matrix3(
		//	camera.right.x, camera.upLocal.x, camera.forward.x,
		//	camera.right.y, camera.upLocal.y, camera.forward.y, 
		//	camera.right.z, camera.upLocal.z, camera.forward.z
		//);
		
		// TODO PLAYER'S HAND---- SO THE PLAYER'S MODEL CAN USE A SIMPLE RECGANGULAR TO REPRESENT?
		Matrix wn1;
		//  camera should match it
		Uzi.update("Armature|08 Fire", dt);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", Uzi.matrices);
	// 	w1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(M_PI/2, 0, M_PI / 2), Vec3(-4, 10, 0));
		//wn1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), cameraRotation * Vec3(M_PI/2, 0, M_PI/2), weaponWorldPos);
		wn1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(M_PI/2, 0, M_PI/2), weaponWorldPos);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &wn1);
		animationTextureShad.apply(&dxcore);
		Uzi.drawTexture(&dxcore, animationTextureShad, &textureManagerWeapon1);
		


		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		//enemySolider.update("Talking", dt);
		////animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", enemySolider.matrices);
		//w1 = Matrix::worldTrans(Vec3(0.5, 0.5, 0.5), Vec3(0, 0, 0), Vec3(7, 0, 0));
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuSffer", "W", &w1);
		//animationTextureShad.apply(&dxcore);
		//enemySolider.drawTexture(&dxcore, animationTextureShad, &textureManager2);

		// TODO MESHES REQUIRES ALPHA TEST: Grass, Trees, etc, Trees for now, also can try grass or stones
		// generate random trees---Roguelike! it's based on file so map for each time running will be same------can create a complex map generation system in future, seed based
		textureAlphaShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &resultMatrix);
		for (const Matrix& mat : trees) {
			pines.updateWorld(mat, textureAlphaShad, dxcore);
			pines.drawTexture(&dxcore, textureAlphaShad, &textureManager);
		 }

		dxcore.Present();
	}
}
