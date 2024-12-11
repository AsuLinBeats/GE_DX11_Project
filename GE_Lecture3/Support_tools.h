#pragma once
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
// Test tool in win32 app

// overload symbol for debugging

// TODO THESE SUPPORT FUNCTIONS WILL BE PUT INTO SUPPORT_TOOLS.H AT END
inline std::ostream& operator<<(std::ostream& os, const Matrix& matrix) {
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

inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}

inline std::vector<std::string> loadMeshFileNames(const std::string& filePath) {
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

inline void DebugLog(const std::string& message) {
	OutputDebugStringA(message.c_str()); // Send string to Output window
}

inline void RandomCreation(std::vector<Matrix>& matrices, unsigned int count) {
	matrices.clear(); // clear matrices to avoid some bugs
	for (unsigned int i = 0; i < count; i++) {
		float x = static_cast<float>(rand() % 30);
		float z = static_cast<float>(rand() % 30);
		Matrix w2;
		w2 = Matrix::worldTrans(Vec3(0.01, 0.01, 0.01), Vec3(0, 0, 0), Vec3(x, 0, z));
		matrices.push_back(w2);
	}
}

inline void SaveTrees(std::vector<Matrix>& matrices) {
	// use fstream to save random tree position into binary file
	std::ofstream file("world.dat", std::ios::binary);
	if (!file.is_open()) return;

	size_t count = matrices.size();
	file.write(reinterpret_cast<const char*>(&count), sizeof(count));
	file.write(reinterpret_cast<const char*>(matrices.data()), sizeof(Matrix) * count);
	file.close();
}

inline bool LoadTrees(std::vector<Matrix>& matrices, const std::string& filename) {
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

inline void SaveDina(std::vector<Matrix>& matrices) {
	// use fstream to save random tree position into binary file
	std::ofstream file("worldEnemy.dat", std::ios::binary);
	if (!file.is_open()) return;

	size_t count = matrices.size();
	file.write(reinterpret_cast<const char*>(&count), sizeof(count));
	file.write(reinterpret_cast<const char*>(matrices.data()), sizeof(Matrix) * count);
	file.close();
}

inline bool LoadDina(std::vector<Matrix>& matrices, const std::string& filename) {
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
