#include "pch.h"

#include <api/create_peerconnection_factory.h>
#include <api/task_queue/default_task_queue_factory.h>
#include <rtc_base/ssl_adapter.h>
#include <rtc_base/strings/json.h>

#include "api/audio_codecs/audio_decoder_factory.h"
#include "api/audio_codecs/audio_encoder_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"

#include "Context.h"

using namespace ::webrtc;

namespace unity
{
	namespace webrtc
	{
		std::unique_ptr<ContextManager> ContextManager::s_instance;

		ContextManager* ContextManager::GetInstance()
		{
			if (s_instance == nullptr)
			{
				s_instance = std::make_unique<ContextManager>();
			}
			return s_instance.get();
		}

		Context* ContextManager::GetContext(int uid) const
		{
			auto it = s_instance->m_contexts.find(uid);
			if (it != s_instance->m_contexts.end())
			{
				return it->second.get();
			}
			return nullptr;
		}

		Context* ContextManager::CreateContext(int uid, ContextDependencies& dependencies)
		{
			auto it = s_instance->m_contexts.find(uid);
			if (it != s_instance->m_contexts.end())
			{
				DebugLog("Using already created context with ID %d", uid);
				return nullptr;
			}
			s_instance->m_contexts[uid] = std::make_unique<Context>(dependencies);
			return s_instance->m_contexts[uid].get();
		}

		void ContextManager::SetCurContext(Context* context)
		{
			curContext = context;
		}

		bool ContextManager::Exists(Context* context)
		{
			for (auto it = s_instance->m_contexts.begin(); it != s_instance->m_contexts.end(); ++it)
			{
				if (it->second.get() == context)
					return true;
			}
			return false;
		}

		void ContextManager::DestroyContext(int uid)
		{
			auto it = s_instance->m_contexts.find(uid);
			if (it != s_instance->m_contexts.end())
			{
				s_instance->m_contexts.erase(it);
			}
		}

		ContextManager::~ContextManager()
		{
			if (m_contexts.size())
			{
				DebugWarning("%lu remaining context(s) registered", m_contexts.size());
			}
			m_contexts.clear();
		}

		Context::Context(ContextDependencies& dependencies)
			: m_workerThread(rtc::Thread::CreateWithSocketServer())
			, m_signalingThread(rtc::Thread::CreateWithSocketServer())
			, m_taskQueueFactory(CreateDefaultTaskQueueFactory())
		{
			m_workerThread->Start();
			m_signalingThread->Start();

			rtc::InitializeSSL();

			std::unique_ptr<webrtc::VideoEncoderFactory> videoEncoderFactory;
			std::unique_ptr<webrtc::VideoDecoderFactory> videoDecoderFactory;

			m_peerConnectionFactory = CreatePeerConnectionFactory(
				m_workerThread.get(),
				m_workerThread.get(),
				m_signalingThread.get(),
				nullptr,
				CreateBuiltinAudioEncoderFactory(),
				CreateBuiltinAudioDecoderFactory(),
				std::move(videoEncoderFactory),
				std::move(videoDecoderFactory),
				nullptr,
				nullptr
			);
		}

		Context::~Context()
		{
			{
				std::lock_guard<std::mutex> lock(mutex);

				m_peerConnectionFactory = nullptr;
				//m_workerThread->BlockingCall([this]() { m_audioDevice = nullptr; });
				m_mapClients.clear();

				// check count of refptr to avoid to forget disposing
				RTC_DCHECK_EQ(m_mapRefPtr.size(), 0);

				m_mapRefPtr.clear();
				//m_mapMediaStreamObserver.clear();
				//m_mapDataChannels.clear();
				//m_mapVideoRenderer.clear();

				m_workerThread->Quit();
				m_workerThread.reset();
				m_signalingThread->Quit();
				m_signalingThread.reset();
			}
		}

		rtc::scoped_refptr<UnityVideoTrackSource> Context::CreateVideoSource()
		{
			return rtc::make_ref_counted<UnityVideoTrackSource>(false, absl::nullopt, m_taskQueueFactory.get());
		}

		PeerConnectionObject* Context::CreatePeerConnection(const PeerConnectionInterface::RTCConfiguration& config)
		{
			std::unique_ptr<PeerConnectionObject> obj = std::make_unique<PeerConnectionObject>(*this);
			PeerConnectionDependencies dependencies(obj.get());
			auto result = m_peerConnectionFactory->CreatePeerConnectionOrError(config, std::move(dependencies));
			if (!result.ok())
			{
				RTC_LOG(LS_ERROR) << result.error().message();
				return nullptr;
			}
			obj->connection = result.MoveValue();
			PeerConnectionObject* ptr = obj.get();
			m_mapClients[ptr] = std::move(obj);
			return ptr;
		}

		void Context::DeletePeerConnection(PeerConnectionObject* obj)
		{
			m_mapClients.erase(obj);
		}

		DataChannelInterface* Context::CreateDataChannel(PeerConnectionObject* obj, const char* label, const DataChannelInit& options)
		{
			const RTCErrorOr<rtc::scoped_refptr<DataChannelInterface>> result =
				obj->connection->CreateDataChannelOrError(label, &options);

			if (!result.ok())
				return nullptr;

			rtc::scoped_refptr<DataChannelInterface> channel = result.value();

			AddDataChannel(channel, *obj);
			return channel.get();
		}

		void Context::AddDataChannel(rtc::scoped_refptr<DataChannelInterface> channel, PeerConnectionObject& pc)
		{
			auto dataChannelObj = std::make_unique<DataChannelObject>(channel, pc);
			m_mapDataChannels[channel.get()] = std::move(dataChannelObj);
		}

		DataChannelObject* Context::GetDataChannelObject(const DataChannelInterface* channel)
		{
			return m_mapDataChannels[channel].get();
		}
		void Context::DeleteDataChannel(DataChannelInterface* channel)
		{
			if (m_mapDataChannels.count(channel) > 0)
			{
				m_mapDataChannels.erase(channel);
			}
		}
	}
}