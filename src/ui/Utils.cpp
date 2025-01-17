﻿#include "Utils.h"

#if JUCE_WINDOWS
#include <Windows.h>
#undef max
#undef min
#endif //JUCE_WINDOWS

namespace utils {
	const juce::File getAppExecFile() {
		return juce::File::getSpecialLocation(juce::File::SpecialLocationType::hostApplicationPath);
	}
	const juce::File getAppRootDir() {
		return getAppExecFile().getParentDirectory();
	}
	const juce::File getRegExecFile() {
#if JUCE_WINDOWS
		return getAppRootDir().getChildFile("./FileRegistrar.exe");

#elif JUCE_LINUX
		return getAppRootDir().getChildFile("./FileRegistrar");

#elif JUCE_MAC
		return getAppRootDir().getChildFile("./FileRegistrar");

#else
		return getAppRootDir().getChildFile("./FileRegistrar");

#endif
	}

	const juce::File getLayoutDir() {
		return getAppRootDir().getChildFile("./layouts/");
	}

	const juce::File getResourceDir() {
		return getAppRootDir().getChildFile("./rc/");
	}

	const juce::File getFontDir() {
		return getAppRootDir().getChildFile("./fonts/");
	}

	const juce::File getSplashConfigDir() {
		return getAppRootDir().getChildFile("./splash/");
	}

	const juce::File getThemeDir() {
		return getAppRootDir().getChildFile("./themes/");
	}

	const juce::File getTransDir() {
		return getAppRootDir().getChildFile("./translates/");
	}

	const juce::File getDataDir() {
		return getAppRootDir().getChildFile("./data/");
	}

	const juce::File getLicenseDir() {
		return getAppRootDir().getChildFile("./licenses/");
	}

	const juce::File getIconDir() {
		return getAppRootDir().getChildFile("./RemixIcon/");
	}

	const juce::File getConfigDir() {
		return getDataDir().getChildFile("./config/");
	}

	const juce::File getAudioDir() {
		return getDataDir().getChildFile("./audio/");
	}

	const juce::File getEditorDir() {
		return getDataDir().getChildFile("./editor/");
	}

	const juce::File getThemeRootDir(const juce::String& name) {
		return getThemeDir().getChildFile("./" + name + "/");
	}

	const juce::File getTransRootDir(const juce::String& name) {
		return getTransDir().getChildFile("./" + name + "/");
	}

	const juce::File getIconClassDir(const juce::String& name) {
		return getIconDir().getChildFile("./" + name + "/");
	}

	const juce::File getConfigFile(const juce::String& name,
		const juce::String& type) {
		return getConfigDir().getChildFile(name + type);
	}

	const juce::File getEditorDataFile(const juce::String& name) {
		return getEditorDir().getChildFile(name);
	}

	const juce::File getKeyMappingFile(
		const juce::String& file) {
		return getConfigDir().getChildFile(file);
	}

	const juce::File getAudioConfigFile(
		const juce::String& file) {
		return getConfigDir().getChildFile(file);
	}

	const juce::File getThemeColorFile(const juce::String& name,
		const juce::String& file) {
		return getThemeRootDir(name).getChildFile(file);
	}

	const juce::File getFontFile(const juce::String& name,
		const juce::String& type) {
		return getFontDir().getChildFile(name + type);
	}

	const juce::File getResourceFile(const juce::String& name) {
		return getResourceDir().getChildFile(name);
	}

	const juce::File getSplashConfigFile(const juce::String& name,
		const juce::String& type) {
		return getSplashConfigDir().getChildFile(name + type);
	}

	const juce::File getTransConfigFile(const juce::String& name,
		const juce::String& file) {
		return getTransRootDir(name).getChildFile(file);
	}

	const juce::File getThemeConfigFile(const juce::String& name,
		const juce::String& file) {
		return getThemeRootDir(name).getChildFile(file);
	}

