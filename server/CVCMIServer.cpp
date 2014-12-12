#include "StdInc.h"

#include <boost/asio.hpp>

#include "../lib/filesystem/Filesystem.h"
#include "../lib/mapping/CCampaignHandler.h"
#include "../lib/CThreadHelper.h"
#include "../lib/Connection.h"
#include "../lib/CModHandler.h"
#include "../lib/CArtHandler.h"
#include "../lib/CGeneralTextHandler.h"
#include "../lib/CHeroHandler.h"
#include "../lib/CTownHandler.h"
#include "../lib/CBuildingHandler.h"
#include "../lib/CSpellHandler.h"
#include "../lib/CCreatureHandler.h"
#include "zlib.h"
#include "CVCMIServer.h"
#include "../lib/StartInfo.h"
#include "../lib/mapping/CMap.h"
#ifndef VCMI_ANDROID
#include "../lib/Interprocess.h"
#endif
#include "../lib/VCMI_Lib.h"
#include "../lib/VCMIDirs.h"
#include "CGameHandler.h"
#include "../lib/mapping/CMapInfo.h"
#include "../lib/GameConstants.h"
#include "../lib/logging/CBasicLogConfigurator.h"
#include "../lib/CConfigHandler.h"
#include "../lib/ScopeGuard.h"

#include "../lib/UnlockGuard.h"

#if defined(__GNUC__) && !defined (__MINGW32__) && !defined(VCMI_ANDROID)
#include <execinfo.h>
#endif

#ifndef VCMI_IOS
std::string NAME_AFFIX = "server";
std::string NAME = GameConstants::VCMI_VERSION + std::string(" (") + NAME_AFFIX + ')'; //application name
#else
std::string SERVER_NAME_AFFIX = "server";
std::string SERVER_NAME = GameConstants::VCMI_VERSION + std::string(" (") + SERVER_NAME_AFFIX + ')'; //application name
#endif
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
#ifndef VCMI_ANDROID
namespace intpr = boost::interprocess;
#endif
bool end2 = false;
int port = 3030;

#ifndef VCMI_IOS
boost::program_options::variables_map cmdLineOptions;
#endif
/*
 * CVCMIServer.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

static void vaccept(tcp::acceptor *ac, tcp::socket *s, boost::system::error_code *error)
{
	ac->accept(*s,*error);
}



CPregameServer::CPregameServer(CConnection *Host, TAcceptor *Acceptor /*= nullptr*/)
	: host(Host), listeningThreads(0), acceptor(Acceptor), upcomingConnection(nullptr),
	  curmap(nullptr), curStartInfo(nullptr), state(RUNNING)
{
	initConnection(host);
}

void CPregameServer::handleConnection(CConnection *cpc)
{
	setThreadName("CPregameServer::handleConnection");
	try
	{
		while(!cpc->receivedStop)
		{
			CPackForSelectionScreen *cpfs = nullptr;
			*cpc >> cpfs;

            logNetwork->infoStream() << "Got package to announce " << typeid(*cpfs).name() << " from " << *cpc;

			boost::unique_lock<boost::recursive_mutex> queueLock(mx);
			bool quitting = dynamic_cast<QuitMenuWithoutStarting*>(cpfs), 
				startingGame = dynamic_cast<StartWithCurrentSettings*>(cpfs);
			if(quitting || startingGame) //host leaves main menu or wants to start game -> we end
			{
				cpc->receivedStop = true;
				if(!cpc->sendStop)
					sendPack(cpc, *cpfs);

				if(cpc == host)
					toAnnounce.push_back(cpfs);
			}
			else
				toAnnounce.push_back(cpfs);

			if(startingGame)
			{
				//wait for sending thread to announce start
				auto unlock = vstd::makeUnlockGuard(mx);
				while(state == RUNNING) boost::this_thread::sleep(boost::posix_time::milliseconds(50));
			}
		}
	}
	catch (const std::exception& e)
	{
		boost::unique_lock<boost::recursive_mutex> queueLock(mx);
        logNetwork->errorStream() << *cpc << " dies... \nWhat happened: " << e.what();
	}

	boost::unique_lock<boost::recursive_mutex> queueLock(mx);
	if(state != ENDING_AND_STARTING_GAME)
	{
		connections -= cpc;

		//notify other players about leaving
		auto pl = new PlayerLeft();
		pl->playerID = cpc->connectionID;
		announceTxt(cpc->name + " left the game");
		toAnnounce.push_back(pl);

		if(connections.empty())
		{
            logNetwork->errorStream() << "Last connection lost, server will close itself...";
			boost::this_thread::sleep(boost::posix_time::seconds(2)); //we should never be hasty when networking
			state = ENDING_WITHOUT_START;
		}
	}

    logNetwork->infoStream() << "Thread listening for " << *cpc << " ended";
	listeningThreads--;
	vstd::clear_pointer(cpc->handler);
}

