﻿#include "MainThreadPool.h"

#define JOB_STOP_TIMEOUT 30000

MainThreadPool::MainThreadPool() {
	this->pool = std::make_unique<juce::ThreadPool>();
}

MainThreadPool::~MainThreadPool() {
	this->stopAll();
}

void MainThreadPool::stopAll() {
	this->pool->removeAllJobs(true, JOB_STOP_TIMEOUT, nullptr);
}

void MainThreadPool::runJob(const Job& job) {
	this->pool->addJob(job);
}

void MainThreadPool::runJob(
	juce::ThreadPoolJob* job, bool deleteJobWhenFinished) {
	this->pool->addJob(job, deleteJobWhenFinished);
}

void MainThreadPool::stopJob(juce::ThreadPoolJob* job) {
	this->pool->removeJob(job, true, JOB_STOP_TIMEOUT);
}

MainThreadPool* MainThreadPool::getInstance() {
	return MainThreadPool::instance ? MainThreadPool::instance
		: (MainThreadPool::instance = new MainThreadPool{});
}

void MainThreadPool::releaseInstance() {
	if (MainThreadPool::instance) {
		delete MainThreadPool::instance;
		MainThreadPool::instance = nullptr;
	}
}

MainThreadPool* MainThreadPool::instance = nullptr;
