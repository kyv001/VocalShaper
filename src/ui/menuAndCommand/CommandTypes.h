﻿#pragma once

enum class CoreCommandType : int {
	NewProject = 0x2000,
	OpenProject,
	SaveProject,

	LoadSource,
	LoadSynthSource,
	SaveSource,
	ExportSource,

	Render
};

enum class GUICommandType : int {
	CloseEditor = 0x3000
};