void CPregameServer::run()
{
	startListeningThread(host);
	start_async_accept();

	while(state == RUNNING)
	{
		{
			boost::unique_lock<boost::recursive_mutex> myLock(mx);
			while(!toAnnounce.empty())
			{
				processPack(toAnnounce.front());
				toAnnounce.pop_front();
			}

// 			//we end sending thread if we ordered all our connections to stop
// 			ending = true;
// 			for(CPregameConnection *pc : connections)
// 				if(!pc->sendStop)
// 					ending = false;

			if(state != RUNNING)
			{
                logNetwork->infoStream() << "Stopping listening for connections...";
				acceptor->close();
			}

			if(acceptor)
			{
				acceptor->get_io_service().reset();
				acceptor->get_io_service().poll();
			}
		} //frees lock

		boost::this_thread::sleep(boost::posix_time::milliseconds(50));
	}

    logNetwork->infoStream() << "Thread handling connections ended";

	if(state == ENDING_AND_STARTING_GAME)
	{
        logNetwork->infoStream() << "Waiting for listening thread to finish...";
		while(listeningThreads) boost::this_thread::sleep(boost::posix_time::milliseconds(50));
        logNetwork->infoStream() << "Preparing new game";
	}
}

CPregameServer::~CPregameServer()
{
	delete acceptor;
	delete upcomingConnection;

	for(CPackForSelectionScreen *pack : toAnnounce)
		delete pack;

	toAnnounce.clear();

	//TODO pregameconnections
}

void CPregameServer::connectionAccepted(const boost::system::error_code& ec)
{
	if(ec)
	{
        logNetwork->infoStream() << "Something wrong during accepting: " << ec.message();
		return;
	}

    logNetwork->infoStream() << "We got a new connection! :)";
#ifndef VCMI_IOS
	CConnection *pc = new CConnection(upcomingConnection, NAME);
#else
    CConnection *pc = new CConnection(upcomingConnection, SERVER_NAME);
#endif
	initConnection(pc);
	upcomingConnection = nullptr;

	startListeningThread(pc);

	*pc << (ui8)pc->connectionID << curmap;

	announceTxt(pc->name + " joins the game");
	auto pj = new PlayerJoined();
	pj->playerName = pc->name;
	pj->connectionID = pc->connectionID;
	toAnnounce.push_back(pj);

	start_async_accept();
}

void CPregameServer::start_async_accept()
{
	assert(!upcomingConnection);
	assert(acceptor);

	upcomingConnection = new TSocket(acceptor->get_io_service());
	acceptor->async_accept(*upcomingConnection, std::bind(&CPregameServer::connectionAccepted, this, _1));
}

void CPregameServer::announceTxt(const std::string &txt, const std::string &playerName /*= "system"*/)
{
    logNetwork->infoStream() << playerName << " says: " << txt;
	ChatMessage cm;
	cm.playerName = playerName;
	cm.message = txt;

	boost::unique_lock<boost::recursive_mutex> queueLock(mx);
	toAnnounce.push_front(new ChatMessage(cm));
}

