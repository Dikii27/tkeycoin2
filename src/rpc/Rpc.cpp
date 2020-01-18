//  Copyright (c) 2017-2020 TKEY DMCC LLC & Tkeycoin Dao. All rights reserved.
//  Website: www.tkeycoin.com
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.


// Rpc.cpp

#include <transport/http/HttpContext.hpp>
#include <transport/LpsContext.hpp>
#include <transport/Transports.hpp>
#include "Rpc.hpp"

RPC::RPC(const std::shared_ptr<Node>& node)
: LogHolder("RPC")
, _node(node)
{
	auto transport = Transports::get("rpc");
	if (!transport)
	{
		throw std::runtime_error("Transport 'rpc' not found");
	}
}

void RPC::handler(const std::shared_ptr<Context>& context)
{
	// Приводим тип контекста
	auto httpContext = std::dynamic_pointer_cast<HttpContext>(context);
	if (!httpContext)
	{
		throw std::runtime_error("Bad context-type");
	}

	auto lpsContext = std::make_shared<LpsContext>(ptr(), httpContext, true, true);

	try
	{
		// Получаем входные данные
		auto&& rawInput = lpsContext->recv();

		// Данных нет или они не являются объектом
		if (!rawInput.is<SObj>())
		{
			throw std::runtime_error("Expected JSON-serialized object");
		}

		auto& input = rawInput.as<SObj>();

//		std::string sid;
//		if (!input.hasOf<SNull>("sid"))
//		{
//			input.lookup("sid", sid);
//		}
//
//		auto& requests = input.getAs<SArr>("requests");
//
//		std::shared_ptr<model::client::Session> session;
//
//		if (!sid.empty())
//		{
//			session = std::dynamic_pointer_cast<model::client::Session>(SessionManager::sessionBySid(sid));
//
//			// Сессия не найдена - протухший или невалидный sid
//			if (!session)
//			{
//				SObj error;
//				error.emplace("error", "session");
//				error.emplace("messages", "Session expired");
//
//				lpsContext->out(std::move(error));
//
//				goto done;
//			}
//
//			session->touch(true);
//
//			// Прерываем старый полинг, если есть
//			session->protectedDo(
//				[this, &session, &lpsContext] {
//					auto oldLpContext = std::dynamic_pointer_cast<wgms::LpsContext>(session->getContext());
//					if (oldLpContext)
//					{
//						if (dynamic_cast<HttpContext*>(oldLpContext->getContext().get()))
//						{
//							session->assignContext(nullptr);
//							oldLpContext->send();
//							_log.debug("Interrupt LP");
//						}
//					}
//
//					session->assignContext(lpsContext);
//					lpsContext->assignSession(session);
//				}
//			);
//		}
//
//		// Нет запросов - старт ожидания
//		if (requests.empty())
//		{
//			if (!session)
//			{
//				throw std::runtime_error("Long polling without session");
//			}
//
//			if (input.has("_"))
//			{
//				auto& aux = input.getAs<SObj>("_");
//
//				// Подтверждение получения событий
//				uint64_t lastConfirmedEvent;
//				aux.trylookup("ce", lastConfirmedEvent);
//				if (lastConfirmedEvent)
//				{
//					session->confirmEvents(lastConfirmedEvent);
//				}
//
//				// Подтверждение получения ответов
//				uint64_t lastConfirmedResponse;
//				aux.trylookup("cr", lastConfirmedResponse);
//				if (lastConfirmedResponse)
//				{
//					session->confirmResponse(lastConfirmedResponse);
//				}
//
//				// Переотправка неподтвержденных событий
//				session->resendEvents();
//			}
//
//			httpContext->setTtl(std::chrono::seconds(30));
//
//			TaskManager::enqueue(
//				[this, wp = std::weak_ptr<wgms::LpsContext>(std::dynamic_pointer_cast<wgms::LpsContext>(lpsContext))] {
//					auto expiredLpsContext = wp.lock();
//					if (!expiredLpsContext) return;
//
//					auto sessionOfExpiredLpsContext = expiredLpsContext->getSession();
//					if (sessionOfExpiredLpsContext)
//					{
//						sessionOfExpiredLpsContext->protectedDo(
//							[&sessionOfExpiredLpsContext] {
//								sessionOfExpiredLpsContext->assignContext(nullptr);
//							}
//						);
//					}
//
//					expiredLpsContext->send();
//					_log.debug("End LP");
//				},
//				std::chrono::seconds(25),
//				"EndLP (model/client)"
//			);
//
//			_log.debug("Begin LP");
//			return;
//		}
//
//		if (session)
//		{
//			if (input.has("_"))
//			{
//				auto& aux = input.getAs<SObj>("_");
//
//				// Подтверждение получения событий
//				uint64_t lastConfirmedEvent;
//				aux.trylookup("ce", lastConfirmedEvent);
//				if (lastConfirmedEvent)
//				{
//					session->confirmEvents(lastConfirmedEvent);
//				}
//
//				// Подтверждение получения ответов
//				uint64_t lastConfirmedResponse;
//				aux.trylookup("cr", lastConfirmedResponse);
//				if (lastConfirmedResponse)
//				{
//					session->confirmResponse(lastConfirmedResponse);
//				}
//
//				// Переотправка неподтвержденных событий
//				session->resendEvents();
//			}
//		}
//
//		for (auto& request : requests)
//		{
//			auto beginExecTime = std::chrono::steady_clock::now();
//
//			if (!request.is<SObj>())
//			{
//				throw std::runtime_error("One of nested request isn't object");
//			}
//
//			auto action = makeAction(lpsContext, std::move(request.as<SObj>()));
//
//			if (session)
//			{
//				// Подтверждение получения событий
//				if (action->lastConfirmedEvent())
//				{
//					session->confirmEvents(action->lastConfirmedEvent());
//				}
//
//				// Подтверждение получения ответов
//				session->confirmResponse(action->lastConfirmedResponse());
//
//				// Переотправка неподтвержденных событий
//				session->resendEvents();
//
//				// Отправка закешированного ответа
//				if (session->resendResponse(action->requestId()))
//				{
//					goto done;
//				}
//			}
//
//			try
//			{
//				if (session) session->setRequestId(action->requestId());
//
//				try
//				{
//					action->doIt("actions/" + service->name() + "/" + _name, beginExecTime);
//				}
//				catch (const std::exception& exception)
//				{
//					if (session) session->setRequestId(0);
//
//					throw;
//				}
//
//				if (session) session->setRequestId(0);
//			}
//			catch (const std::exception& exception)
//			{
//				lpsContext->out(action->error(exception.what()));
//
//				goto done;
//			}
//		}
	}
	catch (const std::exception& exception)
	{
		SObj error;
		error.emplace("error", "exception");
		error.emplace("messages", exception.what());

		lpsContext->out(std::move(error));
	}

	done:

	lpsContext->send();
}
