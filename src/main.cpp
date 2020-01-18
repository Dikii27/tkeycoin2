#include <iostream>

#include "types/Blobs.hpp"
#include "node/Node.hpp"
#include <utils/Daemon.hpp>
#include <configs/Options.hpp>
#include <configs/Config.hpp>
#include <log/LoggerManager.hpp>
#include <thread/ThreadPool.hpp>
#include <transport/Transports.hpp>
#include <telemetry/SysInfo.hpp>
#include <thread/TaskManager.hpp>
#include <net/ConnectionManager.hpp>
#include <node/AddressManager.hpp>

int main(int argc, char *argv[])
{
	Daemon::SetProcessName("tkey");
	Daemon::StartManageSignals();

	// Processing CLI oprions
	std::shared_ptr<Options> options;
	try
	{
		options = std::make_shared<Options>(argc, argv);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Fail get options ← " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	// Processing config file
	std::shared_ptr<Config> configs;
	try
	{
		configs = std::make_shared<Config>(options);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Fail get configuration ← " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}
	auto& settings = configs->settings();

	// Init logging
	try
	{
		auto& logSettings = settings.getAs<SObj>("logs");
		LoggerManager::init(logSettings);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Fail logging initialize ← " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	Log log("Main");
	log.info("Preparing to start");

	size_t workerCount = 0;

	// Init core
	try
	{
		auto& coreSettings = settings.getAs<SObj>("core");

		if (coreSettings.has("timeZone"))
		{
			std::string timeZone;
			coreSettings.lookup("timeZone", timeZone);
			if (!timeZone.empty())
			{
				setenv("TZ", timeZone.c_str(), 1);
				tzset();
			}
		}

		if (coreSettings.has("processName"))
		{
			std::string processName;
			coreSettings.lookup("processName", processName);
			if (!processName.empty())
			{
				Daemon::SetProcessName(processName);
			}
		}

		if (coreSettings.hasOf<SStr>("workers"))
		{
			if (coreSettings.getAs<SStr>("workers") == "auto")
			{
				workerCount = std::max<size_t>(2, std::thread::hardware_concurrency());
			}
		}
		if (workerCount == 0)
		{
			workerCount = coreSettings.getAs<SInt>("workers");
			if (workerCount < 2)
			{
				throw std::runtime_error("Count of workers too few. Programm won't be work correctly");
			}
		}
	}
	catch (const std::exception& exception)
	{
		log.critical("Can't configure of core ← %s", exception.what());
		LoggerManager::finalFlush();
		exit(EXIT_FAILURE);
	}

//	// Init databases
//	if (configs.has("databases"))
//	{
//		for (const auto& setting : configs.getAs<SArr>("databases"))
//		{
//			try
//			{
//				DbManager::openPool(setting.as<SObj>());
//			}
//			catch (const std::exception& exception)
//			{
//				log.warn("Can't init one of database connection pool ← %s", exception.what());
//			}
//		}
//	}

	// Init transports
	try
	{
		auto& transportsSettings = settings.getAs<SObj>("transports");

		for (const auto& [name, setting] : transportsSettings)
		{
			try
			{
				Transports::add(name, setting.as<SObj>());
			}
			catch (const std::exception& exception)
			{
				log.warn("Can't init transport '%s' ← %s", name.c_str(), exception.what());
			}
		}
	}
	catch (const std::exception& exception)
	{
		log.critical("Can't configure of transports ← %s", exception.what());
		LoggerManager::finalFlush();
		exit(EXIT_FAILURE);
	}

	// Init Blockchain
	try
	{
		auto& blockchainSettings = settings.getAs<SObj>("blockchain");
		Blockchain::init(blockchainSettings);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Fail configure address manager ← " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	// Init AddressManager
	try
	{
		auto& addrSettings = settings.getAs<SObj>("addresses");
		AddressManager::init(addrSettings);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "Fail configure address manager ← " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	// Init node
	std::shared_ptr<Node> node;
	try
	{
		node = std::make_shared<Node>(configs->settings());
	}
	catch (const std::exception& exception)
	{
		log.error("Fail init node ← %s", exception.what());
		std::cerr << "Fail init node ← " << exception.what() << std::endl;
		return EXIT_FAILURE;
	}

	// Start statistic collection
	SysInfo::start();

	// Start connection dispatcher
	TaskManager::enqueue(
		ConnectionManager::dispatch,
		"Start ConnectionManager dispatcher"
	);

//	Daemon::SetDaemonMode();

	// Start workers
	ThreadPool::setThreadNum(workerCount);

	node->up();

	log.info("Start daemon (pid=%u)", getpid());

	Daemon::doAtShutdown([node]{
		node->down();
	});

	// Wait workers
	ThreadPool::wait();

	log.info("Stop daemon");

	LoggerManager::finalFlush();

	return EXIT_SUCCESS;
}
