# How to create an autonomous with this project:

1. Make sure ```CanRecordAuto``` is true in ```AutoConstants``` in ```Constants.h``` (The only reason it would be off is for if you're just running it without wanting to record an auto)<br>
2. Press down on the left stick to begin recording while in teleop<br>
3. Do whatever you want to do<br>
4. Press down on the left stick to stop recording while in teleop (do not disable the robot before pressing the left stick again)<br>
5. It should now be saved to the roborio under ```lvuser/controllerRecordings/recording_TIME-AND-DATE.csv```<br>
6. To rename it to something more readable, open up a terminal on your computer (PowerShell on windows)<br>
7. While connected to the robot with radio run ```ssh lvuser@10.TE.AM.2``` (for us lvuser@10.32.67.2)<br>
8. Do ```cd controllerRecordings```<br>
9. Do ```ls``` to find out the name of the recording you just made<br>
10. Do ```mv YOUR-RECORDING.csv YOUR-NEW-NAME.csv``` to rename it (tip: you can press tab to auto complete)<br>
11. You can close your terminal now<br>
12. Now go into ```RobotContainer.cpp``` and add an autonomous to the autonomous chooser like this:
```m_chooser.AddOption("WHATEVER-YOU-WANT", "/home/lvuser/controllerRecordings/YOUR-RECORDING-NAME.csv");```<br>
13. Deploy to the robot, select your autonomous and run it. It's not my fault if it crashes into a wall