// LeavesWind.h
// The Leaves Wind
// By PaintDream (paintdream@paintdream.com)
// 2015-6-20
//

#ifndef __LEAVESWIND_H__
#define __LEAVESWIND_H__

#include "../../Core/PaintsNow.h"
#include "../../Core/Interface/Interfaces.h"
#include "../../Core/Template/TFactory.h"
#include "../../Core/Common/ZType.h"
#include "../../Utility/BridgeSunset/BridgeSunset.h"
#include "../../Utility/FlameWork/FlameWork.h"
#include "../../Utility/HeartVioliner/HeartVioliner.h"
#include "../../Utility/SnowyStream/SnowyStream.h"
#include "../../Utility/LunaWidget/LunaWidget.h"
#include "../../Utility/IceSpring/IceSpring.h"
#include "../../Utility/MythForest/MythForest.h"
#include "../../Utility/FluteLeaves/FluteLeaves.h"
#include "../../Utility/RayForce/RayForce.h"
#include "../../Utility/EchoLegend/EchoLegend.h"
#include "../../Utility/Remembery/Remembery.h"
#include "../../Utility/GalaxyWeaver/GalaxyWeaver.h"

#if defined(_WIN32) || defined(WIN32)
// Add COM Support
#include "../../Utility/RayForce/COM/ComBridge.h"
#endif

namespace PaintsNow {
	namespace NsLeavesWind {
		class LeavesWind : public IScript::Library, public IFrame::Callback {
		public:
			LeavesWind(bool nogui, Interfaces& interfaces, const TWrapper<IArchive*, IStreamBase&, size_t>& subArchiveCreator, size_t threadCount, size_t warpCount);
			virtual ~LeavesWind();
			virtual bool IsRendering() const;
			virtual TObject<IReflect>& operator () (IReflect& reflect);

			// Script interfaces
			virtual void Require(IScript::Request& request);
			void RequestListenConsole(IScript::Request& request, IScript::Request::Ref callback);
			void RequestPrint(IScript::Request& request, String& text);
			void RequestExit(IScript::Request& request);
			void RequestLoadLibrary(IScript::Request& request, String& path);
			void RequestGetFullPath(IScript::Request& request, String& path);
			void RequestWarpCursor(IScript::Request& request, ZInt2 position);
			void RequestShowCursor(IScript::Request& request, String& type);
			void RequestSetAppTitle(IScript::Request& request, String& title);
			void RequestSetScreenSize(IScript::Request& request, ZInt2& size);
			void RequestGetScreenSize(IScript::Request& request);
			void RequestForward(IScript::Request& request, IScript::Request::Ref ref);

			void Reset(bool reload);
			void EnterMainLoop();
			void EnterStdinLoop();
			void BeginConsole();
			void EndConsole();
			bool ConsoleProc(IThread::Thread* thread, size_t index);
			bool ProcessCommand(const String& command);

			virtual void OnWindowSize(const PaintsNow::ZInt2&);
			virtual void Execute(const String& file, const std::vector<String>& params);

			// Object observer
			struct Object
			{
				std::list<String> keyChain;
				std::map<String, String> attribString;
				std::map<String, double> attribDouble;
				std::map<String, int64_t> attribInteger;
				std::set<String> attribSubObject;
				typedef std::map<String, std::pair<IScript::Request::StackInfo, std::vector<IScript::Request::LocalInfo> > > AttribFunction;
				AttribFunction attribFunction;
				std::map<String, IScript::Object*> attribUserObject;

				void swap(Object& rhs)
				{
					std::swap(keyChain, rhs.keyChain);
					std::swap(attribString, rhs.attribString);
					std::swap(attribDouble, rhs.attribDouble);
					std::swap(attribInteger, rhs.attribInteger);
					std::swap(attribSubObject, rhs.attribSubObject);
					std::swap(attribFunction, rhs.attribFunction);
					std::swap(attribUserObject, rhs.attribUserObject);
				}
			};

			void ModulesControl(bool init);
		public:
			virtual void OnInitialize(void* param);
			virtual void OnRender();
			virtual void OnMouse(const IFrame::EventMouse& mouse);
			virtual void OnKeyboard(const IFrame::EventKeyboard& keyboard);
			virtual void OnConsoleOutput(const String& text);
			virtual void Print(const String& text);

		protected:
			void ClearRefs();
			Interfaces& interfaces;

		protected:
			NsBridgeSunset::BridgeSunset bridgeSunset;
			NsFlameWork::FlameWork flameWork;
			NsEchoLegend::EchoLegend echoLegend;
			NsLunaWidget::LunaWidget lunaWidget;
			NsSnowyStream::SnowyStream snowyStream;
			NsMythForest::MythForest mythForest;
			NsHeartVioliner::HeartVioliner heartVioliner;
			NsFluteLeaves::FluteLeaves fluteLeaves;
			NsRayForce::RayForce rayForce;
			NsRemembery::Remembery remembery;
			NsIceSpring::IceSpring iceSpring;
			NsGalaxyWeaver::GalaxyWeaver galaxyWeaver;


#if defined(_WIN32) || defined(WIN32)
			NsRayForce::ComBridge comBridge;
#endif

#ifdef __linux__
			std::list<void*> dynamicRefs;
#endif
			IThread::Thread* consoleThread;
			IScript::Request::Ref listenConsole;
			bool nogui;
			bool resetting;
		};
	}
}

#endif // __LEAVESWIND_H__
