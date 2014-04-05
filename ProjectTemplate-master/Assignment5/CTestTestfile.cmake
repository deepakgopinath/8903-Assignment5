# CMake generated Testfile for 
# Source directory: /Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master
# Build directory: /Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(AudioIo "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/bin/TestCL" "AudioIo" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Tests/testdata/")
ADD_TEST(Fft "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/bin/TestCL" "Fft")
ADD_TEST(RingBuff "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/bin/TestCL" "RingBuff")
ADD_TEST(Lfo "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/bin/TestCL" "Lfo")
ADD_TEST(InputBuffSrc "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/bin/TestCL" "InputBuffSrc")
ADD_TEST(MyProject "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/bin/TestCL" "MyProject")
ADD_TEST(CppCheck "/usr/local/bin/cppcheck" "--error-exitcode=1" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/MyProject/src/MyProject" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/MyProject/src/MyProjectCL" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/MyProject/src/JuceLibraryCode" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/SharedSources/AudioFileIO" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/SharedSources/TestCL" "/Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/SharedSources/Dsp" " -I /Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/MyProject/incl -I /Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/Assignment5/incl -I /Users/deepak/Desktop/GeorgiaTech/Semester2Acads/SoftWareDevAlexLerch/Assignments/Assignment5/ProjectTemplate-master/SharedSources/incl")
SUBDIRS(MyProject)
SUBDIRS(MyProjectCL)
SUBDIRS(JuceLibraryCode)
SUBDIRS(AudioFileIO)
SUBDIRS(TestCL)
SUBDIRS(Dsp)