void CPregameServer::announcePack(const CPackForSelectionScreen &pack)
{
	for(CConnection *pc : connections)
		sendPack(pc, pack);
}

void CPregameServer::sendPack(CConnection * pc, const CPackForSelectionScreen & pack)
{
	if(!pc->sendStop)
	{
        logNetwork->infoStream() << "\tSending pack of type " << typeid(pack).name() << " to " << *pc;
		*pc << &pack;
	}

	if(dynamic_cast<const QuitMenuWithoutStarting*>(&pack))
	{
		pc->sendStop = true;
	}
	else if(dynamic_cast<const StartWithCurrentSettings*>(&pack))
	{
		pc->sendStop = true;
	}
}

void CPregameServer::processPack(CPackForSelectionScreen * pack)
{
	if(dynamic_cast<CPregamePackToHost*>(pack))
	{
		sendPack(host, *pack);
	}
	else if(SelectMap *sm = dynamic_cast<SelectMap*>(pack))
	{
		vstd::clear_pointer(curmap);
		curmap = sm->mapInfo;
		sm->free = false;
		announcePack(*pack);
	}
	else if(UpdateStartOptions *uso = dynamic_cast<UpdateStartOptions*>(pack))
	{
		vstd::clear_pointer(curStartInfo);
		curStartInfo = uso->options;
		uso->free = false;
		announcePack(*pack);
	}
	else if(dynamic_cast<const StartWithCurrentSettings*>(pack))
	{
		state = ENDING_AND_STARTING_GAME;
		announcePack(*pack);
	}
	else
		announcePack(*pack);

	delete pack;
}

void CPregameServer::initConnection(CConnection *c)
{
	*c >> c->name;
	connections.insert(c);
    logNetwork->infoStream() << "Pregame connection with player " << c->name << " established!";
}

void CPregameServer::startListeningThread(CConnection * pc)
{	
	listeningThreads++;
	pc->enterPregameConnectionMode();
	pc->handler = new boost::thread(&CPregameServer::handleConnection, this, pc);
}

CVCMIServer::CVCMIServer()
: io(new boost::asio::io_service()), acceptor(new TAcceptor(*io, tcp::endpoint(tcp::v4(), port))), firstConnection(nullptr)
{
    logNetwork->debugStream() << "CVCMIServer created!";
}
CVCMIServer::~CVCMIServer()
{
	//delete io;
	//delete acceptor;
	//delete firstConnection;
}

CGameHandler * CVCMIServer::initGhFromHostingConnection(CConnection &c)
{
	auto gh = new CGameHandler();
	StartInfo si;
	c >> si; //get start options

	if(!si.createRandomMap())
	{
		bool mapFound = CResourceHandler::get()->existsResource(ResourceID(si.mapname, EResType::MAP));

		//TODO some checking for campaigns
		if(!mapFound && si.mode == StartInfo::NEW_GAME)
		{
			c << ui8(1); //WRONG!
			return nullptr;
		}
	}

	c << ui8(0); //OK!

	gh->init(&si);
	gh->conns.insert(&c);

	return gh;
}

void CVCMIServer::newGame()
{
	CConnection &c = *firstConnection;
	ui8 clients;
	c >> clients; //how many clients should be connected 
	assert(clients == 1); //multi goes now by newPregame, TODO: custom lobbies

	CGameHandler *gh = initGhFromHostingConnection(c);

	auto onExit = vstd::makeScopeGuard([&]()
	{
		vstd::clear_pointer(gh);
	});

	gh->run(false);
}

void CVCMIServer::newPregame()
{
	auto cps = new CPregameServer(firstConnection, acceptor);
	cps->run();
	if(cps->state == CPregameServer::ENDING_WITHOUT_START)
	{
		delete cps;
		return;
	}

	if(cps->state == CPregameServer::ENDING_AND_STARTING_GAME)
	{
		CGameHandler gh;
		gh.conns = cps->connections;
		gh.init(cps->curStartInfo);

		for(CConnection *c : gh.conns)
			c->addStdVecItems(gh.gs);

		gh.run(false);
	}
}

