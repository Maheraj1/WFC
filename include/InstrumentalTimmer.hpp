#pragma once

#include <chrono>
#include <fstream>

#define COMBINE(x, y) x##y
#define PROFILE_SCOPE(name) InstrumentationTimer COMBINE(timmer, __LINE__) (name)
#define PROFILE_FUNC() PROFILE_SCOPE( __FUNCTION__ )
#define START_SESSION(name) Profile_Instrument::Get().BeginSession(name); 
#define END_SESSION Profile_Instrument::Get().EndSession(); 

// Modified version of https://gist.github.com/TheCherno/31f135eea6ee729ab5f26a6908eb3a5e

namespace WFC {
	struct ProfileResult {
		std::string Name;
		long long Start, End;
		uint32_t ThreadID;
	};

	struct InstrumentationSession {
		std::string Name;
	};

	class Profile_Instrument {
		private:
			InstrumentationSession* m_CurrentSession;
			std::ofstream m_OutputStream;
			int m_ProfileCount;
		public:
			Profile_Instrument();

			void BeginSession(const std::string& name, const std::string& filepath = "results.json");
			void EndSession();
			void WriteProfile(const ProfileResult& result);
			void WriteHeader();
			void WriteFooter();
			static Profile_Instrument& Get();
	};

	class InstrumentationTimer
	{
		public:
			InstrumentationTimer(const char* name);
			~InstrumentationTimer();
			void Stop();
		private:
			const char* m_Name;
			std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimePoint;
			bool m_Stopped;
	};
}