-- Test SeqSource
AC.addSequencerTrack(-1, -1);
AC.removeSequencerTrack(0);

-- Link Source MIDI Output
AC.addSequencerTrackMidiOutputToInstr(0, 0);
AC.addSequencerTrackMidiOutputToMixer(0, 0);

-- Link Source Audio Output
AC.addSequencerTrackOutput(0, 0, 0, 0);
AC.addSequencerTrackOutput(0, 1, 0, 1);

-- Source Bypass
AC.setSequencerTrackBypass(0, true);
AC.setSequencerTrackBypass(0, false);

-- Init Source
AC.initAudio(0, "New Audio", 48000, 2, 2);
AC.initMIDI(0, "New MIDI");

-- Load And Save Source
AC.loadAudio(0, "test.wav");
AC.saveAudio(0, "test.wav");
AC.loadMIDI(0, "test.mid", true);
AC.saveMIDI(0, "test.mid");

-- Seq Block
AC.addSequencerBlock(0, 0, 300, 0);
AC.removeSequencerBlock(0, 0);

-- Link Seq Audio Input
AC.addSequencerTrackInputFromDevice(0, 0, 0);
AC.addSequencerTrackInputFromDevice(0, 0, 1);

-- Link Seq MIDI Input
AC.addSequencerTrackMidiInput(0);

-- Seq Recording
AC.setSequencerTrackRecording(0, 0x21);

-- Audio Save Config
AC.setAudioSaveBitsPerSample(".wav", 32);
AC.setAudioSaveMetaData(".wav", { ["bwav description"] = "test description", ["bwav origination date"] = "2023-08-26", ["bwav origination time"] = "00:42:00" });
AC.setAudioSaveQualityOptionIndex(".wav", 0);
