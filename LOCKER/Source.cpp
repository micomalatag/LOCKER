// Form header files
#include "loginForm.h"
#include "registerForm.h"
#include "homeForm.h"
#include "uploadForm.h"
#include "editForm.h"

#include <msclr/marshal_cppstd.h>
// User Handling header file
#include "Users.h"	
#include <filesystem>
#include <ctime>
#include "json.hpp"
namespace fs = std::filesystem;
using namespace System;
using namespace System::Windows::Forms;

// Window Switching Functions
void startLocker();
void regisWindow();
void homeWindow();
void uploadWindow();
void editWindow();

std::string currUser;
std::string currUserPath = fs::current_path().string() + "\\UserFolders\\" + currUser;
int currPicIndex;


[STAThreadAttribute]
int main(array<String^>^ args) {
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);

	// Start the application
	startLocker();

	return 0;
}

void startLocker() {
	LOCKER::loginForm login;
	Application::Run(% login);
	

	if (login.openRegis) {
		regisWindow();
	}

	if (login.openHome) {
		System::String^ temp = login.currentUser;
		currUser = msclr::interop::marshal_as<std::string>(temp);
		homeWindow();
	}
} // end of startLocker

void regisWindow() {
	LOCKER::registerForm regis;
	regis.ShowDialog();

	if (regis.backToLogin) {
		startLocker();
	}
} // end of regisWindow

void homeWindow() {
	static int currUserIndex = 0;
	std::ifstream inJson(currUserPath + currUser + "\\" + currUser + ".json");
	nlohmann::ordered_json imageJson;
	inJson >> imageJson;
	LOCKER::homeForm home;
	std::string jsonPathStr = currUserPath + currUser + "\\" + currUser + ".json";
	System::String^ jsonPath = gcnew String(jsonPathStr.c_str());
	home.jsonFilePath = jsonPath;
	home.ShowDialog();

	currPicIndex = home.currentPicture;

	if (home.signOff) {
		startLocker();
	}

	if (home.openUpload) {
		uploadWindow();
	}

	if (home.openEditor) {
		editWindow();
	}
} // end of homeWindow

void uploadWindow() {
	LOCKER::uploadForm upload;
	upload.ShowDialog();
	if (upload.backToHomeCancel) {
		homeWindow();
	}
	if (upload.backToHomeSuccess) {

		nlohmann::ordered_json currUserJson;
		System::String^ imgpath = upload.path;
		System::String^ imgname = upload.name;
		System::String^ title = upload.title;
		System::String^ desc = upload.description;
		System::String^ month = upload.month;
		System::String^ year = upload.year;

		std::stringstream timestamp;
		timestamp << time(NULL);
		std::string imgpathstr = msclr::interop::marshal_as<std::string>(imgpath);
		std::string titlestr = msclr::interop::marshal_as<std::string>(title);
		std::string descstr = msclr::interop::marshal_as<std::string>(desc);
		std::string monthstr = msclr::interop::marshal_as<std::string>(month);
		std::string yearstr = msclr::interop::marshal_as<std::string>(year);

		// Individual image info object
		nlohmann::ordered_json imageInfo;
		imageInfo["img_title"] = titlestr;
		imageInfo["img_desc"] = descstr;
		imageInfo["img_month"] = monthstr;
		imageInfo["img_year"] = yearstr;
		imageInfo["imgpath"] = currUserPath + currUser + "\\" + timestamp.str() + ".jpg";

		// Read existing json file if it exists
		std::ifstream inJson(currUserPath + currUser + "\\" + currUser + ".json");
		if (inJson.good()) {
			currUserJson = nlohmann::json::parse(inJson);
		}
		inJson.close();

		// Append new data
		currUserJson["images"].push_back(imageInfo);

		fs::path imgPath = msclr::interop::marshal_as<std::string>(imgname);
		fs::path destPath = currUserPath + currUser + "\\" + timestamp.str() + ".jpg";
		fs::copy_file(imgPath, destPath, fs::copy_options::overwrite_existing);

		// Write updated json file
		std::ofstream outJson(currUserPath + currUser + "\\" + currUser + ".json");
		outJson << std::setw(4) << currUserJson;
		outJson.close();

		homeWindow();
	}
} // end of upload window

void editWindow() {
	static int currUserIndex = 0;
	std::ifstream inJson(currUserPath + currUser + "\\" + currUser + ".json");
	nlohmann::ordered_json imageJson;
	inJson >> imageJson;
	LOCKER::editForm edit;
	std::string jsonPathStr = currUserPath + currUser + "\\" + currUser + ".json";
	System::String^ jsonPath = gcnew String(jsonPathStr.c_str());
	edit.jsonFilePathEdit = jsonPath;
	edit.ShowDialog();

	edit.currentPicEdit = currPicIndex;

	if (edit.saveEntry) {
		homeWindow();
	}
} // end of edit window