	const juce::File getLayoutFile(const juce::String& name,
		const juce::String& type) {
		return getLayoutDir().getChildFile(name + type);
	}

	const juce::File getIconFile(
		const juce::String& className, const juce::String& name,
		const juce::String& type) {
		return getIconClassDir(className).getChildFile(name + type);
	}

	const juce::File getPluginBlackListFile(
		const juce::String& file) {
		return getAudioDir().getChildFile(file);
	}

	const juce::File getPluginListFile(
		const juce::String& file) {
		return getAudioDir().getChildFile(file);
	}

	const juce::File getPluginSearchPathFile(
		const juce::String& file) {
		return getAudioDir().getChildFile(file);
	}

	const juce::File getPluginDeadTempDir(
		const juce::String& path) {
		return getAudioDir().getChildFile(path);
	}

	const juce::File getPluginPresetDir() {
		return getAudioDir().getChildFile("./presets/");
	}

	const juce::URL getHelpPage(const juce::String& version,
		const juce::String& branch, const juce::String& language) {
		return juce::URL{ "https://help.daw.org.cn" }
			.getChildURL("#")
			.getChildURL(language + "/")
			.withParameter("version", version)
			.withParameter("branch", branch)
			.withParameter("language", language);
	}

	const juce::URL getUpdatePage(const juce::String& version,
		const juce::String& branch) {
		return juce::URL{ "https://update.daw.org.cn" }
			.withParameter("version", version)
			.withParameter("branch", branch);
	}

	const juce::URL getBilibiliPage() {
		return juce::URL{ "https://space.bilibili.com/2060981097" };
	}

	const juce::URL getGithubPage() {
		return juce::URL{ "https://github.com/Do-sth-sharp/VocalShaper" };
	}

	const juce::URL getWebsitePage() {
		return juce::URL{ "https://daw.org.cn" };
	}

	const juce::Rectangle<int> getScreenSize(const juce::Component* comp) {
		/** Get current screen */
		if (!comp) { return { 0, 0 }; }
		auto ptrScreen = juce::Desktop::getInstance().getDisplays()
			.getDisplayForRect(comp->getScreenBounds());
		if (!ptrScreen) { return { 0, 0 }; }

		/** Base size */
		juce::Rectangle<int> rect = ptrScreen->userArea;
		juce::Rectangle<int> rectTotal = ptrScreen->totalArea;
		int width = rect.getWidth();
		int height = rect.getHeight();
		int widthT = rectTotal.getWidth();
		int heightT = rectTotal.getHeight();

		/** Width is the longer side */
		if (width < height) {
			int temp = width;
			width = height;
			height = temp;
		}
		if (widthT < heightT) {
			int temp = widthT;
			widthT = heightT;
			heightT = temp;
		}

		/** Calculate aspect ratio */
		double pNow = 9.0 / 16.0, pTotal = 9.0 / 16.0;
		pNow = (double)height / (double)width;
		pTotal = (double)heightT / (double)widthT;

		/** Scale to the standard */
		constexpr double proportion = 816.0 / 1536.0;
		constexpr double prop4Scale = 1080.0 / 1920.0;
		const double scaleM = 1.25 / 1920;
		const double scaleN = 1.0 / ((pTotal >= prop4Scale) ? widthT : (heightT / prop4Scale));
		const double scaleMN = scaleN / scaleM;

		/** Limit aspect ratio */
		if (pNow > proportion) {
			height = width * proportion;
		}
		else if (pNow < proportion) {
			width = height / proportion;
		}

		/** Result */
		return juce::Rectangle<int>{
			(int)(width * scaleMN), (int)(height * scaleMN) };
	}

	double getScreenScale(const juce::Component* comp) {
		/** Get current screen */
		if (!comp) { return 1.0; }
		auto ptrScreen = juce::Desktop::getInstance().getDisplays()
			.getDisplayForRect(comp->getScreenBounds());
		if (!ptrScreen) { return 1.0; }

		/** Result */
		return ptrScreen->scale;
	}

