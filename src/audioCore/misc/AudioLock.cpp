﻿#include "AudioLock.h"

namespace audioLock {
	class LockHelper final : private juce::DeletedAtShutdown {
	public:
		juce::ReadWriteLock audioLock;
		juce::ReadWriteLock sourceLock;
		juce::ReadWriteLock pluginLock;
		juce::ReadWriteLock positionLock;
		juce::ReadWriteLock audioControlLock;
		juce::ReadWriteLock levelMeterLock;
	};

	static LockHelper* lock = new LockHelper;

	juce::ReadWriteLock& getAudioLock() {
		return lock->audioLock;
	}

	juce::ReadWriteLock& getSourceLock() {
		return lock->sourceLock;
	}

	juce::ReadWriteLock& getPluginLock() {
		return lock->pluginLock;
	}

	juce::ReadWriteLock& getPositionLock() {
		return lock->positionLock;
	}

	juce::ReadWriteLock& getAudioControlLock() {
		return lock->audioControlLock;
	}

	juce::ReadWriteLock& getLevelMeterLock() {
		return lock->levelMeterLock;
	}
}
