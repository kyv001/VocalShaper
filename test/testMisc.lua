-- Plugin
AC.searchPlugin();
AC.clearPlugin();

-- Play
AC.play();
AC.pause();
AC.stop();
AC.rewind();
AC.setPlayPosition();
AC.startRecord();
AC.stopRecord();

-- Render
AC.renderNow("./", "test", ".wav", { 0, 1, 2 });

-- Project
AC.save("D:/music/vsp4/test/testProj.vsp4");