	const juce::StringArray parseCommand(const juce::String& command) {
		return juce::StringArray::fromTokens(command, true);
	}

	bool regProjectFileInSystem() {
#if JUCE_WINDOWS
		juce::String regExecFilePath = getRegExecFile().getFullPathName();
		juce::String appExecFilePath = getAppExecFile().getFullPathName();

		std::string fileStr = regExecFilePath.toStdString();
		std::string paramStr = juce::String{ "\"" + appExecFilePath + "\"" + " true" }.toStdString();

		SHELLEXECUTEINFO sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = "runas";
		sei.lpFile = fileStr.c_str();
		sei.lpParameters = paramStr.c_str();
		sei.nShow = SW_HIDE;

		DWORD exitCode = -1;
		if (ShellExecuteEx(&sei)) {
			if (sei.hProcess != NULL) {
				WaitForSingleObject(sei.hProcess, INFINITE);
				GetExitCodeProcess(sei.hProcess, &exitCode);
				CloseHandle(sei.hProcess);
			}
		}

		return exitCode == 0;

#else //JUCE_WINDOWS
		return false;

#endif //JUCE_WINDOWS
	}

	bool unregProjectFileFromSystem() {
#if JUCE_WINDOWS
		juce::String regExecFilePath = getRegExecFile().getFullPathName();
		juce::String appExecFilePath = getAppExecFile().getFullPathName();

		std::string fileStr = regExecFilePath.toStdString();
		std::string paramStr = juce::String{ "\"" + appExecFilePath + "\"" + " false" }.toStdString();

		SHELLEXECUTEINFO sei = {};
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.lpVerb = "runas";
		sei.lpFile = fileStr.c_str();
		sei.lpParameters = paramStr.c_str();
		sei.nShow = SW_HIDE;

		DWORD exitCode = -1;
		if (ShellExecuteEx(&sei)) {
			if (sei.hProcess != NULL) {
				WaitForSingleObject(sei.hProcess, INFINITE);
				GetExitCodeProcess(sei.hProcess, &exitCode);
				CloseHandle(sei.hProcess);
			}
		}

		return exitCode == 0;

#else //JUCE_WINDOWS
		return false;

#endif //JUCE_WINDOWS
	}

	static const std::vector<int> preKMP(const juce::String& strLong) {
		std::vector<int> next;
		next.resize(strLong.length());

		next[0] = -1;/**< While the first char not match, i++, j++ */
		int j = 0;
		int k = -1;
		while (j < strLong.length() - 1) {
			if (k == -1 || strLong[j] == strLong[k]) {
				j++;
				k++;
				next[j] = k;
			}
			else {
				k = next[k];
			}
		}

		return next;
	}

	static int KMP(const juce::String& strShort, const juce::String& strLong, const std::vector<int>& next) {
		int i = 0;
		int j = 0;
		while (i < strLong.length() && j < strShort.length()) {
			if (j == -1 || strLong[i] == strShort[j]) {
				i++;
				j++;
			}
			else {
				j = next[j];
			}
		}
		if (j == strShort.length()) {
			return i - j;
		}
		return -1;
	}

	int matchKMP(const juce::String& line, const juce::String& word) {
		return KMP(word, line, preKMP(line));
	}

	const juce::StringArray searchKMP(const juce::StringArray& list, const juce::String& word) {
		juce::StringArray result;
		for (auto& s : list) {
			if (matchKMP(s, word) >= 0) {
				result.add(s);
			}
		}
		return result;
	}

	bool fuzzyMatch(const juce::String& line, const juce::StringArray& keyWords, bool ignoreCase) {
		juce::String lineTemp = (ignoreCase ? line.toLowerCase() : line);

		auto kmpTemp = preKMP(lineTemp);

		for (auto& key : keyWords) {
			if (KMP((ignoreCase ? key.toLowerCase() : key), lineTemp, kmpTemp) < 0) {
				return false;
			}
		}
		return true;
	}

