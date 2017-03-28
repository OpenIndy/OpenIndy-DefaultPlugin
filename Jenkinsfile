node {
    try {
        def currentWorkSpace = pwd()
		stage('Checkout')
			checkout scm
        stage('compile debug') {
            bat '''call "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat" x86_amd64
            "C:\\Qt\\5.5\\msvc2013_64\\bin\\qmake.exe" OpenIndy-DefaultPlugin.pro -r -spec win32-msvc2013 "CONFIG+=debug"
            nmake clean
            nmake'''
            bat "echo build openIndy default plugin debug finished"
        }
        stage('compile release'){
            bat '''call "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\VC\\vcvarsall.bat" x86_amd64
            "C:\\Qt\\5.5\\msvc2013_64\\bin\\qmake.exe" OpenIndy-DefaultPlugin.pro -r -spec win32-msvc2013 "CONFIG+=release"
            nmake clean
            nmake'''
            bat "echo build openIndy default plugin release finished"
        }
    } catch (e) {
        // If there was an exception thrown, the build failed
        currentBuild.result = "FAILED"
        throw e
    } finally {
        // Success or failure, always send notifications
        //notifyBuild(currentBuild.result)
    }
}