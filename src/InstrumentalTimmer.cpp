#include "InstrumentalTimmer.hpp"

#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <thread>

namespace WFC {

	Profile_Instrument::Profile_Instrument()
	: m_CurrentSession(nullptr), m_ProfileCount(0)
	{ }

	void Profile_Instrument::BeginSession(const std::string& name, const std::string& filepath)
	{
		m_OutputStream.open(filepath);
		WriteHeader();
		m_CurrentSession = new InstrumentationSession{ name };
	}

	void Profile_Instrument::EndSession()
	{
		WriteFooter();
		m_OutputStream.close();
		delete m_CurrentSession;
		m_CurrentSession = nullptr;
		m_ProfileCount = 0;
	}

	void Profile_Instrument::WriteProfile(const ProfileResult& result)
	{
		if (m_ProfileCount++ > 0)
			m_OutputStream << ",";

		std::string name = result.Name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_OutputStream << "{";
		m_OutputStream << "\"cat\":\"function\",";
		m_OutputStream << "\"dur\":" << (result.End - result.Start) << ',';
		m_OutputStream << "\"name\":\"" << name << "\",";
		m_OutputStream << "\"ph\":\"X\",";
		m_OutputStream << "\"pid\":0,";
		m_OutputStream << "\"tid\":" << result.ThreadID << ",";
		m_OutputStream << "\"ts\":" << result.Start;
		m_OutputStream << "}";

		m_OutputStream.flush();
	}

	void Profile_Instrument::WriteHeader()
	{
		m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_OutputStream.flush();
	}

	void Profile_Instrument::WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	static Profile_Instrument& Get()
	{
		static Profile_Instrument instance;
		return instance;
	}

	InstrumentationTimer::InstrumentationTimer(const char* name)
	:m_Name(name), m_Stopped(false) {
		m_StartTimePoint = std::chrono::high_resolution_clock::now();
	}

	InstrumentationTimer::~InstrumentationTimer() {
		if (!m_Stopped)
			Stop();
	}

	void InstrumentationTimer::Stop() {
		auto endTimePoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>
			(m_StartTimePoint).time_since_epoch().count();
		
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>
			(endTimePoint).time_since_epoch().count();

		uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());
		Profile_Instrument::Get().WriteProfile({ m_Name, start, end, threadID });

		m_Stopped = true;
	}
}