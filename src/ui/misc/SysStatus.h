﻿#pragma once

#include <JuceHeader.h>

class SysStatus final: private juce::DeletedAtShutdown {
public:
	SysStatus();
	~SysStatus();

	struct CPUPercTemp final {
		std::array<uint64_t, 2> cpuTemp;
	};

	double getCPUUsage(CPUPercTemp& temp);
	double getMemUsage();
	uint64_t getProcMemUsage();

private:
	void* hProcess = nullptr;/**< For Windows Only */
	void* ptrProcessorInfo = nullptr;
	int nProcessors = 0;

public:
	static SysStatus* getInstance();
	static void releaseInstance();

private:
	static SysStatus* instance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SysStatus)
};