	const juce::StringArray searchFuzzy(const juce::StringArray& list, const juce::String& word, bool ignoreCase) {
		/** Split Tokens */
		juce::String breakChars = " \r\n\t";
		juce::String quoteChars = "\"";
		juce::StringArray keyWords = juce::StringArray::fromTokens(word, breakChars, quoteChars);

		for (auto& key : keyWords) {
			if (key.startsWith(quoteChars) && key.endsWith(quoteChars)) {
				key = key.substring(1, key.length() - 1);
			}
		}

		/** Match */
		juce::StringArray result;
		for (auto& s : list) {
			if (fuzzyMatch(s, keyWords, ignoreCase)) {
				result.add(s);
			}
		}
		return result;
	}

	bool saveXml(const juce::File& file, juce::XmlElement* xml) {
		if (!xml) { return false; }

		juce::FileOutputStream ostream(file);
		if (!ostream.openedOk()) { return false; }
		ostream.setPosition(0);
		ostream.truncate();

		xml->writeTo(ostream, juce::XmlElement::TextFormat{});
		return true;
	}

	std::unique_ptr<juce::XmlElement> readXml(const juce::File& file) {
		juce::XmlDocument doc(file);
		return doc.getDocumentElement(false);
	}

	void panic() {
		/** Force SIGSEGV */
		*((char*)-1) = 'x';
	}

	float logRMS(float rms) {
#if JUCE_MSVC
		return 20.f * std::log10f(rms);
#else //JUCE_MSVC
		return 20.f * std::log10(rms);
#endif //JUCE_MSVC
	}

	float getLogLevelPercent(float logLevel, float total) {
		return (logLevel - (-total)) / total;
	}

	float getLogFromPercent(float percent, float total) {
		return (-total) + percent * total;
	}

	TextIntegerFilter::TextIntegerFilter(int minLimit, int maxLimit)
		: minLimit(minLimit), maxLimit(maxLimit) {}

	juce::String TextIntegerFilter::filterNewText(
		juce::TextEditor& editor, const juce::String& newInput) {
		auto selected = editor.getHighlightedRegion();

		if (!newInput.containsOnly("-0123456789")) { 
			return selected.isEmpty() ? ""
				: editor.getText().substring(selected.getStart(), selected.getEnd());
		}

		auto caret = editor.getCaretPosition();
		int num = selected.isEmpty() 
			? editor.getText().replaceSection(caret, 0, newInput).getIntValue()
			: editor.getText().replaceSection(selected.getStart(), selected.getLength(), newInput).getIntValue();
		if (num < this->minLimit) {
			return selected.isEmpty() ? ""
				: editor.getText().substring(selected.getStart(), selected.getEnd());
		}
		if (num > this->maxLimit) {
			return selected.isEmpty() ? ""
				: editor.getText().substring(selected.getStart(), selected.getEnd());
		}
		return newInput;
	}

	TextDoubleFilter::TextDoubleFilter(
		double minLimit, double maxLimit, int numberOfDecimalPlaces)
		: minLimit(minLimit), maxLimit(maxLimit),
		numberOfDecimalPlaces(numberOfDecimalPlaces) {}