void CVCMIServer::start()
{
#ifndef VCMI_ANDROID
	ServerReady *sr = nullptr;
	intpr::mapped_region *mr;
	try
	{
		intpr::shared_memory_object smo(intpr::open_only,"vcmi_memory",intpr::read_write);
		smo.truncate(sizeof(ServerReady));
		mr = new intpr::mapped_region(smo,intpr::read_write);
		sr = reinterpret_cast<ServerReady*>(mr->get_address());
	}
	catch(...)
	{
		intpr::shared_memory_object smo(intpr::create_only,"vcmi_memory",intpr::read_write);
		smo.truncate(sizeof(ServerReady));
		mr = new intpr::mapped_region(smo,intpr::read_write);
		sr = new(mr->get_address())ServerReady();
	}
#endif

	boost::system::error_code error;
    logNetwork->infoStream()<<"Listening for connections at port " << acceptor->local_endpoint().port();
	auto  s = new tcp::socket(acceptor->get_io_service());
	boost::thread acc(std::bind(vaccept,acceptor,s,&error));
#ifndef VCMI_ANDROID
	sr->setToTrueAndNotify();
	delete mr;
#endif

	acc.join();
	if (error)
	{
        logNetwork->warnStream()<<"Got connection but there is an error " << error;
		return;
	}
    logNetwork->infoStream()<<"We've accepted someone... ";
#ifndef VCMI_IOS
	firstConnection = new CConnection(s,NAME);
#else
    firstConnection = new CConnection(s,SERVER_NAME);
#endif
    logNetwork->infoStream()<<"Got connection!";
	while(!end2)
	{
		ui8 mode;
		*firstConnection >> mode;
		switch (mode)
		{
		case 0:
			firstConnection->close();
			exit(0);
		case 1:
			firstConnection->close();
			return;
		case 2:
			newGame();
			break;
		case 3:
			loadGame();
			break;
		case 4:
			newPregame();
			break;
		}
	}
}

void CVCMIServer::loadGame()
{
	CConnection &c = *firstConnection;
	std::string fname;
	CGameHandler gh;
	boost::system::error_code error;
	ui8 clients;

	c >> clients >> fname; //how many clients should be connected

// 	{
// 		char sig[8];
// 		CMapHeader dum;
// 		StartInfo *si;
// 
// 		CLoadFile lf(CResourceHandler::get("local")->getResourceName(ResourceID(fname, EResType::LIB_SAVEGAME)));
// 		lf >> sig >> dum >> si;
// 		logNetwork->infoStream() <<"Reading save signature";
// 
// 		lf >> *VLC;
// 		logNetwork->infoStream() <<"Reading handlers";
// 
// 		lf >> (gh.gs);
// 		c.addStdVecItems(gh.gs);
// 		logNetwork->infoStream() <<"Reading gamestate";
// 	}

	{
		CLoadFile lf(*CResourceHandler::get("local")->getResourceName(ResourceID(fname, EResType::SERVER_SAVEGAME)), minSupportedVersion);
		gh.loadCommonState(lf);
		lf >> gh;
	}

	c << ui8(0);

	CConnection* cc; //tcp::socket * ss;
	for(int i=0; i<clients; i++)
	{
		if(!i) 
		{
			cc = &c;
		}
		else
		{
			auto  s = new tcp::socket(acceptor->get_io_service());
			acceptor->accept(*s,error);
			if(error) //retry
			{
                logNetwork->warnStream()<<"Cannot establish connection - retrying...";
				i--;
				continue;
			}
#ifndef VCMI_IOS
			cc = new CConnection(s,NAME);
#else
            cc = new CConnection(s,SERVER_NAME);
#endif
		}	
		gh.conns.insert(cc);
	}

	gh.run(true);
}

