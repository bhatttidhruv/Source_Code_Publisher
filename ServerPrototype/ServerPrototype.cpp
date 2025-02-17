/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 1.0                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2018           //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../CppParser/FileSystem/FileSystem.h"
#include <chrono>
#include "../executive/IExecutive.h"

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

std::function<Msg(Msg)> downloadFiles = [](Msg msg) {

	std::cout << "\n Req 6 - This project support converting source code in the server and, with a separate request,";
	std::cout << "one or more converted files back to the local client in downloaded folder, using the communication channel.";
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("downloadFiles");
	reply.attribute("name","../DownloadedPages/" + msg.value("fileName"));
	return reply;
};

std::function<Msg(Msg)> convertFiles = [](Msg msg) {
	
	std::vector<std::string> args;
	std::vector<std::string> cmd;
	std::string delimiter = "##";

	size_t pos = 0;
	std::string token;
	std::string s = msg.value("strArgs");
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		cmd.push_back(token);
		s.erase(0, pos + delimiter.length());
	}
	char array[20][100] = { {0} };
	strcpy_s(array[0], "");
	std::cout << "\n-------- Received command line parameters in CPP  -------- \n";
	for (size_t i = 1; i <= cmd.size(); i++) {
		strcpy_s(array[i], cmd[i - 1].c_str());
		std::cout << cmd[i - 1] << std::endl;
	}
	// converting into char** so that can be passed as argv
	char *ptr_array[20];
	for (int i = 0; i < 20; i++)
		ptr_array[i] = array[i];
	char **tmp_array = ptr_array;

	ObjectFactory factory;
	IExecutiveClass *obj = factory.createClient();
	std::vector<std::string> files;
	files = obj->initialization(cmd.size()+1, tmp_array);
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	reply.command("convertFiles");
	size_t count = 0;
	for (auto item : files)
	{
		std::string countStr = Utilities::Converter<size_t>::toString(++count);
		reply.attribute("file" + countStr, item);
	}
	return reply;
};

std::function<Msg(Msg)> getFiles = [](Msg msg) {
	
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

void PrjReq() {
	std::cout << "\n -------------- Dhruv G BHatti OODProject 4 --------------";
	std::cout << "\n Req 1 - This project uses Microsoft Visual Studio Community 2017 and its C++ Windows Console Projects.";
	std::cout << "\n Req 2 - use Windows Presentation Foundation (WPF) for the Client's user display";
	std::cout << "\n Req 3 - This Project has been assembled by working parts from Projects #1, #2, and #3 into a Client-Server configuration";
	std::cout << "\n The client is comunicating with the remote server for converting files";
	std::cout << "\n Req 4 - This project uses a Graphical User Interface (GUI) for the client that supports navigating remote directories to find a project for conversion,";
	std::cout << "\n and supports displaying the converted results in the client side in web browser.";
	std::cout << "\n Req 8 - This project include an automated test that accepts the server url and remote path3 to your project directory on its command line, ";
	std::cout << "\n invokes the Code Publisher, through its interface, to convert all the project files matching a pattern that accepts *.h and *.cpp files, and then opens the Client GUI's Display view.";
	std::cout << "Note: Close the opened file in browser to use GUI";
}

int main()
{


  //StaticLogger<1>::attach(&std::cout);
  //StaticLogger<1>::start();
  PrjReq();
  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  std::cout << "\n Req 5 - testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("convertFiles",convertFiles);
  server.addMsgProc("downloadFiles", downloadFiles);
  server.processMessages();
  
  Msg msg(serverEndPoint, serverEndPoint);  // send to self
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  std::cout << "\n  press enter to exit";
  std::cin.get();
  std::cout << "\n";

  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();

  std::cout << "Req 7 - This project demonstrate correct operations for two or more concurrent clients from run.bat";

  return 0;
}