	juce::String TextDoubleFilter::filterNewText(
		juce::TextEditor& editor, const juce::String& newInput) {
		auto selected = editor.getHighlightedRegion();

		if (!newInput.containsOnly("-0123456789.")) {
			return selected.isEmpty() ? ""
				: editor.getText().substring(selected.getStart(), selected.getEnd());
		}

		auto currentText = editor.getText();
		auto caret = editor.getCaretPosition();
		int dotPlace = currentText.indexOf(0, ".");
		if ((dotPlace > -1) && newInput.containsChar('.')) {
			if (!selected.contains(dotPlace)) {
				return selected.isEmpty() ? ""
					: editor.getText().substring(selected.getStart(), selected.getEnd());
			}
		}

		currentText = selected.isEmpty()
			? currentText.replaceSection(caret, 0, newInput)
			: currentText.replaceSection(selected.getStart(), selected.getLength(), newInput);
		dotPlace = currentText.indexOf(0, ".");
		if ((dotPlace > -1) && (this->numberOfDecimalPlaces > -1)) {
			if ((currentText.length() - dotPlace - 1) > this->numberOfDecimalPlaces) {
				return selected.isEmpty() ? ""
					: editor.getText().substring(selected.getStart(), selected.getEnd());
			}
		}

		double num = selected.isEmpty()
			? editor.getText().replaceSection(caret, 0, newInput).getDoubleValue()
			: editor.getText().replaceSection(selected.getStart(), selected.getLength(), newInput).getDoubleValue();
		if (num < this->minLimit) {
			return selected.isEmpty() ? ""
				: editor.getText().substring(selected.getStart(), selected.getEnd());
		}
		if (num > this->maxLimit) {
			return selected.isEmpty() ? ""
				: editor.getText().substring(selected.getStart(), selected.getEnd());
		}
		return newInput;
	}

	juce::String TextColorRGBFilter::filterNewText(
		juce::TextEditor& e, const juce::String& newInput) {
		auto selected = e.getHighlightedRegion();

		if (!newInput.containsOnly("0123456789")) {
			return selected.isEmpty() ? ""
				: e.getText().substring(selected.getStart(), selected.getEnd());
		}

		auto caret = e.getCaretPosition();
		int num = selected.isEmpty()
			? e.getText().replaceSection(caret, 0, newInput).getIntValue()
			: e.getText().replaceSection(selected.getStart(), selected.getLength(), newInput).getIntValue();
		if (num > 255) {
			return selected.isEmpty() ? ""
				: e.getText().substring(selected.getStart(), selected.getEnd());
		}
		return newInput;
	};

	juce::String TextColorHexFilter::filterNewText(
		juce::TextEditor& e, const juce::String& newInput) {
		auto selected = e.getHighlightedRegion();

		if (!newInput.containsOnly("0123456789abcdefABCDEF")) {
			return selected.isEmpty() ? ""
				: e.getText().substring(selected.getStart(), selected.getEnd());
		}

		auto caret = e.getCaretPosition();
		int charNum = selected.isEmpty()
			? e.getText().replaceSection(caret, 0, newInput).length()
			: e.getText().replaceSection(selected.getStart(), selected.getLength(), newInput).length();
		if (charNum > 6) {
			return selected.isEmpty() ? ""
				: e.getText().substring(selected.getStart(), selected.getEnd());
		}
		return newInput;
	};

	const juce::Array<PluginGroup> groupPlugin(const juce::Array<juce::PluginDescription>& list,
		PluginGroupType groupType, bool search, const juce::String& searchText) {
		/** Plugin Groups */
		std::map<juce::String, juce::Array<juce::PluginDescription>> groupMap;

		/** Group Plugin */
		for (auto& i : list) {
			/** Group Index */
			juce::StringArray indexs;
			switch (groupType) {
			case PluginGroupType::Format:
				indexs.add(i.pluginFormatName);
				break;
			case PluginGroupType::Manufacturer:
				indexs.add(i.manufacturerName);
				break;
			case PluginGroupType::Category: {
				indexs = juce::StringArray::fromTokens(i.category, "|", "\"\'");
				break;
			}
			}

			/** Searching By Name */
			if (search) {
				if (utils::searchFuzzy({ i.name }, searchText, true).isEmpty()
					&& utils::searchFuzzy(indexs, searchText, true).isEmpty()) {
					continue;
				}
			}

			/** Insert To Group List */
			for (auto& index : indexs) {
				auto& groupList = groupMap[index];
				groupList.add(i);
			}
		}

		/** Plugin Group List */
		juce::Array<PluginGroup> groupList;
		for (auto& i : groupMap) {
			groupList.add({ i.first, i.second });
		}

		return groupList;
	}