static void handleCommandOptions(int argc, char *argv[])
{
#ifndef VCMI_IOS
	namespace po = boost::program_options;
	po::options_description opts("Allowed options");
	opts.add_options()
		("help,h", "display help and exit")
		("version,v", "display version information and exit")
		("port", po::value<int>()->default_value(3030), "port at which server will listen to connections from client")
		("resultsFile", po::value<std::string>()->default_value("./results.txt"), "file to which the battle result will be appended. Used only in the DUEL mode.");

	if(argc > 1)
	{
		try
		{
			po::store(po::parse_command_line(argc, argv, opts), cmdLineOptions);
		}
		catch(std::exception &e) 
		{
			std::cerr << "Failure during parsing command-line options:\n" << e.what() << std::endl;
		}
	}

	po::notify(cmdLineOptions);

	if (cmdLineOptions.count("help"))
	{
		printf("%s - A Heroes of Might and Magic 3 clone\n", GameConstants::VCMI_VERSION.c_str());
		printf("Copyright (C) 2007-2014 VCMI dev team - see AUTHORS file\n");
		printf("This is free software; see the source for copying conditions. There is NO\n");
		printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
		printf("\n");
		printf("Usage:\n");
		std::cout << opts;
		exit(0);
	}

	if (cmdLineOptions.count("version"))
	{
		printf("%s\n", GameConstants::VCMI_VERSION.c_str());
		std::cout << VCMIDirs::get().genHelpString();
		exit(0);
	}
#endif
}

#if defined(__GNUC__) && !defined (__MINGW32__) && !defined(VCMI_ANDROID)
void handleLinuxSignal(int sig)
{
	const int STACKTRACE_SIZE = 100;
	void * buffer[STACKTRACE_SIZE];
	int ptrCount = backtrace(buffer, STACKTRACE_SIZE);
	char ** strings;

	logGlobal->errorStream() << "Error: signal " << sig << ":";
	strings = backtrace_symbols(buffer, ptrCount);
	if(strings == nullptr)
	{
		logGlobal->errorStream() << "There are no symbols.";
	}
	else
	{
		for(int i = 0; i < ptrCount; ++i)
		{
			logGlobal->errorStream() << strings[i];
		}
		free(strings);
	}

	_exit(EXIT_FAILURE);
}
#endif

#ifdef VCMI_IOS
int server_main(int server_port) {

    //	if(fork() != 0)	return 0; // TEMPORARY!!! JUST FOR TESTING ON THE EMULATOR. TODO:: Find other way to run the server

#else
    int main(int argc, char** argv)
    {
#endif
	// Installs a sig sev segmentation violation handler
	// to log stacktrace
	#if defined(__GNUC__) && !defined (__MINGW32__) && !defined(VCMI_ANDROID)
	signal(SIGSEGV, handleLinuxSignal);
	#endif

	console = new CConsoleHandler;
	CBasicLogConfigurator logConfig(VCMIDirs::get().userCachePath() / "VCMI_Server_log.txt", console);
	logConfig.configureDefault();
#ifndef VCMI_IOS
	handleCommandOptions(argc, argv);
	port = cmdLineOptions["port"].as<int>();
	logNetwork->infoStream() << "Port " << port << " will be used.";

	preinitDLL(console);
	settings.init();
	logConfig.configure();

	loadDLLClasses();
#else
    port = server_port;
    logNetwork->infoStream() << "Port " << port << " will be used.";
#endif
	srand ( (ui32)time(nullptr) );
	try
	{
		io_service io_service;
		CVCMIServer server;

		try
		{
			while(!end2)
			{
				server.start();
			}
			io_service.run();
		}
		catch(boost::system::system_error &e) //for boost errors just log, not crash - probably client shut down connection
		{
            logNetwork->errorStream() << e.what();
			end2 = true;
		}
		catch(...)
		{
			handleException();
		}
	}
	catch(boost::system::system_error &e)
	{
        logNetwork->errorStream() << e.what();
		//catch any startup errors (e.g. can't access port) errors
		//and return non-zero status so client can detect error
		throw;
	}
	//delete VLC; //can't be re-enabled due to access to already freed memory in bonus system
	CResourceHandler::clear();

  return 0;
}