	juce::PopupMenu createPluginMenu(const juce::Array<PluginGroup>& list,
		const std::function<void(const juce::PluginDescription&)>& callback) {
		juce::PopupMenu menu;

		for (auto& [name, plugins] : list) {
			menu.addSectionHeader(name);
			for (auto& i : plugins) {
				auto tempName = i.name;
				if (i.hasARAExtension) {
					tempName += "(ARA)";
				}
				menu.addItem(tempName, std::bind(callback, i));
			}
		}

		return menu;
	}

	const TimeInSeconds splitTime(double seconds) {
		int msec = (uint64_t)(seconds * (uint64_t)1000) % 1000;
		int sec = (uint64_t)std::floor(seconds) % 60;
		int minute = ((uint64_t)std::floor(seconds) / 60) % 60;
		int hour = seconds / 3600;

		return { hour, minute, sec, msec };
	}

	const TimeInBeats splitBeat(uint64_t measures, double beats) {
		int mbeat = (uint64_t)(beats * (uint64_t)100) % 100;
		int beat = (uint64_t)std::floor(beats) % 100;
		int measure = (uint64_t)std::floor(measures) % 10000;

		return { measure, beat, mbeat };
	}

	const std::array<uint8_t, 8> createTimeStringBase(const TimeInSeconds& time) {
		std::array<uint8_t, 8> num = { 0 };
		auto& [hour, minute, sec, msec] = time;

		num[0] = (hour / 1) % 10;
		num[1] = (minute / 10) % 10;
		num[2] = (minute / 1) % 10;
		num[3] = (sec / 10) % 10;
		num[4] = (sec / 1) % 10;
		num[5] = (msec / 100) % 10;
		num[6] = (msec / 10) % 10;
		num[7] = (msec / 1) % 10;

		return num;
	}

	const std::array<uint8_t, 8> createBeatStringBase(const TimeInBeats& time) {
		std::array<uint8_t, 8> num = { 0 };
		auto& [measure, beat, mbeat] = time;

		num[0] = (measure / 1000) % 10;
		num[1] = (measure / 100) % 10;
		num[2] = (measure / 10) % 10;
		num[3] = (measure / 1) % 10;
		num[4] = (beat / 10) % 10;
		num[5] = (beat / 1) % 10;
		num[6] = (mbeat / 10) % 10;
		num[7] = (mbeat / 1) % 10;

		return num;
	}

	const juce::String createTimeString(const TimeInSeconds& time) {
		/** Get Num */
		auto num = utils::createTimeStringBase(time);

		/** Result */
		return juce::String{ num[0] } + ":"
			+ juce::String{ num[1] } + juce::String{ num[2] } + ":"
			+ juce::String{ num[3] } + juce::String{ num[4] } + "."
			+ juce::String{ num[5] } + juce::String{ num[6] } + juce::String{ num[7] };
	}

	const juce::String createBeatString(const TimeInBeats& time) {
		/** Get Num */
		auto num = utils::createBeatStringBase(time);

		/** Result */
		return juce::String{ num[0] } + juce::String{ num[1] } + juce::String{ num[2] } + juce::String{ num[3] } + ":"
			+ juce::String{ num[4] } + juce::String{ num[5] } + ":"
			+ juce::String{ num[6] } + juce::String{ num[7] };
	}

	float colorLuminance(const juce::Colour& color) {
		auto normalize = [](int c) {
			float normalized = c / 255.0f;
			return (normalized <= 0.03928) ? normalized / 12.92 : std::pow((normalized + 0.055) / 1.055, 2.4);
			};
		return 0.2126f * normalize(color.getRed()) + 0.7152f * normalize(color.getGreen()) + 0.0722f * normalize(color.getBlue());
	}

	float colorContrastRatio(float luminance1, float luminance2) {
		float L1 = std::max(luminance1, luminance2);
		float L2 = std::min(luminance1, luminance2);
		return (L1 + 0.05f) / (L2 + 0.05f);
	}

	bool isLightColor(const juce::Colour& color,
		const juce::Colour& textColorLight, const juce::Colour& textColorDark) {
		float backgroundLuminance = colorLuminance(color);
		float darkLuminance = colorLuminance(textColorDark);
		float lightLuminance = colorLuminance(textColorLight);

		float darkContrast = colorContrastRatio(backgroundLuminance, darkLuminance);
		float lightContrast = colorContrastRatio(backgroundLuminance, lightLuminance);

		return darkContrast < lightContrast;
	}

	juce::Colour chooseTextColor(const juce::Colour& backgroundColor,
		const juce::Colour& textColorLight, const juce::Colour& textColorDark) {
		return utils::isLightColor(backgroundColor, textColorLight, textColorDark)
			? textColorLight : textColorDark;
	}

	const IntSectionList getUnionSections(const IntSectionList& source) {
		if (source.size() <= 0) return {};

		auto compareFunc = [](const IntSection& a, const IntSection& b) {
			return a.first < b.first;
			};
		IntSectionList sortedSource = source;
		std::sort(sortedSource.begin(), sortedSource.end(), compareFunc);

		IntSectionList merged;
		merged.add(sortedSource[0]);

		for (int i = 1; i < sortedSource.size(); i++) {
			auto& last = merged.getReference(merged.size() - 1);
			if (sortedSource[i].first <= last.second) {
				last.second = std::max(last.second, sortedSource[i].second);
			}
			else {
				merged.add(sortedSource[i]);
			}
		}

		return merged;
	}

	float bezierInterpolate(float p0, float p1, float p2, float p3, float t) {
		float u = 1 - t;
		return u * u * u * p0 + 3 * u * u * t * p1 + 3 * u * t * t * p2 + t * t * t * p3;
	}

	juce::Colour bezierInterpolateColor(
		const juce::Colour& color1, const juce::Colour& control1,
		const juce::Colour& control2, const juce::Colour& color2, float t) {
		return juce::Colour::fromRGB(
			(int)bezierInterpolate(color1.getRed(), control1.getRed(), control2.getRed(), color2.getRed(), t),
			(int)bezierInterpolate(color1.getGreen(), control1.getGreen(), control2.getGreen(), color2.getGreen(), t),
			(int)bezierInterpolate(color1.getBlue(), control1.getBlue(), control2.getBlue(), color2.getBlue(), t));
	}

	const juce::Array<juce::Colour> generateBezierColorGradient(
		const juce::Colour& baseColor, const juce::Colour& targetColor, int num,
		float control1, float control2) {
		juce::Array<juce::Colour> gradient;

		auto controlColor1 = juce::Colour::fromRGB(
			baseColor.getRed() + (int)((targetColor.getRed() - baseColor.getRed()) * control1),
			baseColor.getGreen() + (int)((targetColor.getGreen() - baseColor.getGreen()) * control1),
			baseColor.getBlue() + (int)((targetColor.getBlue() - baseColor.getBlue()) * control1));

		auto controlColor2 = juce::Colour::fromRGB(
			baseColor.getRed() + static_cast<int>((targetColor.getRed() - baseColor.getRed()) * control2),
			baseColor.getGreen() + static_cast<int>((targetColor.getGreen() - baseColor.getGreen()) * control2),
			baseColor.getBlue() + static_cast<int>((targetColor.getBlue() - baseColor.getBlue()) * control2));

		for (int i = 0; i < num; i++) {
			float t = i / (float)(num - 1);
			gradient.add(bezierInterpolateColor(baseColor, controlColor1, controlColor2, targetColor, t));
		}

		return gradient;
	}
}
